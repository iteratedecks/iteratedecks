#define RESERVE_LEAVES_COUNT	20000
#define RESERVE_SMALL_COUNT		100
#define MAX_TURN				50

#define PROC_CHANCE				0.5

// Module is outdated - need to implement Berserk, Protect and Supply, also fix Pierce for shield

class Node
{
typedef vector<Node> VBN;
	Node *Parent;
// while using vector, I can't use references to actual Node classes since they can
// moved when vector is reallocated, should be very careful with pointers and references
	VBN Children;
	double Weight;
	UINT PickNode;
	UINT iTurn;
	bool IsParentClone;
	Node *Clone;
private:
	void Preserve()
	{
		if (Children.empty())
			Children.reserve(2); // at least 2
		else
			if (Children.size() == 2)
				Children.reserve(10); // if it's more than 2 it could be up to 10
	}
	const double FetchResult(bool bWins) const
	{
		if (bWins && Atk.Commander.IsAlive() && (!Def.Commander.IsAlive()))
			return Weight; // normalize
		if ((!bWins) && (!Atk.Commander.IsAlive()) && Def.Commander.IsAlive())
			return Weight; // normalize
		if (Children.empty())
			return 0;
		double res = 0;
		for (UINT c=0;c<Children.size();c++)
		{
			res += Children[c].FetchResult(bWins);
		}
		return res;
	}
public:
	void PrintResults(double &iWin,double &iLoss)
	{
		iWin += FetchResult(true);
		iLoss += FetchResult(false);
		printf("W: %f L: %f",iWin,iLoss);	
	}
	void NormalizeEvenly()
	{
		if (Children.empty() || (Children.size() == 1))
			return;
		double xmul = 0.0;
		for (UINT i=0;i<Children.size();i++)
			xmul += Children[i].Weight;
		//_ASSERT(xmul >= 1.0);
		if (xmul < 1.0)
			xmul = Children.size();
		for (UINT i=0;i<Children.size();i++)
			Children[i].Weight /= xmul;
	}
	void Normalize(const double Base=1.0)
	{
		return;

		if (Base > 1.0) return;
		_ASSERT(Base > 0.0);
		printf("	%f * %f\n",Weight,Base);
		if (/*(Weight >= 1) && */(Base < 1.0))
			Weight *= Base;

		if (Children.empty()) return;

		// weight is our new base from this point ...

		double xmul = 0;
		for (UINT i=0;i<Children.size();i++)
			xmul += Children[i].GetWeight();
		if (xmul > 1)
			xmul = Weight / xmul; // weight
		else
			xmul = Weight; // just mul
		for (UINT i=0;i<Children.size();i++)
			Children[i].Normalize(xmul);
	}
	void MulWeight(const double xmul)
	{
		Weight *= xmul;
	}
public:
typedef vector<Node *> VPBN;
	ActiveDeck Atk;
	ActiveDeck Def;
	Node()
	{
		Parent = 0;
		Weight = 1;
		iTurn = 0;
		Clone = 0;
		IsParentClone = false;
		PickNode = 0;
		Children.clear();
	}
	Node(const ActiveDeck &atk, const ActiveDeck &def)
	{
		Parent = 0;
		Weight = 1;
		iTurn = 0;
		Clone = 0;
		IsParentClone = false;
		PickNode = 0;
		Atk = ActiveDeck(atk);
		Def = ActiveDeck(def);
	}
	Node(const Node &node) // just a copy constructor
	{
		Parent = node.Parent;
		Weight = node.Weight;
		PickNode = node.PickNode;
		iTurn = node.iTurn;
		IsParentClone = node.IsParentClone;
		Clone = node.Clone;
		Children.clear();
		Children.reserve(node.Children.size());
		for (UINT i=0;i<node.Children.size();i++)
			Children.push_back(node.Children[i]);
		Atk = ActiveDeck(node.Atk);
		Def = ActiveDeck(node.Def);
	}
	void ClearChildren()
	{
		Children.clear();
	}
	void SetPickNode(const UINT Id)
	{
		PickNode = Id;
	}
	void SetClone(Node *clone)
	{
		Clone = clone;
	}
	void SetTurn(const UINT Turn)
	{
		iTurn = Turn;
	}
	void GetNodePicks(UINT *CardPicks) const
	{
		vector<UINT> vids;
		vids.reserve(MAX_TURN);
		if (PickNode != 0)
			vids.push_back(PickNode);
		Node *cnode = this->Parent;
		while (cnode)
		{
			if (cnode->PickNode != 0)
				vids.push_back(cnode->PickNode);
			cnode = cnode->Parent;
		}
		_ASSERT(vids.size() < MAX_TURN);
		for (UCHAR i=0;i<vids.size();i++)
			CardPicks[i] = vids[i];
	}
	bool AddClone(const double weight)
	{
		for (UINT i=0;i<Children.size();i++)
			if (Children[i].IsParentClone)
			{
				Children[i].Weight += weight;
				return true;
			}
		return false;
	}
	void AddRootChild(const ActiveDeck &atk, const ActiveDeck &def, const double setweight)
	{
		Preserve();
		Children.push_back(Node(atk, def));
		Children.back().Parent = this;
		Children.back().iTurn = iTurn;
		Children.back().Weight = setweight;
		Children.back().IsParentClone = false;
	}
	void AddChild(const ActiveDeck &atk, const ActiveDeck &def, const bool IsClone=false, const double BaseChance = 1.0)
	{
		Preserve();
		if (IsClone && AddClone(Weight * BaseChance))
			return;
		Children.push_back(Node(atk, def));
		Children.back().Parent = this;
		Children.back().iTurn = iTurn;
		Children.back().Weight = Weight * BaseChance;//Weight;
		Children.back().IsParentClone = IsClone;
	}
	/*void AddChild(const Node &node, const bool IsClone=false)
	{
		Preserve();
		if (IsClone && AddClone(Weight * node.Weight))
			return;
		Children.push_back(node);
		Children.back().Parent = this;
		Children.back().iTurn = iTurn;
		Children.back().Weight = Weight * node.Weight;
		Children.back().IsParentClone = IsClone;
	}*/
	bool CompressVertical()
	{
		// compression will remove all nodes that have exactly one child node,
		// effectively skipping all straightforward picks
		if (Children.empty()) 
			return false; // do nothing

		bool bCompressed = false;
		while (Children.size() == 1)
		{
			if (Children[0].PickNode)
				break; // don't want to mess with them, at least for now
			Atk = Children[0].Atk;
			Def = Children[0].Def;
			Weight = Children[0].Weight;
			iTurn = Children[0].iTurn;
			IsParentClone = false; // reset flag
			if (!Children[0].Children.empty())
			{
				Children.reserve(Children[0].Children.size() + 1);
				for (UINT i=0;i<Children[0].Children.size();i++)
					Children.push_back(Children[0].Children[i]);
				Children.erase(Children.begin()); // delete first element
				bCompressed = true;
			}
			else
				return bCompressed;
		}
		for (UINT i=0;i<Children.size();i++)
		{
			bCompressed |= Children[i].CompressVertical();
		}
		return bCompressed;
	}
	void Print(UINT iOffset=0)
	{
		for (UINT i=0;i<iOffset;i++)
			printf("  ");
		printf("%d\n",this);
		for (UINT i=0;i<Children.size();i++)
			Children[i].Print(iOffset+1);
	}
	const UINT GetSize() const
	{
		UINT iSize = 1;
		for (UINT i=0;i<Children.size();i++)
			iSize += Children[i].GetSize();
		return iSize;
	}
	void GetLeaves(VPBN &GetInto)
	{
		//GetInto.reserve(GetLeavesCount()); // optimization?
		//Compress();
		if (Children.empty())
		{
			if (IsActive() && (!IsClone()))
				GetInto.push_back(this);
		}
		else
			for (UINT i=0;i<Children.size();i++)
				Children[i].GetLeaves(GetInto);
	}
	const UINT GetLeavesCount(bool bSkipInactive, bool bSkipClones) const
	{	
		if (Children.empty())
		{
			if ((IsActive() || (!bSkipInactive)) && ((!bSkipClones) || (!IsClone())))
				return 1;
			else
				return 0;
		}
		else
		{
			UINT iSize = 0;
			for (UINT i=0;i<Children.size();i++)
				iSize += Children[i].GetLeavesCount(bSkipInactive,bSkipClones);
			return iSize;
		}		
	}
	const double GetWeight() const
	{
		return Weight;
	}
	void IncWeight(double iWeightInc)
	{
// WTF?
//		printf("Ins %f + %f\n",Weight,iWeightInc);
		Weight += iWeightInc;
	}
	const bool IsClone() const
	{
		return (Clone != 0);
	}
	const bool IsActive() const
	{
		return (Atk.Commander.IsAlive() && Def.Commander.IsAlive());
	}
	void PickCard(bool bNormal)
	{
		ActiveDeck *atk,*def;
		if (bNormal)
		{
			atk = &Atk;
			def = &Def;
		}
		else
		{
			atk = &Def;
			def = &Atk;
		}
		set<UINT> clones;
		if (!atk->Deck.empty())
		{
			UCHAR size = atk->Deck.size();
			for (UCHAR idx = 0; idx < size; idx++)
			{
				if (clones.find(atk->Deck[idx].GetId()) != clones.end())
				{
					Children.back().Weight += Weight; // increase it's chance
					continue; // skip same card picks
				}
				else
					clones.insert(atk->Deck[idx].GetId());
				ActiveDeck tmpA(*atk);
				ActiveDeck tmpD(*def);
				VCARDS::iterator vi = tmpA.Deck.begin() + idx; // this is bad
				if (vi->GetType() == TYPE_ASSAULT)
					tmpA.Units.push_back(*vi);
				if (vi->GetType() == TYPE_STRUCTURE)
					tmpA.Structures.push_back(*vi);
				if (vi->GetType() == TYPE_ACTION)
					tmpA.Actions.push_back(*vi);
				UINT pickedid = vi->GetId();
				tmpA.Deck.erase(vi);
				// add branch
				// tmpD is same exact copy as def! can optimize here
				if (bNormal)
					AddChild(tmpA,tmpD);
				else
					AddChild(tmpD,tmpA);
				Children.back().SetPickNode(pickedid); // mark this nodes, they are special
				Children.back().SetTurn(iTurn);
			}
			for (UINT i=0;i<Children.size();i++)
				Children[i].Weight /= size;
		}
		//Compress();
	}
	void IncreaseLastChildWeight()
	{
		printf("IncreaseLastChildWeight is not tweaked!\n");
		if (!Children.empty())
			Children.back().Weight++;
	}
	bool operator==(const Node &other) const
	{
		return ((Atk == other.Atk) && (Def == other.Def)); 
	}
	bool operator<(const Node &other) const
	{
		if (Atk == other.Atk)
			return (Def < other.Def);
		else
			return (Atk < other.Atk); 
	}
};

#define VAR_FLAG_NORMAL		0
#define VAR_FLAG_NEW		1
#define VAR_FLAG_DELETED	2
class Variation
{
	UINT CardPicks[MAX_TURN];
	UINT iFlag;
	double iWeight;
public:
	ActiveDeck Atk;
	ActiveDeck Def;
private:
	Variation()
	{
		iFlag = VAR_FLAG_NORMAL;
		iWeight = 0;
		memset(CardPicks,0,sizeof(UINT)*MAX_TURN);
	}
public:
	Variation(ActiveDeck &atk, ActiveDeck &def)
	{
		iFlag = VAR_FLAG_NORMAL;
		iWeight = 1;
		memset(CardPicks,0,sizeof(UINT)*MAX_TURN);
		Atk = ActiveDeck(atk);
		Def = ActiveDeck(def);
	}
	Variation(const Variation &v)
	{
		iFlag = VAR_FLAG_NORMAL; // note that flag resets here
		iWeight = v.iWeight;
		memcpy(CardPicks,v.CardPicks,sizeof(UINT)*MAX_TURN);
		Atk = ActiveDeck(v.Atk);
		Def = ActiveDeck(v.Def);
	}
	Variation(const Node &node)
	{
		iFlag = VAR_FLAG_NORMAL;
		iWeight = node.GetWeight();
		memset(CardPicks,0,sizeof(UINT)*MAX_TURN);
		Atk = ActiveDeck(node.Atk);
		Def = ActiveDeck(node.Def);
		node.GetNodePicks(CardPicks);
	}
	void SetFlag(const UINT flag)
	{
		iFlag = flag;
	}
	const UINT GetFlag() const
	{
		return iFlag;
	}
	const bool IsActive() const
	{
		return (Atk.Commander.IsAlive() && Def.Commander.IsAlive());
	}
	void SetW(UINT Weight)
	{
		iWeight = Weight;
	}
	const double GetW() const
	{
		return iWeight;
	}
	void IncW(UINT Increase)
	{
		iWeight += Increase;
	}
	bool operator==(const Variation &other) const
	{
		return ((Atk == other.Atk) && (Def == other.Def)); 
	}
	bool operator<(const Variation &other) const
	{
		if (Atk == other.Atk)
			return (Def < other.Def);
		else
			return (Atk < other.Atk); 
	}
};

// this class should build all possible games for a pair of decks
// branch should apper on every PROC and on every target filter
// also, there should be some special branch for hand (3 cards)
// and pick of card from that hand, it's a lot of branches, but
// this will remove any inaccuracy that is in random executions now,
// should have started from this, this will give the exact win ratio
// and is a framework for AI
typedef	set<Variation>	SVAR;
class Branching
{
	Node Root;
	SVAR Vars;
	UINT iTurn;
	bool bSurge; // mode
	RESULTS r;

	double dWWin;
	double dWLoss;
protected:
	Node::VPBN Leaves;
private:
	const UINT GetSize() const { /*return Root.GetSize();*/ return Vars.size(); }
	bool ApplyEffects(PlayedCard &csrc,Node &node,int cardindex,bool bNormal,bool IsMimiced=false,bool IsFusioned=false)
	{
		UCHAR aid,faction;
		Node::VPBN Leaves;
		Leaves.reserve(RESERVE_LEAVES_COUNT);

		UINT nodesize = node.GetSize();
		UCHAR effect;
		UCHAR FusionMultiplier = 1;
		if (IsFusioned)
			FusionMultiplier = 2;

		Leaves.clear();
		// enfeeble
		aid = ACTIVATION_ENFEEBLE;
		effect = csrc.GetAbility(aid) * FusionMultiplier;
		if (effect > 0)
		{
			if (IsMimiced)
				faction = FACTION_NONE;
			else
				faction = csrc.GetTargetFaction(aid);
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);

			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *atk,*def;
				if (bNormal)
				{
					atk = &Leaves[idx]->Atk;
					def = &Leaves[idx]->Def;
				}
				else
				{
					atk = &Leaves[idx]->Def;
					def = &Leaves[idx]->Atk;
				}
				if (csrc.GetTargetCount(aid) != TARGETSCOUNT_ALL)
				{
					// single target
					for (UCHAR i=0;i<def->Units.size();i++)
					{
						if (((def->Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) && def->Units[i].IsAlive())
						{						
							if (def->Units[i].GetAbility(DEFENSIVE_EVADE))
							{
								// evaded - variation
								if (bNormal)
									Leaves[idx]->AddChild(*atk,*def,true);
								else
									Leaves[idx]->AddChild(*def,*atk,true);
							}
							ActiveDeck tmpA(*atk);
							ActiveDeck tmpD(*def);
							tmpD.Units[i].SetEffect(aid,tmpD.Units[i].GetEffect(aid) + effect);
							// normal variation
							if (bNormal)
								Leaves[idx]->AddChild(tmpA,tmpD);
							else
								Leaves[idx]->AddChild(tmpD,tmpA);							
							if ((cardindex >= 0) && (tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT)))  
							{
								// payback variation
								tmpA.Units[cardindex].SetEffect(aid,tmpA.Units[cardindex].GetEffect(aid) + effect);
								if (bNormal)
									Leaves[idx]->AddChild(tmpA,tmpD);
								else
									Leaves[idx]->AddChild(tmpD,tmpA);							
							}
						}
					}
				}
				else
				{
					// multi-target
					// ok, so we can safely assume there won't be cards with both evade and payback skills,
					// since it will screw up a lot in this sim ;)
					// so either evade or payback, or nothing
					// this assumption makes multitarget skills branching a bit easier, can use
					// binary operators for them
					UCHAR bcnt = 1;  // can't be greater than 2^CARD_COUNT
					bool bCntIsSet = false;
					for (UCHAR b=0;b<bcnt;b++) 
					{
						ActiveDeck tmpA(*atk); // local copies
						ActiveDeck tmpD(*def);
						// iterate through every possible variation
						for (UCHAR i=0;i<tmpD.Units.size();i++)
						{
							UCHAR isplit = 0;
							if (((tmpD.Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) && tmpD.Units[i].IsAlive())
							{
								// can't have both evade and payback
								_ASSERT(!((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) && 
									(tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK))));
								// determine whether skill procs in this iteration or not
								bool proc = ((b >> isplit) & 0x1);
								if (proc)
								{
									// it was evade that proc'd
									if ((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) && proc)
									{
										// don't change
									}
									else
										tmpD.Units[i].SetEffect(aid,tmpD.Units[i].GetEffect(aid) + effect);
									// it was payback
									if ((cardindex >= 0) && ((tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT))))
									{
										// apply payback
										tmpA.Units[cardindex].SetEffect(aid,tmpA.Units[cardindex].GetEffect(aid) + effect);
									}
								}
								else
								{
									// nothing proc'd - apply effect
									tmpD.Units[i].SetEffect(aid,tmpD.Units[i].GetEffect(aid) + effect);
								}
								if ((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) || 
									((cardindex >= 0) && tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT))) 
								{
									if (!bCntIsSet)
										bcnt *= 2;
									isplit++;
								}								
							}
						}
						if (bNormal)
							Leaves[idx]->AddChild(tmpA,tmpD);
						else
							Leaves[idx]->AddChild(tmpD,tmpA);
						if (!bCntIsSet)
							bCntIsSet = true;
					}
				}
			}
		}
		// heal
		aid = ACTIVATION_HEAL;
		effect = csrc.GetAbility(aid) * FusionMultiplier;
		if (effect > 0)
		{
			if (IsMimiced)
				faction = FACTION_NONE;
			else
				faction = csrc.GetTargetFaction(aid);
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);
			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *atk,*def;
				if (bNormal)
				{
					atk = &Leaves[idx]->Atk;
					def = &Leaves[idx]->Def;
				}
				else
				{
					atk = &Leaves[idx]->Def;
					def = &Leaves[idx]->Atk;
				}
				if (csrc.GetTargetCount(aid) != TARGETSCOUNT_ALL)
				{
					// single target
					for (UCHAR i=0;i<atk->Units.size();i++)
					{
						if (((atk->Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) && (atk->Units[i].IsAlive()) &&
							 (atk->Units[i].GetHealth() != atk->Units[i].GetMaxHealth()))
						{
							ActiveDeck tmpA(*atk); // local copies
							ActiveDeck tmpD(*def);
							tmpA.Units[i].Heal(effect);
							if (bNormal)
								Leaves[idx]->AddChild(tmpA,tmpD);
							else
								Leaves[idx]->AddChild(tmpD,tmpA);
						}
					}
				}
				else
				{
					// multi target
					ActiveDeck tmpA(*atk); // local copies
					ActiveDeck tmpD(*def);
					for (UCHAR i=0;i<tmpA.Units.size();i++)
					{
						if (((tmpA.Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) && (tmpA.Units[i].IsAlive()) &&
							 (tmpA.Units[i].GetHealth() != tmpA.Units[i].GetMaxHealth()))
						{							
							tmpA.Units[i].Heal(effect);
						}
					}
					if (bNormal)
						Leaves[idx]->AddChild(tmpA,tmpD);
					else
						Leaves[idx]->AddChild(tmpD,tmpA);
				}
			}
		}
		// infuse is processed on the upper level
		// ******
		// jam
		aid = ACTIVATION_JAM;
		effect = csrc.GetAbility(aid);
		if (effect > 0)
		{
			if (IsMimiced)
				faction = FACTION_NONE;
			else
				faction = csrc.GetTargetFaction(aid);
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);
			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *atk,*def;
				if (bNormal)
				{
					atk = &Leaves[idx]->Atk;
					def = &Leaves[idx]->Def;
				}
				else
				{
					atk = &Leaves[idx]->Def;
					def = &Leaves[idx]->Atk;
				}
				double basechance = PROC_CHANCE;
				if (csrc.GetTargetCount(aid) != TARGETSCOUNT_ALL)
				{
					bool bHit = false;
					// single target
					for (UCHAR i=0;i<def->Units.size();i++)
					{
						if (((def->Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) && (def->Units[i].IsAlive()) &&
							(!def->Units[i].GetEffect(aid)) && (!def->Units[i].GetWait()))
						{	
							bHit = true;
							if (def->Units[i].GetAbility(DEFENSIVE_EVADE))
							{
								basechance *= PROC_CHANCE;
								// evaded - variation
								if (bNormal)
									Leaves[idx]->AddChild(*atk,*def,true,basechance);
								else
									Leaves[idx]->AddChild(*def,*atk,true,basechance);
							}
							ActiveDeck tmpA(*atk);
							ActiveDeck tmpD(*def);
							tmpD.Units[i].SetEffect(aid,effect);					
							if ((cardindex >= 0) && (tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT)))  
							{
								basechance *= PROC_CHANCE;
								// normal variation
								if (bNormal)
									Leaves[idx]->AddChild(tmpA,tmpD,false,basechance);
								else
									Leaves[idx]->AddChild(tmpD,tmpA,false,basechance);
								// payback variation
								tmpA.Units[cardindex].SetEffect(aid,effect);
								if (bNormal)
									Leaves[idx]->AddChild(tmpA,tmpD,false,basechance);
								else
									Leaves[idx]->AddChild(tmpD,tmpA,false,basechance);							
							}
							else
							{
								// normal variation
								if (bNormal)
									Leaves[idx]->AddChild(tmpA,tmpD,false,basechance);
								else
									Leaves[idx]->AddChild(tmpD,tmpA,false,basechance);
							}									
						}
					}
					// normalize
					if (bHit)
						Leaves[idx]->NormalizeEvenly();
				}
				else
				{
					// multi-target
					// ok, so we can safely assume there won't be cards with both evade and payback skills,
					// since it will screw up a lot in this sim ;)
					// so either evade or payback, or nothing
					// this assumption makes multitarget skills branching a bit easier, can use
					// binary operators for them
					UCHAR bcnt = 1;  // can't be greater than 2^CARD_COUNT
					bool bCntIsSet = false;
					for (UCHAR b=0;b<bcnt;b++) 
					{
						ActiveDeck tmpA(*atk); // local copies
						ActiveDeck tmpD(*def);
						// iterate through every possible variation
						for (UCHAR i=0;i<tmpD.Units.size();i++)
						{
							UCHAR isplit = 0;
							if (((tmpD.Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) && (tmpD.Units[i].IsAlive()) &&
								(!tmpD.Units[i].GetEffect(aid)) && (!tmpD.Units[i].GetWait()))
							{
								// can't have both evade and payback
								_ASSERT(!((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) && 
									(tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK))));
								// determine whether skill procs in this iteration or not
								bool proc = ((b >> isplit) & 0x1);
								if (proc)
								{
									// it was evade that proc'd
									if ((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) && proc)
									{
										// don't change
									}
									else
										tmpD.Units[i].SetEffect(aid,effect);
									// it was payback
									if ((cardindex >= 0) && ((tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT))))
									{
										// apply payback
										tmpA.Units[cardindex].SetEffect(aid,effect);
									}
								}
								else
								{
									// nothing proc'd - apply effect
									tmpD.Units[i].SetEffect(aid,tmpD.Units[i].GetEffect(aid) + effect);
								}
								if ((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) || 
									((cardindex >= 0) && tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT))) 
								{
									if (!bCntIsSet)
										bcnt *= 2;
									isplit++;
								}								
							}
						}
						if (bNormal)
							Leaves[idx]->AddChild(tmpA,tmpD);
						else
							Leaves[idx]->AddChild(tmpD,tmpA);
						if (!bCntIsSet)
							bCntIsSet = true;
					}
				}
				// variation where JAM didn't proc
				if (bNormal)
					Leaves[idx]->AddChild(*atk,*def,true,PROC_CHANCE);
				else
					Leaves[idx]->AddChild(*def,*atk,true,PROC_CHANCE);
			}
		}
		// mimic - could be tricky
		aid = ACTIVATION_MIMIC;
		effect = csrc.GetAbility(aid);
		if ((effect > 0) && (!IsMimiced))
		{
			if (IsMimiced) // mimic can't be mimiced ;) it's just sad copypaste, previous line prevents this being mimiced
				faction = FACTION_NONE;
			else
				faction = csrc.GetTargetFaction(aid);
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);
			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *atk,*def;
				if (bNormal)
				{
					atk = &Leaves[idx]->Atk;
					def = &Leaves[idx]->Def;
				}
				else
				{
					atk = &Leaves[idx]->Def;
					def = &Leaves[idx]->Atk;
				}
				// mimic is always single target
				for (UCHAR i=0;i<def->Units.size();i++)
				{
					if (((def->Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) && def->Units[i].IsAlive())
					{			
						// this will split selected node and add more leaves	
						// NEED A CHILD EVEN IF NOTHING WAS MIMICED
						// normal variation
						bool bLocalApply = ApplyEffects(def->Units[i],*Leaves[idx],i,bNormal,true);
						if ((!bLocalApply) || (def->Units[i].GetAbility(DEFENSIVE_EVADE)))
						{
							// evaded - variation
							if (bNormal)
								Leaves[idx]->AddChild(*atk,*def,true);
							else
								Leaves[idx]->AddChild(*def,*atk,true);
							/* WTF?
							if ((!bLocalApply) && (def->Units[i].GetAbility(DEFENSIVE_EVADE)))
								Leaves[idx]->IncreaseLastChildWeight(); // increase chance of nothing changing
								*/
						}
					}
				}
			}
		}
		// rally
		// does rally prefer units with 0 attack? ???? or its completely random
		// afaik it's completely random
		aid = ACTIVATION_RALLY;
		effect = csrc.GetAbility(aid) * FusionMultiplier;
		if (effect > 0)
		{
			if (IsMimiced)
				faction = FACTION_NONE;
			else
				faction = csrc.GetTargetFaction(aid);
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);
			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *atk,*def;
				if (bNormal)
				{
					atk = &Leaves[idx]->Atk;
					def = &Leaves[idx]->Def;
				}
				else
				{
					atk = &Leaves[idx]->Def;
					def = &Leaves[idx]->Atk;
				}
				if (csrc.GetTargetCount(aid) != TARGETSCOUNT_ALL)
				{
					// single target
					for (UCHAR i=0;i<atk->Units.size();i++)
					{
						if (((atk->Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) && atk->Units[i].IsAlive() &&
							((!atk->Units[i].GetWait()) && (!atk->Units[i].GetPlayed())))
						{
							ActiveDeck tmpA(*atk); // local copies
							ActiveDeck tmpD(*def);
							tmpA.Units[i].Rally(effect);
							if (bNormal)
								Leaves[idx]->AddChild(tmpA,tmpD);
							else
								Leaves[idx]->AddChild(tmpD,tmpA);
						}
					}
				}
				else
				{
					// multi target
					ActiveDeck tmpA(*atk); // local copies
					ActiveDeck tmpD(*def);
					for (UCHAR i=0;i<tmpA.Units.size();i++)
					{
						if (((tmpA.Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) && tmpA.Units[i].IsAlive() &&
							 ((!tmpA.Units[i].GetWait()) && (!tmpA.Units[i].GetPlayed())))
						{							
							tmpA.Units[i].Rally(effect);
						}
					}
					if (bNormal)
						Leaves[idx]->AddChild(tmpA,tmpD);
					else
						Leaves[idx]->AddChild(tmpD,tmpA);
				}
			}
		}
		// recharge -  only action cards
		if (csrc.GetAbility(ACTIVATION_RECHARGE) > 0)
		{
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);
			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *atk,*def;
				if (bNormal)
				{
					atk = &Leaves[idx]->Atk;
					def = &Leaves[idx]->Def;
				}
				else
				{
					atk = &Leaves[idx]->Def;
					def = &Leaves[idx]->Atk;
				}
				ActiveDeck tmpA(*atk); // local copies
				ActiveDeck tmpD(*def);
				// normal variation
				if (bNormal)
					Leaves[idx]->AddChild(tmpA,tmpD);
				else
					Leaves[idx]->AddChild(tmpD,tmpA);
				// proc'd variation
				tmpA.Deck.push_back(csrc);
				if (bNormal)
					Leaves[idx]->AddChild(tmpA,tmpD);
				else
					Leaves[idx]->AddChild(tmpD,tmpA);					
			}
		}
		// repair
		aid = ACTIVATION_REPAIR;
		effect = csrc.GetAbility(aid) * FusionMultiplier;
		if (effect > 0)
		{
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);
			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *atk,*def;
				if (bNormal)
				{
					atk = &Leaves[idx]->Atk;
					def = &Leaves[idx]->Def;
				}
				else
				{
					atk = &Leaves[idx]->Def;
					def = &Leaves[idx]->Atk;
				}
				if (csrc.GetTargetCount(aid) != TARGETSCOUNT_ALL)
				{
					// single target
					for (UCHAR i=0;i<atk->Structures.size();i++)
					{
						if (((atk->Structures[i].GetFaction() == faction) || (faction == FACTION_NONE)) && atk->Structures[i].IsAlive() &&
							 (atk->Structures[i].GetHealth() != atk->Structures[i].GetMaxHealth()))
						{
							ActiveDeck tmpA(*atk); // local copies
							ActiveDeck tmpD(*def);
							tmpA.Structures[i].Heal(effect);
							if (bNormal)
								Leaves[idx]->AddChild(tmpA,tmpD);
							else
								Leaves[idx]->AddChild(tmpD,tmpA);
						}
					}
				}
				else
				{
					// multi target
					ActiveDeck tmpA(*atk); // local copies
					ActiveDeck tmpD(*def);
					for (UCHAR i=0;i<tmpA.Structures.size();i++)
					{
						if (((tmpA.Structures[i].GetFaction() == faction) || (faction == FACTION_NONE)) && tmpA.Structures[i].IsAlive() &&
							 (tmpA.Structures[i].GetHealth() != tmpA.Structures[i].GetMaxHealth()))
						{							
							tmpA.Structures[i].Heal(effect);
						}
					}
					if (bNormal)
						Leaves[idx]->AddChild(tmpA,tmpD);
					else
						Leaves[idx]->AddChild(tmpD,tmpA);
				}
			}
		}
		// shock
		effect = csrc.GetAbility(ACTIVATION_SHOCK) * FusionMultiplier;
		if (effect > 0)
		{
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);
			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *def;
				if (bNormal)
					def = &Leaves[idx]->Def;
				else
					def = &Leaves[idx]->Atk;
				def->Commander.SufferDmg(effect); // no real choice here
			}
		}
		// siege
		aid = ACTIVATION_SIEGE;
		effect = csrc.GetAbility(aid) * FusionMultiplier;
		if (effect > 0)
		{
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);
			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *atk,*def;
				if (bNormal)
				{
					atk = &Leaves[idx]->Atk;
					def = &Leaves[idx]->Def;
				}
				else
				{
					atk = &Leaves[idx]->Def;
					def = &Leaves[idx]->Atk;
				}
				if (csrc.GetTargetCount(aid) != TARGETSCOUNT_ALL)
				{
					// single target
					for (UCHAR i=0;i<def->Structures.size();i++)
					{				
						if (!def->Structures[i].IsAlive())
							continue;
						if (def->Structures[i].GetAbility(DEFENSIVE_EVADE))
						{
							// evaded - variation
							if (bNormal)
								Leaves[idx]->AddChild(*atk,*def,true);
							else
								Leaves[idx]->AddChild(*def,*atk,true);
						}
						ActiveDeck tmpA(*atk);
						ActiveDeck tmpD(*def);
						tmpD.Structures[i].SufferDmg(effect);
						// normal variation
						if (bNormal)
							Leaves[idx]->AddChild(tmpA,tmpD);
						else
							Leaves[idx]->AddChild(tmpD,tmpA);							
					}
				}
				else
				{
					// multi-target
					// ok, so we can safely assume there won't be cards with both evade and payback skills,
					// since it will screw up a lot in this sim ;)
					// so either evade or payback, or nothing
					// this assumption makes multitarget skills branching a bit easier, can use
					// binary operators for them
					UCHAR bcnt = 1;  // can't be greater than 2^CARD_COUNT
					bool bCntIsSet = false;
					for (UCHAR b=0;b<bcnt;b++) 
					{
						ActiveDeck tmpA(*atk); // local copies
						ActiveDeck tmpD(*def);
						// iterate through every possible variation
						for (UCHAR i=0;i<tmpD.Structures.size();i++)
						{
							if (!tmpD.Structures[i].IsAlive())
								continue;
							UCHAR isplit = 0;
							// determine whether skill procs in this iteration or not
							bool proc = ((b >> isplit) & 0x1);
							if (proc)
							{
								// it was evade that proc'd
								if ((tmpD.Structures[i].GetAbility(DEFENSIVE_EVADE)) && proc)
								{
									// don't change
								}
								else
									tmpD.Structures[i].SufferDmg(effect);
							}
							else
							{
								// nothing proc'd - apply effect
								tmpD.Structures[i].SufferDmg(effect);
							}
							if (tmpD.Structures[i].GetAbility(DEFENSIVE_EVADE)) 
							{
								if (!bCntIsSet)
									bcnt *= 2;
								isplit++;
							}
						}
						if (bNormal)
							Leaves[idx]->AddChild(tmpA,tmpD);
						else
							Leaves[idx]->AddChild(tmpD,tmpA);
						if (!bCntIsSet)
							bCntIsSet = true;
					}
				}
			}			
		}
		// split
		effect = csrc.GetAbility(ACTIVATION_SPLIT);
		if ((effect > 0) && (!IsMimiced))
		{
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);
			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *atk;
				if (bNormal)
					atk = &Leaves[idx]->Atk;
				else
					atk = &Leaves[idx]->Def;
				atk->Units.push_back(PlayedCard(csrc.GetOriginalCard()));
			}
		}
		// strike - Only targets active Assault cards on play with at least 1 Attack that are neither Jammed nor Immobilized
		aid = ACTIVATION_STRIKE;
		effect = csrc.GetAbility(aid) * FusionMultiplier;
		if (effect > 0)
		{
			if (IsMimiced)
				faction = FACTION_NONE;
			else
				faction = csrc.GetTargetFaction(aid);
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);
			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *atk,*def;
				if (bNormal)
				{
					atk = &Leaves[idx]->Atk;
					def = &Leaves[idx]->Def;
				}
				else
				{
					atk = &Leaves[idx]->Def;
					def = &Leaves[idx]->Atk;
				}
				if (csrc.GetTargetCount(aid) != TARGETSCOUNT_ALL)
				{
					bool bHit = false;
					// single target
					for (UCHAR i=0;i<def->Units.size();i++)
					{
						if (((def->Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) && def->Units[i].IsAlive())
						{	
							bHit = true;
							double basechance = 1.0;
							if (def->Units[i].GetAbility(DEFENSIVE_EVADE))
							{
								basechance *= PROC_CHANCE;
								// evaded - variation
								if (bNormal)
									Leaves[idx]->AddChild(*atk,*def,true,basechance);
								else
									Leaves[idx]->AddChild(*def,*atk,true,basechance);
							}
							ActiveDeck tmpA(*atk);
							ActiveDeck tmpD(*def);
							tmpD.Units[i].StrikeDmg(effect);							
							if ((cardindex >= 0) && (tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT)))  
							{
								basechance *= PROC_CHANCE;
								// normal variation
								if (bNormal)
									Leaves[idx]->AddChild(tmpA,tmpD,false,basechance);
								else
									Leaves[idx]->AddChild(tmpD,tmpA,false,basechance);
								// payback variation
								tmpA.Units[cardindex].StrikeDmg(effect);
								if (bNormal)
									Leaves[idx]->AddChild(tmpA,tmpD,false,basechance);
								else
									Leaves[idx]->AddChild(tmpD,tmpA,false,basechance);							
							}
							else
							{
								// normal variation
								if (bNormal)
									Leaves[idx]->AddChild(tmpA,tmpD,false,basechance);
								else
									Leaves[idx]->AddChild(tmpD,tmpA,false,basechance);
							}
						}
					}
					// normalize
					if (bHit)
						Leaves[idx]->NormalizeEvenly();
				}
				else
				{
					// multi-target
					// ok, so we can safely assume there won't be cards with both evade and payback skills,
					// since it will screw up a lot in this sim ;)
					// so either evade or payback, or nothing
					// this assumption makes multitarget skills branching a bit easier, can use
					// binary operators for them
					UCHAR bcnt = 1;  // can't be greater than 2^CARD_COUNT
					bool bCntIsSet = false;
					for (UCHAR b=0;b<bcnt;b++) 
					{
						ActiveDeck tmpA(*atk); // local copies
						ActiveDeck tmpD(*def);
						// iterate through every possible variation
						for (UCHAR i=0;i<tmpD.Units.size();i++)
						{
							UCHAR isplit = 0;
							if (((tmpD.Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) && tmpD.Units[i].IsAlive())
							{
								// can't have both evade and payback
								_ASSERT(!((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) && 
									(tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK))));
								// determine whether skill procs in this iteration or not
								bool proc = ((b >> isplit) & 0x1);
								if (proc)
								{
									// it was evade that proc'd
									if ((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) && proc)
									{
										// don't change
									}
									else
										tmpD.Units[i].StrikeDmg(effect);
									// it was payback
									if ((cardindex >= 0) && ((tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT))))
									{
										// apply payback
										tmpA.Units[cardindex].StrikeDmg(effect);
									}
								}
								else
								{
									// nothing proc'd - apply effect
									tmpD.Units[i].StrikeDmg(effect);
								}
								if ((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) || 
									((cardindex >= 0) && tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT))) 
								{
									if (!bCntIsSet)
										bcnt *= 2;
									isplit++;
								}								
							}
						}
						if (bNormal)
							Leaves[idx]->AddChild(tmpA,tmpD);
						else
							Leaves[idx]->AddChild(tmpD,tmpA);
						if (!bCntIsSet)
							bCntIsSet = true;
					}
				}
			}
		}
		// weaken
		// i've played like 20 times mission 66 just farming and noticed that AI,
		// when played beholder one of the first cards kept weakening my unit aligned
		// to beholder every fucking turn, so I died of fear almost every time enemy
		// played beholder 1-2nd card, I just couldn't kill it, Is he that smart?
		// I played rush cards so he had plenty cards to pick from to weaken
		// I fucking hate random ...
		// no, that not true, AI is dumb
		aid = ACTIVATION_WEAKEN;
		effect = csrc.GetAbility(aid) * FusionMultiplier;
		if (effect > 0)
		{
			if (IsMimiced)
				faction = FACTION_NONE;
			else
				faction = csrc.GetTargetFaction(aid);
			Leaves.clear();
			node.GetLeaves(Leaves);
			DisposeClones(Leaves);
			for (UINT idx = 0; idx < Leaves.size(); idx++)
			{
				ActiveDeck *atk,*def;
				if (bNormal)
				{
					atk = &Leaves[idx]->Atk;
					def = &Leaves[idx]->Def;
				}
				else
				{
					atk = &Leaves[idx]->Def;
					def = &Leaves[idx]->Atk;
				}
				if (csrc.GetTargetCount(aid) != TARGETSCOUNT_ALL)
				{
					// single target
					for (UCHAR i=0;i<def->Units.size();i++)
					{
						if (((def->Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) &&
							 def->Units[i].IsAlive() &&
							(def->Units[i].GetWait() <= 1) &&
							(def->Units[i].GetAttack() >= 1) && // at least 1 Attack
							(!def->Units[i].GetEffect(ACTIVATION_JAM)) && // neither Jammed
							(!def->Units[i].GetEffect(DMGDEPENDANT_IMMOBILIZE)))    // nor Immobilized
						{						
							if (def->Units[i].GetAbility(DEFENSIVE_EVADE))
							{
								// evaded - variation
								if (bNormal)
									Leaves[idx]->AddChild(*atk,*def,true);
								else
									Leaves[idx]->AddChild(*def,*atk,true);
							}
							ActiveDeck tmpA(*atk);
							ActiveDeck tmpD(*def);
							tmpD.Units[i].Weaken(effect);
							// normal variation
							if (bNormal)
								Leaves[idx]->AddChild(tmpA,tmpD);
							else
								Leaves[idx]->AddChild(tmpD,tmpA);							
							if ((cardindex >= 0) && (tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT)))  
							{
								// payback variation
								tmpA.Units[cardindex].Weaken(effect);
								if (bNormal)
									Leaves[idx]->AddChild(tmpA,tmpD);
								else
									Leaves[idx]->AddChild(tmpD,tmpA);							
							}
						}
					}
				}
				else
				{
					// multi-target
					// ok, so we can safely assume there won't be cards with both evade and payback skills,
					// since it will screw up a lot in this sim ;)
					// so either evade or payback, or nothing
					// this assumption makes multitarget skills branching a bit easier, can use
					// binary operators for them
					UCHAR bcnt = 1;  // can't be greater than 2^CARD_COUNT
					bool bCntIsSet = false;
					for (UCHAR b=0;b<bcnt;b++) 
					{
						ActiveDeck tmpA(*atk); // local copies
						ActiveDeck tmpD(*def);
						// iterate through every possible variation
						for (UCHAR i=0;i<tmpD.Units.size();i++)
						{
							UCHAR isplit = 0;
							if (((tmpD.Units[i].GetFaction() == faction) || (faction == FACTION_NONE)) &&
								 def->Units[i].IsAlive() &&
								 (tmpD.Units[i].GetWait() <= 1) &&
								 (tmpD.Units[i].GetAttack() >= 1) && // at least 1 Attack
								 (!tmpD.Units[i].GetEffect(ACTIVATION_JAM)) && // neither Jammed
								 (!tmpD.Units[i].GetEffect(DMGDEPENDANT_IMMOBILIZE)))    // nor Immobilized
							{
								// can't have both evade and payback
								_ASSERT(!((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) && 
									(tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK))));
								// determine whether skill procs in this iteration or not
								bool proc = ((b >> isplit) & 0x1);
								if (proc)
								{
									// it was evade that proc'd
									if ((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) && proc)
									{
										// don't change
									}
									else
										tmpD.Units[i].Weaken(effect);
									// it was payback
									if ((cardindex >= 0) && ((tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT))))
									{
										// apply payback
										tmpA.Units[cardindex].Weaken(effect);
									}
								}
								else
								{
									// nothing proc'd - apply effect
									tmpD.Units[i].Weaken(effect);
								}
								if ((tmpD.Units[i].GetAbility(DEFENSIVE_EVADE)) || 
									((cardindex >= 0) && tmpD.Units[i].GetAbility(DEFENSIVE_PAYBACK) && (tmpA.Units[cardindex].GetType() == TYPE_ASSAULT))) 
								{
									if (!bCntIsSet)
										bcnt *= 2;
									isplit++;
								}								
							}
						}
						if (bNormal)
							Leaves[idx]->AddChild(tmpA,tmpD);
						else
							Leaves[idx]->AddChild(tmpD,tmpA);
						if (!bCntIsSet)
							bCntIsSet = true;
					}
				}
			}
		}
		node.Normalize();
		return (nodesize != node.GetSize());
	}
	void Attack(UCHAR index, Node &node, bool bNormal)
	{
		Node::VPBN Leaves;

		Leaves.clear();
		node.GetLeaves(Leaves);
		DisposeClones(Leaves);
		for (UINT idx = 0; idx < Leaves.size(); idx++)
		{
			ActiveDeck *atk,*def;
			if (bNormal)
			{
				atk = &Leaves[idx]->Atk;
				def = &Leaves[idx]->Def;
			}
			else
			{
				atk = &Leaves[idx]->Def;
				def = &Leaves[idx]->Atk;
			}		
			// factors to take into account here are
			// rolls: flurry, swipe
			// procs: flying-miss, immobilize
			// flurry gives us up to 4 attacks total
			// swipe gives us 3 targets instead of 1
			// procs are 50%
			// so it is at 4 * 3 * 2 * 2
			// also, flurry may proc or not... ah ok, previous line is fine
#define SRC	tmpA.Units[index]
			bool bExit = false;
			for (UCHAR iiflurry=0;((iiflurry<2) && (iiflurry<(atk->Units[index].GetAbility(COMBAT_FLURRY)+1)));iiflurry++)
			{
				ActiveDeck tmpA(*atk);
				ActiveDeck tmpD(*def);
// does anyone know if VALOR procs on commander? imagine combo of valor+flurry or valor+fear
// ok let's assume it does
#define VALOR_HITS_COMMANDER	true
				//printf("%s %d\n",SRC.GetName(),SRC.GetHealth());
				_ASSERT(SRC.IsDefined() && SRC.IsAlive()); // baby, don't hurt me, no more
				UCHAR iflurry = 1;
				if (iiflurry > 0)
					iflurry = SRC.GetAbility(COMBAT_FLURRY)+1;
				if ((index >= (UCHAR)tmpD.Units.size()) || (!tmpD.Units[index].IsAlive()) || (SRC.GetAbility(COMBAT_FEAR)))
				{
					// Deal DMG To Commander BUT STILL PROC50 FLURRY and PROBABLY VALOR
					UCHAR valor = (VALOR_HITS_COMMANDER && SRC.GetAbility(COMBAT_VALOR) && (tmpA.Units.size() < tmpD.Units.size())) ? SRC.GetAbility(COMBAT_VALOR) : 0;
					for (UCHAR i=0;i<iflurry;i++)
						tmpD.Commander.HitCommander(SRC.GetAttack()+valor,SRC,tmpD.Structures);
					if (bNormal)
						Leaves[idx]->AddChild(tmpA,tmpD);
					else
						Leaves[idx]->AddChild(tmpD,tmpA); // and thats it!!!
					bExit = true;
				}
			}
			if (bExit)
				continue; // all dmg already dealt

			ActiveDeck tmpA(*atk);
			ActiveDeck tmpD(*def);
			UCHAR maxflurry = (SRC.GetAbility(COMBAT_FLURRY)) ? (SRC.GetAbility(COMBAT_FLURRY)+1) : 1; // max count of attacks
			UINT cnt = maxflurry; // cnt will be count of variations
			UCHAR swipe = (SRC.GetAbility(COMBAT_SWIPE)) ? 3 : 1;
			cnt *= swipe;
			bool bCanImmobilize = (SRC.GetAbility(DMGDEPENDANT_IMMOBILIZE) > 0);
			if (bCanImmobilize)
				cnt *= 2;
			bool bCanMissAir = (!SRC.GetAbility(COMBAT_ANTIAIR)) && (!SRC.GetAbility(DEFENSIVE_FLYING));
			if (bCanMissAir)
				cnt *= 2;
			PlayedCard *targets[3];
			for (UINT ivar=0;ivar<cnt;ivar++)
			{
				// flurry will actually mess up a lot :('
				// need decent CompressHorizontal() to clean up the mess
				UCHAR iswipe = ivar % swipe; // current target (in swipe)
				UCHAR iflurry = (ivar / swipe) % maxflurry; // current attack no (in flurry)
				bool bDoImmobilize = false;
				bool bMissIfAir = false;
				if (bCanImmobilize)
				{
					bDoImmobilize = ((ivar / (maxflurry * swipe)) % 2) == 1;
					if (bCanMissAir)
						bMissIfAir = ((ivar / (maxflurry * swipe * 2)) % 2) == 1;
				}
				else
				{
					if (bCanMissAir)
						bMissIfAir = ((ivar / (maxflurry * swipe)) % 2) == 1;
				}
				// we can only store results when
				// iflurry = 0 (it means flurry didn't proc) OR
				// iflurry = maxflurry-1 (it means flurry proc'd
				// also, we have to reset tempA & tempD at the beginning of each flurry
				// when iflurry = 0
				if (iflurry == 0) // reset sequence
				{
					tmpA = ActiveDeck(*atk);
					tmpD = ActiveDeck(*def);
					if (swipe > 1)
					{
						if ((index > 0) && (tmpD.Units[index-1].IsAlive()))
							targets[0] = &tmpD.Units[index-1];
						else
							targets[0] = 0;
						targets[1] = &tmpD.Units[index];
						_ASSERT(targets[1]); // this is aligned to SRC and must be present
						if ((index+1 < (UCHAR)tmpD.Units.size()) && (tmpD.Units[index+1].IsAlive()))
							targets[2] = &tmpD.Units[index+1];
						else
							targets[2] = 0;
					}
					else
						targets[0] = &tmpD.Units[index];
				}
				//*****************************					
				// swipe
				if (((!iswipe) || (iswipe == 2)) && ((!targets[iswipe]) || (!targets[iswipe]->IsAlive()))) // empty slot to the left or right, swipe doesnt proc, if swipe == 1 then targets[0] can't be null
					continue;
				// if target dies during flurry and slot(s == 1) is aligned to SRC, we deal dmg to commander
				if ((!targets[iswipe]->IsAlive()) && ((swipe == 1) || (iswipe == 1)))
				{
					UCHAR valor = (VALOR_HITS_COMMANDER && SRC.GetAbility(COMBAT_VALOR) && (tmpA.Units.size() < tmpD.Units.size())) ? SRC.GetAbility(COMBAT_VALOR) : 0;
					tmpD.Commander.HitCommander(SRC.GetAttack()+valor,SRC,tmpD.Structures);
					// might want to add here check:
					// if (!Def.Commander.IsAlive()) return;
					continue;
				}
				//_ASSERT(targets[iswipe]->IsAlive()); // must be alive here
				// actual attack
				// must check valor before every attack
				UCHAR valor = (SRC.GetAbility(COMBAT_VALOR) && (tmpA.Units.size() < tmpD.Units.size())) ? SRC.GetAbility(COMBAT_VALOR) : 0;
				// attacking flyer
				UCHAR antiair = SRC.GetAbility(COMBAT_ANTIAIR);
				bool bMiss = (!SRC.IsAlive()); // completely ignore dmg if attacker is dead (from swipe counter or flurry counter)

				// lets cross fingers and hope I am doing it right ...
				if ((!targets[iswipe]->GetAbility(DEFENSIVE_FLYING)) && bMissIfAir)
					continue; // completely skip this variation as INVALID

				if (targets[iswipe]->GetAbility(DEFENSIVE_FLYING))
				{
					bMiss = bMissIfAir && bCanMissAir;
				}
				else antiair = 0; // has no effect if target is not flying
				if (!bMiss)
				{
					// enfeeble is taken into account before armor
					UCHAR enfeeble = targets[iswipe]->GetEffect(ACTIVATION_ENFEEBLE);
					// now armor & pierce
					UCHAR dmg = SRC.GetAttack() + valor + antiair + enfeeble;
					UCHAR armor = targets[iswipe]->GetAbility(DEFENSIVE_ARMORED);
					if (armor) 
					{
						UCHAR pierce = SRC.GetAbility(COMBAT_PIERCE);
						if (pierce >= armor)
							armor = 0;
						else
							armor -= pierce;
						// substract armor from dmg
						if (armor >= dmg)
							dmg = 0;
						else
							dmg -= armor; 
					}
					// now we actually deal dmg
					//printf("%s %d = %d => %s %d\n",SRC.GetName(),SRC.GetHealth(),dmg,targets[s]->GetName(),targets[s]->GetHealth());
					if (dmg)
						dmg = targets[iswipe]->SufferDmg(dmg);
					// and now dmg dependant effects
					if (!targets[iswipe]->IsAlive()) // target just died
					{
						// afaik it ignores walls
						if (targets[iswipe]->GetAbility(SPECIAL_BACKFIRE))
							tmpD.Commander.SufferDmg(targets[iswipe]->GetAbility(SPECIAL_BACKFIRE));
						// crush
						if (SRC.GetAbility(DMGDEPENDANT_CRUSH))
							tmpD.Commander.HitCommander(SRC.GetAbility(DMGDEPENDANT_CRUSH),SRC,tmpD.Structures,false);
					}
					// counter
					if (targets[iswipe]->GetAbility(DEFENSIVE_COUNTER))
						SRC.SufferDmg(targets[iswipe]->GetAbility(DEFENSIVE_COUNTER) + SRC.GetEffect(ACTIVATION_ENFEEBLE)); // counter dmg is enhanced by enfeeble
					// if target is dead, we dont need to process this effects
					if (targets[iswipe]->IsAlive() && (dmg > 0))
					{
						// immobilize
						if (SRC.GetAbility(DMGDEPENDANT_IMMOBILIZE) && PROC50)
							targets[iswipe]->SetEffect(DMGDEPENDANT_IMMOBILIZE,SRC.GetAbility(DMGDEPENDANT_IMMOBILIZE));
						// poison
						if (SRC.GetAbility(DMGDEPENDANT_POISON))
							if (targets[iswipe]->GetEffect(DMGDEPENDANT_POISON) < SRC.GetAbility(DMGDEPENDANT_POISON)) // overflow
								targets[iswipe]->SetEffect(DMGDEPENDANT_POISON,SRC.GetAbility(DMGDEPENDANT_POISON));
					}
					// leech
					if (SRC.IsAlive() && SRC.GetAbility(DMGDEPENDANT_LEECH))
					{
						UCHAR leech = (SRC.GetAbility(DMGDEPENDANT_LEECH) < dmg) ? SRC.GetAbility(DMGDEPENDANT_LEECH) : dmg;
						if (leech)
							SRC.Heal(leech);
					}
					// siphon
					if (SRC.GetAbility(DMGDEPENDANT_SIPHON))
					{
						UCHAR siphon = (SRC.GetAbility(DMGDEPENDANT_SIPHON) < dmg) ? SRC.GetAbility(DMGDEPENDANT_SIPHON) : dmg;
						if (siphon)
							tmpA.Commander.Heal(siphon);
					}
				}
				//*****************************					
				if (iflurry == 0)
				{
					// variation when flurry didn't proc
					if (bNormal)
						Leaves[idx]->AddChild(tmpA,tmpD);
					else
						Leaves[idx]->AddChild(tmpD,tmpA);
				}
				else
					if (iflurry == maxflurry - 1)
					{
						// variation when flurry proc'd and finished
						if (bNormal)
							Leaves[idx]->AddChild(tmpA,tmpD);
						else
							Leaves[idx]->AddChild(tmpD,tmpA); 
					}
			}
		}
#undef SRC
		node.Normalize();
	}
	struct NodePtrCmp
	{
	  bool operator()(const Node* a, const Node* b) const
	  {
		  _ASSERT(a && b);
		  //printf("%d %d : %d %d\n",a->Atk < b->Atk,b->Atk < a->Atk,a->Def < b->Def,b->Def < a->Def);
		  //_ASSERT(!((a->Atk < b->Atk) && (b->Atk < a->Atk)));
		 // return (a->Atk < b->Atk); 
		  if (a->Atk == b->Atk)
			  return (a->Def < b->Def);
		  else
			  return (a->Atk < b->Atk);
		  //return ((a->Atk < b->Atk) && (a->Def < b->Def)); 
		  //return (a->Atk.Deck.size() < b->Atk.Deck.size());
		  //return (a < b);
		  //return ((*a) < (*b));
	  }
	};
	void DisposeClones(Node::VPBN &leaves)
	{
		return; // ignore, it is done via Variation class now
		
		typedef set <Node *, NodePtrCmp>	SNODE;
		SNODE SNodes;

		// check for cloWnes
		UINT cnt = 0, basecnt = leaves.size();
		// if size = 1 we dont need this ...
		if (leaves.size() <= 1)
			return;
		for (UINT i=0;i<leaves.size();i++)
		{
			pair<SNODE::iterator,bool> ir = SNodes.insert(leaves[i]);
			if (!ir.second)
			{
				leaves[i]->SetClone((*ir.first));
				cnt++;
			}
		}
		if ((SNodes.size() > 0) && (leaves.size() != SNodes.size()))
			printf("[%d] %d : %d -> %d\n",iTurn,Root.GetSize(),leaves.size(),SNodes.size());

		// now cleanup
		for (Node::VPBN::iterator vi=leaves.begin();vi!=leaves.end();vi++)
		{
			while ((vi != leaves.end()) && (*vi)->IsClone())
				vi = leaves.erase(vi);
			if (vi != leaves.end())
				break;
		}
	}
	void PrintResults()
	{
		printf(" ");
		//Root.Normalize();
		Root.PrintResults(dWWin,dWLoss);
		/*if (iWWin > 1)
			printf(" BREAKPOINT ");*/
		printf(" ");
	}
	bool AttackDeck(bool bNormal) // I think I should just copy these objects because they will be moved in vector, cant use references to them ;(
	{
		if (!Vars.empty())
		{
			Root.ClearChildren();
			for (SVAR::iterator vi=Vars.begin();vi!=Vars.end();vi++)
				if (vi->GetW() > 0.001) // drop low chance nodes !!!!!!!!!!! WTF
			{
				Root.AddRootChild(vi->Atk,vi->Def,vi->GetW());
				printf("%f\n",vi->GetW());
				//if (vi->GetW() > 1)
				//	printf("BREAKPOINT");
				//vi->SetFlag(VAR_FLAG_DELETED);
			}
		}

		Leaves.reserve(RESERVE_LEAVES_COUNT);
		printf("A");
		// poison
		Leaves.clear();
		Root.GetLeaves(Leaves);
		DisposeClones(Leaves);
		if (Leaves.empty())
		{
			PrintResults();
			return false;
		}
		for (UINT i=0;i<Leaves.size();i++)
		{
			ActiveDeck *atk;
			if (bNormal)
				atk = &Leaves[i]->Atk;
			else
				atk = &Leaves[i]->Def;
			for (UCHAR i=0;i<atk->Units.size();i++)
				atk->Units[i].ProcessPoison();
		}
		printf("B");
		// pick
		Leaves.clear();
		Root.GetLeaves(Leaves);
		DisposeClones(Leaves);
		if (Leaves.empty())
		{
			PrintResults();
			return false;
		}
		for (UINT i=0;i<Leaves.size();i++)
		{
			Leaves[i]->PickCard(bNormal);
		}
		printf("C");
		Leaves.clear();
		Root.GetLeaves(Leaves);
		DisposeClones(Leaves);
		if (Leaves.empty())
		{
			PrintResults();
			return false;
		}
		for (UINT l=0;l<Leaves.size();l++)
		{		
			ActiveDeck *atk;
			if (bNormal)
				atk = &Leaves[l]->Atk;
			else
				atk = &Leaves[l]->Def;
			// action cards
			for (UCHAR i=0;i<atk->Actions.size();i++)
			{
				// apply actions somehow ...
				ApplyEffects(atk->Actions[i],(*Leaves[l]),-1,bNormal);
			}
		}
		printf("D");
		Leaves.clear();
		Root.GetLeaves(Leaves);
		DisposeClones(Leaves);
		if (Leaves.empty())
		{
			PrintResults();
			return false;
		}
		for (UINT l=0;l<Leaves.size();l++)
		{		
			ActiveDeck *atk;
			if (bNormal)
				atk = &Leaves[l]->Atk;
			else
				atk = &Leaves[l]->Def;
			atk->Actions.clear();
			// commander card
			// ok lets work out Infuse:
			// infuse - dont know how this works :(
			// ok so afaik it changes one random card from either of your or enemy deck into bloodthirsty
			// faction plus it changes heal and rally skills faction, if there were any, into bloodthirsty
			// i believe it can't be mimiced and paybacked(can assume since it's commander skill) and 
			// it can be evaded, according to forums
			// "his own units that have evade wont ever seem to evade.
			// (every time ive seen the collossus infuse and as far as i can see… he has no other non bt with evade.)"
			// so, I assume, evade works for us, but doesn't work for his cards
			// the bad thing about infuse is that we need faction as an attribute of card, we can't pull it out of
			// library, I need to add PlayedCard.Faction, instead of using Card.Faction
			// added
			if (atk->Commander.GetAbility(ACTIVATION_INFUSE) > 0)
			{
				// pick a card
				/*PVCARDS targets;
				targets.reserve(DEFAULT_DECK_RESERVE_SIZE);
				GetTargets(Def.Units,FACTION_BLOODTHIRSTY,targets,true);
				UCHAR defcount = targets.size();
				GetTargets(Units,FACTION_BLOODTHIRSTY,targets,true);
				if (!targets.empty())
				{
					UCHAR i = rand() % targets.size(); 
					PlayedCard *t = targets[i];
					if ((i < defcount) && (t->GetAbility(DEFENSIVE_EVADE)) && PROC50) // we check evade only on our cards, enemy cards don't seem to actually evade infuse since it's rather helpful to them then harmful
					{
						// evaded infuse
						//printf("Evaded\n");
					}
					else
						t->Infuse(FACTION_BLOODTHIRSTY);
				}*/
			}
		}
		printf("E");
		Leaves.clear();
		Root.GetLeaves(Leaves);
		DisposeClones(Leaves);
		if (Leaves.empty())
		{
			PrintResults();
			return false;
		}
		for (UINT l=0;l<Leaves.size();l++)
		{		
			ActiveDeck *atk;
			if (bNormal)
				atk = &Leaves[l]->Atk;
			else
				atk = &Leaves[l]->Def;
			// apply actions same way 
			ApplyEffects(atk->Commander,(*Leaves[l]),-1,bNormal);
		}
		printf("F");
		Leaves.clear();
		Root.GetLeaves(Leaves);
		DisposeClones(Leaves);
		if (Leaves.empty())
		{
			PrintResults();
			return false;
		}
		for (UINT l=0;l<Leaves.size();l++)
		{		
			ActiveDeck *atk;
			if (bNormal)
				atk = &Leaves[l]->Atk;
			else
				atk = &Leaves[l]->Def;
			PlayedCard Empty;
			UCHAR iFusionCount = 0;
			for (UCHAR i=0;i<atk->Structures.size();i++)
			{
				if (atk->Structures[i].GetAbility(SPECIAL_FUSION) > 0)
					iFusionCount++;
			}
			// structure cards
			for (UCHAR i=0;i<atk->Structures.size();i++)
			{
				// apply actions somehow ...
				Node::VPBN localLeaves;
				localLeaves.reserve(RESERVE_SMALL_COUNT);
				Leaves[l]->GetLeaves(localLeaves);
				DisposeClones(localLeaves);
				for (UINT z=0;z<localLeaves.size();z++)
				{
					ActiveDeck *latk;
					if (bNormal)
						latk = &localLeaves[z]->Atk;
					else
						latk = &localLeaves[z]->Def;
					if (latk->Structures[i].BeginTurn())
						ApplyEffects(latk->Structures[i],(*localLeaves[z]),-1,bNormal,false,(iFusionCount >= 3));

					Node::VPBN localILeaves;
					localILeaves.reserve(RESERVE_SMALL_COUNT);
					localLeaves[z]->GetLeaves(localILeaves);
					DisposeClones(localLeaves);
					for (UINT x=0;x<localILeaves.size();x++)
					{
						ActiveDeck *liatk;
						if (bNormal)
							liatk = &localILeaves[x]->Atk;
						else
							liatk = &localILeaves[x]->Def;
						liatk->Structures[i].EndTurn();
					}
				}
			}
		}
		time_t t,t1;
		time(&t);
		printf("G");
		Leaves.clear();
		Root.GetLeaves(Leaves);
		DisposeClones(Leaves);
		if (Leaves.empty())
		{
			PrintResults();
			return false;
		}
		for (UINT l=0;l<Leaves.size();l++)
		{		
			ActiveDeck *atk;
			if (bNormal)
				atk = &Leaves[l]->Atk;
			else
				atk = &Leaves[l]->Def;
			// assault cards
			for (UCHAR i=0;i<atk->Units.size();i++)
			{
				Node::VPBN localLeaves;
				localLeaves.reserve(RESERVE_SMALL_COUNT);
				Leaves[l]->GetLeaves(localLeaves);
				DisposeClones(localLeaves);
				for (UINT z=0;z<localLeaves.size();z++)
				{
					ActiveDeck *latk;
					if (bNormal)
						latk = &localLeaves[z]->Atk;
					else
						latk = &localLeaves[z]->Def;
					if (latk->Units[i].BeginTurn())
					{
						//if (!Units[i].GetEffect(ACTIVATION_JAM)) // jammed - checked in beginturn
						ApplyEffects(latk->Units[i],(*localLeaves[z]),i,bNormal);
						Node::VPBN localILeaves;
						localILeaves.reserve(RESERVE_SMALL_COUNT);
						localLeaves[z]->GetLeaves(localILeaves);
						DisposeClones(localILeaves);
						for (UINT x=0;x<localILeaves.size();x++)
						{
							ActiveDeck *liatk;
							if (bNormal)
								liatk = &localILeaves[x]->Atk;
							else
								liatk = &localILeaves[x]->Def;							
							if ((!liatk->Units[i].GetEffect(DMGDEPENDANT_IMMOBILIZE)) ) 
							{
								if (liatk->Units[i].IsAlive() && liatk->Units[i].GetAttack()) // can't attack with dead unit ;) also if attack = 0 then dont attack at all
									Attack(i, (*localILeaves[x]), bNormal);
							}							
						}
					}
					Node::VPBN localIILeaves;
					localIILeaves.reserve(RESERVE_SMALL_COUNT);
					localLeaves[z]->GetLeaves(localIILeaves);
					DisposeClones(localIILeaves);
					for (UINT y=0;y<localIILeaves.size();y++)
					{
						ActiveDeck *liiatk;
						if (bNormal)
							liiatk = &localIILeaves[y]->Atk;
						else
							liiatk = &localIILeaves[y]->Def;
						liiatk->Units[i].EndTurn();
					}
				}
			}
		}
		time(&t1);
		if (t1 != t)
			printf("H(%d)",t1-t);
		else
			printf("H");
		Leaves.clear();
		Root.GetLeaves(Leaves);
		DisposeClones(Leaves);
		if (Leaves.empty())
		{
			PrintResults();
			return false;
		}
		for (UINT l=0;l<Leaves.size();l++)
		{		
			ActiveDeck *atk, *def;
			if (bNormal)
			{
				atk = &Leaves[l]->Atk;
				def = &Leaves[l]->Def;
			}
			else
			{
				atk = &Leaves[l]->Def;
				def = &Leaves[l]->Atk;
			}

			// clear dead units here yours and enemy
			VCARDS::iterator vi = atk->Units.begin();
			while (vi != atk->Units.end())
				if (!vi->IsAlive())
					vi = atk->Units.erase(vi);
				else
				{
					vi->ResetPlayedFlag();
					vi++;
				}
			vi = atk->Structures.begin();
			while (vi != atk->Structures.end())
				if (!vi->IsAlive())
					vi = atk->Structures.erase(vi);
				else
					vi++;
			vi = atk->Actions.begin();
			while (vi != atk->Actions.end())
				if (!vi->IsAlive())
					vi = atk->Actions.erase(vi);
				else
					vi++;
			//
			vi = def->Units.begin();
			while (vi != def->Units.end())
				if (!vi->IsAlive())
					vi = def->Units.erase(vi);
				else
					vi++;
			vi = def->Structures.begin();
			while (vi != def->Structures.end())
				if (!vi->IsAlive())
					vi = def->Structures.erase(vi);
				else
					vi++;
			vi = def->Actions.begin();
			while (vi != def->Actions.end())
				if (!vi->IsAlive())
					vi = def->Actions.erase(vi);
				else
					vi++;
			// check if delete record from vector via iterator and then browse forward REALLY WORKS????
			// shift cards*/
		}
		//Root.CompressVertical();

		time(&t);
		if (t1 != t)
			printf("I(%d)",t-t1);
		else
			printf("I");

		Leaves.clear();
		Root.GetLeaves(Leaves);
		if (Leaves.empty())
		{
			PrintResults();
			return false;
		}
		//printf(" %d / %d ",Leaves.size(),Root.GetSize());

		time(&t1);
		if (t1 != t)
			printf("J(%d)",t1-t);
		else
			printf("J");

		PrintResults();
		Vars.clear();
		for (UINT l=0;l<Leaves.size();l++)
		{
//printf("%d",l);
//Variation var(Leaves[l]->Atk,Leaves[l]->Def);
//	printf("-%d",Leaves[l]->Atk < Leaves[l]->Def);
			pair<SVAR::iterator,bool> ir = Vars.insert(*Leaves[l]/*->Atk,Leaves[l]->Def*/);
//printf(";");
			if (!ir.second)
			{
				// update and reinsert
				Leaves[l]->IncWeight(ir.first->GetW());
				Vars.erase(ir.first);
				Vars.insert(*Leaves[l]);
			}
			
				//ir.first->IncW(Leaves[l]->GetWeight());
		}
		/*printf("J");
		for (SVAR::iterator vi=Vars.begin();vi!=Vars.end();vi++)
		{
			if (vi->GetFlag() == VAR_FLAG_DELETED)
			{
				do
				{
					vi = Vars.erase(vi);
				}
				while ((vi != Vars.end()) && (vi->GetFlag() == VAR_FLAG_DELETED));
			}
		}*/
		//Sweep(Leaves);
		time(&t);
		if (t1 != t)
			printf("K(%d) _%d_ %d < %d\n",(t-t1),Vars.size(),Leaves.size());
		else
			printf("K %d < %d\n",Vars.size(),Leaves.size());

		return true;
	}
public:
	Branching(ActiveDeck &atk, ActiveDeck &def, bool Surge = false)
	{
		dWWin = 0;
		dWLoss = 0;
		Root = Node(atk,def);
		bSurge = Surge;
		iTurn = 0;
		Leaves.reserve(RESERVE_LEAVES_COUNT);
	}
	bool Evaluate()
	{
		if (bSurge)
		{
			if (++iTurn > MAX_TURN)
				return false;
printf("%2d: ",iTurn);
			if (!AttackDeck(false))
				return false;
		}
		if (++iTurn > MAX_TURN)
			return false;
printf("%2d: ",iTurn);
		if (!AttackDeck(true))
			return false;
		if (!bSurge)
		{
			if (++iTurn > MAX_TURN)
				return false;
printf("%2d: ",iTurn);
			if (!AttackDeck(false))
				return false;
		}
		return true;
	}
};
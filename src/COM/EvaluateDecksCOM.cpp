// EvaluateDecksCOM.cpp : Implementation of CEvaluateDecksCOM

#include "stdafx.h"
#include "EvaluateDecksCOM.h"

// CEvaluateDecksCOM


//STDMETHODIMP CEvaluateDecksCOM::get_Msg(BSTR* pVal)
//{
//	//*pVal = SysAllocString(msg);
//	//*pVal = msg;
//
//	//CComBSTR tmp(msg);
//	//*pVal = tmp.Detach();
//
//	//CComBSTR str(L"Hello World");
//    //str.CopyTo(pVal);
//
//	//CComBSTR bStr("Hello World");
//    //*pVal= bStr.Detach();  // return local CComBSTR using Detach
//
//	//*pVal=SysAllocString(L"SHIT");
//
//	char* pCharval = "this string";
//	CComBSTR ret(pCharval);
//	*pVal = ret.Copy();
//	::SysFreeString(ret);
//
//	return S_OK;
//}


//STDMETHODIMP CEvaluateDecksCOM::get_Str(BSTR* pVal)
//{
//	char* pCharval = "this string";
//	CComBSTR ret(pCharval);
//	*pVal = ret.Copy();
//	::SysFreeString(ret);
//	return S_OK;
//}


//STDMETHODIMP CEvaluateDecksCOM::TestString(BSTR st1, BSTR* st2)
//{
//	CComBSTR ret1(st1);
//	*st2 = ret1.Copy();
//	::SysFreeString(ret1);
//	return S_OK;
//}


STDMETHODIMP CEvaluateDecksCOM::LoadCardXML(BSTR FileName, BYTE* ret)
{
	char buffer[255];
	wcstombs((CHAR*)&buffer,FileName,sizeof(buffer));
	if (DB.LoadCardXML((const char*)buffer))
	{
		*ret = 1;
		return S_OK;
	}
	else
	{
		*ret = 0;
		return S_FALSE;
	}
}

STDMETHODIMP CEvaluateDecksCOM::SetCommander(USHORT DeckIndex, SHORT Id, BYTE* ret)
{
	const Card *c = &DB.GetCard(Id);
	if (c->IsCard() && (c->GetType() == TYPE_COMMANDER))
	{
		if (!DeckIndex)
		{
			Bot.Commander = c;
			Bot.Commander.SetCardSkillProcBuffer(Bot.SkillProcs);
		}
		else
		{
			Top.Commander = c;
			Top.Commander.SetCardSkillProcBuffer(Top.SkillProcs);
		}
		*ret = 1;
		return S_OK;
	}
	else
	{
		*ret = 0;
		return S_FALSE;
	}
}


STDMETHODIMP CEvaluateDecksCOM::AppendCard(USHORT DeckIndex, USHORT Id, BYTE* ret)
{
	const Card *c = &DB.GetCard(Id);
	ActiveDeck *d;
	if (!DeckIndex) d = &Bot; else d = &Top;
	if (c->IsCard())
	{
		if (c->GetType() == TYPE_ASSAULT)
		{
			d->Units.push_back(c);
			d->Units.back().SetCardSkillProcBuffer(d->SkillProcs);
			*ret = d->Units.size();
		}
		if (c->GetType() == TYPE_STRUCTURE)
		{
			d->Structures.push_back(c);
			d->Structures.back().SetCardSkillProcBuffer(d->SkillProcs);
			*ret = d->Structures.size();
		}
		if (c->GetType() == TYPE_ACTION)
		{
			d->Actions.push_back(c);
			d->Actions.back().SetCardSkillProcBuffer(d->SkillProcs);
			*ret = d->Actions.size();
		}	
		if (c->GetType() == TYPE_COMMANDER)
		{
			d->Commander = c;
			d->Commander.SetCardSkillProcBuffer(d->SkillProcs);
			*ret = 1;
		}		
		return S_OK;
	}
	else
	{
		*ret = 0;
		return S_FALSE;
	}
}


STDMETHODIMP CEvaluateDecksCOM::AppendCardEx(USHORT DeckIndex, USHORT Id, USHORT Attack, SHORT Health, BYTE* ret)
{
	const Card *c = &DB.GetCard(Id);
	ActiveDeck *d;
	if (!DeckIndex) d = &Bot; else d = &Top;
	if (c->IsCard())
	{
		if (c->GetType() == TYPE_ASSAULT)
		{
			d->Units.push_back(c);
			d->Units.back().SetCardSkillProcBuffer(d->SkillProcs);
			d->Units.back().SetAttack((UCHAR)Attack);
			d->Units.back().SetHealth((UCHAR)Health);
			*ret = d->Units.size();
		}
		if (c->GetType() == TYPE_STRUCTURE)
		{
			d->Structures.push_back(c);
			d->Structures.back().SetCardSkillProcBuffer(d->SkillProcs);
			d->Structures.back().SetHealth((UCHAR)Health);
			*ret = d->Structures.size();
		}
		if (c->GetType() == TYPE_ACTION)
		{
			d->Actions.push_back(c);
			d->Actions.back().SetCardSkillProcBuffer(d->SkillProcs);
			*ret = d->Actions.size();
		}	
		if (c->GetType() == TYPE_COMMANDER)
		{
			d->Commander = c;
			d->Commander.SetCardSkillProcBuffer(d->SkillProcs);
			d->Commander.SetHealth((UCHAR)Health);
			*ret = 1;
		}
		return S_OK;
	}
	else
	{
		*ret = 0;
		return S_FALSE;
	}
}


STDMETHODIMP CEvaluateDecksCOM::SetEffect(USHORT DeckIndex, USHORT CardPosition, BSTR AbilityName, USHORT EffectValue, BYTE* ret)
{
	ActiveDeck *d;
	if (!DeckIndex) d = &Bot; else d = &Top;
	if (CardPosition >= d->Units.size())
	{
		*ret = 0;
		return S_FALSE;
	}
	char buffer[255];
	wcstombs((CHAR*)&buffer,AbilityName,sizeof(buffer));
	UCHAR AbilityID = DB.GetSkillID((const char*)buffer);
	if (!AbilityID)
	{
		*ret = 0;
		return S_FALSE;
	}
	d->Units[CardPosition].SetEffect(AbilityID,(UCHAR)EffectValue);
	*ret = (UCHAR)EffectValue;
	return S_OK;
}


STDMETHODIMP CEvaluateDecksCOM::Attack(USHORT DeckIndex)
{
	if (!DeckIndex)
		Bot.AttackDeck(Top,true);
	else
		Top.AttackDeck(Bot,true);
	return S_OK;
}

STDMETHODIMP CEvaluateDecksCOM::GetCardAttack(USHORT DeckIndex, USHORT RowIndex, USHORT CardPosition, BYTE* ret)
{
	ActiveDeck *d;
	if (!DeckIndex) d = &Bot; else d = &Top;
	if (RowIndex > 1)
	{
		// Assault
		if (d->Units.size() >= CardPosition)
		{
			*ret = 0;
			return S_FALSE;
		}
		else
			*ret = d->Units[CardPosition].GetNativeAttack();
	}
	else
		// Commander or Structure
		*ret = 0;
	return S_OK;
}


STDMETHODIMP CEvaluateDecksCOM::GetCardHealth(USHORT DeckIndex, USHORT RowIndex, USHORT CardPosition, BYTE* ret)
{
	ActiveDeck *d;
	if (!DeckIndex) d = &Bot; else d = &Top;
	if (!RowIndex)
	{
		// Commander
		*ret = d->Commander.GetHealth();
	}
	else
		if (RowIndex == 1)
		{
			// Structure
			if (d->Structures.size() >= CardPosition)
			{
				*ret = 0;
				return S_FALSE;
			}
			else
				*ret = d->Structures[CardPosition].GetHealth();
		}
		else
		{
			// Assault
			if (d->Units.size() >= CardPosition)
			{
				*ret = 0;
				return S_FALSE;
			}
			else
				*ret = d->Units[CardPosition].GetHealth();
		}
	return S_OK;	
}


STDMETHODIMP CEvaluateDecksCOM::GetCardEffect(USHORT DeckIndex, USHORT CardPosition, USHORT EffectNo, BSTR* EffectName, BYTE* ret)
{
	ActiveDeck *d;
	if (!DeckIndex) d = &Bot; else d = &Top;
	if (d->Units.size() >= CardPosition)
	{
		
		UCHAR ano = 0;
		for (UCHAR i=0;i<CARD_ABILITIES_MAX;i++)
			if (d->Units[CardPosition].GetEffect(i))
			{
				ano++;
				if (ano > EffectNo)
				{
					*ret = d->Units[CardPosition].GetEffect(i);
					CComBSTR ret1(DB.Skills[i].SkillName);
					*EffectName = ret1.Copy();
					::SysFreeString(ret1);
					return S_OK;
				}
			}
		
	}
	*ret = 0;
	return S_FALSE;
}

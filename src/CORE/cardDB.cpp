#include "cardDB.hpp"

#include <cstring>
// FIXME remove ancient C io code
#include <cstdio>
#include "compat.h"
#include "deckIndex.hpp"
#include "assert.hpp"
#include "cardSet.hpp"
#include "cardPool.hpp"

#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

// TODO should be replaced by a cheaper implementation
#include "activeDeck.hpp"

// For XML stuff.
// I am fairly certain MSVC can understand forward slashes aswell
#include "pugixml/src/pugixml.hpp"
#include "pugixml/src/pugixml.cpp"

#include <boost/algorithm/string/predicate.hpp>

namespace IterateDecks {
    namespace Core {

        inline bool strEqualIgnoreCase(char const * const a, char const * const b) {
            return boost::iequals(a,b);
        }

        // FIXME: Remove global variable
        extern bool bConsoleOutput;

        #define CARD_NAME_SYNTAX_EXCLUSION	"Kapak, the Deceiver"

        char * FormatCardName(char * Name)
        {
            UCHAR cut = 0;
            for (UCHAR i=0;i<250;i++) {
                if (!Name[i]) {
                    break;
                } else {
                    if (Name[i] == ',') {
                        cut++;
                    }
                    if (cut > 0) {
                        Name[i] = Name[i+cut];
                    }
                }
            }
            return Name;
        }

        AchievementRequirementCompare DetectCompare(char const * const compare)
        {
            // Lets use c++ strings
            std::string sCompare(compare);
            // make it lower case
            std::transform(sCompare.begin(), sCompare.end(), sCompare.begin(), ::tolower);

            if (!compare) {
                return UNDEFINED;
            } else if (!compare[0]) {
                return UNDEFINED;
            } else if (sCompare.compare("less") == 0) {
                return LESS;
            } else if (sCompare.compare("less_equal") == 0) {
                return LESSEQUAL;
            } else if (sCompare.compare("great") == 0) {
                return GREATER;
            } else if (sCompare.compare("great_equal") == 0) {
                return GREATEREQUAL;
            } else {
                return EQUAL;
            }
        }

        CardDB::CardDB()
        {
            memset(this->CDB, 0, CARD_MAX_ID * sizeof(Card));
            this->Initialize();
        }

        bool CardDB::SaveIndex(char const * FileName, MDECKS & index)
        {
            FILE *f;
            if (!fopen_s(&f,FileName,"w"))
            {
                for (MDECKS::iterator mi = index.begin(); mi != index.end(); mi++)
                {
                    fprintf_s(f,"%s:",mi->first.name.c_str());
                    for (VSTRINGS::iterator vi = mi->second.begin(); vi != mi->second.end(); vi++)
                    {
                        if (vi != mi->second.begin())
                            fprintf_s(f,",");
                        fprintf_s(f,"%s",vi->c_str());
                    }
                    fprintf_s(f,"\n");
                }
                return (fclose(f) == 0);
            }
            return false;
        }

        CardType CardDB::DetectTypeByID(UINT ID)
        {
            // Card ID < 1000 = Creature, Card ID < 2000 = Commander, Card ID < 3000 = Building, else Spell.
            if (ID < 1000)
                return TYPE_ASSAULT;
            if (ID < 2000)
                return TYPE_COMMANDER;
            if (ID < 3000)
                return TYPE_STRUCTURE;
            if (ID < 4000)
                return TYPE_ACTION;
            if (ID < 5000)
                return TYPE_ASSAULT;
            return TYPE_ASSAULT;
        }

        Faction CardDB::RemapFaction(UCHAR XmlID)
        {
        //<unitType> <id>1</id> <name>Imperial</name> <image>CardImperialStyled</image> </unitType> -<unitType>
        //<unitType> <id>3</id> <name>Bloodthirsty</name> <image>CardBloodthirstyStyle</image> </unitType>
        //<unitType> <id>4</id> <name>Xeno</name> <image>CardXenoStyle</image> </unitType>
        //<unitType> <id>9</id> <name>Raider</name> <image>CardRaiderStyle</image> </unitType>
            if (XmlID == 1)
                return FACTION_IMPERIAL;
            if (XmlID == 3)
                return FACTION_BLOODTHIRSTY;
            if (XmlID == 4)
                return FACTION_XENO;
            if (XmlID == 8)
                return FACTION_RIGHTEOUS;
            if (XmlID == 9)
                return FACTION_RAIDER;
            return FACTION_NONE;
        }

        Rarity CardDB::RemapRarity(UCHAR XmlID, UCHAR UniqueFlag)
        {
            if (XmlID == 1)
                return RARITY_COMMON;
            if (XmlID == 2)
                return RARITY_UNCOMMON;
            if (XmlID == 3)
            {
                if (UniqueFlag)
                    return RARITY_UNIQUE;
                else
                    return RARITY_RARE;
            }
            if (XmlID == 4)
            {
                if (UniqueFlag)
                    return RARITY_LEGENDARY;
                else
                    return RARITY_LEGENDARY; // that was only for Fenrir ??????
            }
            if (XmlID == 10)
                return RARITY_STORYCOMMANDER;
            // rarity UNKNOWN
            return RARITY_COMMON;
        }
        const Card *CardDB::GetPointer() const { return (const Card *)&CDB[0]; }
        bool CardDB::LoadCardXML(const char *FileName, char *returnnewcards, size_t MaxBufferSize)
        {
            pugi::xml_document doc;
            if (returnnewcards)
                returnnewcards[0] = 0;
            if (!doc.load_file(FileName)) {
                std::cerr << "Error opening file: " << FileName << std::endl;
                assertX(false);
                return false;
            }

            size_t loaded = 0;
            // FIXME: unused variable
            size_t ss = 0;
            pugi::xml_node root = doc.child("root");
            for (pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
            {
                if (strEqualIgnoreCase(it->name(),"skillType"))
                {
                    const char *id = it->child("id").child_value();
                    //const char *name = it->child("name").child_value();
                    //<cardValue passive='1' cost='2.5' />
                    for (pugi::xml_node child = it->first_child(); child; child = child.next_sibling())
                    {
                        if (strEqualIgnoreCase(child.name(),"cardValue"))
                        {
                            MSKILLS::iterator si = SIndex.find(id);
                            if (si == SIndex.end())
                            {
                                if (bConsoleOutput) {
                                    printf("Skill \"%s\" not found in index!\n",id);
                                }
                            }
                            else
                            {
                                Skills[si->second].IsPassive = child.attribute("passive").as_bool();
                                Skills[si->second].CardValue = child.attribute("cost").as_float();
                                //printf("%s : %d %.1f\n",id,child.attribute("passive").as_bool(),child.attribute("cost").as_float());
                            }
                        }
                    }
                }
                if (strEqualIgnoreCase(it->name(),"unit"))
                {
                    char Name[250];
                    strcpy(Name,it->child("name").child_value());
                    FormatCardName(Name);
                    UINT Id = atoi(it->child("id").child_value());
                    const char *Pic = it->child("picture").child_value();
                    UCHAR Attack = atoi(it->child("attack").child_value());
                    UCHAR Health = atoi(it->child("health").child_value());
                    UCHAR Rarity = atoi(it->child("rarity").child_value());
                    Faction faction = RemapFaction(atoi(it->child("type").child_value()));
                    UCHAR Wait = atoi(it->child("cost").child_value());
                    UCHAR Unique = atoi(it->child("unique").child_value());
                    UINT Set = atoi(it->child("set").child_value());
                    /*if ((!Wait) && (it->child("attack").empty()) && (it->child("cost").empty()))
                    {
                        printf("Set : %d\n",Set);
                        ss = Set;
                    }*/
                    //if (Id == 426)
                    //{
                    //	printf("Apollo: %d\n",Id);
                    //}
                    Card c(Id,Name,Pic,RemapRarity(Rarity,Unique),DetectTypeByID(Id)/*(Attack,Health,Wait,Faction,(!it->child("attack").empty()),(!it->child("cost").empty()))*/,faction,Attack,Health,Wait,Set);
                    //printf("%s: %d %d %d %d %d\n",Name,Attack,Health,Rarity,Faction,Wait);
                    //<name>Duncan</name>
                    //<attack>0</attack>
                    //<health>10</health>
                    //<rarity>2</rarity>
                    //<skill x="1" id="rally" y="9"/>
                    //<skill x="1" id="siege"/>
                    //<skill x="2" id="heal" y="3" all="1"/>
                    //<skill x="1" id="repair" all="1"/>
                    //<type>9</type>
                    for (pugi::xml_node child = it->first_child(); child; child = child.next_sibling())
                    {
                        //printf("%s\n",child.name());
                        //printf("  %s = %s\n",child.name(),child.child_value());
                        if (strEqualIgnoreCase(child.name(),"skill"))
                        {
                            UCHAR Id = GetSkillID(child.attribute("id").value());
                            assertX(Id); // unknown skill
                            if (!Id)
                                continue;
                            EFFECT_ARGUMENT Effect = child.attribute("x").as_uint();
                            if (!Effect) {
                                Effect = ABILITY_ENABLED; // this value can't be 0, since it will disable the ability
                            }
                            UCHAR TC = child.attribute("all").as_uint();
                            if (!TC)
                                TC = TARGETSCOUNT_ONE;
                            else
                                TC = TARGETSCOUNT_ALL;
                            UCHAR TF = RemapFaction(child.attribute("y").as_uint());
                            UCHAR skillevent = EVENT_EMPTY;
                            if (child.attribute("died").as_uint() > 0) {
                                skillevent += EVENT_DIED;
                            }
                            if (child.attribute("played").as_uint() > 0 || Id == SPECIAL_BLITZ) {
                                skillevent += EVENT_PLAYED;
                            }
                            if (child.attribute("attacked").as_uint() > 0) {
                                skillevent += EVENT_ATTACKED;
                            }
                            if (child.attribute("kill").as_uint() > 0) {
                                skillevent += EVENT_KILL;
                            }
                            c.AddAbility(Id,Effect,TC,TF,skillevent);
                            //for (pugi::xml_attribute attr = child.first_attribute(); attr; attr = attr.next_attribute())
                                //printf("    %s = %s\n",attr.name(),attr.value());
                        }
                    }
                    //MSUINT::iterator mi = Index.find(c.GetName());
                    //if (mi != Index.end())
                    //	printf("Conflicted: %s : %d and %d\n",c.GetName(),mi->second,c.GetId());
                    // so card should be treated as new if it's picture changed, so API would know it should download new pic
                    bool isnew = ((!CDB[Id].IsCard()) || (strEqualIgnoreCase(CDB[Id].GetPicture(),c.GetPicture())));
                    bool ins = Insert(c);
                    if (ins && returnnewcards && isnew)
                    {
                        if (returnnewcards[0])
                            strcat_s(returnnewcards,MaxBufferSize,",");
                        strcat_s(returnnewcards,MaxBufferSize,it->child("id").child_value());
                    }
                    loaded++;
                }
                else
                    if (strEqualIgnoreCase(it->name(),"cardSet"))
                    {
                        UINT Id = atoi(it->child("id").child_value());
                        const char *Name = it->child("name").child_value();
                        const char *Icon = it->child("icon").child_value();
                        UINT Visible = atoi(it->child("visible").child_value());
                        SetIndex.insert(PAIRMSETS(Id,CardSet((char*)Name,(char*)Icon,(Visible != 0))));
                    }
            }
            return (loaded > 0);
        }
        const UCHAR CardDB::RemapUnitType(const UCHAR unitType)
        {
            switch (unitType)
            {
                case 1: return TYPE_COMMANDER;
                case 2: return TYPE_ASSAULT;
                case 4: return TYPE_STRUCTURE;
                case 8: return TYPE_ACTION;
            }
            return 0;
        };
        bool CardDB::LoadAchievementXML(const char *FileName)
        {
            pugi::xml_document doc;
            if (!doc.load_file(FileName)) {
                std::cerr << "Error opening file: " << FileName << std::endl;
                assertX(false);
                return false;
            }

            size_t loaded = 0;
            AIIndex.clear(); // clean index, we don't really need to clean array, it doesn't take too much space and cleaning it up will take time
            pugi::xml_node root = doc.child("root");
            for (pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
            {
                if (strEqualIgnoreCase(it->name(),"achievement"))
                {
                    const char *Name = it->child("name").child_value();
                    UINT Id = atoi(it->child("id").child_value());
                    const char *Desc = it->child("desc").child_value();
                    UINT Number = atoi(it->child("number").child_value());
                    UINT Order = atoi(it->child("order").child_value());

                    if (Order > 1) continue;
                    if (!it->child("type").attribute("tournament_id").empty()) continue; // skip <type tournament_id='*' />

                    UINT index = (UINT)AIIndex.size();
                    assertX(index < ACHIEVEMENT_MAX_COUNT);


                    AchievementInfo ai(Id,Name,Desc,Number);
                    // <type enemy_id="*" winner="1"/> - we don't really need those
                    // <type mission_id="166" winner="1"/>
                    // <type mission_id="172" compare="great_equal" winner="1"/>
                    if (!it->child("type").empty())
                        ai.SetType(
                            it->child("type").attribute("enemy_id").value(),
                            it->child("type").attribute("mission_id").as_uint(),
                            it->child("type").attribute("winner").as_bool(),
                            DetectCompare(it->child("type").attribute("compare").value()));
                    for (pugi::xml_node_iterator di = it->begin(); di != it->end(); ++di)
                        if (strEqualIgnoreCase(di->name(),"req"))
                        {
                            AchievementRequirement r;
                            if (!di->attribute("skill_id").empty())
                            {
                                MSKILLS::iterator si = SIndex.find(di->attribute("skill_id").value());
                                if (si == SIndex.end())
                                {
                                    if (!strcmp(di->attribute("skill_id").value(),"0"))
                                    {
                                        if (bConsoleOutput)
                                            printf("Skill \"%s\" not found in index!\n",di->attribute("skill_id").value());
                                        r.SkillID = SPECIAL_ATTACK;
                                    }
                                    else
                                        continue;
                                }
                                else
                                    r.SkillID = si->second;
                            }
                            // <req num_turns="9" compare="less_equal"/>
                            // <req unit_type="2" num_killed="10" compare="equal"/>
                            // <req skill_id="disease" num_used="5" status="1"/>
                            if (!di->attribute("num_turns").empty())
                                r.NumTurns = di->attribute("num_turns").as_uint();
                            if (!di->attribute("unit_id").empty())
                                r.UnitID = di->attribute("unit_id").as_uint();
                            if (!di->attribute("unit_race").empty())
                                r.UnitRace = RemapFaction(di->attribute("unit_race").as_uint());
                            if (!di->attribute("unit_rarity").empty())
                                r.UnitRarity = RemapRarity(di->attribute("unit_rarity").as_uint());
                            if (!di->attribute("num_played").empty())
                                r.NumPlayed = di->attribute("num_played").as_uint();
                            if (!di->attribute("unit_type").empty())
                                r.UnitType = RemapUnitType(di->attribute("unit_type").as_uint());
                            if (!di->attribute("num_killed").empty())
                                r.NumKilled = di->attribute("num_killed").as_uint();
                            if (!di->attribute("num_used").empty())
                                r.NumUsed = di->attribute("num_used").as_uint();
                            if (!di->attribute("num_killed_with").empty())
                                r.NumKilledWith = di->attribute("num_killed_with").as_uint();
                            if (!di->attribute("status").empty())
                                r.Status = di->attribute("status").as_bool();
                            if (!di->attribute("damage").empty())
                                r.Damage = di->attribute("damage").as_uint();
                            if (!di->attribute("com_total").empty())
                                r.ComTotal = di->attribute("com_total").as_uint();
                            if (!di->attribute("compare").empty())
                                r.Compare = DetectCompare(di->attribute("compare").value());
                            ai.AddRequirement(r);
                        }
                    if (ai.IsValid())
                    {
                        AIIndex.insert(PAIRMSUINT(Name,index));
                        ADB[index] = ai;
                        loaded++;
                    }
                }
            }
            return (loaded > 0);
        }
        bool CardDB::LoadMissionXML(const char *FileName)
        {
            pugi::xml_document doc;
            if (!doc.load_file(FileName)) {
                std::cerr << "Error opening file: " << FileName << std::endl;
                assertX(false);
                return false;
            }

            size_t loaded = 0;
            MIIndex.clear(); // clean index, we don't really need to clean array, it doesn't take too much space and cleaning it up will take time
            pugi::xml_node root = doc.child("root");
            for (pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
            {
                if (strEqualIgnoreCase(it->name(),"mission"))
                {
                    const char *Name = it->child("name").child_value();
                    UINT Id = atoi(it->child("id").child_value());
                    UINT Commander = atoi(it->child("commander").child_value());

                    assertX(Id < MISSION_MAX_ID);
                    if (Id >= MISSION_MAX_ID)
                        continue;
                    MDB[Id] = MissionInfo(Commander,Name);
                    MIIndex.insert(PAIRMSUINT(Name,Id));

                    pugi::xml_node deck = it->child("deck");
                    for (pugi::xml_node_iterator di = deck.begin(); di != deck.end(); ++di)
                    {
                        assertX(strEqualIgnoreCase(di->name(),"card")); // all siblings must be "card", but, i'd better check
                        MDB[Id].Add(atoi(di->child_value()));
                        //printf("%d : %s\n",Id,di->child_value());
                    }
                    loaded++;
                }
            }
            return (loaded > 0);
        }
        bool CardDB::LoadRaidXML(const char *FileName)
        {
            pugi::xml_document doc;
            if (!doc.load_file(FileName)) {
                std::cerr << "Error opening file: " << FileName << std::endl;
                assertX(false);
                return false;
            }

            size_t loaded = 0;
            RIIndex.clear(); // clean index, we don't really need to clean array, it doesn't take too much space and cleaning it up will take time
            pugi::xml_node root = doc.child("root");
            for (pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
            {
                if (strEqualIgnoreCase(it->name(),"raid"))
                {
                    const char *Name = it->child("name").child_value();
                    UINT Id = atoi(it->child("id").child_value());
                    UINT Commander = atoi(it->child("commander").child_value());

                    assertX(Id < RAID_MAX_ID);
                    RDB[Id] = RaidInfo(Commander,Name);
                    RIIndex.insert(PAIRMSUINT(Name,Id));
                    loaded++;

                    pugi::xml_node deck = it->child("deck");
                    pugi::xml_node alwaysinclude = deck.child("always_include");
                    for (pugi::xml_node_iterator di = alwaysinclude.begin(); di != alwaysinclude.end(); ++di)
                    {
                        assertX(strEqualIgnoreCase(di->name(),"card")); // all siblings must be "card", but, i'd better check
                        RDB[Id].AddAlways(atoi(di->child_value()));
                    }
                    for (pugi::xml_node_iterator di = deck.begin(); di != deck.end(); ++di)
                        if (strEqualIgnoreCase(di->name(),"card_pool"))
                        {
                            CardPool p(di->attribute("amount").as_int());
                            for (pugi::xml_node_iterator ti = di->begin(); ti != di->end(); ++ti)
                            {
                                //assertX(strEqualIgnoreCase(ti->name(),"card")); // all siblings must be "card", but, i'd better check
                                // ok here can be mistakes, gotta seed them out
                                if (strEqualIgnoreCase(ti->name(),"card"))
                                    p.pool.push_back(atoi(ti->child_value()));
                            }
                            RDB[Id].AddPool(p);
                        }
                }

            }
            return (loaded > 0);
        }
        bool CardDB::LoadQuestXML(const char *FileName)
        {
            pugi::xml_document doc;
            if (!doc.load_file(FileName)) {
                std::cerr << "Error opening file: " << FileName << std::endl;
                throw LogicError("");
            }

            size_t loaded = 0;
            RIIndex.clear(); // clean index, we don't really need to clean array, it doesn't take too much space and cleaning it up will take time
            pugi::xml_node root = doc.child("root");
            for (pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it)
            {
                if (strEqualIgnoreCase(it->name(),"battleground"))
                {
                    const char *Name = it->child("name").child_value();
                    const char *Desc = it->child("desc").child_value();
                    UINT Id = atoi(it->child("id").child_value());
                    BattleGroundEffect effect = BattleGroundEffect::normal;
                    if (!it->child("effect").empty())
                    {
                        MapBattleGroundEffects::iterator si = QuestEffectIndex.find(it->child("effect").child_value());
                        if (si == QuestEffectIndex.end()) {
                            if (bConsoleOutput) {
                                printf("Quest effect \"%s\" not found in index!\n",it->child("effect").child_value());
                            }
                        } else {
                            effect = si->second;
                        }
                    }
                    assertX(Id < BATTLEGROUND_MAX_ID);
                    BGDB[Id] = BgInfo(Id,Name,Desc,effect);
                    loaded++;
                }
                else
                    if (strEqualIgnoreCase(it->name(),"step"))
                    {
                        UINT Id = atoi(it->child("id").child_value());
                        UINT BgId = atoi(it->child("battleground_id").child_value());
                        UINT Commander = atoi(it->child("commander").child_value());

                        assertX(Id < RAID_MAX_ID);
                        STDB[Id] = StepInfo(Id,BgId,Commander);
                        loaded++;

                        pugi::xml_node deck = it->child("deck");
                        for (pugi::xml_node_iterator di = deck.begin(); di != deck.end(); ++di)
                            if (strEqualIgnoreCase(di->name(),"card_pool"))
                            {
                                CardPool p(di->attribute("amount").as_int());
                                for (pugi::xml_node_iterator ti = di->begin(); ti != di->end(); ++ti)
                                {
                                    //assertX(strEqualIgnoreCase(ti->name(),"card")); // all siblings must be "card", but, i'd better check
                                    // ok here can be mistakes, gotta seed them out
                                    if (strEqualIgnoreCase(ti->name(),"card"))
                                        p.pool.push_back(atoi(ti->child_value()));
                                }
                                STDB[Id].AddPool(p);
                            }
                    }

            }
            return (loaded > 0);
        }
        const BattleGroundEffect CardDB::GetQuestEffectId(UINT QuestId)
        {
            assertX(QuestId < STEP_MAX_ID);
            UINT bg = STDB[QuestId].GetBGId();
            assertX(bg < BATTLEGROUND_MAX_ID);
            return BGDB[bg].GetEffectId();
        }
        bool CardDB::RateCard(const UINT Id, double &OffenceValue, double &DefenceValue, const UCHAR iFormulaVersion)
        {
            const Card *c = &CDB[Id];
            if (!c->IsCard()) // invalid
                return false;
            float fd = 0.0, fo = 0.0;
            for (UCHAR k=0;k<c->GetAbilitiesCount();k++)
            {
                UCHAR aid = c->GetAbilityInOrder(k);
                UCHAR cnt = c->GetTargetCount(aid);
                if (cnt < 1)
                    cnt = 1;
                float fmod = ((float)cnt + 5) / 6;//(cnt + 1) / 2;
                if (c->GetTargetFaction(aid) != FACTION_NONE)
                    fmod *= 0.75; // should be 0.75 or 0.8 methinks
                //ss += c->GetAbility(aid) * DB.Skills[aid].CardValue * fmod;
                //printf("[%d]: %d x %.1f x %.1f ~ %.1f | ",cnt,c->GetAbility(aid),DB.Skills[aid].CardValue,fmod,c->GetAbility(aid) * DB.Skills[aid].CardValue * fmod);
                //if (c->GetTargetFaction(aid) != FACTION_NONE)
                //	printf(" specific");
                //if (c->GetTargetFaction(aid) != FACTION_NONE)
                //	fmod /= 2;
                if (Skills[aid].IsPassive)
                    fd += c->GetAbility(aid) * Skills[aid].CardValue;
                else
                    fo += c->GetAbility(aid) * Skills[aid].CardValue * fmod;
                //f += c->GetAbility(aid) * DB.Skills[aid].CardValue;/* * fmod /** (1 + DB.Skills[aid].IsPassive) / 1*/;
                //printf("%d : %d x %.1f = %.1f | ",aid,c->GetAbility(aid),DB.Skills[aid].CardValue,c->GetAbility(aid) * DB.Skills[aid].CardValue);
            }
            //printf("%.1f + %.1f	%.1f	|",fd,fo,fo+fd+2.5*c->GetAttack()+1.5*(c->GetHealth()/*-1*/)-2.5*c->GetWait());
            //printf("	%.1f\n",fo / (c->GetWait() + 1)+fd+2.5*c->GetAttack() / (c->GetWait() + 0.5)+1.5*(c->GetHealth()/*-1*/));
            /*printf("	D %.1f	O- %.1f	O* %.1f\n",
                fd + 1.5*(c->GetHealth()),
                fo + 2.5*c->GetAttack() - 2.5*c->GetWait(),
                fo / (c->GetWait() + 0.5) + 2.5*c->GetAttack() / (c->GetWait() + 0.5)
                );*/
            if (!iFormulaVersion)
            {
                OffenceValue = (fo + 1.5*c->GetAttack()) * (10 - c->GetWait()) / 10;
                DefenceValue = fd + 1*(c->GetHealth());
            }
            else
            {
                OffenceValue = fo + 2.5*c->GetAttack() - 2.5*c->GetWait();
                DefenceValue = fd + 1.5*(c->GetHealth());
            }
            return true;
        }
        void CardDB::GenRaidDeck(ActiveDeck &D, UINT RaidID)
        {
            assertX(RaidID < RAID_MAX_ID);
            assertX(RDB[RaidID].IsValid());
            unsigned int const & commanderId(RDB[RaidID].GetCommander());
            D = ActiveDeck(&CDB[commanderId],CDB); // replace
            RDB[RaidID].GetAlways(CDB,D.Deck);
            RDB[RaidID].GetPools(CDB,D.Deck);
        }
        void CardDB::GenQuestDeck(ActiveDeck &D, UINT QuestID)
        {
            assertX(QuestID < STEP_MAX_ID);
            assertX(STDB[QuestID].IsValid());
            unsigned int const & commanderId(STDB[QuestID].GetCommander());
            D = ActiveDeck(&CDB[commanderId],CDB); // replace
            STDB[QuestID].GetPools(CDB,D.Deck);
        }
        DWORD CardDB::LoadPointers(Card **Ptr, DWORD MaxCount)
        {
            DWORD cnt = 0;
            for (DWORD i=0;(i<CARD_MAX_ID) && (cnt<MaxCount);i++)
                if (CDB[i].IsCard())
                {
                    Ptr[cnt] = &CDB[i];
                    cnt++;
                }
            return cnt;
        }
        void CardDB::Initialize()
        {
            // must load skills earlier than anything
            AddSkill(ACTIVATION_CHAOS,"Chaos");
            AddSkill(ACTIVATION_CLEANSE,"Cleanse");
            AddSkill(ACTIVATION_ENFEEBLE,"Enfeeble");
            AddSkill(ACTIVATION_HEAL,"Heal");
            AddSkill(ACTIVATION_INFUSE,"Infuse");
            AddSkill(ACTIVATION_FREEZE,"Freeze");
            AddSkill(ACTIVATION_JAM,"Jam");
            AddSkill(ACTIVATION_MIMIC,"Mimic");
            AddSkill(ACTIVATION_PROTECT,"Protect");
            AddSkill(ACTIVATION_RALLY,"Rally");
            AddSkill(ACTIVATION_RECHARGE,"Recharge");
            AddSkill(ACTIVATION_REPAIR,"Repair");
            AddSkill(ACTIVATION_RUSH,"Rush");
            AddSkill(ACTIVATION_SHOCK,"Shock");
            AddSkill(ACTIVATION_SIEGE,"Siege");
            AddSkill(ACTIVATION_SPLIT,"Split");
            AddSkill(ACTIVATION_STRIKE,"Strike");
            AddSkill(ACTIVATION_SUMMON,"Summon");
            AddSkill(ACTIVATION_SUPPLY,"Supply");
            AddSkill(ACTIVATION_WEAKEN,"Weaken");
            AddSkill(ACTIVATION_AUGMENT,"Augment");

            AddSkill(DEFENSIVE_ARMORED,"Armored");
            AddSkill(DEFENSIVE_COUNTER,"Counter");
            AddSkill(DEFENSIVE_EVADE,"Evade");
            AddSkill(DEFENSIVE_FLYING,"Flying");
            AddSkill(DEFENSIVE_INTERCEPT,"Intercept");
            AddSkill(DEFENSIVE_PAYBACK,"Payback");
            AddSkill(DEFENSIVE_REFRESH,"Refresh");
            AddSkill(DEFENSIVE_REGENERATE,"Regenerate");
            AddSkill(DEFENSIVE_TRIBUTE,"Tribute");
            AddSkill(DEFENSIVE_WALL,"Wall");
            AddSkill(DEFENSIVE_EMULATE,"Emulate");

            AddSkill(COMBAT_ANTIAIR,"AntiAir");
            AddSkill(COMBAT_BURST,"Burst");
            AddSkill(COMBAT_FEAR,"Fear");
            AddSkill(COMBAT_FLURRY,"Flurry");
            AddSkill(COMBAT_PIERCE,"Pierce");
            AddSkill(COMBAT_PIERCE,"Pierce_p"); // lets assume Pierce = Pierce_p
            AddSkill(COMBAT_SWIPE,"Swipe");
            AddSkill(COMBAT_VALOR,"Valor");

            AddSkill(DMGDEPENDANT_BERSERK,"Berserk");
            AddSkill(DMGDEPENDANT_CRUSH,"Crush");
            AddSkill(DMGDEPENDANT_DISEASE,"Disease");
            AddSkill(DMGDEPENDANT_IMMOBILIZE,"Immobilize");
            AddSkill(DMGDEPENDANT_LEECH,"Leech");
            AddSkill(DMGDEPENDANT_POISON,"Poison");
            AddSkill(DMGDEPENDANT_SIPHON,"Siphon");

            AddSkill(SPECIAL_BACKFIRE,"Backfire");

            AddSkill(SPECIAL_FUSION,"Fusion");
            AddSkill(SPECIAL_MIST,"Mist");
            AddSkill(SPECIAL_BLIZZARD,"Blizzard");
            AddSkill(SPECIAL_BLITZ,"Blitz");
            AddSkill(SPECIAL_LEGION,"Legion");

            AddSkill(SPECIAL_ATTACK,"Attack");

            AddQuestEffect(BattleGroundEffect::timeSurge,"time_surge");
            AddQuestEffect(BattleGroundEffect::copyCat,"copycat");
            AddQuestEffect(BattleGroundEffect::quicksilver,"quicksilver");
            AddQuestEffect(BattleGroundEffect::decay,"decay");
            AddQuestEffect(BattleGroundEffect::highSkies,"high_skies");
            AddQuestEffect(BattleGroundEffect::impenetrable,"impenetrable");
            AddQuestEffect(BattleGroundEffect::invigorate,"invigorate");
            AddQuestEffect(BattleGroundEffect::cloneProject,"clone_project");
            AddQuestEffect(BattleGroundEffect::friendlyFire,"friendly_fire");
            AddQuestEffect(BattleGroundEffect::genesis,"genesis");
            AddQuestEffect(BattleGroundEffect::artilleryStrike,"artillery_strike");
            AddQuestEffect(BattleGroundEffect::photonShield,"photon_shield");
            AddQuestEffect(BattleGroundEffect::enfeebleAll,"enfeeble_all");
            AddQuestEffect(BattleGroundEffect::protectAll,"protect_all");
            AddQuestEffect(BattleGroundEffect::commanderFreeze,"commander_freeze");
            AddQuestEffect(BattleGroundEffect::splitFive,"split_five");
            AddQuestEffect(BattleGroundEffect::poisonAll,"poison_all");
        }
        void CardDB::AddSkill(UCHAR Id, const char *Name)
        {
            char buffer[CARD_NAME_MAX_LENGTH];
            strcpy_s(buffer,CARD_NAME_MAX_LENGTH,Name);
            strlwr(buffer);
            strcpy_s(Skills[Id].SkillName,CARD_NAME_MAX_LENGTH,Name);
            SIndex.insert(PAIRMSKILLS(buffer,Id));
        }
        void CardDB::AddQuestEffect(BattleGroundEffect effect, const char *Name)
        {
            char buffer[CARD_NAME_MAX_LENGTH];
            strcpy_s(buffer,CARD_NAME_MAX_LENGTH,Name);
            strlwr(buffer);
            QuestEffectIndex.insert(PairBattleGroundEffects(buffer,effect));
        }
        const char *CardDB::GetSkill(UCHAR Indx) const
        {
            return Skills[Indx].SkillName;
        }
        UCHAR CardDB::GetSkillID(const char *Name)
        {
            char buffer[CARD_NAME_MAX_LENGTH];
            strcpy_s(buffer,CARD_NAME_MAX_LENGTH,Name);
            strlwr(buffer);
            MSKILLS::iterator it = SIndex.find(buffer);
            if (it == SIndex.end())
                return 0;
            else
                return it->second;
        }
        UCHAR CardDB::GetSkillIDSlow(const char *Name)
        {
            for (UCHAR i=0;i<CARD_ABILITIES_MAX;i++)
                if (strEqualIgnoreCase(Name,Skills[i].SkillName))
                    return i;
            return 0; // not found
        }
        size_t CardDB::GetSize()
        {
            return Index.size();
        }
        bool CardDB::Insert(Card &c)
        {
            if (c.GetId())
            {
                if (c.GetId() > CARD_MAX_ID)
                    throw 0;
                CDB[c.GetId()] = c;

                // so we need to add to index only those cards that have set - available for player
                std::pair<MSUINT::iterator, bool> mi = Index.insert(PAIRMSUINT((char*)c.GetName(),c.GetId()));
                if ((!mi.second) && (!CDB[mi.first->second].GetSet())) {
                    mi.first->second = c.GetId();
                }
                return true;
            }
            return false;
        }
        bool CardDB::CreateDeck(const char *CardsList, ActiveDeck &D)
        {
            char buffer[4096];
            strcpy_s(buffer,4096,CardsList);
            size_t len = strlen(buffer);
            size_t st = 0, cnt = 0;
            for (size_t i = 0; i < len+1; i++)
            {
                if ((buffer[i] == '"') || (buffer[i] == ',') || (!buffer[i]))
                {
                    if (    (buffer[i] == ',')
                         && (strEqualIgnoreCase(&buffer[st],CARD_NAME_SYNTAX_EXCLUSION))
                       ) {
                        continue;
                    }
                    // tokenize
                    buffer[i] = 0;
                    if (st != i)
                    {
                        // token
                        FormatCardName(&buffer[st]);
                        const Card *c = GetCardSmart(&buffer[st]);
                        if (c)
                        {
                            if (!cnt)
                                D.Commander = c;
                            else
                                D.Deck.push_back(c);
                            cnt++;
                        }
                    }
                    st = i+1;
                }
            }
            D.pCDB = this->GetPointer();
            return cnt > 0;
        }
        char *CardDB::trim(char *str, char c) const
        {
            size_t len = strlen(str);
            int beg=0;
            while(str[len-1]==c) str[--len]=0;
            while(str[beg]==c) str[beg++]=0;
            if(beg) memmove(str, str+beg, len-beg+1);
            return str;
        }
        bool CardDB::SaveCustomDecks(const char *FileName) { return SaveIndex(FileName,DIndex); }
        bool CardDB::SaveMissionDecks(const char *FileName)
        {
            FILE *f;
            if (!fopen_s(&f,FileName,"w"))
            {
                for (UINT i=0;i<MISSION_MAX_ID;i++)
                    if (MDB[i].GetCardCount())
                    {
                        fprintf_s(f,"%s:",MDB[i].GetName().c_str());
                        fprintf_s(f,"%s",CDB[MDB[i].GetCommander()].GetName());
                        for (UINT k=0;k<MDB[i].GetCardCount();k++)
                        {
                            fprintf_s(f,",%s",CDB[MDB[i].GetCardID(k)].GetName());
                        }
                        fprintf_s(f,"\n");
                    }
                return (fclose(f) == 0);
            }
            return false;
        }
        bool CardDB::ClearDeckIndex()
        {
            DIndex.clear();
            return DIndex.empty();
        }
        int CardDB::LoadDecks(const char *FileName, bool bRewrite)
        {
            FILE *f;
            int errline = -1, cline = 0; // no errors
            Tag tag = TAG_CUSTOM;
            if (!fopen_s(&f,FileName,"r"))
            {
                // detect Tag
                UINT z=0,x=0;
                for (UINT i=0;FileName[i];i++)
                    if (FileName[i] == '\\')
                    {
                        x = z;
                        z = i;
                    }
                if (x || z)
                {
                    char buffer[50];
                    memcpy(buffer,FileName+x+1,z-x-1);
                    buffer[z-x-1] = 0;

                    if (strEqualIgnoreCase(buffer,"CUSTOM"))
                        tag = TAG_CUSTOM;
                    else
                        if (strEqualIgnoreCase(buffer,"MISSION"))
                            tag = TAG_SOMEMISSION;
                        else
                            if (strEqualIgnoreCase(buffer,"RAID"))
                                tag = TAG_SOMERAID;
                            else
                                if (strEqualIgnoreCase(buffer,"BATCHEVAL"))
                                    tag = TAG_BATCHEVAL;
                }
                //
                if (bRewrite)
                {
                    DIndex.clear();
                }
    #define MAX_BUFFER_SIZE	65535
                char buffer[MAX_BUFFER_SIZE];
                while (!feof(f))
                {
                    bool bReadLine = false;
                    fgets(buffer,MAX_BUFFER_SIZE,f);
                    for (UINT i=0;(i<MAX_BUFFER_SIZE)&&(buffer[i]);i++)
                        if (buffer[i] == '\n')
                        {
                            buffer[i] = 0;
                            break;
                        }
                        else
                            if (!isspace(buffer[i]))
                                bReadLine = true;
                    if ((buffer[0] != '/') && (buffer[1] != '/') && bReadLine)
                        if (!InsertDeck(tag,buffer))
                        {
                            errline = cline;
                            break;
                        }
                    cline++;
                }
                fclose(f);
                return errline;
            }
            return -2; // fnf
        }
        int CardDB::LoadOwnedCards(const char *FileName, MUUINT &OwnedCards)
        {
            FILE *f;
            int errline = -1, cline = 0; // no errors
            if (!fopen_s(&f,FileName,"r"))
            {
    #define MAX_BUFFER_SIZE	65535
                char buffer[MAX_BUFFER_SIZE];
                while (!feof(f))
                {
                    bool bReadLine = false;
                    size_t brs = 0, bre = 0;
                    fgets(buffer,MAX_BUFFER_SIZE,f);
                    for (UINT i=0;(i<MAX_BUFFER_SIZE)&&(buffer[i]);i++)
                        if (buffer[i] == '\n')
                        {
                            buffer[i] = 0;
                            break;
                        }
                        else
                            if (!isspace(buffer[i]))
                            {
                                bReadLine = true;
                                if (buffer[i] == '(')
                                    brs = i;
                                else
                                    if (buffer[i] == ')')
                                        bre = i;
                            }
                    if (!bReadLine)
                        continue;
                    UINT amount = 1;
                    if ((brs > 0) && (bre > 0))
                    {
                        buffer[brs] = 0;
                        buffer[bre] = 0;
                        trim(buffer + brs + 1);
                        amount = atoi(buffer + brs + 1);
                        memset(buffer + brs,' ',bre - brs + 1);
                    }
                    trim(buffer);
                    if ((buffer[0] != '/') && (buffer[1] != '/') && bReadLine)
                    {
                        const Card *c = GetCardSmart(buffer);
                        if ((!c) || (!c->IsCard()))
                            errline = cline;
                        else
                            OwnedCards.insert(PAIRMUUINT(c->GetId(),amount));
                    }
                    cline++;
                }
                fclose(f);
                return errline;
            }
            return -2; // fnf
        }
        int CardDB::LoadCardList(const char *FileName, SCID &CardPool) const
        {
            FILE *f;
            int errline = -1, cline = 0; // no errors
            if (!fopen_s(&f,FileName,"r"))
            {
    #define MAX_BUFFER_SIZE	65535
                char buffer[MAX_BUFFER_SIZE];
                while (!feof(f))
                {
                    bool bReadLine = false;
                    fgets(buffer,MAX_BUFFER_SIZE,f);
                    for (UINT i=0;(i<MAX_BUFFER_SIZE)&&(buffer[i]);i++)
                        if (buffer[i] == '\n')
                        {
                            buffer[i] = 0;
                            break;
                        }
                        else
                            if (!isspace(buffer[i]))
                                bReadLine = true;
                    trim(buffer);
                    if ((buffer[0] != '/') && (buffer[1] != '/') && bReadLine)
                    {
                        const Card *c = GetCardSmart(buffer);
                        if ((!c) || (!c->IsCard()))
                            errline = cline;
                        else
                            CardPool.insert(c->GetId());
                    }
                    cline++;
                }
                fclose(f);
                return errline;
            }
            return -2; // fnf
        }
        bool CardDB::InsertDeck(Tag tag, const char *List, char *output_id_buffer)
        {
            VSTRINGS cardlist;
            MDECKS *Into = &DIndex;
            MDECKS::iterator mi = Into->end();
            char buffer[4*CARD_NAME_MAX_LENGTH]; // somehow it's corrupted often, overflows :(
            size_t len = strlen(List);
            size_t p = 0,brs = 0,cnt = 0;
            size_t decknameend = 0;
            if (output_id_buffer)
                output_id_buffer[0] = 0;
            for (size_t i=0;i<len+1;i++)
                if ((List[i] == ',') || ((List[i] == ':') && (!p)) || (!List[i]))
                {
                    if (    (List[i] == ',')
                         && (strEqualIgnoreCase(List+p,CARD_NAME_SYNTAX_EXCLUSION))
                       ) {
                        continue;
                    }
                    if (!p)
                    {
                        if (List[i] == ':')
                        {
                            decknameend = i;
                            continue;
                        }
                        else
                            if (decknameend > 0)
                            {
                                memcpy(buffer,List,decknameend);
                                buffer[decknameend] = 0; // finalize string
                                FormatCardName(buffer);
                                if (output_id_buffer)
                                {
                                    // we dont need deck name here
                                }
                                else
                                {
                                    // detect IDs in square brackets [ ]
                                    int z=-1,x=-1;
                                    for (UINT v=0;buffer[v];v++)
                                        if (buffer[v] == '[')
                                            z = v;
                                        else
                                            if (buffer[v] == ']')
                                                x = v;
                                    Tag NewTag = TAG_CUSTOM;
                                    if ((z >= 0) && (x >= 0))
                                    {
                                        buffer[z]=0;
                                        buffer[x]=0;
                                        NewTag = intToTag(atoi(trim(buffer + z + 1)));
                                        strcat(buffer,trim(buffer+x+1)); // remove [id] from deck name
                                    }
                                    if (tag == TAG_SOMERAID)
                                    {
                                        if (NewTag)
                                        {
                                            tag = intToTag(-NewTag); // negative tags for raids
                                            // P: WTF?
                                        }
                                        else
                                            if (!RIIndex.empty())
                                            {
                                                // try to detect by name
                                                for (MSUINT::iterator ri = RIIndex.begin(); ri != RIIndex.end(); ri++)
                                                {
                                                    // full raid name
                                                    const char *ptr = ri->first.c_str();
                                                    if (strstr(buffer,ptr))
                                                    {
                                                        NewTag = intToTag(ri->second); // found
                                                        tag = intToTag(-NewTag);
                                                        break;
                                                    }
                                                    // abbrevation
                                                    char abv[10];
                                                    UCHAR x=0;
                                                    bool bStore = true;
                                                    for (UINT z=0;ptr[z];z++)
                                                        if (isspace(ptr[z]))
                                                            bStore = true;
                                                        else
                                                            if (bStore)
                                                            {
                                                                abv[x] = ptr[z];
                                                                x++;
                                                                bStore = false;
                                                            }
                                                    if (strEqualIgnoreCase(ptr,"Blightbloom"))
                                                    {
                                                        abv[0] = 'B';
                                                        abv[1] = 'B';
                                                        x = 2;
                                                    }
                                                    if (x < 2) // it is pointless to search 1 char abbrevation
                                                        continue;
                                                    abv[x] = 0;
                                                    const char *f = strstr(buffer,abv);
                                                    while (f)
                                                    {
                                                        const char c = f[x];
                                                        if (((f == buffer) || (isspace(*((unsigned char*)f-1)))) && ((!c) || (isspace(c))))
                                                        {
                                                            NewTag = intToTag(ri->second); // found
                                                            break;
                                                        }
                                                        f = strstr(abv,ptr);
                                                    }
                                                    if (NewTag)
                                                    {
                                                        tag = intToTag(-NewTag);
                                                        break;
                                                    }
                                                }
                                            }
                                    }
                                    else
                                        if (tag == TAG_SOMEMISSION)
                                        {
                                            if (NewTag)
                                            {
                                                tag = NewTag; // positive tags for missions
                                            }
                                        }
                                    mi = Into->insert(PAIRMDECKS(DeckIndex(trim(buffer),tag),cardlist)).first;
                                    mi->second.clear();
                                }
                                p = decknameend+1;
                            }
                    }
                    memcpy(buffer,List+p*sizeof(char),i-p);
                    buffer[i-p] = 0; // finalize string
                    if (brs > 0)
                    {
                        for (size_t k = brs - p; k < i - p; k++)
                        {
                            if ((buffer[k] == '(') || (buffer[k] == ')') || isdigit(buffer[k]))
                            {
                                buffer[k] = ' ';
                            }
                        }
                        brs = 0;
                    }
                    if ((output_id_buffer) || (mi != Into->end()))
                    {
                        // fix buffer
                        for (UCHAR z=0;buffer[z];z++)
                            if (buffer[z] == '') // this char is so decieving ;(
                                buffer[z] = '\'';
                        FormatCardName(buffer);
                        do
                        {
                            if (!output_id_buffer)
                                mi->second.push_back(trim(buffer));
                            else
                            {
                                const Card * c = GetCardSmart(trim(buffer));
                                if (c)
                                {
                                    char itoabuffer[10];
                                    _itoa_s(c->GetId(),itoabuffer,10);
                                    if (output_id_buffer[0])
                                        strcat(output_id_buffer,",");
                                    strcat(output_id_buffer,itoabuffer);
                                }
                            }
                            if (cnt)
                                cnt--;
                        }
                        while (cnt > 0);
                        cnt = 0;
                    }
                    p = i + 1;
                }
                else
                    if (List[i] == '(')
                        brs = i;
                    else
                        if ((List[i] == ')') && (brs > 0))
                        {
                            memcpy(buffer,List+(brs+1)*sizeof(char),i-brs-1);
                            buffer[i-brs-1] = 0;
                            cnt = atoi(buffer);
                        }
            return (!output_id_buffer) || (output_id_buffer[0]);
        }
        void CardDB::GetAchievementList(char *Buffer, DWORD MaxBufferSize)
        {
            Buffer[0]=0;
            for (UINT i=0;i<ACHIEVEMENT_MAX_COUNT;i++)
            {
                if (ADB[i].IsValid())
                {
                    if (i > 0)
                        strcat_s(Buffer,MaxBufferSize,",");
                    strcat_s(Buffer,MaxBufferSize,"\"");
                    strcat_s(Buffer,MaxBufferSize,ADB[i].GetName());
                    strcat_s(Buffer,MaxBufferSize,"\"");
                }
            }
        }
        const char *CardDB::GetAchievementDescription(DWORD AchievementIndex)
        {
            if ((AchievementIndex < ACHIEVEMENT_MAX_COUNT) && (ADB[AchievementIndex].IsValid()))
                return ADB[AchievementIndex].GetDescription();
            else
                return 0;
        }
        const UINT CardDB::GetAchievementID(DWORD AchievementIndex)
        {
            if ((AchievementIndex < ACHIEVEMENT_MAX_COUNT) && (ADB[AchievementIndex].IsValid()))
                return ADB[AchievementIndex].GetID();
            else
                return 0;
        }
        bool CardDB::CheckAchievementMission(UINT AchievementIndex, UINT MissionID)
        {
            if ((AchievementIndex < ACHIEVEMENT_MAX_COUNT) && (ADB[AchievementIndex].IsValid()))
                return ADB[AchievementIndex].CheckMissionID(MissionID);
            else
                return 0;
        }
        void CardDB::Print()
        {
            for (UINT i=0;i<CARD_MAX_ID;i++)
                if (CDB[i].IsCard())
                //if (it->second.GetName()[0]=='E')
                //	break;
                //else
                printf("%s\n",CDB[i].GetName());
    #define MAX_ID_LEN	8
            char buffer[MAX_ID_LEN * DEFAULT_DECK_RESERVE_SIZE]; // buffer for ids
            buffer[0] = 0;
            for (MSUINT::iterator it=MIIndex.begin();it!=MIIndex.end();it++)
            {
                std::string str;
                str = MDB[it->second].GetDeck();
                strncat(buffer, str.c_str(), MAX_ID_LEN * DEFAULT_DECK_RESERVE_SIZE);
                printf("%s:%s\n",it->first.c_str(),buffer);
            }
            for (MDECKS::iterator it=DIndex.begin();it!=DIndex.end();it++)
            {
                printf("%s:",it->first.name.c_str());
                for (VSTRINGS::iterator et=it->second.begin();et!=it->second.end();et++)
                {
                    if (et!=it->second.begin())
                        printf(",");
                    printf("%s",et->c_str());
                }
                printf("\n");
            }
        }
        const Card *CardDB::CARD(const char *Name)
        {
            MSUINT::iterator it=Index.find(Name);
            if (it != Index.end())
                return &CDB[it->second];
            else
                return 0;
        }
        const Card *CardDB::GetCard(std::string Name) const
        {
            return GetCard(Name.c_str());
        }
        const Card *CardDB::GetCard(const char *Name) const
        {
            MSUINT::const_iterator it=Index.find(Name);
            if (it != Index.end())
                return &CDB[it->second];
            else
            {
                printf("Can't find %s in database\n",Name);
                return 0;
            }
        }
        /*const int GetCardIndex(const char *Name)
        {
            MSUINT::iterator it=Index.find(Name);
            if (it != Index.end())
                return std::distance(Index.begin(), it);
            else
                return -1;
        }*/
        const Card &CardDB::GetCard(const UINT Id) const
        {
            if (Id > CARD_MAX_ID)
                throw 0;

            return CDB[Id];
        }
        const Card *CardDB::GetCardSmart(const char *Name) const
        {
            const char* i=strchr(Name,'[');
            if (!i)
                return GetCard(Name);
            const char* k=strchr(i,']');
            if (i && k)
            {
                char bu[10];
                memcpy(bu,i+1,k-i-1);
                bu[k-i-1] = 0;
                return &GetCard(atoi(bu));
            }
            return GetCard(Name);
        }
        const bool CardDB::GetCardListSorted(char *buffer, DWORD size)
        {
            std::string s;
            char lbuff[50];
            std::set<std::string> ss;
            for (UINT i=0;i<CARD_MAX_ID;i++)
                if (CDB[i].IsCard())
                {
                    s = CDB[i].GetName();
                    if (!CDB[i].GetSet())
                    {
                        sprintf(lbuff,"[%d]",i);
                        s += lbuff;
                    }
                    ss.insert(s);
                }
            s.clear();
            for (std::set<std::string>::iterator si = ss.begin(); si != ss.end(); si++)
            {
                if (!s.empty())
                    s += ',';
                s += '"' + *si + '"';
            }
            if (s.length() > size)
                return false;
            strcpy(buffer,s.c_str());
            return (!s.empty());
        }
        const char* CardDB::GetCustomDecksList(char *buffer, size_t size, Tag byTag)
        {
            buffer[0] = 0;
            for (MDECKS::iterator mi = DIndex.begin(); mi != DIndex.end(); mi++)
            {
                if ((byTag != TAG_ANY) && (byTag != mi->first.tag) && (byTag || (mi->first.tag != TAG_BATCHEVAL))) // skip
                    continue;
                if (buffer[0])
                    strcat_s(buffer,size,","); // commatext :)
                strcat_s(buffer,size,"\"");
                strcat_s(buffer,size,mi->first.name.c_str());
                strcat_s(buffer,size,"\""); // yep, it's bad, but i dont care since it's called from delphi
            }
            return buffer;
        }
        const char* CardDB::GetMissionDecksList(char *buffer, size_t size, bool bSortById)
        {
            buffer[0] = 0;
            if (bSortById)
                for (UINT id=0;id<MISSION_MAX_ID;id++)
                {
                    if (MDB[id].IsValid())
                    {
                        if (buffer[0] != 0)
                            strcat_s(buffer,size,","); // commatext :)
                        strcat_s(buffer,size,"\"");
                        strcat_s(buffer,size,MDB[id].GetName().c_str());
                        strcat_s(buffer,size,"\""); // yep, it's bad, but i dont care since it's called from delphi
                    }
                }
            else
                for (MSUINT::iterator mi = MIIndex.begin(); mi != MIIndex.end(); mi++)
                {
                    if (mi != MIIndex.begin())
                        strcat_s(buffer,size,","); // commatext :)
                    strcat_s(buffer,size,"\"");
                    strcat_s(buffer,size,mi->first.c_str());
                    strcat_s(buffer,size,"\""); // yep, it's bad, but i dont care since it's called from delphi
                }
            return buffer;
        }
        const char* CardDB::GetCustomDeck(const char* DeckName, Tag const tag, char *buffer, size_t size)
        {
            buffer[0] = 0;
            MDECKS::iterator mi = DIndex.find(DeckIndex(DeckName,tag));
            if ((!tag) && (mi == DIndex.end()))
                 mi = DIndex.find(DeckIndex(DeckName,TAG_BATCHEVAL));
            if (mi == DIndex.end())
                return buffer; // not found
            if (mi->second.empty())
                return buffer; // empty deck :(
            for (VSTRINGS::iterator vi = mi->second.begin();vi != mi->second.end();vi++)
            {
                if (vi != mi->second.begin())
                    strcat_s(buffer,size,","); // commatext :)
                strcat_s(buffer,size,"\"");
                strcat_s(buffer,size,(*vi).c_str());
                strcat_s(buffer,size,"\""); // yep, it's bad, but i dont care since it's called from delphi
            }
            return buffer;
        }
        const char* CardDB::GetMissionDeck(const char* DeckName, char *buffer, size_t size)
        {
            buffer[0] = 0;
            MSUINT::iterator mi = MIIndex.find(DeckName);
            if (mi == MIIndex.end()) {
                return buffer; // not found
            }

            // TODO untested since I don't think we use this version of GetMissionDeck anymore
            std::stringstream ss;
            ss << MDB[mi->second].GetCommander();
            ss >> buffer;

            std::string str;
            str = MDB[mi->second].GetDeck();
            strncat(buffer, str.c_str(), size);
            return buffer;
        }
        // named decks
        ActiveDeck CardDB::GetMissionDeck(const UINT missionId) const
        {
            assertGE(MISSION_MAX_ID, missionId);
            assertX(MDB[missionId].GetCardCount() > 0);

            ActiveDeck r(&CDB[MDB[missionId].GetCommander()], this->CDB);
            for (UINT k=0;k<MDB[missionId].GetCardCount();k++)
            {
                r.Add(&CDB[MDB[missionId].GetCardID(k)]);
            }
            return r;
        }
        const UINT CardDB::GetMissionDeckIndex(const char* DeckName)
        {
            MSUINT::iterator mi = MIIndex.find(DeckName);
            if (mi == MIIndex.end())
                return 0; // not found
            return mi->second;
        }
        const char* CardDB::GetRaidDecksList(char *buffer, size_t size)
        {
            buffer[0] = 0;
            for (UCHAR i=0;i<RAID_MAX_ID;i++)
                if (RDB[i].IsValid())
                {
                    if (buffer[0] != 0)
                        strcat_s(buffer,size,","); // commatext :)
                    strcat_s(buffer,size,"\"");
                    strcat_s(buffer,size,RDB[i].GetName());
                    strcat_s(buffer,size,"\""); // yep, it's bad, but i dont care since it's called from delphi
                }
            return buffer;
        }
        const char* CardDB::GetQuestDecksList(char *buffer, size_t size)
        {
            buffer[0] = 0;
            for (UCHAR i=0;i<STEP_MAX_ID;i++)
                if (STDB[i].IsValid())
                {
                    if (buffer[0] != 0)
                        strcat_s(buffer,size,","); // commatext :)
                    strcat_s(buffer,size,"\"");
                    strcat_s(buffer,size,BGDB[STDB[i].GetBGId()].GetName().c_str());
                    char bu[5];
                    strcat_s(buffer,size,"(");
                    itoa(i,bu,10);
                    strcat_s(buffer,size,bu);
                    strcat_s(buffer,size,") - ");
                    strcat_s(buffer,size,CDB[STDB[i].GetCommander()].GetName());
                    strcat_s(buffer,size,"[");
                    itoa(STDB[i].GetCommander(),bu,10);
                    strcat_s(buffer,size,bu);
                    strcat_s(buffer,size,"]\""); // yep, it's bad, but i dont care since it's called from delphi
                }
            return buffer;
        }
        const UINT CardDB::GetRaidCommanderID(UINT RaidIndex)
        {
            return RDB[RaidIndex].GetCommander();
        }
        const UINT CardDB::GetQuestCommanderID(UINT QuestIndex)
        {
            return STDB[QuestIndex].GetCommander();
        }
        void CardDB::GenRandomDeckFromCard(VCARDS &Deck,size_t i)
        {
            MSUINT::iterator mi = Index.begin();
            for (; mi != Index.end(); mi++)
            {
                if ((!i) && (CDB[mi->second].GetRarity() < RARITY_UNIQUE))
                    break;
                else
                    i--;
            }
            if (mi != Index.end())
                for (UCHAR k=0;k<DEFAULT_DECK_SIZE;k++)
                    Deck.push_back(&CDB[mi->second]);
        }
        const UINT CardDB::GetCustomCount()
        {
            UINT c = 0;
            for (MDECKS::iterator mi = DIndex.begin(); mi != DIndex.end(); mi++)
                //if (mi->first.Tag == TAG_CUSTOM) GET ANY COUNT
                    c++;
            return c;
        }
        bool CardDB::GetCustomDeck(UINT Index, ActiveDeck &R)
        {
            for (MDECKS::iterator mi = DIndex.begin(); mi != DIndex.end(); mi++)
                //if (mi->first.Tag == TAG_CUSTOM) GET ANY DECK
                {
                    if (!Index)
                    {
                        if (mi->second.empty())
                            return false;
                        VSTRINGS::iterator vi = mi->second.begin();
                        R.Commander = GetCardSmart(vi->c_str());
                        R.Deck.clear();
                        for (vi++;vi != mi->second.end();vi++)
                            R.Add(GetCardSmart(vi->c_str()));
                        return true;
                    }
                    Index--;
                }
            return false;
        }
        bool CardDB::CheckAchievement(int achievementId, const UINT iTurn, ActiveDeck &Atk, ActiveDeck &Def)
        {
            if (achievementId < 0) return true;

            AchievementInfo achievementInfo;
            for (int i = 0; i < ACHIEVEMENT_MAX_COUNT; i ++)
            {
                if(ADB[i].GetID() == achievementId) {
                    achievementInfo = ADB[i];
                    break;
                }
            }

            for (std::vector<AchievementRequirement>::iterator vi = achievementInfo.Reqs.begin(); vi != achievementInfo.Reqs.end(); vi++)
            {
                UINT cnt = 0;
                UINT rcnt = 0;
                // attack damage
                if (vi->Damage >= 0)
                {
                    cnt = Atk.StrongestAttack;
                    rcnt = vi->Damage;
                }
                // damage to commander
                if (vi->ComTotal >= 0)
                {
                    cnt = Atk.FullDamageToCommander;
                    rcnt = vi->ComTotal;
                }
                // skill procs
                if ((vi->SkillID >= 0) || (vi->NumUsed >= 0))
                {
                    if (vi->NumUsed >= 0)
                    {
                        cnt = Atk.SkillProcs[vi->SkillID];
                        rcnt = vi->NumUsed;
                    }
                    if (vi->NumKilledWith >= 0)
                    {
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (Def.CardDeaths[i])
                            {
                                if (CDB[Def.CardDeaths[i]].GetAbility((UCHAR)vi->SkillID)) cnt++;
                            }
                            else break;
                        rcnt = vi->NumKilledWith;
                    }
                }
                // turns
                if (vi->NumTurns >= 0)
                {
                    cnt = iTurn;
                    rcnt = vi->NumTurns;
                }
                // cards
                if (vi->UnitID >= 0)
                {
                    if (vi->NumPlayed >= 0)
                    {
                        rcnt = vi->NumPlayed;
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (Atk.CardPicks[i])
                            {
                                // In a quite unlikely expansion a card ID of MAX_UINT - 1 could be used,in that case the previous version of the check would fail
                                if (vi->UnitID >= 0 && Atk.CardPicks[i] == (UINT)vi->UnitID) cnt++;
                            }
                            else break;
                        if (vi->UnitID >= 0 && Atk.Commander.GetId() == (UINT)vi->UnitID) cnt++;
                    }
                    if (vi->NumKilled >= 0)
                    {
                        rcnt = vi->NumKilled;
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (Def.CardDeaths[i])
                            {
                                if (vi->UnitID >= 0 && Def.CardDeaths[i] == (UINT)vi->UnitID) cnt++;
                            }
                            else break;
                    }
                }
                // cards of a type
                if (vi->UnitType >= 0)
                {
                    if (vi->NumPlayed >= 0)
                    {
                        rcnt = vi->NumPlayed;
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (Atk.CardPicks[i])
                            {
                                if (CDB[Atk.CardPicks[i]].GetType() == vi->UnitType) cnt++;
                            }
                            else break;
                    }
                    if (vi->NumKilled >= 0)
                    {
                        rcnt = vi->NumKilled;
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (Def.CardDeaths[i])
                            {
                                if (CDB[Def.CardDeaths[i]].GetType() == vi->UnitType) cnt++;
                            }
                            else break;
                    }
                }
                // cards of a rarity
                if (vi->UnitRarity >= 0)
                {
                    if (vi->NumPlayed >= 0)
                    {
                        rcnt = vi->NumPlayed;
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (Atk.CardPicks[i])
                            {
                                if (CDB[Atk.CardPicks[i]].GetRarity() == vi->UnitRarity) cnt++;
                            }
                            else break;
                    }
                    if (vi->NumKilled >= 0)
                    {
                        rcnt = vi->NumKilled;
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (Def.CardDeaths[i])
                            {
                                if (CDB[Def.CardDeaths[i]].GetRarity() == vi->UnitRarity) cnt++;
                            }
                            else break;
                    }
                }
                // cards of a faction
                if (vi->UnitRace >= 0)
                {
                    if (vi->NumPlayed >= 0)
                    {
                        rcnt = vi->NumPlayed;
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (Atk.CardPicks[i])
                            {
                                if (CDB[Atk.CardPicks[i]].GetFaction() == vi->UnitRace) cnt++;
                            }
                            else break;
                    }
                    if (vi->NumKilled >= 0)
                    {
                        rcnt = vi->NumKilled;
                        for (UCHAR i=0;i<DEFAULT_DECK_RESERVE_SIZE;i++)
                            if (Def.CardDeaths[i])
                            {
                                if (CDB[Def.CardDeaths[i]].GetFaction() == vi->UnitRace) cnt++;
                            }
                            else break;
                    }
                }
                // checks
                if ((vi->Compare == UNDEFINED) && (cnt < rcnt)) return false;
                if ((vi->Compare == EQUAL) && (cnt != rcnt)) return false;
                if ((vi->Compare == GREATEREQUAL) && (cnt < rcnt)) return false;
                if ((vi->Compare == GREATER) && (cnt <= rcnt)) return false;
                if ((vi->Compare == LESSEQUAL) && (cnt > rcnt)) return false;
                if ((vi->Compare == LESS) && (cnt >= rcnt)) return false;
            }
            return true;
        }
        // named decks
        ActiveDeck CardDB::GetNamedDeck(const char* DeckName, Tag tag)
        {
            MDECKS::iterator mi = DIndex.find(DeckIndex(DeckName,tag));
            if (mi == DIndex.end())	{
                printf("Can't find deck %s in database\n",DeckName);
            }
            assertX(mi != DIndex.end());
            if (mi->second.empty()) {
                printf("Deck %s has 0 cards\n",DeckName);
            }
            assertX(!mi->second.empty());
            VSTRINGS::iterator vi = mi->second.begin();
            ActiveDeck r(GetCard(*vi), this->CDB);
            for (vi++;vi != mi->second.end();vi++) {
                r.Add(GetCard(*vi));
            }
            return r;
        }

    }
}

#ifndef ITERATEDECKS_CORE_ACTIVEDECK_HPP
    #define ITERATEDECKS_CORE_ACTIVEDECK_HPP

    #include "playedCard.hpp"
    #include "results.hpp"
    #include "logger.forward.hpp"

    namespace IterateDecks {
        namespace Core {

            class ActiveDeck {
                public:
                    PlayedCard Commander;
                    LCARDS Deck;
                    LCARDS Units;
                    LCARDS Structures;
                    LCARDS Actions;
                    //
                    bool bOrderMatters;
                    MSID Hand;
                    bool bDelayFirstCard; // tournament-like
                    //
                    const UCHAR *CSIndex;
                    RESULT_BY_CARD *CSResult;
                    /**
                     *  Each deck counts the number of times a skill is proced. This buffer will be provided to every PlayedCard in this deck.
                     */
                    UCHAR SkillProcs[CARD_ABILITIES_MAX];
                    //
                    UINT CardPicks[DEFAULT_DECK_RESERVE_SIZE];
                    UINT CardDeaths[DEFAULT_DECK_RESERVE_SIZE];
                    //
                    UINT DamageToCommander; // for points calculation - damage dealt to ENEMY commander
                    UINT FullDamageToCommander;
                    UINT StrongestAttack;
                    // logging related stuff
                    UCHAR LogDeckID;
                    VLOG *Log;
                    // Quest effects
                    BattleGroundEffect QuestEffectId;
                    // used to get the actual card for summon
                    Card const * pCDB;
                    // logging
                    DeckLogger * logger;
                public:
                    PlayedCard & getUnitAt(unsigned int const index);
                    PlayedCard & getActionAt(unsigned int const index);
                    PlayedCard & getStructureAt(unsigned int const index);
                    PlayedCard & getCardAt(unsigned int const index);
                    PlayedCard const & getCardAt(unsigned int const index) const;
                private:
                    void Reserve();
                    LOG_RECORD* LogAdd(LOG_CARD src, UCHAR AbilityID, UCHAR Effect = ABILITY_ENABLED);
                    LOG_RECORD* LogAdd(LOG_CARD src, LOG_CARD dest, UCHAR AbilityID, UCHAR Effect = ABILITY_ENABLED);
                    UCHAR GetAliveUnitsCount();
                    void AttackCommanderOnce(UCHAR const & index
                                            ,PlayedCard & src
                                            ,EFFECT_ARGUMENT const & valor
                                            ,ActiveDeck & Def
                                            ,bool const variant1
                                            );

                    void AttackCommanderOnce1(UCHAR const & index
                                             ,PlayedCard & SRC
                                             ,EFFECT_ARGUMENT const & valor
                                             ,ActiveDeck & Def
                                             );

                    void AttackCommanderOnce2(UCHAR const & index
                                             ,PlayedCard & SRC
                                             ,EFFECT_ARGUMENT const & valor
                                             ,ActiveDeck & Def
                                             );

                    /**
                     * @returns true iff we should continue in the swipe loop
                     */
                    bool AttackUnitOrCommanderOnce2(PlayedCard & SRC
                                                   ,UCHAR const & index
                                                   ,PlayedCard & target
                                                   ,UCHAR const & targetindex
                                                   ,UCHAR const & swipe
                                                   ,UCHAR const & s
                                                   ,UCHAR & iSwiped
                                                   ,ActiveDeck & Def
                                                   );
                    void Attack(UINT index, ActiveDeck &Def);
                public:
                    ActiveDeck();
                    ~ActiveDeck();
                public:
                #define REQ_MAX_SIZE			5
                    bool CheckRequirements(const REQUIREMENT *Reqs);
                    bool IsAnnihilated();
                    void SetFancyStatsBuffer(const UCHAR *resindex, RESULT_BY_CARD *res);
                    UCHAR GetCountInDeck(UINT Id);
                    void SetQuestEffect(BattleGroundEffect EffectId);
                    // NR: please note, contructors don't clean up storages, must do it manually and beforehand, even copy constructor
                    // P: I have no idea what he means by that
                    ActiveDeck(const char *HashBase64, const Card *pCDB);
                    ActiveDeck(const Card *Cmd, Card const * const pCDB);
                    ActiveDeck(const ActiveDeck &D);
                    bool operator==(const ActiveDeck &D) const;
                    bool operator<(const ActiveDeck &D) const;
                    const bool IsValid(bool bSoftCheck = false) const;
                    void SetOrderMatters(const bool bMatters);
                    void DelayFirstCard();
                    void Add(const Card *c);
                    bool IsInTargets(PlayedCard *pc, PPCIV *targets);
                    static bool Evade(PlayedCard *defender, BattleGroundEffect QuestEffectId, bool chaos);
                    UCHAR Intercept(PPCIV &targets, UCHAR destindex, ActiveDeck &Dest);
                    static bool Payback(ActiveDeck &targetDeck, PlayedCard *target, PlayedCard &attacker, EVENT_CONDITION EffectType, AbilityId effectId, EFFECT_ARGUMENT effect, bool chaos);
                    bool Tribute(PlayedCard *tributeCard, PlayedCard *targetCard, ActiveDeck *procDeck, EVENT_CONDITION EffectType, AbilityId effectId, EFFECT_ARGUMENT effect);
                    void PlayCard(const Card *c, ActiveDeck &Def);
                    void ApplyDamageDependentEffects(BattleGroundEffect QuestEffectId,PlayedCard &attacker,ActiveDeck &defenseDeck,PlayedCard &defender,UCHAR dmg);
                    void ApplyDefensiveEffects(BattleGroundEffect QuestEffectId,PlayedCard &attacker,ActiveDeck &defenseDeck,PlayedCard &defender,UCHAR dmg);
                    void ApplyEffects(BattleGroundEffect QuestEffectId,EVENT_CONDITION EffectType, PlayedCard &Src,int Position,ActiveDeck &Dest,bool IsMimiced=false,bool IsFusioned=false,PlayedCard *Mimicer=0,UCHAR StructureIndex = 0, PlayedCard * target=NULL);
                    void applyDamageDependentEffectOnAttack(BattleGroundEffect questEffectId, PlayedCard & src, AbilityId const & abilityId, EFFECT_ARGUMENT const & effectArgument, ActiveDeck & otherDeck, PlayedCard & target);
                    void SweepFancyStats(PlayedCard &pc);
                    void SweepFancyStatsRemaining();
                    const Card *PickNextCard(bool bNormalPick = true);
                    void CheckDeathEvents(PlayedCard &src, ActiveDeck &Def);
                    void AttackDeck(ActiveDeck &Def, bool bSkipCardPicks, unsigned int turn);
                    void PrintShort();
                    //void appendCard(std::stringstream * os,PlayedCard const & card,unsigned int const w);
                    //void appendCards(std::stringstream * os, LCARDS const & cards, unsigned int const w);
                    //void concatStreams(std::stringstream & os, std::stringstream const * const oss);
                    //void appendCards(std::stringstream & os, LCARDS const & cards, unsigned int const w);

                    /**
                     * Returns a nice string representation of the deck.
                     */
                    std::string toString(bool const & reversed = false, unsigned int const w = 20);
                    std::string GetDeck() const;
                    std::string GetHash64(bool bCardPicks = false) const;

                    bool isAlive() const;
                protected:
                    static void GetTargets(LCARDS &From, UCHAR TargetFaction, PPCIV &GetTo, bool bForInfuse = false);
                    static void FilterTargets(PPCIV &targets, const EFFECT_ARGUMENT skipEffects[], const EFFECT_ARGUMENT targetSkills[], const int waitMin, const int waitMax, const int attackLimit, bool skipPlayed);
                    void RandomizeTarget(PPCIV &targets, UCHAR targetCount, ActiveDeck &Dest, bool canIntercept);
            };

        }
    }
#endif

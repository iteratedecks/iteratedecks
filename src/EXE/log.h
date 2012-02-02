#define INVALID_ROWID	100
struct LOG_CARD
{
	UCHAR DeckID;
	UCHAR RowID;
	UCHAR CardID;
	LOG_CARD() { RowID = INVALID_ROWID; CardID = 100; };
	LOG_CARD(const UCHAR deckID, const UCHAR rowID, const UCHAR cardID) { DeckID = deckID; RowID = rowID; CardID = cardID; };
	bool IsValid() { return (RowID != INVALID_ROWID); };
};
struct LOG_RECORD
{
	LOG_CARD Src;
	LOG_CARD Target;
	UCHAR AbilityID;
	UCHAR Effect;
	LOG_RECORD();
	LOG_RECORD(LOG_CARD &src, const UCHAR abilityID, const UCHAR effect) { Src = src; AbilityID = abilityID; Effect = effect; };
	LOG_RECORD(LOG_CARD &src, LOG_CARD &target, const UCHAR abilityID, const UCHAR effect) { Src = src; Target = target; AbilityID = abilityID; Effect = effect; };
};
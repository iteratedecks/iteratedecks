#define INVALID_ROWID	100
struct LOG_CARD
{
	UCHAR DeckID;
	UCHAR RowID;
	UCHAR CardID;

	// FIXME: Is it intentional that DeckID is uninitialized?
	LOG_CARD()
	: RowID(INVALID_ROWID),
	  CardID(100)
	{};

	LOG_CARD(const UCHAR deckID, const UCHAR rowID, const UCHAR cardID) 
	: DeckID(deckID),
	  RowID(rowID),
	  CardID(cardID)
	{};

	bool IsValid() const { return (RowID != INVALID_ROWID); };
};
struct LOG_RECORD
{
	LOG_CARD Src;
	LOG_CARD Target;
	UCHAR AbilityID;
	UCHAR Effect;

	// FIXME: Is it intentional that all fields are unitialized?
	LOG_RECORD();

	// FIXME: Is it intentional that Target is uninitialized?
	LOG_RECORD(LOG_CARD src, const UCHAR abilityID, const UCHAR effect)
	: Src(src),
	  AbilityID(abilityID),
	  Effect(effect)
	{};

	LOG_RECORD(LOG_CARD src, LOG_CARD target, const UCHAR abilityID, const UCHAR effect) 
	: Src(src),
	  Target(target),
	  AbilityID(abilityID),
	  Effect(effect)
	{};
};


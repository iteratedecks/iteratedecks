unit EDImports;

interface

uses Windows;

const
  MAX_CARD_COUNT = 1200;
  MAX_DECK_SIZE = 20;   // this is only for API
  MAX_SETS_COUNT = 20;
  DEFAULT_DECK_SIZE = 10;
  DECK_MAX_SIZE = 15;   // this is for actual cards and decks and data exchange
  CARD_NAME_MAX_LENGTH = 50;
  FILENAME_MAX_LENGTH = 50;
  REQ_MAX_SIZE = 5;
  CARD_ABILITIES_MAX = 70;
  cMaxBuffer = $FFFFFF;//65535;
  cDefaultMaxTurn = 50;

type
  TCard = record
    Id: UINT;
    Name: array[0..CARD_NAME_MAX_LENGTH - 1] of char;
    Picture: array[0..FILENAME_MAX_LENGTH - 1] of char;
    CardType: byte;
    Wait: byte;
    CardSet: UINT; // 'set' is a reserved word in Delphi :(
    Faction: byte;
    Attack: byte;
    Health: byte;
    Rarity: byte;
    Effects: array[0..CARD_ABILITIES_MAX - 1] of byte;
    TargetCounts: array[0..CARD_ABILITIES_MAX - 1] of byte;
    TargetFactions: array[0..CARD_ABILITIES_MAX - 1] of byte;
    AbilityEvent: array[0..CARD_ABILITIES_MAX - 1] of byte;
    ChunkData: array[0..12] of byte; // it was 16
  end;

type
  TCardSet = record
    Name: array[0..CARD_NAME_MAX_LENGTH - 1] of char;
    Icon: array[0..CARD_NAME_MAX_LENGTH - 1] of char;
    Visible: boolean;
  end;

type
  RESULTS = record
    Win: DWORD;
    Loss: DWORD;
    Games: DWORD;
    Points: DWORD;
    AutoPoints: DWORD;
    LPoints: DWORD;
    LAutoPoints: DWORD;
  end;

type
  PICK_STATS = record
    Win: DWORD;
    Stall: DWORD;
    Loss: DWORD;
  end;

type
  RESULT_BY_CARD = record
    Id: DWORD;
    WLGames: DWORD;
	WLWin: DWORD;
	WLLoss: DWORD;
	FSRecordCount: DWORD;
	FSMitigated: DWORD;
	FSAvoided: DWORD;
	FSDamage: DWORD;
	FSHealing: DWORD;
	FSSpecial: DWORD;
    FSOverkill: DWORD;
    FSDeaths: DWORD;
    PickStats: array[0..DECK_MAX_SIZE-1] of PICK_STATS;
  end;

type
  RESULT_BY_ORDER = record
	CardOrder: array[0..((DECK_MAX_SIZE * 2)-1)] of Char;
	CardIDs: array[0..DECK_MAX_SIZE-1] of DWORD;
    result: PICK_STATS;
  end;

type
  FULLRESULT = record
    Result: RESULTS;
    ResultByCard: array[0..DECK_MAX_SIZE] of RESULT_BY_CARD;
  end;

type
  REQUIREMENT = record
    SkillID: byte;
    ProcsCount: byte;
  end;

const
  MAX_DECKSTRING_SIZE = 1024;
type
  TEvalParams = record
    Seed: DWORD;
    AtkDeck: array[0..MAX_DECKSTRING_SIZE - 1] of Char;
    DefDeck: array[0..MAX_DECKSTRING_SIZE - 1] of Char;
    GamesPerThread: DWORD;
    Threads: DWORD;
    Result: RESULTS;
    ResultByCard: array[0..DECK_MAX_SIZE] of RESULT_BY_CARD;
    Seconds: DWORD;
    RaidID: integer;
    QuestID: integer;
    Surge: boolean;
    OrderMatters: boolean;
    WildCardId: integer;
    WildFilterType: integer;
    WildFilterRarity: integer;
    WildFilterFaction: integer;
    WildFilterSpecific: boolean;
    WildCardIds: array[0..DECK_MAX_SIZE-1] of DWORD;
    WildCardWins: array[0..DECK_MAX_SIZE-1] of DWORD;
    MaxTurn: DWORD;
    State: integer;
    TournamentMode: byte;
    OrderHeapSize: DWORD;
    Reqs: array[0..REQ_MAX_SIZE -1] of REQUIREMENT;
    SkillProcs: array[0..CARD_ABILITIES_MAX-1] of DWORD;
    ResultByOrder: array[0..DECK_MAX_SIZE-1] of RESULT_BY_ORDER;
    FullAmountOfGames: DWORD;
    SkipWildcardsWeDontHave: boolean;
    Annihilator: boolean;
    SurrenderAtLoss: boolean;
    AchievementIndex: integer;
    SecondPassMultiplier: DWORD;
  end;

const
  DLLFILE = 'IterateDecksDLL.dll';

function GetCard(CardName: string): Pointer; cdecl; external DLLFILE;

function GetCardByID(Id: UINT): Pointer; cdecl; external DLLFILE;

function GetCustomDecksList(Tag: integer; buffer: PChar; size: DWORD): PChar; cdecl; external
  DLLFILE;

function GetMissionDecksList(buffer: PChar; size: DWORD; bSortByID: boolean =
  true): PChar; cdecl; external DLLFILE;

function GetCustomDeck(DeckName: string; Tag: integer; buffer: PChar; size: DWORD): PChar;
  cdecl; external DLLFILE;

function GetMissionDeck(DeckName: string; buffer: PChar; size: DWORD): PChar;
  cdecl; external DLLFILE;

function GetRaidDecksList(buffer: PChar; size: DWORD): PChar; cdecl; external
  DLLFILE;

function GetRaidCommanderID(RaidIndex: UINT): UINT; cdecl; external DLLFILE;

function GetQuestDecksList(buffer: PChar; size: DWORD): PChar; cdecl; external
  DLLFILE;

function GetQuestCommanderID(QuestIndex: UINT): UINT; cdecl; external DLLFILE;

function Evaluate(AtkDeck: string; DefDeck: string; gamescount: DWORD): DWORD;
  cdecl; external DLLFILE;

procedure PrepareDecks(AtkDeck: string; DefDeck: string); cdecl; external
  DLLFILE;

procedure PrepareDeck(AtkDeck: string); cdecl; external DLLFILE;

procedure GetSkillList(buffer: PChar; size: DWORD); cdecl; external DLLFILE;

function GetSkillID(SkillName: PChar): byte; cdecl; external DLLFILE;

function GetSkillName(Id: DWORD): PChar; cdecl; external DLLFILE;

procedure EvaluateOnce(var r: RESULTS; bSurge: boolean = false); cdecl; external
  DLLFILE;

function EvaluateRaidOnce(var r: RESULTS; RaidID: DWORD): boolean; cdecl;
  external DLLFILE;
function EvaluateQuestOnce(var r: RESULTS; QuestID: DWORD): boolean; cdecl;
  external DLLFILE;

function GetCardSize(): integer; cdecl; external DLLFILE;

function LoadCardXML(FileName: string; returnnewcards: PChar; MaxBufferSize:
  DWORD): boolean; cdecl; external DLLFILE;

function LoadMissionXML(FileName: string): boolean; cdecl; external DLLFILE;

function LoadAchievementXML(FileName: string): boolean; cdecl; external DLLFILE;

function LoadRaidXML(FileName: string): boolean; cdecl; external DLLFILE;

function LoadQuestXML(FileName: string): boolean; cdecl; external DLLFILE;

function GetDBSize(): DWORD; cdecl; external DLLFILE;

function GetSets(Indexes: Pointer; Sets: Pointer; Size: DWORD): DWORD; cdecl;
  external DLLFILE;

function LoadCustomDecks(FileName: string): integer; cdecl; external DLLFILE;
function ClearCustomDecks: boolean; cdecl; external DLLFILE;

function LoadPointers(Ptr: Pointer; MaxCount: DWORD): DWORD; cdecl; external
  DLLFILE;

function InsertCustomDeck(List: string): boolean; cdecl; external DLLFILE;
function GetCardListSorted(buffer: PChar; size: DWORD): boolean; cdecl; external DLLFILE;

procedure EvaluateInThreads(Seed: DWORD; var r: RESULTS; gamesperthread: DWORD;
  threadscount: DWORD = 1; bSurge: boolean = false); cdecl; external DLLFILE;

function AbilityHasExtendedDesc(AbilityID: Byte): boolean; cdecl; external DLLFILE;

function GetHashFromDeck(Deck: string; buffer: PChar; size: DWORD): boolean; cdecl; external DLLFILE;

function BuildResultHash(CardList: string; Version: DWORD; Revision: DWORD; HashType: DWORD; HashID: DWORD; MissionID: DWORD; GamesOverall: DWORD; GamesWon: DWORD; buffer: PChar; size: DWORD): boolean; cdecl; external DLLFILE;

function GetDeckFromHash(Hash: string; buffer: PChar; size: DWORD): boolean; cdecl; external DLLFILE;
function GetDeckFromString(CardList: string; OutputIDList: PChar): boolean; cdecl; external DLLFILE;

function GetMissionDeckIndex(DeckName: string): DWORD; cdecl; external DLLFILE;

procedure GetAchievementList(buffer: PChar; size: DWORD); cdecl; external DLLFILE;
function GetAchievementDescription(Index: DWORD): PChar; cdecl; external DLLFILE;
function GetAchievementID(Index: DWORD): DWORD; cdecl; external DLLFILE;
function CheckAchievementMission(Index: DWORD; MissionID: DWORD): boolean; cdecl; external DLLFILE;

function LoadOwnedCards(FileName: string): boolean; cdecl; external DLLFILE;
function CheckOwnedCard(Id: DWORD): DWORD; cdecl; external DLLFILE;

procedure SpeedTest; cdecl; external DLLFILE;

function RateCard(Id: DWORD; var OffenceValue: double; var DefenceValue: double; iFormulaUsed: byte): boolean; cdecl; external DLLFILE;


implementation

end.

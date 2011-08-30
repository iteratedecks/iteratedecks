unit EvaluateDecksSrc;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ImgList, cxGraphics, cxControls, cxContainer, cxEdit, cxImage,
  StdCtrls, ExtCtrls, cxTextEdit, cxMaskEdit, cxDropDownEdit, cxCheckBox, Menus,
  cxLookAndFeelPainters, cxButtons, cxLabel, cxMemo, cxPC, Buttons, cxStyles,
  cxCustomData, cxFilter, cxData, cxDataStorage, DB, cxDBData,
  cxGridCustomTableView, cxGridTableView, cxGridDBTableView, cxGridLevel,
  cxClasses, cxGridCustomView, cxGrid, cxCalendar, IdBaseComponent, IdComponent,
  IdTCPConnection, IdTCPClient, IdHTTP, OleCtrls, SHDocVw_EWB, EwbCore,
  EmbeddedWB, WinInet, ComCtrls, cxListView, Jpeg, cxCheckComboBox,
  cxCurrencyEdit, ActiveX, cxSpinEdit, cxRadioGroup, cxGroupBox, DBClient,
  GIFImg, ShellApi, cxCheckListBox, cxGridBandedTableView;

const
  MAX_CARD_COUNT = 600;
  MAX_DECK_SIZE = 20;
  MAX_SETS_COUNT = 20;
  CARD_NAME_MAX_LENGTH = 50;
  FILENAME_MAX_LENGTH = 50;
  CARD_ABILITIES_MAX = 70;
  cMaxBuffer = 65535;
type
   TCard = Record
     Id: UINT;
     Name: array[0..CARD_NAME_MAX_LENGTH-1] of char;
     Picture: array[0..FILENAME_MAX_LENGTH-1] of char;
     CardType: byte;
     Wait: byte;
     CardSet: UINT; // 'set' is a reserved word in Delphi :(
     Faction: byte;
     Attack: byte;
     Health: byte;
     Rarity: byte;
     Effects: array[0..CARD_ABILITIES_MAX-1] of byte;
     TargetCounts: array[0..CARD_ABILITIES_MAX-1] of byte;
     TargetFactions: array[0..CARD_ABILITIES_MAX-1] of byte;
   end;

type
   TCardSet = record
     Name: array[0..CARD_NAME_MAX_LENGTH-1] of char;
     Icon: array[0..CARD_NAME_MAX_LENGTH-1] of char;
	 Visible: boolean;
   end;

type
  RESULTS = Record
	Win: DWORD;
	Loss: DWORD;
	Points: DWORD;
	AutoPoints: DWORD;
	LPoints: DWORD;
	LAutoPoints: DWORD;
   End;

const MAX_DECKSTRING_SIZE = 1024;
type TEvalParams = record
    Seed: DWORD;
	AtkDeck: array [0..MAX_DECKSTRING_SIZE-1] of Char;
	DefDeck: array [0..MAX_DECKSTRING_SIZE-1] of Char;
	GamesPerThread: DWORD;
	Threads: DWORD;
	Result: RESULTS;
    Seconds: DWORD;
    RaidID: integer;
    Surge: boolean;
end;

type
  TEvaluateDecksForm = class(TForm)
    tLoad: TTimer;
    tCancelHover: TTimer;
    PC: TcxPageControl;
    rPanel: TPanel;
    sbRight: TScrollBox;
    pLibTop: TPanel;
    cbFaction: TcxComboBox;
    cbRarity: TcxComboBox;
    cbType: TcxComboBox;
    cbWait: TcxComboBox;
    pClient: TPanel;
    pTopDeck: TPanel;
    pTopLeft: TPanel;
    imgTop: TcxImage;
    cbAllowMore: TcxCheckBox;
    sbTop: TScrollBox;
    pMiddle: TPanel;
    cbCustom: TcxComboBox;
    cbRaid: TcxComboBox;
    lLoadCustom: TcxLabel;
    bCustomAtk: TcxButton;
    bCustomDef: TcxButton;
    cbUseRaid: TcxCheckBox;
    pBottom: TPanel;
    pBottomLeft: TPanel;
    imgBot: TcxImage;
    sbBot: TScrollBox;
    tsDecks: TcxTabSheet;
    tsHelp: TcxTabSheet;
    cxMemo1: TcxMemo;
    tsEval: TcxTabSheet;
    cbIterations: TcxComboBox;
    bRun: TcxButton;
    cxView: TcxGridTableView;
    cxGridLevel1: TcxGridLevel;
    cxGrid: TcxGrid;
    vcAtk: TcxGridColumn;
    vcDef: TcxGridColumn;
    vcWins: TcxGridColumn;
    vcGames: TcxGridColumn;
    vcRatio: TcxGridColumn;
    cbMission: TcxComboBox;
    lMission: TcxLabel;
    bMission: TcxButton;
    vcAvgD: TcxGridColumn;
    vcAvgDA: TcxGridColumn;
    cbSurge: TcxCheckBox;
    vcType: TcxGridColumn;
    cbSkill: TcxComboBox;
    ceFilter: TcxTextEdit;
    vcAvgS: TcxGridColumn;
    vcAvgSA: TcxGridColumn;
    vcNet: TcxGridColumn;
    tsUpdate: TcxTabSheet;
    bDLXML: TcxButton;
    lUpdateXML: TcxLabel;
    ceLastDate: TcxDateEdit;
    IdHTTP: TIdHTTP;
    IL: TcxImageList;
    ILS: TcxImageList;
    ccbSets: TcxCheckComboBox;
    lSet: TcxLabel;
    bCustom: TcxButton;
    bSaveTopDeck: TcxButton;
    bSaveBotCustomDeck: TcxButton;
    tsSettings: TcxTabSheet;
    rgEval: TcxRadioGroup;
    bSlowProgress: TcxRadioButton;
    bFastThreaded: TcxRadioButton;
    seThreads: TcxSpinEdit;
    lThreads: TcxLabel;
    bClear: TcxButton;
    pmGrid: TPopupMenu;
    bDeleteSelected: TMenuItem;
    bDeleteAll: TMenuItem;
    gbSeed: TcxGroupBox;
    cbRandomSeed: TcxCheckBox;
    seSeed: TcxSpinEdit;
    cxLabel2: TcxLabel;
    cbSkillTargetFaction: TcxComboBox;
    tsMin: TcxTabSheet;
    gTop: TcxGrid;
    vTop: TcxGridTableView;
    vTopName: TcxGridColumn;
    vTopFaction: TcxGridColumn;
    vTopWait: TcxGridColumn;
    gTopLevel1: TcxGridLevel;
    vTopID: TcxGridColumn;
    cbUseHidden: TcxCheckBox;
    vTopAttack: TcxGridColumn;
    vTopHealth: TcxGridColumn;
    vTopSkills: TcxGridColumn;
    cbOneLineSkills: TcxCheckBox;
    lmCustom: TcxLabel;
    lmMission: TcxLabel;
    cbmCustom: TcxComboBox;
    cbmMission: TcxComboBox;
    cbmUseRaid: TcxCheckBox;
    cbmRaid: TcxComboBox;
    bmMission: TcxButton;
    bmCustomDefence: TcxButton;
    bmCustomAttack: TcxButton;
    gBot: TcxGrid;
    vBot: TcxGridTableView;
    vBotID: TcxGridColumn;
    vBotName: TcxGridColumn;
    vBotFaction: TcxGridColumn;
    vBotWait: TcxGridColumn;
    vBotAttack: TcxGridColumn;
    vBotHealth: TcxGridColumn;
    vBotSkills: TcxGridColumn;
    cxGridLevel2: TcxGridLevel;
    bSaveTop: TcxButton;
    bSaveBot: TcxButton;
    bTopVisual: TcxButton;
    bBotVisual: TcxButton;
    xgmlogo: TcxImage;
    teamliquidlogo: TcxImage;
    tsBatch: TcxTabSheet;
    clbTestAgainst: TcxCheckListBox;
    cbBIterations: TcxComboBox;
    bBRun: TcxButton;
    gBatchResult: TcxGrid;
    cxGridLevel3: TcxGridLevel;
    lNote: TcxLabel;
    vBatchResult: TcxGridBandedTableView;
    vcbAgainst: TcxGridBandedColumn;
    vcbFWins: TcxGridBandedColumn;
    vcbFGames: TcxGridBandedColumn;
    vcbFAvgD: TcxGridBandedColumn;
    vcbFAvgDA: TcxGridBandedColumn;
    vcbFAvgS: TcxGridBandedColumn;
    vcbFAvgSA: TcxGridBandedColumn;
    vcbFNet: TcxGridBandedColumn;
    vcbFRatio: TcxGridBandedColumn;
    vcbFLoss: TcxGridBandedColumn;
    vcbSWins: TcxGridBandedColumn;
    vcbSLoss: TcxGridBandedColumn;
    vcbSGames: TcxGridBandedColumn;
    vcbSRatio: TcxGridBandedColumn;
    vcbSAvgD: TcxGridBandedColumn;
    vcbSAvgDA: TcxGridBandedColumn;
    vcbSAvgS: TcxGridBandedColumn;
    vcbSAvgSA: TcxGridBandedColumn;
    vcbSNet: TcxGridBandedColumn;
    procedure FormCreate(Sender: TObject);
    procedure sbRightMouseWheel(Sender: TObject; Shift: TShiftState;
      WheelDelta: Integer; MousePos: TPoint; var Handled: Boolean);
    procedure FormMouseWheel(Sender: TObject; Shift: TShiftState;
      WheelDelta: Integer; MousePos: TPoint; var Handled: Boolean);
    procedure GenericImageMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure tLoadTimer(Sender: TObject);
    procedure UpdateFilter;
    procedure cbRarityPropertiesChange(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure CopyCard(FromCard: TcxImage; ToCard: TcxImage);
    procedure ClearCard(CardImg: TcxImage);
    procedure RearrangeDeck(var Deck: array of TcxImage; iOffset: integer = 0);
    procedure tCancelHoverTimer(Sender: TObject);
    procedure GenericTopHoverMouseMove(Sender: TObject;
      Shift: TShiftState; X, Y: Integer);
    procedure GenericBotHoverMouseMove(Sender: TObject;
      Shift: TShiftState; X, Y: Integer);
    procedure cbAllowMorePropertiesChange(Sender: TObject);
    procedure bCustomAtkClick(Sender: TObject);
    procedure AddCard(var Deck: array of TcxImage; bIsTopDeck: boolean; CardImg: TcxImage);
    procedure cbUseRaidClick(Sender: TObject);
    procedure ClearDeck(var Deck:array of TcxImage);
    procedure cbRaidPropertiesChange(Sender: TObject);
    procedure LoadRaidCommander;
    procedure LoadCustomDeck(DeckName: string; var Deck: array of TcxImage; bIsTopDeck: boolean; bIsMission: boolean = false; bAlwaysUseId: boolean = false);
    procedure bCustomDefClick(Sender: TObject);
    procedure GenericTopMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure GenericBotMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure imgMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure Hover(img: TcxImage;var Deck: array of TcxImage;bIsTopDeck: boolean);
    procedure bRunClick(Sender: TObject);
    procedure bMissionClick(Sender: TObject);
    procedure vcTypeCustomDrawCell(Sender: TcxCustomGridTableView;
      ACanvas: TcxCanvas; AViewInfo: TcxGridTableDataCellViewInfo;
      var ADone: Boolean);
    procedure IdHTTPWorkBegin(ASender: TObject; AWorkMode: TWorkMode;
      AWorkCountMax: Int64);
    procedure IdHTTPWorkEnd(ASender: TObject; AWorkMode: TWorkMode);
    procedure IdHTTPWork(ASender: TObject; AWorkMode: TWorkMode;
      AWorkCount: Int64);
    procedure ProgressStart(Max: Int64; Text: string = ''; Stoppable: boolean = false);
    function ProgressUpdate(Cur: Int64; Text: string = ''): boolean;
    procedure ProgressFinish;
    procedure bDLXMLClick(Sender: TObject);
    function GetRarity(Rarity: integer): string;
    function LoadCardImage(C: Pointer; Index: integer): boolean;
    procedure bCustomClick(Sender: TObject);
    procedure UpdateButtonsVisibility;
    procedure bSaveTopDeckClick(Sender: TObject);
    procedure bSaveBotCustomDeckClick(Sender: TObject);
    function GetCardID(CardName: string): integer;
    procedure bFastThreadedClick(Sender: TObject);
    procedure bClearClick(Sender: TObject);
    procedure bDeleteAllClick(Sender: TObject);
    procedure bDeleteSelectedClick(Sender: TObject);
    procedure cbRandomSeedClick(Sender: TObject);
    procedure cbSkillPropertiesChange(Sender: TObject);
    function GetIndexFromID(Id: string): integer;
    procedure tsDecksShow(Sender: TObject);
    procedure vTopDataControllerRecordChanged(
      ADataController: TcxCustomDataController; ARecordIndex,
      AItemIndex: Integer);
    procedure vTopCustomDrawCell(Sender: TcxCustomGridTableView;
      ACanvas: TcxCanvas; AViewInfo: TcxGridTableDataCellViewInfo;
      var ADone: Boolean);
    procedure cbUseHiddenClick(Sender: TObject);
    procedure vTopNamePropertiesValidate(Sender: TObject;
      var DisplayValue: Variant; var ErrorText: TCaption; var Error: Boolean);
    procedure cbmUseRaidClick(Sender: TObject);
    procedure bmCustomAttackClick(Sender: TObject);
    procedure vBotNamePropertiesValidate(Sender: TObject;
      var DisplayValue: Variant; var ErrorText: TCaption; var Error: Boolean);
    procedure vBotDataControllerRecordChanged(
      ADataController: TcxCustomDataController; ARecordIndex,
      AItemIndex: Integer);
    procedure bmCustomDefenceClick(Sender: TObject);
    procedure bmMissionClick(Sender: TObject);
    procedure bSaveTopClick(Sender: TObject);
    procedure bSaveBotClick(Sender: TObject);
    procedure bTopVisualClick(Sender: TObject);
    procedure bBotVisualClick(Sender: TObject);
    procedure xgmlogoClick(Sender: TObject);
    procedure teamliquidlogoClick(Sender: TObject);
    procedure vTopKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
    procedure vBotKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
    procedure bBRunClick(Sender: TObject);
 private
    { Private declarations }
    Images: array[0..MAX_CARD_COUNT] of TcxImage;
    ImageCount: integer;
    iOffset: integer; // hiding images while loading
    Cards: array[0..MAX_CARD_COUNT] of ^TCard;
    RemapMinID: array[0..MAX_CARD_COUNT] of UINT;
    RemapMinIDInversed: array[0..MAX_CARD_COUNT] of UINT;
    TopDeck: array[0..MAX_DECK_SIZE] of TcxImage;
    BotDeck: array[0..MAX_DECK_SIZE] of TcxImage;
    iHoverTag: integer;
    slLibIndex: TStringList;
    slIDIndex: TStringList;
    slSkillList: TStringList;
    //slSetList: TStringList;
    slUpdate: TStringList;
    sLocalDir: string;
    // used while loading cards, must have them global :(
    intPicture: TPicture;
    intBitmap: TBitmap;
    intmask : TBitmap;
    intal: TStringList;
    CardsLoaded: integer;
    LastCardIndex: integer;
    LastCardIndexBot: integer;
    //LastRecordIndex: integer;
  public
    { Public declarations }
  end;

var
  EvaluateDecksForm: TEvaluateDecksForm;

implementation

{$R *.dfm}

uses LoadingSrc;

const
  cImageW = 160;
  cImageH = 220;
  cOffset = (cImageW div 3);
  cBorder = 2;
  cHoverOffset = (cImageW - cOffset + cBorder);
  TYPE_COMMANDER = 1;
  sRaidsFile = 'raids.xml';
  sMissionsFile = 'missions.xml';
  sCardsFile = 'cards.xml';
  sCardsDir = 'cards\';
  sAssetsFolder = 'http://kg.tyrantonline.com/assets/';
  sImagesFolder = 'http://cdn.tyrantonline.com/warhawk/images/';
  sCustomDecks = 'Custom.txt';
  sCardNotFound = 'Card "%s" not found in database.';

const DLLFILE = 'IterateDecksDLL.dll';
function GetCard(CardName: string): Pointer; cdecl; external DLLFILE;
function GetCardByID(Id: UINT): Pointer; cdecl; external DLLFILE;
function GetCustomDecksList(buffer: PChar; size: DWORD): PChar; cdecl; external DLLFILE;
function GetMissionDecksList(buffer: PChar; size: DWORD; bSortByID: boolean = true): PChar; cdecl; external DLLFILE;
function GetCustomDeck(DeckName: string; buffer: PChar; size: DWORD): PChar; cdecl; external DLLFILE;
function GetMissionDeck(DeckName: string; buffer: PChar; size: DWORD): PChar; cdecl; external DLLFILE;
function GetRaidDecksList(buffer: PChar; size: DWORD): PChar; cdecl; external DLLFILE;
function GetRaidCommanderID(RaidIndex: UINT): UINT; cdecl; external DLLFILE;
function Evaluate(AtkDeck: string; DefDeck: string; gamescount: DWORD): DWORD; cdecl; external DLLFILE;
procedure PrepareDecks(AtkDeck: string; DefDeck: string); cdecl; external DLLFILE;
procedure PrepareDeck(AtkDeck: string); cdecl; external DLLFILE;
procedure GetSkillList(buffer: PChar; size: DWORD); cdecl; external DLLFILE;
procedure EvaluateOnce(var r: RESULTS; bSurge: boolean=false); cdecl; external DLLFILE;
function EvaluateRaidOnce(var r: RESULTS; RaidID: DWORD): boolean; cdecl; external DLLFILE;
function GetCardSize(): integer; cdecl; external DLLFILE;
function LoadCardXML(FileName: string; returnnewcards: PChar; MaxBufferSize: DWORD): boolean; cdecl; external DLLFILE;
function LoadMissionXML(FileName: string): boolean; cdecl; external DLLFILE;
function LoadRaidXML(FileName: string): boolean; cdecl; external DLLFILE;
function GetDBSize(): DWORD; cdecl; external DLLFILE;
function GetSets(Indexes: Pointer; Sets: Pointer; Size: DWORD): DWORD; cdecl; external DLLFILE;
function ReLoadCustomDecks(FileName: string): integer; cdecl; external DLLFILE;
function LoadPointers(Ptr: Pointer; MaxCount: DWORD): DWORD; cdecl; external DLLFILE;
function InsertCustomDeck(List: string): boolean; cdecl; external DLLFILE;
procedure EvaluateInThreads(Seed: DWORD; var r: RESULTS; gamesperthread: DWORD; threadscount: DWORD = 1; bSurge: boolean = false); cdecl; external DLLFILE;
procedure SpeedTest; cdecl; external DLLFILE;

procedure AppendLine(FileName: string; Line: string);
var
  LogFile: TextFile;
begin
  AssignFile(LogFile, FileName);
  try
    if FileExists(FileName) then
      Append(LogFile)
    else
      Rewrite(Logfile);
    writeln(logfile, line);
  finally
    CloseFile(LogFile);
  end;
end;

function GetFactionColor(Faction: integer): TColor;
begin
  if Faction = 1 then
    result := clBlue
  else
    if Faction = 2 then
      result := clMaroon
    else
      if Faction = 3 then
        result := clPurple
      else
        if Faction = 4 then
          result := clGray
        else
          result := clBlack;
end;

function GetAbilityList(X: Pointer): string;
  var C: ^TCard;
  function GetFactionString(Id: Byte): string;
  begin
    if Id = 0 then
      result := ''
    else
      if Id = 1 then
        result := ' Imperial'
      else
        if Id = 2 then
          result := ' Raider'
        else
          if Id = 3 then
            result := ' Bloodthirsty'
          else
            if Id = 4 then
              result := ' Xeno';
  end;
  function GetCountString(Id: Byte): string;
  begin
    if Id = 0 then
      result := ''
    else
      result := ' All';
  end;
  function AddLine(Name: string; Id: Byte; bPower: boolean): string;
  begin
    result := '';
    if (C.Effects[Id] > 0) then
    begin
      result := Name+GetCountString(C.TargetCounts[Id])+GetFactionString(C.TargetFactions[Id]);
      if (bPower) then
        result := result + ' ' + IntToStr(C.Effects[Id]);
      result := result + #13;
    end;
  end;
begin
  C := X;
  result := '';
  result := result + AddLine('Enfeeble',11,true);
  result := result + AddLine('Heal',12,true);
  result := result + AddLine('Infuse',13,false);
  result := result + AddLine('Jam',14,false);
  result := result + AddLine('Mimic',15,false);
  result := result + AddLine('Rally',16,true);
  result := result + AddLine('Recharge',17,false);
  result := result + AddLine('Repair',18,true);
  result := result + AddLine('Shock',19,true);
  result := result + AddLine('Siege',20,true);
  result := result + AddLine('Split',21,false);
  result := result + AddLine('Strike',22,true);
  result := result + AddLine('Weaken',23,true);

  result := result + AddLine('Armored',31,true);
  result := result + AddLine('Counter',32,true);
  result := result + AddLine('Evade',33,false);
  result := result + AddLine('Flying',34,false);
  result := result + AddLine('Payback',35,false);
  result := result + AddLine('Regenerate',36,true);
  result := result + AddLine('Wall',37,false);

  result := result + AddLine('AntiAir',41,true);
  result := result + AddLine('Fear',42,false);
  result := result + AddLine('Flurry',43,true);
  result := result + AddLine('Pierce',44,true);
  result := result + AddLine('Swipe',45,false);
  result := result + AddLine('Valor',46,true);

  result := result + AddLine('Crush',51,true);
  result := result + AddLine('Immobilize',52,false);
  result := result + AddLine('Leech',53,true);
  result := result + AddLine('Poison',54,true);
  result := result + AddLine('Siphon',55,true);

  result := result + AddLine('Backfire',61,true);
  result := result + AddLine('Fusion',62,false);
  result := result + AddLine('Berserk',63,false);
  result := result + AddLine('Protection',64,false);
  result := result + AddLine('Augment',65,false);
  result := result + AddLine('Mist',66,false);
end;

function IterateDecks(Exe: string; Cwd: string; Seed: DWORD; AtkDeck: string; DefDeck: string; RaidID: integer; GamesPerThread: DWORD; Threads: DWORD; bIsSurge: boolean): RESULTS;
var
  SI: TStartupInfo;
  PI: TProcessInformation;

  hFileMapObj: THandle;
  lpBaseAddress: PChar;
  ep: TEvalParams;
begin
  ep.RaidID := RaidID;
  StrPCopy(ep.AtkDeck,AtkDeck);
  StrPCopy(ep.DefDeck,DefDeck);
{  StrPCopy(ep.AtkDeck,'Thadius,Sharpshooter,Sharpshooter,Sharpshooter,Sharpshooter,Sharpshooter,Sharpshooter,Sharpshooter,Sharpshooter,Sharpshooter,Rocket Infantry');
  StrPCopy(ep.DefDeck,'Vyander,Vaporwing,Vaporwing,Vaporwing,Vaporwing,Vaporwing,Vaporwing,Vaporwing,Vaporwing,Vaporwing,Vaporwing');
  StrPCopy(ep.AtkDeck,'Dracorex,Azure Reaper,Azure Reaper,Azure Reaper,Azure Reaper,Sustainer Xolan,Sustainer Xolan,Sustainer Xolan,Vaporwing,Rifter,Apex');
  StrPCopy(ep.DefDeck,'Dracorex,Azure Reaper,Azure Reaper,Azure Reaper,Azure Reaper,Sustainer Xolan,Sustainer Xolan,Sustainer Xolan,Vaporwing,Rifter,Apex');
}
  ep.Seed := Seed;
  ep.GamesPerThread := GamesPerThread;
  ep.Threads := Threads;
  ep.Surge := bIsSurge;

  hFileMapObj := CreateFileMapping(INVALID_HANDLE_VALUE, Nil, PAGE_READWRITE, 0, 256, 'Local\IterateDecksSharedMemory');
  if (hFileMapObj = 0) then
    //ошибочка вышла
    ShowMessage('Не могу создать FileMapping!')
  else
  begin
    //подключим FileMapping к адресному пространству
    //и получим начальный адрес данных
    lpBaseAddress := MapViewOfFile(hFileMapObj, FILE_MAP_ALL_ACCESS, 0, 0, 0);
  end;
  if lpBaseAddress = nil then
    //ошибочка вышла
    ShowMessage('Не могу подключить FileMapping!');

  try
    with SI do
    begin
      FillChar(SI, SizeOf(SI), 0);
      cb := SizeOf(SI);
      dwFlags := STARTF_USESHOWWINDOW or STARTF_USESTDHANDLES;
      wShowWindow := SW_HIDE;
      hStdInput := GetStdHandle(STD_INPUT_HANDLE); // don't redirect stdin
      hStdOutput := GetStdHandle(STD_OUTPUT_HANDLE); // don't redirect stdin
      hStdError := GetStdHandle(STD_ERROR_HANDLE); // don't redirect stdin
      //hStdOutput := StdOutPipeWrite;
      //hStdError := StdOutPipeWrite;
    end;
    CopyMemory(lpBaseAddress,Addr(ep),SizeOf(ep));

    if CreateProcess(nil, PChar(Exe),
                            nil, nil, True, 0, nil,
                            PChar(Cwd), SI, PI) then
      try
        {repeat
          WasOK := ReadFile(StdOutPipeRead, Buffer, 255, BytesRead, nil);
          if BytesRead > 0 then
          begin
            Buffer[BytesRead] := #0;
            Result := Result + Buffer;
          end;
        until not WasOK or (BytesRead = 0); }
        WaitForSingleObject(PI.hProcess, INFINITE);
      finally
        CloseHandle(PI.hThread);
        CloseHandle(PI.hProcess);
      end;
  finally
    CopyMemory(Addr(ep),lpBaseAddress,SizeOf(ep));

    result := ep.Result;

    UnMapViewOfFile(lpBaseAddress);
    //освободим объект FileMapping
    CloseHandle(hFileMapObj);
  end;
end;

function TEvaluateDecksForm.GetIndexFromID(Id: string): integer;
  begin
    if slIDIndex.IndexOfName(Id) < 0 then
      result := -1
    else
    try
      result := StrToInt(slIDIndex.Values[Id]);
    except
      result := -1;
    end;
  end;

procedure TEvaluateDecksForm.CopyCard(FromCard: TcxImage; ToCard: TcxImage);
begin
  ToCard.Picture := FromCard.Picture;
  ToCard.ShowHint := true;
  ToCard.Hint := FromCard.Hint;
end;

procedure TEvaluateDecksForm.ClearCard(CardImg: TcxImage);
begin
  CardImg.Clear;
  CardImg.Hint := '';
  CardImg.ShowHint := false;
end;

procedure TEvaluateDecksForm.ClearDeck(var Deck:array of TcxImage);
var i: integer;
begin
  for i := 0 to MAX_DECK_SIZE - 1 do
    if not Assigned(Deck[i]) then
      break
    else
      if (Deck[i].Hint <> '') then
      begin
        ClearCard(Deck[i]);
        Deck[i].Visible := false;
      end;
end;

function TEvaluateDecksForm.GetCardID(CardName: string): integer;
var
  i,k: integer;
  s: string;
begin
  i := Pos('[',CardName);
  k := Pos(']',CardName);
  if (i > 0) and (k > 0) then
  begin
    s := Copy(CardName,i+1,k-i-1);
    try
      result := StrToInt(slIDIndex.Values[s]);
    except
      //ShowMessage(slIDIndex.Text);
      result := -1;
    end;
  end
  else
  begin
    s := CardName;
    if slLibIndex.IndexOfName(s) < 0 then
      result := -1
    else
    try
      result := StrToInt(slLibIndex.Values[CardName]);
    except
      result := -1;
    end;
  end;
end;

procedure TEvaluateDecksForm.LoadRaidCommander;
var
  p1 : pchar;
  id: integer;
begin
  // LoadDeck
  GetMem(p1, cMaxBuffer); // initialize
  try
    id := GetIndexFromID(IntToStr(GetRaidCommanderID(cbRaid.ItemIndex+1)));
    if id >= 0 then
      CopyCard(Images[id],imgBot)
    else
      ShowMessage(Format(sCardNotFound,[p1]));
  finally
    FreeMem(p1);
  end;
end;

procedure TEvaluateDecksForm.AddCard(var Deck: array of TcxImage; bIsTopDeck: boolean; CardImg: TcxImage);
var i: integer;
begin
// add card
    for i := 0 to MAX_DECK_SIZE - 1 do
      if not Assigned(Deck[i]) then
      begin
        Deck[i] := TcxImage.Create(Self);
        if bIsTopDeck then
        begin
          Deck[i].Parent := sbTop;
          Deck[i].Left := imgTop.Left + cOffset * i;
          Deck[i].Top := imgTop.Top;
          Deck[i].OnMouseDown := GenericTopMouseDown;
          Deck[i].OnMouseMove := GenericTopHoverMouseMove;
       end
        else
        begin
          Deck[i].Parent := sbBot;
          Deck[i].Left := imgBot.Left + cOffset * i;
          Deck[i].Top := imgBot.Top;
          Deck[i].OnMouseDown := GenericBotMouseDown;
          Deck[i].OnMouseMove := GenericBotHoverMouseMove;
       end;
        Deck[i].Properties.GraphicClassName := 'TJPEGImage';
        Deck[i].Properties.ReadOnly := true;
        Deck[i].Properties.ShowFocusRect := false;
        Deck[i].AutoSize := true;
        Deck[i].Tag := i;
        Deck[i].Properties.PopupMenuLayout.MenuItems := [];
        break;
      end
      else
        if (Deck[i].Hint = '') then
          break;
    // here we have slot [i] ready
    if (i < MAX_DECK_SIZE) then
    begin
      if ((i < 10) OR (cbAllowMore.Checked)) OR (not bIsTopDeck) then
      begin
        CopyCard(CardImg,Deck[i]);
        Deck[i].Visible := true;
        inc(i);
      end
      else
        Deck[i].Visible := false;
    end;
    if i > 0 then
      if bIsTopDeck then
      begin
        if i > 10 then
          bSaveTopDeck.Enabled := false
        else
          if imgTop.Hint <> '' then // commander
            bSaveTopDeck.Enabled := true;
      end
      else
      if imgBot.Hint <> '' then  // commander
      begin
        bSaveBotCustomDeck.Enabled := true;
      end;
      
end;

procedure TEvaluateDecksForm.RearrangeDeck(var Deck: array of TcxImage; iOffset: integer = 0);
var i: integer;
begin
  for i := iOffset to MAX_DECK_SIZE - 1 do
    if not Assigned(Deck[i]) then
      break
    else
    begin
      if (Deck[i].Hint = '') and
        (i+1 < MAX_DECK_SIZE - 1) and
        (Assigned(Deck[i+1])) and
        (Deck[i+1].Hint <> '') then
      begin
        // shift to eliminate empty spaces
        CopyCard(Deck[i+1],Deck[i]);
        Deck[i+1].Clear;
        Deck[i+1].Hint := '';
        Deck[i+1].ShowHint := false;
      end;
      Deck[i].Visible := (Deck[i].Hint <> '');
    end;
end;

procedure TEvaluateDecksForm.GenericImageMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  if Button = mbLeft then
    if Cards[(Sender as TcxImage).Tag].CardType = TYPE_COMMANDER then
      CopyCard((Sender as TcxImage),imgTop)
    else
      AddCard(TopDeck,true,Sender as TcxImage);
  if Button = mbRight then
  begin
    if Cards[(Sender as TcxImage).Tag].CardType = TYPE_COMMANDER then
      CopyCard((Sender as TcxImage),imgBot)
    else
      AddCard(BotDeck,false,Sender as TcxImage);
  end;
end;

procedure TEvaluateDecksForm.UpdateButtonsVisibility;
begin
  bSaveTopDeck.Enabled := (imgTop.Hint <> '') AND (Assigned(TopDeck[0]) AND (TopDeck[0].Hint <> '')) AND ((not Assigned(TopDeck[10])) OR (TopDeck[10].Hint = ''));

  bSaveBotCustomDeck.Enabled := (imgBot.Hint <> '') AND (Assigned(BotDeck[0])) AND (BotDeck[0].Hint <> '');
end;

procedure TEvaluateDecksForm.imgMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  if Assigned((Sender as TcxImage).Picture) then
    ClearCard(Sender as TcxImage);

  UpdateButtonsVisibility;
end;

procedure TEvaluateDecksForm.GenericTopMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  if Button = mbLeft then
  begin
    if Assigned((Sender as TcxImage).Picture) then
    begin
      ClearCard(Sender as TcxImage);
      RearrangeDeck(TopDeck,(Sender as TcxImage).Tag);
    end;
  end;
  UpdateButtonsVisibility;
  Abort;
end;

procedure TEvaluateDecksForm.GenericBotMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  if Button = mbLeft then
  begin
    if Assigned((Sender as TcxImage).Picture) then
    begin
      ClearCard(Sender as TcxImage);
      RearrangeDeck(BotDeck,(Sender as TcxImage).Tag);
    end;
  end;
  UpdateButtonsVisibility;
  Abort;
end;

procedure TEvaluateDecksForm.GenericTopHoverMouseMove(Sender: TObject;
  Shift: TShiftState; X, Y: Integer);
begin
  Hover((Sender as TcxImage),TopDeck,true);
end;

procedure TEvaluateDecksForm.GenericBotHoverMouseMove(Sender: TObject;
  Shift: TShiftState; X, Y: Integer);
begin
  Hover((Sender as TcxImage),BotDeck,false);
end;

procedure TEvaluateDecksForm.Hover(img: TcxImage;var Deck: array of TcxImage;bIsTopDeck: boolean);
var
  i,max,l: integer;
begin
  if bIsTopDeck then  
    l := imgTop.Left
  else
    l := imgBot.Left;
  with img do
  begin
    if (Tag <> iHoverTag) then
    begin
      if (iHoverTag >= 0) then
        for i := iHoverTag to Tag+1 do
          if not Assigned(Deck[i]) then
            break
          else
            Deck[i].Left := l + cOffset * i;
      if iHoverTag > Tag then
        max := iHoverTag+1
      else
        max := MAX_DECK_SIZE-1;
        for i := Tag+1 to max do
          if not Assigned(Deck[i]) then
            break
          else
            Deck[i].Left := l + cHoverOffset + cOffset * i;
      iHoverTag := Tag;
      tCancelHover.Enabled := true;
    end;
  end;
end;

procedure TEvaluateDecksForm.IdHTTPWork(ASender: TObject; AWorkMode: TWorkMode;
  AWorkCount: Int64);
begin
  if slUpdate.Count = 0 then
    ProgressUpdate(AWorkCount);
end;

procedure TEvaluateDecksForm.IdHTTPWorkBegin(ASender: TObject;
  AWorkMode: TWorkMode; AWorkCountMax: Int64);
begin
  if slUpdate.Count = 0 then
    ProgressStart(AWorkCountMax,'Download XML sheet ...');
end;

procedure TEvaluateDecksForm.IdHTTPWorkEnd(ASender: TObject;
  AWorkMode: TWorkMode);
begin
  if slUpdate.Count = 0 then
    ProgressFinish;
end;

procedure TEvaluateDecksForm.ProgressStart(Max: Int64; Text: string = ''; Stoppable: boolean = false);
begin
  Screen.Cursor := crHourGlass;
  LoadingForm.cxProgress.Position := 0;
  LoadingForm.cxProgress.Properties.Max := Max;
  LoadingForm.pText.Caption := Text;
  LoadingForm.bHalt := false;
  LoadingForm.bStop.Visible := Stoppable;
  Enabled := false;
  LoadingForm.Show;
end;

function TEvaluateDecksForm.ProgressUpdate(Cur: Int64; Text: string = ''): boolean;
begin
  LoadingForm.cxProgress.Position := Cur;
  if Text <> '' then
    LoadingForm.pText.Caption := Text;
  Application.ProcessMessages;
  result := LoadingForm.bHalt;
end;

procedure TEvaluateDecksForm.ProgressFinish;
begin
  Screen.Cursor := crDefault;
  LoadingForm.Close;
  Enabled := true;
  SetFocus;
end;

procedure TEvaluateDecksForm.UpdateFilter;
var
  i,k, turns: integer;
  skillid: integer;
begin
  k := 0;
  for I := 0 to ImageCount - 1 do
  begin
    if (cbRarity.ItemIndex > 0) and (Cards[i].Rarity <> cbRarity.ItemIndex-1)  then
    begin
      Images[i].Hide;
      continue;
    end;
    if (cbFaction.ItemIndex > 0) and (Cards[i].Faction <> cbFaction.ItemIndex) then
    begin
      Images[i].Hide;
      continue;
    end;
    if (cbType.ItemIndex > 0) and (Cards[i].CardType <> cbType.ItemIndex) then
    begin
      Images[i].Hide;
      continue;
    end;
    turns := cbWait.ItemIndex-1;
    if turns = 5 then
      turns := 6;
    if (cbWait.ItemIndex > 0) and (Cards[i].Wait <> turns) then
    begin
      Images[i].Hide;
      continue;
    end;
    if (cbSkill.ItemIndex > 0) then
    begin
      skillid := slSkillList.IndexOf(cbSkill.Text);
      if (Cards[i].Effects[skillid] = 0) then
      begin
        Images[i].Hide;
        continue;
      end;
      if (cbSkillTargetFaction.ItemIndex > 0) and (Cards[i].TargetFactions[skillid] <> cbSkillTargetFaction.ItemIndex) then
      begin
        Images[i].Hide;
        continue;
      end;
    end;
    if (ceFilter.Text <> '') and (Pos(LowerCase(ceFilter.Text),LowerCase(Cards[i].Name)) = 0) then
    begin
      Images[i].Hide;
      continue;
    end;
    if Pos(';'+IntToStr(Cards[i].CardSet)+';',';'+ccbSets.Text+';') = 0 then
    begin
      Images[i].Hide;
      continue;
    end;
    {wait: byte;
     Attack: byte;
     Health: byte;}
    Images[i].Left := (cImageW + cBorder) * (k mod 2);
    Images[i].Top := (cImageH + cBorder) * (k div 2);
    Images[i].Show;
    inc(k);
  end;
end;

procedure TEvaluateDecksForm.vBotDataControllerRecordChanged(
  ADataController: TcxCustomDataController; ARecordIndex, AItemIndex: Integer);
var skd: string;
begin
  //LastRecordIndex := vTop.DataController.FocusedRecordIndex;
  if (AItemIndex = vBotName.Index) then
  if LastCardIndexBot < 0 then
  begin
    vBot.DataController.SetValue(ARecordIndex,vBotID.Index,-1);
    vBot.DataController.SetValue(ARecordIndex,vBotFaction.Index,NULL);
    vBot.DataController.SetValue(ARecordIndex,vBotWait.Index,NULL);
    vBot.DataController.SetValue(ARecordIndex,vBotAttack.Index,NULL);
    vBot.DataController.SetValue(ARecordIndex,vBotHealth.Index,NULL);
    vBot.DataController.SetValue(ARecordIndex,vBotSkills.Index,NULL);
  end
  else
  begin
    vBot.DataController.SetValue(ARecordIndex,vBotID.Index,LastCardIndexBot);
    vBot.DataController.SetValue(ARecordIndex,vBotFaction.Index,Cards[LastCardIndexBot].Faction);
    vBot.DataController.SetValue(ARecordIndex,vBotWait.Index,Cards[LastCardIndexBot].Wait);
    vBot.DataController.SetValue(ARecordIndex,vBotAttack.Index,Cards[LastCardIndexBot].Attack);
    vBot.DataController.SetValue(ARecordIndex,vBotHealth.Index,Cards[LastCardIndexBot].Health);
    if cbOneLineSkills.Checked then
      with TStringList.Create do
      try
        Text := GetAbilityList(Cards[LastCardIndexBot]);
        skd := StringReplace(StringReplace(CommaText,',',', ',[rfReplaceAll]),'"','',[rfReplaceAll]);
      finally
        Free;
      end
    else
      skd := GetAbilityList(Cards[LastCardIndexBot]);
    vBot.DataController.SetValue(ARecordIndex,vBotSkills.Index,skd);
    //vTop.DataController.SetValue(ARecordIndex,4,string(Cards[LastCardIndex].Name));
  //vTop.DataController.SetDisplayText(LastRecordIndex,1,Inttostr(LastCardIndex));
  end;
end;

procedure TEvaluateDecksForm.vBotKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  if (key = 46) OR (key = 8) then
  with vBot.DataController do
  begin
    SetValue(FocusedRecordIndex,vTopID.Index,-1);
    SetValue(FocusedRecordIndex,vTopName.Index,NULL);
    SetValue(FocusedRecordIndex,vTopFaction.Index,NULL);
    SetValue(FocusedRecordIndex,vTopWait.Index,NULL);
    SetValue(FocusedRecordIndex,vTopAttack.Index,NULL);
    SetValue(FocusedRecordIndex,vTopHealth.Index,NULL);
    SetValue(FocusedRecordIndex,vTopSkills.Index,NULL);
  end;
end;

procedure TEvaluateDecksForm.vBotNamePropertiesValidate(Sender: TObject;
  var DisplayValue: Variant; var ErrorText: TCaption; var Error: Boolean);
var
  i,k:integer;
begin
  if (Sender as TcxComboBox).ItemIndex < 0 then
    LastCardIndexBot := -1
  else
  begin
    LastCardIndexBot := (Sender as TcxComboBox).ItemIndex;
    // do checks on unique etc
    with vBot.DataController do
    begin
      for i := 0 to RecordCount - 1 do
        if (i <> FocusedRecordIndex) and (not VarIsNull(Values[i,vBotID.Index])) then
        begin
          k := Values[i,vBotID.Index];
          if (k < 0) then
            continue;
          if (Cards[k].CardType = 1) and (Cards[LastCardIndexBot].CardType = 1) then
          begin
            LastCardIndexBot := -1;
            ErrorText := 'Commander already present.';
            Error := true;
            break;
          end;
          if (Cards[k].Rarity = 4) and (Cards[LastCardIndexBot].Rarity = 4) then
          begin
            LastCardIndexBot := -1;
            ErrorText := 'Legendary card already present.';
            Error := true;
            break;
          end;
          if (Cards[k].Rarity = 3) and (Cards[LastCardIndexBot].Rarity = 3) and
             (k = LastCardIndexBot) then
          begin
            LastCardIndexBot := -1;
            ErrorText := 'This unique card already present.';
            Error := true;
            break;
          end;
        end;
    end;
  end;
end;

procedure TEvaluateDecksForm.vcTypeCustomDrawCell(
  Sender: TcxCustomGridTableView; ACanvas: TcxCanvas;
  AViewInfo: TcxGridTableDataCellViewInfo; var ADone: Boolean);
begin
  if AViewInfo.GridRecord.Values[vcType.Index] = 'Fight' then
    ACanvas.Font.Color := clRed
  else
    if AViewInfo.GridRecord.Values[vcType.Index] = 'Surge' then
      ACanvas.Font.Color := clBlue;
end;


{IF VarIsNull(ARecord.Values[Sender.Index]) then exit;
IF ARecord.Values[Sender.Index]=0 then
 Begin
   AText:='';
   exit;
 End;
case ARecord.Values[0] of
 0 : AText := FormatFloat('#', ARecord.Values[Sender.Index]);
 1 : AText := FormatFloat('#.0', ARecord.Values[Sender.Index]);
 2 : AText := FormatFloat('#.00', ARecord.Values[Sender.Index]);
End;  }
{  if (ARecord.Index = LastCardIndex) then
    AText := inttostr(LastRecordIndex)
  else
    exit; // dont change  }
procedure TEvaluateDecksForm.vTopCustomDrawCell(Sender: TcxCustomGridTableView;
  ACanvas: TcxCanvas; AViewInfo: TcxGridTableDataCellViewInfo;
  var ADone: Boolean);
Var
  id : integer;
  colid: integer;
  ARecord: TcxCustomGridRecord;
begin
  ARecord := AViewInfo.GridRecord;
  if (AViewInfo.Item.Tag = 0) then
    ACanvas.Brush.Color := clBtnFace
  else
    if ARecord.Index >= 11 then 
      ACanvas.Brush.Color := clSkyBlue;

  if Sender = vTop then
    colid := vTopId.Index
  else
    colid := vBotId.Index;
  if Assigned(ARecord) and (not VarIsNull(ARecord.Values[colid])) and (AViewInfo.Item.Index <> colid) then
  begin
    id := ARecord.Values[colid];
    if id < 0 then
      exit;
    ACanvas.Canvas.Font.Color := GetFactionColor(Cards[id].Faction);
    if Cards[id].CardType = 1 then // commander
      ACanvas.Canvas.Font.Style := [fsBold]
    else
      if Cards[id].CardType = 4 then // action
        ACanvas.Canvas.Font.Style := [fsItalic];
  end;
end;

procedure TEvaluateDecksForm.vTopDataControllerRecordChanged(
  ADataController: TcxCustomDataController; ARecordIndex, AItemIndex: Integer);
var skd: string;
begin
  //LastRecordIndex := vTop.DataController.FocusedRecordIndex;
  if (AItemIndex = vTopName.Index) then
  if LastCardIndex < 0 then
  begin
    vTop.DataController.SetValue(ARecordIndex,vTopID.Index,-1);
    vTop.DataController.SetValue(ARecordIndex,vTopFaction.Index,NULL);
    vTop.DataController.SetValue(ARecordIndex,vTopWait.Index,NULL);
    vTop.DataController.SetValue(ARecordIndex,vTopAttack.Index,NULL);
    vTop.DataController.SetValue(ARecordIndex,vTopHealth.Index,NULL);
    vTop.DataController.SetValue(ARecordIndex,vTopSkills.Index,NULL);
  end
  else
  begin
    vTop.DataController.SetValue(ARecordIndex,vTopID.Index,LastCardIndex);
    vTop.DataController.SetValue(ARecordIndex,vTopFaction.Index,Cards[LastCardIndex].Faction);
    vTop.DataController.SetValue(ARecordIndex,vTopWait.Index,Cards[LastCardIndex].Wait);
    vTop.DataController.SetValue(ARecordIndex,vTopAttack.Index,Cards[LastCardIndex].Attack);
    vTop.DataController.SetValue(ARecordIndex,vTopHealth.Index,Cards[LastCardIndex].Health);
    if cbOneLineSkills.Checked then
      with TStringList.Create do
      try
        Text := GetAbilityList(Cards[LastCardIndex]);
        skd := StringReplace(StringReplace(CommaText,',',', ',[rfReplaceAll]),'"','',[rfReplaceAll]);
      finally
        Free;
      end
    else
      skd := GetAbilityList(Cards[LastCardIndex]);
    vTop.DataController.SetValue(ARecordIndex,vTopSkills.Index,skd);
    //vTop.DataController.SetValue(ARecordIndex,4,string(Cards[LastCardIndex].Name));
  //vTop.DataController.SetDisplayText(LastRecordIndex,1,Inttostr(LastCardIndex));
  end;
end;

procedure TEvaluateDecksForm.vTopKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  if (key = 46) OR (key = 8) then
  with vTop.DataController do
  begin
    SetValue(FocusedRecordIndex,vTopID.Index,-1);
    SetValue(FocusedRecordIndex,vTopName.Index,NULL);
    SetValue(FocusedRecordIndex,vTopFaction.Index,NULL);
    SetValue(FocusedRecordIndex,vTopWait.Index,NULL);
    SetValue(FocusedRecordIndex,vTopAttack.Index,NULL);
    SetValue(FocusedRecordIndex,vTopHealth.Index,NULL);
    SetValue(FocusedRecordIndex,vTopSkills.Index,NULL);
  end;
end;

procedure TEvaluateDecksForm.vTopNamePropertiesValidate(Sender: TObject;
  var DisplayValue: Variant; var ErrorText: TCaption; var Error: Boolean);
var
  i,k:integer;
begin
  if (Sender as TcxComboBox).ItemIndex < 0 then
    LastCardIndex := -1
  else
  begin
    LastCardIndex := RemapMinID[(Sender as TcxComboBox).ItemIndex];
    // do checks on unique etc
    with vTop.DataController do
    begin
      for i := 0 to RecordCount - 1 do
        if (i <> FocusedRecordIndex) and (not VarIsNull(Values[i,vTopID.Index])) then
        begin
          k := Values[i,vTopID.Index];
          if (k < 0) then
            continue;
          if (Cards[k].CardType = 1) and (Cards[LastCardIndex].CardType = 1) then
          begin
            LastCardIndex := -1;
            ErrorText := 'Commander already present.';
            Error := true;
            break;
          end;
          if (Cards[k].Rarity = 4) and (Cards[LastCardIndex].Rarity = 4) then
          begin
            LastCardIndex := -1;
            ErrorText := 'Legendary card already present.';
            Error := true;
            break;
          end;
          if (Cards[k].Rarity = 3) and (Cards[LastCardIndex].Rarity = 3) and
             (k = LastCardIndex) then
          begin
            LastCardIndex := -1;
            ErrorText := 'This unique card already present.';
            Error := true;
            break;
          end;
        end;
    end;
  end;
end;

procedure TEvaluateDecksForm.xgmlogoClick(Sender: TObject);
begin
  ShellExecute(Handle, 'open', 'http://xgm.ru/?username=NETRAT', nil, nil, SW_SHOWNORMAL) ;
end;

{function TEvaluateDecksForm.GetCardID(CardName: string): integer;
var
  s: string;
  i: integer;
begin
  i := Pos('['
  
  s := slLibIndex.Values[CardName];
  if True then

end;    }

procedure TEvaluateDecksForm.LoadCustomDeck(DeckName: string; var Deck: array of TcxImage; bIsTopDeck: boolean; bIsMission: boolean = false; bAlwaysUseId: boolean = false);
  function GetCommanderNameEx(BaseName: string; var sMod: string): string;
  var p,v : integer;
  begin
    p := Pos('[',BaseName);
    if p > 0 then
    begin
      // modified commander
      v := Pos(']',BaseName);
      result := Copy(BaseName,1,p-1);
      sMod := Copy(BaseName,p+1,v-p-1);
    end
    else
    begin
      result := BaseName;
      sMod := '';
    end;
  end;
var
  p1 : pchar;
  i,id: integer;
  //cname, cmod: string;
begin
  // LoadDeck
  GetMem(p1, cMaxBuffer); // initialize
  with TStringList.Create do
  try
    if bIsMission then
      CommaText := GetMissionDeck(DeckName,p1,cMaxBuffer)
    else
      CommaText := GetCustomDeck(DeckName,p1,cMaxBuffer);
    if Count > 0 then
    begin
      ClearDeck(Deck);
      //cname := GetCommanderNameEx(Strings[0],cmod);
      if bAlwaysUseId then
        id := GetIndexFromID(Strings[0])
      else
        id := GetCardID(Strings[0]);
      if id < 0 then
        ShowMessage(Format(sCardNotFound,[Strings[0]]))
      else
        if bIsTopDeck then
        begin
          CopyCard(Images[id],imgTop);
          //if cmod <> '' then
          //  imgTop.Hint := cname + '[' + cmod + ']'; // fix the hint - add mod
        end
        else
        begin
          CopyCard(Images[id],imgBot);
          //if cmod <> '' then
          //  imgBot.Hint := Strings[0]; // fix the hint
        end;
      for i := 1 to Count - 1 do
      begin
        if bAlwaysUseId then
          id := GetIndexFromID(Strings[i])
        else
          id := GetCardID(Strings[i]);
        if id < 0 then
          ShowMessage(Format(sCardNotFound,[Strings[i]]))
        else
          AddCard(Deck,bIsTopDeck,Images[id]);
      end;
    end;
  finally
    FreeMem(p1);
    Free;
  end;
end;

procedure TEvaluateDecksForm.bBotVisualClick(Sender: TObject);
var
  i,id: integer;
begin
    ClearDeck(BotDeck);
    with vBot.DataController do
      for i := 0 to RecordCount - 1 do
        if (not VarIsNull(Values[i,vBotName.Index])) and (not VarIsNull(Values[i,vBotID.Index])) then
        begin
          id := values[i,vBotID.Index];
          if Cards[id].CardType = 1 then
          begin
            CopyCard(Images[id],imgBot);
          end
          else
            AddCard(BotDeck,false,Images[id]);
        end;
end;

procedure TEvaluateDecksForm.bBRunClick(Sender: TObject);
  var
  i, games, rec, seed, id: integer;
  sl1,sl2: TStringList;
  r: RESULTS;
  rand, tc: DWORD;
  dt: TDateTime;

  atk, def: string;
  ret: LongWord;

  LibHandle: THandle;
  bImages: boolean;
  s:string;
  p1: pchar;
begin
  vBatchResult.DataController.RecordCount := 0;

  sl1 := TStringList.Create;
  sl2 := TStringList.Create;

  bImages := (imgTop.Hint <> '');

  with EvaluateDecksForm do
  try
    if bImages then
    begin
      sl1.Add(imgTop.Hint);
      for i := 0 to MAX_DECK_SIZE - 1 do
        if (Assigned(TopDeck[i])) then
          if (TopDeck[i].Hint <> '') then
            sl1.Add(TopDeck[i].Hint);
    end
    else
    begin
      with vTop.DataController do
      for i := 0 to RecordCount - 1 do
        if (not VarIsNull(Values[i,vTopId.Index])) and (not VarIsNull(Values[i,vTopName.Index])) then
        begin
          id := values[i,vTopID.Index];
          s := Values[i,vTopName.Index];
          if Cards[remapminidinversed[id]].CardType = 1 then
            sl1.Insert(0,s)
          else
            if s <> '' then
              sl1.Add(s);
        end;
    end;

    games := StrToInt(cbBIterations.Text);

    atk := StringReplace(sl1.CommaText,'"','',[rfReplaceAll]);
      tc := seThreads.Value;

    if (atk = '') then
    begin
      ShowMessage('Your deck is empty.');
      exit;
    end;

  for i := 0 to clbTestAgainst.Count - 1 do
  begin
    if not clbTestAgainst.Items[i].Checked then
      continue;

    GetMem(p1, cMaxBuffer); // initialize
    with TStringList.Create do
    try
      sl2.CommaText := GetCustomDeck(clbTestAgainst.Items[i].Text,p1,cMaxBuffer);
    finally
      FreeMem(p1);
    end;

    def := StringReplace(sl2.CommaText,'"','',[rfReplaceAll]);
    if (def = '') then
    begin
      //ShowMessage('One of the decks is empty.');
      continue;
    end;
    with vBatchResult.DataController do
    begin
      AppendRecord;
      rec := RecordCount-1;
      Values[rec,vcbAgainst.Index] := StringReplace(def,',',', ',[rfReplaceAll]);
    end;

      if cbRandomSeed.Checked then
      begin
        seed := random(MaxInt);
        seSeed.Text := IntToStr(seed);
      end
      else
        try
          seed := StrToInt(seSeed.Text);
        except
          seed := 0;
        end;

      r := IterateDecks('IterateDecks.exe',sLocalDir,seed,atk,def,-1,games div tc,tc,false);

    with vBatchResult.DataController do
      begin
        Values[rec,vcbFWins.Index] := r.Win;
        Values[rec,vcbFLoss.Index] := r.Loss;
        Values[rec,vcbFGames.Index] := games;
        Values[rec,vcbFRatio.Index] := r.Win / games;
        Values[rec,vcbFAvgD.Index] := r.Points / games;
        Values[rec,vcbFAvgDA.Index] := r.AutoPoints / games;
        Values[rec,vcbFAvgS.Index] := r.LPoints / games;
        Values[rec,vcbFAvgSA.Index] := r.LAutoPoints / games;
        Values[rec,vcbFNet.Index] := Integer(r.Points - r.LPoints) / games;
      end;

    Application.ProcessMessages;

      r := IterateDecks('IterateDecks.exe',sLocalDir,seed,atk,def,-1,games div tc,tc,true);

    with vBatchResult.DataController do
      begin
        Values[rec,vcbSWins.Index] := r.Win;
        Values[rec,vcbSLoss.Index] := r.Loss;
        Values[rec,vcbSGames.Index] := games;
        Values[rec,vcbSRatio.Index] := r.Win / games;
        Values[rec,vcbSAvgD.Index] := r.Points / games;
        Values[rec,vcbSAvgDA.Index] := r.AutoPoints / games;
        Values[rec,vcbSAvgS.Index] := r.LPoints / games;
        Values[rec,vcbSAvgSA.Index] := r.LAutoPoints / games;
        Values[rec,vcbSNet.Index] := Integer(r.Points - r.LPoints) / games;
      end;

    Application.ProcessMessages;

    end;
    //ShowMessage('Attacker wins '+inttostr(wins)+' of '+inttostr(games)+' games.');
  finally
    sl1.Free;
    sl2.Free;
  end;
  //EndThread(0);
end;

procedure TEvaluateDecksForm.bClearClick(Sender: TObject);
begin
  cxView.DataController.RecordCount := 0;
end;

procedure TEvaluateDecksForm.bCustomAtkClick(Sender: TObject);
begin
  LoadCustomDeck(cbCustom.Text, TopDeck, true);
end;

procedure TEvaluateDecksForm.bCustomClick(Sender: TObject);
var
  i: integer;
  p1: PChar;
begin
  i := ReLoadCustomDecks(sLocalDir + sCustomDecks);
  if i = -2 then
    ShowMessage('File '+sCustomDecks+' not found.');
  if i >= 0 then
    ShowMessage('Error while loading file '+sCustomDecks+' on line '+IntToStr(i));
  GetMem(p1, cMaxBuffer); // initialize
  GetCustomDecksList(p1,cMaxBuffer);
  cbCustom.Properties.Items.CommaText := p1;
  cbmCustom.Properties.Items.CommaText := p1;
  for i := 0 to cbmCustom.Properties.Items.Count - 1 do
  begin
    clbTestAgainst.AddItem(cbmCustom.Properties.Items[i]);
    clbTestAgainst.Items[i].Checked := true;
  end;
  if cbCustom.Properties.Items.Count > 0 then
  begin
    cbCustom.ItemIndex := 0;
    cbmCustom.ItemIndex := 0;
  end;
  FreeMem(p1);
end;

procedure TEvaluateDecksForm.bCustomDefClick(Sender: TObject);
begin
  LoadCustomDeck(cbCustom.Text, BotDeck, false);
end;

procedure TEvaluateDecksForm.bDeleteAllClick(Sender: TObject);
begin
  cxView.DataController.RecordCount := 0;
end;

procedure TEvaluateDecksForm.bDeleteSelectedClick(Sender: TObject);
begin
  //cxView.DataController.GetSelectedCount;
  //cxView.DataController.DeleteFocused;
  with cxView.DataController do
    DeleteRecord(cxView.DataController.FocusedRowIndex);
end;

procedure TEvaluateDecksForm.bDLXMLClick(Sender: TObject);
  {function IsValidFileName(const fileName : string) : boolean;
 const
   InvalidCharacters : set of char = ['\', '/', ':', '*', '?', '"', '<', '>', '|'];
 var
   c : char;
 begin
   result := fileName <> '';
 
   if result then
   begin
     for c in fileName do
     begin
       result := NOT (c in InvalidCharacters) ;
       if NOT result then break;
     end;
   end;
 end; }(* IsValidFileName *)
var
  ms: TMemoryStream;
  fileDate, i: integer;
  p1 : pchar;
  C: ^TCard;
  fname: string;
begin
  ms := TMemoryStream.Create;
  try
    try
      ms.Position := 0;
      IdHttp.Get(sAssetsFolder + sCardsFile,ms);
      ms.SaveToFile(sLocalDir + sCardsFile);
      ms.Position := 0;
      IdHttp.Get(sAssetsFolder + sRaidsFile,ms);
      ms.SaveToFile(sLocalDir + sRaidsFile);
      ms.Position := 0;
      IdHttp.Get(sAssetsFolder + sMissionsFile,ms);
      ms.SaveToFile(sLocalDir + sMissionsFile);
      fileDate := FileAge(sLocalDir + sCardsFile);
      if fileDate > -1 then
        ceLastDate.Date := FileDateToDateTime(fileDate);

      // reload database
      GetMem(p1, cMaxBuffer); // initialize
      try
        LoadMissionXML(sLocalDir + sMissionsFile);
        LoadRaidXML(sLocalDir + sRaidsFile);
        slUpdate.Clear;
        if LoadCardXML(sLocalDir + sCardsFile,p1,cMaxBuffer) then
        begin
          slUpdate.CommaText := p1;
          if slUpdate.Count = 0 then
          begin
            ShowMessage('Database is already up to date.');
            exit;
          end;
          //
          ProgressStart(slUpdate.Count,'Downloading images ...');
          for i := 0 to slUpdate.Count - 1 do
          begin
            ms.Position := 0;
            C := GetCardByID(StrToInt(slUpdate[i]));
            fname := sLocalDir + sCardsDir + {slUpdate[i]}C.Picture;
            if not FileExists(fname) then
            try
              IdHttp.Get(sImagesFolder + C.Picture,ms);
              if (ms.Size > 0) {and IsValidFileName(fname)} then
              begin
                ms.SaveToFile(fname);
              end;
            except
              on E: Exception do
              begin
                ShowMessage('Error while downloading file'#13+sImagesFolder + C.Picture+#13+E.Message);
              end;
            end;
            // load into lib
            if FileExists(fname) then
              if LoadCardImage(C,ImageCount) then
                inc(ImageCount);

            ProgressUpdate(i);
          end;
        end;
      finally
        FreeMem(p1);
        ProgressFinish;
      end;
    except
      on E: Exception do
      begin
        ShowMessage(E.Message);
      end;
    end;
  finally
    ms.Free;
  end;
end;

procedure TEvaluateDecksForm.bFastThreadedClick(Sender: TObject);
begin
  seThreads.Enabled := bFastThreaded.Checked;
  seSeed.Enabled := bFastThreaded.Checked;
  cbRandomSeed.Enabled := bFastThreaded.Checked;
end;

procedure TEvaluateDecksForm.bmCustomAttackClick(Sender: TObject);
var
  p1 : pchar;
  i,id: integer;
begin
  // LoadDeck
  GetMem(p1, cMaxBuffer); // initialize
  with TStringList.Create do
  try
    CommaText := GetCustomDeck(cbmCustom.Text,p1,cMaxBuffer);
    if Count > 0 then
    begin
      for i := 0 to Count - 1 do
      begin
        id := GetCardID(Strings[i]);
        if id < 0 then
          ShowMessage(Format(sCardNotFound,[Strings[i]]))
        else
        begin
          LastCardIndex := id;
          vTop.DataController.SetValue(i,vTopID.Index,id);
          vTop.DataController.SetValue(i,vTopName.Index,(vTopName.Properties as TcxComboBoxProperties).Items[remapMinIDInversed[id]]);
          //vTop.DataController.SetValue(i,2,id);
          //vTop.DataController.SetValue(i,3,id);
        end;
      end;
      for i := Count to vTop.DataController.RecordCount - 1 do
        begin
          LastCardIndex := -1;
          if VarIsNull(vTop.DataController.Values[i,vTopID.Index]) then
            Continue;
          vTop.DataController.SetValue(i,vTopID.Index,-1);
          vTop.DataController.SetValue(i,vTopName.Index,'');
        end;
    end;
  finally
    FreeMem(p1);
    Free;
  end;
end;

procedure TEvaluateDecksForm.bmCustomDefenceClick(Sender: TObject);
var
  p1 : pchar;
  i,id: integer;
begin
  // LoadDeck
  GetMem(p1, cMaxBuffer); // initialize
  with TStringList.Create do
  try
    CommaText := GetCustomDeck(cbmCustom.Text,p1,cMaxBuffer);
    if Count > 0 then
    begin
      for i := 0 to Count - 1 do
      begin
        id := GetCardID(Strings[i]);
        if id < 0 then
          ShowMessage(Format(sCardNotFound,[Strings[i]]))
        else
        begin
          LastCardIndexBot := id;
          vBot.DataController.SetValue(i,vBotID.Index,id);
          vBot.DataController.SetValue(i,vBotName.Index,(vBotName.Properties as TcxComboBoxProperties).Items[id]);
          //vTop.DataController.SetValue(i,2,id);
          //vTop.DataController.SetValue(i,3,id);
        end;
      end;
      for i := Count to vBot.DataController.RecordCount - 1 do
        begin
          if VarIsNull(vBot.DataController.Values[i,vBotID.Index]) then
            Continue;
          LastCardIndexBot := -1;
          vBot.DataController.SetValue(i,vBotID.Index,-1);
          vBot.DataController.SetValue(i,vBotName.Index,'');
        end;
    end;
  finally
    FreeMem(p1);
    Free;
  end;
end;

procedure TEvaluateDecksForm.bMissionClick(Sender: TObject);
begin
  LoadCustomDeck(cbMission.Text, BotDeck, false, true, true);
end;

procedure TEvaluateDecksForm.bmMissionClick(Sender: TObject);
var
  p1 : pchar;
  i,id: integer;
begin
  // LoadDeck
  GetMem(p1, cMaxBuffer); // initialize
  with TStringList.Create do
  try
    CommaText := GetMissionDeck(cbmMission.Text,p1,cMaxBuffer);
    if Count > 0 then
    begin
      for i := 0 to Count - 1 do
      begin
        id := GetIndexFromID(Strings[i]);
        if Count > vBot.DataController.RecordCount then
          vBot.DataController.RecordCount := Count;
        if id < 0 then
          ShowMessage(Format(sCardNotFound,[Strings[i]]))
        else
        begin
          LastCardIndexBot := id;
          vBot.DataController.SetValue(i,vBotID.Index,id);
          vBot.DataController.SetValue(i,vBotName.Index,(vBotName.Properties as TcxComboBoxProperties).Items[id]);
          //vTop.DataController.SetValue(i,2,id);
          //vTop.DataController.SetValue(i,3,id);
        end;
      end;
      for i := Count to vBot.DataController.RecordCount - 1 do
        begin
          if VarIsNull(vBot.DataController.Values[i,vBotID.Index]) then
            Continue;
          LastCardIndexBot := -1;
          vBot.DataController.SetValue(i,vBotID.Index,-1);
          vBot.DataController.SetValue(i,vBotName.Index,'');
        end;
    end;
  finally
    FreeMem(p1);
    Free;
  end;
end;

procedure TEvaluateDecksForm.bRunClick(Sender: TObject);
{var
  ret: LongWord;
begin
  CloseHandle(BeginThread(nil, 0, Addr(EvaluateThread),
    Addr(Self), 0, ret));}
var
  i, games, rec, raid, seed, id: integer;
  sl1,sl2: TStringList;
  bIsRaid, bIsSurge: boolean;
  r: RESULTS;
  rand, tc: DWORD;
  dt: TDateTime;

  atk, def: string;
  ret: LongWord;

  //SpeedTest: procedure;
  LibHandle: THandle;
  bImages: boolean;
  s:string;
begin
  //CoInitialize(nil);
  sl1 := TStringList.Create;
  sl2 := TStringList.Create;

  bImages := (imgTop.Hint <> '') and (imgBot.Hint <> '');

  with EvaluateDecksForm do
  try
    if bImages then
    begin
      sl1.Add(imgTop.Hint);
      for i := 0 to MAX_DECK_SIZE - 1 do
        if (Assigned(TopDeck[i])) then
          if (TopDeck[i].Hint <> '') then
            sl1.Add(TopDeck[i].Hint);
    end
    else
    begin
      with vTop.DataController do
      for i := 0 to RecordCount - 1 do
        if (not VarIsNull(Values[i,vTopId.Index])) and (not VarIsNull(Values[i,vTopName.Index])) then
        begin
          id := values[i,vTopID.Index];
          s := Values[i,vTopName.Index];
          if Cards[remapminidinversed[id]].CardType = 1 then
            sl1.Insert(0,s)
          else
            if s <> '' then
              sl1.Add(s);
        end;
    end;

    bIsRaid := (cbUseRaid.Checked and bImages) OR cbmUseRaid.Checked;
    bIsSurge := cbSurge.Checked;

    if not bIsRaid then
    begin
      if bImages then
      begin
        sl2.Add(imgBot.Hint);
        for i := 0 to MAX_DECK_SIZE - 1 do
          if (Assigned(BotDeck[i])) then
            if (BotDeck[i].Hint <> '') then
              sl2.Add(BotDeck[i].Hint);
      end
      else
      begin
        with vBot.DataController do
        for i := 0 to RecordCount - 1 do
          if (not VarIsNull(Values[i,vBotID.Index])) and (not VarIsNull(Values[i,vBotName.Index])) then
          begin
            id := values[i,vBotID.Index];
            s := Values[i,vBotName.Index];
            if Cards[remapminidinversed[id]].CardType = 1 then
              sl2.Insert(0,s)
            else
              if s <> '' then
                sl2.Add(s);
          end;
      end;
    end
    else
      if bImages then      
        sl2.Add(cbRaid.Text)
      else
        sl2.Add(cbmRaid.Text);
    //cxmemo1.Lines.Add(sl1.CommaText);
    //cxmemo1.Lines.Add(sl2.CommaText);
    games := StrToInt(cbIterations.Text);
    if not bFastThreaded.Checked then
    begin
      r.Win := 0;
      r.Loss := 0;
      r.Points := 0;
      r.AutoPoints := 0;
      if not bIsRaid then
        PrepareDecks(sl1.CommaText,sl2.CommaText)
      else
        PrepareDeck(sl1.CommaText);
    end;

    atk := StringReplace(sl1.CommaText,'"','',[rfReplaceAll]);
    def := StringReplace(sl2.CommaText,'"','',[rfReplaceAll]);
    if (atk = '') or ((def = '') and (not bIsRaid))  then
    begin
      Exception.Create('One of the decks is empty, can''t continue');
      exit;
    end;
    with cxView.DataController do
    begin
      AppendRecord;
      rec := RecordCount-1;
      if bIsRaid then
        Values[rec,vcType.Index] := 'Raid'
      else
        if bIsSurge then
          Values[rec,vcType.Index] := 'Surge'
        else
          Values[rec,vcType.Index] := 'Fight';
      Values[rec,vcAtk.Index] := StringReplace(atk,',',', ',[rfReplaceAll]);
      Values[rec,vcDef.Index] := StringReplace(def,',',', ',[rfReplaceAll]);
    end;

    if not bFastThreaded.Checked then
    begin
      Enabled := false;
      ProgressStart(games,'Evaluation is in progress ...',true);
      i := 0;
      while i < games do
      begin
        if not bIsRaid then
        begin
          EvaluateOnce(r, bIsSurge);
        end
        else
          EvaluateRaidOnce(r, cbRaid.ItemIndex+1);

        inc(i);
        if ProgressUpdate(i+1, 'Evaluation is in progress, '+IntToStr(r.Win) + ' wins, '+IntToStr(r.Loss)+' loss so far') then
          break;
      end;
      ProgressFinish;
    end
    else
    begin
      if bIsRaid then
        if bImages then
          raid := cbRaid.ItemIndex + 1
        else
          raid := cbmRaid.ItemIndex + 1
      else
        raid := -1;
      tc := seThreads.Value;
      if cbRandomSeed.Checked then
      begin
        seed := random(MaxInt);
        seSeed.Text := IntToStr(seed);
      end
      else
        try
          seed := StrToInt(seSeed.Text);
        except
          seed := 0;
        end;
      r := IterateDecks('IterateDecks.exe',sLocalDir,seed,atk,def,raid,games div tc,tc,bIsSurge);
      i := games;
    end;
    //wins := Evaluate(sl1.CommaText,sl2.CommaText,games);

    with cxView.DataController do
      begin
        Values[rec,vcWins.Index] := r.Win;
        Values[rec,vcGames.Index] := i;
        Values[rec,vcRatio.Index] := r.Win / i;
        Values[rec,vcAvgD.Index] := r.Points / i;
        Values[rec,vcAvgDA.Index] := r.AutoPoints / i;
        Values[rec,vcAvgS.Index] := r.LPoints / i;
        Values[rec,vcAvgSA.Index] := r.LAutoPoints / i;
        Values[rec,vcNet.Index] := Integer(r.Points - r.LPoints) / i;
      end;

    //ShowMessage('Attacker wins '+inttostr(wins)+' of '+inttostr(games)+' games.');
  finally
    sl1.Free;
    sl2.Free;
  end;
  //EndThread(0);
end;

procedure TEvaluateDecksForm.bSaveBotClick(Sender: TObject);
var i, id: integer;
slr: TStringList;
name,line: string;
begin
  name := InputBox('Deck name','Input deck name','');
  if name <> '' then
  begin
    slr := TStringList.Create;
    with vBot.DataController do
      for i := 0 to RecordCount - 1 do
        if (not VarIsNull(Values[i,vBotName.Index])) and (not VarIsNull(Values[i,vBotID.Index])) then
        begin
          id := values[i,vBotID.Index];
          if (id = -1) then
            continue;
          if Cards[id].CardType = 1 then
            slr.Insert(0,Values[i,vBotName.Index])
          else
            slr.Add(Values[i,vBotName.Index]);
        end;

    line := name + ':' + StringReplace(slr.CommaText,'"','',[rfReplaceAll]);
    if InsertCustomDeck(line) then
    begin
      cbCustom.Properties.Items.Add(name);
      cbmCustom.Properties.Items.Add(name);
      clbTestAgainst.AddItem(name);
      AppendLine(sLocalDir + sCustomDecks,line);
    end;
    slr.Free;
  end;
end;

procedure TEvaluateDecksForm.bSaveBotCustomDeckClick(Sender: TObject);
var
  i: integer;
  name,line: string;
begin
  name := InputBox('Deck name','Input deck name','');
  if name <> '' then
  begin
    with TStringList.Create do
    try
      Add(imgBot.Hint);
      for i := 0 to MAX_DECK_SIZE - 1 do
        if (Assigned(BotDeck[i])) then
          if (BotDeck[i].Hint <> '') then
            Add(BotDeck[i].Hint);
      line := name + ':' + StringReplace(CommaText,'"','',[rfReplaceAll]);
      if InsertCustomDeck(line) then
      begin
        cbCustom.Properties.Items.Add(name);
        cbmCustom.Properties.Items.Add(name);
        clbTestAgainst.AddItem(name);
        AppendLine(sLocalDir + sCustomDecks,line);
      end;
    finally
      Free;
    end;
  end;
end;

procedure TEvaluateDecksForm.bSaveTopClick(Sender: TObject);
var i, id: integer;
slr: TStringList;
name, line: string;
begin
  name := InputBox('Deck name','Input deck name','');
  if name <> '' then
  begin
    slr := TStringList.Create;
    with vTop.DataController do
      for i := 0 to RecordCount - 1 do
        if (not VarIsNull(Values[i,vTopName.Index])) and (not VarIsNull(Values[i,vTopID.Index])) then
        begin
          id := values[i,vTopID.Index];
          if Cards[remapminidinversed[id]].CardType = 1 then
            slr.Insert(0,Values[i,vTopName.Index])
          else
            slr.Add(Values[i,vTopName.Index]);
        end;

    line := name + ':' + StringReplace(slr.CommaText,'"','',[rfReplaceAll]);
    if InsertCustomDeck(line) then
    begin
      cbCustom.Properties.Items.Add(name);
      cbmCustom.Properties.Items.Add(name);
      clbTestAgainst.AddItem(name);
      AppendLine(sLocalDir + sCustomDecks,line);
    end;
    slr.Free;
  end;
end;

procedure TEvaluateDecksForm.bSaveTopDeckClick(Sender: TObject);
var
  i: integer;
  name,line: string;
begin
  name := InputBox('Deck name','Input deck name','');
  if name <> '' then
  begin
    with TStringList.Create do
    try
      Add(imgTop.Hint);
      for i := 0 to MAX_DECK_SIZE - 1 do
        if (Assigned(TopDeck[i])) then
          if (TopDeck[i].Hint <> '') then
            Add(TopDeck[i].Hint);
      line := name + ':' + StringReplace(CommaText,'"','',[rfReplaceAll]);
      if InsertCustomDeck(line) then
      begin
        cbCustom.Properties.Items.Add(name);
        cbmCustom.Properties.Items.Add(name);
        clbTestAgainst.AddItem(name);
        AppendLine(sLocalDir + sCustomDecks,line);
      end;
    finally
      Free;
    end;
  end;
end;

procedure TEvaluateDecksForm.bTopVisualClick(Sender: TObject);
var
  i,id: integer;
begin
    ClearDeck(TopDeck);
    with vTop.DataController do
      for i := 0 to RecordCount - 1 do
        if (not VarIsNull(Values[i,vTopName.Index])) and (not VarIsNull(Values[i,vTopID.Index])) then
        begin
          id := values[i,vTopID.Index];
          if Cards[remapminidinversed[id]].CardType = 1 then
          begin
            CopyCard(Images[id],imgTop);
          end
          else
            AddCard(TopDeck,true,Images[id]);
        end;
end;

procedure TEvaluateDecksForm.cbAllowMorePropertiesChange(Sender: TObject);
var
  i: integer;
begin
  if not cbAllowMore.Checked then
  begin
    for i := 0 to MAX_DECK_SIZE - 1 do
      if (Assigned(TopDeck[i])) then
      begin
        if (TopDeck[i].Hint <> '') and (i > 9) then
        begin
          cbAllowMore.Checked := true;
          raise Exception.Create('You have more than 10 cards already.');
          //ErrorText := 'You have more than 10 cards already.';
          //Error := true;
          break;
        end;
      end else break;
  end;
end;

procedure TEvaluateDecksForm.cbmUseRaidClick(Sender: TObject);
begin
  bmCustomDefence.Enabled := not cbmUseRaid.Checked;
  bmMission.Enabled := not cbmUseRaid.Checked;
  cbSurge.Enabled := not cbmUseRaid.Checked;
  if cbmUseRaid.Checked then
    cbSurge.Checked := false;
  gBot.Enabled := not cbmUseRaid.Checked;
end;

procedure TEvaluateDecksForm.cbRaidPropertiesChange(Sender: TObject);
begin
  if cbUseRaid.Checked then
    LoadRaidCommander;
end;

procedure TEvaluateDecksForm.cbRandomSeedClick(Sender: TObject);
begin
  seSeed.Properties.ReadOnly := cbRandomSeed.Checked;
  if seSeed.Properties.ReadOnly then
    seSeed.Style.Color := clBtnFace
  else
    seSeed.Style.Color := clWindow;
end;

procedure TEvaluateDecksForm.cbRarityPropertiesChange(Sender: TObject);
begin
  UpdateFilter;
end;

procedure TEvaluateDecksForm.cbSkillPropertiesChange(Sender: TObject);
begin
  UpdateFilter;
  cbSkillTargetFaction.Enabled := cbSkill.ItemIndex > 0;  
end;

procedure TEvaluateDecksForm.cbUseHiddenClick(Sender: TObject);
var i,k: integer;
begin
  k := 0;
  (vTopName.Properties as TcxComboBoxProperties).Items.Clear;
  for i := 0 to CardsLoaded - 1 do
    if cbUseHidden.Checked or (Cards[i].CardSet > 0) then
    begin
      if Cards[i].CardSet = 0 then
        (vTopName.Properties as TcxComboBoxProperties).Items.Add(Cards[i].Name + '['+IntToStr(Cards[i].Id)+']')
      else
        (vTopName.Properties as TcxComboBoxProperties).Items.Add(Cards[i].Name);
      RemapMinID[k] := i;
      RemapMinIDInversed[i] := k;
      inc(k);
    end;
end;

procedure TEvaluateDecksForm.cbUseRaidClick(Sender: TObject);
begin
  bCustomDef.Enabled := not cbUseRaid.Checked;
  bMission.Enabled := not cbUseRaid.Checked;
  cbSurge.Enabled := not cbUseRaid.Checked;
  if cbUseRaid.Checked then
    cbSurge.Checked := false;
  if cbUseRaid.Checked then
  begin
    ClearDeck(BotDeck);
    LoadRaidCommander;
  end;
end;

procedure TEvaluateDecksForm.FormCreate(Sender: TObject);
var
  p1 : pchar;
  i: integer;
  fileDate   : Integer;
begin
  randomize();
  sLocalDir := ExtractFilePath(ParamStr(0));
  if SizeOf(TCard) <> GetCardSize() then
  begin
    tLoad.Enabled := false;
    ShowMessage('Interface is incompatible with dll.');
    Application.Terminate;
    Halt;
  end;
  ImageCount := 0;
  iHoverTag := -1;
  slLibIndex := TStringList.Create;
  slIDIndex := TStringList.Create;
  slSkillList := TStringList.Create;
  //slSetList := TStringList.Create;
  slUpdate := TStringList.Create;
  GetMem(p1, cMaxBuffer); // initialize
  try
    GetSkillList(p1, cMaxBuffer);
    slSkillList.CommaText := p1;
  finally
    FreeMem(p1);
  end;
  for I := 0 to slSkillList.Count - 1 do
    if (slSkillList.Strings[i] <> '') then
      cbSkill.Properties.Items.Append(slSkillList.Strings[i]);

  fileDate := FileAge(sLocalDir + sCardsFile);

  // Did we get the file age OK?
  if fileDate > -1 then
    ceLastDate.Date := FileDateToDateTime(fileDate);

  intPicture := TPicture.Create;
  intBitmap := TBitmap.Create;
  intmask := TBitmap.Create;
  intal := TStringList.Create;

  if not DirectoryExists(sLocalDir + sCardsDir) then
    CreateDir(sLocalDir + sCardsDir);
end;

procedure TEvaluateDecksForm.FormDestroy(Sender: TObject);
var i: integer;
begin
  for I := 0 to ImageCount - 1 do
    if Assigned(Images[i]) then
      Images[i].Free;
  for I := 0 to MAX_DECK_SIZE - 1 do
    if Assigned(TopDeck[i]) then
      TopDeck[i].Free;

  intPicture.Free;
  intBitmap.Free;
  intmask.Free;
  intal.Free;

  slLibIndex.Free;
  slIDIndex.Free;
  slSkillList.Free;
  //slSetList.Free;
  slUpdate.Free;
end;

procedure TEvaluateDecksForm.FormMouseWheel(Sender: TObject; Shift: TShiftState;
  WheelDelta: Integer; MousePos: TPoint; var Handled: Boolean);
var
 c: TControl;
 mp: TPoint;
begin
 mp := ScreenToClient(MousePos);
 c := ControlAtPos(mp, false, true);
 //if Assigned(c) then
 //   pLibTop.Caption := c.Name;
 if c = rPanel then
   sbRightMouseWheel(Sender,Shift,WheelDelta,MousePos,Handled);
// if c <> nil then
//   TControl(c).DoMouseWheel(Shift, WheelDelta, mp);
end;

procedure TEvaluateDecksForm.sbRightMouseWheel(Sender: TObject; Shift: TShiftState;
  WheelDelta: Integer; MousePos: TPoint; var Handled: Boolean);
begin
    Handled := true;
    sbRight.VertScrollBar.Position := sbRight.VertScrollBar.Position - WheelDelta;
end;

procedure TEvaluateDecksForm.tCancelHoverTimer(Sender: TObject);
var
  i: integer;
  bLeave: boolean;
begin
  bLeave :=
    ((not Assigned(TopDeck[iHoverTag])) OR (not TopDeck[iHoverTag].MouseInClient)) AND
    ((not Assigned(BotDeck[iHoverTag])) OR (not BotDeck[iHoverTag].MouseInClient));
  if bLeave then
  begin
    tCancelHover.Enabled := false;
    if Assigned(TopDeck[iHoverTag]) then
      if not TopDeck[iHoverTag].MouseInClient then
      begin
        for i := 0 to MAX_DECK_SIZE-1 do
          if not Assigned(TopDeck[i]) then
            break
          else
            TopDeck[i].Left := imgTop.Left + cOffset * i;
      end;
    if Assigned(BotDeck[iHoverTag]) then
      if not BotDeck[iHoverTag].MouseInClient then
      begin
        for i := 0 to MAX_DECK_SIZE-1 do
          if not Assigned(BotDeck[i]) then
            break
          else
            BotDeck[i].Left := imgBot.Left + cOffset * i;
      end;
      iHoverTag := -1;
  end;
end;

procedure TEvaluateDecksForm.teamliquidlogoClick(Sender: TObject);
begin
  ShellExecute(Handle, 'open', 'http://www.teamliquid.net', nil, nil, SW_SHOWNORMAL) ;
end;

function TEvaluateDecksForm.GetRarity(Rarity: integer): string;
begin
  case Rarity of
      0 : result := 'Common';
      1 : result := 'Uncommon';
      2 : result := 'Rare';
      3 : result := 'Unique';
      4 : result := 'Legendary';
      10 : result := 'Story Commander';
      else '';
   end;
end;

function TEvaluateDecksForm.LoadCardImage(C: Pointer; Index: integer): boolean;
var
   K: integer;
    //C: ^TCard;
     ListItem: TListItem;
     card: ^TCard;
begin
  card := C;
  if not Assigned(Cards[Index]) then
  begin
          try
            if (Index >= MAX_CARD_COUNT) then
              Exception.Create('Card count limit exceeded.');
            Cards[Index] := C;
          except
            on E:Exception do
            begin
              ShowMessage('Failed to load card '+card.Name+' from database.'#13'Must terminate application, sorry.'#13+E.Message);
              Application.Terminate;
              Halt;
            end;
          end;
  end;
  try
  //ChangeFileExt(ExtractFileName(card.Picture),'');


          intPicture.LoadFromFile(sLocalDir + sCardsDir + card.Picture);


     intbitmap.Height := IL.Height;
     intbitmap.Width := intPicture.Graphic.Width;
     Brush.Style := bsSolid;
     with intbitmap.Canvas do begin
       brush.Color := GetFactionColor(Cards[index].Faction);

       FillRect(ClipRect);
       //FillRect(RECT(0,0,IL.Width,20)) ;

       Font.Size := 10;
       TextOut(16,2,Cards[Index].Name);

       Brush.Color := clSilver;
       FillRect(RECT(0,170,IL.Width,170+50)) ;

       Draw(0,20,intPicture.Graphic);

       intal.Text := GetAbilityList(Cards[Index]);
       Brush.Style := bsclear;
       Font.Size := 7;
       for k := 0 to intal.Count - 1 do
         TextOut(25,172 + k * 12,intal[k]);


       Font.Size := 16;
       if Cards[Index].CardType = 2 then // assault
         TextOut(7,IL.Height-27,IntToStr(Cards[Index].Attack));
       if (Cards[Index].CardType = 2) OR (Cards[Index].CardType = 3) then // assault or structure
         TextOut(IL.Width-16,0,IntToStr(Cards[Index].Wait));
       if Cards[Index].CardType <> 4 then // not action
         if Cards[Index].Health > 9 then
           TextOut(IL.Width-25,IL.Height-27,IntToStr(Cards[Index].Health))
         else
           TextOut(IL.Width-16,IL.Height-27,IntToStr(Cards[Index].Health));
     end;
            //Bitmap.Canvas.Ellipse(50,50,150,150);
            //Bitmap.Canvas.CopyRect(RECT(0,0,IL.Width,IL.Height),Picture.Bitmap.Canvas,RECT(0,0,Picture.Width,Picture.Height));
            //Bitmap.Canvas.StretchDraw(RECT(0,20,IL.Width,IL.Height+20),Picture.Graphic);
            //Bitmap.Canvas.Draw(0, 0, Picture.Graphic);
            //IL.Add(Bitmap,Bitmap);
       IL.AddMasked(intBitmap,clNone);

// "OLD" images inteface

          if not Assigned(Images[Index]) then
          begin
            Images[Index] := TcxImage.Create(Self);
            Images[Index].Parent := sbRight;
            Images[Index].AutoSize := true;
            Images[Index].Left := (cImageW + cBorder) * ((Index - iOffset) mod 2);
            Images[Index].Top := (cImageH + cBorder) * ((Index - iOffset) div 2);
          end;
          Images[Index].Properties.GraphicClassName := 'TJPEGImage';
          Images[Index].Properties.ReadOnly := true;
          Images[Index].Picture.Bitmap.Assign(intBitmap);
          if card.CardSet = 0 then
            Images[Index].Hint := card.Name + '[' + IntToStr(card.Id) + ']'
          else
            Images[Index].Hint := card.Name;
          Images[Index].ShowHint := true;
          Images[Index].Tag := Index;
          Images[Index].Properties.ShowFocusRect := false;
          Images[Index].OnMouseDown := GenericImageMouseDown;
          Images[Index].Properties.PopupMenuLayout.MenuItems := [];
          if Pos(IntToStr(Cards[Index].CardSet),ccbSets.Text) = 0 then
          begin
            Images[Index].Hide;
            inc(iOffset);
          end;

       // Small
{     intbitmap.Height := ILS.Height;
     intbitmap.Width := intPicture.Graphic.Width div 2;
     Brush.Style := bsSolid;
     with intbitmap.Canvas do begin
       brush.Color := GetFactionColor(cards[index].Faction);

       FillRect(ClipRect);
       //FillRect(RECT(0,0,IL.Width,20)) ;

       Brush.Style := bsclear;
       Font.Size := 6;
       TextOut(4,1,Cards[Index].Name);

       Brush.Color := clSilver;
       FillRect(RECT(0,85,IL.Width,85+25)) ;

       StretchDraw(RECT(0,10,75,85),intPicture.Graphic);

       //al.Text := GetAbilityList(Cards[i]);
       //Font.Size := 7;
       //for k := 0 to al.Count - 1 do
       //  TextOut(15,86 + k * 7,al[k]);


       Font.Size := 9;
       if Cards[Index].CardType = 2 then // assault
         TextOut(4,ILS.Height-15,IntToStr(Cards[Index].Attack));
       if Cards[Index].CardType <> 4 then // not action
         if Cards[Index].Health > 9 then
           TextOut(ILS.Width-15,ILS.Height-15,IntToStr(Cards[Index].Health))
         else
           TextOut(ILS.Width-11,ILS.Height-15,IntToStr(Cards[Index].Health));
     end;
            //Bitmap.Width := ILS.Width;
            //Bitmap.Height := ILS.Height;
            //Bitmap.Canvas.StretchDraw(RECT(0,10,Bitmap.Width,Bitmap.Height+10),Picture.Graphic);
       ILS.AddMasked(intBitmap,clNone);
}

    result := true;
  except
    result := false;
  end;
end;

procedure TEvaluateDecksForm.tLoadTimer(Sender: TObject);
  {procedure LoadCards(Path: string);
  var
    i: integer;
    SR: TSearchRec;
  begin
    i := 0;
    LV.items.BeginUpdate;
    with TStringList.Create do
    if FindFirst(Path + '*.jpg', faAnyFile, SR) = 0 then
    try
      repeat
        if (SR.Attr <> faDirectory) then
        begin
          LoadCardImage(Path + SR.Name,i);

          inc(i);
          //pLibBottom.Caption := 'Cards loaded: '+IntToStr(i);
          ProgressUpdate(i);
        end;
      until FindNext(SR) <> 0;
      FindClose(SR);
      ImageCount := i;
    finally
      LV.items.EndUpdate;
    end;
  end; }
var
  p1 : pchar;
  s: DWORD;
  checked: string;
  it: TcxCheckComboBoxItem;
  //
  i, k: integer;
  Indexes: array[0..MAX_SETS_COUNT] of UINT;
  Sets: array[0..MAX_SETS_COUNT] of TCardSet;
begin
  tLoad.Enabled := false;

  LoadMissionXML(sLocalDir + sMissionsFile);
  LoadRaidXML(sLocalDir + sRaidsFile);

  i := ReLoadCustomDecks(sLocalDir + sCustomDecks);
  if i = -2 then
    ShowMessage('File '+sCustomDecks+' not found.');
  if i >= 0 then
    ShowMessage('Error while loading file '+sCustomDecks+' on line '+IntToStr(i));

  if FileExists(sLocalDir + sCardsFile) then
    LoadCardXML(sLocalDir + sCardsFile,nil,0)
  else
  begin
    if (Application.MessageBox('Your database is empty, dou you want to download database files?'#13'You can download them later, but you won''t be able to do anything until then','Warning',MB_YESNO) = IDYES) then
      bDLXMLClick(Sender);
  end;

  s := GetSets(@Indexes,@Sets,MAX_SETS_COUNT);
  ccbSets.Properties.Items.BeginUpdate;
  ccbSets.Properties.Items.Clear;
  ccbSets.Properties.EditValueFormat := cvfStatesString;
  it := ccbSets.Properties.Items.AddCheckItem('<Hidden>','0');
  it.Tag := 0;
  checked := '0';
  for i := 0 to s - 1 do
  begin
    //slSetList.Values[IntToStr(Indexes[i])] := IntToStr(i);
    it := ccbSets.Properties.Items.AddCheckItem(Sets[i].Name,{Sets[i].Name}IntToStr(Indexes[i]));
    it.Tag := Indexes[i];
    if Sets[i].Visible then
      checked := checked + '1'
    else
      checked := checked + '0';
  end;
  ccbSets.Value := checked;
  ccbSets.Properties.Items.EndUpdate;

  GetMem(p1, cMaxBuffer); // initialize
  GetCustomDecksList(p1,cMaxBuffer);
  cbCustom.Properties.Items.CommaText := p1;
  cbmCustom.Properties.Items.CommaText := p1;
  for i := 0 to cbmCustom.Properties.Items.Count - 1 do
  begin
    clbTestAgainst.AddItem(cbmCustom.Properties.Items[i]);
    clbTestAgainst.Items[i].Checked := true;
  end;
  if cbCustom.Properties.Items.Count > 0 then
  begin
    cbCustom.ItemIndex := 0;
    cbmCustom.ItemIndex := 0;
  end;
  GetRaidDecksList(p1,cMaxBuffer);
  cbRaid.Properties.Items.CommaText := p1;
  cbmRaid.Properties.Items.CommaText := p1;
  if cbRaid.Properties.Items.Count > 0 then
  begin
    cbRaid.ItemIndex := 0;
    cbmRaid.ItemIndex := 0;
  end;
  GetMissionDecksList(p1,cMaxBuffer,true);
  cbMission.Properties.Items.CommaText := p1;
  cbmMission.Properties.Items.CommaText := p1;
  if cbMission.Properties.Items.Count > 0 then
  begin
    cbMission.ItemIndex := 0;
    cbmMission.ItemIndex := 0;
  end;
  FreeMem(p1);

  CardsLoaded := LoadPointers(@Cards,MAX_CARD_COUNT);
  for i := 0 to CardsLoaded - 1 do
  begin
    if Cards[i].CardSet <> 0 then
      slLibIndex.Values[Cards[i].Name] := inttostr(i);
    slIDIndex.Values[IntToStr(Cards[i].Id)] := inttostr(i);
  end;

  //
  vTop.DataController.RecordCount := 11;
  vBot.DataController.RecordCount := 16;

  k := 0;
  (vTopName.Properties as TcxComboBoxProperties).Items.Clear;
  (vBotName.Properties as TcxComboBoxProperties).Items.Clear;
  for i := 0 to CardsLoaded - 1 do
  begin
    if cbUseHidden.Checked or (Cards[i].CardSet > 0) then
    begin
      if Cards[i].CardSet = 0 then
        (vTopName.Properties as TcxComboBoxProperties).Items.Add(Cards[i].Name + '['+IntToStr(Cards[i].Id)+']')
      else
        (vTopName.Properties as TcxComboBoxProperties).Items.Add(Cards[i].Name);
      RemapMinID[k] := i;
      RemapMinIDInversed[i] := k;
      inc(k);
    end;
    if Cards[i].CardSet = 0 then
      (vBotName.Properties as TcxComboBoxProperties).Items.Add(Cards[i].Name + '['+IntToStr(Cards[i].Id)+']')
    else
      (vBotName.Properties as TcxComboBoxProperties).Items.Add(Cards[i].Name);
  end;
end;

procedure TEvaluateDecksForm.tsDecksShow(Sender: TObject);
var s, i: DWORD;
begin
  if ImageCount > 0 then
    exit; // already loaded

  s := GetDBSize();
  if s > 0 then
  begin
    ProgressStart(s,'Loading images ...');

    i := 0;
    try
      while (i < CardsLoaded) do
      begin
          LoadCardImage(Cards[i],i);

          inc(i);
          //pLibBottom.Caption := 'Cards loaded: '+IntToStr(i);
          ProgressUpdate(i);
      end;
    finally
    end;
    ImageCount := i;

    iOffset := 0;
    //LoadCards(sCardsDir);
    ProgressFinish;
    bTopVisual.Enabled := ImageCount > 0;
    bBotVisual.Enabled := ImageCount > 0;
  end;
end;

end.

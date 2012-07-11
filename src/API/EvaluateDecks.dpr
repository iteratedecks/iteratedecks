program EvaluateDecks;

uses
  Forms,
  Windows,
  SysUtils,
  EvaluateDecksSrc in 'EvaluateDecksSrc.pas' {EvaluateDecksForm},
  LoadingSrc in 'LoadingSrc.pas' {LoadingForm},
  Extended in 'Extended.pas',
  EDImports in 'EDImports.pas';

{$R *.res}

function GetAppVersion: string;
var
  Size, Size2: DWord;
  Pt, Pt2: Pointer;
begin
  Size := GetFileVersionInfoSize(PChar(ParamStr(0)), Size2);
  if Size > 0 then
  begin
    GetMem(Pt, Size);
    try
      GetFileVersionInfo(PChar(ParamStr(0)), 0, Size, Pt);
      VerQueryValue(Pt, '\', Pt2, Size2);
      with TVSFixedFileInfo(Pt2^) do
      begin
        Result := IntToStr(HiWord(dwFileVersionMS)) + '.' +
          IntToStr(LoWord(dwFileVersionMS)) + '.' +
          IntToStr(HiWord(dwFileVersionLS)) + '.' +
          IntToStr(LoWord(dwFileVersionLS));
      end;
    finally
      FreeMem(Pt);
    end;
  end;
end;

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.Title := 'Evaluate Decks '+GetAppVersion;
  Application.CreateForm(TEvaluateDecksForm, EvaluateDecksForm);
  Application.CreateForm(TLoadingForm, LoadingForm);
  Application.Run;
end.

program EvaluateDecks;

uses
  Forms,
  EvaluateDecksSrc in 'EvaluateDecksSrc.pas' {EvaluateDecksForm},
  LoadingSrc in 'LoadingSrc.pas' {LoadingForm};

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.Title := 'Evaluate Decks';
  Application.CreateForm(TEvaluateDecksForm, EvaluateDecksForm);
  Application.CreateForm(TLoadingForm, LoadingForm);
  Application.Run;
end.

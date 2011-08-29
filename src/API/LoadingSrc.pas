unit LoadingSrc;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, cxControls, cxContainer, cxEdit, cxProgressBar, ExtCtrls, Menus,
  cxLookAndFeelPainters, StdCtrls, cxButtons;

type
  TLoadingForm = class(TForm)
    pBG: TPanel;
    pText: TPanel;
    cxProgress: TcxProgressBar;
    bStop: TcxButton;
    procedure bStopClick(Sender: TObject);
  private
  public
    bHalt: boolean;
  end;

var
  LoadingForm: TLoadingForm;

implementation

{$R *.dfm}


procedure TLoadingForm.bStopClick(Sender: TObject);
begin
  bHalt := true;
end;

end.

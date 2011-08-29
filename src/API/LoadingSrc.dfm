object LoadingForm: TLoadingForm
  Left = 0
  Top = 0
  BorderStyle = bsNone
  Caption = 'LoadingForm'
  ClientHeight = 47
  ClientWidth = 426
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object pBG: TPanel
    Left = 0
    Top = 0
    Width = 426
    Height = 47
    Align = alClient
    BevelKind = bkFlat
    BevelOuter = bvNone
    TabOrder = 0
    object pText: TPanel
      Left = 0
      Top = 0
      Width = 422
      Height = 22
      Align = alClient
      BevelOuter = bvNone
      Caption = 'Please wait while images are loading ...'
      TabOrder = 0
      object bStop: TcxButton
        Left = 352
        Top = 4
        Width = 68
        Height = 16
        Caption = 'Stop'
        TabOrder = 0
        OnClick = bStopClick
      end
    end
    object cxProgress: TcxProgressBar
      Left = 0
      Top = 22
      Align = alBottom
      Properties.Max = 411.000000000000000000
      Properties.ShowTextStyle = cxtsPosition
      TabOrder = 1
      Width = 422
    end
  end
end

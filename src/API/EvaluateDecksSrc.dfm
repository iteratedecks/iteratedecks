object EvaluateDecksForm: TEvaluateDecksForm
  Left = 0
  Top = 0
  Caption = 'Evaluate Decks'
  ClientHeight = 648
  ClientWidth = 852
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnMouseWheel = FormMouseWheel
  PixelsPerInch = 96
  TextHeight = 13
  object PC: TcxPageControl
    Left = 0
    Top = 0
    Width = 852
    Height = 648
    ActivePage = tsMin
    Align = alClient
    Rotate = True
    ShowFrame = True
    TabOrder = 0
    TabPosition = tpLeft
    ClientRectBottom = 647
    ClientRectLeft = 67
    ClientRectRight = 851
    ClientRectTop = 1
    object tsMin: TcxTabSheet
      Caption = 'Minimalistic'
      ImageIndex = 6
      DesignSize = (
        784
        646)
      object gTop: TcxGrid
        AlignWithMargins = True
        Left = 3
        Top = 24
        Width = 778
        Height = 257
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
        object vTop: TcxGridTableView
          NavigatorButtons.ConfirmDelete = False
          OnCustomDrawCell = vTopCustomDrawCell
          DataController.Summary.DefaultGroupSummaryItems = <>
          DataController.Summary.FooterSummaryItems = <>
          DataController.Summary.SummaryGroups = <>
          DataController.OnRecordChanged = vTopDataControllerRecordChanged
          OptionsView.CellAutoHeight = True
          OptionsView.ColumnAutoWidth = True
          Styles.OnGetContentStyle = vTopStylesGetContentStyle
          object vTopID: TcxGridColumn
            Caption = 'ID'
            DataBinding.ValueType = 'Integer'
            Visible = False
            Options.Editing = False
            Width = 35
          end
          object vTopName: TcxGridColumn
            Tag = 111
            Caption = 'Card name'
            PropertiesClassName = 'TcxComboBoxProperties'
            Properties.DropDownListStyle = lsEditFixedList
            Properties.OnValidate = vTopNamePropertiesValidate
            Width = 150
          end
          object vTopFaction: TcxGridColumn
            Caption = 'Faction'
            Visible = False
            FooterAlignmentHorz = taCenter
            GroupSummaryAlignment = taCenter
            Options.Editing = False
            Width = 90
          end
          object vTopWait: TcxGridColumn
            Caption = 'Wait'
            DataBinding.ValueType = 'Smallint'
            FooterAlignmentHorz = taCenter
            GroupSummaryAlignment = taCenter
            Options.Editing = False
          end
          object vTopAttack: TcxGridColumn
            Caption = 'Attack'
            DataBinding.ValueType = 'Smallint'
            FooterAlignmentHorz = taCenter
            GroupSummaryAlignment = taCenter
            Options.Editing = False
          end
          object vTopHealth: TcxGridColumn
            Caption = 'Health'
            DataBinding.ValueType = 'Smallint'
            FooterAlignmentHorz = taCenter
            GroupSummaryAlignment = taCenter
            Options.Editing = False
          end
          object vTopSkills: TcxGridColumn
            Caption = 'Skills'
            Options.Editing = False
            Width = 230
          end
        end
        object gTopLevel1: TcxGridLevel
          GridView = vTop
        end
      end
      object cbUseHidden: TcxCheckBox
        Left = 3
        Top = 3
        Caption = 'Allow hidden cards'
        Enabled = False
        State = cbsChecked
        TabOrder = 1
        OnClick = cbUseHiddenClick
        Width = 121
      end
      object cbOneLineSkills: TcxCheckBox
        Left = 130
        Top = 3
        Caption = 'Display skills in one line'
        State = cbsChecked
        TabOrder = 2
        Width = 143
      end
      object lmCustom: TcxLabel
        Left = 3
        Top = 287
        Caption = 'Load custom deck: '
      end
      object lmMission: TcxLabel
        Left = 3
        Top = 314
        Caption = 'Load mission deck: '
      end
      object cbmCustom: TcxComboBox
        Left = 97
        Top = 286
        Properties.DropDownListStyle = lsFixedList
        TabOrder = 5
        Width = 182
      end
      object cbmMission: TcxComboBox
        Left = 97
        Top = 313
        Properties.DropDownListStyle = lsFixedList
        TabOrder = 6
        Width = 246
      end
      object cbmUseRaid: TcxCheckBox
        Left = 3
        Top = 340
        Caption = 'Use raid deck for defence:'
        TabOrder = 7
        OnClick = cbmUseRaidClick
        Width = 152
      end
      object cbmRaid: TcxComboBox
        Left = 161
        Top = 340
        Properties.DropDownListStyle = lsFixedList
        TabOrder = 8
        Width = 182
      end
      object bmMission: TcxButton
        Left = 349
        Top = 313
        Width = 73
        Height = 21
        Caption = 'As defence'
        TabOrder = 9
        OnClick = bmMissionClick
      end
      object bmCustomDefence: TcxButton
        Left = 349
        Top = 286
        Width = 73
        Height = 21
        Caption = 'As defence'
        TabOrder = 10
        OnClick = bmCustomDefenceClick
      end
      object bmCustomAttack: TcxButton
        Left = 285
        Top = 286
        Width = 58
        Height = 21
        Caption = 'As attack'
        TabOrder = 11
        OnClick = bmCustomAttackClick
      end
      object gBot: TcxGrid
        AlignWithMargins = True
        Left = 3
        Top = 367
        Width = 778
        Height = 276
        Anchors = [akLeft, akTop, akRight, akBottom]
        TabOrder = 12
        object vBot: TcxGridTableView
          NavigatorButtons.ConfirmDelete = False
          OnCustomDrawCell = vTopCustomDrawCell
          DataController.Summary.DefaultGroupSummaryItems = <>
          DataController.Summary.FooterSummaryItems = <>
          DataController.Summary.SummaryGroups = <>
          DataController.OnRecordChanged = vBotDataControllerRecordChanged
          OptionsView.CellAutoHeight = True
          OptionsView.ColumnAutoWidth = True
          Styles.OnGetContentStyle = vTopStylesGetContentStyle
          object vBotID: TcxGridColumn
            Caption = 'ID'
            DataBinding.ValueType = 'Integer'
            Visible = False
            Options.Editing = False
            Width = 35
          end
          object vBotName: TcxGridColumn
            Tag = 111
            Caption = 'Card name'
            PropertiesClassName = 'TcxComboBoxProperties'
            Properties.DropDownListStyle = lsEditFixedList
            Properties.OnValidate = vBotNamePropertiesValidate
            Width = 150
          end
          object vBotFaction: TcxGridColumn
            Caption = 'Faction'
            Visible = False
            FooterAlignmentHorz = taCenter
            GroupSummaryAlignment = taCenter
            Options.Editing = False
            Width = 90
          end
          object vBotWait: TcxGridColumn
            Caption = 'Wait'
            DataBinding.ValueType = 'Smallint'
            FooterAlignmentHorz = taCenter
            GroupSummaryAlignment = taCenter
            Options.Editing = False
          end
          object vBotAttack: TcxGridColumn
            Caption = 'Attack'
            DataBinding.ValueType = 'Smallint'
            FooterAlignmentHorz = taCenter
            GroupSummaryAlignment = taCenter
            Options.Editing = False
          end
          object vBotHealth: TcxGridColumn
            Caption = 'Health'
            DataBinding.ValueType = 'Smallint'
            FooterAlignmentHorz = taCenter
            GroupSummaryAlignment = taCenter
            Options.Editing = False
          end
          object vBotSkills: TcxGridColumn
            Caption = 'Skills'
            Options.Editing = False
            Width = 230
          end
        end
        object cxGridLevel2: TcxGridLevel
          GridView = vBot
        end
      end
      object bSaveTop: TcxButton
        Left = 428
        Top = 286
        Width = 93
        Height = 21
        Caption = 'Save top deck'
        TabOrder = 13
        OnClick = bSaveTopClick
      end
      object bSaveBot: TcxButton
        Left = 428
        Top = 340
        Width = 93
        Height = 21
        Caption = 'Save bottom deck'
        TabOrder = 14
        OnClick = bSaveBotClick
      end
      object bTopVisual: TcxButton
        Left = 527
        Top = 286
        Width = 93
        Height = 21
        Caption = 'To visual'
        Enabled = False
        TabOrder = 15
        OnClick = bTopVisualClick
      end
      object bBotVisual: TcxButton
        Left = 527
        Top = 340
        Width = 93
        Height = 21
        Caption = 'To visual'
        Enabled = False
        TabOrder = 16
        OnClick = bBotVisualClick
      end
    end
    object tsDecks: TcxTabSheet
      Caption = 'Visual'
      ImageIndex = 0
      OnShow = tsDecksShow
      object rPanel: TPanel
        Left = 434
        Top = 0
        Width = 350
        Height = 646
        Align = alRight
        BevelOuter = bvSpace
        Caption = 'rPanel'
        TabOrder = 0
        object sbRight: TScrollBox
          Left = 1
          Top = 123
          Width = 348
          Height = 522
          HorzScrollBar.Visible = False
          VertScrollBar.Smooth = True
          VertScrollBar.Tracking = True
          Align = alClient
          BevelInner = bvNone
          BevelOuter = bvNone
          BorderStyle = bsNone
          TabOrder = 0
          TabStop = True
          OnMouseWheel = sbRightMouseWheel
        end
        object pLibTop: TPanel
          Left = 1
          Top = 1
          Width = 348
          Height = 122
          Align = alTop
          BevelOuter = bvSpace
          TabOrder = 1
          object cbFaction: TcxComboBox
            Left = 119
            Top = 8
            Properties.DropDownListStyle = lsFixedList
            Properties.Items.Strings = (
              'Any'
              'Imperial'
              'Raider'
              'Bloodthirsty'
              'Xeno')
            Properties.OnChange = cbRarityPropertiesChange
            TabOrder = 0
            Text = 'Any'
            Width = 106
          end
          object cbRarity: TcxComboBox
            Left = 8
            Top = 8
            Properties.DropDownListStyle = lsFixedList
            Properties.Items.Strings = (
              'Any'
              'Common'
              'Uncommon'
              'Rare'
              'Unique'
              'Legendary')
            Properties.OnChange = cbRarityPropertiesChange
            TabOrder = 1
            Text = 'Any'
            Width = 105
          end
          object cbType: TcxComboBox
            Left = 231
            Top = 8
            Properties.DropDownListStyle = lsFixedList
            Properties.Items.Strings = (
              'Any'
              'Commander'
              'Assault'
              'Structure'
              'Action')
            Properties.OnChange = cbRarityPropertiesChange
            TabOrder = 2
            Text = 'Any'
            Width = 106
          end
          object cbWait: TcxComboBox
            Left = 8
            Top = 35
            Properties.DropDownListStyle = lsFixedList
            Properties.Items.Strings = (
              'Any'
              '0 turns'
              '1 turn'
              '2 turns'
              '3 turns'
              '4 turns'
              '6 turns')
            Properties.OnChange = cbRarityPropertiesChange
            TabOrder = 3
            Text = 'Any'
            Width = 65
          end
          object cbSkill: TcxComboBox
            Left = 8
            Top = 62
            Properties.DropDownListStyle = lsFixedList
            Properties.Items.Strings = (
              'Any')
            Properties.OnChange = cbSkillPropertiesChange
            TabOrder = 4
            Text = 'Any'
            Width = 106
          end
          object ceFilter: TcxTextEdit
            Left = 191
            Top = 35
            Properties.OnChange = cbRarityPropertiesChange
            TabOrder = 5
            Width = 146
          end
          object ccbSets: TcxCheckComboBox
            Left = 43
            Top = 89
            Properties.Items = <>
            Properties.OnChange = cbRarityPropertiesChange
            TabOrder = 6
            Width = 299
          end
          object lSet: TcxLabel
            Left = 8
            Top = 91
            Caption = 'Sets:'
          end
          object cbSkillTargetFaction: TcxComboBox
            Left = 120
            Top = 62
            Enabled = False
            Properties.DropDownListStyle = lsFixedList
            Properties.Items.Strings = (
              'Any'
              'Imperial'
              'Raider'
              'Bloodthirsty'
              'Xeno')
            Properties.OnChange = cbRarityPropertiesChange
            TabOrder = 8
            Text = 'Any'
            Width = 106
          end
        end
      end
      object pClient: TPanel
        Left = 0
        Top = 0
        Width = 434
        Height = 646
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 1
        object pTopDeck: TPanel
          Left = 0
          Top = 0
          Width = 434
          Height = 290
          Align = alTop
          BevelOuter = bvSpace
          TabOrder = 0
          object pTopLeft: TPanel
            Left = 1
            Top = 1
            Width = 176
            Height = 288
            Align = alLeft
            BevelOuter = bvSpace
            TabOrder = 0
            object imgTop: TcxImage
              Left = 8
              Top = 8
              Properties.GraphicClassName = 'TJPEGImage'
              Properties.PopupMenuLayout.MenuItems = []
              Properties.ReadOnly = True
              Properties.ShowFocusRect = False
              TabOrder = 0
              OnMouseDown = imgMouseDown
              Height = 220
              Width = 160
            end
            object cbAllowMore: TcxCheckBox
              Left = 8
              Top = 234
              Caption = 'Allow more than 10 cards'
              Properties.OnChange = cbAllowMorePropertiesChange
              TabOrder = 1
              Width = 152
            end
            object bSaveTopDeck: TcxButton
              Left = 8
              Top = 260
              Width = 160
              Height = 21
              Caption = 'Save custom Deck'
              Enabled = False
              TabOrder = 2
              OnClick = bSaveTopDeckClick
            end
          end
          object sbTop: TScrollBox
            Left = 177
            Top = 1
            Width = 256
            Height = 288
            HorzScrollBar.Tracking = True
            VertScrollBar.Tracking = True
            Align = alClient
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = bsNone
            TabOrder = 1
          end
        end
        object pMiddle: TPanel
          Left = 0
          Top = 290
          Width = 434
          Height = 89
          Align = alTop
          BevelOuter = bvSpace
          TabOrder = 1
          object cbCustom: TcxComboBox
            Left = 103
            Top = 5
            Properties.DropDownListStyle = lsFixedList
            TabOrder = 0
            Width = 182
          end
          object cbRaid: TcxComboBox
            Left = 167
            Top = 60
            Properties.DropDownListStyle = lsFixedList
            Properties.OnChange = cbRaidPropertiesChange
            TabOrder = 1
            Width = 182
          end
          object lLoadCustom: TcxLabel
            Left = 9
            Top = 6
            Caption = 'Load custom deck: '
          end
          object bCustomAtk: TcxButton
            Left = 291
            Top = 6
            Width = 58
            Height = 21
            Caption = 'As attack'
            TabOrder = 3
            OnClick = bCustomAtkClick
          end
          object bCustomDef: TcxButton
            Left = 355
            Top = 6
            Width = 73
            Height = 21
            Caption = 'As defence'
            TabOrder = 4
            OnClick = bCustomDefClick
          end
          object cbUseRaid: TcxCheckBox
            Left = 9
            Top = 60
            Caption = 'Use raid deck for defence:'
            TabOrder = 5
            OnClick = cbUseRaidClick
            Width = 152
          end
          object cbMission: TcxComboBox
            Left = 103
            Top = 33
            Properties.DropDownListStyle = lsFixedList
            TabOrder = 6
            Width = 246
          end
          object lMission: TcxLabel
            Left = 9
            Top = 34
            Caption = 'Load mission deck: '
          end
          object bMission: TcxButton
            Left = 355
            Top = 33
            Width = 73
            Height = 21
            Caption = 'As defence'
            TabOrder = 8
            OnClick = bMissionClick
          end
        end
        object pBottom: TPanel
          Left = 0
          Top = 379
          Width = 434
          Height = 267
          Align = alClient
          TabOrder = 2
          object pBottomLeft: TPanel
            Left = 1
            Top = 1
            Width = 177
            Height = 265
            Align = alLeft
            BevelOuter = bvSpace
            TabOrder = 0
            object imgBot: TcxImage
              Left = 8
              Top = 8
              Properties.GraphicClassName = 'TJPEGImage'
              Properties.PopupMenuLayout.MenuItems = []
              Properties.ReadOnly = True
              Properties.ShowFocusRect = False
              TabOrder = 0
              OnMouseDown = imgMouseDown
              Height = 220
              Width = 160
            end
            object bSaveBotCustomDeck: TcxButton
              Left = 8
              Top = 234
              Width = 160
              Height = 21
              Caption = 'Save custom Deck'
              Enabled = False
              TabOrder = 1
              OnClick = bSaveBotCustomDeckClick
            end
          end
          object sbBot: TScrollBox
            Left = 178
            Top = 1
            Width = 255
            Height = 265
            HorzScrollBar.Tracking = True
            VertScrollBar.Tracking = True
            Align = alClient
            BevelInner = bvNone
            BevelOuter = bvNone
            BorderStyle = bsNone
            TabOrder = 1
          end
        end
      end
    end
    object tsEval: TcxTabSheet
      Caption = 'Evaluate'
      ImageIndex = 2
      DesignSize = (
        784
        646)
      object cbIterations: TcxComboBox
        Left = 3
        Top = 3
        Properties.DropDownListStyle = lsFixedList
        Properties.Items.Strings = (
          '100'
          '200'
          '500'
          '1000'
          '2000'
          '5000'
          '10000'
          '20000'
          '50000'
          '100000'
          '200000'
          '500000'
          '1000000')
        TabOrder = 0
        Text = '100000'
        Width = 121
      end
      object bRun: TcxButton
        Left = 130
        Top = 3
        Width = 75
        Height = 21
        Caption = 'Run'
        TabOrder = 1
        OnClick = bRunClick
      end
      object cxGrid: TcxGrid
        AlignWithMargins = True
        Left = 3
        Top = 30
        Width = 772
        Height = 607
        Anchors = [akLeft, akTop, akRight, akBottom]
        PopupMenu = pmGrid
        TabOrder = 2
        object cxView: TcxGridTableView
          NavigatorButtons.ConfirmDelete = False
          DataController.Summary.DefaultGroupSummaryItems = <>
          DataController.Summary.FooterSummaryItems = <>
          DataController.Summary.SummaryGroups = <>
          OptionsView.CellAutoHeight = True
          OptionsView.ColumnAutoWidth = True
          object vcType: TcxGridColumn
            Caption = 'Type'
            OnCustomDrawCell = vcTypeCustomDrawCell
          end
          object vcAtk: TcxGridColumn
            Caption = 'Your deck'
            Width = 273
          end
          object vcDef: TcxGridColumn
            Caption = 'Enemy deck'
            Width = 255
          end
          object vcWins: TcxGridColumn
            Caption = 'Wins'
          end
          object vcGames: TcxGridColumn
            Caption = 'Games'
          end
          object vcRatio: TcxGridColumn
            Caption = 'Ratio'
            PropertiesClassName = 'TcxCurrencyEditProperties'
            Properties.DisplayFormat = ',0.###;-,0.###'
          end
          object vcAvgD: TcxGridColumn
            Caption = 'Avg. Dmg Dealt'
            PropertiesClassName = 'TcxCurrencyEditProperties'
            Properties.DisplayFormat = ',#.##;-,#.##'
            Width = 64
          end
          object vcAvgDA: TcxGridColumn
            Caption = 'Avg. Dmg Dealt (Auto)'
            PropertiesClassName = 'TcxCurrencyEditProperties'
            Properties.DisplayFormat = ',#.##;-,#.##'
            Width = 64
          end
          object vcAvgS: TcxGridColumn
            Caption = 'Avg. Dmg Suffered'
            PropertiesClassName = 'TcxCurrencyEditProperties'
            Properties.DisplayFormat = ',#.##;-,#.##'
            Width = 90
          end
          object vcAvgSA: TcxGridColumn
            Caption = 'Avg. Dmg Suffered (Auto)'
            PropertiesClassName = 'TcxCurrencyEditProperties'
            Properties.DisplayFormat = ',#.##;-,#.##'
            Width = 90
          end
          object vcNet: TcxGridColumn
            Caption = 'Avg. net gain'
            PropertiesClassName = 'TcxCurrencyEditProperties'
            Properties.DisplayFormat = ',#.##;-,#.##'
          end
        end
        object cxGridLevel1: TcxGridLevel
          GridView = cxView
        end
      end
      object cbSurge: TcxCheckBox
        Left = 211
        Top = 3
        Caption = 'Surge'
        TabOrder = 3
        Width = 121
      end
      object bClear: TcxButton
        Left = 271
        Top = 3
        Width = 89
        Height = 21
        Caption = 'Clear history'
        TabOrder = 4
        OnClick = bClearClick
      end
    end
    object tsUpdate: TcxTabSheet
      Caption = 'Update DB'
      ImageIndex = 3
      object bDLXML: TcxButton
        Left = 3
        Top = 3
        Width = 134
        Height = 25
        Caption = 'Download XML sheets'
        TabOrder = 0
        OnClick = bDLXMLClick
      end
      object lUpdateXML: TcxLabel
        Left = 143
        Top = 6
        Caption = 'Last update: '
      end
      object ceLastDate: TcxDateEdit
        Left = 217
        Top = 3
        Properties.ShowTime = False
        TabOrder = 2
        Width = 96
      end
      object bCustom: TcxButton
        Left = 3
        Top = 66
        Width = 134
        Height = 25
        Caption = 'Reload Custom decks'
        TabOrder = 3
        OnClick = bCustomClick
      end
    end
    object tsHelp: TcxTabSheet
      Caption = 'Tips'
      ImageIndex = 1
      DesignSize = (
        784
        646)
      object cxMemo1: TcxMemo
        Left = 3
        Top = 3
        Anchors = [akLeft, akTop, akRight, akBottom]
        Lines.Strings = (
          
            'Use left mouse button to add cards from library to attacking dec' +
            'k'
          
            'Use right mouse button to add cards from library to defending de' +
            'ck'
          
            'To use minimalistic interface instead of visual, clear one of th' +
            'e commander cards in visual interface'
          'You will probably do just fine without my tips ;)'
          ''
          'My kongregate account:'
          'http://www.kongregate.com/accounts/NETRAT'
          ''
          'Project thread:'
          
            'http://www.kongregate.com/forums/65-tyrant/topics/195043-yet-ano' +
            'ther-battlesim-evaluate-decks'
          'or'
          'http://tinyurl.com/tyrantEvalDecks')
        Properties.ReadOnly = True
        TabOrder = 0
        Height = 640
        Width = 778
      end
    end
    object tsSettings: TcxTabSheet
      Caption = 'Settings'
      ImageIndex = 5
      object rgEval: TcxRadioGroup
        Left = 3
        Top = 3
        Caption = 'Evaluation'
        Properties.Items = <>
        TabOrder = 0
        Height = 102
        Width = 190
      end
      object bSlowProgress: TcxRadioButton
        Left = 16
        Top = 24
        Width = 169
        Height = 17
        Caption = 'Slow dll, but with progress bar'
        TabOrder = 1
        OnClick = bFastThreadedClick
      end
      object bFastThreaded: TcxRadioButton
        Left = 16
        Top = 47
        Width = 145
        Height = 17
        Caption = 'Blazing fast executable'
        Checked = True
        TabOrder = 2
        TabStop = True
        OnClick = bFastThreadedClick
      end
      object seThreads: TcxSpinEdit
        Left = 144
        Top = 70
        Properties.MaxValue = 50.000000000000000000
        Properties.MinValue = 1.000000000000000000
        Properties.ReadOnly = False
        TabOrder = 3
        Value = 2
        Width = 41
      end
      object lThreads: TcxLabel
        Left = 91
        Top = 71
        Caption = 'Threads:'
      end
      object gbSeed: TcxGroupBox
        Left = 3
        Top = 111
        Caption = 'Seed'
        TabOrder = 5
        Height = 130
        Width = 190
        object cbRandomSeed: TcxCheckBox
          Left = 13
          Top = 47
          Caption = 'random seed'
          State = cbsChecked
          TabOrder = 0
          OnClick = cbRandomSeedClick
          Width = 90
        end
        object seSeed: TcxSpinEdit
          Left = 13
          Top = 20
          Properties.SpinButtons.Visible = False
          Style.Color = clBtnFace
          TabOrder = 1
          Width = 164
        end
        object cxLabel2: TcxLabel
          Left = 13
          Top = 74
          AutoSize = False
          Caption = 
            'Same seeds will yield same random sequences and results will be ' +
            'the same.'
          Properties.WordWrap = True
          Height = 47
          Width = 164
        end
      end
    end
  end
  object tLoad: TTimer
    Interval = 100
    OnTimer = tLoadTimer
    Left = 336
    Top = 176
  end
  object tCancelHover: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tCancelHoverTimer
    Left = 336
    Top = 224
  end
  object IdHTTP: TIdHTTP
    OnWork = IdHTTPWork
    OnWorkBegin = IdHTTPWorkBegin
    OnWorkEnd = IdHTTPWorkEnd
    AllowCookies = True
    ProxyParams.BasicAuthentication = False
    ProxyParams.ProxyPort = 0
    Request.ContentLength = -1
    Request.ContentRangeEnd = -1
    Request.ContentRangeStart = -1
    Request.ContentRangeInstanceLength = -1
    Request.Accept = 'text/html, */*'
    Request.BasicAuthentication = False
    Request.UserAgent = 'Mozilla/3.0 (compatible; Indy Library)'
    Request.Ranges.Units = 'bytes'
    Request.Ranges = <>
    HTTPOptions = [hoForceEncodeParams]
    Left = 360
    Top = 64
  end
  object IL: TcxImageList
    Height = 210
    Width = 150
    FormatVersion = 1
    DesignInfo = 11534616
  end
  object ILS: TcxImageList
    Height = 105
    Width = 75
    FormatVersion = 1
    DesignInfo = 14680344
  end
  object pmGrid: TPopupMenu
    Left = 400
    Top = 200
    object bDeleteSelected: TMenuItem
      Caption = 'Delete selected'
      OnClick = bDeleteSelectedClick
    end
    object bDeleteAll: TMenuItem
      Caption = 'Delete all'
      OnClick = bDeleteAllClick
    end
  end
end

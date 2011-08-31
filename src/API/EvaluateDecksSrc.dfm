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
          OnKeyDown = vTopKeyDown
          NavigatorButtons.ConfirmDelete = False
          OnCustomDrawCell = vTopCustomDrawCell
          DataController.Summary.DefaultGroupSummaryItems = <>
          DataController.Summary.FooterSummaryItems = <>
          DataController.Summary.SummaryGroups = <>
          DataController.OnRecordChanged = vTopDataControllerRecordChanged
          OptionsData.Deleting = False
          OptionsData.DeletingConfirmation = False
          OptionsData.Inserting = False
          OptionsView.CellAutoHeight = True
          OptionsView.ColumnAutoWidth = True
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
          OnKeyDown = vBotKeyDown
          NavigatorButtons.ConfirmDelete = False
          OnCustomDrawCell = vTopCustomDrawCell
          DataController.Summary.DefaultGroupSummaryItems = <>
          DataController.Summary.FooterSummaryItems = <>
          DataController.Summary.SummaryGroups = <>
          DataController.OnRecordChanged = vBotDataControllerRecordChanged
          OptionsData.Deleting = False
          OptionsData.DeletingConfirmation = False
          OptionsData.Inserting = False
          OptionsView.CellAutoHeight = True
          OptionsView.ColumnAutoWidth = True
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
              'Xeno'
              'Righteous')
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
              'Xeno'
              'Righteous')
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
            DataBinding.ValueType = 'Integer'
          end
          object vcGames: TcxGridColumn
            Caption = 'Games'
            DataBinding.ValueType = 'Integer'
          end
          object vcRatio: TcxGridColumn
            Caption = 'Ratio'
            DataBinding.ValueType = 'Float'
            PropertiesClassName = 'TcxCurrencyEditProperties'
            Properties.DisplayFormat = ',0.###;-,0.###'
          end
          object vcAvgD: TcxGridColumn
            Caption = 'Avg. Dmg Dealt'
            DataBinding.ValueType = 'Float'
            PropertiesClassName = 'TcxCurrencyEditProperties'
            Properties.DisplayFormat = ',#.##;-,#.##'
            Width = 64
          end
          object vcAvgDA: TcxGridColumn
            Caption = 'Avg. Dmg Dealt (Auto)'
            DataBinding.ValueType = 'Float'
            PropertiesClassName = 'TcxCurrencyEditProperties'
            Properties.DisplayFormat = ',#.##;-,#.##'
            Width = 64
          end
          object vcAvgS: TcxGridColumn
            Caption = 'Avg. Dmg Suffered'
            DataBinding.ValueType = 'Float'
            PropertiesClassName = 'TcxCurrencyEditProperties'
            Properties.DisplayFormat = ',#.##;-,#.##'
            Width = 90
          end
          object vcAvgSA: TcxGridColumn
            Caption = 'Avg. Dmg Suffered (Auto)'
            DataBinding.ValueType = 'Float'
            PropertiesClassName = 'TcxCurrencyEditProperties'
            Properties.DisplayFormat = ',#.##;-,#.##'
            Width = 90
          end
          object vcNet: TcxGridColumn
            Caption = 'Avg. net gain'
            DataBinding.ValueType = 'Float'
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
        Left = 375
        Top = 3
        Width = 89
        Height = 21
        Caption = 'Clear history'
        TabOrder = 4
        OnClick = bClearClick
      end
      object bEvalExport: TcxButton
        Left = 270
        Top = 3
        Width = 99
        Height = 21
        Caption = 'Export to Excel'
        TabOrder = 5
        OnClick = bEvalExportClick
      end
    end
    object tsBatch: TcxTabSheet
      Caption = 'Batch Eval'
      ImageIndex = 6
      DesignSize = (
        784
        646)
      object clbTestAgainst: TcxCheckListBox
        Left = 3
        Top = 3
        Width = 230
        Height = 118
        Items = <>
        TabOrder = 0
      end
      object cbBIterations: TcxComboBox
        Left = 239
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
        TabOrder = 1
        Text = '10000'
        Width = 121
      end
      object bBRun: TcxButton
        Left = 366
        Top = 3
        Width = 75
        Height = 21
        Caption = 'Run'
        TabOrder = 2
        OnClick = bBRunClick
      end
      object gBatchResult: TcxGrid
        AlignWithMargins = True
        Left = 3
        Top = 127
        Width = 772
        Height = 510
        Anchors = [akLeft, akTop, akRight, akBottom]
        TabOrder = 3
        object vBatchResult: TcxGridBandedTableView
          NavigatorButtons.ConfirmDelete = False
          DataController.Summary.DefaultGroupSummaryItems = <>
          DataController.Summary.FooterSummaryItems = <
            item
              Kind = skAverage
              Column = vcbFNet
            end
            item
              Kind = skAverage
              Column = vcbFAvgSA
            end
            item
              Kind = skAverage
              Column = vcbFAvgS
            end
            item
              Kind = skAverage
              Column = vcbFAvgDA
            end
            item
              Kind = skAverage
              Column = vcbFAvgD
            end
            item
              Kind = skAverage
              Column = vcbFRatio
            end
            item
              Kind = skSum
              Column = vcbFGames
            end
            item
              Kind = skSum
              Column = vcbFWins
            end
            item
              Kind = skCount
              Column = vcbAgainst
            end
            item
              Kind = skSum
              Column = vcbFLoss
            end
            item
              Kind = skAverage
              Column = vcbSNet
            end
            item
              Kind = skAverage
              Column = vcbSRatio
            end
            item
              Kind = skAverage
              Column = vcbSAvgD
            end
            item
              Kind = skAverage
              Column = vcbSAvgDA
            end
            item
              Kind = skAverage
              Column = vcbSAvgS
            end
            item
              Kind = skAverage
              Column = vcbSAvgSA
            end
            item
              Kind = skSum
              Column = vcbSWins
            end
            item
              Kind = skSum
              Column = vcbSLoss
            end
            item
              Kind = skSum
              Column = vcbSGames
            end>
          DataController.Summary.SummaryGroups = <>
          OptionsCustomize.BandMoving = False
          OptionsView.CellAutoHeight = True
          OptionsView.ColumnAutoWidth = True
          OptionsView.Footer = True
          OptionsView.GroupByBox = False
          OptionsView.Header = False
          OptionsView.HeaderAutoHeight = True
          Bands = <
            item
              Caption = 'Against deck'
              Width = 204
            end
            item
              Caption = 'Fight'
            end
            item
              Caption = 'Wins'
              Position.BandIndex = 1
              Position.ColIndex = 0
            end
            item
              Caption = 'Loss'
              Position.BandIndex = 1
              Position.ColIndex = 1
            end
            item
              Caption = 'Games'
              Position.BandIndex = 1
              Position.ColIndex = 2
            end
            item
              Caption = 'Win ratio'
              Position.BandIndex = 1
              Position.ColIndex = 3
            end
            item
              Caption = 'Avg. Dmg Dealt'
              Position.BandIndex = 1
              Position.ColIndex = 4
              Width = 67
            end
            item
              Caption = 'Avg. Dmg Dealt (Auto)'
              Position.BandIndex = 1
              Position.ColIndex = 5
              Width = 61
            end
            item
              Caption = 'Avg. Dmg Suffered'
              Position.BandIndex = 1
              Position.ColIndex = 6
            end
            item
              Caption = 'Avg. Dmg Suffered (Auto)'
              Position.BandIndex = 1
              Position.ColIndex = 7
            end
            item
              Caption = 'Avg. net gain'
              Position.BandIndex = 1
              Position.ColIndex = 8
            end
            item
              Caption = 'Surge'
            end
            item
              Caption = 'Wins'
              Position.BandIndex = 11
              Position.ColIndex = 0
            end
            item
              Caption = 'Loss'
              Position.BandIndex = 11
              Position.ColIndex = 1
            end
            item
              Caption = 'Games'
              Position.BandIndex = 11
              Position.ColIndex = 2
            end
            item
              Caption = 'Win ratio'
              Position.BandIndex = 11
              Position.ColIndex = 3
            end
            item
              Caption = 'Avg. Dmg Dealt'
              Position.BandIndex = 11
              Position.ColIndex = 4
            end
            item
              Caption = 'Avg. Dmg Dealt (Auto)'
              Position.BandIndex = 11
              Position.ColIndex = 5
            end
            item
              Caption = 'Avg. Dmg Suffered'
              Position.BandIndex = 11
              Position.ColIndex = 6
            end
            item
              Caption = 'Avg. Dmg Suffered (Auto)'
              Position.BandIndex = 11
              Position.ColIndex = 7
            end
            item
              Caption = 'Avg. net gain'
              Position.BandIndex = 11
              Position.ColIndex = 8
            end>
          object vcbAgainst: TcxGridBandedColumn
            Position.BandIndex = 0
            Position.ColIndex = 0
            Position.RowIndex = 0
            IsCaptionAssigned = True
          end
          object vcbFWins: TcxGridBandedColumn
            DataBinding.ValueType = 'Integer'
            Position.BandIndex = 2
            Position.ColIndex = 0
            Position.RowIndex = 0
            IsCaptionAssigned = True
          end
          object vcbFLoss: TcxGridBandedColumn
            DataBinding.ValueType = 'Integer'
            Position.BandIndex = 3
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbFGames: TcxGridBandedColumn
            DataBinding.ValueType = 'Integer'
            Position.BandIndex = 4
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbFRatio: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 5
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbFAvgD: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 6
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbFAvgDA: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 7
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbFAvgS: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 8
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbFAvgSA: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 9
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbFNet: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 10
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbSWins: TcxGridBandedColumn
            DataBinding.ValueType = 'Integer'
            Position.BandIndex = 12
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbSLoss: TcxGridBandedColumn
            DataBinding.ValueType = 'Integer'
            Position.BandIndex = 13
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbSGames: TcxGridBandedColumn
            DataBinding.ValueType = 'Integer'
            Position.BandIndex = 14
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbSRatio: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 15
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbSAvgD: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 16
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbSAvgDA: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 17
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbSAvgS: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 18
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbSAvgSA: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 19
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
          object vcbSNet: TcxGridBandedColumn
            DataBinding.ValueType = 'Float'
            Position.BandIndex = 20
            Position.ColIndex = 0
            Position.RowIndex = 0
          end
        end
        object cxGridLevel3: TcxGridLevel
          GridView = vBatchResult
        end
      end
      object lNote: TcxLabel
        Left = 239
        Top = 30
        Caption = 'This simulations always run in blazing fast mode'
      end
      object bBEvalExport: TcxButton
        Left = 239
        Top = 96
        Width = 130
        Height = 25
        Caption = 'Export to Excel'
        TabOrder = 5
        OnClick = bBEvalExportClick
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
      object xgmlogo: TcxImage
        Left = 440
        Top = 368
        Cursor = crHandPoint
        Hint = 'xgm.ru'
        Anchors = [akRight, akBottom]
        ParentShowHint = False
        Picture.Data = {
          0A544A504547496D61676554610000FFD8FFE000104A46494600010100000100
          010000FFDB004300010101010101010101010101010202030202020202040303
          0203050405050504040405060706050507060404060906070808080808050609
          0A09080A07080808FFDB00430101010102020204020204080504050808080808
          0808080808080808080808080808080808080808080808080808080808080808
          08080808080808080808080808FFC0001108012C019003012200021101031101
          FFC4001F0000010501010101010100000000000000000102030405060708090A
          0BFFC400B5100002010303020403050504040000017D01020300041105122131
          410613516107227114328191A1082342B1C11552D1F02433627282090A161718
          191A25262728292A3435363738393A434445464748494A535455565758595A63
          6465666768696A737475767778797A838485868788898A92939495969798999A
          A2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6
          D7D8D9DAE1E2E3E4E5E6E7E8E9EAF1F2F3F4F5F6F7F8F9FAFFC4001F01000301
          01010101010101010000000000000102030405060708090A0BFFC400B5110002
          0102040403040705040400010277000102031104052131061241510761711322
          328108144291A1B1C109233352F0156272D10A162434E125F11718191A262728
          292A35363738393A434445464748494A535455565758595A636465666768696A
          737475767778797A82838485868788898A92939495969798999AA2A3A4A5A6A7
          A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE2E3
          E4E5E6E7E8E9EAF2F3F4F5F6F7F8F9FAFFDA000C03010002110311003F00FEFE
          28A28A0028A28A0028A28A0028A28A0028A28A0028A090064900547E6C7FDEA0
          0928A01046410451400514514005145140051451400514514005145140051451
          4005145140051451400514514005145140051451400514523305196381400B45
          47E6C7FDEFD29E08201072280168A28A0028A28A0028A28A0028A28A0028A28A
          0028A28A0028A28A0028A28A0028A28A00290900124E052D55B99422919E0726
          813656B9BB58C65C803D3D2BCC8FC45B5BBF8870F8074CFF0049BBB6D3FF00B5
          354914656CE37631C11B1ECF2B2CCC39CED81B8E41147C7FE37D2BC29A36B3AF
          EB57A961A3D8DACB7B773B02C2086342EEE42824E1549C004F15F347ECC4FAC6
          ABA778B3E27F896DA6B2F1278C759935630CB1AF9965608AB059DBF98A489116
          288481B8CF9C4ED1935B469697672CEAFBD63EF7B79776D20E54D5EAC1D30978
          1327078ADEAC4E983D028A28A0A0A28A2800A28A2800A28A2800A28A2800A28A
          2800A28A2800A28A2800A28A2800A28A2800A28A2800ACEB89B68672781D2B46
          B9ED5372C0FED9A691337A1E636DF118DAFC4BD47E1F6AF1A4125C69FF00DAFA
          3CC1891770232C77119F9400F13BC2C464E56743D881EB56B76B2287420FA8F5
          AF827F6999751D06C3C37F17740B39EFFC4BE0CBF6D4A2B68CA2B5FD94A860BA
          B6323B048D191C48D210C57C8040CE2BE9DF00F8D74DF1468FA3EBBA4DE477DA
          55F5AC5796D32F49A19143230F62181FC6B5953D2E8E5A75B5B1EDA082010722
          96AADB4A1D40CF079156AB13AD30A28A28185145140051451400514514005145
          14005145140051451400514514005731AE5E8B7B691C9C1C66BA494E11BA7A57
          9278F3545B5B49BE7C000D5415D99559591F9CDFB70FC45D4A0F07E9DE01F0F4
          D09F107886FD2DA2567C1F2D1D0E40CFCCAD335A44C3FB93374EB5F5C7C2ED3A
          D34BD3B49D234CB786D34DB4823B6B786350A91448A155540E80050315F979F1
          7359B6F1BFED4FE00D0C5E1BAB6D3EEEDADDA3643FE87731A49A93907A36F582
          C867B6DEB918AFD58F85EA1A28D9BAD774E3689E4509F3546CFA4F4D8F6C510C
          7BD6BD51B3C0551C7DDABD5E79ED456814514505051451400514514005145140
          0514514005145140051451400514514005145140051451400514514005636A71
          EE86518F7AD9AA17A0157079F96844CF63E3BF8C7A2DA7887C3FE24F0D5FA93A
          7EA36571633AE7198E58D91BF4635F2F7EC37F12EFB52F002F83F5B9FF00E279
          A14E2D5E36932EB0B8DE831801511CCF6EAA3A2DB81D8D7D87F14136473C800C
          735F963F0675EB4F87FF00B49F8FFC2CDA841E5EADA85C0FF544C93DD483FB4A
          34DE0FCAB1C7757830473C723A1F4611BC753C5AD3E5A899FBB3A1DE8B8B689C
          367806BA6AF22F016A82EECE1C1CE40AF5A88E517A7A5704D59D8F5E94AE87D1
          451526A145145001451450014514500145145001451450014514500145145005
          6BA6DB1FE66BE5EF8B7AB1B7B69D7711C1AFA6EFDB113F6C2D7C53F1B2ECAC53
          0048EBD2BA30D1BC8E1C6CED167E4A7C23F11378C3F684D6759D4AE16FE62DAE
          4F672118C431DEC76F0903DA0755CF5C1F735FABB65E36B1F0BDB780EC67BB96
          D2EF5AF1059E956AE8BBBE7F9AE195BD03476D2AE7B6EAFC58FD992E5A4F8A1A
          05C8C843E1CBFCFB16B9B23FD0D7E8BF8EF5B963F12FECD9089105BA78DA19E5
          0DFC4459DCC4B8FC6707F0AEDAB1BC4F1F0952D23F596C2E44B1A953F30ADD56
          0CA18579A785EFFCF8D32C0935E8F09253F1AF2E4AC7D0D295D12D145148D428
          A28A0028A28A0028A28A0028A28A0028A28A0028A28A0028A28A0028A28A0028
          A28A0028A28A006BB6D52D5CFEA576B0C4D92031ADBB820281F8D796F8B352F2
          2393E6C7E355157665565647CACFE3BB2F19787BC5135A1B8FF897EBFAAE90E6
          5E0B4905D48848FF00678E3DABF273E20EAE7C3FFB4758EAB6CED05E27887469
          1993AEC9D62B4627D8A348A7DABEAEF81DE28BAD4ECFF6918AE26DF0C1F157C4
          3040B9FB91EE85B03FE04CE7F135F0F7C63BA27E38DCDD8C9097DA14407A94B8
          47FF00DA82BD4A30B44F9DC554BC8FDF3F849AA79F6B07CD9E057D3B6CD94FD6
          BE31F82772CD6F0673C57D9364C0C6BEE05706223691ED60A578A65DA28A2B03
          B428A28A0028A28A0028A28A0028A28A0028A28A0028A28A0029AEC1064D3AA9
          DDBED04F6033409BB1CA78CBC47A7F867C3DAEF887569D6DF4EB1B49AF6E643D
          1228D0BB1FC029AF8DFE335D25D5B3CD190D13AEF423B835DEFED4BA95C4BF02
          FE35D8D9AB4B7B3784F588A151C9676B394281F52457C8DE18F880DF12BE0B7C
          3BF154F756B73A8DDE8B6C6F8C2A4247789184B8400F4DB2A4A9FF0001EF5DD8
          586A7938EAAB95A3F30BE03EA29E1DF8B3A5E9174ABE76755D01707FE5B43296
          3FA59495F5CFC79F16DA6813FC0CD6F50D59347D26C7C5B697D7972CBB8436F1
          1F365240E71B11C1C7AF43D2BE2FD656C7C0BFB41412DB0BB105BF8A567FDF29
          C3BEA0A5642A7BAAB6A2F823A14C1E86BDBBF6A88A6D4FC07E149C24B25ADAEB
          6BF6A2A384865B69E1F98F60649215FA915DFCB783F23C08D6B49799FBB1F0FB
          575B88E201F3C0E2BDFED5B7275EC0D7E637EC85F146E3C6FF000D3C0BAEEA77
          AB79ABC966B6FA84BB76892F21630CE4000601962931818C62BF49F48B959A08
          5C1CE462BC9AF0B33EA307554A26E51451581DC1451450014514500145145001
          450480324802A233A0F5228025A2A9B5DA8E9B47E340BB53D4A11ED40B991728
          A856743D722A604119041140EE1451450014514500145145001451450053BC70
          A84FA035F37FC46D59618E501C0FC6BDEF5BBB582DE572C071FA57E5AFEDA7F1
          4A7F08FC32F174DA75D4D06B37918D2AC5E198C52C534EDE5F9B1B0E7746ACF3
          71DA23C8EB5D18785DDCE0C6D5E547C99FB3078861D5BC2BF1835B856EA18F52
          F1CDF6ABB271B6446B8B4B4998303C820C8460F23A57CC3F126FA7D43E39DED8
          C711565F146856C0FF00794FD89CB7E01D87E15EDBFB33B2DAFC32D47508C38F
          ED1D6AF2E0927EF18CADAE47FE02FE95E01E06FB7FC41F8EF61AC3C5FDA5A78F
          115E5FCF227CABF6481658ED25C7A663B23EE48AF5B96D04BB9F312AB793F23F
          73BE1A6BD67E1CF0FEA5AEEA12F9561656B2DDCEDFDD8D14B31FC81AFB3FC31A
          E5A6B9A3E95AB58C9BEDAEADE3B98987F123A8607F222BF253E3078F6D3C05FB
          387C59D66EAE67B50DA1CFA7C72469B9A39AE80B689B1E81E7424F6193DABF41
          BE0B5ECB6DE04F03E993EE5B9B7D26CE09013C865851483F88AF3B150D6E7BF8
          0ABEEA47D268DBD41EFDE9F552D5B70FA8CD5BAE23D64C28A28A061451450014
          514500145145001451450014514500159BA8FF00AB7FF76B4AA8DF26E8DC7AA9
          142265B1F217C5E9566D3F52B1988304D0BC2E08C82ACA41CFE75F8E3FB1A78E
          2F26F879AF7807552D14FA74E97B6B1B7FCB286E54B491E7BB09D2E19B1D3CD5
          CF5AFD84F8D513C56F3B0071C9AFE72B55F10DDFC1FF008F7E31D5AC85D4D0E9
          FAEDD4B75044986B9B0BBC5CBC28A3AED59D1914100BC11E4819C7AD4159293D
          8F98C7CDF338A3D13F6ABB2BF8BC79A45E437B2DB477DA43476B2282459DCDB4
          C5BCCC71F31375111CFF00CB2AF79F11CB6BF14BE11DE3D8C06E66BDB04BCB6B
          7170148BB8889521774DC0159A2547033D18579FFED0F1DA78A3E1BC5E2DD2AE
          E2BC8B4D68F588A585F29716850894EE19DC9E548D28C705A34AF35FD9E7C72B
          637DAAF80AFEE1C4370CDA8695BDFE50D8FDFC0BD87204C1464B1798F4535EA4
          60955E496D23E7E55B9A97B48EF13DD3F612F8B76FA3F89355F064D7D10D3754
          D9ACE94CCC57CC9762ACD1AEEC75458645403276CEC7A1AFE81FC03AF45A8584
          1870D951DEBF93AF1BE9DACFC2EF887249E1EBE974790DD1D6B44B88F3841E68
          778881B432A484A3460E0C4F1827E722BF75BF651FDA0F47F893E15D3758B399
          60B80FF65BEB52FB9ACAE940DF13640271B9583606E4656030C2BCAC4507AC5E
          E8FA1CB7189D9AD99FA911B875073CF7A7D737A2EAB15EC11C88E1B2067DEBA3
          0410083915E5347D3465742D1451414145145001504B3AA6471EE4F6A7CADB50
          E3A9E2B84F12EB6BA6DBCAC5B0AA0934D2B9139D91B17FADDBDB025E45623DEB
          C6F5CF8DBE0CD33C4969E0FB8F14E85078AAE216B8B7D35AED05CCD10CE5D622
          771185739C7F0B7F74E3F2BFE287ED7BE2EFDA07E1D6B1AD7ECB5E25D4FC2DAD
          E8974B797B6DA959C4B73AADB1B7924805BE44C9B649136EC6D8CC63218AAE56
          4F8FBE257C75F07FFC2D6F813FB46786ACE2BA8357B1BE935B9E34CBCA12282D
          43150582CD0ACD382137332C263CB6148EEA584BEACF1B1199A4EC8FD60F027E
          DDDE1BF8AFF0E3E2478FFC05E15F125C4DA0C53BDBE9F7ED1C2FAAB2DBF9D104
          78CCBB0480A820A975CFDC3900F3D79FF0504D13C29F053E1D7C5FF1CF81BC57
          672EBEDB574DD3764E615C48FE779D2B4486378E2F3501C3BAB0C2F0C57F26F5
          6F8F1078335DFDA174AD3AF458E93E2768EFB449F485550B7F3DAC56F2CBB978
          323931CFBC039F2E56639037739E11F8ADA578D2D7E07FC39F1A41A4DF783FC3
          70CD2EA11DFA2084CD6E9716F681C92061618CBB2B0DAEB3C64E41C56FF5457E
          5EBFF06C707F6ACADCDD3FE05CFE8F748FDA23E1E5CF887C39E1097C5DA2DB78
          AF55B2FED0B0D3269C47757306D2DB9626C3744908046488A520111BEDF7CD3F
          5DB6BA0A5245CFB1AFE6C3C0BF157E1D78DBE2FF0089BE3BF89C369BA5785F4F
          B0B6D1A796468E291375CEDBA789465A42D23A4719C9DAD1E5049B427BE7C1DF
          DB27E257C28F8736BF15FF0068DD62FF00C63E1DF11CF6A3C3F61A4585A4779A
          79686E677F346E8A3581A3862281A49241BB0ED9DD8C6AE12CAE8EEC3E669BB4
          B43F7DA39D5C64918EC4558AF31F06789EDF5FD32C350B6766B5B889268CB0C1
          DAC01190790707A57A544C5939E48E2B81AB1ED42774494514522C28A28A002A
          295C229E706A4660A327A5725AFEB30D85B4AEF2056C7AF4A695F42652B2B9C0
          FC45F1245A7D84FF00BC030A7BD7F3CDFB6B7C4FB8F157C43B0F095B2DC4FA5E
          8ABF6B91147375A84C9B51501C12C913903070C6E48EABC7E817ED8DFB4959FC
          39F0CDD1B29EDEEFC5179BEDF49B3639F366C7323804110C790CE723B283B9D0
          1FC6AF847E1CB8F19F8EADEF75292E2FEC74D9C6AFA85D4C326F2F4B978C3118
          06469774EC467063008C480D7AB87A0DDA0B767CB6658D4AF27B23EBEB8907C2
          6F82D359D94B63FDA7A7694C91C815962B8D41C7DFC6490249E4CF5E37D7937E
          C99A3C31EBFAF6A0B04CD0697636FA5DBDC3393BCB9DF2A30E8582C36AD9EBF3
          F6C9CF03F1FF00C7B0EB1E20B2F0BD9DCA7F65E8C5AE6FDC7DD378D1E1573C1F
          DDC4EE4F552665FE28C81EDFF0665D37E1BFC278FC4BE219A2D2D6E217D77529
          0C6D9883A29556500B17489618C8009664E072057A9CAA557963B44F02359C69
          294B7916BF6C4F11DEF8B20F037C1AD1AE621757CED7770BE60044926EB5B556
          5EA559E5B861FED423BE2BF6F7E16DFB5D2C2410462BF9BEF86FAA6AFF0014BE
          3F784354D7A369E7B9D54EAF3C25C30D3E0B64324089DB09225A46481F31667C
          65891FD1AFC1780B5AC0E6BCBAE938B923E8B0127CEA27D6F61CC69FEED68553
          B350B181C70055CAF24FA78EC145145030A28A2800A28A2800A28A2800A28A28
          00A28A2800AA1752AED6C91B403F8D5E6242B11D715C86BD766DAD98838E29A5
          7226EC8F9DFE2CFD8753B2D462B5B9B7B99A1630CCA8E18C526D0DB580E876B2
          9C1ECC0F715FCFAFED83E13B7D13C57A578D228FCA7BE234ABA20F0D222C9244
          DB42F52BE729766FE18940AFD03F8FFF0016E2F807FB53E99E22F106A02CFE14
          F8FF00434B0D52495DCC5A76AD6526C8EED8924468D0DCC31380146D432337EE
          8E7CB7F68BF045A78EBC39AEE88F2ADB9B98C341385DDE44EAC1E3931C676BAA
          363BE307835EE6028F3C5D33E3B38C4724D555D3F23E1FF821F106D1EDBFE157
          6B30C6D6AF1CA74D32B8749D0EE692D7630FE11B9D464829BD40558867C4BC51
          A55F781BC637DA3D95CDC69977A7CE977A4DD46F991206C986405CB16230F136
          FC8731C8082AC41E06F60D574FB896C355B6BDD035BB6986F504892D678DF2AF
          1B91CE194323818236B0C822BDDAE7503F1BFC2F0986386D3E28E868646B742A
          91EA903603F9458F0AE554852479722AAB36C21DFAA85EBD3F63FF002F23B79F
          97A9E3629AC35555F7A53D25D95F67E8CF56B7D46D3E3BFC3A96D1AE2D747F1B
          D99569514B6CB5BB00ED6DA725ADA501B079F94B0C89233B7C9BE11FC5DF14FC
          0BF1EB6B70D8DEA49130B3D774669023CE83E60327E5F3137EF8DB3B595CE182
          CBBC793F84FC73AB7843511AFF0087D2D64BC2861960BA42A268F702D13F1BA3
          6CAF5C655872A7054FD25E26D0B41F8E3E1AB3F19F82EE23B1F155B29B72B70A
          11891F3359DD85CED20B6E571B82EFDE9B9243BED258A873474A91E9DC8F68F0
          553967AD296CFB3FEBFADCFDF1FD9EFF00685F0C7C46F0D697AF681ABC77FA6C
          CA003F75E171F7A3910F28EA782A7906BEDCD2B5886F61478E4524F6CF5AFE34
          7E18FC5DF1FF00C0CF175DEAFE1B4934ED552458356D1EFB724575B46764A173
          B5C0605265CF05586F8D8ABFEFCFECBFFB61F84BE2DE901F4EBC92C75BB6D89A
          869B72409EC9CF43E8F1B6095917E56C11C32B2AF89528F36DA3EC7D8E131BCB
          64DDD7467EAAAB2B0C834EAE0740F15D9EA70C6C932963DF35DBC53A4801C81E
          FD8D70B563DC84D3D89E8A28A4590CFF00707D6BC6FC7F6D33D9DC6CDDD3AD7B
          3C8542302474AE2FC491DB496B22CA573B79ABA6ECCC6B46EAC7E017ED4BF0F2
          FF00E1FEA3A97C5BF84564DE17F142C81F5EB4D3A0416DE20B6F319DA4BAB600
          096646776F3462531BCA0316D9B7F31BC2FE0DF12F89E186FBC2BA0897472D23
          A5DB5C1B6D3212C1433461998B28548E3DF1AC8408950B7C9C7F455F1B746B17
          172C8533CE6BF0D3E29FC22F1C6BBABEB7ABFC51F881E03D2BC236D7323DA4F7
          45A782085A56F2C340FE4451380E8BBF73B1E0166E09FA2A3415449D9BF25A7E
          3D0F82C7E2254A6D7325E6D37F725B9E7BE2BF06D9F84FC2F73A99F893F0EAC3
          5E86DA5BA7D2CB0916E24C962B1482447627E63911166278527AC5E06F0A3F8C
          BC2106BD1F8F7C0FFDB72C4CC9A53FEEBC9712151E64BE63BA860030CC418646
          466A69FC2BF00EC92181BE34EA725C05C48DA69B496276EFB76DBC981EDB8FD4
          D4F2781BE045DE9F335BFC75B08EE9D3F75FDA97BA71543FF4D21F2E2723DB2A
          7DEBB560E17F8236EDCFF89E3FD7EA5BF8B2BDF7E4FC3FAD4E6FC5BE15F14F87
          AD2687C59A55F699A516205EDB5C996C99C828B2798A46C6FDE15469550E5C85
          E49AFD28FD953C23AAFC51D4F42F8C5F12D2CF5CBC4431F86B4FFB385B2D2200
          541B98213BB124A630EB2167609B30C01D89F0EF82FE0EFC45D0C5AEA5F08BC7
          1F0EF56D22E72DE62B496F6374A08187B78C4F1CA0ED642C19580C85652723F7
          17E06693A6DBA59AAAC512280AAA060281C600ED5C95B0EA9A6ECD793D7F15B9
          EB60316EB492E64FCD5D7DE9EC7DE3F0FED664B4877A91D0D7B4DBAE149FC2B8
          7F0BFD952DA3588AFDDE2BBB8994A800F3E95F3751DD9F7D4236449451474EB5
          06E14C775419279F4A866B98E2524B003D4D79DF897C6965A4C12334EAA403C9
          35518B7B19CEA28ABB3A1D6B5EB7B085DE4954100FE15F9F7FB4D7ED37E17F85
          FE1ABED6359BF7C64C56B690156B8BF98F48A14246E6EE4920280CCC555491E2
          DFB577EDBDE1EF85D04BA269CCBE22F1C4F1192D74C8A5DA1139025B89003E54
          79181C16620ED5215CAFE1A78A3C67F107E34F8E1352D6259BC4FE32BC0C9043
          106482CE1046E08A4B082DD72B96249276E4BBB0DDDD468F2BB2D64FA1E16331
          F74DDED15BB3A9F1A78EFC7DF1BFC7B26A178AB7DE24BF664B1B05909B6D2ED8
          1CE0BE01F2D3702F311B9D8F0B968E21EC5E24D734CF825E07B2F0A786A68EEB
          C6178ACCB70C8373CA40125ECC0E7A701539E4468308A4A675B47E1FF801E109
          2F6FCC1AF78DAFFE51B06C6BD940C88D49C98EDA3CE4B1CE371386770ADF2DEA
          9AE6B9E26D625D5B5379358F10DE3A42A9047B77B1388E08509F95016C2824F5
          2CCC49663ECC97D5616DEACBF03E3E1378D9F36D463F8FFC0FEBD3BEF87FE1B8
          7C6FE2CB6D06FA0B9BDD20892FB5590B67CC42D92B23306DCD33B61B3866532B
          0605735E8BF1BFE2447E21BC6F03697347368B633ABEA128E45C5D21CAC20E70
          5622033707F7814654C4EA79ED4F56FF008537E173E10D1EEACEE7E266A63ED1
          AA5F43965D3908C2ECDC3E62A3E58D48009DF2B2F251FC5347D3B50D5AFB48F0
          CF87E012EA5752ADADB07259509C932487392AAAACEDCE48538C922A711FB982
          C3AD6A4B7FF234C24BEB151E2E5A538FC3E76DDFA763EF3FD8D3C1B15F6B1AA7
          8F658CCB3393A559123EE44AE1A76560C43069151082A0AB5B1FEF57EFAFC30B
          CD33468344B2BBBC82DEF2F1CC16B1B361A7758DA42AA3D9518FE15F969F047C
          3DA27807C3FA65946F0D868DA75BFCD34C5500551979646E0649DCECC7A9249E
          B5DC7EC9DF13A6F8F9FB447C44F8CF6B35F9F879E1CD2D3C1FE144990A248D33
          A5C5E5CF96DCA4AE21B339203795244AC1595857363A8F245535D0F6725AFCD2
          755F53F6D2D65560B8C106AF5721A0DD35C5B29639E2BAE539504F715E13563E
          C60EE85A28A291614514500145145001451450014514500145145002119047A8
          AE1FC510B4966E40E403F8577358FAA5B09E19508CE464534F522A46E8FC61FD
          B8FE1DCFF11BE1A788746B5B1FB7F882C5BFB57484DC549BB89586C1C85CC91B
          CD082DF28F37775008FCECFD9B7E3F9F1469167F0CBC67AB0BBD4D22FF00891D
          F4CE375F5B85C8B7724FCF2A28255BAB20E794667FDCBF8E5E1691A1B99A28CE
          704F4AFE6B7F69DF86571F0DBE225DEB560D2DB787758BC6BDB3688BC6F617E0
          F99246B203956DCA678C82187CE14011035EF529B8C556874DCF89C7414A6E84
          FAEDEA7AAFED27F0E2E58C9F103468A4967B48366A56C99CCF6C0E44C8BD0BC7
          96C8182C99E498D14FC8BA5EBB7FA4DFD86B7A2DFC965A8C0DE641346720E460
          823A32303820F047BE08FB2FE0D7C7587C7D6F6FE0EF19DC43078E9108B7B9DA
          A91EB28AA49650301670A0978C603005D0637A45E45F19FE0D5C6813DF78BBC1
          B653DC694EDBEF74DB78B71B53DE585179287AB200482491C642FA58BC27B68A
          C5619FBCB7EFEBEA7CEE0F1EB0F3782C62F75ECDEDAF47E4FA7DC61EB16507C4
          AD36EBC71E11B1587C5D6E9BF5FD0206DEF37FD3DDA2F56CE09298CBF38FDE29
          59785F0778FB58F066AD07887C37730DCA3AAADC5B3C9FB8D461CE423900E08C
          92B2004A1278656747E0F4CD6EEEC2F2C35DD0F5292C3518B12DADDDBB02541E
          E0F21948C65482AC38208AF6382DFC33F18AE23974B9B4DF04FC5970CF7B62C1
          92C35E6032D3407929260172065C61C3071B66AE6A4FEB3255293E5ACBA6DCDE
          6BCFBAEBD0E8AABEA5174ABAE6C3BD9EEE1E4FAF2F6974D9F467BEEB3A4781FE
          3DE936FADE89A9FF006378C2D604566C299ADD5B244175103F3C7BB7ED607860
          FB1B970DF301BBF1AFC32F15C463BCD4BC19E34B33E641716B300C5323F79136
          312C2D81C302A7957504320E245D7887C25E20CFFC4DFC23E2EB2C8E4049A10C
          7F14922629FEDC6FB7F880AFA0AD3E2D7813E226871786FE31DA5A691A8AC98B
          6D5E2529044D8189565E5AD9FEF06DC4C640C3310E631AC9D2C5BE5A9FBBACBB
          E89FAF67FD6A65055F02B9E97EF683EDAB4BBAEEBFAD0FD0AFD9A7FE0A370C0F
          65E1AF8CD35AF87AF9102A6B8A76D8DDFCC07EFD7FE5D9F072589317C8EC5A3C
          AC75FB63E05F8B9A4EBD696D3C17D6F71148A19191C32B83D083D08AFE3DFC7F
          F0E35FF8768B7D3CADAF78518E63D5618C058D4F2A2E0027CB38E37FFAB638E5
          4B041D6FC11FDA47E26FC04BDB71E0CD5BED7E1512879F42BC666B471BB73791
          8E6D9CE5BE64F94B3167490815E462B0F2A73F675D59FF005F7FA9F4F96E690A
          B05570F2E68FF5F73F267F6A161AC5BDD223472AB023A66B51AE102923F5AFC6
          3FD9C7FE0A1BF0DFE2549A6E8175AABF857C67228074AD45823CAFCE45BC9F72
          7FBA58053BF6E0B227207E81CBF1934C6D3CC82F23E99FBD5C4F0B2E9A9EFD2C
          C60D6BA33D975FF15D9E950C8F24CA08073935F9EFFB45FEDA9F0EFE10D9BAEB
          FAE09356951DAD34DB4C4B79798563F2460FCAA4A95F31CAC618A82E0B0CFC67
          FB6BFEDDB3F831EE7C05F0F6EADEF7C7B3A033CEE37C3A344C38771D1E661CA4
          7D003BDF8D892FE1FEAFAF6A7E20D6355F10EBDA8DD6AFAF5F4C6E2F2EE76CCB
          7121EEC4718030028C2AA80AA00000D7D9AA7A3D59C1571B2A9F0E88FB3FE317
          EDBDF177E28DDDCC3A1DC45F0E74124858ED4ADC5E4A33D649E45DAB918F9634
          05493F3B706BE4ABCD5AFB52B95BCD5752D4B57BE08631717B72F7136C2C5B6F
          99212DB7249C6702B95FB5333C50431DCDDDCC8DB228608DA59666C13B511416
          63804E0027835ED5E16F819F107C4B6897B770DA7846D5C1D82FC192E31B54AB
          F9087EE9DCC0ABBA382A4151906BA6861B1188D29A6FF2FF0023C6C663F0D86F
          7AB4927F8FDDB9E6FF006A1CE4E3F1A4FB57BD7D37A5FECAD23464EB1E33D6EE
          E5CF06C2C63B75FCA4331FD68D4FF65695514E8FE33D6AD25FE237F611DCAE3D
          846613FAD777FABB89B74FBD1E5FFADB83BFDAB77E57FF000E7CD16DA85C595D
          2DF58DD5CD85F846896E2DE468A6456C642C8A4300703A1EC3D2BE8AF853FB5B
          7C67F8537B6C6DFC46FE33D0D1943D86B2C657D99CB08AE87EF55CF40D219554
          7F01E2BCF3C51F02BE20F86ED64BCB24B4F17DB201BC59031DC60062CFE439C6
          D1B5405477762D80A719AF115BCDCF3C4CB3413C4E639A2951A392071C94746C
          323723E5201E6B871185C461DDAA26BF2FF23D4C166186C52E6A324DFE2BE5BA
          3FA52FD987F6FAF057C5729A35D4AFE14F18C6407D2AFA74F3275C1FDE5BB838
          993E539C00CBC6E550C85BF50BC31E35B2D62089D2656247635FC368B92B2432
          ABBC73C522CB1488C55E291482AEAC395652010C39046457EBC7EC61FB786AD1
          EA3A6FC36F8AFAB093547658749D62460A2FC9E905C7659BA057E927DD387C79
          BCDCB1A9A6CCF5E962E54B7D51FD2EA5D2B2861B587AE6B1353D7AD6C90B4B2A
          71CF5E2BE5BB7F8E7A58D3564377193B73F7ABF35BF687FF008296781BC38355
          D0FC01769F103C5513795B6D25C595BBEEC379B740152570D948F7B6E015B667
          7085846BE2D11DB53348DBDDD59FA73F143E3CF86BC17A4EA5AAEB5AEE97A2E9
          56E8649EEAEAE1618A141FC4EEC42A8F726BF0BFF68EFF0082896BFE317BFF00
          0E7C1D7BCD26C19B6C9E20BA88091D70722D6DDC1DBC951E64A3230C047CAC83
          F3BFE2AFC69F1FFC62D587883E257890EA6B0132DBDA2030D8E9F80726287710
          1B96FDE396930705C8000D3F077C30BDD774D3E2CF17EA47C09E058879F25DDD
          6D865B98BA65049C4484FF00CB471C8C6D521D5C76E130D3AB2E4A0B5EFDBFC8
          F0733CD69D187B4C44ACBA2EEFB25D4A5E1CD17C53F123C45756DA6C973AB6A5
          24825D4B51BC99A516F903F79712B12CEE40185C967C7650CCBF4DFDBBC0FF00
          B3EF876F6DACE45F1178D6E6349A58DE554B9BF6C9542E403E4DBA92FB460800
          4840772DBBC7F5EF8DBA2F87B4283C29F05B4F8345D3D0BA3EA32DB60F403CC8
          A390664918924CB30392B92B26FDC3C4742D2B5FF17EBEFA6687697DAF6BD72E
          26BA9A4767D80F1E75CCED9DAB85382C492136A06202D7AD09D3C3BF6787F7EA
          BEBD17A77FEBD0F9BA90AD8B5EDB17FBAA0B5B3D1BF5EDFD7A9D0F883C5BAFF8
          A7537D4F5EBDB8D53539E4091450C6C554B10161B78464E3240541B9989E4B33
          127D1A130FC1EB68B52D452CB53F8B9770FF00A2583B0921F0DC0E0832CDB490
          D330C8E0F3CA21D82491F12EB55F0DFC2492EEC3C337D69E2FF8AAAEF0DC6A8F
          16EB4F0F29F95A3814E43CF8CA9EA412E5CA8DB0B78D5C6A0EF34F7B77733DDD
          E4F286965909925B995880093C97763800724F00761585497D564E527CD59FCD
          47FCE5F8236A5078E8A8C5726197C9CFFCA3F8BF43A0B9D52567BCD4752BE9EF
          2EA5669EE6E676DCF339EACC7F4C0000000000000FBAFF00677F86173E1EB37F
          13F88ADC43E24BF8D42C2E837E9D6D9C888B1E77B1DAEE0606551707CBDEDE5F
          F04FE0ACF1DC58F8D3C6D66E978BB65D3B4C997FE3D181C89E61FF003D780553
          FE59F53F3E047A9F1AFE3E0D2A3D47C01F0FB5278F5452D6FA9EAB03906CC838
          782071FF002DBAAB480FEE8E403E664C7D585C22C341E2311F13D975FF008767
          2E2B1EF19516130BF02DDF4D3F45F8F4F3E83F695F8EB3EA10BFC24F87F7B717
          F645BC8D6DF4F06492FE6DDB469D115E5B2D8122AFDE62B113FEB52BF593F639
          F8710FC35F87FE19F09C10C4BA904FB5EA72A6D63717B27CD2B170AA5D54E234
          2C32238E3524EDAFC5AFD917E15CFE32F1C5878BEEEC557C29A2CBB6CD4A0D97
          57E000BB54FF000420EECE31E618F69CC4E07F493F047C28D0DB5BCB247D81E4
          579955B717567BBDBD0FA5C0417B45469EAA3BFA9F5B786E068ECD339CE00C57
          67D3A56669D6E218A340B8007EB5A75E137767DA538D905145148B0A28A2800A
          28A2800A28A2800A28A2800A28A2800A8E44DEB8EFDAA4A2803C77E20785E2D5
          AC275F2F7654D7E38FED3FF03F4EF14E91ACE81ACDACED613FCCB2458592DE40
          7292C648203AB0046410718208241FDDFBFB749A364700AB0FD6BE2BFDA13C2D
          245E1BD6357B0F0F5E7892EE08CCBF62B528B3DC28FBCB16F2AA5F19214B2827
          032339AF572DC5724AD2D8F9CCF32DF6B0BC7747F21BE34F09F883E1F788E7F0
          CF88C183528584F6D756ECD18B9456CA5C42C0EE421941E0EE461D4E031FA7BE
          16FED230DD9B7F0E7C4DB882DA60B8875C6DB1C4F8FE1BA030236C73E60C21E7
          210805BEA1F891E0CF86DF1E3C357B79A45DFF0069DA5BDD4F69E7246D05F687
          7C9F2C90CB148A1E09D0E3743228CFCBB948233F97FF00107E1978CFE1A5DBC7
          AEDAFDBF462D88755B68C8B797E60A048324C2E4B2808C7049C2B3E0E3D8F675
          70AFEB186D60F7F2F5FF0033E2DD6A18C5F54C67BB516DE7E9FAA3EB9F8A5F00
          ACBC42F77E23F02C967A478826733CD6EE48B3BF27259BE507CA90921B7A8218
          EE2CA59F7AFC63AD699AEF867514D33C47A66A9E1AD65584B0AC8DB1F7215612
          412A1C36D250878D8ED6C721860745E00F8DBE37F872B6D63A75C41ADF86108F
          F894DE310880003104A01683A0E0064EA7665893F6368DF11FE0F7C6CB63E18B
          D4B66BF91B29A56AF02C733380C43C07255DC2AB3E6262C8393B4D74F26131DE
          F41F255FCDFF005DBEE38555C7E58B92A47DAD1EFD52FEBA3D3A2678068FF163
          C37E32B4B0F097C64D216FA15DB1D9EBF00227B46E39976FCCB928B974CAB640
          9102AB31C0F18FC23F1478741D5FC398F1D782E41E6417D625649923278F3634
          FBC00DA3CC8F2A7962B18E0765E3BFD9935AB096FB53F00EA6BAAD8E5E45D2AF
          1824D1F522386E3EEBF65025DA7BB486BC2F44F19FC43F84DA95E699A6497BE1
          8BDDFE75CE97A8DA9304C72C3CC31123863BBF7B130DFB07CCC140A9C5292B43
          31837DA6B7F9F7F9EA3C0B836EA64F5127D69CB6F9758BF4D0D4F047C4EF13F8
          2C40DE1AD6A39B406259F4E9809ACA753BB3B53398F25D9B3195DCD82DB80C1B
          5A8DDF81BC4A1AE742B6B7F863AF905DF4FBDB9CE8F305058FD9EE828FB31DA3
          EECAA91642AAECC9277751F177C24F897134DE26D3EEFE13F8E6594349AAE9F1
          7DA6CAEE43B86E9D540241C296775461851E7100D725E20F865E2ED06CA1D6AC
          45978E3C2D21222D5F426375038C919755CB47F74927E645C81BC9ACE542BAA7
          68355A92FBD7CBE28FE5DCD238AC34AB73554F0D5DF7D14BE7F0CFFF004AEC73
          BA8DB5D594E74CD66C24B2BB31ACA6DE75077C649DAEA412B2464A9DB221646C
          65588E6BE8AF047ED73F1A3C11A3DE6867C432F8CB4F303A5A2EB133CB359CA4
          1DAC2E3991D72D92B26F242AAAB462BE4937066D35EC6D2F1EDACA4769D7C9DA
          55647285A54041512308914C806E2B95CE18832C570CB1A2BC825900019B18DC
          7D71DABE7FDBA84B9A936BD7FAD7EE5E87D7AA129C396BA4FD2FFD2FBDFA9D76
          A1AEEABAD5F5DEAFAEEAB7DADEB370DE65CDDDCBEF9277C632C7A76000180000
          0000015D0F823C21AFFC43D79740F0EAA46CAAB2DDDE4AA5A1B0889203B80416
          63860B1820B9079555675F3AB44BBD4AF6C34CD3A1375A95D4F1DB5B44323CC9
          5D82A82403819232DD00C93C035FB07F023E0F58F833C3B65A3C045DB0637179
          7663D86F2E1800D215C9C0C2AAAAE4ED5555C9C66BD0CA32F55E4EA55F817E2F
          B7F99E467D9A4B0F154687F125B792EFFE5FF00A7F09BE03685E16842685A7BB
          5ECAA16E350B9C3DD5C8C8386900002E46762854049214126BED3F03FC1ED3AF
          1D239DA2790603AE412BC679AF853F691FDA92D3E134573F0FBE1C4D6D71F11C
          A2FDA6E4C6B245A146CA183303957B865652919042822471B7624BF9F1F0B3E3
          EFC47F83DF10EEBE25F85B5996F75DBD977EB30DE48CF1EBC9BB256E5B93BB24
          ED946590938CAB3A37A598673CAD52868976E879D94F0FC5AF6D57DE93EAF767
          F52BA17ECF9A2CF6E8CB6F1B1C7A0A4D7BF67ED16081D8DBC4A76FA5701FB35F
          ED7BE0BF8BBE0EB3F12E857CC8B9F26EED27C2DC585C000B43320270C3703904
          860559495604A7ED2DFB5BF84BE13782753F146B177E6220F26D6D6161E7EA17
          0C0EC821527966C13E8AAACEC42AB11E67B4AD7BDF4EE7D33A584E4B5B5EC786
          FC41F861A16912C51BDED8DA4B3CBE45BAC922A99A4DA5B6267EF36158E07385
          27B57C5BF167E05E89E2C88FF6A5988356854ADAEA30AE27B71927693FC71E49
          CA3647391860187E6F7C5EF8D7E3BF8E3E2F93C65E3CD4CDC5D2330D3ECE263F
          67D2222C1847003D0E510B487E6765527015557EC7FD9BBF69FB8F12DCD8FC2F
          F8A77DF6CD4E5C43A3EB33B7CD7671C5B5CB1EB376494FFACE15BF798697D3CB
          F39527EC6A6B17DFA9F339BF0F24BDBD1F764BAADD7F5D4F8CFC69E15D7BC03A
          FCFA06BF12EFF9A4B4BA45222BF841C79899CE08CA878C925091C956477E51E6
          0EBB77C919E086572AC841C821860820E0820E411915FAA3F1D7E0F58F8E7C3B
          71A6BB9B4BE8DBED3A7DD85CFD96E0021491FC48412ACBD4AB1C104061F920D2
          DCC125C5ADEDBBD95F432BDBDC40E72D04C8C55D091C12ACAC38F4AF3B38CB95
          092A94FE096DE4FB7F91E970FE6D2C445D2ADFC48EFE6BA3FF003FF828F72F18
          FC75F8A9E3DD22CB40F11F8CB517D122B416935A5AB1823D47E52ACF7414E656
          70D8642445C0C46A724F99D945737D23DBD8476CAB16C59659E78EDEDAD43676
          F9D3C8563881DAD80C416DAC143118AE56498C91488B2C909652372E372F1D46
          72334C823883D85BC5199A7DFE4DB039772F23FDD4CE4EE676E83A93EB5E646B
          29CEF52EFCBFAFF23D6A945C21CB46D1F37D3F2BFDE7B268DE26F07F83DEDAFA
          C746FF008581E2C8C2BC77B77E641A558C99C8686DDC2CB70E8C17E791631950
          C853A9E77C53E35F10F8C7524BDF12EAF7DAC4EF700DA5A0C98A190960896F6E
          BC6FC3940403230382CC6B42D3E1A6AF6F6306B1E3DD7349F855A2CA1DA2FED7
          07EDF76AB827ECF6208919BA8C36D6070423835D5C1F14BC07F0FAD23B6F84BE
          166D735EDAE1BC45E208FF007A372A731C436BED201568C790015070D935F432
          A155D351C4C951A7FCBD5FFDBBBBF591F230C561D5573C1C1E22B6DCDF657FDB
          DF0AF489B1E17F83D7C2D97C4BF14752B7F87DE104653B279D12EEE893F2A9CE
          5610C4A8C1CCA72576A361841E23F8C96D0E9327833E1569327827C20A191AF1
          4B477B7A781E606CEF8F214932313336E0498D9486F32127C49F8BFAE5B33C7A
          DF8DB5588BAA49E5AA5BD8E402C33F2C3065767A3380BF7CD7D2FE01FD9823B6
          B9B5D57E206B09AA4B190E34CB12CB6DB876966204928C80400231D55838CE77
          C346A548F26021C917BCDEEFE7FA239F193A34A4AAE6B5154A8B6847E15F2EAF
          CE47CD9E12F0A789FC6928B0F0668ADA9A44FE449286115AD990071249D17019
          7E450CF8390A466BEDCF87DF077C2DF0D2DCF8B7C51A8D95E6B96D1492CBA8DD
          388ADB4D4C1DC620C76A6172A6563B882FCAAB14187E29F8FDF0CBE1D5A1F0E7
          83AD2DBC4BA8598FB3C763A60586CAD080708D3805140C152B187653C151CE3E
          37F1D7C53F17FC449567F15EAB09D361732C1616EBE55A5B118C36D24977181F
          3B96232DB768622973E0F01AA7CF53F2FF002FC596E9E6199E935ECA8BFBDAFD
          7F05EA7D13F153F68DBBD5D6FF00C31F0F25BAD2748C1867D59731DC5D03C116
          FF00C512F6F33873C95D98576F1BF865F0F35AF897AFDA7873415FB1699098FE
          DF751A802C2DFF00D8182BE610A55148233C90555AAD7C38F827E35F88B7092C
          B6F79E14F0D070B25F5CC05669C7522DA2700B718C4AC3CBF9811E66196BF4D3
          C196BF0D7E0A5AF85FC3DF63B89355D46E041A4E87A6426EB54D72E36F2523CE
          E73841BA690AC6800DEEA315CEB0F56B3FACE2F48F45DFFE07E676C7154282FA
          9E015E7D5EF6F57D5F96C8FB0BF66CF835A7689A768BA468FA625868F6A8B1C1
          12E4E0772CC725989259989258924924935FAE1E07F0EC7A558C2044AB803F3A
          F0BF813E12BC83C37A1DDEB7A45B68BACC96F1C973691CFE7ADA484026312800
          3ED271B8000E32057D736902C68A00014703EB5E2E638AE79596C7DB64796AA3
          4F5DCB68A1140EFDE9D4515E61F4214514500145145001451450014514500145
          1450014514500145145004530CA678E0D79678CF4DFB4DBCC02E4E33D2BD6080
          4107A5626A362B711B23283E87D6AE12B3B98D68732B1F827FB5E7EC98FE2DD7
          AFFE29FC28D6AE7E1AFC6B8E2006A36CFE5DBEB4100D915FA00C24185540E55B
          0020659163441F9A5A5FED197DE1FF00114DF0F7E3E785E6F0578C6D4AC7717B
          6F19780962DB1A5854B322B0DA0491992363B9B31A8E3FA8BF887E0117F14EE9
          16EC83DABF2DBF68DFD98BC23F12AC27D3FC5BA02DD4B1A48B697D08D97560CD
          FC50CB8C8E4292A728DB46E560315F4D9762E7177A32B3EDD1FF005DCFCFB3EC
          AA9CD72D78F347BADD7F5D99F95DAF7ECE3F0CFC6D64BE21F86DAE5A787E1B81
          BA2974B74BAD366F7584305503D226419EA0D7CC7E2BF815F14FC2F1DDC977E1
          9FF84934B41FEBF4926EFCC0582A8F230262DCE4808CAA01F9B0335F36FED85F
          0C3FE0A85FB16789350F1E7ECB7E1EF027C6DF86729924924D2344BC3ACD880A
          C156FF004D5BB68EF02AB1DB2C68E370663143F2E7E58F813FF070AEABA6DE7F
          C231FB59FC0578AF2095A1B9D5FC1EC619A0652AA525D32EDFEF821CB309D704
          0023AE9AF3C155972E2A0E94FBAD9FE9F87CCF1B0D4332A11E6C1D455A9F67BA
          FD7F1F91FA41E0BF8EDF117C252793A3F8BE7D6EC21251F4FD549BC8D0FA1662
          264233C2890018E410315EF12FED0DF0DFE225AAF877E2BFC3E9ADB4F32978EE
          62717715B10AD89770093C5260903CA562376376335F3F689FF053AFF8260FC7
          8FB1AEBFF13BC29A3EB122C6883C53A2CFA7DC5A33E06D178F1EC423805965DA
          31F7B1CD76B37817F674F8C2F716BFB377ED35F0B2EBC40A1634B5B3D62D3C49
          6D0E178CC30DCA5C1627049698E79E39C8ECC3D1C5415B0B5A3563D9B5FD7E27
          998DC460A6F9B1D879D19FF324F4F3BA4BF265AD5FE1DFC31D70193E14FC59F0
          E35F30678F46D72F04331E842A3B01322850D8F32372C4F2E0735E51E24D03C5
          FF000DEFA3BCD76CB5DF07DCB8548752B79DA24946E6DAB1DE42DB493CB6C0FB
          803C819AF00F8D1FB35FFC14D7C1F34F2FC2AB3FD93BE39E85BD52288DADF691
          A83F072CF6F73786DD54118E2E189CF4AFCFEF107C6AFF0082B8FC1CFB707FD9
          0757F0BDB342CD7571E1CF0FEB12C0D10EBE6CD6178C9B47270CDC579F8CC24E
          2F9A7879539778DDAFBB6FBA48F532DC7D392E4A78C85587F2CECA5F7E8FEF8B
          3F55EE279AF7529758BCBDBCBFD42407CD9A59373DC1279795CFCD23FF00B6E4
          B7BD486E71DEBF9EED5BFE0A75FB4FF857539F49D77E18FC2BF0E6AD12AACB69
          77A36A36F2A753978DAEC1DC724962327B93543FE1EBDFB457FD09BF05FF00F0
          5D7DFF00C995F295AB3736DB6DF9EE7DE61B0BCB4D28A49765B7CB45F91FD64F
          EC89E0C83C61F11EF75BBA8C4EBA34119B7429C7DA27DE8240C1BAAC71CCBB48
          20F9C0F05457ECE6BBE1DD7B45F871AF37855ACAD3C4CD6130D3E5B952628EE4
          C67CB6900C9281F6E7DB35F927FF00044AD73C4FF1AFF663F06FC67F1A5BF876
          DF5FF156ADA95E3C3A6412470DBC76F72F62A8048EEDFF002E4CFF00788CC871
          8AFE8E3C41F0E8DD78536C51127CAEC3DABEBE9558D2A14E9F7577F3D7F2B1F1
          6B0B2C4626B56ECF957FDBBA7E7767F1FF00AC7F6D596B3ACD9F8957508FC4D1
          DD4A352176FBE7FB5162D2348D92199998B160486DDB81208273BED5EA715FA5
          1FB72FECD5E32F12787FC4DAC7C23FF844745F8CD0405F4C9F59B5924B1D4593
          245B5DF94E8E15B955941263241C38050FF1BFE37FF8293FED6DF0DBC5FE23F0
          178EFE17FC28F0B78C748BB92C751D3EEF4BBE596D66438656FF004CC1F50C09
          04104120835E1669974F0ED4F784B67FA3F33DEC9735862EF4F6A91DD7EABCBF
          E18FE8E3E18FC55F15FC27F155BF8A3C27A94D6CCC162BEB5DC7C9D46004FEEE
          55F519628FD50938E19959DF13FE2BF8B7E2DF8A24F1478BAFCDC488AD0D8DAA
          1220D3A0C83B23524FCCC5559DCF2EC07455454FE677FE1EBDFB457FD09BF05F
          FF0005D7DFFC9949FF000F5EFDA2BFE84DF82FFF0082EBEFFE4CAF3BEB93E4F6
          77D0F5BFB3E3ED3DADBDEEFF00D7E67F443F69CF7AB7610EA3AAEA3A7E97A45B
          5DDF6B1733A41690DBB6D964989F9423646D6CE0EEC80B8DC480091FCF1E93FF
          000548FDA7B5FD574CD0B42F877F09F59D6EF6E23B4B3B3B4D2350967BB9DD82
          A45146B7659DD99828500924803935FD7E7FC13CBF678F8A969E15F0BF8E3F68
          3D3BC2B07C6DBF83CDB8D3B4781D6D3C3B13F22DC17965325C630249436DCFC8
          9F286797BF2BCBE589937B463BBFEBAB3CBCEF348E12293D672D22BBF9FA2EBF
          71FA75E1CF0AEBB79F0E7C3F0F892ED758F11C3A7C11DFDDAA80B7372B1A8924
          002A8019831C05039E83A57E36FED59E1387C1DF1496EAD6D12CED758B5374D8
          CFEF6E6260933019C2FCAD6C480065998F24935FD32E85F0E3ECBE12FDEC3FF2
          CBB8F6AFE643FE0BA9F10BC73FB3AFC0CD0FE2D7C39F0CF85B5BD72C3C596D61
          7926AB04B2C769A7DC413872A22923605A78EC97AE3DBA63DFC4D55570F521DB
          55F2FF008173E768E125431546AFF368FE7FF06C7CEC2E47AE2A283519745BAB
          CD66CB5AD5F42B996DFECD713DAEA12DB6F80904C6E51D4142554953C1207A57
          F3C7FF000F5EFDA27FE84DF831FF0082EBEFFE4CAD1D07FE0A71FB51F897C43A
          65AF85BE1D7C33D4FC50498ED134DD3B53FB436782144779B8E738FC6BE42855
          6A6B95B4FCB73EDB1386BC1F324D79EDF3DCFE8FBC3DE02F14EBD67FDABA7697
          63A368B2BEFF00ED2D5265B0B49090732091C6655F97968D5F1819AF56D13C2F
          FB3EE8A2DA7F1BFC4F8BC7178464DAE8A656B2570BBB6B4B6FB9988C11967456
          C8CA02457E247C3CD47FE0AB9F13FCAD5747FD937E116916B2A990DEEBB11D37
          8CF56171A82C99EF8DB93D6BEF6F097C2DF8EBE15B59356FDAABF6A3FD867E0E
          5A24898B6D3A3BAB8DE857244973797F6AB1481B8C049410339AFAEC160EAC57
          3D3C336FF9A6F4FB9F2AFC59F01996634652F67571A947F9692D7E6D733FC11F
          755C7ED4D69A3E9E345F86FF000DF4FF000E6931478B417A523481C9C91F64B7
          F936E4B9E25073CE3935F3F6BDF107C79F123539349D5FC43E24F15DDCC1C8D1
          AC95D9193192A2CA01FBC500672EAE401927A9AD0FF85CDFF04D8F85F636779E
          37FDAB7E1378E2F19D583C3E2882F36385C3016FA73122327271287C71F31AE0
          BC4BFF0005A9FF008272FC23B09F4EF87B73E2AF192C6CEAB67E14F0A9B58998
          6483BAE7ECC9B5893F30CF5CD75622955A8BFDB31118AEC9AFF81FA9CB82AF46
          93FF0084FC24A72FE6927F7DDDFF0043E89F0C7ECD7F1375DB851AD47A7782F4
          FF009B74B7322DCCC783B4A43136181206773A100F427207D13A4FC24F82FF00
          0663B6F14F8BF55B59B504903DB5EEB73AB14947CC05AC0005F3011952AAD2F6
          DC6BF9E0F1D7FC177FF6A6F8CBE31B6F057EC7BFB37E95A7492B94821B9B4B9F
          116AF7DF3ED57586DC471C208640536CB86FF96841AFD3BFD89FF64FFDBBBE3C
          DFB7C4AFDB52DBC09F09F4A9CA08747B2B7B9BCF11EA31EDCE67B996EE68ACE3
          194558CACAE4070521DAACD86171185A6ED82A6E72FE67B2FEBD11D58AC163AB
          46F98D554E2FEC4777F77EAD9F661F8D7F113E2EF89AE7E1BFECE7E1D963D40B
          05935CBB55CC10EE61E7ED6564B788EDC87903BB0DCAB107DA2BF5CFF63BFD94
          B40F84535CF89EFAF2FBC6DF143538D5356F116A05A4B8987CA4C30972CD1C39
          443B7259B647B99847184D3F80DFB3CF873C11A55AE89E10F0D59E81A5821D96
          2525E66C05DF2C8D9791F0AA0BB92C7039E2BF4AFC05E035D3A184B438381DAB
          8B30C549BE6AB2BCBF05E87D06479642292A51E58FE2FD5FE9B1E8FE10D3CDB5
          BC40A95C0AF4884613EBCD67D8D92C281554003AD6AF4E95F35295DDCFBDA50E
          5560A28A2A4D428A28A0028A28A0028A28A0028A28A0028A28A0028A28A0028A
          28A002919430C30C8A5A28031EFB4D8AE5195D411D335E25E33F86D69AAC32FE
          E14923D2BE85AAF2DBA4808C0FA1E95A53AAE2F439EB61E335667E557C43F818
          E8D3CB0DB9EBE95F877FB75FC23FD806FF00C41A0F857F6D6F0A7C27D2F54D4E
          13168DAFF8B34F6D3A39951C7FA35BF880AC691CD962DF64170B2B2867F2CA02
          D5FD70F883C356D790C80C4A49E0F15F00FED19FB3CF817E28784F5EF05FC42F
          037863C7FE0CBF4D97BA4EB3A7C57B6776A183012432AB23619430C8E0804722
          BE83059A36B92567EA7C5669C3D08CBDA42E9F968CFE36FC63FF00041DFD81FE
          2B69A7C57F06BC75F123C1FA5CF6E52C66F0F788EDF56D2DE41FF2D09B88E692
          4F70B3A8FA57C4DE30FF008372FC516C613E06FDA9341D57AF989AB785A4B5DB
          E9B5A2B8973F90AFBEFF0069CFF837B24F0278975AF88BFF0004FDF8FF00E38F
          D9C7C5521731E8979AADEAD92827FD5C3A95BB1BB8231E8E2E09F515F937E2AD
          1FFE0BEFFB35EA92699717FF00B49F8CED6DDCC70DE5818BC5D6F748090AE095
          B870085CE24557008DCAA4E2BAEA470DBD6C3BF58BD3F0B1E243EBA9F2E1F18B
          D26ACFF1BDFE454BAFF82087ED9DE1CBC93FE115F8B9F04E4B7E4ACC9AAEA368
          EC7DD56D8E3F3AE4758FF822DFFC145234783FE12EF87BAEDB804909E30B80A7
          F09625156AF3F6AEFF0082EDEA492584BE19FDA7E0321DA4C7F08D2161F471A7
          0DBF5CD79678ABE057FC1647F69580F873C7DE1FFDA6FC4DA25EFF00ADD3F5DD
          50E99A74B860C3CD8279618060852372F0471D288D2C0C95A950AADF65A7F9FE
          44D5AF9AC1F357C5508AEED5FF0058FE67E737C68F849AC7C13F1B5DF813C45E
          2CF87DE2DF104081EF24F0DEB31EA96D6D2127F74F731663328C6595598AE70D
          83903C9ABE82FDA23F668F88FF00B2DF89B42F02FC5BB8F08587C41BAB01A85E
          685A76A91DF5CE871B3111A5EBC3BA1492451E62A248E763231DBB973F3ED7C8
          E328BA7565071E56BA3DD7AF99FA0E5B5A35A842A466A69AF896CFCD6FA76D5E
          9D59FE81FF00F06DE78C74AF127EC35F0CB45B6B98A7B9F0FEB9AD691768BD61
          99EFA4BC0ADEFE5DF44DF4615FD7ADAAD8B786D7CE11E3CBFE95FE79DFF06C07
          ED0D1695E20F8EFF00B3A6A5A808E4F32DBC6BA4DB6D3F3A9D9697AFBB6E0608
          D30609C9DDC0E1ABFB78F897F1CF4BF877F093C53E38D61EFDF47D1F49B9D56E
          C59C0D3CED0431348E238901691F6A1C228C938039AFA19C5D5A54A51ED6FBB4
          3E730952386AF5A12EEDFDFAFEA7C07FF051EFDA0FE0B7ECB5F0C3C5BF18FE2B
          EBF0E8BE19D3D4A4514415EEB54BB607CAB4B48891E64F215202E400033B32A2
          3BAFF9A0FED6DFB49F893F6B5F8F9E3AF8E7E26D1F4BF0EDC6AB2C715A69F68A
          36D959C482382279300CD20445DD2372CD9C0550A8BEF5FF000520FF0082887C
          57FF008289FC74BFF891E339AF341F875A7BCD6DE10F0B898B41A259B372EE07
          CAF752ED46965EE55546123451F9E95E76619ACEA535878BF717E2FBFF0091E8
          E5990C295678B9AFDE495BD176FF0030A2B5742D075CF146B5A5786FC33A36AB
          E22F115FDC47696361616EF71737B3BB0548A2890167762400AA092480056EFC
          40F879E3AF853E2FD6BC01F12BC27AF781FC69A74822BDD3352B66827B725432
          92AC39565656561956565604820D793ECA5CBCF6D36BF43DB7561CFECDB5CD6B
          DBADBBDBB799E8DFB31FC78D5FF664F8F1F0DFE39E89E1FD17C557DE1FBE3727
          4EBF5062BA89E368A450D826393648FB2500947DAD86C6D3FE95DFF04D5FDA4F
          E09FED69F0CFC33F177E156BB6DA9E937388AFACA565179A2DE000C969771024
          C72AE41F4652AEA591D58FF96CD7DC3FB02FEDEDF19BFE09F7F1BF4DF8B3F0B2
          F5B52D06E1A2B6F13786AE262B67E23B157C98DF83E5CC996314E0168D89E191
          E48DFD5CBB3695283A12F81FE0FB9E266B9053AD5A38B8AFDE474F55BDBFC8FF
          0060EBD4B15F0CB08427FAB3D3E95FC677FC1CC7E2C87C3DFB1FAE8E2D8DC1D7
          BC71A5E94AEAD8F20A457377BC8EE3FD08AE3FDA1E95FD2A7C11FDA4347F8D9F
          00BE1D7C58D021D5F4FF000FF89BC3D63E20B183508BC9B982DEEADD26449A3C
          90B2059002012339C1239AFE1EFF00E0E72F8E43C49F18BE03FC0EB1BD93CAD3
          34CBDF14EA10A487648F732FD9EDCBAE31B916D2EB1CE4098F00119F4ACE950A
          927D55BEFD0F36BCE388C451847A3BFDDA9FCB8D7B17C0FF0084BA77C6AF1AA7
          81EF7E2E7C27F839772C0D25A6A1E33BDB8B2D3EE645C1309BA8E1952172BB98
          198C68DB7687DEC88DE3B5F53FECA3FB2AEB9FB5D78CB57F86DE09F897F0BBC1
          1F10A3B6177A5E9BE27BB9ED46BEA3779B1DACB1432869D0046F2880594B30E2
          3723C0C051954AD18463CCDF4DAFE47D0E6D5E3430D3AB39F224BE2B5EDE76D7
          4EFE5DB73F48FC37FF000410FDA77C416961AA47F17FF6759745B98D66B7BCB0
          D4AF6F229E2600ABC6CB6C1594820820E0D7B0E8DFF06EE7C58B99EDC6B5FB48
          7C3CD3ED89FDEB5AE89733BA8FF6559E307F122BE4FD0FFE09DFFF00056BF801
          2DFC9F0B3C3BF133C316827DCD2F83FC756D02DD91D1FC986ED2461FEFA03ED5
          DEDB699FF05DF82348206FDAF1D546D05AE8C87FEFA2C49FCEBEB5E1B090D2AE
          12A27EADFF0091F9F43199855D686614A51EF68AFD647E877C3FFF0083747E11
          5A5ECB27C4DFDA3BE2578B34E31E122D0747B5D22447F569276BC0C3DB68FAD7
          D3307FC12BBFE091DFB355A69D27C6DBFF000BA5E247BD6EFE2078FBEC6F72A4
          E3718525B6858678E23C57E517C3EFD853FE0B6FFB4EEA31D8FC40F899F193E1
          C7846E03C77179E30F1EDC416C146329F60B79649896078CC2109182C3AD7ED6
          7EC4BFF06E97ECE5F0D3C49A4F8EBF683F106AFF00B4DF8BE0B85BA8EC2FECC5
          9E861C1C8F3ACF7C8F75CF244B218D870D1904E6E1EC92BD2C328AEF2FF27763
          70AEDDABE31C9F682FD55923F46FF618BAFD9A3E27E857FA3FEC77E02B1B1F83
          56334C8FAE681E153A3787AEEF924F29E3B491E387EDD265096B8B74961F970D
          2EE2AA7F653E1E7C0C0A22927B63DBB5777F077E14E97A1699A669DA769365A5
          E9B6F12C305BDBC2B1C5046A301111400AA00C0038AFB3744D0A0B58A3558946
          071C579D8ECD676E45F81F4594F0F534FDA35F7EACE13C27F0FAD34A8A30B0A2
          E31DBA57AE5A584702008A157F53572385500E3247E953578139B93BB3ED2950
          8C15908000000302968A2A0D828A28A0028A28A0028A28A0028A28A0028A28A0
          028A28A0028A28A0028A28A0028A28A0028A28A0086688480F193FCEB83F1078
          62DF518A406356C8C735E854D64571C8E7D6AA326B62274D49599F0C78E7E09C
          1A8099A3801CE7B57C75E3AF807A8DBC17B269F6913DDEC6310932A8CF8E0310
          090338E403F4AFD9BB9D3619D58322B0FA570BAE7836CAFA3933023120F38AF5
          F099BD4A7A5CF99CC786E8D6D5A3F909F8EFE15FF82A7E8D26B107C24F837FB1
          9DEC2849B4BAD53C59ABCCF22F60F6C2D2DC06FA4D8F7AFC9FF89FF06BFE0BD9
          F1AB52D4349D4FC5FF0003BF672F0BC91C714A9E1BBF48237C38CC915C2A5D5F
          2377204A80804639C57F7A7E2AF8396B7BE6B2DB28CE7B57CA1F127E0B43A569
          7AA6A8FA75F5DC76F0BCED15ADBB4B34814676C71A82CCC71800724F15EFD0CD
          155D2A54925E4EDF91F178CE1C743DEA5460DF771BFE6EC7F9B9FEDAFF00B03E
          93FB2DE87F0DFE1FEB7E32F127ED1DFB6BFC4DD711A274DE96FA7DBEE4593CB0
          F299AE2EA7BA95235B89FE478C4DFBB8DD43D7C25FB5AFEC95F143F634F89D67
          F0BBE294BA06A1A95D6936DAC59DFE9533CB677B049B95BCB67547CA4B14D110
          CAA7E4DD8DACA4FF00729F093FE09E1F15BC7BFB52F8D3F6F7FDA77C3765E19F
          1BCB62342F873E080F1DCC9E07D1543013DECE9946D4A6124E59222C9089E550
          F26E023F97FF00E0B05FB04EADFB47FC01B8D47C27A5BDCFC5DF0619F58D0235
          4CBEA30951F6AB10704EE912357403ACB0C4B901890EBE47431342A55A2AD35F
          0AEE96F7EADCB7FB91CB87E26C660715468E25F35277E797694B6B5B4518D92D
          34D5BF33F90CFD8BFF00699D73F642FDA63E157C7DD1A2BABDB7D17500354B28
          9B0DA869B2A98AE61009505CC4EE5371DA245427A57FA69FC35F8CDE10F8E7F0
          83C2BF107E1DF892CFC55E0CD6F4E8B50D32FEDCB04BA8245CAB6D60191BB146
          019482AC01040FF28960CACCAEA559490411820FA11DABF7B7FE08EBFF00056D
          4FD8DB506F807F1EF50D56F7F671D46ECCFA75FA235C49E0EBB91B32308C65DA
          CE424BBA20251F73AA93249BBE7724CC234E5ECAA7C2F6F27FF04FB4E25CAA75
          63EDA8EB24ACD775FE68F4CFF82C1FFC128352F869E24F187ED3DFB37785D64F
          873712497FE29F0BE9F01DDA04849325ED9C4A39B3272CF12FFA824B28F2B221
          FE793C31E19F1178D7C45A27847C21A26A9E25F13EA5731D9D869F630B4D3DE4
          CE70A91C6A096624F415FEA6D0D8784FE2B787347F15785356D23C45A06A5691
          5F58DF594E93DBDEDB4A81E396291095923756565604820823835F2EFC16FF00
          825C7ECE3F0A3E3278ABE367C35F841A0F863E22EB65C5E5FC2F2BA441DB749F
          668198C56DBCFDEF25533D3A715EF63F20A156A2AB097227F12FD57F97FC31F2
          994F15632852950A90F68D7C2FF497A775AF4F33F2ABFE0981FF0004AFD3FF00
          654D06DBE277C48D3B4AF157ED197F09DF728A2683C2B03AE0DADA3F469882C2
          49C633B8C69F206693CCFF00E0B25FB20697F1D752FD9C9ADBEC5E17F89F737B
          AD6949AD3DB190CBA7DAE8B7FAA9B6914152E37D830424FEECCF21C1DC41FEC3
          FC0BFB3EDA5969A1A7B64DFB79C8AFCB6FF8282FC0FB2D4FF680FD807C3F3DAB
          369DAA78F3C4BA6CE10ED2C8FE08F1012011D0E10F35EC3CD30CE87D4211FDD5
          D69F35ADFBF99F3AB8731AB17FDAF566DD7B4B5EDEEBB24BB796DDCFF32B6492
          3D8258DE22CA180618C82320FD0D7EF5FF00C121BFE096FA87ED05E2BF0EFED0
          FF001F3C332C7F036C675B9D1347BC8C8FF84BAE11B2AF246473628464E78998
          6DE503E7FA47F887FF00048DFD967E2427C09B2F1B7C2CB7D5E2F87B676BA568
          CE6431BEA1A74116C8ECF5064C7DAE00C166D8FF00C7BF042CD324BF7DC7A578
          2FE10F856F7C41E23D4BC3FE0FF0A69164D3DC5D5DCD1DAD9E9B6B1264BBBB15
          48A2445C92485503B015E4E0786A951A9ED2AC94D2D979F9FA7E3F81EFE6BC67
          88C4D15470F074DBBF337D15FECF9B5D7A6CAFB9D1FC4CF8CFE0FF0080FF0007
          7C59F113E22788AD3C2DE0CD134E9750D4AF67276C1122E4E00E5DC9C2AA282C
          EC555412403FE659FB537C7FF11FED47FB427C57F8F5E2832C7A8F88F5692EE1
          B776DDF62B35023B6B707D23863863CF7DB9EF5FAA1FF0586FF82AD37ED9DE21
          1F03FE095DDCDAFECD7A35EADCBDF346F0CDE2EBE4DC167746C325A264F97132
          8666FDE38CF96B1FE1764735E067B98C6ACBD9D3F857E2FF00E01F5BC2F944E8
          C3DB56F89AB25D97F9BFF23D43E0C7C25F177C77F8ABE03F83FE0482DE6F15F8
          87518B4EB469CB0860DC72F34A5416114681E4720121518804F15FADDFB0D7EC
          01F0E3F6BDF06FC51F83FE26D4FC4DFB3CFED7FF000CF5D7B79351B38CCE2EAD
          9E7C837B66CEBE64D6F34375109609612164837799B173F6C7FC10ABF60CD734
          F86E7F6B9F885A25FD8EA9A9DABE9FE0AB4B88CA32D8C98F3B512A4671280238
          8F198FCC7C32CB1B0FDB7F1E7FC13D3E2AF873F694F0A7EDC9FB2ED9E8F77F13
          96C9744F1FF83AF9C5BDBFC41D0C6CC8827E043A9C6228BCA79088DCC50ABB22
          AB6FF6F2DC9E950C3C313885772BDE2FF95ECD754D3D74E87CB671C455F178DA
          982C1CB96304AD35FCE9EA9F47169F2BBAB731F937F0FF00F60DFF0082E07C2B
          90689F0C3F6D0F86FE24F0D9C057F115DCDA8BAA827002DF69F72C99EE11FBF5
          E2BF603F67EF825FF0555B9D5FC3D6DF19BC55FB0DBF85D1717F368BA16BB36A
          331FF64BDC43003D790A074E2BF73FE1FF00C0F8EF2C34FBD6D3E7B532C49208
          E68F6C91EE00ED65ECC3A11D8D7D4BE16F8416763E5BB5B29C7B515B32852D29
          CE56ECE4D9B61387E75ECEB5385FBA8A4FF03E30F027ECFD72F1DA1BB89259C2
          8DECB195566C724292703DB27EA6BEC3F057C1AB6D3044CD6E011CE36D7D13A3
          7852CEC9142C0A0FD3935D9C363144061557E82BC2C566B52A753ECB2EE1EA34
          52B2397D0FC3D069F1A2A4607A01DEBB28E3118ED9A7AAAA8C280296BCA6DBDC
          FA2841455905145148B0A28A2800A28A2800A28A2800A28A2800A28A2800A28A
          2800A28A2800A28A2800A28A2800A28A2800A28A2800A28A2800A63C6AFD783E
          B4FA2803327D3A2941DC8ADF8570DADF852DAF15F312B67BE2BD32A278958920
          9535519B5B1954A4A4AC7CCFA9FC2BB3B912936E873EA2BE19F8FF00F02C3DA5
          C4F6B6A3201FE1AFD776B442A4614FB62BCDFC69E0FB5D66CA78648436E0474A
          F530399CE94D493D8F9FCDF20A588A2E0D6E7F98DFFC1627FE0945AD787F5FF1
          37ED39FB3BF84964D39C4979E31F0D5840DE6C72EE2CFA95A44B9DCA412668D4
          02A54C80306936FF0033190457FAFAFC7FFD9E9A46BBBBB3B3F9F93C0AFE447F
          E0A29FF044CF09FC54D53C47F13FE017F66FC2DF8B771335D5EE993864D1F5E9
          59B2EE42826D276C96DE8AC8EC3E6452ED28F7B1F91C31ABEB183F8DEF1EFE6B
          CFCBEE3E1B2BE26A9954D60B32BBA6B48CF7B2ED2F2ECFA75EE7E127EC2BFF00
          0557FDA9BF60DBCB6D1BC07AF5BF8EFE1119FCDB9F06EBCEF2D92E58976B3901
          DF69231666263F9198EE78E422BFAF2FD8DFFE0E3CFD883E2BD9E9DA6FC63BDD
          67F66CF1F6638E4B6D7E07B9D32E246DE58DBEA36EAC823508B96B95B7E6450A
          1F04D7F08DF1ABF679F8D3FB3BF8964F0A7C66F877E21F02EAB9C42F73106B6B
          C1CFCD05CA168A65E0F28C71820E0822BC66BE6238DC461DFB2A8B6E8FA7EA8F
          BA797E131496228B5EF7DA8B4D3FCD33FD70BC31FF000558FD8635ED2E0B9D1B
          F6B2FD9BAEE074DEBB7C6BA686DBEA54CD91F88AF01F8CDFF0512FF8272DFDF7
          83BC59E3AFDA57F664BED4FC35A949AB68770DE2FB09E6D32FA4B3B8B3696158
          E52DB9ADAF6EE23C1F9656F6C7F95D119F4A3A76E2AE39AC53BF27E22A993CE4
          B95D4FC0FEF2FF006AFF00F838DBF635F85B1DFE8FF0274DF137ED1DE31D922A
          49A740DA76916F2AB95D935EDCA077E9B83410CA8C3F8C57F291FB6CFF00C14F
          FF006A9FDB9EFAEF4EF893E2D4F0AFC2DF3C4B6BE0DD08B41A6A6D60C8D71C97
          BB914AAB0694B05604A2C79C57E7657A67C2DF831F163E3778813C2DF08FE1EF
          8AFE20EB84AEF8B4CB37996D94B050F34806C863CB282F215519E48A8AB98623
          10FD947AF45D7F5614729C26117B79DB4FB527B7E88F34CF5F4AFD8FFF008260
          7FC12DBC5BFB5F788B49F8ABF1474ED4BC37FB3B595D8D8195A2B8F184B1B7CD
          05A9E0ADA8236CB703A9DD1C64B891A1FD01FD84BFE0855631DC68DE3BFDAD1D
          3C4DAF2CC93DBF83B4DB8DD636E060817F70BCDC3678314644636F2F286207F6
          21FB3BFECCD69A75AE8F6569A259E95A45AC51C36D6B6F0AC50DBC4A00544450
          15540000503000C57D265DC391C3A588C7FCA3D7FEDEFF002FBFB1F179AF194F
          1D2783CA2EEFA3A9D12FEEF77FDED974BEEB6FF66CFD9EEDAD6C349B6B6D2ADE
          C74DB78A382DE08A2091C11A801511470AA000001C002BF4EFC33F09ECAC6087
          16CA08033915D67C39F87967A05940A2DD632A076AF6F86CD5140002FA71CD79
          79A66F3AD51BB9F53C3FC354B0B4541238BD1FC336F648B88D001ED5D9C3631C
          6176AAA8FD6ADAC4ABCF53EF52578929367D642928AB21AA8A8381CFAD3A8A2A
          4D028A28A0028A28A0028A28A0028A28A0028A28A0028A28A0028A28A0028A28
          A0028A28A0028A28A0028A28A0028A28A0028A28A0028A28A0028A28A0028A29
          A580EFCD003AA9DC047073803BFBD4AF2000E5B15CDEABA9A5B46C4B8E055462
          DBD0CEA4D257679EF8DF43D3B50B799678E36E0F615F99FF001BBE13691762E6
          58A0895B9E82BEE3F1F78F60B08662F305C03DEBF383E317C6BB5805D462E118
          F23835F4396D59D26A57B1F159E61E9E253A6D5CFCD0F8D7F07BC39AD69FA9F8
          7BC53E1ED07C53E1E9C159EC751B48EE6DE65C630F1480AB0E4F51DEBF19FE32
          7FC12FFF0062BF1BAEA6D1FC2F9FE1D6B33B03FDA1E19D465B4683049223B673
          25AA839E7F724E3A62BF64FE22FC479B589EE0479E4915F306B367A8EAAEEC91
          48D9F406BEB3FD6BA1523C989829AF3499F9A3F0EB174A6EAE06ACA937FCADAF
          BEDA3F99F827E2DFF8233FC2F9B529A4F077C7AF1A683A4E06C8354D1E0BF994
          F7CCB1BDBA9FFBE0566F87FF00E08CBE044D56CE5F11FED11E23D57451203716
          F63E1D8AD27953B8499E79950FB946FA1AFDC8B8F87FAD5C31616F375EA01A86
          3F87DAC40D96827F5E45737D77256F99E1D5FD65F95EC762CA78A12E458C76DB
          E185FEFE5BFE373E25F843FF0004ACFD8B3C20B0FF0068F813C41F13F53598CB
          1DDF897579642A08C6C30DB79103AFFBF1B1F7AFD9BF81FF00063C23E19D274D
          F0CF827C2DE1DF06F86E1FF5361A5D9476B6F164F2447180A09EFC735F3768FA
          7EA1A6488D24522E3D8D7D2FF0FF00E204DA3CD00906002335D51E2BC3D35C98
          6A6A1E892FBDF5389F8778CAF25531D5A555AFE693697A2D97C8FD58F829F087
          4984DA4D3C31B3707915FA7BF0FF00C33A5E9B6D02C50C6A428030057E447C20
          F8DF6C7ECD1BDC2A1E3BD7E937C3BF88D6D7D14052E15B2073BABE5B31AF3AFE
          F5EE7E8D9161296112872D8FB2ECD238D142801476F4AD2073C8AE1744D692EA
          343B8367DEBB18E60C010D906BE6A5169EA7DD52A89AD0B3452020F439A5A935
          0A28A2800A28A2800A28A2800A28A2800A28A2800A28A2800A28A2800A28A280
          0A28A2800A28A2800A28A2800A28A2800A28A2800A28A2800A28A4240EA68016
          90903A9A81E70A09C803D6B32E2FE38C125863DE9A5725C9234DE70A0F200F5A
          CAB9D4638F24B003D6B99BED7E18C1CC807D4D79BF883C5AB0C4E5651902B685
          16CE5AB8848F4EBAD763556F9C74AF1AF1C78C23B3B699CCA14004F5E95E5F79
          F114099E369B9FAD7CF9F15BE2115B19FCB9C8241EF5DB468DB5679389C5B7EE
          A3C67E387C5E922FB54305C10DC81835F9AFE2BD7355F115ECC15E49371EC6BD
          7FC67797DAFEA328DCEFB9ABB8F86DF07A5D5EE209A6B7DC09CF22B0AB59CDF2
          A37C3E1634D734B73E70F0C7C1FD57C433C6D240EE1B1FC35F5B7823F64EFB6A
          C2D73644E40EAB5F68783FE1AE8FE1BB689E7862560072457B5681ADE8B6B2A4
          31987D2AE386B2BB2278CBBB23E43B3FD8EF4C3082D651F4EEB581ACFEC91A5C
          2AE16D23EFC6DEB5FA8567A958CD6DB93CB3C579878B359B5B79189D98A6A9EA
          12AAAD747E48F8C3F65936CB2B5BDA76ECB5F2DF897E106ABA0C8ECB6EE81493
          D2BF71AEB5DD16F018A610313D735E6FE2AF871A3788AD6492086266233C0A73
          C2A6AE88A78D69D99F8D7A06A1AAE837718DD246411DEBEFFF00829F17E68A4B
          482E2E1BB0E4D7937C49F8412E917134B0DB954C920E2BCB7C3CB79A1DFC6416
          50ADEB5853AD283B33A6B61A1563CD1DCFDFBF877E3A8EFADADDBCD072077AFA
          274FD7639114871D3D6BF233E0E7C439238208A598F000E4D7D85A3FC440ED14
          6B367F1AEAAB494973238F0F88707C923EDAB7BF49003B862B4D26040CF35E0B
          A0F8AD658A32F28E47AF5AF44B2D72293187047D6B82545A3D9A7884CEF0107A
          1A5AC7B7BD47030C2B49250D593474A9264D452020F229691414514500145145
          0014514500145145001451450014514500145145001451450014514500145145
          001451450014514C67001C5000EE147BD66DC5D040D934B7170A8A493CD79F6B
          DADADBA39DDD3DEAE31B98D4A9635AFF005B4877664515E75ACF8B638D5F130C
          FD6BCBBC53E34950B889CE7EB5E1FAB78A753B8760A5B1F5AEA8451E7D4A927B
          1EADE20F1BCCA5F64A49FAD78FEB7E38BA70E0B93F8D72777A86A531CB8761EB
          5CAEA0F3B2B6F539ADA3248E39424F73135EF164B1CACE24607D8D789F8BBC43
          73AAAB21667FC6BB1D7A1925661F3035C8268AD34CA0A9EBD715CF5710F6475D
          0C22F899CF782BC1126B3A9C65A1DC377719AFD0BF03F81EDB42D29267855182
          F5C579FF00C1CF04A493C3234593C76AFB1F56F0DB47A33242873B28A4D2572A
          BC5C9D91F1AFC46F1A1D3D6682072A471806BC0B4AF8857E9A8826570BBBD6BD
          1FE27786B5017B705D1C2727915E12346961973B4820FA529E22571D2C1C6C7D
          C3E0DF8842E6CE3579B276D725F11BC57FBB91A39BE6F635E03A2EA575A700A1
          980A76B7A95CEA2A518B1AE8FADC5C7CCE2FA8494FC8E62E3C677E979912BEDC
          E7AD7D11F0E3C646F84505C49BB38EB5F32FF62C923E429249F4EB5ED5F0DBC3
          17C6FA131A3E322B9A9E22499DB5707168F76F1B781ED75DD29E7485198AE7A5
          7E7C78D7C0EDA5EA32E22C0C9ED5FB1DA47861E4D1424C87253FA57C75F187C1
          4915C4AE908EA79C53ACD495C9C3C5C5D8F8F7C29A85C69722A825715F4A7853
          C53296477909207E35E1C74468E53F29183E9D2BB9F0FC0F094C8E476C5146B3
          D98F13864FDE47D63A378F678D5155CE3EB5EBFE1DF1C4B214DF29F7E6BE42B2
          9655552A8DB8575963AADFDB9528AE2BA5C9338E1068FBFB47F14C722A6E9067
          EB5E8363AC24A07CE39AF80346F196A103A872F8AF73F0BF8D5A6F2D5DC83F5A
          C27156B9D94EA35B9F58C1721F906AFAB061EF5E6FA2EB0B7089B5C7B577304E
          18020D72CA363D0A752E68D14C570DF5A7D49A85145140051451400514514005
          1451400514514005145140051451400514514005145140051454323E323B5002
          49200302B2EE2ED5013919A4B9B8D8080466B8BD53512A1806AB8C4C2A4C9B53
          D5D1037CE0578EF89355598380C0F5EF5635ABFBA959847BB15C1CDA75F5DB1D
          C18A9AD93B1CB3BBDCE3F508A299D99F2C4D60C9696B9242026BD317C2F2B1CB
          E686F0BC6A0E46455739838DCF22B882DC023CA1F9572DA958C0CA4EC03D4E2B
          DB6EFC3A39D8A4FE15CC5DF86E56DC04668E7638513E6BD4F46124A76A039AAB
          63A0033C7F271915EF92F83E5724F9593F4A9AD3C1B2472A168F207B572B773D
          08AB46C7A6FC23D0D208A1250678ED5F533E9B14D67B1906315E3FE02D3CD9C7
          1A9502BDA4CFB6DC0EBC56E8E78A5D4F987E247812DAEA39A4110C9E7A57C67A
          F784859DCC8BE58C66BF477C52C268DD40C9C57CD3AEF869EF2E1DC459E7AE2A
          6A2561D26EE7CAFF00D818FE1A3FB033FC3CD7D087C132E7FD5E281E099BAF94
          6B13B6E8F18D13C27F6BBA8D3CBC9CFA57D8DF0DFC076B6C90CA624CF07A579F
          68DE1892CEE11CC58E7D2BE93F09910471AE306B585AC71D66EE7A1A69914365
          E5AA8C62BE54F8BBA1A4C9332A64F35F5C098B41B7AF15E1FE3ED34DE2480296
          AB23ADCFCF2BBD031336130339E95A3A5E8CA928DE878F6AF74B9F064CF23111
          F534D8BC1F2A3731103E9582766754F54737A769D02852501AEA20B3B5DA3300
          03E95D059F86A45C0D871F4AE9ADFC35B80DCB83F4AEA533CE95238B8B4FB427
          EE053FCABA3D3218EDDD4C6C47D2BA44F0B647030695BC373C7CA034F9C231E8
          7A3786F55110405FA63BD7AEE9DAAAB85CB8FCEBE65B6B5BEB470406C76AF44D
          1352B84DAB26EA87666D0BA3E8082E95C0E456924808C13CD79EE9BA86E0A0B1
          AEBADAE3781C8CD6328D8EBA750D9A2A28DF200A96A0D828A28A0028A28A0028
          A28A0028A28A0028A28A0028A28A0028A28A0028A28A0043C026A8CCD8C9E6AE
          BFDD35997192AD8E7AD04CB639CBF9892C066B8EBA85E663939AECEE62249383
          59525A93D1456D168E6946E716FA6C64925431FA545F6145CED8C7E55D91B066
          EA2A55D340C1652285227D99C3FD859B236E29A74B279615DE35981D1727E951
          35931E898143920E43CFE4D1E339F93354A4D0E3E7F763F2AF4A3A79E3231519
          D3C74DA293994A07990D062FF9E793F4A906851023F76B5E8DFD9C07F093FA52
          FD80FF00707E5591A239FD26D440400315D5CAACD16003D2A28ACB6302148AD9
          8EDF726304F15B46464E1A9E6DA85A3CCCC08CD619D0909E6306BD626D303138
          5E2A04D27E6E5411F4AC9B7D4B4AC798AF872361FEA87E543787234FF9663F2A
          F66B7D2100C103F2A2E3474C70A3F2A433C546848A41118ADAD3ECCC0EA029C7
          F2AEDE4D2B04E16A4874CDAC32B8A71DC1EA5788622F9B9AE5B56B25B824119A
          EFE4B7DA98C64D64CB6459892B935A4A5A11186A7991D0A2E7118A8CE851723C
          BE7E95E97F603FDC1F951FD9FC6369AC8D1A3CF23D1231D107E02AEA692AA785
          C577034F1C7CB520D3C8190335A2999B81C60D308FBAA2945891D57A576CB64C
          320A0C54A2C948FBA45529264A877386FECF8DCE1A307F0A9A3D39139000AEB9
          F4D04642E4520B175E828E7054CCAB48DE271D715D9584C7E506B223B6231F28
          AD5B684A9180450D971563A785B38E6AF03900D66C1C05AD053902B13A22F41D
          4514505051451400514514005145140051451400514514005145140051451400
          C704E303354A48C9E315A1450268C46B42C4E4134D1603825467DEB768A05CA6
          29B21FDD19A69B3FF67F4ADCA2827D99826C47719FC28362BC7C9FA56F5140FD
          9983F601E98FC283603AE09ADEA280E439F360A78DB8FC29BFD9E7D0D7454503
          5148E7C580FEEE4D4AB698E71FA56DD14038DCC9FB283DA9C2D8290700D6A514
          0B90A4B1E3D850D113D06455DA282AC6535A86EC29A6D40ED9FC2B5E8A04E062
          359E73C1351FD8477535BF4500A3639EFECFE73B467E94A2C3FD922BA0A281B4
          8C016007F090697EC0BCFCBFA56F51413C88C2FB02FF00747E540B103A0FD2B7
          68A05ECCC3165D38FD297EC4B8FBB5B74503E4460FF678F4A916D4A9E01C56D5
          140F94A11A118E0F5ABA8081CD3A8A0AB0514514005145140051451400514514
          01FFD9}
        Properties.Caption = 'xgm.ru'
        Properties.GraphicClassName = 'TJPEGImage'
        Properties.PopupMenuLayout.MenuItems = []
        Properties.ReadOnly = True
        Properties.ShowFocusRect = False
        Style.BorderStyle = ebsNone
        Style.HotTrack = False
        Style.Shadow = False
        TabOrder = 1
        OnClick = xgmlogoClick
        Height = 265
        Width = 301
      end
      object teamliquidlogo: TcxImage
        Left = 56
        Top = 432
        Cursor = crHandPoint
        Hint = 'teamliquid.net'
        Anchors = [akLeft, akBottom]
        ParentShowHint = False
        Picture.Data = {
          0A544A504547496D616765572B0000FFD8FFE000104A46494600010100000100
          010000FFDB004300010101010101010101010101010202030202020202040303
          0203050405050504040405060706050507060404060906070808080808050609
          0A09080A07080808FFDB00430101010102020204020204080504050808080808
          0808080808080808080808080808080808080808080808080808080808080808
          08080808080808080808080808FFC00011080097009603012200021101031101
          FFC4001F0000010501010101010100000000000000000102030405060708090A
          0BFFC400B5100002010303020403050504040000017D01020300041105122131
          410613516107227114328191A1082342B1C11552D1F02433627282090A161718
          191A25262728292A3435363738393A434445464748494A535455565758595A63
          6465666768696A737475767778797A838485868788898A92939495969798999A
          A2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6
          D7D8D9DAE1E2E3E4E5E6E7E8E9EAF1F2F3F4F5F6F7F8F9FAFFC4001F01000301
          01010101010101010000000000000102030405060708090A0BFFC400B5110002
          0102040403040705040400010277000102031104052131061241510761711322
          328108144291A1B1C109233352F0156272D10A162434E125F11718191A262728
          292A35363738393A434445464748494A535455565758595A636465666768696A
          737475767778797A82838485868788898A92939495969798999AA2A3A4A5A6A7
          A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE2E3
          E4E5E6E7E8E9EAF2F3F4F5F6F7F8F9FAFFDA000C03010002110311003F00FEFE
          28A28A0028ACFD5756D2F42D3AF358D6F52D3F47D26DD0CB71757532C50C083A
          B3BB10AA3DC9AFCA0FDA37FE0B73FF0004FCFD9E0EA1A647F159FE3578BE02E9
          FD95E07846A4378E30D7A592D073C1C4A5873F2D7B79270D6619954F6597D195
          597F75376F57B2F99E3E73C4381CBE9FB5C7568D38FF0079A57F4EAFE47EB7D1
          5F9F3FF04E9FF82847807FE0A1FF000A7C5BF117C25E18BBF006B5A26B92691A
          9683757CB753DB4654496F705D5106C950B638C078E45CB6DC9FD06AE7CE327C
          4E5F8A9E0F190E4AB07669F47F2D3EE37CA736C3E3B0D0C5E125CF4E6AE9AEBF
          7EA7E6A7EDE1FF000550FD9B3FE09FD7FA2F85BE2A5AF8F3C53F11B54D33FB5B
          4CD0F44D38399ED8C8F12C925CCAC90C6A5E290637338DB9D98233FCD9FED1BF
          F07217ED51F10C5FE8FF00B3E780BC11F003427DC91DFDC01AD6AC0740C24995
          6DD0E3B792C41E8D5F4FFF00C1CFBF0D25F37F64FF008C56D013095D63C337B2
          E3EEB660B8B7527DFF00D2CFE06BF92FAFEDAF03BC28E1DC664B4336C451F6B5
          657BF3BBC53526ADCBF0F4BEA99FC75E33789F9FE1737AD95D0ADECA946D6E55
          66D38A7F16FD7A347FA6B7FC13ABE29789BE357EC3DFB32FC50F1A6BF7BE2AF1
          86ADE16B69B55D46E0832DE5E2168E591F680325E36E805792FF00C1603E2E5D
          7C18FF008273FED35E24D36F65B1D6751D1A3F0D5A3C6C55F76A1711DA3ED239
          04453CCD91D36D7807FC1003E2447E3BFF00826F7C3FD04DCADC5E78535FD67C
          3D30CF299B9378808FF72F900FA57977FC1C83AECBA57EC07E1AD2E290A0D4FE
          22E956B2007EFA25A5F4D8FF00BEA243F857F3C60387231E3C5974A3682C4EDD
          3954EE95BB38FE07EF58EE2093E0978F8CAF2787DFAF33859BF5B9FCB9FECEBF
          F0569FDBDFF6665B0D3FC13F1E7C45E2AF09DB808BA178AFFE27164107444F3C
          99615F68A44AFDE3FD9E3FE0E67F05EA7F61D23F6A4F807AC785AE89092EB7E0
          CB91776E4FF79AC6E19648D47FB32C87D057F1F95F62FEC41FB13FC5EFDBAFE3
          6691F08FE17583DB69C852EBC45AFCD11367E1CD3F761A798F1B9CF2B1C40EE9
          1B81801997FB6B8E7C33E16C561EA63334A31A6A29B94E3EE35E6DC6D77EA9DC
          FE3AE0BF1138970D5E184CB6B4A6E4D2507EFA7E493BD97A347FA0DFECF1FF00
          0503FD8E3F6A78AD63F829F1F7C09E25D765008D16E6E0D86A8A4F6FB15C08E6
          6C74CAA95F7AFB22BFCC57F6E3FD93BC5BFB0F7ED3BE3FF80BAFEA371AAC3A5C
          D1DEE85AC797E49D5B4C946FB7B90013B5F19470090B247228276E6BD8BF676F
          F82B4FEDEFFB338B1B0F057C7AF11F8ABC29000ABA1F8ACFF6C596C1D113CF26
          5857DA2912BF9E335FA314315878E3B87B16A74E694A2AA2B5D3575EF457E0E2
          BCCFDEB2CFA474F0D5E583CF70AE1520DC64E0EF669D9FBADFE527E47FA44515
          FCAB7ECEBFF0733783F536B1D1FF006A6F809AA7862E09092EB9E0CB9FB55BE7
          FBCD6370CB2228EFB6690FA0AFDE3FD9EBFE0A05FB1C7ED4B1DA47F053E3F780
          BC4DAE4CA08D16E2E7EC3AA293DBEC5702398E3A65548F7AFC1389FC2ECFB28B
          CB1D869282FB4BDE8FFE051BA5F3B33F6FE1CF12724CD6CB07888B93FB2FDD97
          FE032B37F2BA3EC6A28A2BE04FB80A28A2800A2BE08FD873FE0A33FB3A7EDE3E
          139350F861AF7F607C45B28C1D73C1DAA4A89A9E94DD0BAA838B8B7CF0278F2B
          C80C118ED1F7BD7A39B6518AC0622585C65370A91DD3567FF0DD9AD1EE8F3F2B
          CD70D8DA11C4E126A74E5B35B7F5DD6EBA9F985FF056DFD8A350FDB77F646F14
          7837C1EF72BF15FC3F27FC247E178926644D46E6243BECA45C85613465917770
          B2889B20039FF39DBCB3BBD3AEEEB4FD42D6E6C6FE091A19E0990A490C8A70C8
          EA7956041041E4115FEB3F5FC6C7FC17E3FE09A171E01F156A7FB71FC14F0F33
          78135AB953E3FD3ED63E348D4A460ABA9051D21B8620487F86621893E77CBFD4
          9F469F132185AAF87F1B2B42A3BD36FA49EF1FFB7B75FDEBADE47F367D21FC3A
          962692CF3091BCE0AD512EB15B4BFEDDD9F96BB23F30BFE0923FB74CDFB0C7ED
          57A17893C497D347F05FC50B1F87FC65172560B667062BE0A3ABDBC877F00931
          B4CA397AFF0045BB2BDB3D4ACED351D3AEEDAFF4FB889668278640F1CD1B0055
          D1864329041047041AFF00260AFEC87FE080BFF052F83C75E18D2FF61AF8D9E2
          045F1BE8D6CDFF000AFF0051BA939D5B4E452CDA6331EB340A0B443F8A105463
          C91BBEBFE92BE194B154971060A379D356A89758ADA5FF006EECFF00BB67B44F
          95FA3CF88D1C355FEC3C64AD09BBD36FA49EF1FF00B7B75FDED3791F7D7FC177
          3E06CDF1A3FE09DBF13753D3ACC5E6BDE0ABEB3F1A5A80B961140CD15C907B05
          B6B9B873FF005CEBFCF92BFD623C5BE16D0BC71E15F13782BC516116ABE1AD63
          4FB8D2F51B57FBB736D346D1C919F664761F8D7F9817ED6DFB3C788FF652FDA3
          FE2F7C00F130B892EFC39AC4B6B6B7322ED37F62D892D6E40F49607864C76DC4
          76A8FA2A71446A60F1193CDFBD07CF1FF0CAC9DBD1A5FF008115F49AE1A94315
          433682F766B925FE2576BEF4DFFE027F471FF06C57C6A8A1D57F69BFD9DEFEEC
          096E20B0F19697096EBE5936B7640F5FDE587E55F79FFC1C73E1D9B59FF827EE
          8FAB431175D23E20E917B2B019D91BDBDE419F6F9A741F8D7F303FF047AF8AFE
          2FF83BFF000505F809E25F0C681E25F12E9B7D7EDE1FD76D74BB296E9C69B78B
          E4493489102DE5C2EF14E4E303C9AFEE6BFE0A27FB346AFF00B5E7EC71F1ABE0
          17864E971F8BF59B2B7974692F6531411DF5BDD4571117700955261DA4807018
          F06BE57C53C3D2C97C40C2E6B51A54AA3A7393ED67C92FC1737CCFA6F0D2BD4C
          E38171396415EA53538A5DEEB9E3F8BB7C8FF3BFFD943F654F8BBFB64FC67F0D
          FC12F837A21D4B5FBD3E75E5ECC185A68B64A4092F2EE400EC89370F7662A8A0
          B3283FE8AFFB127EC5BF093F616F821A27C1DF85D64B757785BAD7F5E9A255BB
          F11EA0570F713119C2FF000A440911A00A327733711FF04F8FD803E14FFC13FF
          00E0CDB7803C171C3E20F1F6A0B1DCF8AFC512C212E35CBB00F03A98EDA3DCCB
          1C40E1412C72EEECDF756A3A8E9FA469F7DAB6AD7D67A5E956B0BDC5CDCDC4AB
          1C56F12296692476202AA804962700024D7C278D3E30D5E21C47D4B04DAC241E
          8BACDFF335DBF957CDEBA2FB6F083C27A790D0FAE631278A9AD5F482FE55E7FC
          CFE4B4D5FF001C7FF073C687A1DBFC6DFD96FC476CB00F125DF85B53B2BB207C
          E6DE1BB478727D375CDCE3F1AFE5FEBF4F3FE0AE5FB6869BFB6D7ED8BE2CF1D7
          83AEA4BAF853E1FB54F0B785642081776703BB3DDED38C09A6966917201F2CC6
          0F20D7E61D7F6CF84F92E232FE1CC261314AD5231BB4F75CCDCACFCD276F91FC
          77E2866F87C7E7F8AC5615DE9CA5A3E8EC946EBC9B57F9853E39248648E68647
          8A5460CACA70548E841EC699457E867C11FA3DFB3AFF00C159FF006F6FD99DB4
          FB3F057C7AF12F8ABC296F851A178ADBFB62C4C63FE59AF9E4CB0A7B432475FB
          E5FB3A7FC1CC9F0F759FB0E8DFB52FC09D77C157AC424BADF83EE05F5A138FBC
          F653B24B12F5FBB24C7DABF8EFA2BF34E28F08387B37BCB15864A6FED43DD97A
          B6AC9FFDBC99FA270DF8AD9F65568E1B10DC17D997BD1F4D76F9347FA72FC0AF
          F82807EC69FB49694FA9FC21FDA23E1B6BF2C70ACF7161757C34FBFB54240DD2
          5A5D79732A8242EEDBB72719E4515FE797FB1CC6927C4DD756444917FB066386
          19FF00978B7A2BF99788FE8E783C3629D2A18997268D5E29BFBD357FB91FD17C
          3FE3EE2F1386552B61E3CDB3B3697DCEFF009B3C1FC2BE3DF1AFC30F1ED878F7
          E1D78AF5FF000478D34CBD373A7EA9A65D3DBDCDA4A18E19244208EE08E84120
          E41C57F587FF0004F4FF008386F47D77FB0FE147EDDD0DAF87B58C25B5AFC42D
          3ADB16B74D9C0FED3B58C7EE58F79E11B3FBD1C6017AFE43F51C8D42F81041F3
          9FAFFBC6A9D7F55F1A78779571061951CC29DE497BB35A4A3E8FB793BA7D8FE6
          4E0EE3ECCF22AFED7033B45EF17AC65EABF5567E67FAC6F86BC4DE1CF19E81A4
          F8ABC21AFE8DE29F0C5FC0B7363A8E9D729716D79137478A54255D4FA8245278
          9BC33E1EF1A78735DF08F8B745D37C47E17D4ED25B0D474FBC844B05EDB48852
          48A446E191958820F506BFCD7BF633FF00828CFED4BFB0CF8823BCF837E3A9AE
          BC132CC25D47C25AC16BAD1F51E72C4C1B818643FF003D6128FD3248E2BFB23F
          D85FFE0B6FFB2A7ED7CBA3F833C63A8C1FB3EFC6E982C5FD87AF5DA8B2D4A623
          A585F90A92127811482390938557C66BF85FC40F01B39C8A4F13864EBD05AF34
          57BD1FF147756EEAEBAB68FED1E05F1B729CE92C3E23F7359E9CB27EECBFC32D
          9DFB3B3EC99FCC57FC15ABFE0953E2DFD84BC7F73F10FE1CD86ADE25FD9675CB
          C3FD95A810D2C9E1AB87248D3AF5FAF1CF95337FAC518277AB67F20BC33E26F1
          07833C47A178BBC27ACEA3E1DF14697770DFE9D7F67298A7B2B98DC3C72C6E39
          565650411D08AFF563F1BF81FC1FF12BC23E22F00F8FFC37A3F8C3C17AB5ABD8
          EA5A66A102CD6F7B038C323A37047EA080460815FC627FC1463FE0813F15BE11
          EBF7DF127F62DD175DF8C1F0A2EEE33278563267D6BC3ACED809103CDE5B0240
          0C3F7A83EF860AD2D7EEFE0FF8F986C7D18E579F4D46B2565397C335DA4DE8A5
          EBA4BD747F8A78AFE07E230556599649172A2DDDC17C507DE2B771F4D63E9AAF
          DE9FF824B7FC14EFC37FB7BFC281E1BF1ADDE95A17ED33E1BB544F116988446B
          ACC030ABA9DA27FCF372409117FD5487180AF193BBFB70FF00C122BF67EFDBBB
          E3A7C2CF8D5F12B5DF1378666D16C24D335DB2D1D52393C536AAFBEDA392E0E4
          C3E5B3CE0BAAB3BA481414D8A47877FC1263FE08F9E17FD8974DD2BE377C6336
          BE2BFDA9AF2CDD7F732EFB2F08432A157B6B620E259CAB15927391C948F0BB9E
          4FDCFAFE66E2DCF70995710D6C5709D69429EA935B2BAB49477BC7F95B49AE9B
          267F4570B6498ACCF21A586E28A4A73D1B4F776D62E5DA5FCC96FD7768F12F81
          7FB377C09FD99FC2507823E047C2DF087C33F0F2A812269B6A166BB23F8EE2E1
          B32CEFFEDC8CCDEF5EDB4515F99E2B1756BD475ABC9CA72D5B6DB6FD5BD59FA2
          E1B0B4A8D354A8C5462B649592F4482BF3F7FE0A53FB247C55FDB43F66BF107C
          1BF84FF1BF52F837AC4EFE7DC402206C7C4B181C58DF4883CE8E063824C648CF
          DF4907CB5FA05457564F9B56C062A9E330ED2A94DA6AE93575E4EE8E5CDB2BA3
          8DC34F0988BF24D34ECDA767E6ACCFF2C6FDA0BF670F8D5FB2D7C46D57E15FC7
          5F00EB5E02F185A92563B94DD0DF459C09ED675CC73C271C48848EA382081E21
          5FEA37FB4EFEC9DF017F6C1F87575F0CBE3DF80F4DF18E86773D95CE3CBBED1E
          7231E7D9DC8F9E193A74F9580C3065C83FC447FC145BFE08B5F1EFF62A9359F8
          8DF0FD753F8DDFB39C65A63ADDA5B66FF408B3F7753B74FBAA3A7DA13319EAC2
          2242D7FA0BE17F8FB97E77CB84C75A8E29E966FDD9BFEEB7B3FEEBD7B391FC2B
          E24781D8EC9B9B1582BD6C32EBF6A2BFBC96EBFBCB4EE91F8B7451457F409F85
          051451401F5A7EC6DFF253F5DFFB00CFFF00A516F451FB1B7FC94FD77FEC033F
          FE945BD15F93F17FFBEBF447EA5C25FEE6BD59FDC07EDB5FF045FF00D90FF6C8
          9756F17C1A1BFC0EF8CB705A46F137866DD112FA53FC57D63C45704F3971E5CA
          7BC9DABF941FDADBFE089FFB6FFECB126A7AE597818FC76F86906E906BDE0E8D
          EEDE28873BAE6C31F688881C9215E3183FBC35FE865457F12F0378EB9EE48A34
          54FDB515F6277765FDD96EBC96A9763FB0B8CFC15C97386EB387B2ACFED434BB
          FEF4767E6F46FB9FE4B524724324914B1BC52AB15656182A475047634CAFF4C0
          FDA5FF00E09B1FB167ED66B7D79F183E067856E3C593824F88B4843A6EAC1FFB
          ED756FB5A523D25DEBED5FCFFF00ED15FF0006CB6AD035FEB1FB2AFC7FB4D421
          F99E1D0BC6F6C62900EA146A16A8558F61BA051EAD5FD57C2DF495C831C9431B
          CD879FF795E3F2947FF6E513F997897E8F19E609B9E12D5E1FDDD25F38BFD1B3
          F307F622FF0082D3FED7DFB20FF63783B52D65BE3DFC1D84A44BE1CF125C4924
          F67167EE58DF7CD2C1C701184910ED18EB5FDF17C24F1BEB1F12BE18F813E206
          BFE04F117C31D6359D2EDF519FC3FABB466F749691037933F964A87008C8E08E
          84290547F28FFF0004AFFF0082207C66F017ED4D73F13FF6D1F87FA7687E0FF0
          3BC17FE1ED3C5FDB5F5B789F552C4C3366277CDBDBECF34AB852D2184105438A
          FEBF2BF9D3E90B9BF0FE233084326841CEDCD3A907EEC9BD969EEB7D652B5EED
          2BDD33F7EF0232ACF6860272CDE7250BDA109AD636DDEBEF25D12DACAF6D5051
          4515FCF27EF0145145001451450067EA7AB695A2DA8BED6752B0D26C8CB14026
          B9996243248EB1C69B9881B99DD114752CC00C922AE4B1457114B04F147340EA
          51D1D415752304107A83E95E5DF1CFE117867E3E7C1CF899F05FC63196F0E789
          B45BAD1EE5D465EDFCD8CAACD1FA491B6D914F66453DABF994FD803FE0A2FF00
          B5E7EC9BFB472FEC17FB7AE85E3AF1A7852D3516F0F695E2AB8D36E6EAFF0048
          903ECB795EE154B5E69D28DA44CDB9D15D5B79452A3EE786B81EB66D82AF88C1
          4D3AD43DE74DE8E50FE687769DEEBD2D76EC7C5F10F1952CAF194686320D52AD
          A29AD529FF002CBB26B67EB7B2573D1BFE0AF5FF000466FD9DDBE157C5CFDAF7
          E065CE9FF027C53E1ED32EBC41AEE8505B6746D756352CC20853FE3CEE18F036
          0F29988051493257F1A35FE953FF000552F0F6AFE28FF8276FED77A568704F73
          A80F06DDDE6C8D49668A02B3CB803AFEEE27AFF356AFED2FA3471263730C9AAC
          71B55D474A7CB1BEAD479534AFBBD6F6BDF6B6C7F217D22387B0780CDA9CB074
          9535521CD2B689CB99ADB65A5AF6F5DC28A28AFE8D3F003EBAFD8BEDAE2EBE28
          EBD1DB412DC483409D8AA0C903ED16FCFEB45761FF0004FAB3B9BDF8CDE268AD
          6169A41E18B96207A7DAED7FC4515F8AF1CE3BD9E3DC74D91FB0F05E0F9F02A5
          AEECF5ED07FE0AB7FF00050AFD9FBC7DE2ED17C1BFB4E78F756D16C758BCB78F
          4FF1118F5AB711ACEE0228BC5959178E88CB8ED5FA0DF0B7FE0E61FDA87C3C96
          D6DF167E077C1DF8976E84079F4D96EB47B9947AB3169E3CFD2302B89FDAA3FE
          0DFAFDB9F48F1E78FBC6FF000A23F86DF1ABC3BA8EAB79A9DBDB69DAB8B1D422
          8E599E40924378238F700C07C92B66BF27BE227EC11FB6B7C289664F1F7ECB1F
          1D74385090D72BE1BBAB8B6E3D2E2147888F70D5CF86CB78133FA51972D09CDA
          57B3509FCECE32FBCDF1198F1AE475251E6AD0826ED74E50F95D4A3F71FD4EFC
          3CFF008399FF00660D64DBC5F137E027C6BF01CCDF7DF4B9ACF568633EECD25B
          B91F44AFBF7E077FC1693FE09F9F1FFC67E11F871E0BF8A3E28B0F1FEBB7D069
          BA5E93A8F85F508E5BABA9582244248E27881248E4B851D490066BFCF3752F0A
          78A34595E1D63C37AFE93329C325CD9C9132FD43018AFE853FE0DC9FD9826F88
          3FB5478D3F685F12E8B39F0FFC3FD17669924F0908DABDF87863652C30DB2DD2
          F09C720BC678E2BF3EF11BC0EE14CBB29AF9A439E1ECE2DA519DD393D22BDE52
          7AC9A5B9F75C01E33F136619A50CB67C93E7959B70B3515AC9FBAE2B449BD8FE
          DD68A28AFE1A3FB3828A28A0028A28A00686562C15958838383D0D3ABE32FD8B
          FE322FC65D33F694B96BD1793F873E3178B3C26DF367CB4B4B9558C7B0F2DA33
          F8D7D9B5DF99E5F3C2D7961EA7C51FF8738B2EC7C3134235E9FC320A28A2B80E
          D2B5E59DA6A36975A7EA16B6F7D613C6D0CF0CC81D268D8619594F0548241078
          20D7F9B77FC150FF00636BBFD88FF6BDF883F0BEC2CAE22F86BA8B7FC243E0E9
          DB2449A4DC3B148B71EAD03ACB6E73C9F28374615FE9355F885FF05DEFD8B4FE
          D3DFB235EFC4FF00086922F7E2CFC34F3FC4165E52665BED24A8FB7DB0EE7091
          A5C28EA5ADF68E5CD7EE7E0071DFF636771A35A56A188B425D93FB12F93767D9
          49B3F17F1CB827FB5B26956A2AF5A85E51EED7DA8FCD6ABCD247F029451457FA
          4A7F9EE7E97FFC12B3436D7FF684F18D9AADCB15F065DCBFBA5C9E2FAC87E5F3
          515EE5FF00042FF0C7FC257FB5A7C44B0312CAB1FC3ABF9B952707FB4F4D1D88
          FEF1A2BF923C62E228E1B3A9526FECC4FEA5F09B209623278D44BED4BF33FD01
          E8AFE53BF6EDFF0082F4FED33FB2E7ED43F1AFF679F087C16F8277B65E19D57E
          C569A8EABF6E9A5B985A18E58E4748E78D4315954E0715F973F107FE0E05FF00
          829078DA1B9B7D17C73F0E7E1843202BFF00120F0C405D07FB2F79F68607DC1C
          D7E2D917D1CF88B1F469E221ECE109A524E53E8D5D7C2A4F667EBF9D78FD9060
          AB4E84B9E5383716947AA767F134B73FBEFB98AD248D9EF23B77894124C8A085
          1DC9CF6AABA45F68FA969F05FE8379A66A1A5CB9314D67223C5260953B59320E
          08238EE2BFCC03E2EFED97FB57FC7A1710FC60FDA27E2FF8F74F973BEC6F75D9
          FEC5CF616AAC2151EC1457FA3D7EC63F0FEDFE15FEC8FF00B33FC3CB7805B1D2
          7C0BA25ACCA06333FD8E232B1F7321763EE4D79FE26F83F538630946AE2710AA
          54AB26B9629A492576EEDDDEEBECA3BBC39F15E9F11E2AB52C3D07085349F336
          9B6DBD1592D367D59F4BD14515F8B9FAF05145140051451401FCBAFF00C11DBF
          69FB4F0AFF00C142FF00E0A1FF00B2778AB5016A7C51F10BC47E28F0EACCD8F3
          350B6D42E52EA1507AC925BF95263FBB68F5FD4557F9D3FF00C144A4F881FB21
          FF00C1573E3D78E3C05A8DD7853C69A5F8EFFE136D0EF11785377B2FD0E3A3C6
          7ED0C8CA7861B94E4122BFA03FD94FFE0E43F80BE3FF00ECEF0E7ED57E00D5FE
          077889B6C6FAEE8EB26A7A34AD9C1678941B9B71CF40B301CE5857F53F8ABE13
          63F308D0CFF26A4EAD3AD4A9B928EB24D452BA5BB4D5B6BBBA77DCFE68F0C7C5
          0C1605D6C8F37A8A9CE9549A8B9689AE67A37B269DF7B2B35D8FE9568AF31F85
          1F1A7E11FC75F0BC1E35F837F123C17F137C2D26317BA2EA115D246C467649B0
          931BFAA300C3B8AF4EAFE5EAF87A94A6E9D58B8C96E9AB35EA99FD2542BC2AC1
          54A6D4A2F66B54FE6151CB1457114B04F147340EA51D1D415752304107A83E95
          251589A9FE713FF0564FD8B2E3F627FDAFBC6DE0ED174C96D3E12F8859BC49E0
          E942FEED6C66725ED01E99B797CC8719CEC5898FDF15F99B5FE849FF0005B1FD
          8B87ED71FB1D788B5AF0BE926FFE2FF8004DE28F0F796999AEE054FF004DB25E
          E7CC8537AA8E5A58211DEBFCF6EBFD38F0478F3FB772484EABBD7A5EE4FBB696
          92FF00B796BEB7EC7F9CBE31F047F62673385256A357DF87649EF1FF00B75FE1
          63FA32FF00836A3C3706B1FB5CFC71D46E63DF05B7C389A1E9D1A4D4EC48FF00
          D1468AF70FF835FF00C36F3FC41FDAEBC5E623E5DAE8DA169AAF8EF34F752119
          FF00B765A2BF8FFE9178E94B8AEBC22FE18C17FE4A9FEA7F577803838C78668C
          A4BE294DFF00E4CD7E87C09FF05FDF039F087FC149FE24EAEB00820F11E83A1E
          B91E070F8B45B466FF00BEEC9EBF1EFC2BE0FF0016F8EB5AB4F0DF823C2DE23F
          19788A73B60B0D2AC65BBB998FA24512B331FA0AFF004C0F8EDFB00FEC89FB4D
          FC4DD03E2F7C7AF82FA1FC4DF1C697A5268D6536A17573E44768B34932A3DB24
          8B149879A539756386C74AF7BF873F07FE10FC18D18E8BF0A3E19FC3EF85FA1A
          A00D0685A45BD84454777112A83D3A9AFD1F21FA4CE1B2DC9B0D81A586954AD4
          A118B6DA8C7DD5656B7337A25D11F9FE77F475C46619BE231B53111A74AA4E52
          4927296AEEEF7E54B5F367F0B5FB307FC103FF006E7F8F173A6EABF113C39A5F
          ECDFE0591834977E287CEA0F1F19F2B4D889943F3F766308F7AFEF834AD3E2D2
          74BD374A831E45B5BC76E9C63E5450A38FC2BF00FF00E0A1BFF05E8F827FB394
          3AF7C31FD9864D0FE3C7C6D4F32DA5D4A390C9A0787E60704C93211F6C954FFC
          B385B6020EE901050FEF0F817C40BE2DF04F83BC548E92AEA7A55A6A01946030
          96157C8F6F9ABF30F17B3FE24CD68E1B30CEE8AA34A5CFECE2959FD9BB69B72D
          74B37BDAE95B7FD1BC2AC9387F2CAD88C0E4F55D5AB1E5F6926EEBED5926AD1D
          35BDB6BD9BB9D5514515F879FB305145140051451401FCA4FF00C1C81FB14EB5
          AFD97817F6DBF0168D36A11695689E19F1BA5BC64B416BE616B3BE703F855E59
          20763D37DBF6048FE436BFD63FC45E1DD07C5DA06B5E15F14E8FA6F887C35A95
          ACB637F61790ACD05E5BC8A51E29636043232B1041E0835FC8DFEDD3FF0006E7
          78C6DBC45ADFC41FD8675BD2359F0C5CC8D707C0BAEDF7D9EEB4F639263B1BD9
          3F773479E152764651806490F35FD9FE03F8DD82C3E0A392E713F66E1A426FE1
          71FE593E8D746F4B69A5B5FE45F1B3C1CC657C64B38CA61CFCFACE0BE24FF9A2
          BAA7D52D6FAEB7D3F9A8F839F1B7E2B7C00F1CE8DF11FE0F78EBC49E02F16D8C
          F1CF1DCE9D772422708C1BCA9954812C471868DB2AC0904106BFD307F640FDA4
          7C31FB5C7ECDDF09FF00681F0A882DEDBC41A5A4D7B668FBBFB37504263BAB52
          7AFEEE6495013D4056E8457F997FC58F849F127E05FC40F11FC2BF8B9E0FD63C
          07F107499561D434BBE4025B72C81D4E54956565756575255958104839AFE9AB
          FE0DA6FDAA27D3FC51F183F63DF11EA2C74ED4203E33F0CA48DC47751EC86F61
          4CF778CDB4A1474F2253DCD7D9FD2378368E67922CE30A94AA51B4AEACF9A9CA
          D7D56E96924FB276DCF91F0038B6B65D9C3CA712DA856BC6CF4E59ADB47B37AC
          5AEF6BEC7F5F35F04FEDD1E26FDAC3E15699F0BFE367EC9FE0ED43E336A9A46B
          1FD9DE2AF87AAE02789B46B90034D0B9E61BAB79238CA4ABC04965DEAEA36D7C
          E3FF000537F8E3FB547EC34BA0FED8FF00043CAF8A5F04A17B7D27E23F80F562
          CD6F6D1B36CB7D52C675064B472CE20971BA325A063131DED5EB1FB10FFC156B
          F64FFDB96D6C745F0478AFFE102F8BAD1EEB8F0678824482FCB0196368F9F2EE
          D07273112E1465913A57F1FE59C2B8FC3E169E7D468AC46195D496AD45DAD28D
          44AD28E8EEA4B4D549493DBFAB331E26C1623133C92AD574311A38EC9BD6F194
          1BBA96AACE2F5D1A69A3EB4F80BF18BC55F18BC2E757F19FC05F8B3F00B5E8D5
          44FA578A52D0B331CE7C996DA795644E3AB046E465457F9E57FC1503E197C1AF
          845FB747ED05E0BF80DE27D1FC47F0F63D65EE521B1198744BB94092E74E56FB
          AC2099A48C6D24280109DCAC07F52BFF0005B0FF0082AF43FB2F785753FD983E
          00788236FDA2F5BB3DBACEA96B265BC15A7CA9905587DDBE951BF763AC487CCE
          09889FE1CA59649A49269A47966762CEEC72589EA49EE6BFA9FE8D9C158CC3FB
          5CF2A2F6346B2B429EAEEAF75377BBB2D546EEED36F6B5FF009A7E909C6184AF
          ECB26A6FDAD5A2EF2A9A2B3B5B955ACAEF796964D25BDEDFDA1FFC1B23E07974
          CFD9BFF68AF88B240D1A6B1E34B7D2A3723FD62D9D9249C7B037EC3F3A2BEF9F
          F82207C2897E14FF00C1373E03ADE5B9B6D57C462FBC59720AE37ADD5CB981BF
          1B78ED8E68AFE5BF16F3458CE25C6D78BBAE7715E91F77F43FA53C2DCB5E1387
          B07465A3E44FFF0002F7BF53EEFF00DA87E374FF00B36FECFBF163E3C43E06D6
          3E248F0AE912EB12E8D613A4135DC31906422470422A217918E090A8D80C700F
          F069FB6CFF00C1647F6BEFDB3E2D53C2577E2187E0DFC1BB8CC6DE15F0CCAF12
          DE447F86FAECE25BAE9CA7C911FF009E60F35FE863AE689A4F89745D63C39AFE
          9F6DAB685A85ACB657B6B32EE8EE60910A3C6E3BAB2B3023D0D7F99A7EDF7FB2
          6EBFFB15FED51F13FE056AB0DEBE8169766FFC377B30FF00909E8D312D6B306E
          8CC1731391C09229076AFDABE8BD84C9B138AAD4F174632C542D28396BEEECEC
          9E89C5D9DED7D77D0FC7FE9238ACDB0F86A33C2D59470D3BC66969EF6EAED6AD
          35756BDB4DB53E36AFF4C6FF008269FC50B7F8C3FB04FECA1E3886E16EA76F06
          5869774E0E7375649F629B3EFE65B495FC11FEC61FF04E8FDA83F6E8F12C3A77
          C1EF044F67E088E758B53F176ACAF6FA3E983386CCF8266907FCF1883BF4C803
          E61FDFCFEC1FFB2168FF00B0DFECDBE10FD9F346F1BEBDF1063D3E7B9BEB9D4A
          FA358849733BF99288215CF9506F2C550B391B892C49AFABFA53E779656C1D1C
          146AA78A84EFCAB56A2D34EF6D9DF96C9EAFB1F31F469C9B31A58BAD8C9526B0
          D385B99E979269AB77D2F76B4F33EC5A28A2BF894FEC50A28A2800A28A2800A2
          8A2803F0DFFE0B49FF0004C5B7FDB3FE15B7C64F847A1C27F69BF095939B48E1
          501FC59A62E5DB4F73DE65259E027F88B467890327F189FB1D7ED19AEFEC6BFB
          527C2BF8FB6BA15F6AB79E17D5246D4349F3CDAC97B6CF1C905CDB172ADB19A3
          9645E54E1B191C57FA81D7F203FF0005EFFF00825E8F0DDFEB9FB757C05F0F30
          D06F26127C45D1ECE2E2CAE1CE06AF1A2F48E46204E07DD7225E43C8CBFD63E0
          1F89B4A74DF0AE70EF46AA71836FF9B474DBED2BFBBD9E9D55BF983C6FF0EAAC
          2A2E26CA55AB526A534BFBBAA9A5DE36F7BBAD7A3BFACF8FBFE0E40FD97FE257
          803C51E00F19FEC89F137C51E1CD6F4E9F4BD574BBBD5ACFECF756D32149232E
          016C1562321411D4608AFE48355D4F4DB1F17EA5ACF801BC41E1FD222D464B9D
          18CF740DED942242D0EF9A30A0CCABB32EA17E6190074AE6E8AFEAEE0DF0EB2A
          C8635239641C554B5D394A4B4F26DA5FAF53F9878B78FB33CEDD3966325270BD
          9A8C62F5F34976FF00236BC47E24F1178C75ED5BC53E2DD7B59F14789AFE76B9
          BED4751BA7B8B9BC998E5A49657259D8F724935D77C1DF85FE24F8D9F15FE1BF
          C20F0840D71E27F136B767A1D900A48596799630ED8E8ABBB713D8293DABCDEB
          FA75FF0083727F62BBBF1A7C54F147EDA3E35D21D7C25E1559B43F0919A3F96F
          3589A3DB717099EAB041218F3D37DCF07319C6DC79C534722C9EB6612B2E48DA
          2BBC9E915F7DBD15DF432E09E1AAB9D66D4B031BBE795E4FB456B27F77DEEC8F
          EC0BE1E781F43F865E00F03FC37F0C41F66F0DF87F48B3D174F8F1F72DEDE158
          A31FF7CA2D15D8D15FE4ED5AB29C9CE6EEDEACFF004FE9D38C22A11564B4415F
          1F7ED27FB077ECB9FB5CF8C7E17F8E3E3FFC33B1F1EEB1E1269FFB356599E28A
          E63976930DDAA106784328711B1DA1B77043B06FB068AEBCBB33C4612AAAF85A
          8E1357578B69EAACF55AEA9D8E6C7E5D87C5527471305383B69249AD1DD68FB3
          573CB7C55E29F843FB377C28D4FC4FE25BEF07FC22F83FE19D3CC92BAC51D9D8
          E976C9C048E28C003248558D172CCC15412403F857FB07FF00C160F56FDB6BFE
          0A4DE36F859A5D9CFE14FD9EE7F085F43E0BD32E1145D5F5F5B4F14AD7B74464
          896583ED04440ED8D2351CB6E66FA4BFE0B25FF04F5F8B1FB767C19D03FE14E7
          C49D6B4DF18F85DE6BFB7F075CDE08B47F14B11FC7D025E200C2295C941BD94E
          C0E645FE3BBF614F14F8BFF641FF00828BFECF97BF11743D73C03E21D07C6F6D
          A1F886C35181ADEE34F82E98D9DC89636C1188AE5DB9E08C11C106BFA3FC2CF0
          EF28CD787F1B8B75155C6B84ED17BD36B58BB3D5B934BDED95EDBDCFE7DF12F8
          FB34CB33DC1E155374B06A71BC96D35A26B4D94537EEEEED7DAC7FA52D14515F
          CC47F4705145140051451400514514005676B1A3E93E21D2355D035ED36C759D
          0EFADA5B3BDB3BA896586EE0914A3C522302191958A952304120D68D14E3269D
          D6E294535667F9ED7FC15F7FE09A9AAFEC1FF1A3FE126F02D8DEDF7ECD7E2CBA
          967F0DDD1DCFFD8971CBC9A5CEE73F320CB44CC732443AB347263F1F2BFD4D3F
          693FD9DBE197ED57F05FC6FF00033E2DE8CBABF83F5BB53133A8027D3EE07315
          D5BB90764D1385756E9C6082A483FC39F847FE085DFB6378A3F6BAF187ECDB3E
          8C341F01685731CF7BF112EAD9D7489B4A90930DC5B7399EE1D01FF46462C8E1
          95D90297AFF403C1EF1C7098DCAE54B3AAAA15B0F1BB949FC715A2979CB64D2D
          5BB35BD97F0C78B1E0CE2B099946A65149CE8D776514BE096EE3E51DDA7B2574
          F6BBF87FF619FD8A7E2A7EDD7F1D342F83FF000E2CE6B3D25592EFC49AFC9096
          B5F0EE9C1B0F3CA7805CF2B1C59064720700332FFA42FC08F825F0FBF671F843
          E01F823F0B3485D17C0DE1DD3D2C2CA238324B8C979A56006F9A476791DF1F33
          BB1EF5E5DFB1E7EC6DF04FF622F845A6FC23F82FA00B4B61B67D5F57B90ADA87
          882F31837377280373750A83091AFCAA00AFAB2BF9A3C65F16AA71262D52A09C
          70B4DFBA9EF27FCD2F3ECBA2F36CFE88F093C2EA7C3D86752B352C4D45EF35B2
          5FCABCBBBEAFC920A28A2BF153F600A28A2800AFCE8FDBC7FE0997FB3CFEDE3A
          025DF8C74E7F027C66B1882E89E38D22255D42C997948EE0702EADC373E5B9CA
          F3B1E3249A28AF5326CEB1797E2238BC15474EA4766BFAD53EA9E8FA9E6E6D93
          E171F42585C6414E9CB74FFAD1AE8D6ABA1F7D785AD75CB1F0C7872CBC4F7F69
          AAF8961B0B78B51BAB742915CDCAC6A249114E485670C403C806B768A2BCD9CB
          99B7DCF4211B24828A28A92828A28A0028A28A0028A28A0028A28A0028A28A00
          28A28A00FFD9}
        Properties.Caption = 'teamliquid.net'
        Properties.GraphicClassName = 'TJPEGImage'
        Properties.PopupMenuLayout.MenuItems = []
        Properties.ReadOnly = True
        Properties.ShowFocusRect = False
        Style.BorderStyle = ebsNone
        Style.HotTrack = False
        Style.Shadow = False
        TabOrder = 2
        OnClick = teamliquidlogoClick
        Height = 161
        Width = 161
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

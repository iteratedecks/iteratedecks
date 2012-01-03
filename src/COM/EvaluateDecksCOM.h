// EvaluateDecksCOM.h : Declaration of the CEvaluateDecksCOM

#pragma once
#include "resource.h"       // main symbols



#include "IterateDecksCOM_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CEvaluateDecksCOM
#include "..\EXE\deck.h"
#include "..\EXE\cards.h"


class ATL_NO_VTABLE CEvaluateDecksCOM :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CEvaluateDecksCOM, &CLSID_EvaluateDecksCOM>,
	public IDispatchImpl<IEvaluateDecksCOM, &IID_IEvaluateDecksCOM, &LIBID_IterateDecksCOMLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
	CardDB DB;
	ActiveDeck Bot;
	ActiveDeck Top;
	
public:
	CEvaluateDecksCOM()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_EVALUATEDECKSCOM)

DECLARE_NOT_AGGREGATABLE(CEvaluateDecksCOM)

BEGIN_COM_MAP(CEvaluateDecksCOM)
	COM_INTERFACE_ENTRY(IEvaluateDecksCOM)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:



	STDMETHOD(LoadCardXML)(BSTR FileName, BYTE* ret);
	STDMETHOD(SetCommander)(USHORT DeckIndex, SHORT Id, BYTE* ret);
	STDMETHOD(AppendCard)(USHORT DeckIndex, USHORT Id, BYTE* ret);
	STDMETHOD(AppendCardEx)(USHORT DeckIndex, USHORT Id, USHORT Attack, SHORT Health, BYTE* ret);
	STDMETHOD(SetEffect)(USHORT DeckIndex, USHORT CardPosition, BSTR AbilityName, USHORT EffectValue, BYTE* ret);
	STDMETHOD(Attack)(USHORT DeckIndex);
	STDMETHOD(GetCardAttack)(USHORT DeckIndex, USHORT RowIndex, USHORT CardPosition, BYTE* ret);
	STDMETHOD(GetCardHealth)(USHORT DeckIndex, USHORT RowIndex, USHORT CardPosition, BYTE* ret);
	STDMETHOD(GetCardEffect)(USHORT DeckIndex, USHORT CardPosition, USHORT EffectNo, BSTR* EffectName, BYTE* ret);
};

OBJECT_ENTRY_AUTO(__uuidof(EvaluateDecksCOM), CEvaluateDecksCOM)

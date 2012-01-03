

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Jan 02 00:38:33 2012
 */
/* Compiler settings for IterateDecksCOM.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __IterateDecksCOM_i_h__
#define __IterateDecksCOM_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IEvaluateDecksCOM_FWD_DEFINED__
#define __IEvaluateDecksCOM_FWD_DEFINED__
typedef interface IEvaluateDecksCOM IEvaluateDecksCOM;
#endif 	/* __IEvaluateDecksCOM_FWD_DEFINED__ */


#ifndef __EvaluateDecksCOM_FWD_DEFINED__
#define __EvaluateDecksCOM_FWD_DEFINED__

#ifdef __cplusplus
typedef class EvaluateDecksCOM EvaluateDecksCOM;
#else
typedef struct EvaluateDecksCOM EvaluateDecksCOM;
#endif /* __cplusplus */

#endif 	/* __EvaluateDecksCOM_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IEvaluateDecksCOM_INTERFACE_DEFINED__
#define __IEvaluateDecksCOM_INTERFACE_DEFINED__

/* interface IEvaluateDecksCOM */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IEvaluateDecksCOM;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EE6EAD4E-AF5B-4356-90AA-1F0DE44C514E")
    IEvaluateDecksCOM : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LoadCardXML( 
            /* [in] */ BSTR FileName,
            /* [retval][out] */ BYTE *ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetCommander( 
            /* [in] */ USHORT DeckIndex,
            /* [in] */ SHORT Id,
            /* [retval][out] */ BYTE *ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AppendCard( 
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT Id,
            /* [retval][out] */ BYTE *ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AppendCardEx( 
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT Id,
            /* [in] */ USHORT Attack,
            /* [in] */ SHORT Health,
            /* [retval][out] */ BYTE *ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetEffect( 
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT CardPosition,
            /* [in] */ BSTR AbilityName,
            /* [in] */ USHORT EffectValue,
            /* [retval][out] */ BYTE *ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Attack( 
            /* [in] */ USHORT DeckIndex) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCardAttack( 
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT RowIndex,
            /* [in] */ USHORT CardPosition,
            /* [retval][out] */ BYTE *ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCardHealth( 
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT RowIndex,
            /* [in] */ USHORT CardPosition,
            /* [retval][out] */ BYTE *ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCardEffect( 
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT CardPosition,
            /* [in] */ USHORT EffectNo,
            /* [out] */ BSTR *EffectName,
            /* [retval][out] */ BYTE *ret) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEvaluateDecksCOMVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEvaluateDecksCOM * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEvaluateDecksCOM * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEvaluateDecksCOM * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IEvaluateDecksCOM * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IEvaluateDecksCOM * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IEvaluateDecksCOM * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IEvaluateDecksCOM * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LoadCardXML )( 
            IEvaluateDecksCOM * This,
            /* [in] */ BSTR FileName,
            /* [retval][out] */ BYTE *ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetCommander )( 
            IEvaluateDecksCOM * This,
            /* [in] */ USHORT DeckIndex,
            /* [in] */ SHORT Id,
            /* [retval][out] */ BYTE *ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AppendCard )( 
            IEvaluateDecksCOM * This,
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT Id,
            /* [retval][out] */ BYTE *ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AppendCardEx )( 
            IEvaluateDecksCOM * This,
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT Id,
            /* [in] */ USHORT Attack,
            /* [in] */ SHORT Health,
            /* [retval][out] */ BYTE *ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetEffect )( 
            IEvaluateDecksCOM * This,
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT CardPosition,
            /* [in] */ BSTR AbilityName,
            /* [in] */ USHORT EffectValue,
            /* [retval][out] */ BYTE *ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Attack )( 
            IEvaluateDecksCOM * This,
            /* [in] */ USHORT DeckIndex);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCardAttack )( 
            IEvaluateDecksCOM * This,
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT RowIndex,
            /* [in] */ USHORT CardPosition,
            /* [retval][out] */ BYTE *ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCardHealth )( 
            IEvaluateDecksCOM * This,
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT RowIndex,
            /* [in] */ USHORT CardPosition,
            /* [retval][out] */ BYTE *ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCardEffect )( 
            IEvaluateDecksCOM * This,
            /* [in] */ USHORT DeckIndex,
            /* [in] */ USHORT CardPosition,
            /* [in] */ USHORT EffectNo,
            /* [out] */ BSTR *EffectName,
            /* [retval][out] */ BYTE *ret);
        
        END_INTERFACE
    } IEvaluateDecksCOMVtbl;

    interface IEvaluateDecksCOM
    {
        CONST_VTBL struct IEvaluateDecksCOMVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEvaluateDecksCOM_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEvaluateDecksCOM_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEvaluateDecksCOM_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEvaluateDecksCOM_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IEvaluateDecksCOM_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IEvaluateDecksCOM_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IEvaluateDecksCOM_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IEvaluateDecksCOM_LoadCardXML(This,FileName,ret)	\
    ( (This)->lpVtbl -> LoadCardXML(This,FileName,ret) ) 

#define IEvaluateDecksCOM_SetCommander(This,DeckIndex,Id,ret)	\
    ( (This)->lpVtbl -> SetCommander(This,DeckIndex,Id,ret) ) 

#define IEvaluateDecksCOM_AppendCard(This,DeckIndex,Id,ret)	\
    ( (This)->lpVtbl -> AppendCard(This,DeckIndex,Id,ret) ) 

#define IEvaluateDecksCOM_AppendCardEx(This,DeckIndex,Id,Attack,Health,ret)	\
    ( (This)->lpVtbl -> AppendCardEx(This,DeckIndex,Id,Attack,Health,ret) ) 

#define IEvaluateDecksCOM_SetEffect(This,DeckIndex,CardPosition,AbilityName,EffectValue,ret)	\
    ( (This)->lpVtbl -> SetEffect(This,DeckIndex,CardPosition,AbilityName,EffectValue,ret) ) 

#define IEvaluateDecksCOM_Attack(This,DeckIndex)	\
    ( (This)->lpVtbl -> Attack(This,DeckIndex) ) 

#define IEvaluateDecksCOM_GetCardAttack(This,DeckIndex,RowIndex,CardPosition,ret)	\
    ( (This)->lpVtbl -> GetCardAttack(This,DeckIndex,RowIndex,CardPosition,ret) ) 

#define IEvaluateDecksCOM_GetCardHealth(This,DeckIndex,RowIndex,CardPosition,ret)	\
    ( (This)->lpVtbl -> GetCardHealth(This,DeckIndex,RowIndex,CardPosition,ret) ) 

#define IEvaluateDecksCOM_GetCardEffect(This,DeckIndex,CardPosition,EffectNo,EffectName,ret)	\
    ( (This)->lpVtbl -> GetCardEffect(This,DeckIndex,CardPosition,EffectNo,EffectName,ret) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEvaluateDecksCOM_INTERFACE_DEFINED__ */



#ifndef __IterateDecksCOMLib_LIBRARY_DEFINED__
#define __IterateDecksCOMLib_LIBRARY_DEFINED__

/* library IterateDecksCOMLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_IterateDecksCOMLib;

EXTERN_C const CLSID CLSID_EvaluateDecksCOM;

#ifdef __cplusplus

class DECLSPEC_UUID("29150E8B-3944-4957-ADD5-43BCB0625C60")
EvaluateDecksCOM;
#endif
#endif /* __IterateDecksCOMLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



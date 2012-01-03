// dllmain.h : Declaration of module class.

class CIterateDecksCOMModule : public ATL::CAtlDllModuleT< CIterateDecksCOMModule >
{
public :
	DECLARE_LIBID(LIBID_IterateDecksCOMLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ITERATEDECKSCOM, "{447D5EBF-D7A3-4043-B6FC-2F623FEEA5C6}")
};

extern class CIterateDecksCOMModule _AtlModule;

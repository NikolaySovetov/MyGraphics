#include "StdAfx.h"
#include "resource.h"
#include "icosahedron.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CShapesApp : public AcRxDbxApp {

public:
	CShapesApp () : AcRxDbxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxDbxApp::On_kInitAppMsg (pkt) ;	
		acrxRegisterService(SHAPES_DBXSERVICE);

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		AcRx::AppRetCode retCode =AcRxDbxApp::On_kUnloadAppMsg (pkt) ;
		delete acrxServiceDictionary->remove(SHAPES_DBXSERVICE);

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CShapesApp)


#include "StdAfx.h"
#include "resource.h"
#include "utilities.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CHardGraphicsApp : public AcRxArxApp {

public:
	CHardGraphicsApp() : AcRxArxApp() {}

	virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt) {
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);

		AcRxObject* pSvc;
		if (!(pSvc = acrxServiceDictionary->at(SHAPES_DBXSERVICE))) {
			// Try to load the module, if it is not yet present 
			if (!acrxDynamicLinker->loadModule(_T("Sahapes.dbx"), 0)) {
				acutPrintf(_T("Unable to load \"Sahapes.dbx\". Unloading this application...\n"));
				return (AcRx::kRetError);
			}
		}

		// TODO: Add your initialization code here

		return (retCode);
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt) {
		AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

		// TODO: Unload dependencies here

		return (retCode);
	}

	virtual void RegisterServerComponents() {
	}

	static void Shapes_icosahedron() {
		try	{
			SymbolTableWrapper blockTable;
			AcDbBlockTable* pBTable = blockTable.GetBlockTable(AcDb::kForWrite);

			SymbolTableRecordWrapper blockTableRecord;
			AcDbBlockTableRecord* pBTRecord =
				blockTableRecord.GetBlockTableRecord(pBTable, ACDB_MODEL_SPACE, AcDb::kForWrite);

			auto icos{ std::make_unique<Icosahedron>() };
			AppendEntity(pBTRecord, icos);
		}
		catch (const std::exception& e){
			acutPrintf(_T("\nEXCEPTION: %s"), e.what());
		}
	}

};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CHardGraphicsApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CHardGraphicsApp, Shapes, _icosahedron, icosahedron, ACRX_CMD_TRANSPARENT, NULL)


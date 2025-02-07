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

	static void Shapes_MyIcosahedron() {
		try	{
			SymbolTableWrapper blockTable;
			AcDbBlockTable* pBTable = blockTable.GetBlockTable(AcDb::kForWrite);

			SymbolTableRecordWrapper blockTableRecord;
			AcDbBlockTableRecord* pBTRecord =
				blockTableRecord.GetBlockTableRecord(pBTable, ACDB_MODEL_SPACE, AcDb::kForWrite);

			auto icos{ std::make_unique<MyTruncIcosahedron>() };
			AppendEntity(pBTRecord, icos);
		}
		catch (const std::exception& e){
			acutPrintf(_T("\nEXCEPTION: %s"), e.what());
		}
	}

	static void Shapes_MyCube() {
		try {
			SymbolTableWrapper blockTable;
			AcDbBlockTable* pBTable = blockTable.GetBlockTable(AcDb::kForWrite);

			SymbolTableRecordWrapper blockTableRecord;
			AcDbBlockTableRecord* pBTRecord =
				blockTableRecord.GetBlockTableRecord(pBTable, ACDB_MODEL_SPACE, AcDb::kForWrite);

			auto cube{ std::make_unique<MyCube>() };
			AppendEntity(pBTRecord, cube);
		}
		catch (const std::exception& e) {
			acutPrintf(_T("\nEXCEPTION: %s"), e.what());
		}
	}

	static void Shapes_MyTetrahedron() {
		try {
			SymbolTableWrapper blockTable;
			AcDbBlockTable* pBTable = blockTable.GetBlockTable(AcDb::kForWrite);

			SymbolTableRecordWrapper blockTableRecord;
			AcDbBlockTableRecord* pBTRecord =
				blockTableRecord.GetBlockTableRecord(pBTable, ACDB_MODEL_SPACE, AcDb::kForWrite);

			auto tetrahedron{ std::make_unique<MyTetrahedron>() };
			AppendEntity(pBTRecord, tetrahedron);
		}
		catch (const std::exception& e) {
			acutPrintf(_T("\nEXCEPTION: %s"), e.what());
		}
	}

	static void Shapes_MySphere() {
		try {
			SymbolTableWrapper blockTable;
			AcDbBlockTable* pBTable = blockTable.GetBlockTable(AcDb::kForWrite);

			SymbolTableRecordWrapper blockTableRecord;
			AcDbBlockTableRecord* pBTRecord =
				blockTableRecord.GetBlockTableRecord(pBTable, ACDB_MODEL_SPACE, AcDb::kForWrite);

			auto sphere{ std::make_unique<MySphere>() };
			AppendEntity(pBTRecord, sphere);
		}
		catch (const std::exception& e) {
			acutPrintf(_T("\nEXCEPTION: %s"), e.what());
		}
	}

	static void Shapes_MyComposition() {
		try {
			SymbolTableWrapper blockTable;
			AcDbBlockTable* pBTable = blockTable.GetBlockTable(AcDb::kForWrite);

			SymbolTableRecordWrapper blockTableRecord;
			AcDbBlockTableRecord* pBTRecord =
				blockTableRecord.GetBlockTableRecord(pBTable, ACDB_MODEL_SPACE, AcDb::kForWrite);

			ShapesBuilder(pBTRecord, AcGePoint3d(0, 0, 0), 1.0, 3);


			/*
			SymbolTableWrapper blockTable;
			AcDbBlockTable* pBTable = blockTable.GetBlockTable(AcDb::kForWrite);

			SymbolTableRecordWrapper blockTableRecord;
			AcDbBlockTableRecord* pBTRecord =
				blockTableRecord.GetBlockTableRecord(pBTable, ACDB_MODEL_SPACE, AcDb::kForWrite);

			auto icos{ std::make_unique<MyTruncIcosahedron>(AcGePoint3d(0, 0, 0 ), 2.0, 3) };
			double inscrRadius{};
			icos->InscribedRadius(inscrRadius);

			auto cube{ std::make_unique<MyCube>(AcGePoint3d(0, 0, 0), inscrRadius, 1 ) };
			auto tetr{ std::make_unique<MyTetrahedron>(AcGePoint3d(0, 0, 0), inscrRadius, 2 ) };
			
			auto sphere{ std::make_unique<MySphere>(AcGePoint3d(0, 0, 0), inscrRadius, 6) };

			tetr->InscribedRadius(inscrRadius);
			auto icos02{ std::make_unique<MyTruncIcosahedron>(AcGePoint3d(0, 0, 0), 1.0, 3) };
			icos02->SetInscribedRadius(inscrRadius);

			AppendEntity(pBTRecord, icos);
			AppendEntity(pBTRecord, sphere);
			AppendEntity(pBTRecord, cube);
			AppendEntity(pBTRecord, tetr);
			AppendEntity(pBTRecord, icos02);
			*/
		}
		catch (const std::exception& e) {
			acutPrintf(_T("\nEXCEPTION: %s"), e.what());
		}
	}

};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CHardGraphicsApp)

ACED_ARXCOMMAND_ENTRY_AUTO(CHardGraphicsApp, Shapes, _MyIcosahedron, MyIcosahedron, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CHardGraphicsApp, Shapes, _MyCube, MyCube, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CHardGraphicsApp, Shapes, _MyTetrahedron, MyTetrahedron, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CHardGraphicsApp, Shapes, _MySphere, MySphere, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CHardGraphicsApp, Shapes, _MyComposition, MyComposition, ACRX_CMD_TRANSPARENT, NULL)


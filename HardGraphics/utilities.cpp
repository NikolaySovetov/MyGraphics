#include "StdAfx.h"
#include "utilities.h"
#include <stdexcept>

SymbolTableWrapper::SymbolTableWrapper()
	: m_pSTable{ nullptr } {
}

SymbolTableWrapper::~SymbolTableWrapper() {
	if (m_pSTable) {
		m_pSTable->close();
	}
}

AcDbBlockTable*
SymbolTableWrapper::GetBlockTable(AcDb::OpenMode mode) {
	if (IsOccupied()) {
		throw std::runtime_error("Attempt to redefine the SymbolTable");
	}

	Acad::ErrorStatus es;
	AcDbBlockTable* pBTable;
	es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBTable, mode);
	if (es != Acad::eOk) {
		throw std::runtime_error("Can't open BlockTable");
	}
	m_pSTable = pBTable;
	return pBTable;
}

bool SymbolTableWrapper::IsOccupied() const {
	return m_pSTable;
}

//--------------------------------------------------------------------
SymbolTableRecordWrapper::SymbolTableRecordWrapper()
	: m_pSTRecord{ nullptr } {
}

SymbolTableRecordWrapper::~SymbolTableRecordWrapper() {
	if (m_pSTRecord) {
		m_pSTRecord->close();
	}
}

AcDbBlockTableRecord*
SymbolTableRecordWrapper::GetBlockTableRecord
(const AcDbBlockTable* pBTable, const ACHAR* entryName, AcDb::OpenMode mode) {
	if (IsOccupied()) {
		throw std::runtime_error("Attempt to redefine the SymbolTableRecord");
	}

	Acad::ErrorStatus es;
	AcDbBlockTableRecord* pBTRecord;
	es = pBTable->getAt(entryName, pBTRecord, mode);
	if (es != Acad::eOk) {
		throw std::runtime_error("Can't open BlockTableRecord");
	}
	m_pSTRecord = pBTRecord;
	return pBTRecord;
}

bool SymbolTableRecordWrapper::IsOccupied() const {
	return m_pSTRecord;
}

void ShapesBuilder
(AcDbBlockTableRecord* pBTRecord,
	const AcGePoint3d& center, const double& radius,
	int geometricEmbeddings, int count) 
{
	if (count >= geometricEmbeddings) {
		return;
	}
	++count;

	auto icosahedron{ std::make_unique<MyTruncIcosahedron>(center, 1.0, 4) };
	icosahedron->SetInscribedRadius(radius);
	double nextRadius{};
	icosahedron->InscribedRadius(nextRadius);

	auto sphere{ std::make_unique<MySphere>(center, nextRadius, 1) };
	
	auto cube{ std::make_unique<MyCube>(center, nextRadius, 2) };
	
	auto tetrahedron{ std::make_unique<MyTetrahedron>(center, nextRadius, 3) };
	tetrahedron->InscribedRadius(nextRadius);

	AppendEntity(pBTRecord, icosahedron);
	AppendEntity(pBTRecord, sphere);
	AppendEntity(pBTRecord, cube);
	AppendEntity(pBTRecord, tetrahedron);

	ShapesBuilder(pBTRecord, center, nextRadius, geometricEmbeddings, count);

}



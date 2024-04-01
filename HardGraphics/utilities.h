#pragma once

#include "icosahedron.h"
#include "cube.h"
#include "tetrahedron.h"
#include "sphere.h"
#include <stdexcept>
#include <memory>
#pragma comment (lib ,"Shapes.lib")

class SymbolTableWrapper {
private:
	AcDbSymbolTable* m_pSTable;

public:
	SymbolTableWrapper();
	~SymbolTableWrapper();
	AcDbBlockTable* GetBlockTable(AcDb::OpenMode mode);
	bool IsOccupied() const;
};

class SymbolTableRecordWrapper {
private:
	AcDbSymbolTableRecord* m_pSTRecord;

public:
	SymbolTableRecordWrapper();
	~SymbolTableRecordWrapper();
	AcDbBlockTableRecord*
		GetBlockTableRecord(const AcDbBlockTable* pBTable, const ACHAR* entryName, AcDb::OpenMode mode);
	bool IsOccupied() const;
};

class EntityWrapper {
private:
	AcDbEntity* m_pEntity;

public:
	EntityWrapper() {
		m_pEntity = nullptr;
	}
	EntityWrapper(AcDbEntity* pEnt) {
		m_pEntity = pEnt;
	}
	~EntityWrapper() {
		if (m_pEntity) {
			m_pEntity->close();
		}
	}
	AcDbEntity* Get() {
		return m_pEntity;
	}
	const AcDbEntity* Get() const {
		return m_pEntity;
	}
};

template<typename T>
void AppendEntity(AcDbBlockTableRecord* pBTRecord, std::unique_ptr<T>& pEntity) {
	if (!pBTRecord || !pEntity) {
		throw std::runtime_error("Can't add the Entity to the SymbolTableRecord");
	}

	Acad::ErrorStatus es;
	es = pBTRecord->appendAcDbEntity(pEntity.get());
	if (es != Acad::eOk) {
		throw std::runtime_error("Can't add the Entity to the SymbolTableRecord");
	}
	pEntity->close();
	pEntity.release();
}











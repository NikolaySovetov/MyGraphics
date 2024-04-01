#pragma once
#include <vector>
#include <utility>

#ifdef SHAPES_MODULE
#define DLLIMPEXP __declspec(dllexport)
#else
//----- Note: we don't use __declspec(dllimport) here, because of the
//----- "local vtable" problem with msvc. If you use __declspec(dllimport),
//----- then, when a client dll does a new on the class, the object's
//----- vtable pointer points to a vtable allocated in that client
//----- dll. If the client dll then passes the object to another dll,
//----- and the client dll is then unloaded, the vtable becomes invalid
//----- and any virtual calls on the object will access invalid memory.
//-----
//----- By not using __declspec(dllimport), we guarantee that the
//----- vtable is allocated in the server dll during the ctor and the
//----- client dll does not overwrite the vtable pointer after calling
//----- the ctor. And, since we expect the server dll to remain in
//----- memory indefinitely, there is no problem with vtables unexpectedly
//----- going away.
#define DLLIMPEXP
#endif

//-----------------------------------------------------------------------------
#include "dbmain.h"

#define SHAPES_DBXSERVICE _T("SHAPES_DBXSERVICE")

//-----------------------------------------------------------------------------
class DLLIMPEXP MyTruncIcosahedron : public AcDbEntity {
private:
	AcGePoint3d m_center;
	double m_circumradius;
	int m_color;
 
public:
	ACRX_DECLARE_MEMBERS(MyTruncIcosahedron) ;

protected:
	static Adesk::UInt32 kCurrentVersionNumber ;

public:
	MyTruncIcosahedron () ;
	MyTruncIcosahedron (const AcGePoint3d& center, const double& radius, int color) ;
	virtual ~MyTruncIcosahedron () = default;

	virtual Acad::ErrorStatus dwgOutFields (AcDbDwgFiler *pFiler) const ;
	virtual Acad::ErrorStatus dwgInFields (AcDbDwgFiler *pFiler) ;

	Acad::ErrorStatus Circumradius(double& circumRadius) const;
	Acad::ErrorStatus InscribedRadius(double& inscrRadius) const;
	Acad::ErrorStatus EdgeLength(double& edgeLen) const;
	Acad::ErrorStatus SetInscribedRadius(const double& inscrRadius);

protected:
	virtual Adesk::Boolean subWorldDraw (AcGiWorldDraw *mode) ;
	virtual Adesk::UInt32 subSetAttributes (AcGiDrawableTraits *traits) ;

	Acad::ErrorStatus Vertices(AcArray<AcGePoint3d>& points);
	Acad::ErrorStatus PointsOfEdges(std::vector<AcArray<AcGePoint3d>>& edgesPoints);
} ;

#ifdef SHAPES_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(MyTruncIcosahedron)
#endif

#include "StdAfx.h"
#include "cube.h"


//-----------------------------------------------------------------------------
Adesk::UInt32 MyCube::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	MyCube, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, UBE,
	SHAPESAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
MyCube::MyCube()
	: AcDbEntity(), m_center{ 0, 0, 0 }, m_circumradius{ 1.0 }, m_color{ 0 }
{
	this->PointsOfEdges(m_edgesPoints);
}

MyCube::MyCube(const AcGePoint3d & center, const double& radius, int color)
	: AcDbEntity(), m_center{ center }, m_circumradius{ radius }, m_color{ color }
{
	this->PointsOfEdges(m_edgesPoints);
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus MyCube::dwgOutFields(AcDbDwgFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(MyCube::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	pFiler->writeItem(m_center);
	pFiler->writeItem(m_circumradius);
	pFiler->writeItem(m_color);

	return (pFiler->filerStatus());
}

Acad::ErrorStatus MyCube::dwgInFields(AcDbDwgFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbEntity::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > MyCube::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < Cube::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....
	switch (version) {
	case 1:
		pFiler->readItem(&m_center);
		pFiler->readItem(&m_circumradius);
		pFiler->readItem(&m_color);
		break;
	}

	return (pFiler->filerStatus());
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean MyCube::subWorldDraw(AcGiWorldDraw * mode) {
	assertReadEnabled();

	for (const auto& edge : m_edgesPoints) {
		mode->subEntityTraits().setColor(m_color);
		mode->geometry().polyline(edge.length(), edge.asArrayPtr());
	}

	return (AcDbEntity::subWorldDraw(mode));
}

Adesk::UInt32 MyCube::subSetAttributes(AcGiDrawableTraits * traits) {
	assertReadEnabled();
	return (AcDbEntity::subSetAttributes(traits));
}

Acad::ErrorStatus MyCube::Circumradius(double& circRadius) const {
	assertReadEnabled();
	circRadius = m_circumradius;
	return Acad::eOk;
}

Acad::ErrorStatus MyCube::InscribedRadius(double& inscrRadius) const {
	assertReadEnabled();
	double edgeLen;
	this->EdgeLength(edgeLen);
	inscrRadius = edgeLen / 2.0;
	return Acad::eOk;
}

Acad::ErrorStatus MyCube::EdgeLength(double& edgeLen) const {
	assertReadEnabled();
	edgeLen = std::sqrt(4.0 * std::pow(m_circumradius, 2.0) / 3.0);
	return Acad::eOk;
}

Acad::ErrorStatus MyCube::Vertices(AcArray<AcGePoint3d>&points) {
	//------------------------------------------------------------------------//
	//    For a cube centered at the origin, with edges parallel to the axes  // 
	//    and with an edge length of 2, the Cartesian coordinates of the      //
	//    vertices are :                                                      // 
	//	  ( +-1, +-1, +-1)													  //
	//																		  //	
	//	  while the interior consists of all points (x0, x1, x2)			  //	
	//    with −1 < xi < 1 for all i.										  //	
	//																		  //
	//										            WikipediA             // 
	//------------------------------------------------------------------------// 

	assertWriteEnabled();

	double edgeLen;
	this->EdgeLength(edgeLen);

	struct signMatrix {
		double xSign{};
		double ySign{};
		double zSign{};
	};

	std::vector<signMatrix> sMatrix{
		{ 1.0,  1.0,  1.0},
		{-1.0,  1.0,  1.0},
		{ 1.0, -1.0,  1.0},
		{ 1.0,  1.0, -1.0}
	};

	for (const auto& matrix : sMatrix) {
		double x{ (edgeLen / 2.0) * matrix.xSign };
		double y{ (edgeLen / 2.0) * matrix.ySign };
		double z{ (edgeLen / 2.0) * matrix.zSign };

		points.append({ x, y, z });
	}

	for (auto& matrix : sMatrix) {
		matrix.xSign *= (-1.0);
		matrix.ySign *= (-1.0);
		matrix.zSign *= (-1.0);
	}

	for (const auto& matrix : sMatrix) {
		double x{ (edgeLen / 2.0) * matrix.xSign };
		double y{ (edgeLen / 2.0) * matrix.ySign };
		double z{ (edgeLen / 2.0) * matrix.zSign };

		points.append({ x, y, z });
	}

	for (auto& coord : points) {
		coord += m_center.asVector();
	}

	return Acad::eOk;
}

Acad::ErrorStatus
MyCube::PointsOfEdges(std::vector<AcArray<AcGePoint3d>>&pointsOfEdges) {
	assertWriteEnabled();

	AcArray<AcGePoint3d> points;
	this->Vertices(points);
	double edgeLength{};
	this->EdgeLength(edgeLength);
	double distance{ 0 };
	AcGeTol tol;

	for (int i{ 0 }; i < points.length() - 1; ++i) {
		for (int j{ i + 1 }; j < points.length(); ++j) {
			distance = points[i].distanceTo(points[j]);
			if (abs(distance - edgeLength) < tol.equalPoint()) {
				AcArray<AcGePoint3d> pArr;
				pArr.append(points[i]);
				pArr.append(points[j]);
				pointsOfEdges.push_back(pArr);
			}
		}
	}

	return Acad::eOk;
}







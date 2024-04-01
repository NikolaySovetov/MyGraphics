﻿#include "StdAfx.h"
#include "tetrahedron.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 MyTetrahedron::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	MyTetrahedron, AcDbObject,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, MYTETRAHEDRON,
	SHAPESAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
MyTetrahedron::MyTetrahedron()
	: AcDbEntity(), m_center{ 0, 0, 0 }, m_circumradius{ 1.0 }, m_color{ 0 }
{
	this->PointsOfEdges(m_edgesPoints);
}

MyTetrahedron::MyTetrahedron(const AcGePoint3d & center, const double& radius, int color)
	: AcDbEntity(), m_center{ center }, m_circumradius{ radius }, m_color{ color }
{
	this->PointsOfEdges(m_edgesPoints);
}


//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus MyTetrahedron::dwgOutFields(AcDbDwgFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(MyTetrahedron::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....
	pFiler->writeItem(m_center);
	pFiler->writeItem(m_circumradius);
	pFiler->writeItem(m_color);

	return (pFiler->filerStatus());
}

Acad::ErrorStatus MyTetrahedron::dwgInFields(AcDbDwgFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > MyTetrahedron::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < MyTetrahedron::kCurrentVersionNumber )
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
Adesk::Boolean MyTetrahedron::subWorldDraw(AcGiWorldDraw * mode) {
	assertReadEnabled();

	for (const auto& edge : m_edgesPoints) {
		mode->subEntityTraits().setColor(m_color);
		mode->geometry().polyline(edge.length(), edge.asArrayPtr());
	}

	return (AcDbEntity::subWorldDraw(mode));
}

//Adesk::UInt32 MyTetrahedron::subSetAttributes(AcGiDrawableTraits * traits) {
//	assertReadEnabled();
//	return (AcDbEntity::subSetAttributes(traits));
//}

Acad::ErrorStatus MyTetrahedron::Circumradius(double& circRadius) const {
	assertReadEnabled();
	circRadius = m_circumradius;
	return Acad::eOk;
}

Acad::ErrorStatus MyTetrahedron::InscribedRadius(double& inscrRadius) const {
	assertReadEnabled();
	inscrRadius = 1.0 / 3.0 * m_circumradius;
	return Acad::eOk;
}

Acad::ErrorStatus MyTetrahedron::EdgeLength(double& edgeLen) const {
	assertReadEnabled();
	edgeLen = m_circumradius / (std::sqrt(6.0) / 4.0);
	return Acad::eOk;
}

Acad::ErrorStatus MyTetrahedron::Vertices(AcArray<AcGePoint3d>&points) {

	// Create the tetrahedron from CUBE vertices

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

	AcArray<AcGePoint3d> cubeVertices;
	double cubeEdgeLen{ std::sqrt(4.0 * std::pow(m_circumradius, 2.0) / 3.0) };
	{
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
			double x{ (cubeEdgeLen / 2.0) * matrix.xSign };
			double y{ (cubeEdgeLen / 2.0) * matrix.ySign };
			double z{ (cubeEdgeLen / 2.0) * matrix.zSign };

			cubeVertices.append({ x, y, z });
		}

		for (auto& matrix : sMatrix) {
			matrix.xSign *= (-1.0);
			matrix.ySign *= (-1.0);
			matrix.zSign *= (-1.0);
		}

		for (const auto& matrix : sMatrix) {
			double x{ (cubeEdgeLen / 2.0) * matrix.xSign };
			double y{ (cubeEdgeLen / 2.0) * matrix.ySign };
			double z{ (cubeEdgeLen / 2.0) * matrix.zSign };

			cubeVertices.append({ x, y, z });
		}

		for (auto& coord : cubeVertices) {
			coord += m_center.asVector();
		}
	}

	double edgeLen{};
	EdgeLength(edgeLen);
	points.append(cubeVertices[0]);
	AcGeTol tol;

	for (int i{ 0 }; i < cubeVertices.length(); ++i) {
		double distance = cubeVertices[0].distanceTo(cubeVertices[i]);
		if (abs(distance - edgeLen) < tol.equalPoint()) {
			points.append(cubeVertices[i]);
		}
	}

	return Acad::eOk;
}

Acad::ErrorStatus
MyTetrahedron::PointsOfEdges(std::vector<AcArray<AcGePoint3d>>&pointsOfEdges) {
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
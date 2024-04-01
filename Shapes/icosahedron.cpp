#include "StdAfx.h"
#include "icosahedron.h"
#include <vector>
#include <memory>

//-----------------------------------------------------------------------------
Adesk::UInt32 Icosahedron::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	Icosahedron, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, ICOSAHEDRON,
	SHAPESAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
Icosahedron::Icosahedron() : AcDbEntity(), m_center{ 0, 0, 0 }, m_circumradius{ 1.0 }
{
	this->Vertices(m_vertices);
	this->PointsOfEdges(m_edgesPoints);
	this->Polygonos(m_polygonos);
}

Icosahedron::Icosahedron(const AcGePoint3d & center, double radius)
	: AcDbEntity(), m_center{ center }, m_circumradius{ radius }
{
	this->Vertices(m_vertices);
	this->PointsOfEdges(m_edgesPoints);
	this->Polygonos(m_polygonos);
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus Icosahedron::dwgOutFields(AcDbDwgFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbEntity::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(Icosahedron::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	pFiler->writeItem(m_center);
	pFiler->writeItem(m_circumradius);

	return (pFiler->filerStatus());
}

Acad::ErrorStatus Icosahedron::dwgInFields(AcDbDwgFiler * pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbEntity::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > Icosahedron::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < Icosahedron::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	switch (version) {
	case 1:
		pFiler->readItem(&m_center);
		pFiler->readItem(&m_circumradius);
		break;
	}

	return (pFiler->filerStatus());
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean Icosahedron::subWorldDraw(AcGiWorldDraw * mode) {
	assertReadEnabled();

	Adesk::UInt16 color{ 1 };

	for (const auto& poligon : m_polygonos) {
		mode->subEntityTraits().setColor(color);
		mode->geometry().polyline(poligon.length(), poligon.asArrayPtr());
		color = (++color % 7);
	}

	return (AcDbEntity::subWorldDraw(mode));
}

Adesk::UInt32 Icosahedron::subSetAttributes(AcGiDrawableTraits * traits) {
	assertReadEnabled();
	return (AcDbEntity::subSetAttributes(traits));
}

Acad::ErrorStatus Icosahedron::Circumradius(double& circRadius) const {
	assertReadEnabled();
	circRadius = m_circumradius;
	return Acad::eOk;
}

Acad::ErrorStatus Icosahedron::MidRadius(double& midRadius) const {
	assertReadEnabled();
	const double midRadiusCoefficient{ 0.914958381743 };
	midRadius = m_circumradius * midRadiusCoefficient;
	return Acad::eOk;
}

Acad::ErrorStatus Icosahedron::InscribedRadius(double& inscrRadius) const {
	assertReadEnabled();
	const double inscrribedRadiusCoefficient{ 0.914958381743 };
	inscrRadius = m_circumradius * inscrribedRadiusCoefficient;
	return Acad::eOk;
}

Acad::ErrorStatus Icosahedron::EdgeLength(double& edgeLen) const {
	assertReadEnabled();
	const double edgelengthCoefficient{ 0.403548212335 };
	edgeLen = m_circumradius * edgelengthCoefficient;
	return Acad::eOk;
}

Acad::ErrorStatus Icosahedron::Vertices(AcArray<AcGePoint3d>&points) {
	//------------------------------------------------------------------------//
	//    Cartesian coordinates for the vertices of a truncated               //
	//    icosahedron centered at the origin are all even permutations of:    // 
	//	  (   0,        +-1,        -3fi),                                    //
	//    ( +-1, +-(2 + fi),       +-2fi),                                    //
	//    (+-fi,        +-2, +-(2fi + 1 )	                                  //  
	//	  fi - golden mean 	                                                  //
	//										            WikipediA             // 
	//------------------------------------------------------------------------// 

	assertWriteEnabled();

	const double scaleCoefficient{ 0.201774106167 };
	double scale{ m_circumradius * scaleCoefficient };
	const double fi{ (1.0 + sqrt(5.0)) / 2.0 };

	struct SignMask {
		double a;
		double b;
		double c;
	};

	std::vector<SignMask> signPermutation01{
		{ 1,  1,  1},
		{-1,  1,  1},
		{ 1, -1,  1},
		{ 1,  1, -1},
		{-1, -1,  1},
		{ 1, -1, -1},
		{-1,  1, -1},
		{-1, -1, -1}
	};

	std::vector<SignMask> signPermutation02{
		{ 1,  1,  1},
		{-1,  1,  1},
		{ 1, -1,  1},
		{-1, -1,  1}
	};

	for (auto& mask : signPermutation01) {
		mask.a *= scale;
		mask.b *= scale;
		mask.c *= scale;
	}

	for (auto& mask : signPermutation02) {
		mask.a *= scale;
		mask.b *= scale;
		mask.c *= scale;
	}

	double x{};
	double y{};
	double z{};

	for (const auto& i : signPermutation01) {
		x = i.a * 1.0;
		y = i.b * (2 + fi);
		z = i.c * 2 * fi;
		points.append(AcGePoint3d(x, y, z));
		points.append(AcGePoint3d(y, z, x));
		points.append(AcGePoint3d(z, x, y));
	}

	for (const auto& i : signPermutation01) {
		x = i.a * fi;
		y = i.b * 2;
		z = i.c * (2 * fi + 1);
		points.append(AcGePoint3d(x, y, z));
		points.append(AcGePoint3d(y, z, x));
		points.append(AcGePoint3d(z, x, y));
	}

	for (const auto& i : signPermutation02) {
		x = 0.0;
		y = i.a * 1.0;
		z = i.b * 3 * fi;
		points.append(AcGePoint3d(x, y, z));
		points.append(AcGePoint3d(y, z, x));
		points.append(AcGePoint3d(z, x, y));
	}

	for (int i{ 0 }; i < points.length(); ++i) {
		points[i] += m_center.asVector();
	}

	return Acad::eOk;
}

Acad::ErrorStatus Icosahedron::Edges(AcArray<AcGeLine3d>&edges) const {
	assertReadEnabled();

	double edgeLength{};
	this->EdgeLength(edgeLength);
	double distance{ 0 };
	AcGeTol tol;

	for (int i{ 0 }; i < m_vertices.length() - 1; ++i) {
		for (int j{ i + 1 }; j < m_vertices.length(); ++j) {
			distance = m_vertices[i].distanceTo(m_vertices[j]);
			if (abs(distance - edgeLength) < tol.equalPoint()) {
				AcGeLine3d edge{ m_vertices[i], m_vertices[j] };
				edges.append(edge);
			}
		}
	}

	return Acad::eOk;
}

Acad::ErrorStatus
Icosahedron::PointsOfEdges(std::vector<AcArray<AcGePoint3d>>&pointsOfEdges) {
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

//inline bool isDuplicates(AcArray<AcGePoint>) {}

Acad::ErrorStatus Icosahedron::Planes(AcArray<AcGePlane>&planes) {
	assertReadEnabled();

	AcGeTol tol;

	AcArray<AcGePlane> planesWithDuplicates;
	for (int i{ 0 }; i < m_edgesPoints.size() - 1; ++i) {
		for (int j{ i + 1 }; j < m_edgesPoints.size(); ++j) {
			AcGePoint3d a{ m_edgesPoints[i][0] };
			AcGePoint3d b{ m_edgesPoints[j][0] };
			if (abs(a.distanceTo(b)) < tol.equalPoint()) {
				AcGePoint3d c{ m_edgesPoints[i][1] };
				AcGePoint3d d{ m_edgesPoints[j][1] };
				planesWithDuplicates.append(AcGePlane(a, c, d));
			}
		}
	}

	// Search duplicates plane from center point 
	bool duplicatFlag{};
	for (int i{ 0 }; i < planesWithDuplicates.length(); ++i) {
		AcGePoint3d a{ planesWithDuplicates[i].closestPointTo(m_center) };
		duplicatFlag = false;
		for (auto& plane : planes) {
			AcGePoint3d b{ plane.closestPointTo(m_center) };
			if (a.distanceTo(b) < tol.equalPoint()) {
				duplicatFlag = true;
				break;
			}
		}
		if (!duplicatFlag) {
			planes.append(planesWithDuplicates[i]);
		}
	}

	//	bool duplicatFlag{};
	//	for (int i{ 0 }; i < icosahedronPlanes.length(); ++i) {
	//		AcGePoint3d temporaryPoint{ icosahedronPlanes[i].closestPointTo(icosOrigin) };
	//		duplicatFlag = false;
	//		for (auto& planes : icosPlanes) {
	//			if (planes.first.distanceTo(temporaryPoint) < tolerance.equalPoint()) {
	//				duplicatFlag = true;
	//			}
	//		}
	//		if (!duplicatFlag) {
	//			AcGeVector3d v(temporaryPoint.asVector());
	//			std::pair<AcGePoint3d, AcGeVector3d> pair(temporaryPoint, v.normalize());
	//			icosPlanes.push_back(pair);
	//		}
	//	}




	return Acad::eOk;
}

void CreateChainByDistance(AcArray<AcGePoint3d>&source, AcArray<AcGePoint3d>&dist, double distance) {

	struct SameSTDOptionalLogic {
		bool empty{};
		AcGePoint3d point{};
	};

	std::vector<SameSTDOptionalLogic> tempArr;
	for (const auto& point : source) {
		tempArr.push_back({ false, point });
	}

	AcGeTol tol;
	tempArr[0].empty = true;
	AcGePoint3d tail{ tempArr[0].point };
	dist.append(tail);

	for (int i{ 1 }; i < tempArr.size(); ++i) {
		for (auto& point : tempArr) {
			if (point.empty) {
				continue;
			}
			double tempDistance{ tail.distanceTo(point.point) };
			if ((tempDistance - distance) < tol.equalPoint()) {
				tail = point.point;
				point.empty = true;
				dist.append(tail);
			}
		}
	}

	dist.append(dist[0]);
}

Acad::ErrorStatus Icosahedron::Polygonos(std::vector<AcArray<AcGePoint3d>>&polygonos) {
	assertWriteEnabled();

	AcGeTol tol;
	double edgeLength{};
	this->EdgeLength(edgeLength);
	std::vector<AcArray<AcGePoint3d>> unsortedPoints;
	AcArray<AcGePlane> planes;
	this->Planes(planes);

	for (int i{ 0 }; i < planes.length(); ++i) {
		AcArray<AcGePoint3d> ps;
		for (int j{ 0 }; j < m_vertices.length(); ++j) {
			if (planes[i].isOn(m_vertices[j])) {
				ps.append(m_vertices[j]);
			}
		}
		if (ps.length() > 0) {
			unsortedPoints.push_back(ps);
		}
	}

	for (auto& points : unsortedPoints) {
		AcArray<AcGePoint3d> sortedPoints;
		CreateChainByDistance(points, sortedPoints, edgeLength);
		polygonos.push_back(sortedPoints);
	}

	return Acad::eOk;
}

















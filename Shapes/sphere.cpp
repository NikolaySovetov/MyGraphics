#include "StdAfx.h"
#include "sphere.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 MySphere::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	MySphere, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, MYSPHERE,
SHAPESAPP
|Product Desc:     A description for your object
|Company:          Your company name
|WEB Address:      Your company WEB site address
)

//-----------------------------------------------------------------------------
MySphere::MySphere()
	: AcDbEntity(), m_center{ 0, 0, 0 }, m_circumradius{ 1.0 }, m_color{ 0 }
{}

MySphere::MySphere(const AcGePoint3d & center, const double& radius, int color)
	: AcDbEntity(), m_center{ center }, m_circumradius{ radius }, m_color{ color }
{}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus MySphere::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (MySphere::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	//.....
	pFiler->writeItem(m_center);
	pFiler->writeItem(m_circumradius);
	pFiler->writeItem(m_color);

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus MySphere::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEntity::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > MySphere::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < MySphere::kCurrentVersionNumber )
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

	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean MySphere::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled () ;

	AcGeVector3d direct;
	mode->subEntityTraits().setColor(m_color);
	mode->geometry().circle(m_center, m_circumradius, direct.kXAxis);
	mode->geometry().circle(m_center, m_circumradius, direct.kYAxis);
	mode->geometry().circle(m_center, m_circumradius, direct.kZAxis);

	double offsetRadius{std::sqrt(3.0 / 4.0 * std::pow(m_circumradius, 2.0))};
	AcGePoint3d offsetCenter{m_center.x, m_center.y, m_center.z + (m_circumradius / 2.0) };
	mode->geometry().circle(offsetCenter, offsetRadius, direct.kZAxis);

	offsetCenter = AcGePoint3d(m_center.x, m_center.y, m_center.z - (m_circumradius / 2.0));
	mode->geometry().circle(offsetCenter, offsetRadius, direct.kZAxis);

	return (AcDbEntity::subWorldDraw (mode)) ;
}

Adesk::UInt32 MySphere::subSetAttributes (AcGiDrawableTraits *traits) {
	assertReadEnabled () ;
	return (AcDbEntity::subSetAttributes (traits)) ;
}



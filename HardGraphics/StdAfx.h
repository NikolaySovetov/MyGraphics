#pragma once
#define HARDGRAPHICS_MODULE

/*#ifndef
#define _ALLOW_RTCc_IN_STL
#endif*/

#pragma pack(push, 8)
#pragma warning(disable : 4786 4996)
//#pragma warning(disable: 4098)

//-----------------------------------------------------------------------------
#include <windows.h>

	//- ObjectARX and OMF headers needs this
#include <map>

				//-----------------------------------------------------------------------------
	//- Include ObjectDBX/ObjectARX headers
	//- Uncomment one of the following lines to bring a given library in your project.
	//#define _BREP_SUPPORT_					//- Support for the BRep API
	//#define _HLR_SUPPORT_						//- Support for the Hidden Line Removal API
	//#define _AMODELER_SUPPORT_				//- Support for the AModeler API
		//#define _ASE_SUPPORT_							//- Support for the ASI/ASE API
	//#define _RENDER_SUPPORT_					//- Support for the AutoCAD Render API
		//#define _ARX_CUSTOM_DRAG_N_DROP_	//- Support for the ObjectARX Drag'n Drop API
	//#define _INC_LEAGACY_HEADERS_			//- Include legacy headers in this project
#include "arxHeaders.h"
#include "tchar.h"

//-----------------------------------------------------------------------------
#include "DocData.h" //- Your document specific data class holder

	//- Declare it as an extern here so that it becomes available in all modules
extern AcApDataManager<CDocData> DocVars;

#pragma pack(pop)


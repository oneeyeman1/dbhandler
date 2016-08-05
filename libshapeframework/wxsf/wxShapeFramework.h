/***************************************************************
 * Name:      wxShapeFramework.h
 * Purpose:   Main header file
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _WXSF_H_
#define _WXSF_H_

// main library classes
#include <wxsf/DiagramManager.h>
#include <wxsf/ShapeCanvas.h>
#include <wxsf/Thumbnail.h>
#include <wxsf/AutoLayout.h>

// shapes' declarations
#include <wxsf/RectShape.h>
#include <wxsf/RoundRectShape.h>
#include <wxsf/FixedRectShape.h>
#include <wxsf/EllipseShape.h>
#include <wxsf/CircleShape.h>
#include <wxsf/DiamondShape.h>
#include <wxsf/TextShape.h>
#include <wxsf/EditTextShape.h>
#include <wxsf/BitmapShape.h>
#include <wxsf/PolygonShape.h>
#include <wxsf/ControlShape.h>
#include <wxsf/GridShape.h>
#include <wxsf/FlexGridShape.h>

// arrows' declarations
#include <wxsf/OpenArrow.h>
#include <wxsf/SolidArrow.h>
#include <wxsf/DiamondArrow.h>
#include <wxsf/CircleArrow.h>

// connection lines' declarations
#include <wxsf/LineShape.h>
#include <wxsf/CurveShape.h>
#include <wxsf/OrthoShape.h>
#include <wxsf/RoundOrthoShape.h>

// library events
#include <wxsf/SFEvents.h>

// printing support
#include <wxsf/Printout.h>

// common functions
#include <wxsf/CommonFcn.h>

// serialize/deserialize functionality
#include <wxxmlserializer/XmlSerializer.h>

#endif //_WXSF_H_

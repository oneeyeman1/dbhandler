// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wxsf/ShapeCanvas.h"
#include "designcanvas.h"

DesignCanvas::DesignCanvas(wxView *view)
{
    startPoint.x = 1;
    startPoint.y = 1;
    m_pManager.SetRootItem( new xsSerializable() );
    SetDiagramManager( &m_pManager );
    Create( view->GetFrame(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxALWAYS_SHOW_SB );
    SetVirtualSize( 1000, 1000 );
    SetScrollRate( 10, 10 );
    m_mode = modeDESIGN;
    SetCanvasColour( *wxWHITE );
}

DesignCanvas::~DesignCanvas ()
{
}

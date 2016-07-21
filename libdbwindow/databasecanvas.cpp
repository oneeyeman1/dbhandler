// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/docview.h"
#include "Defs.h"
#include "wxsf/ShapeCanvas.h"
#include "DiagramManager.h"
#include "databasecanvas.h"

DatabaseCanvas::DatabaseCanvas(wxView *view, wxWindow *parent) : wxScrolledWindow(view->GetFrame())
{
    m_view = view;
    SetVirtualSize( 1000, 1000 );
    SetScrollRate( 20, 20 );

    SetBackgroundColour( *wxWHITE );
}

DatabaseCanvas::~DatabaseCanvas()
{
}

void DatabaseCanvas::OnDraw(wxDC& dc)
{
/*    if( m_view )
        m_view->OnDraw( &dc );*/
}

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STD_IOSTREAM
    #include "wx/ioswrap.h"
#else
    #include "wx/txtstrm.h"
#endif
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/cmdproc.h"
//#include "Defs.h"
//#include "wxsf/ShapeCanvas.h"
//#include "ErdTable.h"
#include "DiagramManager.h"
#include "database.h"
#include "table.h"
#include "databasedoc.h"
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

void DatabaseCanvas::DisplayTables()
{
	int size = ((DrawingDocument *)m_view->GetDocument())->GetTables().size();
    if( size == 1 )
    {
//        Table table = new Table( 
//        ErdTable *panel = new ErdTable( this, );
    }
}

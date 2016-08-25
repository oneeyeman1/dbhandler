// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include <string>
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
#include "XmlSerializer.h"
#include "database.h"
#include "table.h"
#include "GUIColumn.h"
#include "GUIDatabaseTable.h"
#include "wxsf/ShapeCanvas.h"
#include "ErdTable.h"
#include "DiagramManager.h"
#include "databasedoc.h"
#include "databasecanvas.h"

DatabaseCanvas::DatabaseCanvas(wxView *view, wxWindow *parent) : wxSFShapeCanvas()
{
    m_view = view;
    m_showComments = m_showIndexKeys = m_showIntegrity = true;
	m_pManager.SetRootItem( new xsSerializable() );
    SetDiagramManager( &m_pManager );
	Create( view->GetFrame(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL );
    SetVirtualSize( 1000, 1000 );
    SetScrollRate( 20, 20 );
    m_mode = modeDESIGN;
    SetBackgroundColour( *wxWHITE );
//    Bind( wxID_TABLEDROPTABLE, &DatabaseCanvas::OnDropTable, this );
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
    wxPoint startPoint( 10, 10 );
    int size = ((DrawingDocument *)m_view->GetDocument())->GetTables().size();
    std::vector<GUIDatabaseTable> tables = ((DrawingDocument *)m_view->GetDocument())->GetTables();
    for( std::vector<GUIDatabaseTable>::iterator it = tables.begin(); it < tables.end(); it++ ) 
    {
        ErdTable *panel = new ErdTable( &(*it) );
        m_pManager.AddShape( panel, NULL, startPoint, sfINITIALIZE, sfDONT_SAVE_STATE );
		panel->UpdateColumns();
		startPoint.x += 200;
    }
}

void DatabaseCanvas::OnLeftDown(wxMouseEvent &event)
{
    wxSFShapeBase* pShape = NULL;
    switch( m_mode )
    {
        case modeTABLE:
            break;
        case modeLine:
            break;
        case modeVIEW:
            break;
        default:
            wxSFShapeCanvas::OnLeftDown( event );
    }
    if( pShape )
    {
        if( !event.ControlDown() )
            m_mode = modeDESIGN;
    }
}

void DatabaseCanvas::OnRightDown(wxMouseEvent &event)
{
    wxSFShapeCanvas::OnRightDown( event );
    wxPoint pt = event.GetPosition();
    wxMenu mnu;
	mnu.Bind( wxEVT_COMMAND_MENU_SELECTED, &DatabaseCanvas::OnDropTable, this, wxID_TABLEDROPTABLE );
    m_selectedShape = GetShapeUnderCursor();
    if( m_selectedShape )
    {
        mnu.Append( wxID_TABLECLOSE, _( "Close" ), _( "Close Table" ), false );
        mnu.AppendSeparator();
        mnu.Append( wxID_TABLEALTERTABLE, _( "Alter Table" ), _( "Alter Table" ), false );
        mnu.Append( wxID_TABLEPROPERTIES, _( "Properties..." ), _( "Table Properties" ), false );
        mnu.AppendSeparator();
        mnu.Append( wxID_TABLENEW, _( "New" ), _( "New" ), false );
        mnu.AppendSeparator();
        mnu.Append( wxID_TABLEDROPTABLE, _( "Drop Table" ), _( "Drop Table" ), false );
        mnu.AppendSeparator();
        mnu.Append( wxID_TABLEEDITDATA, _( "Edit Data" ), _( "Edit Data" ), false );
        mnu.AppendSeparator();
        mnu.Append( wxID_TABLEDATATRANSFER, _( "Data Transfer" ), _( "Data Transfer" ), false );
        mnu.AppendSeparator();
        mnu.Append( wxID_TABLEPRINTDEFINITION, _( "Print Definition" ), _( "Print Definition" ), false );
    }
    else
    {
        mnu.Append( wxID_VIEWSELECTTABLES, _( "Select Table..." ), _( "Select Table" ), false );
        mnu.Append( wxID_VIEWARRANGETABLES, _( "Arramge Tables..." ), _( "Arrange Tables" ), false );
        mnu.AppendCheckItem( wxID_VIEWSHOWCOMMENTS, _( "Show Comments" ), _( "Show Comments" ) );
        mnu.AppendCheckItem( wxID_VIEWSHOWINDEXKEYS, _( "Show Index Keys" ), _( "Show Index Keys" ) );
        mnu.AppendCheckItem( wxID_VIEWSHOWINTEGRITY, _( "Show Referential Integrity" ), _( "Show Referential Integrity" ) );
        mnu.Check( wxID_VIEWSHOWCOMMENTS, true );
        mnu.Check( wxID_VIEWSHOWINDEXKEYS, true );
        mnu.Check( wxID_VIEWSHOWINTEGRITY, true );
    }
    PopupMenu( &mnu, pt.x, pt.y );
}

void DatabaseCanvas::OnDropTable(wxCommandEvent &event)
{
    wxMessageBox( "Table is dropping!" );
}

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
#include "database.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/FlexGridShape.h"
#include "constraint.h"
#include "GUIColumn.h"
#include "MyErdTable.h"
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

void DatabaseCanvas::DisplayTables(const std::vector<wxString> &selections)
{
    wxPoint startPoint( 10, 10 );
    std::vector<MyErdTable *> tables = ((DrawingDocument *)m_view->GetDocument())->GetTables();
    int size = tables.size();
    for( std::vector<MyErdTable *>::iterator it = tables.begin(); it < tables.end(); it++ ) 
    {
//        ErdTable *panel = new ErdTable( &(*it) );
        m_pManager.AddShape( (*it), NULL, startPoint, sfINITIALIZE, sfDONT_SAVE_STATE );
		(*it)->UpdateTable();
		startPoint.x += 200;
    }
    Refresh();
    for( std::vector<MyErdTable *>::iterator it2 = tables.begin(); it2 < tables.end(); it2++ )
    {
        std::map<int, std::vector<FKField *> > foreignKeys = const_cast<DatabaseTable &>( (*it2)->GetTable() ).GetForeignKeyVector();
        for( std::map<int, std::vector<FKField *> >::iterator it3 = foreignKeys.begin(); it3 != foreignKeys.end(); it3++ )
        {
            for( std::vector<FKField *>::iterator it4 = (*it3).second.begin(); it4 < (*it3).second.end(); it4++ )
            {
                wxString referencedTableName = (*it4)->GetReferencedTableName();
                if( std::find( selections.begin(), selections.end(), referencedTableName ) != selections.end() )
                {
                    Constraint* pConstr = new Constraint();
                    pConstr->SetLocalColumn( (*it4)->GetOriginalFieldName() );
                    pConstr->SetRefCol( (*it4)->GetReferencedFieldName() );
                    pConstr->SetRefTable( referencedTableName );
                    pConstr->SetType( Constraint::foreignKey );
                    switch( (*it4)->GetOnUpdateConstraint() )
                    {
                        case RESTRICT_UPDATE:
                            pConstr->SetOnUpdate( Constraint::restrict );
                            break;
                        case SET_NULL_UPDATE:
                            pConstr->SetOnUpdate( Constraint::setNull );
                            break;
                        case SET_DEFAULT_UPDATE:
                        case CASCADE_UPDATE:
                            pConstr->SetOnUpdate( Constraint::cascade );
                            break;
                        case NO_ACTION_UPDATE:
                            pConstr->SetOnUpdate( Constraint::noAction );
                            break;
                    }
                    switch( (*it4)->GetOnDeleteConstraint() )
                    {
                        case RESTRICT_DELETE:
                            pConstr->SetOnUpdate( Constraint::restrict );
                            break;
                        case SET_NULL_DELETE:
                            pConstr->SetOnUpdate( Constraint::setNull );
                            break;
                        case SET_DEFAULT_UPDATE:
                        case CASCADE_DELETE:
                            pConstr->SetOnUpdate( Constraint::cascade );
                            break;
                        case NO_ACTION_DELETE:
                            pConstr->SetOnUpdate( Constraint::noAction );
                            break;
                    }
                    (*it2)->GetShapeManager()->CreateConnection( (*it2)->GetId(), dynamic_cast<DrawingDocument *>( m_view->GetDocument() )->GetReferencedTable( referencedTableName )->GetId(), new ErdForeignKey( pConstr ), sfDONT_SAVE_STATE );
                }
            }
        }
    }
    Refresh();
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

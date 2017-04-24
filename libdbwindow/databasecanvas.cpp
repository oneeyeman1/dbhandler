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
#include "wx/dynlib.h"
#include "wx/notebook.h"
#include "wx/grid.h"
#include "database.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/FlexGridShape.h"
#include "constraintsign.h"
#include "constraint.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "MyErdTable.h"
#include "wxsf/ShapeCanvas.h"
#include "fieldwindow.h"
#include "FieldShape.h"
#include "DiagramManager.h"
#include "wherehavingpage.h"
#include "syntaxproppage.h"
#include "databasedoc.h"
#include "databasecanvas.h"
#include "databaseview.h"
#include "ErdForeignKey.h"

typedef void (*TABLESELECTION)(wxDocMDIChildFrame *, Database *, std::vector<wxString> &);
/*
BEGIN_EVENT_TABLE(DatabaseCanvas, wxSFShapeCanvas)
    EVT_MENU(wxID_TABLEDROPTABLE, DatabaseCanvas::OnDropTable)
END_EVENT_TABLE()
*/
DatabaseCanvas::DatabaseCanvas(wxView *view, const wxPoint &pt, wxWindow *parent) : wxSFShapeCanvas()
{
    m_view = view;
    m_showDataTypes = m_showLabels = m_showToolBox = m_showComments = m_showIndexKeys = m_showIntegrity = true;
    startPoint.x = 10;
    startPoint.y = 10;
    m_showComments = m_showIndexKeys = m_showIntegrity = true;
    m_pManager.SetRootItem( new xsSerializable() );
    SetDiagramManager( &m_pManager );
    Create( view->GetFrame(), wxID_ANY, pt, wxDefaultSize, wxHSCROLL | wxVSCROLL );
    SetVirtualSize( 1000, 1000 );
    SetScrollRate( 20, 20 );
    m_mode = modeDESIGN;
    SetCanvasColour( *wxWHITE );
//    Bind( wxID_TABLEDROPTABLE, &DatabaseCanvas::OnDropTable, this );
    Bind( wxEVT_MENU, &DatabaseCanvas::OnDropTable, this, wxID_TABLEDROPTABLE );
}

DatabaseCanvas::~DatabaseCanvas()
{
}

void DatabaseCanvas::OnDraw(wxDC &WXUNUSED(dc))
{
}

void DatabaseCanvas::DisplayTables(std::vector<wxString> &selections)
{
    std::vector<MyErdTable *> tables = ((DrawingDocument *)m_view->GetDocument())->GetTables();
    for( std::vector<MyErdTable *>::iterator it = tables.begin(); it < tables.end(); it++ ) 
    {
        if( !IsTableDisplayed( (*it)->GetTableName() ) )
        {
            std::vector<Fields *> fields = (*it)->GetTable()->GetFields();
            m_pManager.AddShape( (*it), NULL, startPoint, sfINITIALIZE, sfDONT_SAVE_STATE );
            if( (*it) == tables.back() )
                (*it)->Select( true );
            (*it)->UpdateTable();
            for( std::vector<Fields *>::iterator it1 = fields.begin(); it1 < fields.end(); it1++ )
            {
                m_page2->AppendField( (*it)->GetTableName() + "." + (*it1)->GetFieldName() );
                m_page4->AppendField( (*it)->GetTableName() + "." + (*it1)->GetFieldName() );
            }
            wxRect rect = (*it)->GetBoundingBox();
            startPoint.x += 200 + rect.GetWidth();
            m_displayedTables.push_back( (*it) );
        }
    }
    for( std::vector<MyErdTable *>::iterator it1 = m_displayedTables.begin(); it1 < m_displayedTables.end(); it1++ )
    {
        wxString name = const_cast<DatabaseTable &>( (*it1)->GetTable() ).GetTableName();
        if( std::find( selections.begin(), selections.end(), name ) == selections.end() )
            selections.push_back( name );
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
                    Constraint* pConstr = new Constraint( ((DrawingView *) m_view)->GetViewType() );
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
                    (*it2)->GetShapeManager()->CreateConnection( (*it2)->GetId(), dynamic_cast<DrawingDocument *>( m_view->GetDocument() )->GetReferencedTable( referencedTableName )->GetId(), new ErdForeignKey( pConstr, ((DrawingView *) m_view)->GetViewType() ), sfDONT_SAVE_STATE );
                }
            }
        }
    }
    Refresh();
}

bool DatabaseCanvas::IsTableDisplayed(const std::wstring &name)
{
    ShapeList listShapes;
    m_pManager.GetShapes( CLASSINFO( MyErdTable ), listShapes );
    bool found = false;
    for( ShapeList::iterator it = listShapes.begin(); it != listShapes.end() && !found; ++it )
    {
        if( dynamic_cast<MyErdTable *>( (*it) )->GetTableName() == name )
            found = true;
    }
    return found;
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
    FieldShape *field = NULL;
    MyErdTable *erdTable = NULL;
    wxPoint pt = event.GetPosition();
    wxMenu mnu;
    mnu.Bind( wxEVT_COMMAND_MENU_SELECTED, &DatabaseCanvas::OnDropTable, this, wxID_TABLEDROPTABLE );
    m_selectedShape = GetShapeUnderCursor();
    ViewType type = dynamic_cast<DrawingView *>( m_view )->GetViewType();
    if( m_selectedShape )
    {
        ShapeList list;
        GetShapesAtPosition( pt, list );
        DeselectAll();
        wxRect tableRect;
        bool fieldSelected = false;
        for( ShapeList::iterator it = list.begin(); it != list.end(); it++ )
        {
            MyErdTable *table = wxDynamicCast( (*it), MyErdTable );
            if( table )
            {
                table->Select( true );
                tableRect = table->GetBoundingBox();
                erdTable = table;
            }
            else
            {
                field = wxDynamicCast( (*it), FieldShape );
                if( field )
                {
                    wxRect rect = field->GetBoundingBox();
                    field->Select( true );
                    field->SetParentRect( tableRect );
                    fieldSelected = true;
                }
            }
        }
        Refresh();
        if( type == DatabaseView )
        {
            if( !fieldSelected )
            {
                mnu.Append( wxID_TABLECLOSE, _( "Close" ), _( "Close Table" ), false );
                mnu.AppendSeparator();
                mnu.Append( wxID_TABLEALTERTABLE, _( "Alter Table" ), _( "Alter Table" ), false );
                mnu.Append( wxID_PROPERTIES, _( "Properties..." ), _( "Table Properties" ), false );
                mnu.AppendSeparator();
                wxMenu *newObjectMenu = new wxMenu();
                newObjectMenu->Append( wxID_OBJECTNEWINDEX, _( "Index..." ), _( "New Index" ) );
                newObjectMenu->Append( wxID_OBJECTNEWFF, _( "Foreign Key..." ), _( "New Foreign Key" ) );
                mnu.AppendSubMenu( newObjectMenu, _( "New" ), _( "New" ) );
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
                mnu.Append( wxID_FIELDDEFINITION, _( "Definition" ), _( "Edit definition of selected object" ), false );
                mnu.Append( wxID_FIELDPROPERTIES, _( "Properties..." ), _( "Show properties of selected object" ), false );
            }
        }
        else
        {
            if( !fieldSelected )
            {
				mnu.Append( wxID_SELECTALLFIELDS, _( "Select All" ), _( "Select all columns for display" ) );
				mnu.Append( wxID_DESELECTALLFIELDS, _("Deselect All" ), _( "Deselect all columns for display" ) );
				mnu.Append( wxID_TABLECLOSE, _( "Close" ), _( "Close Table" ), false );
            }
        }
    }
    else
    {
        if( type == DatabaseView )
        {
            mnu.Append( wxID_SELECTTABLE, _( "Select Table..." ), _( "Select Table" ), false );
            mnu.Append( wxID_VIEWARRANGETABLES, _( "Arramge Tables..." ), _( "Arrange Tables" ), false );
            mnu.AppendCheckItem( wxID_VIEWSHOWCOMMENTS, _( "Show Comments" ), _( "Show Comments" ) );
            mnu.AppendCheckItem( wxID_VIEWSHOWINDEXKEYS, _( "Show Index Keys" ), _( "Show Index Keys" ) );
            mnu.AppendCheckItem( wxID_VIEWSHOWINTEGRITY, _( "Show Referential Integrity" ), _( "Show Referential Integrity" ) );
            mnu.Check( wxID_VIEWSHOWCOMMENTS, true );
            mnu.Check( wxID_VIEWSHOWINDEXKEYS, true );
            mnu.Check( wxID_VIEWSHOWINTEGRITY, true );
        }
        else
        {
            mnu.Append( wxID_SELECTTABLE, _( "Select Table..." ), _( "Select addtional tables for the query" )  );
            mnu.Append( wxID_ARRANGETABLES, _( "Arrange Tables" ), _( "Arrange Tables" ) );
            wxMenu *showMenu = new wxMenu();
            showMenu->AppendCheckItem( wxID_SHOWDATATYPES, _( "Datatypes" ), _( "Datatypes" ) );
            showMenu->AppendCheckItem( wxID_SHOWLABELS, _( "Labels" ), _( "Labels" ) );
            showMenu->AppendCheckItem( wxID_SHOWCOMMENTS, _( "Comments" ), _( "Comments" ) );
            showMenu->AppendCheckItem( wxID_SHOWSQLTOOLBOX, _( "SQL Toolbox" ), _( "SQL Toolbox" ) );
            mnu.AppendSubMenu( showMenu, _( "Show" ) );
        }
    }
    int rc = GetPopupMenuSelectionFromUser( mnu, pt );
    if( rc == wxID_NONE && field )
    {
//        if( field )
        {
            field->Select( false );
            erdTable->GetFieldGrid()->Refresh();
            erdTable->Refresh();
        }
    }
    else
    {
        if( field )
        {
            field->Select( false );
            erdTable->GetFieldGrid()->Refresh();
            erdTable->Refresh();
        }
        wxCommandEvent evt( wxEVT_MENU, rc );
        evt.SetEventObject( field );
        m_view->ProcessEvent( evt );
    }
}

void DatabaseCanvas::OnDropTable(wxCommandEvent &WXUNUSED(event))
{
    std::vector<std::wstring> errors;
    MyErdTable *erdTable = dynamic_cast<MyErdTable *>( m_selectedShape );
    DatabaseTable *table = &( const_cast<DatabaseTable &>( erdTable->GetTable() ) );
    wxString name = const_cast<DatabaseTable &>( erdTable->GetTable() ).GetTableName();
    DrawingDocument *doc = (DrawingDocument *) m_view->GetDocument();
    Database *db = doc->GetDatabase();
    int answer = wxMessageBox( _( "You are about to delete table " ) + name + _( ". Are you sure?" ), _( "Database" ), wxYES_NO | wxNO_DEFAULT );
    if( answer == wxYES )
    {
        if( !db->DeleteTable( name.ToStdWstring(), errors ) )
        {
            m_pManager.RemoveShape( erdTable );
            std::map<std::wstring, std::vector<DatabaseTable *> > tables = db->GetTableVector().m_tables;
            std::vector<DatabaseTable *> tableVec = tables.at( db->GetTableVector().m_dbName );
            tableVec.erase( std::remove( tableVec.begin(), tableVec.end(), table ), tableVec.end() );
            std::vector<std::wstring> names = doc->GetTableNameVector();
            names.erase( std::remove( names.begin(), names.end(), table->GetTableName() ), names.end() );
        }
        else
        {
            for( std::vector<std::wstring>::iterator it = errors.begin(); it < errors.end(); it++ )
            {
                wxMessageBox( (*it) );
            }
        }
    }
}

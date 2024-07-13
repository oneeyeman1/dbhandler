// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include <memory>
#include <string>
#include <vector>
#if wxUSE_STD_IOSTREAM
    #include "wx/ioswrap.h"
#else
    #include "wx/txtstrm.h"
#endif
#include "wx/docview.h"
#include "wx/fontenum.h"
#include "wx/docmdi.h"
#include "wx/cmdproc.h"
#include "wx/dynlib.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "wx/notebook.h"
#include "wx/fdrepdlg.h"
#include "wx/grid.h"
#include "wx/stc/stc.h"
#include "wx/bmpcbox.h"
#include "wx/listctrl.h"
#include "wx/dataview.h"
#include "wx/renderer.h"
#include "database.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/FlexGridShape.h"
#include "wxsf/ShapeCanvas.h"
#include "configuration.h"
#include "commentfieldshape.h"
#include "fieldtypeshape.h"
#include "objectproperties.h"
#include "constraint.h"
#include "constraintsign.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "nametableshape.h"
#include "commenttableshape.h"
#include "fontcombobox.h"
#include "MyErdTable.h"
#include "field.h"
#include "fieldwindow.h"
#include "FieldShape.h"
#include "DiagramManager.h"
#include "sortgroupbypage.h"
#include "wherehavingpage.h"
#include "syntaxproppage.h"
#include "databasedoc.h"
#include "databasecanvas.h"
#include "designcanvas.h"
#include "databaseview.h"
#include "ErdForeignKey.h"

typedef void (*TABLESELECTION)(wxDocMDIChildFrame *, Database *, std::vector<wxString> &);
typedef int (*CREATEFOREIGNKEY)(wxWindow *parent, wxString &, DatabaseTable *, std::vector<std::wstring> &, std::vector<std::wstring> &, std::wstring &, int &, int &, Database *, bool &, bool, std::vector<FKField *> &, int &);
typedef int (*SELECTJOINTYPE)(wxWindow *parent, const wxString &origTable, const wxString &refTable, const wxString &origField, const wxString &refField, int &type);
/*
BEGIN_EVENT_TABLE(DatabaseCanvas, wxSFShapeCanvas)
    EVT_MENU(wxID_TABLEDROPTABLE, DatabaseCanvas::OnDropTable)
END_EVENT_TABLE()
*/

XS_IMPLEMENT_CLONABLE_CLASS(QueryRoot, xsSerializable)

QueryRoot::QueryRoot()
{
    m_dbName = "";
    m_dbType = "";
    m_query = "";

    XS_SERIALIZE( m_dbName, "database_name" );
    XS_SERIALIZE( m_dbType, "database_type" );
    XS_SERIALIZE( m_query, "query" );
    XS_SERIALIZE( m_tables, "query_tables" );
    XS_SERIALIZE( m_fields, "query_fields" );
    XS_SERIALIZE( m_sortDest, "sort_dest" );
    XS_SERIALIZE( m_wheres, "query_where" );
    XS_SERIALIZE( m_arguments, "query_arguments" );
    XS_SERIALIZE( m_groupbySource, "groupby_source" );
    XS_SERIALIZE( m_groupbyDest, "groupby_dest" );
}

QueryRoot::QueryRoot(const QueryRoot &root)
{
    m_dbName = root.m_dbName;
    m_dbType = root.m_dbType;
    m_query = root.m_query;
    m_tables = root.m_tables;
    m_fields = root.m_fields;
    m_arguments = root.m_arguments;
    m_sortDest = root.m_sortDest;
    m_wheres = root.m_wheres;
    m_groupbySource = root.m_groupbySource;
    m_groupbyDest = root.m_groupbyDest;
    
    XS_SERIALIZE( m_dbName, "database_name" );
    XS_SERIALIZE( m_dbType, "database_type" );
    XS_SERIALIZE( m_query, "query" );
    XS_SERIALIZE( m_tables, "query_tables" );
    XS_SERIALIZE( m_fields, "query_fields" );
    XS_SERIALIZE( m_sortDest, "sort_dest" );
    XS_SERIALIZE( m_wheres, "query_where" );
    XS_SERIALIZE( m_arguments, "query_arguments" );
    XS_SERIALIZE( m_groupbySource, "groupby_source" );
    XS_SERIALIZE( m_groupbyDest, "groupby_dest" );
}

DatabaseCanvas::DatabaseCanvas(wxView *view, const wxPoint &pt, const wxString &dbName, const wxString &dbType, wxWindow *parent) : wxSFShapeCanvas()
{
    m_view = view;
    m_dbName = dbName;
    m_dbType = dbType;
    m_showDataTypes = m_showLabels = m_showToolBox = m_showComments = m_showIndexKeys = m_showIntegrity = true;
    m_oldSelectedSign = NULL;
    startPoint.x = 10;
    startPoint.y = 10;
    auto stdPath = wxStandardPaths::Get();
#ifdef __WXOSX__
    wxFileName fn( stdPath.GetExecutablePath() );
    fn.RemoveLastDir();
    m_libPath = fn.GetPathWithSep() + "Frameworks/";
#elif __WXGTK__
    m_libPath = stdPath.GetInstallPrefix() + "/";
#elif __WXMSW__
    wxFileName fn( stdPath.GetExecutablePath() );
    m_libPath = fn.GetPathWithSep();
#endif
    auto root = new QueryRoot();
    root->SetDbName( dbName );
    root->SetDbType( dbType );
    m_pManager.SetRootItem( root );
    SetDiagramManager( &m_pManager );
    Create( view->GetFrame(), wxID_ANY, pt, wxDefaultSize, wxHSCROLL | wxVSCROLL | wxALWAYS_SHOW_SB );
    SetVirtualSize( 1000, 1000 );
    SetScrollRate( 20, 20 );
    m_mode = modeDESIGN;
    SetCanvasColour( *wxWHITE );
//    Bind( wxID_TABLEDROPTABLE, &DatabaseCanvas::OnDropTable, this );
    Bind( wxEVT_MENU, &DatabaseCanvas::OnDropTable, this, wxID_DROPOBJECT );
    Bind( wxEVT_MENU, &DatabaseCanvas::OnCloseTable, this, wxID_TABLECLOSE );
    Bind( wxEVT_UPDATE_UI, &DatabaseCanvas::nUpdateTableParam, this, wxID_SHOWDATATYPES );
    Bind(  wxEVT_UPDATE_UI, &DatabaseCanvas::nUpdateTableParam, this, wxID_SHOWCOMMENTS );
}

DatabaseCanvas::~DatabaseCanvas()
{
}

void DatabaseCanvas::OnDraw(wxDC &WXUNUSED(dc))
{
}

void DatabaseCanvas::DisplayTables(std::map<wxString,std::vector<TableDefinition> > &selections, const std::vector<TableField *> &queryFields, wxString &query, std::vector<wxString> &relations)
{
    std::vector<MyErdTable *> tables = ((DrawingDocument *)m_view->GetDocument())->GetTables();
    for( std::vector<MyErdTable *>::iterator it = tables.begin(); it < tables.end(); it++ ) 
    {
        if( !IsTableDisplayed( (*it)->GetTableName().ToStdWstring() ) )
        {
            std::vector<TableField *> fields = const_cast<DatabaseTable *>( (*it)->GetTable() )->GetFields();
            m_pManager.AddShape( (*it), NULL, startPoint, sfINITIALIZE, sfDONT_SAVE_STATE );
            if( (*it) == tables.back() && dynamic_cast<DrawingView *>( m_view )->GetViewType() == DatabaseView )
                (*it)->Select( true );
            (*it)->UpdateTable();
            if( dynamic_cast<DrawingView *>( m_view )->GetViewType() == QueryView || dynamic_cast<DrawingView *>( m_view )->GetViewType() == NewViewView )
            {
                for( std::vector<TableField *>::iterator it1 = fields.begin(); it1 < fields.end(); it1++ )
                {
                    dynamic_cast<DrawingView *>( m_view )->GetWherePage()->AppendField( (*it)->GetTableName().ToStdWstring() + L"." + (*it1)->GetFieldName() );
                    dynamic_cast<DrawingView *>( m_view )->GetHavingPage()->AppendField( (*it)->GetTableName().ToStdWstring() + L"." + (*it1)->GetFieldName() );
                }
            }
            wxRect rect = (*it)->GetBoundingBox();
            startPoint.x += 200 + rect.GetWidth();
            m_displayedTables.push_back( (*it) );
        }
    }
    ShapeList listShapes;
    m_pManager.GetShapes( CLASSINFO( FieldShape ), listShapes );
    for( std::vector<MyErdTable *>::iterator it1 = m_displayedTables.begin(); it1 < m_displayedTables.end(); it1++ )
    {
        wxString name = const_cast<DatabaseTable *>( (*it1)->GetTable() )->GetTableName();
        if( dynamic_cast<DrawingView *>( m_view )->GetViewType() == QueryView ||  dynamic_cast<DrawingView *>( m_view )->GetViewType() == NewViewView )
        {
            query += "\"" + (*it1)->GetTable()->GetSchemaName() + L"." + (*it1)->GetTableName() + "\"";
            if( it1 != m_displayedTables.end() - 1 )
                query += ",\r     ";
        }
    }
    bool found = false;
    for( std::vector<TableField *>::const_iterator it = queryFields.begin(); it < queryFields.end() && !found; ++it )
    {
        for( ShapeList::iterator it1 = listShapes.begin(); it1 != listShapes.end() && !found; ++it1 )
        {
            if( dynamic_cast<FieldShape *>( (*it1) )->GetField()->GetFieldName() == (*it)->GetFieldName() )
            {
                (*it1)->Select( true );
                found = true;
            }
        }
        found = false;
    }
    Constraint *pConstr = NULL;
    found = false;
    bool secondIteration = false;
    for( std::vector<MyErdTable *>::iterator it2 = tables.begin(); it2 < tables.end(); it2++ )
    {
        std::map<unsigned long, std::vector<FKField *> > foreignKeys = const_cast<DatabaseTable *>( (*it2)->GetTable() )->GetForeignKeyVector();
        for( std::map<unsigned long, std::vector<FKField *> >::iterator it3 = foreignKeys.begin(); it3 != foreignKeys.end(); it3++ )
        {
//            if( ((DrawingView *) m_view)->GetViewType() == DatabaseView )
//                pConstr = new DatabaseConstraint( (*it3).second );
            for( std::vector<FKField *>::iterator it4 = (*it3).second.begin(); it4 < (*it3).second.end(); it4++ )
            {
                wxString referencedTableName = (*it4)->GetReferencedTableName();
                wxString referentialSchemaName = (*it4)->GetReferentialSchemaName();
                for( auto sel : selections )
                {
                    for( auto table : sel.second )
                    {
                        if( table.schemaName == referentialSchemaName && table.tableName == referencedTableName )
                        {
                            if( found )
                                secondIteration = true;
                            if( !found )
                            {
                                query += "\nWHERE ";
                                found = true;
                            }
                            if( ((DrawingView *) m_view)->GetViewType() == QueryView || ((DrawingView *) m_view)->GetViewType() == NewViewView )
                            {
                                pConstr = new QueryConstraint( ((DrawingView *) m_view)->GetViewType() );
                                pConstr->SetLocalColumn( (*it4)->GetOriginalFieldName() );
                                pConstr->SetRefColumn( (*it4)->GetReferencedFieldName() );
                                pConstr->SetRefTable( referencedTableName );
                                pConstr->SetType( QueryConstraint::foreignKey );
                                pConstr->SetFKDatabaseTable( (*it2)->GetTable() );
                                dynamic_cast<QueryConstraint *>( pConstr )->SetSign( 0 );
                            }
                            if( ((DrawingView *) m_view)->GetViewType() == DatabaseView )
                            {
                                pConstr = new DatabaseConstraint( (*it4)->GetFKName() );
                                pConstr->SetLocalColumns( (*it4)->GetOriginalFields() );
                                pConstr->SetRefColumns( (*it4)->GetReferencedFields() );
                                pConstr->SetLocalColumn( (*it4)->GetOriginalFieldName() );
                                pConstr->SetRefColumn( (*it4)->GetReferencedFieldName() );
                                pConstr->SetRefTable( referencedTableName );
                                pConstr->SetType( QueryConstraint::foreignKey );
                                pConstr->SetFKDatabaseTable( (*it2)->GetTable() );
                                pConstr->SetPGMatch( (*it4)->GetMatchOPtion() );
                            }
                            if( dynamic_cast<DrawingView *>( m_view )->GetViewType() == QueryView || ((DrawingView *) m_view)->GetViewType() == NewViewView )
                            {
                                if( secondIteration )
                                    query += " AND\r      ";
                                query += wxString::Format( "\"%s\".\"%s\" = \"%s\".\"%s\"", referencedTableName, (*it4)->GetReferencedFieldName(), (*it2)->GetTableName(), (*it4)->GetOriginalFieldName() );
                                relations.push_back( wxString::Format( "\"%s\".\"%s\" = \"%s\".\"%s\"", referencedTableName, (*it4)->GetReferencedFieldName(), (*it2)->GetTableName(), (*it4)->GetOriginalFieldName() ) );
                            }
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
                                case SET_DEFAULT_DELETE:
                                case CASCADE_DELETE:
                                    pConstr->SetOnUpdate( Constraint::cascade );
                                    break;
                                case NO_ACTION_DELETE:
                                    pConstr->SetOnUpdate( Constraint::noAction );
                                    break;
                            }
                            (*it2)->GetShapeManager()->CreateConnection( (*it2)->GetId(), dynamic_cast<DrawingDocument *>( m_view->GetDocument() )->GetReferencedTable( referencedTableName )->GetId(), new ErdForeignKey( pConstr, ((DrawingView *) m_view)->GetViewType(), m_pManager ), sfDONT_SAVE_STATE );
                        }
                    }
                }
            }
        }
    }
    query += ";";
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
    ViewType type = dynamic_cast<DrawingView *>( m_view )->GetViewType();
    wxSFShapeBase* pShape = NULL;
    ShapeList shapes, list;
    ConstraintSign *sign = NULL;
    GetSelectedShapes( shapes );
    GetShapesAtPosition( event.GetPosition(), list );
    int count = 0;
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end(); it++ )
    {
        MyErdTable *table = wxDynamicCast( (*it), MyErdTable );
        if( table )
            count++;
        sign = wxDynamicCast( (*it), ConstraintSign );
        if( sign )
            count++;
    }
    bool found = false;
    for( ShapeList::iterator it1 = list.begin(); it1 != list.end() && !found; it1++ )
    {
        sign = wxDynamicCast( (*it1), ConstraintSign );
        if( sign )
        {
            if( sign != m_oldSelectedSign )
            {
                if( m_oldSelectedSign )
                    m_oldSelectedSign->Select( false );
                m_oldSelectedSign = sign;
            }
            found = true;
            sign->Select( true );
            Refresh();
        }
    }
    switch( m_mode )
    {
        case modeTABLE:
            break;
        case modeLine:
            break;
        case modeVIEW:
            break;
        default:
            if( !found )
                wxSFShapeCanvas::OnLeftDown( event );
    }
    if( pShape )
    {
        if( !event.ControlDown() )
            m_mode = modeDESIGN;
    }
    if( type == DatabaseView && list.IsEmpty() )
    {
        for( ShapeList::iterator it = shapes.begin(); it != shapes.end(); it++ )
        {
            MyErdTable *table = wxDynamicCast( (*it), MyErdTable );
            if( table )
                table->Select( true );
        }
        Refresh();
    }
    wxSFTextShape *shape = nullptr;
    if( type == QueryView || type == NewViewView )
    {
        if( sign )
        {
        }
        else
        {
            for( ShapeList::iterator it1 = shapes.begin(); it1 != shapes.end(); it1++ )
            {
                FieldShape *field = wxDynamicCast( (*it1), FieldShape );
                if( field )
                {
                    field->Select( true );
                    shape = field->GetCommentShape();
                    if( shape )
                        shape->Select( true );
                    shape = field->GetTypeShape();
                    if( shape )
                        shape->Select( true );
                }
            }
            FieldShape *fld = NULL;
            MyErdTable *tbl = NULL;
            for( ShapeList::iterator it = list.begin(); it != list.end(); it++ )
            {
                MyErdTable *table = wxDynamicCast( (*it), MyErdTable );
                if( table )
                {
                    tbl = table;
                }
                else
                {
                    FieldShape *field = wxDynamicCast( (*it), FieldShape );
                    if( field )
                    {
                        fld = field;
                        field->Select( !field->IsSelected() );
                        shape = field->GetCommentShape();
                        if( shape )
                            shape->Select( !field->IsSelected() );
                        shape = field->GetTypeShape();
                        if( shape )
                            shape->Select( !field->IsSelected() );
                    }
                    else
                    {
                        FieldTypeShape *typeShape = wxDynamicCast( (*it), FieldTypeShape );
                        if( typeShape )
                        {
                            shape = typeShape->GetFieldShape();
                            typeShape->Select( !shape->IsSelected() );
                            shape->Select( !shape->IsSelected() );
                            shape = typeShape->GetCommentShape();
                            if( shape )
                                shape->Select( typeShape->IsSelected() );
                        }
                        else
                        {
                            CommentFieldShape *comment = wxDynamicCast( (*it), CommentFieldShape );
                            if( comment )
                            {
                                shape = comment->GetFieldShape();
                                comment->Select( !shape->IsSelected() );
                                shape->Select( !shape->IsSelected() );
                                shape = comment->GetTypeShape();
                                if( shape )
                                    shape->Select( !comment->IsSelected() );
                            }
                        }
                    }
                }
            }
            Refresh();
            if( fld )
            {
                dynamic_cast<DrawingView *>( m_view )->AddFieldToQuery( *fld, fld->IsSelected() ? ADD : REMOVE, const_cast<DatabaseTable *>( tbl->GetTable() )->GetFullName(), false );
                if( fld->IsSelected() )
                    dynamic_cast<QueryRoot *>( m_pManager.GetRootItem() )->AddQueryField( fld->GetField()->GetFullName() );
                else
                    dynamic_cast<QueryRoot *>( m_pManager.GetRootItem() )->DeleteQuieryField( fld->GetField()->GetFullName() );
            }
        }
    }
}

void DatabaseCanvas::OnRightDown(wxMouseEvent &event)
{
    FieldShape *erdField = NULL;
    MyErdTable *erdTable = NULL;
    ConstraintSign *erdSign = NULL;
    wxPoint pt = event.GetPosition();
    ShapeList selection;
    GetSelectedShapes( selection );
    wxString query;
    auto page = dynamic_cast<DrawingView *>( m_view )->GetSyntaxPage();
    if( page )
        query = page->GetSyntaxCtrl()->GetValue();
    else
        query = "";
    for( ShapeList::iterator it = selection.begin(); it != selection.end(); ++it )
    {
        MyErdTable *table = wxDynamicCast( (*it), MyErdTable );
        if( table )
            m_realSelectedShape = table;
    }
    wxMenu mnu;
    int allSelected = 0;
    mnu.Bind( wxEVT_COMMAND_MENU_SELECTED, &DatabaseCanvas::OnDropTable, this, wxID_DROPOBJECT );
    mnu.Bind( wxEVT_COMMAND_MENU_SELECTED, &DatabaseCanvas::OnCloseTable, this, wxID_TABLECLOSE );
    m_selectedShape = GetShapeUnderCursor();
    ViewType type = dynamic_cast<DrawingView *>( m_view )->GetViewType();
    if( m_selectedShape )
    {
        ShapeList list;
        GetShapesAtPosition( pt, list );
        if( type == DatabaseView )
            if( m_selectedShape->IsKindOf( CLASSINFO( MyErdTable ) ) )
                DeselectAll();
        wxRect tableRect;
        bool fieldSelected = false;
        bool signSelected = false;
        auto found = false;
        for( ShapeList::iterator it = list.begin(); it != list.end() && !found; ++it )
        {
            MyErdTable *table = wxDynamicCast( (*it), MyErdTable );
            if( table )
            {
                if( type == DatabaseView )
                    table->Select( true );
                tableRect = table->GetBoundingBox();
                erdTable = table;
                if( list.GetCount() == 1 )
                    found = true;
            }
            else
            {
                FieldShape *field = wxDynamicCast( (*it), FieldShape );
                if( field )
                {
                    if( type == DatabaseView )
                        field->Select( true );
                    field->SetParentRect( tableRect );
                    fieldSelected = true;
                    erdField = field;
                    found = true;
                }
                else
                {
                    ConstraintSign *sign = wxDynamicCast( (*it), ConstraintSign );
                    if( sign )
                    {
                        if( type == DatabaseView )
                            sign->Select( true );
                        signSelected = true;
                        erdSign = sign;
                        found = true;
                    }
                }
            }
        }
        if( type == DatabaseView && list.empty() )
        {
            MyErdTable *table = wxDynamicCast( m_selectedShape, MyErdTable );
            if( table )
            {
                table->Select( true );
            }
            else
            {
                table = wxDynamicCast( m_selectedShape->GetParentShape()->GetParentShape(), MyErdTable );
                table->Select( true );
            }
        }
        if( type == QueryView )
        {
            size_t selectedCount = 0;
            SerializableList tableFields;
			if( erdTable )
			{
                erdTable->GetChildrenRecursively( CLASSINFO( FieldShape ), tableFields );
                SerializableList::compatibility_iterator node = tableFields.GetFirst();
                while( node )
                {
                    FieldShape *shape = dynamic_cast<FieldShape *>( node->GetData() );
                    if( shape->IsSelected() )
                        selectedCount++;
                    node = node->GetNext();
                }
                if( selectedCount == const_cast<DatabaseTable *>( erdTable->GetTable() )->GetFields().size() )
                    allSelected = 1;
                else
                    allSelected = -1;
			}
        }
        Refresh();
        if( type == DatabaseView )
        {
            if( !fieldSelected && !signSelected )
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
                mnu.Append( wxID_DROPOBJECT, _( "Drop Table" ), _( "Drop Table" ), false );
                mnu.AppendSeparator();
                mnu.Append( wxID_TABLEEDITDATA, _( "Edit Data" ), _( "Edit Data" ), false );
                mnu.AppendSeparator();
                mnu.Append( wxID_TABLEDATATRANSFER, _( "Data Transfer" ), _( "Data Transfer" ), false );
                mnu.AppendSeparator();
                mnu.Append( wxID_TABLEPRINTDEFINITION, _( "Print Definition" ), _( "Print Definition" ), false );
            }
            else if( fieldSelected )
            {
                mnu.Append( wxID_FIELDDEFINITION, _( "Definition" ), _( "Edit definition of selected object" ), false );
                mnu.Append( wxID_PROPERTIES, _( "Properties..." ), _( "Show properties of selected object" ), false );
            }
            else
            {
                mnu.Append( wxID_FKDEFINITION, _( "Definition..." ), _( "Edit definition of selected object" ) );
                mnu.Append( wxID_FKOPENREFTABLE, _( "Open Referenced Table" ), _( "Open Referenced Table" ) );
                mnu.Append( wxID_DROPOBJECT, _( "Drop Foreign Key..." ), _( "Drop Foreign Key for the table" ) );
            }
        }
        else
        {
            if( erdTable )
            {
                mnu.Append( wxID_SELECTALLFIELDS, _( "Select All" ), _( "Select all columns for display" ) );
                mnu.Append( wxID_DESELECTALLFIELDS, _("Deselect All" ), _( "Deselect all columns for display" ) );
                mnu.Append( wxID_TABLECLOSE, _( "Close" ), _( "Close Table" ), false );
                if( allSelected == -1 )
                    mnu.FindItem( wxID_DESELECTALLFIELDS )->Enable( false );
                else if( allSelected == 1 )
                    mnu.FindItem( wxID_SELECTALLFIELDS )->Enable( false );
            }
        }
    }
    else
    {
        if( type == DatabaseView )
        {
            mnu.Append( wxID_SELECTTABLE, _( "Select Table..." ), _( "Select Table" ), false );
            mnu.Append( wxID_VIEWARRANGETABLES, _( "Arramge Tables..." ), _( "Arrange Tables" ), false );
            mnu.AppendCheckItem( wxID_SHOWCOMMENTS, _( "Show Comments" ), _( "Show Comments" ) );
            mnu.AppendCheckItem( wxID_VIEWSHOWINDEXKEYS, _( "Show Index Keys" ), _( "Show Index Keys" ) );
            mnu.AppendCheckItem( wxID_VIEWSHOWINTEGRITY, _( "Show Referential Integrity" ), _( "Show Referential Integrity" ) );
            if( m_showComments )
                mnu.Check( wxID_SHOWCOMMENTS, true );
            else
                mnu.Check( wxID_SHOWCOMMENTS, false );
            if( m_showIndexKeys )
                mnu.Check( wxID_VIEWSHOWINDEXKEYS, true );
            else
                mnu.Check( wxID_VIEWSHOWINDEXKEYS, false );
            if( m_showIntegrity )
                mnu.Check( wxID_VIEWSHOWINTEGRITY, true );
            else
                mnu.Check( wxID_VIEWSHOWINTEGRITY, false );
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
            if( m_showDataTypes )
                showMenu->Check( wxID_SHOWDATATYPES, true );
            else
                showMenu->Check( wxID_SHOWDATATYPES, false );
            if( m_showLabels )
                showMenu->Check( wxID_SHOWLABELS, true );
            else
                showMenu->Check( wxID_SHOWLABELS, false );
            if( m_showComments )
                showMenu->Check( wxID_SHOWCOMMENTS, true );
            else
                showMenu->Check( wxID_SHOWCOMMENTS, false );
            if( m_showToolBox )
                showMenu->Check( wxID_SHOWSQLTOOLBOX, true );
            else
                showMenu->Check( wxID_SHOWSQLTOOLBOX, false );
        }
    }
    int rc = GetPopupMenuSelectionFromUser( mnu, pt );
    if( type == DatabaseView && erdField )
    {
        erdField->Select( false );
        Refresh();
    }
    switch( rc )
    {
        case wxID_NONE:
            if( erdField )
            {
                erdField->Select( false );
                erdTable->GetFieldGrid()->Refresh();
                erdTable->Refresh();
            }
            break;
        case wxID_SHOWCOMMENTS:
            ShowHideTablePart( 4, !m_showComments );
            break;
        case wxID_SHOWDATATYPES:
            ShowHideTablePart( 1, !m_showDataTypes );
            break;
        case wxID_SHOWSQLTOOLBOX:
            dynamic_cast<DrawingView *>( m_view )->HideShowSQLBox( !m_showToolBox );
            break;
        case wxID_SELECTTABLE:
            dynamic_cast<DrawingView *>( m_view )->SelectTable( false, dynamic_cast<DrawingView *>( m_view )->GetTablesMap(), query, false );
            break;
        case wxID_SELECTALLFIELDS:
            {
                ShapeList shapes;
                m_pManager.GetShapes( CLASSINFO( FieldShape ), shapes );
                for( ShapeList::iterator it = shapes.begin (); it != shapes.end (); ++it )
                {
                    FieldShape *fld = wxDynamicCast( (*it), FieldShape );
                    wxString tableName;
                    if( fld )
                    {
                        tableName = fld->GetField()->GetFullName();
                        tableName = tableName.substr( tableName.find( '.' ) + 1 );
                        tableName = tableName.substr( 0, tableName.find( '.' ) );
                        if( tableName == erdTable->GetTable()->GetTableName() )
                        {
                            fld->Select( true );
                            dynamic_cast<DrawingView *>( m_view )->AddFieldToQuery( *fld, ADD, const_cast<DatabaseTable *>( erdTable->GetTable() )->GetTableName(), false );
                            dynamic_cast<QueryRoot *>( m_pManager.GetRootItem() )->AddQueryField( fld->GetField()->GetFullName() );
                        }
                    }
                    Refresh();
                }
            }
            break;
        case wxID_DESELECTALLFIELDS:
            {
                ShapeList shapes;
                m_pManager.GetShapes( CLASSINFO( FieldShape ), shapes );
                for( ShapeList::iterator it = shapes.begin (); it != shapes.end (); ++it )
                {
                    FieldShape *fld = wxDynamicCast( (*it), FieldShape );
                    if( fld )
                    {
                        fld->Select( false );
                        dynamic_cast<DrawingView *>( m_view )->AddFieldToQuery( *fld, REMOVE, const_cast<DatabaseTable *>( erdTable->GetTable() )->GetTableName(), false );
                    }
                    Refresh();
                }
            }
            break;
        case wxID_PROPERTIES:
            {
                if( erdTable && !erdField )
                    dynamic_cast<DrawingView *>( m_view )->SetProperties( erdTable );
                else if( erdTable && erdField )
                    dynamic_cast<DrawingView *>( m_view )->SetProperties( erdField );
            }
            break;
    }
/*    if( rc == wxID_NONE && erdField )
    {
        if( field )
        {
            erdField->Select( false );
            erdTable->GetFieldGrid()->Refresh();
            erdTable->Refresh();
        }
    }
    else
    {
        if( erdField )
        {
            erdField->Select( false );
            erdTable->GetFieldGrid()->Refresh();
            erdTable->Refresh();
        }
        wxCommandEvent evt( wxEVT_MENU, rc );
        if( erdField )
            evt.SetEventObject( erdField );
        else
            evt.SetEventObject( erdTable );
        if( rc == wxID_SHOWSQLTOOLBOX || rc == wxID_SHOWCOMMENTS )
            GetEventHandler()->ProcessEvent( evt );
        else
            m_view->ProcessEvent( evt );
    }*/
}

void DatabaseCanvas::OnMouseMove(wxMouseEvent &event)
{
    ViewType viewType = dynamic_cast<DrawingView *>( m_view )->GetViewType();
    if( viewType == QueryView || viewType == NewViewView )
    {
        wxSFShapeBase *shape = GetShapeUnderCursor();
        FieldShape *field = wxDynamicCast( shape, FieldShape );
        if( field )
        {
            SetCursor( wxCURSOR_HAND );
            return;
        }
        else
        {
            FieldTypeShape *type = wxDynamicCast( shape, FieldTypeShape );
            if( type )
            {
                SetCursor( wxCURSOR_HAND );
                return;
            }
            else
            {
                CommentFieldShape *comment = wxDynamicCast( shape, CommentFieldShape );
                if( comment )
                {
                    SetCursor( wxCURSOR_HAND );
                    return;
                }
                else
                {
                    MyErdTable *table = wxDynamicCast( shape, MyErdTable );
                    if( table )
                        SetCursor( wxCURSOR_HAND );
                    else
                        SetCursor( *wxSTANDARD_CURSOR );
                }
            }
        }
    }
    wxSFShapeCanvas::OnMouseMove( event );
}

void DatabaseCanvas::OnDropTable(wxCommandEvent &event)
{
    ShapeList list;
    bool isTable;
    int answer;
    MyErdTable *erdTable = NULL;
    DatabaseTable *table = NULL;
    wxString name;
    ConstraintSign *sign = NULL;
    Constraint *constraint = NULL;
    DrawingDocument *doc = (DrawingDocument *) m_view->GetDocument();
    Database *db = doc->GetDatabase();
    std::vector<std::wstring> errors, localColumns, refColumn;
    std::vector<FKField *> newFK;
    std::wstring command;
    int match = 0;
    GetSelectedShapes( list );
    if( list.size() == 1 )
        isTable = true;
    else
        isTable = false;
    for( ShapeList::iterator it = list.begin(); it != list.end(); it++ )
    {
        MyErdTable *tbl = wxDynamicCast( (*it), MyErdTable );
        if( tbl )
            erdTable = tbl;
        ConstraintSign *s = wxDynamicCast( (*it), ConstraintSign );
        if( s )
            sign = s;
    }
    if( isTable )
    {
        table = ( const_cast<DatabaseTable *>( erdTable->GetTable() ) );
        name = const_cast<DatabaseTable *>( erdTable->GetTable() )->GetTableName();
    }
    else
    {
        constraint = sign->GetConstraint();
        constraint->GetLocalColumns( localColumns );
        constraint->GetRefColumns( refColumn );
        match = constraint->GetPGMatch();
    }
    int eventId = event.GetId();
    if( eventId == wxID_DROPOBJECT )
    {
        wxString message = _( "You are about to delete " );
        if( isTable )
            message += _( "table " ) + name + _( ". Are you sure?" );
        else
        {
            message += _( "foreign key " );
            wxString fkName = constraint->GetName();
            if( !fkName.empty() )
                message += fkName;
            else
                message += _( " on " ) + const_cast<DatabaseTable *>( constraint->GetFKTable() )->GetTableName() + _( " references " ) + constraint->GetRefTable() + _( ". Are you sure?" );
        }
        answer = wxMessageBox( message, _( "Database" ), wxYES_NO | wxNO_DEFAULT );
    }
    else
        answer = wxYES;
    if( answer == wxYES )
    {
        if( isTable && ( ( eventId == wxID_DROPOBJECT && !db->DeleteTable( name.ToStdWstring(), errors ) ) || eventId != wxID_DROPOBJECT ) )
        {
            if( m_realSelectedShape == m_selectedShape )
            {
                m_realSelectedShape = NULL;
                ShapeList listShapes;
                m_pManager.GetShapes( CLASSINFO( MyErdTable ), listShapes );
                unsigned long size = listShapes.size();
                if( listShapes.size() == 1 )
                    m_realSelectedShape = NULL;
                else
                {
                    MyErdTable *tableToRemove = (MyErdTable *) ( listShapes.Item( size - 1 )->GetData() );
                    if( tableToRemove == erdTable )
                        m_realSelectedShape = (MyErdTable *) ( listShapes.Item( size - 2 )->GetData() );
                    else
                    {
                        bool found = false;
                        int i;
                        for( i = 0; i < size - 1 || !found; i++ )
                            if( listShapes.Item( i )->GetData() == erdTable )
                                found = true;
                        m_realSelectedShape = listShapes.Item( i + 1 )->GetData();
                    }
                }
            }
            m_pManager.RemoveShape( erdTable );
/*            for( ShapeList::iterator it = listShapes.begin(); it != listShapes.end() || !nextShapeFound; ++it )
            {
                CommentFieldShape *shape = wxDynamicCast( (*it), CommentFieldShape );
                if( m_showComments )
                {
                    shape->SetText( const_cast<Field *>( shape->GetFieldForComment() )->GetComment() );
                }
                else
                {
                    shape->SetText( wxEmptyString );
                }
            }*/
            std::map<std::wstring, std::vector<DatabaseTable *> > tables = db->GetTableVector().m_tables;
            std::vector<DatabaseTable *> tableVec = tables.at( db->GetTableVector().m_dbName );
            std::vector<std::wstring> &names = doc->GetTableNameVector();
            if( event.GetId() == wxID_DROPOBJECT )
            {
                tableVec.erase( std::remove( tableVec.begin(), tableVec.end(), table ), tableVec.end() );
            }
            else
                names.erase( std::remove( names.begin(), names.end(), table->GetTableName() ), names.end() );
/*            if( m_realSelectedShape == m_selectedShape )
            {
                
            }
            else
            {*/
                if( m_realSelectedShape )
                    m_realSelectedShape->Select( true );
//            }
        }
        else if( !isTable && !db->ApplyForeignKey( command, constraint->GetName().ToStdWstring(), *( const_cast<DatabaseTable *>( constraint->GetFKTable() ) ), localColumns, constraint->GetRefTable().ToStdWstring(), refColumn, constraint->GetOnDelete(), constraint->GetOnUpdate(), false, newFK, false, match, errors ) )
        {
            sign->DeleteConstraint();
            m_pManager.RemoveShape( sign->GetParentShape() );
            Refresh();
        }
        else
        {
            for( std::vector<std::wstring>::iterator it = errors.begin(); it < errors.end(); it++ )
            {
                wxMessageBox( (*it) );
            }
        }
    }
    Refresh();
}

void DatabaseCanvas::CreateFKConstraint(const DatabaseTable *fkTable, const std::vector<FKField *> &foreignKeyField)
{
    Constraint* pConstr = NULL;
    bool found = false;
    for( std::vector<MyErdTable *>::iterator it = m_displayedTables.begin(); it < m_displayedTables.end() && !found; it ++ )
    {
        if( const_cast<DatabaseTable *>( (*it)->GetTable() )->GetTableName() == foreignKeyField.at( 0 )->GetReferencedTableName() )
        {
            found = true;
            for( std::vector<FKField *>::const_iterator it4 = foreignKeyField.begin(); it4 < foreignKeyField.end(); it4++ )
            {
                if( ((DrawingView *) m_view)->GetViewType() == DatabaseView )
                {
                    pConstr = new DatabaseConstraint( (*it4)->GetFKName() );
                    pConstr->SetLocalColumn( (*it4)->GetOriginalFieldName() );
                    pConstr->SetRefColumn( (*it4)->GetReferencedFieldName() );
                    pConstr->SetRefTable( (*it4)->GetReferencedTableName() );
                    pConstr->SetType( Constraint::foreignKey );
                    pConstr->SetFKDatabaseTable( fkTable );
                    pConstr->SetPGMatch( (*it4)->GetMatchOPtion() );
                    switch( foreignKeyField.at( 0 )->GetOnUpdateConstraint() )
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
                    switch( foreignKeyField.at( 0 )->GetOnDeleteConstraint() )
                    {
                        case RESTRICT_DELETE:
                            pConstr->SetOnUpdate( Constraint::restrict );
                            break;
                        case SET_NULL_DELETE:
                            pConstr->SetOnUpdate( Constraint::setNull );
                            break;
                        case SET_DEFAULT_DELETE:
                        case CASCADE_DELETE:
                            pConstr->SetOnUpdate( Constraint::cascade );
                            break;
                        case NO_ACTION_DELETE:
                            pConstr->SetOnUpdate( Constraint::noAction );
                            break;
                    }
                    (*it)->GetShapeManager()->CreateConnection( (*it)->GetId(), dynamic_cast<DrawingDocument *>( m_view->GetDocument() )->GetReferencedTable( foreignKeyField.at( 0 )->GetReferencedTableName() )->GetId(), new ErdForeignKey( pConstr, ((DrawingView *) m_view)->GetViewType(), GetDiagramManager() ), sfDONT_SAVE_STATE );
                }
            }
        }
    }
}

void DatabaseCanvas::OnLeftDoubleClick(wxMouseEvent& event)
{
    int result;
    std::vector<std::wstring> errors;
    m_selectedShape = GetShapeUnderCursor();
    std::vector<FKField *> newFK;
    ViewType type = dynamic_cast<DrawingView *>( m_view )->GetViewType();
    ConstraintSign *sign = NULL;
    if( m_selectedShape )
    {
        ShapeList list;
        GetShapesAtPosition( event.GetPosition(), list );
        bool found = false;
        for( ShapeList::iterator it = list.begin(); it != list.end() && !found; it++ )
        {
            sign = wxDynamicCast( (*it), ConstraintSign );
            if( sign )
                found = true;
        }
        if( type == DatabaseView && !sign )
            DeselectAll();
        if( sign && type == DatabaseView )
        {
            bool logOnly;
            Constraint *constraint = sign->GetConstraint();
            std::wstring kName = constraint->GetName().ToStdWstring(), refTable, fkTable;
            std::vector<std::wstring> foreignKeyFields, refKeyFields;
            constraint->GetLocalColumns( foreignKeyFields );
            constraint->GetRefColumns( refKeyFields );
            DatabaseTable *table = const_cast<DatabaseTable *>( constraint->GetFKTable() );
            wxString refTableName = constraint->GetRefTable();
            int match = constraint->GetPGMatch();
            bool found1 = false, found2 = false;
            for( std::vector<MyErdTable *>::iterator it = m_displayedTables.begin(); it < m_displayedTables.end() && !found1 && !found2; it++ )
            {
                if( const_cast<DatabaseTable *>( (*it)->GetTable() )->GetTableName() == const_cast<DatabaseTable *>( constraint->GetFKTable() )->GetTableName() )
                {
                    (*it)->Select( true );
                    fkTable = const_cast<DatabaseTable *>( (*it)->GetTable() )->GetTableName();
                    found1 = true;
                }
                if( const_cast<DatabaseTable *>( (*it)->GetTable() )->GetTableName() == refTableName )
                {
                    refTable = const_cast<DatabaseTable *>( (*it)->GetTable() )->GetTableName();
                    found2 = true;
                }
            }
            int deleteProp, updateProp;
            FK_ONUPDATE actionUpdate;
            FK_ONDELETE actionDelete;
            Constraint::constraintAction action = constraint->GetOnDelete();
            if( action == Constraint::restrict )
            {
                deleteProp = 1;
                actionDelete = RESTRICT_DELETE;
            }
            if( action == Constraint::cascade )
            {
                deleteProp = 2;
                actionDelete = CASCADE_DELETE;
            }
            if( action == Constraint::setNull )
            {
                deleteProp = 3;
                actionDelete = SET_NULL_DELETE;
            }
            if( action == Constraint::noAction )
            {
                deleteProp = 0;
                actionDelete = NO_ACTION_DELETE;
            }
            if( action == Constraint::setDefault )
            {
                deleteProp = 4;
                actionDelete = SET_DEFAULT_DELETE;
            }
            action = constraint->GetOnUpdate();
            if( action == Constraint::restrict )
            {
                updateProp = 1;
                actionUpdate = RESTRICT_UPDATE;
            }
            if( action == Constraint::cascade )
            {
                updateProp = 2;
                actionUpdate = CASCADE_UPDATE;
            }
            if( action == Constraint::setNull )
            {
                updateProp = 3;
                actionUpdate = SET_NULL_UPDATE;
            }
            if( action == Constraint::noAction )
            {
                updateProp = 0;
                actionUpdate = NO_ACTION_UPDATE;
            }
            if( action == Constraint::setDefault )
            {
                updateProp = 4;
                actionUpdate = SET_DEFAULT_UPDATE;
            }
/*            int id = 0;
            std::vector<std::wstring>::iterator it1 = refKeyFields.begin();
            for( std::vector<std::wstring>::iterator it = foreignKeyFields.begin(); it < foreignKeyFields.end(); it++ )
            {
                newFK.push_back( new FKField( id, kName, L"", const_cast<DatabaseTable *>( constraint->GetFKTable() )->GetTableName(), (*it), L"", constraint->GetRefTable().ToStdWstring(),  (*it1), actionUpdate, actionDelete ) );
                id++;
                it1++;
            }*/
            wxString libName;
            wxDynamicLibrary lib;
#ifdef __WXMSW__
            libName = m_libPath + "dialogs";
#elif __WXMAC__
            libName = m_libPath + "liblibdialogs.dylib";
#else
            libName = m_libPath + "libdialogs";
#endif
            lib.Load( libName );
            wxString constraintName = constraint->GetName();
//                std::wstring refTableName = constraint->GetRefTable().ToStdWstring();
            if( lib.IsLoaded() )
            {
                CREATEFOREIGNKEY func = (CREATEFOREIGNKEY) lib.GetSymbol( "CreateForeignKey" );
                result = func( m_view->GetFrame(), constraintName, table, foreignKeyFields, refKeyFields, const_cast<std::wstring &>( refTableName.ToStdWstring() ), deleteProp, updateProp, dynamic_cast<DrawingDocument *>( m_view->GetDocument() )->GetDatabase(),  logOnly, true, newFK, match );
                if( result != wxID_CANCEL )
                {
                    std::wstring command = L"";
                    int res = ((DrawingDocument *) m_view->GetDocument())->GetDatabase()->ApplyForeignKey( command, kName, *table, foreignKeyFields, refTableName.ToStdWstring(), refKeyFields, deleteProp, updateProp, logOnly, newFK, false, match, errors );
                    if( res )
                    {
                        for( std::vector<std::wstring>::iterator it = errors.begin(); it < errors.end(); it++ )
                        {
                            wxMessageBox( (*it), _( "Error" ), wxOK | wxICON_ERROR );
                        }
                    }
                    else if( logOnly )
                    {
                        dynamic_cast<DrawingView *>( m_view )->GetTextLogger()->AppendText( command );
                        dynamic_cast<DrawingView *>( m_view )->GetTextLogger()->AppendText( "\n\r\n\r" );
                        if( !dynamic_cast<DrawingView *>( m_view )->GetLogWindow()->IsShown() )
                            dynamic_cast<DrawingView *>( m_view )->GetLogWindow()->Show();
                    }
                    else
                    {
                        sign->DeleteConstraint();
                        m_pManager.RemoveShape( sign->GetParentShape() );
//                        Refresh();
//                        m_pManager.RemoveShape( sign );
                        if( newFK.size() > 0 )
                            CreateFKConstraint( table, newFK );
                        Refresh();
                    }
                }
                for( std::vector<FKField *>::iterator it = newFK.begin(); it < newFK.end(); ++it )
                {
                    delete (*it);
                    (*it) = NULL;
                }
                newFK.clear();
            }
        }
        else if( sign && type == QueryView )
        {
            wxString libName;
            wxDynamicLibrary lib;
#ifdef __WXMSW__
            libName = m_libPath + "dialogs";
#elif __WXMAC__
            libName = m_libPath + "liblibdialogs.dylib";
#else
            libName = m_libPath + "libdialogs";
#endif
            lib.Load( libName );
            QueryConstraint *constraint = (QueryConstraint *) sign->GetConstraint();
            long oldSign = 0;
            int constraintSign = oldSign = constraint->GetSign();
            SELECTJOINTYPE func = (SELECTJOINTYPE) lib.GetSymbol( "SelectJoinType" );
            result = func( m_view->GetFrame(), const_cast<DatabaseTable *>( constraint->GetFKTable() )->GetTableName(), constraint->GetRefTable(), constraint->GetLocalColumn(), constraint->GetRefColumn(), constraintSign );
            if( constraintSign != constraint->GetSign () )
            {
                switch( constraintSign )
                {
                case 0:
                case 1:
                case 2:
                    sign->SetSign( "=" );
                    break;
                case 3:
                    sign->SetSign( "<" );
                    break;
                case 4:
                    sign->SetSign( ">" );
                    break;
                case 5:
                    sign->SetSign( "<=" );
                    break;
                case 6:
                    sign->SetSign( "<>" );
                    break;
                }
                constraint->SetSign( constraintSign );
                ((DrawingView *) m_view)->UpdateQueryFromSignChange( constraint, oldSign );
            }
            sign->Select( false );
            Refresh();
        }
        m_oldSelectedSign = NULL;
    }
}

void DatabaseCanvas::AddQuickQueryFields(const wxString &tbl, std::vector<TableField *> &quickSelectFields, bool quickSelect)
{
    ShapeList shapes;
    m_pManager.GetShapes( CLASSINFO( FieldShape ), shapes );
    for( ShapeList::iterator it = shapes.begin (); it != shapes.end (); ++it )
    {
        auto fld = wxDynamicCast( (*it), FieldShape );
        auto fieldName = wxDynamicCast( (*it), FieldShape )->GetField()->GetFieldName();
        auto found = false;
        for( std::vector<TableField *>::iterator it2 = quickSelectFields.begin(); it2 < quickSelectFields.end() && !false; ++it2 )
        {
            if( (*it2)->GetFieldName() == fieldName )
            {
                found = true;
                (*it)->Select( true );
                dynamic_cast<DrawingView *>( m_view )->AddFieldToQuery( *fld, fld->IsSelected() ? ADD : REMOVE, tbl.ToStdWstring(), quickSelect );
            }
        }
    }
    Refresh();
}

void DatabaseCanvas::ShowHideTablePart(int part, bool show)
{
    ShapeList list;
    m_pManager.GetShapes( CLASSINFO( MyErdTable ), list );
    for( ShapeList::iterator it = list.begin(); it != list.end(); ++it )
    {
        MyErdTable *shape = wxDynamicCast( (*it), MyErdTable );
        switch( part )
        {
            case 1:
                m_showDataTypes = !m_showDataTypes;
                shape->DisplayTypes( show, DISPLAYTYPES );
                dynamic_cast<DrawingView *>( m_view )->ChangeTableTypeMMenu();
                break;
            case 4:
                m_showComments = !m_showComments;
                shape->DisplayTypes( show, DISPLAYCOMMENTS );
                dynamic_cast<DrawingView *>( m_view )->ChangeTableCommentsMenu();
                break;
        }
        shape->UpdateTable();
    }
    Refresh();
}

void DatabaseCanvas::OnCloseTable(wxCommandEvent &WXUNUSED(event))
{
    wxString replace;
    DrawingView *view = dynamic_cast<DrawingView *>( m_view );
    if( m_displayedTables.size () == 1 )
    {
        m_pManager.RemoveShape( m_selectedShape );
        m_displayedTables.clear();
        view->DropTableFromQeury();
    }
    else
    {
        m_selectedShape = GetShapeUnderCursor();
        wxString tbl = wxDynamicCast( m_selectedShape, MyErdTable )->GetTable()->GetTableName();
        ShapeList selection, children;
        m_selectedShape->GetChildShapes( CLASSINFO( FieldShape ), children, true );
        GetSelectedShapes( selection );
        for( ShapeList::iterator it = selection.begin(); it != selection.end(); it++ )
        {
            FieldShape *field = wxDynamicCast( (*it), FieldShape );
            if( children.Find( field ) )
                view->AddFieldToQuery( *field, REMOVE, tbl.ToStdWstring(), true );
        }
        view->GetSortPage()->RemoveTable( tbl );

        view->GetDocument()->DeleteSortedTable( tbl, replace );

        view->GetGroupByPage()->RemoveTable( tbl );

        view->GetDocument()->DeleteGroupByTable( tbl, replace );

        view->GetSyntaxPage()->RemoveTableFromQuery( tbl );
        m_pManager.RemoveShape( m_selectedShape );
        view->DropTableFromQeury( tbl );
    }
}

void DatabaseCanvas::CheckSQLToolbox()
{
    m_showToolBox = !m_showToolBox;
}

void DatabaseCanvas::GetAllSelectedShapes(ShapeList &shapes)
{
    GetSelectedShapes( shapes );
}

bool DatabaseCanvas::UpdateCanvasWithQuery()
{
    auto success = true;
    auto root = wxDynamicCast( m_pManager.GetRootItem(), QueryRoot );
    if( root )
    {
        if( root->GetDbType() != m_dbType )
        {
            wxMessageBox( _( wxString::Format( "The database type you are connected to does not match the database type of the qery you are loading. Connected to %s, loading %s", m_dbType, root->GetDbType() ) ) );
            success = false;
        }
        else if( root->GetDbName() != m_dbName )
        {
            wxMessageBox( _( wxString::Format( "The name of the database you are connected to does not match the name of the database of the query you are loading. Connected to %s, loading %s", m_dbName, root->GetDbName() ) ) );
            success = false;
        }
        else
        {
            ShapeList lstShapes;
            GetDiagramManager().GetShapes( CLASSINFO( MyErdTable ), lstShapes );
            for( ShapeList::iterator it = lstShapes.begin(); it != lstShapes.end(); ++it )
            {
                (( MyErdTable*) *it )->UpdateTable();
            }
            Refresh( false );
        }
    }
    Refresh();
    return success;
}

void DatabaseCanvas::LoadQuery(const std::map<std::wstring, std::vector<DatabaseTable *> > &tables)
{
    ShapeList listTables, listConnections;
    std::vector<std::wstring> errors;
    dynamic_cast<DrawingView *>( m_view )->GetSyntaxPage()->GetSyntaxCtrl()->SetValue( dynamic_cast<QueryRoot *>( m_pManager.GetRootItem() )->GetQuery() );
    m_pManager.GetShapes( CLASSINFO( MyErdTable ), listTables );
    m_pManager.GetShapes( CLASSINFO( ErdLineShape ), listConnections );
    auto fields = dynamic_cast<QueryRoot *>( m_pManager.GetRootItem() )->GetFields();
    for( ShapeList::iterator it = listTables.begin(); it != listTables.end(); it++ )
    {
        auto found = false;
        MyErdTable *table = ((MyErdTable*) *it );
        for( auto it1 = tables.begin(); it1 != tables.end() && !found; ++it1 )
        {
            for( auto it2 = (*it1).second.begin(); it2 < (*it1).second.end() && !found; ++it2 )
            {
                auto index = 0;
                if( m_dbType == L"SQLite" )
                {
                    if( table->GetSchemaName() == (*it2)->GetSchemaName() && table->GetTableName() == (*it2)->GetTableName() )
                    {
                        found = true;
                        table->SetDataaseTable( (*it2) );
                        for( auto field : (*it2)->GetFields() )
                        {
                            dynamic_cast<DrawingView *>( m_view )->GetGroupByPage()->GetSourceList()->InsertItem( index++, table->GetSchemaName() + "." + table->GetTableName() + "." + field->GetFieldName() );
                        }
                    }
                }
                else
                {
                    if( table->GetCatalogName() == (*it2)->GetCatalog() && table->GetSchemaName() == (*it2)->GetSchemaName() && table->GetTableName() == (*it2)->GetTableName() )
                    {
                        found = true;
                        table->SetDataaseTable( (*it2) );
                        for( auto field : (*it2)->GetFields() )
                        {
                            dynamic_cast<DrawingView *>( m_view )->GetGroupByPage()->GetSourceList()->InsertItem( index++, table->GetCatalogName() + "." + table->GetSchemaName() + "." + table->GetTableName() + "." + field->GetFieldName() );
                        }
                    }
                }
            }
        }
        found = false;
        for( ShapeList::iterator it0 = listConnections.begin(); it0 != listConnections.end() && !found; it0++ )
        {
            ErdLineShape *connection = ((ErdLineShape *) *it0);
            if( m_dbType == L"SQLite" )
            {
                if( connection->GetSourceName() == table->GetTable()->GetSchemaName() + "." + table->GetTable()->GetTableName() )
                    connection->SetSourceTable( table );
                if( connection->GetTargetName() == table->GetTable()->GetSchemaName() + "." + table->GetTable()->GetTableName() )
                    connection->SetTargetTable( table );
            }
            else
            {

            }
        }
    }
    auto args = dynamic_cast<QueryRoot *>( m_pManager.GetRootItem() )->GetQueryArguments();
    std::vector<QueryArguments> arguments;
    for( wxArrayString::iterator it4 = args.begin(); it4 < args.end(); ++it4 )
    {
        wxStringTokenizer tokens( (*it4) );
        auto pos = wxAtoi( tokens.GetNextToken() );
        auto name = tokens.GetNextToken();
        auto type = tokens.GetNextToken();
        dynamic_cast<DrawingView * >( m_view )->SetQueryArguments( QueryArguments( pos, name, type ) );
        arguments.push_back( QueryArguments( pos, name, type ) );
    }
    dynamic_cast<DrawingView *>( m_view )->GetWherePage()->SetQueryArguments( arguments );
    dynamic_cast<DrawingView *>( m_view )->GetHavingPage()->SetQueryArguments( arguments );
    UpdateCanvasWithQuery();
    for( ShapeList::iterator it = listTables.begin(); it != listTables.end(); it++ )
    {
        auto found = false;
        MyErdTable *table = ((MyErdTable*) *it );
        for( auto it1 = tables.begin(); it1 != tables.end() && !found; ++it1 )
        {
            for( auto it2 = (*it1).second.begin(); it2 < (*it1).second.end() && !found; ++it2 )
            {
                if( m_dbType == L"SQLite" )
                {
                    if( table->GetSchemaName() == (*it2)->GetSchemaName() && table->GetTableName() == (*it2)->GetTableName() )
                    {
                        found = true;
                        table->SetDataaseTable( (*it2) );
                    }
                }
                else
                {
                    if( table->GetCatalogName() == (*it2)->GetCatalog() && table->GetSchemaName() == (*it2)->GetSchemaName() && table->GetTableName() == (*it2)->GetTableName() )
                    {
                        found = true;
                        table->SetDataaseTable( (*it2) );
                    }
                }
            }
            for( auto field : fields )
            {
                auto temp3 = field.substr( field.find( "." ) + 1 );
                temp3 = temp3.substr( 0, temp3.find( "." ) );
                auto temp4 = table->GetTable()->GetTableName();
                ShapeList guiFields;
                if( m_dbType == L"SQLite" )
                {
                    if( field.substr( 0, field.find( "." ) ) == table->GetTable()->GetSchemaName() && temp3 == table->GetTable()->GetTableName() )
                    {
                        table->GetChildShapes( CLASSINFO( FieldShape ), guiFields, true );
                        for( ShapeList::iterator it6 = guiFields.begin(); it6 != guiFields.end(); ++it6 )
                        {
                            if( dynamic_cast<FieldShape *>( (*it6) )->GetField()->GetFullName() == field )
                            {
                                (*it6)->Select( true );
                                dynamic_cast<DrawingView *>( m_view )->AddFieldToQuery( *( dynamic_cast<FieldShape *>( (*it6) ) ), ADD, const_cast<DatabaseTable *>( table->GetTable() )->GetFullName(), false );
                            }
                        }
                    }
                }
                else
                {

                }
            }
        }
    }
    for( auto str : dynamic_cast<QueryRoot *>( m_pManager.GetRootItem() )->GetSortDest() )
    {
        auto pos = 0;
        auto found = false;
        wxStringTokenizer tokens( str );
        wxVector<wxVariant> data;
        auto name = tokens.GetNextToken();
        data.push_back( name );
        for( ; pos < dynamic_cast<DrawingView *>( m_view )->GetSortPage()->GetSortSourceList()->GetItemCount() && !found; ++pos )
        {
            if( dynamic_cast<DrawingView *>( m_view )->GetSortPage()->GetSortSourceList()->GetTextValue( pos, 0 ) == name )
            {
                dynamic_cast<DrawingView *>( m_view )->GetSortPage()->GetSortSourceList()->DeleteItem( pos );
                found = true;
            }
        }
        if( wxAtoi( tokens.GetNextToken() ) == 1 )
            data.push_back( (wxVariant)  true );
        else
            data.push_back( (wxVariant)  false );
        dynamic_cast<DrawingView *>( m_view )->GetSortPage()->GetSourceDestList()->AppendItem( data, pos );
    }
    auto rowCount = dynamic_cast<DrawingView *>( m_view )->GetWherePage()->GetGrid()->GetNumberRows();
    for( auto str : dynamic_cast<QueryRoot *>( m_pManager.GetRootItem() )->GetWhereLines() )
    {
        wxStringTokenizer tokens( str, "," );
        auto row = wxAtoi( tokens.GetNextToken() );
        auto var = tokens.GetNextToken();
        auto sign = tokens.GetNextToken();
        auto value = tokens.GetNextToken();
        auto condition = tokens.GetNextToken();
        if( rowCount < row )
            dynamic_cast<DrawingView *>( m_view )->GetWherePage()->AppendRowsToGrid( row - rowCount );
        dynamic_cast<DrawingView *>( m_view )->GetWherePage()->GetGrid()->SetCellValue( row, 0, var );
        dynamic_cast<DrawingView *>( m_view )->GetWherePage()->GetGrid()->SetCellValue( row, 1, sign );
        dynamic_cast<DrawingView *>( m_view )->GetWherePage()->GetGrid()->SetCellValue( row, 2, value );
        dynamic_cast<DrawingView *>( m_view )->GetWherePage()->GetGrid()->SetCellValue( row, 3, condition );
    }
    for( auto str : dynamic_cast<QueryRoot *>( m_pManager.GetRootItem() )->GetGropByDest() )
    {
//        dynamic_cast<DrawingView *>( m_view )->GetGroupByPage()->GetDestList()->AppendString( str );
    }
}

void DatabaseCanvas::SetQueryArguments(const std::vector<QueryArguments> arguments)
{
    dynamic_cast<QueryRoot *>( m_pManager.GetRootItem() )->AddQueryArgument( arguments );
}

void DatabaseCanvas::nUpdateTableParam( wxUpdateUIEvent &event )
{
    switch( event.GetId() )
    {
    case wxID_SHOWDATATYPES:
        event.Check( m_showDataTypes );
        break;
    case wxID_SHOWCOMMENTS:
        event.Check( m_showComments );
        break;
    }
}

void DatabaseCanvas::UnselectAllTables()
{
    ShapeList selected;
    m_pManager.GetShapes( CLASSINFO( MyErdTable ), selected );
    for( ShapeList::iterator it1 = selected.begin(); it1 != selected.end(); ++it1 )
    {
        (*it1)->Select( false );
    }
}

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#ifdef __WXOSX__
#include "../dbhandler/res/database_profile.xpm"
#include "../dbhandler/res/table.xpm"
#include "../dbhandler/res/properties.xpm"
#endif

#include <string>
#include "wx/docview.h"
#include "wx/grid.h"
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "wx/cmdproc.h"
#include "database.h"
#include "tablecanvas.h"
#include "tabledoc.h"
#include "tableview.h"

const wxEventTypeTag<wxCommandEvent> wxEVT_SET_TABLE_PROPERTY( wxEVT_USER_FIRST + 1 );

typedef int (*TABLESELECTION)(wxDocMDIChildFrame *, Database *, std::vector<wxString> &, std::vector<std::wstring> &, bool);
typedef int (*CREATEINDEX)(wxWindow *, DatabaseTable *, Database *, wxString &);
typedef int (*CREATEPROPERTIESDIALOG)(wxWindow *parent, Database *, int type, void *object, wxString &, bool, const wxString &, const wxString &);
typedef int (*CREATEFOREIGNKEY)(wxWindow *parent, DatabaseTable *, Database *, wxString &, bool &);

// ----------------------------------------------------------------------------
// DrawingView implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(TableView, wxView);

wxBEGIN_EVENT_TABLE(TableView, wxView)
wxEND_EVENT_TABLE()

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool TableView::OnCreate(wxDocument *doc, long flags)
{
    m_isCreated = false;
    if( !wxView::OnCreate( doc, flags ) )
        return false;
    wxDocMDIParentFrame *parent = wxStaticCast( wxTheApp->GetTopWindow(), wxDocMDIParentFrame );
    wxWindowList children = parent->GetChildren();
    bool found = false;
    int height = 0;
    for( wxWindowList::iterator it = children.begin(); it != children.end() && !found; it++ )
    {
        m_tb = wxDynamicCast( *it, wxToolBar );
        if( m_tb && m_tb->GetName() == "Second Toolbar" )
        {
            found = true;
            height = m_tb->GetSize().GetHeight();
        }
    }
    auto stdPath = wxStandardPaths::Get();
#ifdef __WXOSX__
    wxFileName fn( stdPath.GetExecutablePath() );
    fn.RemoveLastDir();
    m_libPath = fn.GetPathWithSe() + "Frameworks/" + wxFileName::GetPathSeparator();
#elif __WXGTK__
    m_libPath = stdPath.GetInstallPrefix() + "/lib/";
#elif __WXMSW__
    m_libPath = stdPath.GetExecutablePath() + "/";
#endif
    wxPoint start( 0, height );
    wxRect clientRect = parent->GetClientRect();
    clientRect.height -= height;
    m_frame = new wxDocMDIChildFrame( doc, this, parent, wxID_ANY, _T( "Database" ), /*wxDefaultPosition*/start, wxSize( clientRect.GetWidth(), clientRect.GetHeight() ) );
#ifdef __WXOSX__
    wxRect parentRect = parent->GetRect();
    wxSize parentClientSize = parent->GetClientSize();
    wxPoint pt;
    pt.x = -1;
    pt.y = parentRect.height - parentClientSize.GetHeight();
    m_frame->Move( pt.x, pt.y );
    m_tb = new wxToolBar( m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_TOP, "Second Toolbar" );
    wxBitmap tmp = wxBitmap( database_profile );
    m_tb->AddTool( wxID_DATABASEWINDOW, _( "Database Profile" ), wxBitmap( database_profile ), wxBitmap( database_profile ), wxITEM_NORMAL, _( "DB Profile" ), _( "Select database profile" ) );
    m_tb->AddTool( wxID_SELECTTABLE, _( "Select Table" ), wxBitmap( table ), wxBitmap( table ), wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" ) );
    m_tb->AddTool( wxID_PROPERTIES, _( "Properties" ), wxBitmap( properties ), wxBitmap( properties ), wxITEM_NORMAL, _( "Properties" ), _( "Proerties" ) );
    m_tb->Realize();
    m_frame->SetToolBar( m_tb );
#endif
    wxASSERT( m_frame == GetFrame() );
    m_frame->Show();
    return true;
}

// Sneakily gets used for default print/preview as well as drawing on the
// screen.
void TableView::OnDraw(wxDC *dc)
{
    dc->SetPen( *wxBLACK_PEN );
}

//std::vector<Table> &DrawingView::GetTablesForView(Database *db)
void TableView::GetTablesForView(Database *db)
{
    std::vector<wxString> tables;
    wxDynamicLibrary lib;
    wxString libName;
#ifdef __WXMSW__
    libName = m_libPath + "dialogs";
#elif __WXMAC__
    libName = m_libPath + "liblibdialogs.dylib";
#else
    libName = m_libPath + "libdialogs";
#endif
    lib.Load( libName );
    if( lib.IsLoaded() )
    {
        TABLESELECTION func = (TABLESELECTION) lib.GetSymbol( "SelectTablesForView" );
        int res = func( m_frame, db, tables, GetDocument()->GetTableNames(), true );
        if( res != wxID_CANCEL )
        {
            bool found = false;
            std::map<std::wstring, std::vector<DatabaseTable *> > tbls = db->GetTableVector().m_tables;
            std::vector<DatabaseTable *> tableVec = tbls.at( db->GetTableVector().m_dbName );
            for( std::vector<DatabaseTable *>::iterator it = tableVec.begin(); it < tableVec.end() && !found; it++ )
            {
                if( (*it)->GetTableName() == tables.at( 0 ).ToStdWstring() )
                {
                    m_table = (*it);
                    found = true;
                }
            }
            ((TableDocument *) GetDocument())->AddTables( tables );
        }
    }
//    return tables;
}

DatabaseTable *TableView::GetDatabaseTable()
{
    return m_table;
}

TableDocument* TableView::GetDocument()
{
    return wxStaticCast( wxView::GetDocument(), TableDocument );
}

void TableView::OnUpdate(wxView* sender, wxObject* hint)
{
    wxView::OnUpdate( sender, hint );
}

// Clean up windows used for displaying the view.
bool TableView::OnClose(bool deleteWindow)
{
    if( !wxView::OnClose( deleteWindow ) )
        return false;

    Activate( false );

    if( deleteWindow )
    {
        GetFrame()->Destroy();
        SetFrame( NULL );
    }
    wxDocManager *manager = GetDocumentManager();
    if( manager->GetDocumentsVector().size() == 0 )
    {
        delete m_tb;
        m_tb = NULL;
    }
    return true;
}

void TableView::OnViewSelectedTables(wxCommandEvent &WXUNUSED(event))
{
    GetTablesForView( GetDocument()->GetDatabase() );
}

void TableView::OnFieldDefinition(wxCommandEvent &WXUNUSED(event))
{
    wxMessageBox( "Field definition" );
}

void TableView::OnFieldProperties(wxCommandEvent &WXUNUSED(event))
{
    std::vector<std::wstring> errors;
    bool found = false;
    int type = 0;
    DatabaseTable *table = NULL;
    TableField *field = NULL;
    wxString command = "";
    bool logOnly = false;
    wxString tableName, schemaName;
    wxDynamicLibrary lib;
    wxString libName;
#ifdef __WXMSW__
    lib.Load( "dialogs" );
#elif __WXMAC__
    lib.Load( m_libPath + "liblibdialogs.dylib" );
#else
    lib.Load( "libdialogs" );
#endif
    lib.Load( libName );
    int res = 0;
    if( lib.IsLoaded() )
    {
        CREATEPROPERTIESDIALOG func = (CREATEPROPERTIESDIALOG) lib.GetSymbol( "CreatePropertiesDialog" );
        if( type == 0 )
            res = func( m_frame, GetDocument()->GetDatabase(), type, table, command, logOnly, wxEmptyString, wxEmptyString );
        if( type == 1 )
            res = func( m_frame, GetDocument()->GetDatabase(), type, field, command, logOnly, tableName, schemaName );
        if( res != wxID_CANCEL && logOnly )
        {
        }
/*        if( res != wxID_CANCEL )
        {
            if( type == 0 )
                res = GetDocument()->GetDatabase()->SetTableProperties( command.ToStdWstring(), errors );
            if( type == 1 )
                GetDocument()->GetDatabase();
            if( res )
            {
                for( std::vector<std::wstring>::iterator it = errors.begin(); it < errors.end(); it++ )
                    wxMessageBox( (*it) );
            }
            else
            {
                if( type == 0 )
                {
                    GetDocument()->GetDatabase()->GetTableProperties( table, errors );
                    erdTable->SetTableComment( table->GetComment() );
                    erdTable->UpdateTable();
                }
            }
        }*/
    }
}

void TableView::OnLogUpdateUI(wxUpdateUIEvent &WXUNUSED(event))
{
}

wxDocMDIChildFrame *TableView::GetChildFrame()
{
    return m_frame;
}

void TableView::OnActivateView(bool activate, wxView *activeView, wxView *deactiveView)
{
    if( activate )
    {
        if( m_isCreated )
            return;
        wxDocMDIParentFrame *parent = wxDynamicCast( wxTheApp->GetTopWindow(), wxDocMDIParentFrame );
        wxWindowList children = parent->GetChildren();
        bool found = false;
        for( wxWindowList::iterator it = children.begin(); it != children.end() && !found; it++ )
        {
            m_tb = wxDynamicCast( *it, wxToolBar );
            if( m_tb && m_tb->GetName() == "Second Toolbar" )
                found = true;
        }
        wxMenuBar *bar = parent->GetMenuBar();
        wxMenu *file_menu = bar->GetMenu( 0 );
        if( file_menu->FindItem( wxID_NEW ) )
            file_menu->Delete( wxID_NEW );
        if( file_menu->FindItem( wxID_OPEN ) )
            file_menu->Delete( wxID_OPEN );
        if (!m_isCreated)
        {
            m_isCreated = true;
            return;
        }
    }
    else
    {
    }
}

void TableView::CreateMenu()
{
    auto bar = new wxMenuBar;
    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append( wxID_UNDO, _( "&Undo\tCtrl+Z" ), _( "Undo operation" ) );
    edit_menu->AppendSeparator();
    edit_menu->Append( wxID_EDITCUTCOLUMN, _( "Cu&t Column" ), _( "Cut Column" ) );
    edit_menu->Append( wxID_EDITCOPYCOLUMN, _( "&Copy Column" ), _( "Copy Column" ) );
    edit_menu->Append( wxID_EDITPASTECOLUMN, _( "&Paste Column" ), _( "Paste Column" ) );
    edit_menu->Append( wxID_EDITINSERTCOLUMN, _( "Insert Column" ), _( "Insert Column" ) );
    edit_menu->Append( wxID_EDITDELETECOLUMN, _( "Delete Column" ), _( "Delete Column" ) );
    edit_menu->AppendSeparator();
    edit_menu->Append( wxID_EDITTABLEPROPERTY, _( "Table Property" ), _( "Table Property" ) );
    bar->Insert( 1, edit_menu, _( "Edit" ) );
    wxMenu *design_menu = new wxMenu;
    design_menu->Append( wxID_DESIGNSYNTAX, _( "Syntax" ), _( "Syntax" ) );
    bar->Insert( 2, design_menu, _( "Design" ) );
    m_frame->SetMenuBar( bar );
}

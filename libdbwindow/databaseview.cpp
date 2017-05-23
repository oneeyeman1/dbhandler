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

//#ifdef __WXOSX__
#include "../dbhandler/res/database_profile.xpm"
#include "../dbhandler/res/table.xpm"
#include "../dbhandler/res/properties.xpm"
#include "res/gui/key-f1.xpm"
//#endif

#include <string>
#include "wx/docview.h"
#include "wx/notebook.h"
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/cmdproc.h"
#include "wx/grid.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "constraintsign.h"
#include "table.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "FieldShape.h"
#include "MyErdTable.h"
#include "fieldwindow.h"
#include "syntaxproppage.h"
#include "wherehavingpage.h"
#include "databasecanvas.h"
#include "databasedoc.h"
#include "databaseview.h"

const wxEventTypeTag<wxCommandEvent> wxEVT_SET_TABLE_PROPERTY( wxEVT_USER_FIRST + 1 );

typedef int (*TABLESELECTION)(wxDocMDIChildFrame *, Database *, std::vector<wxString> &, std::vector<std::wstring> &, bool);
typedef int (*CREATEINDEX)(wxWindow *, DatabaseTable *, Database *, wxString &);
typedef int (*CREATEPROPERTIESDIALOG)(wxWindow *parent, Database *, int type, void *object, wxString &, bool, const wxString &, const wxString &);
typedef int (*CREATEFOREIGNKEY)(wxWindow *parent, DatabaseTable *, Database *, wxString &, bool &);
typedef void (*TABLE)(wxWindow *, wxDocManager *, Database *, DatabaseTable *, const wxString &);
typedef int (*CHOOSEOBJECT)(wxWindow *, int);
typedef Database *(*DBPROFILE)(wxWindow *, const wxString &, wxString &);

// ----------------------------------------------------------------------------
// DrawingView implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(DrawingView, wxView);

wxBEGIN_EVENT_TABLE(DrawingView, wxView)
    EVT_MENU(wxID_SELECTTABLE, DrawingView::OnViewSelectedTables)
    EVT_MENU(wxID_OBJECTNEWINDEX, DrawingView::OnNewIndex)
    EVT_MENU(wxID_FIELDPROPERTIES, DrawingView::OnFieldProperties)
    EVT_MENU(wxID_PROPERTIES, DrawingView::OnFieldProperties)
    EVT_MENU(wxID_FIELDPROPERTIES, DrawingView::OnFieldProperties)
    EVT_MENU(wxID_OBJECTNEWFF, DrawingView::OnForeignKey)
    EVT_UPDATE_UI(wxID_STARTLOG, DrawingView::OnLogUpdateUI)
    EVT_UPDATE_UI(wxID_STOPLOG, DrawingView::OnLogUpdateUI)
    EVT_UPDATE_UI(wxID_SAVELOG, DrawingView::OnLogUpdateUI)
    EVT_UPDATE_UI(wxID_CLEARLOG, DrawingView::OnLogUpdateUI)
    EVT_MENU(wxID_STARTLOG, DrawingView::OnStartLog)
    EVT_MENU(wxID_STOPLOG, DrawingView::OnStopLog)
    EVT_MENU(wxID_SAVELOG, DrawingView::OnSaveLog)
    EVT_MENU(wxID_CLEARLOG, DrawingView::OnClearLog)
    EVT_MENU(wxID_TABLEALTERTABLE, DrawingView::OnAlterTable)
    EVT_MENU(wxID_FIELDDEFINITION, DrawingView::OnFieldDefinition)
    EVT_MENU(wxID_CREATEDATABASE, DrawingView::OnCreateDatabase)
    EVT_MENU(wxID_SELECTALLFIELDS, DrawingView::OnSelectAllFields)
    EVT_MENU(wxID_DESELECTALLFIELDS, DrawingView::OnSelectAllFields)
wxEND_EVENT_TABLE()

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool DrawingView::OnCreate(wxDocument *doc, long flags)
{
    m_isActive = false;
    m_tb = NULL;
    wxToolBar *tb = NULL;
    m_isCreated = false;
    m_fields = NULL;
    m_queryBook = NULL;
    m_page2 = m_page4 = NULL;
    m_page6 = NULL;
    if( !wxView::OnCreate( doc, flags ) )
        return false;
    wxDocMDIParentFrame *parent = wxStaticCast( wxTheApp->GetTopWindow(), wxDocMDIParentFrame );
    wxRect clientRect = parent->GetClientRect();
    wxWindowList children = parent->GetChildren();
    bool found = false;
    int height = 0;
    for( wxWindowList::iterator it = children.begin(); it != children.end() && !found; it++ )
    {
        tb = wxDynamicCast( *it, wxToolBar );
        if( m_tb && m_tb->GetName() == "ViewBar" )
        {
            found = true;
            m_tb = tb;
        }
    }
    m_frame = new wxDocMDIChildFrame( doc, this, parent, wxID_ANY, _T( "Database" ), wxDefaultPosition, wxSize( clientRect.GetWidth(), clientRect.GetHeight() ) );
    m_log = new wxFrame( m_frame, wxID_ANY, _( "Activity Log" ), wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT );
    m_text = new wxTextCtrl( m_log, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY );
    wxPoint ptCanvas;
#ifdef __WXOSX__
    wxRect parentRect = parent->GetRect();
    wxSize parentClientSize = parent->GetClientSize();
    wxPoint pt;
    pt.x = -1;
    pt.y = parentRect.height - parentClientSize.GetHeight();
    m_frame->SetSize( pt.x, pt.y, parentRect.GetWidth(), parentRect.GetHeight() );
/*    m_tb = new wxToolBar( m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_TOP, "Second Toolbar" );
    wxBitmap tmp = wxBitmap( database_profile );
    m_tb->AddTool( wxID_DATABASEWINDOW, _( "Database Profile" ), wxBitmap( database_profile ), wxBitmap( database_profile ), wxITEM_NORMAL, _( "DB Profile" ), _( "Select database profile" ) );
    m_tb->AddTool( wxID_SELECTTABLE, _( "Select Table" ), wxBitmap( table ), wxBitmap( table ), wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" ) );
    m_tb->AddTool( wxID_PROPERTIES, _( "Properties" ), wxBitmap( properties ), wxBitmap( properties ), wxITEM_NORMAL, _( "Properties" ), _( "Proerties" ) );
    m_tb->Realize();
    m_frame->SetToolBar( m_tb );
    ptCanvas.x = -1;
    ptCanvas.y = m_tb->GetSize().GetHeight();*/
#else
    ptCanvas = wxDefaultPosition;
#endif
    wxASSERT( m_frame == GetFrame() );
    sizer = new wxBoxSizer( wxVERTICAL );
    if( m_type == QueryView )
    {
        m_fields = new FieldWindow( m_frame, 1, wxDefaultPosition, wxDefaultCoord );
        sizer->Add( m_fields, 0, wxEXPAND, 0 );
        m_fields->Show( false );
    }
    m_canvas = new DatabaseCanvas( this, wxDefaultPosition/*ptCanvas*/ );
    sizer->Add( m_canvas, 2, wxEXPAND, 0 );
    if( m_type == QueryView )
    {
        m_queryBook = new wxNotebook( m_frame, wxID_ANY );
        m_page2 = new WhereHavingPage( m_queryBook );
        m_queryBook->AddPage( m_page2, _( "Where" ) );
        m_page4 = new WhereHavingPage( m_queryBook );
        m_queryBook->AddPage( m_page4, _( "Having" ) );
        m_page6 = new SyntaxPropPage( m_queryBook );
        m_queryBook->AddPage( m_page6, _( "Syntax" ), true );
        sizer->Add( m_queryBook, 0, wxEXPAND, 0 );
        m_queryBook->Show( false );
        m_queryBook->Bind( wxEVT_NOTEBOOK_PAGE_CHANGED, &DrawingView::OnSQLNotebookPageChanged, this );
    }
    m_frame->SetSizer( sizer );
    sizer->Layout();
    m_frame->Layout();
    m_frame->Show();
    m_log->Bind( wxEVT_CLOSE_WINDOW, &DrawingView::OnCloseLogWindow, this );
    Bind( wxEVT_SET_TABLE_PROPERTY, &DrawingView::OnSetProperties, this );
#if defined __WXMSW__ || defined __WXGTK__
    CreateViewToolBar();
#endif
    return true;
}

DrawingView::~DrawingView()
{
}

#if defined __WXMSW__ || defined __WXGTK__
void DrawingView::CreateViewToolBar()
{
    int offset;
    long style = wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT;
    wxMDIParentFrame *parent = m_frame->GetMDIParent();
    wxSize size = parent->GetClientSize();
    if( !m_tb )
        m_tb = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, "ViewBar" );
	else
		m_tb->ClearTools();
    if( m_type == DatabaseView )
    {
        m_tb->AddTool( wxID_DATABASEWINDOW, _( "Database Profile" ), wxBitmap( database_profile ), wxBitmap( database_profile ), wxITEM_NORMAL, _( "DB Profile" ), _( "Select database profile" ) );
        m_tb->AddTool( wxID_SELECTTABLE, _( "Select Table" ), wxBitmap( table ), wxBitmap( table ), wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" ) );
        m_tb->AddTool( wxID_PROPERTIES, _( "Properties" ), wxBitmap( properties ), wxBitmap( properties ), wxITEM_NORMAL, _( "Properties" ), _( "Proerties" ) );
    }
    else
    {
    }
    m_tb->Realize();
    wxMDIClientWindow *frame = (wxMDIClientWindow *) parent->GetClientWindow();
    m_tb->SetSize( 0, 0, size.x, wxDefaultCoord );
    offset = m_tb->GetSize().y;
    frame->SetSize( 0, offset, size.x, size.y - offset );
    m_frame->SetSize( 0, 0, size.x, size.y - offset - 2 );
}
#endif

// Sneakily gets used for default print/preview as well as drawing on the
// screen.
void DrawingView::OnDraw(wxDC *WXUNUSED(dc))
{
}

void DrawingView::OnSetProperties(wxCommandEvent &event)
{
    std::vector<std::wstring> errors;
    DatabaseTable *table;
    int res = -1;
    ShapeList shapes;
    bool found = false;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( wxSFRectShape ), shapes );
    wxString tableName, schemaName;
    MyErdTable *erdTable = NULL;
    FieldShape *field = NULL;
    int isLogOnly = event.GetInt();
    long type = event.GetExtraLong();
    wxString *command = (wxString *) event.GetClientData();
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end() && !found; ++it )
    {
        if( (*it)->IsSelected() )
        {
            if( type == 0 )
            {
                erdTable = (MyErdTable *)(*it);
                found = true;
            }
            if( type == 1 )
            {
                MyErdTable *temp = (MyErdTable *)(*it);
                if( temp )
                {
                    erdTable = temp;
                    continue;
                }
                field = (FieldShape *)(*it);
                found = true;
            }
        }
    }
    if( isLogOnly )
    {
        m_text->AppendText( *command );
        m_text->AppendText( "\n\r\n\r" );
        if( !m_log->IsShown() )
            m_log->Show();
    }
    else
    {
        if( type == 0 )
            res = GetDocument()->GetDatabase()->SetTableProperties( command->ToStdWstring(), errors );
        if( type == 1 )
            res = GetDocument()->GetDatabase()->SetFieldProperties( command->ToStdWstring(), errors );
        if( res )
        {
            for( std::vector<std::wstring>::iterator it = errors.begin(); it < errors.end(); it++ )
                wxMessageBox( (*it) );
        }
        else
        {
            if( type == 0 )
            {
                table = const_cast<DatabaseTable *>( &((MyErdTable *) erdTable)->GetTable() );
                GetDocument()->GetDatabase()->GetTableProperties( table, errors );
                erdTable->SetTableComment( table->GetComment() );
                erdTable->UpdateTable();
            }
        }
    }
}

void DrawingView::OnCloseLogWindow(wxCloseEvent &WXUNUSED(event))
{
    m_log->Hide();
}

void DrawingView::GetTablesForView(Database *db, bool init)
{
    int res;
    wxString query;
    std::vector<wxString> tables;
    wxDynamicLibrary lib;
#ifdef __WXMSW__
    lib.Load( "dialogs" );
#elif __WXMAC__
    lib.Load( "liblibdialogs.dylib" );
#else
    lib.Load( "libdialogs" );
#endif
    if( lib.IsLoaded() )
    {
        if( m_type == QueryView )
        {
            if( init )
            {
                CHOOSEOBJECT func = (CHOOSEOBJECT) lib.GetSymbol( "ChooseObject" );
                res = func( m_frame, 1 );
                if( res == wxID_CANCEL )
                {
                    m_frame->Close();
                    return;
                }
                m_fields->Show( true );
                m_queryBook->Show( true );
                m_frame->Layout();
                sizer->Layout();
#ifdef __WXGTK__
                wxDocMDIParentFrame *parent = wxDynamicCast( m_frame->GetMDIParent(), wxDocMDIParentFrame );
                wxSize size = parent->GetSize();
                parent->SetSize( size.GetWidth() - 5, size.GetHeight() - 5 );
                parent->SetSize( size.GetWidth() + 5, size.GetHeight() + 5 );
#endif
            }
        }
        TABLESELECTION func = (TABLESELECTION) lib.GetSymbol( "SelectTablesForView" );
        int res = func( m_frame, db, tables, GetDocument()->GetTableNames(), false );
        if( res != wxID_CANCEL )
        {
            if( m_type == QueryView )
            {
                std::vector<std::wstring> queryFields = GetDocument()->GetQueryFields();
                query = "SELECT ";
                if( queryFields.size() == 0 )
                    query += "<unknown fields>\n";
                else
                {
                    for( std::vector<std::wstring>::iterator it = queryFields.begin(); it < queryFields.end(); it++ )
                    {
                        query += (*it);
                        if( it != queryFields.end() - 1 )
                            query += ",";
                    }
                    query += "\n";
                }
                query += "FROM ";
            }
            ((DrawingDocument *) GetDocument())->AddTables( tables );
            ((DatabaseCanvas *) m_canvas)->DisplayTables( tables, query );
            if( m_type == QueryView )
                m_page6->SetSyntaxText(query);
        }
    }
//    return tables;
}

DrawingDocument* DrawingView::GetDocument()
{
    return wxStaticCast( wxView::GetDocument(), DrawingDocument );
}

void DrawingView::OnUpdate(wxView* sender, wxObject* hint)
{
    wxView::OnUpdate( sender, hint );
    if( m_canvas )
        m_canvas->Refresh();
}

// Clean up windows used for displaying the view.
bool DrawingView::OnClose(bool deleteWindow)
{
    m_isActive = false;
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

void DrawingView::OnNewIndex(wxCommandEvent &WXUNUSED(event))
{
    int result;
    wxString command;
    std::vector<std::wstring> errors;
    DatabaseTable *table = NULL;
    ShapeList shapes;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( MyErdTable ), shapes );
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end(); ++it )
    {
        if( (*it)->IsSelected() )
            table = const_cast<DatabaseTable *>( &((MyErdTable *) *it)->GetTable() );
    }
    wxDynamicLibrary lib;
#ifdef __WXMSW__
    lib.Load( "dialogs" );
#elif __WXMAC__
    lib.Load( "liblibdialogs.dylib" );
#else
    lib.Load( "libdialogs" );
#endif
    if( lib.IsLoaded() )
    {
        CREATEINDEX func = (CREATEINDEX) lib.GetSymbol( "CreateIndexForDatabase" );
        result = func( m_frame, table, GetDocument()->GetDatabase(), command );
        if( result != wxID_OK && result != wxID_CANCEL )
        {
            m_text->AppendText( command );
            m_text->AppendText( "\n\r\n\r" );
            if( !m_log->IsShown() )
                m_log->Show();
        }
        else if( result == wxID_OK )
        {
            dynamic_cast<DrawingDocument *>( GetDocument() )->GetDatabase()->CreateIndex( command.ToStdWstring(), errors );
            for( std::vector<std::wstring>::iterator it = errors.begin(); it < errors.end(); it++ )
                wxMessageBox( (*it) );
        }
    }
    else
        wxMessageBox( _( "Error loading the DLL/so" ) );
}

void DrawingView::OnForeignKey(wxCommandEvent &WXUNUSED(event))
{
    std::vector<std::wstring> errors;
    int result;
    DatabaseTable *table = NULL;
    ShapeList shapes;
    wxString command;
    bool logOnly = false;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( MyErdTable ), shapes );
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end(); ++it )
    {
        if( (*it)->IsSelected() )
            table = const_cast<DatabaseTable *>( &((MyErdTable *) *it)->GetTable() );
    }
    wxDynamicLibrary lib;
#ifdef __WXMSW__
    lib.Load( "dialogs" );
#elif __WXMAC__
    lib.Load( "liblibdialogs.dylib" );
#else
    lib.Load( "libdialogs" );
#endif
    if( lib.IsLoaded() )
    {
        CREATEFOREIGNKEY func = (CREATEFOREIGNKEY) lib.GetSymbol( "CreateForeignKey" );
        result = func( m_frame, table, GetDocument()->GetDatabase(), command, logOnly );
        if( logOnly )
        {
            m_text->AppendText( command );
            m_text->AppendText( "\n\r\n\r" );
            if( !m_log->IsShown() )
                m_log->Show();
        }
        else
        {
            if( result != wxID_CANCEL )
                GetDocument()->GetDatabase()->ApplyForeignKey( command.ToStdWstring(), *table, errors );
        }
    }
    else
        wxMessageBox( _( "Error loading the DLL/so" ) );
}

void DrawingView::OnViewSelectedTables(wxCommandEvent &WXUNUSED(event))
{
    GetTablesForView( GetDocument()->GetDatabase(), false );
}

void DrawingView::OnFieldProperties(wxCommandEvent &event)
{
    std::vector<std::wstring> errors;
    bool found = false;
    int type = 0;
    DatabaseTable *table = NULL;
    Field *field = NULL;
    ShapeList shapes;
    wxString command = "";
    bool logOnly = false;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( wxSFRectShape ), shapes );
    wxString tableName, schemaName;
    MyErdTable *erdTable = NULL;
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end() && !found; ++it )
    {
        if( event.GetId() == wxID_PROPERTIES )
        {
            if( (*it)->IsSelected() )
            {
                erdTable = (MyErdTable *)(*it);
                table = const_cast<DatabaseTable *>( &((MyErdTable *) *it)->GetTable() );
                type = 0;
                found = true;
            }
        }
        if( event.GetId() == wxID_FIELDPROPERTIES )
        {
            field = ((FieldShape *) event.GetEventObject())->GetField();
            if( (*it)->IsSelected() )
            {
                MyErdTable *table = dynamic_cast<MyErdTable *>( *it );
                if( table )
                {
                    erdTable = table;
                    tableName = const_cast<DatabaseTable *>( &erdTable->GetTable() )->GetTableName();
                    schemaName = const_cast<DatabaseTable *>( &erdTable->GetTable() )->GetSchemaName();
                    type = 1;
                    found = true;
                }
            }
        }
    }
    wxDynamicLibrary lib;
#ifdef __WXMSW__
    lib.Load( "dialogs" );
#elif __WXMAC__
    lib.Load( "liblibdialogs.dylib" );
#else
    lib.Load( "libdialogs" );
#endif
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
            m_text->AppendText( command );
            m_text->AppendText( "\n\r\n\r" );
            if( !m_log->IsShown() )
                m_log->Show();
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

void DrawingView::OnLogUpdateUI(wxUpdateUIEvent &event)
{
    if( m_log->IsShown() )
    {
        if( event.GetId() == wxID_STARTLOG )
            event.Enable( false );
        if( event.GetId() == wxID_STOPLOG )
            event.Enable( true );
        if( event.GetId() == wxID_SAVELOG )
            event.Enable( true );
        if( event.GetId() == wxID_CLEARLOG )
            event.Enable( true );
    }
    else
    {
        if( event.GetId() == wxID_STARTLOG )
            event.Enable( true );
        if( event.GetId() == wxID_STOPLOG )
            event.Enable( false );
        if( event.GetId() == wxID_SAVELOG )
            event.Enable( false );
        if( event.GetId() == wxID_CLEARLOG )
            event.Enable( false );
    }
}

void DrawingView::OnStartLog(wxCommandEvent &WXUNUSED(event))
{
    m_log->Show();
}

void DrawingView::OnStopLog(wxCommandEvent &WXUNUSED(event))
{
    m_log->Hide();
}

void DrawingView::OnSaveLog(wxCommandEvent &WXUNUSED(event))
{
    wxFileDialog dlg( m_frame, _( "Save Log As..." ), ::wxGetCwd(), "", _( "SQL script (*.sql)|*.sql" ), wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
    if( dlg.ShowModal() != wxID_CANCEL )
        m_text->SaveFile( dlg.GetPath() );
}

void DrawingView::OnClearLog(wxCommandEvent &WXUNUSED(event))
{
    m_text->Clear();
}

void DrawingView::SetViewType(ViewType type)
{
	m_type = type;
}

ViewType DrawingView::GetViewType()
{
	return m_type;
}

#if defined __WXMSW__ || defined __WXGTK__
void DrawingView::OnActivateView(bool activate, wxView *activeView, wxView *deactiveView)
{
    wxDocMDIParentFrame *frame = (wxDocMDIParentFrame *) m_frame->GetMDIParent();
    wxSize clientSize = frame->GetClientSize();
    if( activate )
        m_isActive = true;
    if( !activate && !m_isActive )
    {
/*        CreateViewToolBar();
        if( m_isCreated )
            return;
        wxDocMDIParentFrame *parent = wxStaticCast( wxTheApp->GetTopWindow(), wxDocMDIParentFrame );
        wxWindowList children = parent->GetChildren();
        bool found = false;
        for( wxWindowList::iterator it = children.begin(); it != children.end() && !found; it++ )
        {
            m_tb = wxDynamicCast( *it, wxToolBar );
            if( m_tb && m_tb->GetName() == "Second Toolbar" )
                found = true;
        }
        m_tb->Show();
        wxMenuBar *bar = parent->GetMenuBar();
        wxMenu *file_menu = bar->GetMenu( 0 );
        if( file_menu->FindItem( wxID_NEW ) )
            file_menu->Delete( wxID_NEW );
        if( file_menu->FindItem( wxID_OPEN ) )
            file_menu->Delete( wxID_OPEN );
        file_menu->Insert( 0, wxID_CLOSE, _( "&Close\tCtrl+W" ), _( "Close Database Window" ) );
        file_menu->Insert( 2, wxID_CREATEDATABASE, _( "Create Database..." ), _( "Create Database" ) );
        file_menu->Insert( 3, wxID_DELETEDATABASE, _( "Delete Database..." ), _( "Delete Database" ) );
        file_menu->InsertSeparator( 4 );
        wxMenu *menuObject = new wxMenu();
        menuObject->Append( wxID_SELECTTABLE, _( "Select Table..." ), _( "Select tables" ) );
        wxMenu *menuNewObject = new wxMenu();
        menuNewObject->Append( wxID_OBJECTNEWTABLE, _( "Table..." ), _( "New Table" ) );
        menuNewObject->Append( wxID_OBJECTNEWINDEX, _( "Index..." ), _( "New Index" ) );
        menuNewObject->Append( wxID_OBJECTNEWVIEW, _( "View" ), _( "New View" ) );
        menuNewObject->Append( wxID_OBJECTNEWFF, _( "Foreign Key..." ), _( "New Foreign Key" ) );
        menuObject->AppendSubMenu( menuNewObject, _( "New" ), _( "New Object" ) );
        menuObject->Append( wxID_TABLEDROPTABLE, _( "Drop" ), _( "Drop database object" ) );
        menuObject->AppendSeparator();
        menuObject->Append( wxID_PROPERTIES, _( "Properties..." ), _( "Properties" ) );
        bar->Insert( 1, menuObject, _( "&Object" ) );
        wxMenu *menuDesign = new wxMenu();
        menuDesign->Append( wxID_STARTLOG, _( "Start Log" ), _( "Start log" ) );
        menuDesign->Append( wxID_STOPLOG, _( "Stop Log" ), _( "Stop log" ) );
        menuDesign->Append( wxID_SAVELOG, _( "Save Log As..." ), _( "Save log to disk file" ) );
        menuDesign->Append( wxID_CLEARLOG, _( "Clear Log" ), _( "Discard content of the log" ) );
        menuDesign->AppendSeparator();
        bar->Insert( 2, menuDesign, _( "&Design" ) );
        parent->SetMenuBar( bar );
#if defined __WXMSW__ || defined __WXGTK__
        m_tb->ClearTools();
        m_tb->AddTool( wxID_DATABASEWINDOW, _( "Database Profile" ), wxBitmap( database_profile ), wxBitmap( database_profile ), wxITEM_NORMAL, _( "DB Profile" ), _( "Select database profile" ) );
        m_tb->AddTool( wxID_OBJECTNEWFF, _( "Foreign Key" ), wxBitmap( key_f1 ), wxBitmap( key_f1 ), wxITEM_NORMAL, _( "Create Foreign Key" ), _( "Create Foreign Key" ) );
        m_tb->AddTool( wxID_SELECTTABLE, _( "Select Table" ), wxBitmap( table ), wxBitmap( table ), wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" ) );
        m_tb->AddTool( wxID_PROPERTIES, _( "Properties" ), wxBitmap( properties ), wxBitmap( properties ), wxITEM_NORMAL, _( "Properties" ), _( "Proerties" ) );
        m_tb->Realize();
#endif
        if( !m_isCreated )
        {
            m_isCreated = true;
            return;
        }
    }
    else
    {*/
        if( !deactiveView && m_tb )
        {
            m_tb->Destroy();
            m_tb = NULL;
            frame->GetParent()->SetSize( 0, 0, clientSize.x, clientSize.y ); 
        }
        else
        {
            m_tb->ClearTools();
            m_tb->Hide();
        }
    }
}
#endif

void DrawingView::OnAlterTable(wxCommandEvent &WXUNUSED(event))
{
    wxDocMDIParentFrame *parent = wxStaticCast( wxTheApp->GetTopWindow(), wxDocMDIParentFrame );
    ShapeList shapes;
    ShapeList::iterator it;
    bool found = false;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( wxSFRectShape ), shapes );
    for( it = shapes.begin(); it != shapes.end() && !found; ++it )
    {
        if( (*it)->IsSelected() )
            found = true;
    }
    wxDynamicLibrary lib1;
#ifdef __WXMSW__
    lib1.Load( "tablewindow" );
#elif __WXOSX__
    lib1.Load( "liblibtablewindow.dylib" );
#else
    lib1.Load( "libtablewindow" );
#endif
    if( lib1.IsLoaded() )
    {
        TABLE func = (TABLE) lib1.GetSymbol( "CreateDatabaseWindow" );
        MyErdTable *erdTable = dynamic_cast<MyErdTable *>( (*it) );
        func( parent, GetDocumentManager(), dynamic_cast<DrawingDocument *>( GetDocument() )->GetDatabase(), const_cast<DatabaseTable *>( &( erdTable->GetTable() ) ), wxEmptyString );                 // create with possible alteration table
    }
    else if( !lib1.IsLoaded() )
        wxMessageBox( "Error loading the library. Please re-install the software and try again." );
    else
        wxMessageBox( "Error connecting to the database. Please check the database is accessible and you can get a good connection, then try again." );
}

void DrawingView::OnFieldDefinition(wxCommandEvent &WXUNUSED(event))
{
    wxDocMDIParentFrame *parent = wxStaticCast( wxTheApp->GetTopWindow(), wxDocMDIParentFrame );
    ShapeList shapes;
    MyErdTable *table;
    FieldShape *field;
    ShapeList::iterator it;
    bool found = false;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( wxSFRectShape ), shapes );
    for( it = shapes.begin(); it != shapes.end() && !found; ++it )
    {
        if( (*it)->IsSelected() )
        {
            if( wxDynamicCast( (*it), MyErdTable ) )
                table = wxDynamicCast( (*it), MyErdTable );
            if( wxDynamicCast( (*it), FieldShape ) )
                field = wxDynamicCast( (*it), FieldShape );
        }
    }
    wxDynamicLibrary lib1;
#ifdef __WXMSW__
    lib1.Load( "tablewindow" );
#elif __WXOSX__
    lib1.Load( "liblibtablewindow.dylib" );
#else
    lib1.Load( "libtablewindow" );
#endif
    if( lib1.IsLoaded() )
    {
        TABLE func = (TABLE) lib1.GetSymbol( "CreateDatabaseWindow" );
        MyErdTable *erdTable = dynamic_cast<MyErdTable *>( (*it) );
        func( parent, GetDocumentManager(), dynamic_cast<DrawingDocument *>( GetDocument() )->GetDatabase(), const_cast<DatabaseTable *>( &( erdTable->GetTable() ) ), field->GetField()->GetFieldName() );                 // display field parameters
    }
    else if( !lib1.IsLoaded() )
        wxMessageBox( "Error loading the library. Please re-install the software and try again." );
    else
        wxMessageBox( "Error connecting to the database. Please check the database is accessible and you can get a good connection, then try again." );
}

WhereHavingPage *DrawingView::GetWherePage()
{
    return m_page2;
}

WhereHavingPage *DrawingView::GetHavingPage()
{
    return m_page4;
}

void DrawingView::OnCreateDatabase(wxCommandEvent &WXUNUSED(event))
{
    Database *db = NULL, *db1 = GetDocument()->GetDatabase();
    wxDynamicLibrary *lib = new wxDynamicLibrary();
#ifdef __WXMSW__
    lib->Load( "dbloader" );
#elif __WXMAC__
    lib->Load( "liblibdbloader.dylib" );
#else
    lib->Load( "libdbloader" );
#endif
    if( lib->IsLoaded() )
    {
        DBPROFILE func = (DBPROFILE) lib->GetSymbol( "ConnectToDb" );
        wxString name = wxEmptyString;
        wxString engine = GetDocument()->GetDatabase()->GetTableVector().m_type;
        db = func( m_frame->GetParent(), name, engine );
        if( db )
        {
            delete db1;
            db1 = NULL;
//            wxGetApp().SetDBEngine( engine );
//            wxGetApp().SetDBName( name );
        }
        db1 = db;
    }
    delete lib;
}

void DrawingView::AddFieldToQuery(const FieldShape &field, bool isAdding, const std::wstring &tableName)
{
    Field *fld = const_cast<FieldShape &>( field ).GetField();
    wxString name = tableName + "." + fld->GetFieldName();
    name = "\"" + name;
    name = name + "\"";
    wxString query = m_page6->GetSyntaxCtrl()->GetValue();
    if( isAdding )
    {
        m_fields->AddField( name );
        GetDocument()->AddRemoveField( name.ToStdWstring(), true );
        std::vector<std::wstring> queryFields = GetDocument()->GetQueryFields();
//        queryFields.push_back( name.ToStdWstring() );
        if( queryFields.size() == 1 )
            query.Replace( "<unknown fields>", name + " " );
        else
        {
            wxString temp = query.substr( query.Find( ' ' ) );
            temp = temp.substr( 0, temp.Find( "FROM" ) - 1 );
            query.Replace( temp, temp + ", " + name + " " );
        }
        m_page6->SetSyntaxText( query );
    }
    else
    {
        GetDocument()->AddRemoveField( name.ToStdWstring(), false );
        std::vector<std::wstring> queryFields = GetDocument()->GetQueryFields();
        m_fields->RemoveField( queryFields );
        if( queryFields.size() == 0 )
        {
			query.Replace( name, "<unknown fields>" );
        }
		else
		{
            wxString str = ",";
            str += " ";
            str += name;
            wxString temp = query.substr( 0, query.Find( str ) );
            wxString temp1 = query.substr( query.Find( name ) + name.length() );
            if( temp == query )
            {
                temp = "SELECT ";
                temp1 = temp1.substr( 2 );
            }
            query = temp + temp1;
		}
        m_page6->SetSyntaxText( query );
    }
}

void DrawingView::OnSQLNotebookPageChanged(wxBookCtrlEvent &event)
{
    wxPanel *panel = dynamic_cast<wxPanel *>( m_queryBook->GetPage( event.GetSelection() ) );
    WhereHavingPage *page = dynamic_cast<WhereHavingPage *>( panel );
    if( page )
        page->OnSelection();
    panel->SetFocus();
}

void DrawingView::OnSelectAllFields(wxCommandEvent &event)
{
    int id = event.GetId();
    MyErdTable *shape = dynamic_cast<MyErdTable *>( event.GetEventObject() );
    AddDeleteFields( shape, id == wxID_DESELECTALLFIELDS ? false : true, const_cast<DatabaseTable &>( shape->GetTable() ).GetTableName() );
}

void DrawingView::AddDeleteFields(MyErdTable *field, bool isAdd, const std::wstring &tableName)
{
    SerializableList children;
    if( field )
    {
        field->GetChildrenRecursively( CLASSINFO( FieldShape ), children, xsSerializable::searchDFS );
        SerializableList::compatibility_iterator node = children.GetFirst();
        while( node )
        {
            FieldShape *field = (FieldShape *) node->GetData();
            if( field && isAdd ? !field->IsSelected() : field->IsSelected() )
            {
                field->Select( isAdd );
                AddFieldToQuery( *field, isAdd, tableName );
            }
            node = node->GetNext();
        }
        m_canvas->Refresh();
    }
}
/*
void DrawingView::OnActivateView(bool activate, wxView *activeView, wxView *deactiveView)
{
    if( activate )
    {
        CreateViewToolBar();
    }
	else
    {
    }
}
*/
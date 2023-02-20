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
#include "../dbhandler/res/properties.xpm"
#include "res/gui/key-f1.xpm"
#include "../dbhandler/res/quit.xpm"
#include "res/gui/toolbox.xpm"
//#include "./res/gui/bold_png.c"
//#endif

#ifdef __WXGTK__
#include "bold.h"
#include "italic.h"
#include "underline.h"
#include "table_svg.h"
#endif
#include "res/gui/preview.c"
#include "res/gui/sql.h"

#include <memory>
#include <string>
#if _MSC_VER >= 1900 || !(defined __WXMSW__)
#include <mutex>
#endif

#include "wx/file.h"

#include "wx/docview.h"
#if defined __WXGTK__
#include "wx/nativewin.h"
#elif defined __WXOSX__
#include "wx/fontpicker.h"
#endif
#include "wx/artprov.h"
#include "wx/fontenum.h"
#include "wx/notebook.h"
#include "wx/fdrepdlg.h"
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/cmdproc.h"
#include "wx/bmpcbox.h"
#include "wx/grid.h"
#include "wx/stc/stc.h"
#include "wx/listctrl.h"
#include "wx/dataview.h"
#include "wx/renderer.h"
#include "wx/config.h"
#include "wx/filepicker.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "objectproperties.h"
#include "colorcombobox.h"
#include "designlabel.h"
#include "constraint.h"
#include "constraintsign.h"
#include "table.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "FieldShape.h"
#include "nametableshape.h"
#include "commenttableshape.h"
#include "fontcombobox.h"
#include "MyErdTable.h"
#include "field.h"
#include "fieldwindow.h"
#include "syntaxproppage.h"
#include "sortgroupbypage.h"
#include "wherehavingpage.h"
#include "databasedoc.h"
#include "databasecanvas.h"
#include "designcanvas.h"
#include "databaseview.h"
#include "divider.h"
#include "propertypagebase.h"
#include "printspec.h"
#include "bandgeneral.h"
#include "tablegeneral.h"
#include "pointerproperty.h"
#include "tableprimarykey.h"
#include "fieldgeneral.h"
#include "designgeneral.h"
#include "fontpropertypagebase.h"
#include "fieldheader.h"
#include "propertieshandlerbase.h"
#include "propertieshandler.h"
#include "fieldpropertieshandler.h"
#include "fieldpropertieshandler.h"
#include "designpropertieshandler.h"
#include "dividerpropertieshandler.h"

const wxEventTypeTag<wxCommandEvent> wxEVT_SET_TABLE_PROPERTY( wxEVT_USER_FIRST + 1 );
const wxEventTypeTag<wxCommandEvent> wxEVT_SET_FIELD_PROPERTY( wxEVT_USER_FIRST + 2 );
const wxEventTypeTag<wxCommandEvent> wxEVT_CHANGE_QUERY( wxEVT_USER_FIRST + 3 );
const wxEventTypeTag<wxCommandEvent> wxEVT_FIELD_SHUFFLED( wxEVT_USER_FIRST + 4 );

typedef int (*TABLESELECTION)(wxDocMDIChildFrame *, Database *, std::map<wxString, std::vector<TableDefinition> > &, std::vector<std::wstring> &, bool, const int);
typedef int (*CREATEINDEX)(wxWindow *, DatabaseTable *, Database *, wxString &, wxString &);
typedef int (*CREATEPROPERTIESDIALOG)(wxWindow *parent, std::unique_ptr<PropertiesHandler> &, const wxString &, wxString &, bool, wxCriticalSection &);
typedef int (*CREATEFOREIGNKEY)(wxWindow *parent, wxString &, DatabaseTable *, std::vector<std::wstring> &, std::vector<std::wstring> &, std::wstring &, int &, int &, Database *, bool &, bool, std::vector<FKField *> &, int &);
typedef void (*TABLE)(wxWindow *, wxDocManager *, Database *, DatabaseTable *, const wxString &);
typedef int (*CHOOSEOBJECT)(wxWindow *, int, std::vector<QueryInfo> &, wxString &, std::vector<LibrariesInfo> &path, bool &update);
typedef int (*NEWQUERY)(wxWindow *, int &, int &);
typedef int (*QUICKSELECT)(wxWindow *, const Database *, std::vector<DatabaseTable *> &, const std::vector<TableField *> &, std::vector<FieldSorter> &allSorted, std::vector<FieldSorter> &qerySorted);
typedef Database *(*DBPROFILE)(wxWindow *, const wxString &, wxString &, const std::wstring &);
typedef int (*RETRIEVEARGUMENTS)(wxWindow *, std::vector<QueryArguments> &arguments, const wxString &, const wxString &);
typedef int (*GOTOLINE)(wxWindow *, int &);
typedef void (*DATAEDITWINDOW)(wxWindow *parent, wxDocManager *docManager, Database *db, const wxString &);
typedef int (*GETDATASOURCE)(wxWindow *parent, wxString &sorce, const std::vector<Profile> &);

#if _MSC_VER >= 1900 || !(defined __WXMSW__)
std::mutex Database::Impl::my_mutex;
#endif

#define wxID_NEWOBJECT 1000

// ----------------------------------------------------------------------------
// DrawingView implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(DrawingView, wxView);

wxBEGIN_EVENT_TABLE(DrawingView, wxView)
    EVT_MENU(wxID_SELECTTABLE, DrawingView::OnViewSelectedTables)
    EVT_MENU(wxID_OBJECTNEWINDEX, DrawingView::OnNewIndex)
    EVT_MENU(wxID_PROPERTIES, DrawingView::OnSetProperties)
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
    EVT_MENU(wxID_DISTINCT, DrawingView::OnDistinct)
    EVT_MENU(wxID_RETRIEVEARGS, DrawingView::OnRetrievalArguments)
    EVT_MENU(wxID_DATASOURCE, DrawingView::OnDataSource)
    EVT_MENU(wxID_DESIGNTABORDER, DrawingView::OnTabOrder)
    EVT_UPDATE_UI(wxID_PREVIEDWQUERY, DrawingView::OnQueryPreviewUpdateUI)
    EVT_MENU(wxID_SHOWSQLTOOLBOX, DrawingView::OnShowSQLBox)
    EVT_MENU(wxID_SHOWDATATYPES, DrawingView::OnShowDataTypes)
    EVT_MENU(wxID_SHOWCOMMENTS, DrawingView::OnShowComments)
    EVT_UPDATE_UI(wxID_CONVERTTOSYNTAX, DrawingView::OnConvertToSyntaxUpdateUI)
    EVT_MENU(wxID_CONVERTTOSYNTAX, DrawingView::OnConvertToSyntax)
    EVT_MENU(wxID_UNDO, DrawingView::OnUndo)
    EVT_MENU(wxID_CUT, DrawingView::OnCut)
    EVT_MENU(wxID_COPY, DrawingView::OnCopy)
    EVT_MENU(wxID_PASTE, DrawingView::OnPaste)
    EVT_MENU(wxID_CLEAR, DrawingView::OnClear)
    EVT_MENU(wxID_SELECTALL, DrawingView::OnSelectAll)
    EVT_MENU(wxID_FIND, DrawingView::OnFind)
    EVT_FIND(wxID_ANY, DrawingView::OnFindReplaceText)
    EVT_FIND_NEXT(wxID_ANY, DrawingView::OnFindReplaceText)
    EVT_MENU(wxID_REPLACE, DrawingView::OnFind)
    EVT_MENU(myID_FINDNEXT, DrawingView::OnFindNext)
    EVT_FIND_REPLACE(wxID_ANY, DrawingView::OnFindReplaceText)
    EVT_FIND_REPLACE_ALL(wxID_ANY, DrawingView::OnFindReplaceText)
    EVT_MENU(wxID_GOTOLINE, DrawingView::OnGotoLine)
    EVT_MENU(wxID_CONVERTTOGRAPHICS, DrawingView::OnConvertToGraphics)
    EVT_MENU(wxID_TABLEEDITDATA, DrawingView::OnTableDataEdit)
    EVT_MENU(wxID_EXPORTSYNTAX, DrawingView::OnExportSyntax)
    EVT_MENU(wxID_SAVEQUERY, DrawingView::OnQuerySave)
    EVT_UPDATE_UI(wxID_SAVEQUERY, DrawingView::OnQuerySaveUpdateUI)
    EVT_UPDATE_UI(wxID_SAVEQUERYAS, DrawingView::OnQuerySaveAsUpdateUI)
wxEND_EVENT_TABLE()

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool DrawingView::OnCreate(wxDocument *doc, long flags)
{
    m_searchDirection = 1;
    m_log = nullptr;
    m_searchPos = 0;
    m_isActive = false;
    m_tb = m_styleBar = nullptr;
    wxToolBar *tb = nullptr;
    m_isCreated = false;
    m_fields = nullptr;
    m_queryBook = nullptr;
    m_page2 = m_page4 = nullptr;
    m_page1 = m_page3 = nullptr;
    m_page6 = nullptr;
    m_fieldText = nullptr;
    m_fontName = nullptr;
    m_fontSize = nullptr;
    if( !wxView::OnCreate( doc, flags ) )
        return false;
    wxConfigBase *config = wxConfigBase::Get( false );
    wxString path = config->GetPath();
    config->SetPath( "Query" );
    m_source = config->Read( "QuerySource", &m_source, 2 );
    m_presentation = config->Read( "QueryPresentation", &m_presentation, 4 );
    config->SetPath( path );
    m_parent = wxStaticCast( wxTheApp->GetTopWindow(), wxDocMDIParentFrame );
    wxRect clientRect = m_parent->GetClientRect();
    wxWindowList children = m_parent->GetChildren();
    for( wxWindowList::iterator it = children.begin(); it != children.end(); it++ )
    {
        tb = wxDynamicCast( *it, wxToolBar );
        if( tb && tb->GetName() == "ViewBar" )
        {
            m_tb = tb;
        }
        if( tb && tb->GetName() == "StyleBar" )
            m_styleBar = tb;
    }
    wxString title;
    if( m_type == QueryView )
    {
        title = "Query - (untitled)";
    }
    else
    {
        title = "Database - " + wxDynamicCast( GetDocument(), DrawingDocument )->GetDatabase()->GetTableVector().m_dbName;
    }
    m_frame = new wxDocMDIChildFrame( doc, this, m_parent, wxID_ANY, title, wxDefaultPosition, wxSize( clientRect.GetWidth(), clientRect.GetHeight() ) );
//    m_frame->SetMenuBar( parent->GetMenuBar() );
    if( m_type == DatabaseView )
    {
        m_log = new wxFrame( m_frame, wxID_ANY, _( "Activity Log" ), wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT );
        m_text = new wxTextCtrl( m_log, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY );
    }
    sizer = new wxBoxSizer( wxVERTICAL );
    CreateViewToolBar();
    wxPoint ptCanvas;
#ifndef __WXOSX__
    ptCanvas = wxDefaultPosition;
#else
    ptCanvas.x = 0;
    ptCanvas.y = m_tb->GetSize().y;
    if( m_styleBar )
        ptCanvas.y += m_styleBar->GetSize().y;
    ptCanvas.y = m_frame->GetSize().y - m_frame->GetClientSize().y;
#endif
    wxASSERT( m_frame == GetFrame() );
    if( m_type == QueryView )
    {
        m_fields = new FieldWindow( m_frame, 1, wxDefaultPosition, wxDefaultCoord );
        m_fields->SetCursor( wxCURSOR_HAND );
        sizer->Add( m_fields, 0, wxEXPAND, 0 );
        m_fields->Show( false );
    }
    auto db = ((DrawingDocument *) GetDocument() )->GetDatabase();
    m_canvas = new DatabaseCanvas( this, ptCanvas, db->GetTableVector().m_dbName, db->GetTableVector().m_type );
    sizer->Add( m_canvas, 2, wxEXPAND, 0 );
    if( m_type == QueryView )
    {
        m_queryBook = new wxNotebook( m_frame, wxID_ANY );
        m_page1 = new SortGroupByPage( m_queryBook, true );
        m_queryBook->AddPage( m_page1, _( "Sort" ) );
        m_page2 = new WhereHavingPage( m_queryBook, GetDocument()->GetDatabase()->GetTableVector().GetDatabaseType(), GetDocument()->GetDatabase()->GetTableVector().GetDatabaseSubtype(), true );
        m_queryBook->AddPage( m_page2, _( "Where" ) );
        m_page3 = new SortGroupByPage( m_queryBook, false );
        m_queryBook->AddPage( m_page3, _( "Group" ) );
        m_page4 = new WhereHavingPage( m_queryBook, GetDocument()->GetDatabase()->GetTableVector().GetDatabaseType(), GetDocument()->GetDatabase()->GetTableVector().GetDatabaseSubtype(), false );
        m_queryBook->AddPage( m_page4, _( "Having" ) );
        m_page6 = new SyntaxPropPage( m_queryBook );
        m_queryBook->AddPage( m_page6, _( "Syntax" ), true );
        sizer->Add( m_queryBook, 0, wxEXPAND, 0 );
        m_queryBook->Show( false );
        m_queryBook->Bind( wxEVT_NOTEBOOK_PAGE_CHANGED, &DrawingView::OnSQLNotebookPageChanged, this );
        m_designCanvas = new DesignCanvas( this, ptCanvas );
        sizer->Add( m_designCanvas, 1, wxEXPAND, 0 );
        m_canvas->Show( false );
        m_edit = new wxStyledTextCtrl( m_frame );
        std::wstring type = GetDocument()->GetDatabase()->GetTableVector().GetDatabaseType();
        std::wstring subtype = GetDocument()->GetDatabase()->GetTableVector().GetDatabaseSubtype();
        if( type == "MySQL" || ( type == "ODBC" && subtype == "MySQL" ) )
            m_edit->SetLexer( wxSTC_LEX_MYSQL );
        if( type == "Microsoft SQL Server" || ( type == "ODBC" && subtype == "Microsoft SQL Server" ) )
            m_edit->SetLexer( wxSTC_LEX_MSSQL );
        if( type == "SQLite" )
            m_edit->SetLexer( wxSTC_LEX_SQL );
        sizer->Add( m_edit, 1, wxEXPAND, 0 );
        m_edit->Show( false );
    }
    m_frame->SetSizer( sizer );
    if( m_log )
        m_log->Bind( wxEVT_CLOSE_WINDOW, &DrawingView::OnCloseLogWindow, this );
    Bind( wxEVT_SET_TABLE_PROPERTY, &DrawingView::OnSetProperties, this );
    Bind( wxEVT_SET_FIELD_PROPERTY, &DrawingView::OnSetProperties, this );
    Bind( wxEVT_MENU, &DatabaseCanvas::OnDropTable, m_canvas, wxID_DROPOBJECT );
    Bind( wxEVT_FIELD_SHUFFLED, &DrawingView::OnFieldShuffle, this );
    m_frame->Bind( wxEVT_CHANGE_QUERY, &DrawingView::OnQueryChange, this );
    m_frame->Bind( wxEVT_ICONIZE, &DrawingView::OnIconise, this );
    if( m_fieldText )
    {
        m_fieldText->Bind( wxEVT_UPDATE_UI, &DrawingView::FieldTextUpdateUI, this );
        m_fieldText->Disable();
    }
    if( m_fontName )
        m_fontName->Bind( wxEVT_COMBOBOX, &DrawingView::OnFontSeectionChange, this );
    sizer->Layout();
    m_frame->Layout();
    m_frame->Show();
#ifdef __WXGTK__
    Activate( true );
#endif
    return true;
}

DrawingView::~DrawingView()
{
    wxMenuBar *bar = m_parent->GetMenuBar();
    for( auto i = bar->GetMenuCount() - 2; i > 0; i-- )
    {
        auto menu = bar->Remove( i );
        delete menu;
    }
#ifndef __WXOSX__
    delete m_styleBar;
    m_styleBar = nullptr;
#endif
}

void DrawingView::CreateViewToolBar()
{
    int offset;
    long style = wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT;
    wxWindow *parent = nullptr;
#ifdef __WXMSW__
    parent = (wxMDIClientWindow *) m_parent->GetClientWindow();
#elif __WXGTK__
    parent = m_parent;
#elif __WXOSX__
    parent = m_frame;
#endif
    wxSize size = m_parent->GetClientSize();
#ifdef __WXSX__
    m_tb = m_frame->CreateToolBar();
    if( m_type == QueryView )
    {
        m_styleBar = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_TOP, "StyleBar" );
    }
#else
    if( !m_tb )
        m_tb = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, "ViewBar" );
    else
        m_tb->ClearTools();
    if( m_type == QueryView )
    {
        if( !m_styleBar )
            m_styleBar = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, "StyleBar" );
        else
            m_styleBar->ClearTools();
    }
#endif
    if( m_type == DatabaseView )
    {
        CreateDBMenu();
        m_tb->AddTool( wxID_DATABASEWINDOW, _( "Database Profile" ), wxBitmap( database_profile ), wxBitmap( database_profile ), wxITEM_NORMAL, _( "DB Profile" ), _( "Select database profile" ) );
        wxBitmapBundle tableSVG;
#ifdef __WXMSW__
        HANDLE gs_wxMainThread = NULL;
        const HINSTANCE inst = wxDynamicLibrary::MSWGetModuleHandle( "dbwindow", &gs_wxMainThread );
        const void* dataTable = nullptr;
        size_t sizeTable = 0;
        if( !wxLoadUserResource( &dataTable, &sizeTable, "table", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            tableSVG = wxBitmapBundle::FromSVG( (const char *) dataTable, wxSize( 16, 16 ) );
        }
#elif __WXOSX__
        tableSVG = wxBitmapBundle::FromSVGResource( "table", wxSize( 16, 16 ) );
#else
        tableSVG = wxBitmapBundle::FromSVG( table, wxSize( 16, 16 ) );
#endif
        m_tb->AddTool( wxID_SELECTTABLE, _( "Select Table" ), tableSVG, tableSVG, wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" ) );
        m_tb->AddTool( wxID_DROPOBJECT, _( "Drop" ), wxArtProvider::GetBitmapBundle( wxART_DELETE ), wxArtProvider::GetBitmapBundle( wxART_DELETE ), wxITEM_NORMAL, _( "Drop" ), _( "Drop database Object" ) );
        m_tb->AddTool( wxID_PROPERTIES, _( "Properties" ), wxBitmap( properties ), wxBitmap( properties ), wxITEM_NORMAL, _( "Properties" ), _( "Proerties" ) );
        m_tb->AddTool( wxID_CLOSE, _( "Close View" ), wxBitmap( quit_xpm ), wxBitmap( quit_xpm ), wxITEM_NORMAL, _( "Close" ), _( "Close Database View" ) );
    }
    else
    {
        CreateQueryMenu( QuerySyntaxMenu );
        m_tb->AddTool( wxID_NEW, _( "New" ), wxArtProvider::GetBitmapBundle( wxART_NEW, wxART_TOOLBAR ), wxArtProvider::GetBitmapBundle( wxART_NEW, wxART_TOOLBAR ), wxITEM_NORMAL, _( "New" ), _( "New Query" ) );
        m_tb->AddTool( wxID_OPEN, _( "Open" ), wxArtProvider::GetBitmapBundle( wxART_FILE_OPEN, wxART_TOOLBAR ), wxArtProvider::GetBitmapBundle( wxART_FILE_OPEN, wxART_TOOLBAR ), wxITEM_NORMAL, _( "Open" ), _( "Open Query" ) );
        wxBitmapBundle save, tableSVG, boldSVG, italicSVG, underlineSVG;
#ifdef __WXMSW__
        HANDLE gs_wxMainThread = NULL;
        const HINSTANCE inst = wxDynamicLibrary::MSWGetModuleHandle( "dbwindow", &gs_wxMainThread );
        const void* data = nullptr, * dataTable = nullptr, * data1 = nullptr, *data2 = nullptr, *data3 = nullptr;;
        size_t size = 0, sizeTable = 0, size1 = 0, size2 = 0, size3 = 0;
        if( !wxLoadUserResource( &data, &size, "save", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            save = wxBitmapBundle::FromSVG( (const char *) data, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &dataTable, &sizeTable, "table", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            tableSVG = wxBitmapBundle::FromSVG( (const char *) dataTable, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &data1, &size1, "bold", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            boldSVG = wxBitmapBundle::FromSVG( (const char *) data1, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &data2, &size2, "italic", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            italicSVG = wxBitmapBundle::FromSVG( (const char *) data2, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &data3, &size3, "underline", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            underlineSVG = wxBitmapBundle::FromSVG( (const char *) data3, wxSize( 16, 16 ) );
        }
#elif __WXOSX__
        save = wxBitmapBundle::FromSVGResource( "save", wxSize( 16, 16 ) );
        tableSVG = wxBitmapBundle::FromSVGResource( "table", wxSize( 16, 16 ) );
        boldSVG = wxBitmapBundle::FromSVGResource( "bold", wxSize( 16, 16 ) );
        italicSVG = wxBitmapBundle::FromSVGResource( "italic", wxSize( 16, 16 ) );
        underlineSVG = wxBitmapBundle::FromSVGResource( "underline", wxSize( 16, 16 ) );
#else
        save = wxArtProvider::GetBitmapBundle( wxART_FLOPPY, wxART_TOOLBAR );
        tableSVG = wxBitmapBundle::FromSVG( table, wxSize( 16, 16 ) );
        boldSVG = wxBitmapBundle::FromSVG( bold, wxSize( 16, 16 ) );
        italicSVG = wxBitmapBundle::FromSVG( italic, wxSize( 16, 16 ) );
        underlineSVG = wxBitmapBundle::FromSVG( underline, wxSize( 16, 16 ) );
#endif
        m_tb->AddTool( wxID_SAVEQUERY, _( "Save" ), save, save, wxITEM_NORMAL, _( "Save" ), _( "Save Query" ) );
        m_tb->AddTool( wxID_SHOWSQLTOOLBOX, _( "Show ToolBox" ), wxBitmap( toolbox), wxBitmap( toolbox ), wxITEM_CHECK, _( "Toolbox" ), _( "Hide/Show SQL Toolbox" ) );
        m_tb->AddTool( wxID_DATASOURCE, _( "Preview SQL" ), wxBitmap::NewFromPNGData( sql, WXSIZEOF( sql ) ), wxNullBitmap, wxITEM_CHECK, _( "Data Source" ), _( "" ) );
        m_tb->AddTool( wxID_CLOSE, _( "Close View" ), wxBitmap( quit_xpm ), wxBitmap( quit_xpm ), wxITEM_NORMAL, _( "Close" ), _( "Close Query View" ) );
        m_tb->ToggleTool( wxID_SHOWSQLTOOLBOX, true );
        m_tb->InsertTool( 4, wxID_SELECTTABLE, _( "Select Table" ), tableSVG, tableSVG, wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" )  );
        m_tb->InsertTool( 5, wxID_PREVIEDWQUERY, _( "Preview" ), wxBitmap::NewFromPNGData( previewIcon, WXSIZEOF( previewIcon ) ), wxNullBitmap, wxITEM_CHECK, ( "Preview" ) );
        m_tb->ToggleTool( wxID_DATASOURCE, true );
        m_fieldText = new wxTextCtrl( m_styleBar, wxID_ANY, "" );
        m_styleBar->AddControl( m_fieldText );
        m_fontName = new FontComboBox( m_styleBar );
        m_styleBar->AddControl( m_fontName );
        const wxString fontSizes[] = 
        {
            "8",
            "9",
            "10",
            "11",
            "12",
            "14",
            "16",
            "18",
            "20",
            "22",
            "24",
            "26",
            "28",
            "36",
            "48",
            "72"
        };
        m_fontSize = new wxComboBox( m_styleBar, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 16, fontSizes );
        m_styleBar->AddControl( m_fontSize );
        m_styleBar->AddTool( 303, _( "Bold" ), boldSVG, boldSVG, wxITEM_NORMAL, _( "Bold" ), _( "Make the font bold" ) );
        m_styleBar->AddTool( 303, _( "Italic" ), italicSVG, italicSVG, wxITEM_NORMAL, _( "Italic" ), _( "Make the font italic" ) );
        m_styleBar->AddTool( 303, _( "Underline" ), underlineSVG, underlineSVG, wxITEM_NORMAL, _( "Make the font underlined" ), _( "Make the font underlined" ) );
    }
    m_tb->Realize();
#ifdef __WXOSX__
    sizer->Add( m_tb, 1, wxEXPAND, 0 );
#endif
    if( m_styleBar )
    {
        m_styleBar->Realize();
#ifdef __WXOSX__
        sizer->Add( m_styleBar, 1, wxEXPAND, 0 );
#endif
    }
    m_tb->SetSize( 0, 0, size.x, wxDefaultCoord );
    offset = m_tb->GetSize().y;
    auto height = size.y - offset;
    if( m_styleBar )
    {
        m_styleBar->SetSize( 0, offset, size.x, wxDefaultCoord );
        offset += m_styleBar->GetSize().y;
        height -= m_styleBar->GetSize().y;
    }
#ifdef __WXOSX__
    wxPoint pt;
    pt.x = -1;
    pt.y = m_parent->GetRect().GetHeight() - m_parent->GetClientSize().GetHeight();
    m_frame->SetSize( pt.x, pt.y, m_parent->GetSize().GetWidth(), m_parent->GetClientSize().GetHeight() );
#else
    m_frame->SetSize( 0, offset, size.x, height );
#endif
#ifdef __WXMSW__
    m_frame->SetToolBar( m_tb );
#endif
}

// Sneakily gets used for default print/preview as well as drawing on the
// screen.
void DrawingView::OnDraw(wxDC *WXUNUSED(dc))
{
}

void DrawingView::OnCloseLogWindow(wxCloseEvent &WXUNUSED(event))
{
    m_log->Hide();
}

void DrawingView::GetTablesForView(Database *db, bool init, const std::vector<QueryInfo> &queries, std::vector<LibrariesInfo> &path)
{
    std::map<wxString, std::vector<TableDefinition> > tables;
    int res = -1;
    wxString query, documentName = "";
    wxDynamicLibrary lib;
    bool quickSelect = false;
    m_queries = queries;
    m_path = path;
    m_canvas->SetQueryInfo( queries );
    m_canvas->SetObjectPath( path );
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
                bool update;
                CHOOSEOBJECT func = (CHOOSEOBJECT) lib.GetSymbol( "ChooseObject" );
                res = func( m_frame, 1, m_queries, documentName, m_path, update );
                if( res == wxID_CANCEL )
                {
                    m_frame->Close();
                    return;
                }
                if( res == wxID_NEWOBJECT )
                {
                    NEWQUERY func1 = (NEWQUERY) lib.GetSymbol( "NewQueryDlg" );
                    auto res1 = func1( m_frame, m_source, m_presentation );
                    if( res1 != wxID_CANCEL )
                    {
                        wxConfigBase *config = wxConfigBase::Get( false );
                        wxString path = config->GetPath();
                        config->SetPath( "Query" );
                        config->Write( "QuerySource", m_source );
                        config->Write( "QueryPresentation", m_presentation );
                        config->SetPath( path );
                        auto res2 = SelectTable( false, m_tables, query, quickSelect );
                        if( res2 == wxID_CANCEL )
                            return;
                    }
                }
                if( res == wxID_OK )
                {
                    wxBusyCursor wait;
                    GetDocument()->SetFilename( documentName + ".qry" );
                    if( GetDocument()->OnOpenDocument( documentName + ".qry" ) && ((DrawingDocument *) GetDocument() )->IsLoadSuccessful() )
                    {
                        GetDatabaseCanvas()->LoadQuery( GetDocument()->GetDatabase()->GetTableVector().m_tables );
                    }
                    else
                    {
                        wxMessageBox( "Error loading the query from the disk" );
                        return;
                    }
                }
                if( res != wxID_CANCEL )
                {
                    if( m_source != 0 )
                    {
//                        wxMDIClientWindow *parent = (wxMDIClientWindow *) m_parent->GetClientWindow();
//                        wxSize parentSize = parent->GetSize();
//                        wxPoint parentPos = parent->GetPosition();
#ifndef __WXOSX__
                        int heightStyleBar = m_styleBar->GetSize().y;
                        wxSize frameSize = m_frame->GetSize();
#endif
                        wxPoint framePosition = m_frame->GetPosition();
                        if( framePosition.y == 0 )
                        {
#ifndef __WXOSX__
                            m_frame->SetSize( frameSize.GetWidth(), frameSize.GetHeight() + heightStyleBar );
#endif
                        }
                        m_queryType = SQLSelectMenu;
                        CreateQueryMenu( SQLSelectMenu );
                        m_frame->Freeze();
                        m_styleBar->Show( false );
                        m_designCanvas->Show( false );
                        m_fields->Show( true );
                        m_canvas->Show( true );
                        m_queryBook->Show( true );
#ifndef __WXOSX__
                        m_frame->SetSize( framePosition.x, framePosition.y - heightStyleBar, frameSize.GetWidth(), frameSize.GetHeight() + heightStyleBar );
#endif
                        m_frame->Layout();
                        sizer->Layout();
                        m_frame->Thaw();
#ifdef __WXGTK__
                        m_parent->SendSizeEvent();
                        wxYield();
#endif
                    }
                    else
                    {
                        QUICKSELECT func2 = (QUICKSELECT) lib.GetSymbol( "QuickSelectDlg" );
                        res = func2( m_frame, db, m_selectTableName, GetDocument()->GetQueryFields(), GetDocument()->GetAllSorted(), GetDocument()->GetQuerySorted() );
                        quickSelect = true;
                    }
                }
            }
            else
            {
                SelectTable( false, m_tables, query, quickSelect );
            }
        }
        else
        {
            SelectTable( false, m_tables, query, false );
            sizer->Layout();
            m_canvas->Show( true );
        }
    }
    if( m_tables.size() > 0 )
    {
        if( m_type == QueryView )
        {
            if( query != L"\n" )
            {
                int i = 0;
                std::vector<MyErdTable *> dbTables = ((DrawingDocument *)GetDocument())->GetTables();
                for( std::vector<MyErdTable *>::iterator it = dbTables.begin(); it < dbTables.end(); ++it )
                {
                    const DatabaseTable *dbTable = (*it)->GetTable();
                    for( std::vector<TableField *>::const_iterator it1 = dbTable->GetFields().begin(); it1 < dbTable->GetFields().end(); ++it1 )
                    {
                        long item = m_page3->GetSourceList()->InsertItem( i++, "\"" + dbTable->GetTableName() + "\".\"" + (*it1)->GetFieldName() + "\"" );
                        m_page3->GetSourceList()->SetItemData( item, item );
                        GetDocument()->AddGroupByAvailableField( "\"" + dbTable->GetTableName() + "\".\"" + (*it1)->GetFieldName() + "\"", item );
                    }
                }
                m_page3->GetSourceList()->SetColumnWidth( 0, m_page3->GetSourceList()->GetSize().GetWidth() );
                m_page3->GetDestList()->SetColumnWidth( 0, m_page3->GetDestList()->GetSize().GetWidth() );
                m_page1->AddQuickSelectSortingFields( GetDocument()->GetAllSorted(), GetDocument()->GetQuerySorted() );
                m_page6->SetSyntaxText( query );
                m_edit->SetText( query );
            }
            if( quickSelect )
            {
                PopuateQueryCanvas();
                auto position = m_frame->GetMenuBar()->FindMenu( _( "Design" ) );
                auto designMenu = m_frame->GetMenuBar()->GetMenu( position );
                designMenu->Check( wxID_DATASOURCE, false );
                m_tb->ToggleTool( wxID_DATASOURCE, false );
            }
        }
    }
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
    wxDocMDIParentFrame *mainWin = (wxDocMDIParentFrame *) m_frame->GetMDIParent();
    m_isActive = false;
    wxMDIClientWindow *frame = (wxMDIClientWindow *) mainWin->GetClientWindow();
    if( GetDocument()->GetViewsVector().size() == 1 )
    {
//        int y = m_tb->GetHeight();
        frame->SetPosition( wxPoint( 0, 0 ) );
        delete m_tb;
        m_tb = nullptr;
    }
    else
        m_tb->ClearTools();
    if( !wxView::OnClose( deleteWindow ) )
        return false;

//    Activate( false );

    return true;
}

void DrawingView::OnNewIndex(wxCommandEvent &WXUNUSED(event))
{
    int result;
    wxString command, indexName;
    std::vector<std::wstring> errors;
    DatabaseTable *dbTable = NULL;
    ShapeList shapes;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( MyErdTable ), shapes );
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end(); ++it )
    {
        if( (*it)->IsSelected() )
            dbTable = const_cast<DatabaseTable *>( ((MyErdTable *) *it)->GetTable() );
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
        result = func( m_frame, dbTable, GetDocument()->GetDatabase(), command, indexName );
        if( result != wxID_OK && result != wxID_CANCEL )
        {
            m_text->AppendText( command );
            m_text->AppendText( "\n\r\n\r" );
            if( !m_log->IsShown() )
                m_log->Show();
        }
        else if( result == wxID_OK )
        {
            Database *db = dynamic_cast<DrawingDocument *>( GetDocument() )->GetDatabase();
            {
#if defined __WXMSW__ && _MSC_VER < 1900
                wxCriticalSectionLocker( *pcs );
#else
//#if _MSC_VER >= 1900 || !(defined __WXMSW__)
                std::lock_guard<std::mutex> locker( db->GetTableVector().my_mutex );
#endif
                db->CreateIndex( command.ToStdWstring(), indexName.ToStdWstring(), dbTable->GetCatalog(), dbTable->GetSchemaName(), dbTable->GetTableName(), errors );
            }
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
    int result, deleteProp, updateProp;
    DatabaseTable *dbTable = NULL;
    std::vector<std::wstring> foreignKeyFields, refKeyFields;
    std::wstring refTableName, command;
    std::vector<FKField *> newFK;
    ShapeList shapes;
    wxString kName;
    std::wstring keyName;
    bool logOnly = false;
    int match = 2;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( MyErdTable ), shapes );
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end(); ++it )
    {
        if( (*it)->IsSelected() )
            dbTable = const_cast<DatabaseTable *>( ((MyErdTable *) *it)->GetTable() );
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
        result = func( m_frame, kName, dbTable, foreignKeyFields, refKeyFields, refTableName, deleteProp, updateProp, GetDocument()->GetDatabase(),  logOnly, false, newFK, match );
        if( result != wxID_CANCEL )
        {
            int res = GetDocument()->GetDatabase()->ApplyForeignKey( command, kName.ToStdWstring(), *dbTable, foreignKeyFields, refTableName, refKeyFields, deleteProp, updateProp, logOnly, newFK, true, match, errors );
            if( res )
            {
                for( std::vector<std::wstring>::iterator it = errors.begin(); it < errors.end(); it++ )
                {
                    wxMessageBox( (*it), _( "Error" ), wxOK | wxICON_ERROR );
                }
            }
            else if( logOnly )
            {
                m_text->AppendText( command );
                m_text->AppendText( "\n\r\n\r" );
                if( !m_log->IsShown() )
                    m_log->Show();
            }
            else
            {
                m_canvas->CreateFKConstraint( dbTable, newFK );
            }
        }
        for( std::vector<FKField *>::iterator it = newFK.begin(); it != newFK.end(); ++it )
        {
            delete (*it);
            (*it) = NULL;
        }
        newFK.clear();
    }
    else
        wxMessageBox( _( "Error loading the DLL/so" ) );
}

void DrawingView::OnViewSelectedTables(wxCommandEvent &WXUNUSED(event))
{
    wxString query = "";
    if( m_page6 )
        query = m_page6->GetSyntaxCtrl()->GetValue();
    SelectTable( false, m_tables, query, false );
}

int DrawingView::SelectTable(bool isTableView, std::map<wxString, std::vector<TableDefinition> > &tables, wxString &query, bool quickSelect)
{
    wxDynamicLibrary lib;
    int res = 0;
#ifdef __WXMSW__
    lib.Load( "dialogs" );
#elif __WXMAC__
    lib.Load( "liblibdialogs.dylib" );
#else
    lib.Load( "libdialogs" );
#endif
    if( lib.IsLoaded() )
    {
        TABLESELECTION func2 = (TABLESELECTION) lib.GetSymbol( "SelectTablesForView" );
        res = func2( m_frame, GetDocument()->GetDatabase(), tables, GetDocument()->GetTableNames(), isTableView, m_type );
    }
    if( m_type == QueryView )
    {
        if( res != wxID_CANCEL )
        {
            std::vector<TableField *> queryFields = GetDocument()->GetQueryFields();
            query = "SELECT ";
            if( !quickSelect && queryFields.size() == 0 )
                query += "<unknown fields>\n";
            else
            {
                if( !quickSelect )
                {
                    for( std::vector<TableField *>::iterator it = queryFields.begin(); it < queryFields.end(); it++ )
                    {
                        query += (*it)->GetFieldName();
                        if( it != queryFields.end() - 1 )
                            query += ",";
                    }
                    query += "\n";
                }
                else
                {
                    for( std::vector<TableField *>::iterator it = queryFields.begin(); it < queryFields.end(); ++it )
                    {
                        query += (*it)->GetFullName();
                        if( it != queryFields.end() - 1 )
                            query += ",";
                        m_fields->AddField( (*it)->GetFullName() );
                    }
                    query += "\n";
                }
            }
            query += "FROM ";
        }
    }
    if( quickSelect && m_selectTableName.size()  > 0 )
    {
        if( GetDocument()->GetDatabase()->GetTableVector().GetDatabaseType() == L"SQLite" )
        {
            wxString name = m_selectTableName[0]->GetSchemaName() + "." + m_selectTableName[0]->GetTableName();
            tables[m_selectTableName[0]->GetSchemaName()].push_back( TableDefinition( L"", m_selectTableName[0]->GetSchemaName(), m_selectTableName[0]->GetTableName() ) );
        }
        else
            tables[m_selectTableName[0]->GetCatalog()].push_back( TableDefinition( m_selectTableName[0]->GetCatalog(), m_selectTableName[0]->GetSchemaName(), m_selectTableName[0]->GetTableName() ) );
    }
    if( res != wxID_CANCEL && m_tables.size() > 0 )
    {
        ((DrawingDocument *) GetDocument())->AddTables( m_tables );
        m_selectTableName = ((DrawingDocument *) GetDocument())->GetDBTables();
        ((DatabaseCanvas *) m_canvas)->DisplayTables( m_tables, GetDocument()->GetQueryFields(), query, m_whereRelatons );
        if( m_type == QueryView )
        {
            if( query != L"\n" )
            {
                int i = 0;
                std::vector<MyErdTable *> dbTables = ((DrawingDocument *)GetDocument())->GetTables();
                for( std::vector<MyErdTable *>::iterator it = dbTables.begin(); it < dbTables.end(); ++it )
                {
                    const DatabaseTable *dbTable = (*it)->GetTable();
                    for( std::vector<TableField *>::const_iterator it1 = dbTable->GetFields().begin(); it1 < dbTable->GetFields().end(); ++it1 )
                    {
                        long item = m_page3->GetSourceList()->InsertItem( i++, "\"" + dbTable->GetTableName() + "\".\"" + (*it1)->GetFieldName() + "\"" );
                        m_page3->GetSourceList()->SetItemData( item, item );
                        GetDocument()->AddGroupByAvailableField( "\"" + dbTable->GetTableName() + "\".\"" + (*it1)->GetFieldName() + "\"", item );
                    }
                }
                m_page3->GetSourceList()->SetColumnWidth( 0, m_page3->GetSourceList()->GetSize().GetWidth() );
                m_page3->GetDestList()->SetColumnWidth( 0, m_page3->GetDestList()->GetSize().GetWidth() );
                m_page1->AddQuickSelectSortingFields( GetDocument()->GetAllSorted(), GetDocument()->GetQuerySorted() );
                m_page6->SetSyntaxText( query );
                m_edit->SetText( query );
            }
            if( quickSelect )
            {
                PopuateQueryCanvas();
                auto position = m_frame->GetMenuBar()->FindMenu( _( "Design" ) );
                auto designMenu = m_frame->GetMenuBar()->GetMenu( position );
                designMenu->Check( wxID_DATASOURCE, false );
                m_tb->ToggleTool( wxID_DATASOURCE, false );
            }
        }
    }
    return res;
}

void DrawingView::OnSetProperties(wxCommandEvent &event)
{
//    SetProperties();
}

void DrawingView::SetProperties(const wxSFRectShape *shape)
{
    ShapeList selections;
    std::vector<std::wstring> errors;
    int type = 0;
    DatabaseTable *dbTable = nullptr;
    Divider *divider = nullptr;
    DesignLabel *label = nullptr;
    MyErdTable *erdTable = NULL;
    ConstraintSign *sign = NULL;
    TableField *field = NULL;
    wxString command = "";
    bool logOnly = false;
    if( !shape )
    {
        m_canvas->GetAllSelectedShapes( selections );
        bool found = false;
        for( ShapeList::iterator it = selections.begin(); it != selections.end() && !found; ++it )
        {
            shape = wxDynamicCast( (*it), wxSFRectShape );
            if( shape )
                found = true;
        }
    }
    wxString tableName, schemaName, ownerName;
        if( !shape )
        {
            type = DesignProperties;
            dbTable = nullptr;
        }
        else
        {
            erdTable = wxDynamicCast( shape, MyErdTable );
            if( erdTable )
            {
                dbTable = const_cast<DatabaseTable *>( ((MyErdTable *) shape)->GetTable() );
                tableName = dbTable->GetTableName();
                schemaName = dbTable->GetSchemaName();
                type = DatabaseTableProperties;
            }
            else
            {
                divider = wxDynamicCast( shape, Divider );
                if( divider )
                {
                    type = DividerProperties;
                    dbTable = nullptr;
                }
                else
                {
                    label = wxDynamicCast( shape, DesignLabel );
                    if( label )
                    {
                        type = DesignLabelProperties;
                        dbTable = nullptr;
                    }
                    else
                    {
                        field = ((FieldShape *) shape)->GetField();
                        MyErdTable *my_table = dynamic_cast<MyErdTable *>( ((FieldShape *) shape)->GetParentShape()->GetParentShape() );
                        if( my_table )
                        {
                            erdTable = my_table;
                            tableName = const_cast<DatabaseTable *>( erdTable->GetTable() )->GetTableName();
                            schemaName = const_cast<DatabaseTable *>( erdTable->GetTable() )->GetSchemaName();
                            ownerName = const_cast<DatabaseTable *>( erdTable->GetTable() )->GetTableOwner();
                            type = DatabaseFieldProperties;
                        }
                        else
                        {
                            sign = wxDynamicCast( shape, ConstraintSign );
                            if( sign )
                            {
                                type = SignProperties;
                            }
                        }
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
    std::unique_ptr<PropertiesHandler> propertiesPtr;
    wxString title;
    if( lib.IsLoaded() )
    {
        CREATEPROPERTIESDIALOG func = (CREATEPROPERTIESDIALOG) lib.GetSymbol( "CreatePropertiesDialog" );
        if( type == DatabaseTableProperties )
        {
#if defined __WXMSW__ && _MSC_VER < 1900
            wxCriticalSectionLocker( *pcs );
#else
            //#if _MSC_VER >= 1900
            std::lock_guard<std::mutex> lock( GetDocument()->GetDatabase()->GetTableVector().my_mutex );
#endif
            res = GetDocument()->GetDatabase()->GetTableProperties( dbTable, errors );
#if __cplusplus > 201300
            auto ptr = std::make_unique<DatabasePropertiesHandler>( GetDocument()->GetDatabase(), dbTable, m_text );
#else
            auto ptr = std::unique_ptr<DatabasePropertiesHandler>( new DatabasePropertiesHandler( GetDocument()->GetDatabase(), dbTable, m_text ) );
#endif
            propertiesPtr = std::move( ptr );
            title = _( "Table " );
            title += schemaName + L"." + tableName;
        }
        if( type == DatabaseFieldProperties )
        {
            {
#if defined __WXMSW__ && _MSC_VER < 1900
                wxCriticalSectionLocker( *pcs );
#else
                //#if _MSC_VER >= 1900
                std::lock_guard<std::mutex> lock( GetDocument()->GetDatabase()->GetTableVector().my_mutex );
#endif
                res = GetDocument()->GetDatabase()->GetFieldProperties( tableName.ToStdWstring(), field, errors );
            }
#if __cplusplus > 201300
            auto ptr = std::make_unique<FieldPropertiesHandler>( GetDocument()->GetDatabase(), tableName, ownerName, field, m_text );
#else
            auto ptr = std::unique_ptr<FieldPropertiesHandler>( new FieldPropertiesHandler( GetDocument()->GetDatabase(), tableName, ownerName, field, m_text ) );
#endif
            propertiesPtr = std::move( ptr );
            title = _( "Column " );
            title += tableName + ".";
            title += field->GetFieldName();
        }
        if( type == DividerProperties )
        {
#if __cplusplus > 201300
            auto ptr = std::make_unique<DividerPropertiesHander>( divider->GetDividerProperties() );
#else
            auto ptr = std::unique_ptr<DividerPropertiesHander>( new DividerPropertiesHander( divider->GetDividerProperties() ) );
#endif
            propertiesPtr = std::move( ptr );
            title = _( "Band Object" );
        }
        if( type == DesignProperties )
        {
#if __cplusplus > 201300
            auto ptr = std::make_unique<DesignPropertiesHander>( m_designCanvas->GetOptions() );
#else
            auto ptr = std::unique_ptr<DesignPropertiesHander>( new DesignPropertiesHander( m_designCanvas->GetOptions() ) );
#endif
            propertiesPtr = std::move( ptr );
            title = _( "Query Object" );
        }
//        TableProperties props = *static_cast<TableProperties *>( properties );
        res = func( m_frame, propertiesPtr, title, command, logOnly, *pcs );
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
        if( !deactiveView && m_tb )
        {
            m_tb->Destroy();
            m_tb = NULL;
            if( frame && frame->GetParent() )
                frame->GetParent()->SetSize( 0, 0, clientSize.x, clientSize.y ); 
        }
        else
        {
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
    lib1.Load( "tabledataedit" );
#elif __WXOSX__
    lib1.Load( "liblibtabledataedit.dylib" );
#else
    lib1.Load( "libtabledataedit" );
#endif
    if( lib1.IsLoaded() )
    {
        TABLE func = (TABLE) lib1.GetSymbol( "CreateDatabaseWindow" );
        MyErdTable *erdTable = dynamic_cast<MyErdTable *>( (*it) );
        func( parent, GetDocumentManager(), dynamic_cast<DrawingDocument *>( GetDocument() )->GetDatabase(), const_cast<DatabaseTable *>( erdTable->GetTable() ), wxEmptyString );                 // create with possible alteration table
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
    MyErdTable *dbTable;
    FieldShape *field = NULL;
    ShapeList::iterator it;
    bool found = false;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( wxSFRectShape ), shapes );
    for( it = shapes.begin(); it != shapes.end() && !found; ++it )
    {
        if( (*it)->IsSelected() )
        {
            if( wxDynamicCast( (*it), MyErdTable ) )
                dbTable = wxDynamicCast( (*it), MyErdTable );
            if( wxDynamicCast( (*it), FieldShape ) )
                field = wxDynamicCast( (*it), FieldShape );
        }
    }
    wxDynamicLibrary lib1;
#ifdef __WXMSW__
    lib1.Load( "tabledataedit" );
#elif __WXOSX__
    lib1.Load( "liblibtabledataedit.dylib" );
#else
    lib1.Load( "libtabledataedit" );
#endif
    if( lib1.IsLoaded() )
    {
        TABLE func = (TABLE) lib1.GetSymbol( "CreateDatabaseWindow" );
        MyErdTable *erdTable = dynamic_cast<MyErdTable *>( (*it) );
        func( parent, GetDocumentManager(), dynamic_cast<DrawingDocument *>( GetDocument() )->GetDatabase(), const_cast<DatabaseTable *>( erdTable->GetTable() ), field->GetField()->GetFieldName() );                 // display field parameters
    }
    else if( !lib1.IsLoaded() )
        wxMessageBox( "Error loading the library. Please re-install the software and try again." );
    else
        wxMessageBox( "Error connecting to the database. Please check the database is accessible and you can get a good connection, then try again." );
}

SortGroupByPage *DrawingView::GetGroupByPage()
{
    return m_page3;
}

SortGroupByPage *DrawingView::GetSortPage()
{
    return m_page1;
}

WhereHavingPage *DrawingView::GetWherePage()
{
    return m_page2;
}

WhereHavingPage *DrawingView::GetHavingPage()
{
    return m_page4;
}

SyntaxPropPage *DrawingView::GetSyntaxPage()
{
    return m_page6;
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
        db = func( m_frame->GetParent(), name, engine, L"" );
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

void DrawingView::AddFieldToQuery(const FieldShape &field, QueryFieldChange isAdding, const std::wstring &tableName, bool quickSelect)
{
    TableField *fld = const_cast<FieldShape &>( field ).GetField();
    wxString name = tableName + "." + fld->GetFieldName();
    name = "\"" + name;
    name = name + "\"";
    wxString query = m_page6->GetSyntaxCtrl()->GetValue();
    if( isAdding == ADD )
    {
        m_fields->AddField( name );
        m_page1->AddRemoveSortingField( true, name );
        GetDocument()->AddRemoveField( fld, QueryFieldChange::ADD );
        std::vector<TableField *> queryFields = GetDocument()->GetQueryFields();
        if( queryFields.size() == 1 )
            query.Replace( "<unknown fields>", name + " " );
        else
        {
            wxString temp = query.substr( query.Find( ' ' ) );
            temp = temp.substr( 0, temp.Find( "FROM" ) - 1 );
            query.Replace( temp, temp + ", " + name + " " );
        }
    }
    else if( isAdding == REMOVE )
    {
        wxString temp1;
        m_fields->RemoveField( name );
        m_page1->AddRemoveSortingField( false, name );
        GetDocument()->AddRemoveField( fld, QueryFieldChange::REMOVE );
        std::vector<TableField *> queryFields = GetDocument()->GetQueryFields();
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
            temp1 = query.substr( query.Find( name ) + name.length() );
            if( temp == query )
            {
                temp = "SELECT ";
                temp1 = temp1.substr( 2 );
            }
            query = temp + temp1;
        }
    }
    else if( isAdding == SHUFFLE )
    {

    }
    m_page6->SetSyntaxText( query );
    m_edit->SetText( query );
    dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->SetQuery( query );
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
    AddDeleteFields( shape, id == wxID_DESELECTALLFIELDS ? false : true, const_cast<DatabaseTable *>( shape->GetTable() )->GetTableName() );
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
            FieldShape *field2add = (FieldShape *) node->GetData();
            if( field2add && isAdd ? !field2add->IsSelected() : field2add->IsSelected() )
            {
                field2add->Select( isAdd );
                AddFieldToQuery( *field2add, isAdd ? ADD : REMOVE, tableName, false );
            }
            node = node->GetNext();
        }
        m_canvas->Refresh();
    }
}

void DrawingView::HideShowSQLBox(bool show)
{
    m_queryBook->Show( show );
    m_tb->ToggleTool( wxID_SHOWSQLTOOLBOX, show );
    m_frame->Layout();
    auto item = m_frame->GetMenuBar()->FindItem( wxID_SHOWSQLTOOLBOX );
    item->Check( show );
    m_canvas->CheckSQLToolbox();
}

void DrawingView::OnDistinct(wxCommandEvent &event)
{
    wxString qry;
    wxString query = m_page6->GetSyntaxCtrl()->GetValue();
    wxTextCtrl *queryText = m_page6->GetSyntaxCtrl();
    if( !dynamic_cast<wxMenu *>( event.GetEventObject() )->IsChecked( wxID_DISTINCT ) )
    {
        query.Replace( "SELECT DISTINCT ", "SELECT " );
    }
    else
    {
        query.Replace( "SELECT ", "SELECT DISTINCT " );
    }
    queryText->SetValue( query );
    m_edit->SetText( query );
    dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->SetQuery( query );
}

wxFrame *DrawingView::GetLogWindow() const
{
    return m_log;
}

wxTextCtrl *DrawingView::GetTextLogger() const
{
    return m_text;
}

void DrawingView::SetPaintersMap(std::map<wxString, wxDynamicLibrary *> &painters)
{
    m_painters = painters;
}

void DrawingView::UpdateQueryFromSignChange(const QueryConstraint *type, const long oldSign)
{
    auto res = true;
    auto sign = type->GetSign();
    auto query = m_page6->GetSyntaxCtrl()->GetValue();
    wxString orig, result, newSign, temp;
    auto it = std::find_if( m_whereRelatons.begin(), m_whereRelatons.end(), [type](wxString iter)
    {
        return( iter.Find( type->GetRefTable() ) && iter.Find( type->GetFKTable()->GetTableName() ) );
    } );
    orig = (*it);
    switch( sign )
    {
    case 0:
    case 1:
    case 2:
        newSign = "=";
        break;
    case 3:
        newSign = "<";
        break;
    case 4:
        newSign = ">";
        break;
    case 5:
        newSign = "<=";
        break;
    case 6:
        newSign = ">=";
        break;
    }
    if( sign != 1 && sign != 2 && oldSign != 1 && oldSign != 2 )
    {
        wxString old = orig.substr( orig.Find( ' ' ) + 1 );
        old = old.substr( 0, old.Find( ' ' ) );
        temp = orig;
        temp.Replace( old, newSign );
        query.Replace( orig, temp );
        result = query;
    }
    else if( ( sign == 1 || sign == 2 ) && oldSign != 1 && oldSign != 2 )
    {
        result = query.substr( 0, query.find( "\n" ) + 1 );
        query = query.substr( query.find( "\n" ) + 1 );
        result += "FROM ";
        if( sign == 1 )
            result += const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + " LEFT OUTER JOIN " + type->GetRefTable() + " ON " + const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + "." + type->GetLocalColumn() + " = " + type->GetRefTable() + "." + const_cast<QueryConstraint *>( type )->GetRefColumn();
        else
            result += type->GetRefTable() + " LEFT OUTER JOIN " + const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + " ON " + type->GetRefTable() + "." + const_cast<QueryConstraint *>( type )->GetRefColumn() + " = " + const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + "." + type->GetLocalColumn();
        query = query.substr( query.find( "WHERE" ) );
        if( m_whereRelatons.size() == 1 && m_whereCondition.size() == 0 )
        {
            size_t pos;
            pos = query.find( "GROUP BY" );
            if( pos == wxNOT_FOUND )
            {
                pos = query.find( "HAVING" );
                if( pos == wxNOT_FOUND )
                {
                    pos = query.find( "ORDER BY" );
                    if( pos == wxNOT_FOUND )
                        pos = query.length() - 1;
                }
            }
            query = query.substr( pos );
            if( query.Length() == 1 )
                result += ";";
            temp = result;
        }
        else
        {
            auto temp1 = query.substr( 0, query.find( "\n" ) + 1 );
            temp1 = temp1.substr( temp1.find( ' ' ) + 1 );
            while( temp1 != wxEmptyString )
            {
                auto temp2 = temp1.substr( 0, temp1.find( ',' ) + 1 );
                if( temp2 != const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() && temp2 != type->GetRefTable() )
                    result += ", " + temp2;
                temp1 = temp1.substr( temp1.find( ',' ) );
            }
        }
    }
    else if( sign != 1 && sign != 2 && ( oldSign == 1 || oldSign == 2 ) )
    {
        result = query.substr( 0, query.find( "WHERE " ) + 7 );
        query = query.substr( query.find( " WHERE " ) + 7 );
        while( res )
        {
            auto temp = query.substr( 0, query.find( ' ' ) );
            res = ( temp == const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + "." + type->GetLocalColumn() ) ||
                  ( temp == type->GetRefTable() + "." + const_cast<QueryConstraint *>( type )->GetRefColumn() );
            if( res )
            {
                result += temp;
                switch( type->GetSign() )
                {
                    case 3:
                        result += " < ";
                        break;
                    case 4:
                        result += " > ";
                        break;
                    case 5:
                        result += " <= ";
                        break;
                    case 6:
                        result += " >= ";
                        break;
                    case 7:
                        result += " <> ";
                        break;
                    case 0:
                        result += " = ";
                        break;
                }
                query = query.substr( query.find( ' ' ) + 1 );
                query = query.substr( query.find( ' ' ) + 1 );
                result += query;
                res = false;
            }
            else
            {
                int pos = query.Find( " AND " );
                if( pos != wxNOT_FOUND )
                {
                    result += query.substr( 0, pos + 5 );
                    query = query.substr( pos + 5 );
                }
                else
                {
                    pos = query.Find( " OR " );
                    result += query.substr( 0, pos + 4 );
                    query = query.substr( pos + 4 );
                }
                res = true;
            }
        }
    }
    m_whereRelatons.erase( it );
    m_whereRelatons.push_back( temp );
    m_page6->SetSyntaxText( result );
    m_edit->SetText( result );
    dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->SetQuery( query );
}

void DrawingView::OnQueryChange(wxCommandEvent &event)
{
    wxString query = m_page6->GetSyntaxCtrl()->GetValue();
    if( event.GetEventObject() == m_page2 )
    {
        wxString wherePart;
        size_t pos = query.find( "WHERE" );
        if( pos != wxNOT_FOUND )
        {
            wherePart = query.substr( pos + 6 );
            pos = wherePart.find( "GROUP BY" );
            if( pos == wxNOT_FOUND )
            {
                pos = wherePart.find( "HAVING " );
                if( pos == wxNOT_FOUND )
                {
                    pos = wherePart.find( "ORDER BY" );
                    if( pos == wxNOT_FOUND )
                        pos = wherePart.length() - 1;
                }
            }
            wherePart = wherePart.substr( 0, pos );
            m_whereCondition[event.GetInt() + 1] = event.GetString();
            ///WhereHavingLines line = *(WhereHavingLines *) event.GetClientData();
        }
        else
        {
            pos = query.find( "GROUP BY" );
            if( pos == wxNOT_FOUND )
            {
                pos = query.find( "HAVING " );
                if( pos == wxNOT_FOUND )
                {
                    pos = query.find( "ORDER BY" );
                    if( pos == wxNOT_FOUND )
                        wherePart = ";";
                }
            }
        }
        wxString newWherePart;
        if( wherePart.Length() > 1 && wherePart.Last() == '\n' )
        {
            wxString rep = " AND\n";
            newWherePart = wherePart;
            newWherePart.Replace( "\n", rep );
        }
        else
            newWherePart = wherePart + "\n";
        newWherePart += "       " + event.GetString();
        query.Replace( wherePart, newWherePart );
        m_page6->GetSyntaxCtrl()->SetValue( query );
        dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->SetQuery( query );
    }
    if( event.GetEventObject () == m_page4 )
    {
        size_t pos = query.find( "HAVING " );
        if( pos != wxNOT_FOUND )
        {
            wxString havingPart = query.substr( pos + 7 );
            havingPart = havingPart.substr( 0, ( pos = havingPart.find( "ORDER BY" ) ) == wxNOT_FOUND ? havingPart.length() - 1 : pos );
            m_havingCondition[event.GetInt()] = event.GetString();
        }
        else
        {

        }
    }
    if( event.GetEventObject() == m_page1 || event.GetEventObject() == m_page3 )
    {
        SortGroupByHandling( event.GetInt(), event.GetString(), m_queryBook->GetSelection(), query, (Positions *)( event.GetClientData() ) );
    }
}

void DrawingView::SortGroupByHandling(const int type, const wxString &fieldName, const int queryType, wxString &query, const Positions *pos)
{
    size_t start, end;
    bool isInserting = false;
    wxString queryString;
    if( queryType == 2 )
    {
        start = query.find( "GROUP BY" );
        end = query.find( "HAVING" );
        if( end == wxNOT_FOUND )
            end = query.find( "ORDER BY" );
        queryString = "GROUP BY ";
    }
    else
    {
        start = query.find( "ORDER BY" );
        end = query.length() - 1;
        queryString = "ORDER BY ";
    }
    if( ( start == wxNOT_FOUND && end == wxNOT_FOUND && queryType == 2 ) || ( start == wxNOT_FOUND && queryType == 4 ) )
    {
        start = query.length() - 1;
        end = start + 1;
    }
    if( end == wxNOT_FOUND )
        end = query.length() - 1;
    if( start == wxNOT_FOUND && end != wxNOT_FOUND && queryType == 0 )
    {
        start = end;
        end = start + 1;
    }
    else if( start == wxNOT_FOUND && end != wxNOT_FOUND && queryType == 2 )
    {
        start = end;
        end = query.length();
        isInserting = true;
    }
    wxString str = query.substr( start, end - start ), replace;
    if( type == ADDFIELD )
    {
        if( queryType == 2 )
        {
            GetDocument()->AddGroupByFieldToQuery( fieldName, pos->position, pos->originalPosition, replace );
            if( str == ";" )
                replace = "\n" + replace + ";";
        }
        else
        {
            GetDocument()->AddSortedField( fieldName, pos->originalPosition, pos->position, replace );
            if( str == ";" )
                replace = "\n" + replace + ";";
        }
    }
    else if( type == REMOVEFIELD )
    {
        if( queryType == 2 )
        {
            GetDocument()->DeleteGroupByField( fieldName, pos->originalPosition, replace );
            if( replace.IsEmpty() )
                str = "\r" + str;
        }
        else
        {
            GetDocument()->DeleteSortedField( fieldName, pos->originalPosition, replace );
            if( replace.IsEmpty() )
                str = "\n" + str;
        }
    }
    else
    {
        if( queryType == 2 )
            GetDocument()->ShuffleGroupByFields( fieldName, pos->position, replace );
    }
    query.Replace( str, replace );
    m_page6->GetSyntaxCtrl()->SetValue( query );
    dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->SetQuery( query );
}

void DrawingView::OnRetrievalArguments(wxCommandEvent &WXUNUSED(event))
{
    std::vector<QueryArguments> arguments;
    wxDynamicLibrary *lib = new wxDynamicLibrary();
#ifdef __WXMSW__
    lib->Load( "dialogs" );
#elif __WXMAC__
    lib->Load( "liblibdialogs.dylib" );
#else
    lib->Load( "libdialogs" );
#endif
    if( lib->IsLoaded() )
    {
        if( m_arguments.size() == 0 )
            arguments.push_back( QueryArguments( 1, "", "" ) );
        else
            arguments = m_arguments;
        RETRIEVEARGUMENTS func = (RETRIEVEARGUMENTS) lib->GetSymbol( "GetQueryArguments" );
        int res = func( m_parent, arguments, GetDocument()->GetDatabase()->GetTableVector().GetDatabaseType(), GetDocument()->GetDatabase()->GetTableVector().GetDatabaseSubtype() );
        if( res == wxID_OK )
        {
            if( arguments.size() > 1 || ( arguments.size() == 1 && arguments[0].m_name != "" ) )
            {
                m_page2->SetQueryArguments( arguments );
                m_page4->SetQueryArguments( arguments );
                GetDatabaseCanvas()->SetQueryArguments( arguments );
            }
            m_arguments = arguments;
        }
    }
    delete lib;
}

wxTextCtrl *DrawingView::GetFieldTextCtrl()
{
    return m_fieldText;
}

void DrawingView::FieldTextUpdateUI (wxUpdateUIEvent &event)
{
    ShapeList shapes;
    m_designCanvas->GetSelectedShapes( shapes );
    if( shapes.size() == 1 && wxDynamicCast( shapes[0], DesignLabel ) )
    {
        event.Enable( true );
        m_fieldText->SetValue( dynamic_cast<DesignLabel *>( shapes[0] )->GetProperties().m_text );
//        m_fontName->SetSelection( m_fontName->FindString( dynamic_cast<DesignLabel *>( shapes[0] )->GetProperties().m_font.GetFaceName() ) );
//        m_fontSize->SetSelection( m_fontSize->FindString( wxString::Format( "%d", dynamic_cast<DesignLabel *>( shapes[0] )->GetProperties().m_font.GetPointSize() ) ) );
    }
    else
    {
        m_fieldText->SetValue( "" );
        event.Enable( false );
//        m_fontName->SetSelection( 0 );
//        m_fontSize->SetSelection( 0 );
    }
}

void DrawingView::OnDataSource(wxCommandEvent &event)
{
    bool emptyQuery = true;
    int heightStyleBar = m_styleBar->GetSize().y;
    wxPoint framePosition = m_frame->GetPosition();
    wxSize frameSize = m_frame->GetSize();
    wxMDIClientWindow *parent = (wxMDIClientWindow *) m_parent->GetClientWindow();
    wxSize parentSize = parent->GetSize();
    wxPoint parentPos = parent->GetPosition();
    wxMenuBar *menuBar = m_parent->GetMenuBar();
    if( m_type == QueryView )
    {
        if( event.IsChecked() )
        {
            menuBar->Remove( 1 );
            if( framePosition.y == 0 )
            {
                parent->SetSize( parentPos.x, parentPos.y - heightStyleBar, parentSize.GetWidth(), parentSize.GetHeight() + heightStyleBar );
                m_frame->SetSize( frameSize.GetWidth(), frameSize.GetHeight() + heightStyleBar );
            }
            m_styleBar->Show( false );
            m_designCanvas->Show( false );
            m_fields->Show( true );
            m_canvas->Show( true );
            m_queryBook->Show( true );
            m_frame->Layout();
            sizer->Layout();
            if( m_queryType == SQLSelectMenu )
            {
                auto *designMenu = new wxMenu;
                designMenu->Append( wxID_DATASOURCE, _( "Data Source" ), _( "Data Source" ), wxITEM_CHECK );
                designMenu->Append( wxID_PREVIEDWQUERY, _( "Preview" ), _( "Preview" ) );
                designMenu->AppendSeparator();
                designMenu->Append( wxID_SELECTTABLE, _( "Select Table..." ) );
                designMenu->AppendSeparator();
                designMenu->Append( wxID_RETRIEVEARGS, _( "Retieval Arguments..." ), _( "Define Retrieval Arguments" ) );
                designMenu->Append( wxID_DISTINCT, _( "Distinct" ), _( "Return distinct rows only" ), wxITEM_CHECK );
                designMenu->AppendSeparator();
                designMenu->Append( wxID_CONVERTTOSYNTAX, _( "Convert To Syntax" ), _( "Convert To Syntax" ) );
                auto show = new wxMenu;
                show->Append( wxID_SHOWDATATYPES, _( "Datatypes" ), _( "Show Datatypes" ), wxITEM_CHECK );
                show->Append( wxID_SHOWLABELS, _( "Labels" ), _( "Show Labels" ), wxITEM_CHECK );
                show->Append( wxID_SHOWCOMMENTS, _( "Comments" ), _( "Show Comments" ), wxITEM_CHECK );
                show->Append( wxID_SHOWSQLTOOLBOX, _( "SQL Toolbox" ), _( "SQL Toolbox" ), wxITEM_CHECK );
                show->Append( wxID_SHOWJOINS, _( "Joins" ), _( "Show Joins" ), wxITEM_CHECK );
                designMenu->AppendSubMenu( show, _( "Show" ) );
                designMenu->Check( wxID_DATASOURCE, true );
                show->Check( wxID_SHOWDATATYPES, true );
                show->Check( wxID_SHOWLABELS, true );
                show->Check( wxID_SHOWCOMMENTS, true );
                show->Check( wxID_SHOWSQLTOOLBOX, true );
                show->Check( wxID_SHOWJOINS, true );
                menuBar->Insert( 1, designMenu, _( "Design" ) );
            }
        }
        else
        {
            if( GetDocument()->GetQueryFields().empty() )
            {
                int res = wxMessageBox( _( "Columns are required.\n\rDo you want to select them?" ), _( "Query - Untitled" ), wxYES_NO );
                if( res == wxYES || res == wxNO )
                {
                    wxMenuItem *dataSourceMenu = m_frame->GetMenuBar()->FindItem( wxID_DATASOURCE );
                    if( dataSourceMenu )
                    {
                        dataSourceMenu->GetMenu()->Check( wxID_DATASOURCE, true );
                        m_tb->ToggleTool( wxID_DATASOURCE, true );
                    }
                }
                return;
//                    emptyQuery = false;
            }
            if( emptyQuery )
            {
                m_styleBar->Show( true );
                if( framePosition.y == 0 )
                {
                    parent->SetSize( parentPos.x, parentPos.y + heightStyleBar, parentSize.GetWidth(), parentSize.GetHeight() - heightStyleBar );
                    m_frame->SetSize( frameSize.GetWidth(), frameSize.GetHeight() - heightStyleBar - 2 );
                }
                m_designCanvas->Show( true );
                m_fields->Show( false );
                m_canvas->Show( false );
                m_queryBook->Show( false );
                m_frame->Layout();
                PopuateQueryCanvas();
                sizer->Layout();
                auto editMenu = new wxMenu;
                editMenu->Append( wxID_UNDO, _( "Can't Undo\tCtrl+Z" ), _( "Undo" ) );
                editMenu->AppendSeparator();
                editMenu->Append( wxID_CUT, _( "Cut\tCtrl+X" ), _( "Cut" ) );
                editMenu->Append( wxID_COPY, _( "Copy\tCtrl+C" ), _( "Copy" ) );
                editMenu->Append( wxID_PASTE, _( "Paste\tCtrl+V" ), _( "Paste" ) );
                editMenu->Append( wxID_DELETE, _( "Clear\tDel" ), _( "Clear" ) );
                editMenu->AppendSeparator();
                editMenu->Append( wxID_PROPERTIES, _( "Properties" ), _( "Properties" ) );
                menuBar->Insert( 1, editMenu, _( "Edit" ) );
            }
        }
    }
    if( !GetDocument()->GetQueryFields().empty() )
    {
        wxMenuItem *dataSourceMenu = m_frame->GetMenuBar()->FindItem( wxID_DATASOURCE );
        if( dataSourceMenu )
        {
            dataSourceMenu->GetMenu()->Check( wxID_DATASOURCE, event.IsChecked() );
            m_tb->ToggleTool( wxID_DATASOURCE, event.IsChecked() );
        }
    }
    if( !emptyQuery )
    {
        wxMenuItem *dataSourceMenu = m_parent->GetMenuBar()->FindItem( wxID_DATASOURCE );
        if( dataSourceMenu )
        {
            dataSourceMenu->GetMenu()->Check( wxID_DATASOURCE, event.IsChecked() );
            m_tb->ToggleTool( wxID_DATASOURCE, true );
        }
    }
}

void DrawingView::OnTabOrder(wxCommandEvent &event)
{
    wxMenuBar *bar = m_parent->GetMenuBar();
    if( event.IsChecked() )
    {
        bar->EnableTop( 0, false );
        bar->EnableTop( 1, false );
        bar->EnableTop( 2, false );
        bar->EnableTop( 4, false );
    }
    else
    {
        bar->EnableTop( 0, true );
        bar->EnableTop( 1, true );
        bar->EnableTop( 2, true );
        bar->EnableTop( 4, true );
    }
}

void DrawingView::OnFontSeectionChange(wxCommandEvent &WXUNUSED(event))
{
    ChangeFontEement();
}

#ifdef __WXMSW__
int CALLBACK DrawingView::EnumFontFamiliesCallback2(ENUMLOGFONT *WXUNUSED(lpelf), NEWTEXTMETRIC *lpntm, int FontType, LPARAM lParam)
{
    DrawingView *pPage = (DrawingView *) lParam;
    wxASSERT( pPage );
    if( !( FontType & TRUETYPE_FONTTYPE ) )
    {
        int height = lpntm->tmHeight - lpntm->tmInternalLeading;
        pPage->AddSize( height, height );
    }
    return 1;
}
#endif

int DrawingView::AddSize(int size, int lfHeight)
{
#ifdef __WXMSW__
    int pointSize, cyppi = ::GetDeviceCaps( ::GetDC( NULL ), LOGPIXELSY );
    if( lfHeight != 0 )
        pointSize = ::MulDiv( size, 72, cyppi );
    else
        pointSize = size;
    if( lfHeight == 0 )
        lfHeight = ::MulDiv( -cyppi, size, 72 );
    int max = m_fontSize->GetCount(), i;
    wxString str;
    wxStringBuffer( str, 16 );
    str.Printf( "%d", pointSize );
    if( lfHeight > 0 )
    {
        for( i = 0; i < max; i++ )
        {
            int iComp = (int)( lfHeight - m_fontSize->GetSelection() );
            if( !iComp )
                return -1;
            if( iComp < 0 )
                break;
        }
    }
    else
    {
        for( i = 0; i < max; i++ )
        {
            int iComp = (int)( lfHeight - m_fontSize->GetSelection() );
            if( !iComp )
                return -1;
            if( iComp > 0 )
                break;
        }
    }
    if( i == max )
        i = m_fontSize->Append( str, &lfHeight );
    else
        i = m_fontSize->Insert( str, i, &lfHeight );
    return i;
#else
	return 1;
#endif
}

void DrawingView::ChangeFontEement()
{
#ifdef __WXMSW__
    m_fontSize->Clear();
    wxArrayInt ttSizes;
    ttSizes.Add( 8 );
    ttSizes.Add( 9 );
    ttSizes.Add( 10 );
    ttSizes.Add( 11 );
    ttSizes.Add( 12 );
    ttSizes.Add( 14 );
    ttSizes.Add( 16 );
    ttSizes.Add( 18 );
    ttSizes.Add( 20 );
    ttSizes.Add( 22 );
    ttSizes.Add( 24 );
    ttSizes.Add( 26 );
    ttSizes.Add( 28 );
    ttSizes.Add( 36 );
    ttSizes.Add( 48 );
    ttSizes.Add( 72 );
    wxString strFaceName = m_fontName->GetStringSelection();
    HDC dc = ::GetDC( NULL );
    EnumFontFamilies( dc, strFaceName, (FONTENUMPROC) DrawingView::EnumFontFamiliesCallback2, (LPARAM) this );
    ::ReleaseDC( NULL, dc );
    if( (int) m_fontName->GetClientData( m_fontName->GetSelection() ) != RASTER_FONTTYPE )
    {
        for( unsigned int i = 0; i < ttSizes.GetCount(); i++ )
        {
            AddSize( ttSizes[i], 0 );
        }
    }
#endif
}

void DrawingView::OnQueryPreviewUpdateUI(wxUpdateUIEvent &event)
{
    if( GetDocument()->GetQueryFields().size() > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}

void DrawingView::OnShowSQLBox (wxCommandEvent &event)
{
    HideShowSQLBox( event.IsChecked() );
}

void DrawingView::OnShowDataTypes(wxCommandEvent &event)
{
    m_canvas->ShowHideTablePart( 1, event.IsChecked() );
    auto item = m_frame->GetMenuBar()->FindItem( wxID_SHOWDATATYPES );
    item->Toggle();
    ChangeTableTypeMMenu();
}

void DrawingView::OnShowComments(wxCommandEvent &event)
{
    m_canvas->ShowHideTablePart( 4, event.IsChecked() );
    auto item = m_frame->GetMenuBar()->FindItem( wxID_SHOWCOMMENTS );
    item->Toggle();
    ChangeTableCommentsMenu();
}

void DrawingView::OnConvertToSyntaxUpdateUI(wxUpdateUIEvent &event)
{
    if( GetDocument()->GetQueryFields().size() > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}

void DrawingView::OnConvertToSyntax(wxCommandEvent &WXUNUSED(event))
{
    m_queryType = QuerySyntaxMenu;
//    CreQueryMenu( m_queryType );
    m_designCanvas->Show( false );
    m_fields->Show( false );
    m_canvas->Show( false );
    m_queryBook->Show( false );
    m_edit->Show( true );
    sizer->Layout();
    m_frame->Layout();
    m_edit->SetFocus();
}

void DrawingView::OnConvertToGraphics(wxCommandEvent &WXUNUSED(event))
{
    m_queryType = SQLSelectMenu;
//    SetQueryMenu( m_queryType );
    m_designCanvas->Show( true );
    m_fields->Show( true );
    m_canvas->Show( true );
    m_queryBook->Show( true );
    m_edit->Show( false );
    sizer->Layout();
    m_frame->Layout();
}

void DrawingView::OnCut(wxCommandEvent &WXUNUSED(event))
{
    m_edit->Cut();
}

void DrawingView::OnCopy(wxCommandEvent &WXUNUSED(event))
{
    m_edit->Copy();
}

void DrawingView::OnPaste(wxCommandEvent &WXUNUSED(event))
{
    m_edit->Paste();
}

void DrawingView::OnClear(wxCommandEvent &WXUNUSED(event))
{
    m_edit->Clear();
}

void DrawingView::OnSelectAll(wxCommandEvent &WXUNUSED(event))
{
    m_edit->SelectAll();
}

void DrawingView::OnUndo(wxCommandEvent &WXUNUSED(event))
{
    m_edit->Undo();
}

void DrawingView::OnFind(wxCommandEvent &event)
{
    m_data.SetFlags( wxFR_DOWN );
    m_findDlg = new wxFindReplaceDialog( m_edit, &m_data, _( "Find Text" ), event.GetId() == wxID_REPLACE ? wxFR_REPLACEDIALOG : 0 );
    m_findDlg->Show( true );
//    if( dlg )
}

void DrawingView::OnFindReplaceText(wxFindDialogEvent &event)
{
    wxEventType type = event.GetEventType();
    m_searchFlags = 0, m_start = m_searchPos, m_end = m_edit->GetTextLength();
    m_stringToFind = event.GetFindString();
    int flags = event.GetFlags();
    if( flags & wxFR_WHOLEWORD )
        m_searchFlags = wxSTC_FIND_WHOLEWORD;
    if( !( flags & wxFR_DOWN ) )
    {
        m_searchDirection = 0;
        std::swap( m_start, m_end );
    }
    else if( type == wxEVT_FIND_NEXT )
        m_start = m_start + m_stringToFind.length();
    if( flags & wxFR_MATCHCASE )
        m_searchFlags |= wxSTC_FIND_MATCHCASE;
    if( type == wxEVT_FIND || type == wxEVT_FIND_NEXT )
    {
        FindTextInEditor();
    }
    else if( type == wxEVT_FIND_REPLACE || type == wxEVT_FIND_REPLACE_ALL )
    {
        wxString repString = event.GetReplaceString();
        FindTextInEditor();
        if( m_searchPos != wxSTC_INVALID_POSITION )
        {
            m_edit->SetTargetRange( m_searchPos, m_searchPos + m_stringToFind.length() );
            m_edit->ReplaceTarget( repString );
        }
    }
}

void DrawingView::OnFindNext(wxCommandEvent &WXUNUSED(event))
{
    if( m_searchDirection )
        m_start += 1;
    FindTextInEditor();
}

void DrawingView::FindTextInEditor()
{
    m_searchPos = m_edit->FindText( m_start, m_end, m_stringToFind, m_searchFlags );
    if( m_searchPos != wxSTC_INVALID_POSITION )
    {
        m_edit->SetSelection( m_searchPos, m_searchPos + m_stringToFind.length() );
        m_start = m_searchPos;
    }
}

void DrawingView::OnGotoLine(wxCommandEvent &WXUNUSED(event))
{
    int lineNo, res;
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
        GOTOLINE func = (GOTOLINE) lib.GetSymbol( "GotoLine" );
        res = func( m_frame, lineNo );
        if( res == wxID_OK && lineNo <= m_edit->GetLineCount() )
            m_edit->GotoLine( lineNo );
    }
}

void DrawingView::PopuateQueryCanvas()
{
    wxMenuBar *menuBar = m_parent->GetMenuBar();
    if( GetDocument()->GetQueryFields().empty() )
    {
        menuBar->Enable( 1, false );
    }
    m_designCanvas->PopulateQueryCanvas( GetDocument()->GetQueryFields(), GetDocument()->GetGroupFields() );
}

void DrawingView::OnIconise(wxIconizeEvent &event)
{
    wxView *iconizedView = GetDocumentManager()->GetCurrentView();
    if( event.IsIconized () )
    {
        if( iconizedView == this )
            m_tb->Hide();
        else
            m_tb->ClearTools();
    }
    else
    {
        m_tb->Show();
    }
    event.Skip();
}

void DrawingView::OnTableDataEdit(wxCommandEvent &event)
{
    MyErdTable *erdTable = (MyErdTable *) event.GetEventObject();
    DATAEDITWINDOW func = (DATAEDITWINDOW) m_painters["EditData"]->GetSymbol( "CreateDataEditWindow" );
    func( m_parent, GetDocumentManager(), GetDocument()->GetDatabase(), erdTable->GetTableName() );
}

void DrawingView::CreateDBMenu()
{
    auto mbar = new wxMenuBar;
    auto fileMenu = new wxMenu;
    fileMenu->Append( wxID_CLOSE, _( "&Close\tCtrl+W" ), _( "Close Database Window" ) );
    fileMenu->AppendSeparator();
    fileMenu->Append( wxID_CREATEDATABASE, _( "Create Database..." ), _( "Create Database" ) );
    fileMenu->Append( wxID_ATTACHDATABASE, _( "Attach Database..." ), _( "Attach Database" ) );
    fileMenu->Append( wxID_DELETEDATABASE, _( "Delete Database..." ), _( "Delete Database" ) );
    if( GetDocument()->GetDatabase()->GetTableVector().GetDatabaseType() == L"SQLite" )
        fileMenu->Append( wxID_DETACHDATABASE, _( "Detach Database" ), _( "Detach Database" ) );
    fileMenu->AppendSeparator();
    fileMenu->Append( wxID_EXIT );
    mbar->Insert( 0, fileMenu, _( "File" ) );
    auto menuObject = new wxMenu();
    menuObject->Append( wxID_SELECTTABLE, _( "Select Table..." ), _( "Select tables" ) );
    auto menuNewObject = new wxMenu();
    menuNewObject->Append( wxID_OBJECTNEWTABLE, _( "Table..." ), _( "New Table" ) );
    menuNewObject->Append( wxID_OBJECTNEWINDEX, _( "Index..." ), _( "New Index" ) );
    menuNewObject->Append( wxID_OBJECTNEWVIEW, _( "View" ), _( "New View" ) );
    menuNewObject->Append( wxID_OBJECTNEWFF, _( "Foreign Key..." ), _( "New Foreign Key" ) );
    menuObject->AppendSubMenu( menuNewObject, _( "New" ), _( "New Object" ) );
    menuObject->Append( wxID_DROPOBJECT, _( "Drop" ), _( "Drop database object" ) );
    menuObject->AppendSeparator();
    menuObject->Append( wxID_EXPORTSYNTAX, _( "Export Syntax to Log..." ), _( "Export" ) );
    menuObject->Append( wxID_PROPERTIES, _( "Properties..." ), _( "Properties" ) );
    mbar->Insert( 1, menuObject, _( "&Object" ) );
    auto menuDesign = new wxMenu();
    menuDesign->Append( wxID_STARTLOG, _( "Start Log" ), _( "Start log" ) );
    menuDesign->Append( wxID_STOPLOG, _( "Stop Log" ), _( "Stop log" ) );
    menuDesign->Append( wxID_SAVELOG, _( "Save Log As..." ), _( "Save log to disk file" ) );
    menuDesign->Append( wxID_CLEARLOG, _( "Clear Log" ), _( "Discard content of the log" ) );
    menuDesign->AppendSeparator();
    mbar->Insert( 2, menuDesign, _( "&Design" ) );
    auto helpMenu = new wxMenu;
    helpMenu->Append( wxID_HELP, _( "Help" ), _( "Help" ) );
    mbar->Append( helpMenu, _( "Help" ) );
    m_frame->SetMenuBar( mbar );
}

void DrawingView::CreateQueryMenu(const int queryType)
{
    wxMenuBar *mbar = nullptr;
    if( queryType == QuerySyntaxMenu )
        mbar = new wxMenuBar;
    else
    {
        mbar = m_frame->GetMenuBar();
        for( size_t i = mbar->GetMenuCount() - 1; i >= 0; --i )
        {
            wxMenu *tmp = mbar->Remove( i );
            delete tmp;
            tmp = nullptr;
            if( i == 0 )
                break;
        }
    }
    auto fileMenu = new wxMenu;
    fileMenu->Append( wxID_CLOSE, _( "&Close\tCtrl+W" ), _( "Close Database Window" ) );
    fileMenu->AppendSeparator();
    fileMenu->Insert( 0, wxID_NEW, _( "New...\tCtrl+N" ), _( "Create new query" ) );
    fileMenu->Insert( 1, wxID_OPEN, _( "Open...\tCtrl+O" ), _( "Open an existing query" ) );
    fileMenu->Append( wxID_SAVEQUERY, _( "Save\tCtrl+S" ), _( "Save the query" ) );
    fileMenu->Append( wxID_SAVEQUERYAS, _( "Save Query As..." ), _( "Save Query As" ) );
    fileMenu->AppendSeparator();
    auto helpMenu = new wxMenu;
    helpMenu->Append( wxID_HELP, _( "Help" ) );
    if( queryType == SQLSelectMenu )
    {
        auto *designMenu = new wxMenu;
        designMenu->Append( wxID_DATASOURCE, _( "Data Source" ), _( "Data Source" ), wxITEM_CHECK );
        designMenu->Append( wxID_PREVIEDWQUERY, _( "Preview" ), _( "Preview" ) );
        designMenu->AppendSeparator();
        designMenu->Append( wxID_SELECTTABLE, _( "Select Table..." ) );
        designMenu->AppendSeparator();
        designMenu->Append( wxID_RETRIEVEARGS, _( "Retieval Arguments..." ), _( "Define Retrieval Arguments" ) );
        designMenu->Append( wxID_DISTINCT, _( "Distinct" ), _( "Return distinct rows only" ), wxITEM_CHECK );
        designMenu->AppendSeparator();
        designMenu->Append( wxID_CONVERTTOSYNTAX, _( "Convert To Syntax" ), _( "Convert To Syntax" ) );
        auto show = new wxMenu;
        show->Append( wxID_SHOWDATATYPES, _( "Datatypes" ), _( "Show Datatypes" ), wxITEM_CHECK );
        show->Append( wxID_SHOWLABELS, _( "Labels" ), _( "Show Labels" ), wxITEM_CHECK );
        show->Append( wxID_SHOWCOMMENTS, _( "Comments" ), _( "Show Comments" ), wxITEM_CHECK );
        show->Append( wxID_SHOWSQLTOOLBOX, _( "SQL Toolbox" ), _( "SQL Toolbox" ), wxITEM_CHECK );
        show->Append( wxID_SHOWJOINS, _( "Joins" ), _( "Show Joins" ), wxITEM_CHECK );
        designMenu->AppendSubMenu( show, _( "Show" ) );
        designMenu->Check( wxID_DATASOURCE, true );
        show->Check( wxID_SHOWDATATYPES, true );
        show->Check( wxID_SHOWLABELS, true );
        show->Check( wxID_SHOWCOMMENTS, true );
        show->Check( wxID_SHOWSQLTOOLBOX, true );
        show->Check( wxID_SHOWJOINS, true );
        if( GetSyntaxPage()->GetSyntaxCtrl()->GetValue().find( "DISTNCT" ) != wxNOT_FOUND )
            designMenu->Check( wxID_DISTINCT, true );
        mbar->Insert( 0, designMenu, _( "Design" ) );
    }
    if( queryType == QuerySyntaxMenu )
    {
        auto editMenu = new wxMenu;
        editMenu->Append( wxID_UNDO, _( "Undo\tCtrl+Z" ), _( "Undo" ) );
        editMenu->AppendSeparator();
        editMenu->Append( wxID_CUT, _( "Cut\tCtrl+X" ), _( "Cut" ) );
        editMenu->Append( wxID_COPY, _( "Copy\tCtrl+C" ), _( "Copy" ) );
        editMenu->Append( wxID_PASTE, _( "Paste\tCtrl+V" ), _( "Paste" ) );
        editMenu->Append( wxID_CLEAR, _( "Clear\tDel" ), _( "Clear" ) );
        editMenu->AppendSeparator();
        editMenu->Append( wxID_SELECTALL, _( "Select all\tCtrl+A" ), _( "Select all" ) );
        auto searchMenu = new wxMenu;
        searchMenu->Append( wxID_FIND, _( "Find Text...\tCtrl+F" ), _( "Find text" ) );
        searchMenu->Append( myID_FINDNEXT, _( "Find Next Text\tCtrl+N" ), _( "Find next text" ) );
        searchMenu->Append( wxID_REPLACE, _( "Replace Text..." ), _( "Replace text" ) );
        searchMenu->AppendSeparator();
        searchMenu->Append( wxID_GOTOLINE, _( "Go to Line...\tCtrl+L" ), _( "Go to line" ) );
        auto designMenu = new wxMenu;
        designMenu->Append( wxID_UNDOALL, _( "Undo All" ), _( "Undo All" ) );
        designMenu->AppendSeparator();
        designMenu->AppendCheckItem( wxID_DATASOURCE, "Data Source", "Data Source" );
        designMenu->Check( wxID_DATASOURCE, true );
        designMenu->AppendSeparator();
        designMenu->Append( wxID_RETRIEVEARGS, _( "Retrieval Arguments..." ), _( "Retrieval Arguments" ) );
        designMenu->Append( wxID_CONVERTTOGRAPHICS, _( "Convert To Graphics" ), _( "Convert To Graphics" ) );
        designMenu->AppendSeparator();
        designMenu->Append( wxID_PROPERTIES, _( "Options..."), _( "Options" ) );
        mbar->Append( editMenu, _( "Edit" ) );
        mbar->Append( searchMenu, _( "Search" ) );
        mbar->Append( designMenu, _( "Design" ) );
    }
    mbar->Insert( 0, fileMenu, _( "File" ) );
    mbar->Append( helpMenu, _( "Help" ) );
    if( queryType == QuerySyntaxMenu )
        m_frame->SetMenuBar( mbar );
    if( queryType == QuerySyntaxMenu )
    {
        mbar->EnableTop( 0, false );
        mbar->EnableTop( 1, false );
        mbar->EnableTop( 2, false );
        mbar->EnableTop( 3, false );
        mbar->EnableTop( 4, false );
    }
}

void DrawingView::OnFieldShuffle(wxCommandEvent &event)
{
    wxString replacement = " ";
    std::vector<wxString> *fields = reinterpret_cast<std::vector<wxString> *>( event.GetClientObject() );
    for( std::vector<wxString>::iterator it = fields->begin (); it < fields->end (); ++it )
    {
        replacement += (*it);
        if( it != fields->end() - 1 )
            replacement += ", ";
        else
#ifdef __WXMSW__
            replacement += " \n\r";
#else
            replacement += "\n";
#endif
    }
    wxString origQuery = m_page6->GetSyntaxCtrl()->GetValue();
    wxString temp = origQuery.substr( origQuery.Find( " " ), origQuery.Find( "FROM" ) - 6 );
    origQuery.Replace( temp, replacement, true );
    m_page6->SetSyntaxText( origQuery );
    dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->SetQuery( origQuery );
}

void DrawingView::DropTableFromQeury(const wxString &name)
{
    wxString replace;
    if( name.IsEmpty() )
    {
        DrawingDocument *doc = wxDynamicCast( GetDocument(), DrawingDocument );
        GetSortPage()->GetSortSourceList()->DeleteAllItems();
        GetSortPage()->GetSourceDestList()->DeleteAllItems();
        GetGroupByPage()->GetSourceList()->DeleteAllItems();
        GetGroupByPage()->GetDestList()->DeleteAllItems();
        GetWherePage()->GetGrid()->ClearGrid();
        GetHavingPage()->GetGrid()->ClearGrid();
        GetSyntaxPage()->ClearQuery();
        m_fields->GetDiagramManager()->Clear();
        GetDocument()->ClearQueryFields();
        m_selectTableName.clear();
        GetDocument()->ClearGroupByVector();
        GetDocument()->ClearSortedVector();
        m_arguments.clear();
        GetTablesForView( doc->GetDatabase(), false, m_queries, m_path );
        dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->SetQuery( "" );
    }
    else
    {
        GetDocument()->DeleteQueryFieldForTable( name, replace );
        if( !replace.IsEmpty() )
        {
            m_selectTableName.erase( std::remove_if( m_selectTableName.begin(), m_selectTableName.end(), 
            [&name](DatabaseTable *tbl)
            {
                return tbl->GetTableName() == name;
            } ), m_selectTableName.end() );
            GetDocument()->DeleteGroupByTable( name, replace );
            GetDocument()->DeleteSortedTable( name, replace );
        }
    }
}

void DrawingView::ChangeTableTypeMMenu()
{
    auto showTypes = m_frame->GetMenuBar()->FindItem( wxID_SHOWDATATYPES );
    showTypes->Check( !showTypes->IsChecked() );
}

void DrawingView::ChangeTableCommentsMenu()
{
    auto showComments = m_frame->GetMenuBar()->FindItem( wxID_SHOWCOMMENTS );
    showComments->Check( !showComments->IsChecked() );
}

void DrawingView::OnExportSyntax(wxCommandEvent &event)
{
    auto tables = dynamic_cast<DrawingDocument *>( GetDocument() )->GetDBTables();
    wxString source;
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
        GETDATASOURCE func = (GETDATASOURCE) lib.GetSymbol( "GetDataSource" );
        int res = func( m_frame, source, m_profiles );
        if( res == wxID_OK )
        {
            std::vector<std::wstring> errors;
            for( std::vector<DatabaseTable *>::iterator it = tables.begin(); it < tables.end(); ++it )
            {
                std::wstring syntax;
                dynamic_cast<DrawingDocument *>( GetDocument() )->GetDatabase()->GetTableCreationSyntax( (*it)->GetSchemaName() + L"." + (*it)->GetTableName(), syntax, errors );

            }
        }
    }
}

void DrawingView::OnQuerySave(wxCommandEvent &WXUNUSED(event))
{
    wxDynamicLibrary lib;
    wxString documentName = "";
#ifdef __WXMSW__
    lib.Load( "dialogs" );
#elif __WXMAC__
    lib.Load( "liblibdialogs.dylib" );
#else
    lib.Load( "libdialogs" );
#endif
    if( lib.IsLoaded() )
    {
        bool update;
        CHOOSEOBJECT func = (CHOOSEOBJECT) lib.GetSymbol( "ChooseObject" );
        int res = func( m_frame, -1, m_queries, documentName, m_path, update );
        if( res == wxID_OK )
        {
            wxString libName;
            auto found = false;
            for( std::vector<LibrariesInfo>::iterator it = m_path.begin(); it < m_path.end() && !found; ++it )
            {
                if( (*it).m_isActive )
                {
                    libName = (*it).m_path;
                    found = true;
                }
            }
            dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->DeleteSortDestItems();
            for( auto i = 0; i < m_page1->GetSourceDestList()->GetItemCount(); ++i )
            {
                auto name = m_page1->GetSourceDestList()->GetTextValue( i, 0 );
                auto direction = m_page1->GetSourceDestList()->GetToggleValue( i, 1 );
                auto sourcePos = m_page1->GetSourceDestList()->GetItemData( m_page1->GetSourceDestList()->RowToItem( i ) );
                dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->AddSortDest( wxString::Format( "%s %d %ld", name, direction ? 1 : 0, sourcePos ) );
            }
            dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->DeleteAllWhereLines();
            for( auto i = 0; i < m_page2->GetGrid()->GetNumberRows(); ++i )
            {
                auto var = m_page2->GetGrid()->GetCellValue( i, 0 );
                auto sign = m_page2->GetGrid()->GetCellValue( i, 1 );
                auto value = m_page2->GetGrid()->GetCellValue( i, 2 );
                auto condition = m_page2->GetGrid()->GetCellValue( i, 3 );
                dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->AddWhereLines( wxString::Format( "%d,%s,%s,%s,%s", i, var, sign, value, condition ) );
            }
            GetDocument()->SetFilename( documentName + ".qry" );
            if( GetDocument()->SaveNewQuery( libName, m_queries, documentName + ".qry", update ) ) 
            {

            }
        }
    }
}

void DrawingView::OnQuerySaveUpdateUI(wxUpdateUIEvent &event)
{
    if( GetDocument()->GetQueryFields().size() > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}

void DrawingView::OnQuerySaveAsUpdateUI(wxUpdateUIEvent &event)
{
    if( GetDocument()->GetQueryFields().size() > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}

std::map<wxString, std::vector<TableDefinition> > &DrawingView::GetTablesMap() 
{
    return m_tables;
}

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
#include "../dbhandler/res/quit.xpm"
#include "res/gui/toolbox.xpm"
//#include "./res/gui/bold_png.c"
//#endif

#if defined( __WXGTK__ ) || defined( __WXQT__ )
#ifndef __WXQT__
#include "pango/pango.h"
#endif
#include "bold.h"
#include "italic.h"
#include "underline.h"
#include "centeralign.h"
#include "leftalign.h"
#include "rightalign.h"
#include "table_svg.h"
#include "commenttext.h"
#include "fontname.h"
#include "fontsize.h"
#include "joins.h"
#include "createview.h"
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
#ifdef __WXQT__
#include <QFontDialog>
#endif
#include "wx/artprov.h"
#include "wx/any.h"
#include "wx/fontenum.h"
#include "wx/notebook.h"
#include "wx/fdrepdlg.h"
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/cmdproc.h"
#include "wx/stdpaths.h"
#include "wx/bmpcbox.h"
#include "wx/grid.h"
#include "wx/stc/stc.h"
#include "wx/listctrl.h"
#include "wx/dataview.h"
#include "wx/renderer.h"
#include "wx/config.h"
#include "wx/colordlg.h"
#include "wx/filepicker.h"
#include "wx/propgrid/propgrid.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "guiobjectsproperties.h"
#include "configuration.h"
#include "propertieshandlerbase.h"
#include "guiobjectsproperties.h"
#include "ablbaseview.h"
#include "dbview.h"
#include "colorcombobox.h"
#include "taborder.h"
#include "designlabel.h"
#include "designfield.h"
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
#include "propertypagebase.h"
#include "printspec.h"
#include "tablegeneral.h"
#include "pointerproperty.h"
#include "tableprimarykey.h"
#include "fieldgeneral.h"
#include "fontpropertypagebase.h"
#include "fieldheader.h"
#include "propertieshandlerbase.h"
#include "divider.h"
#include "designgeneral.h"
#include "bandgeneral.h"
#include "designcanvas.h"
#include "databaseview.h"
#include "databasetemplate.h"

const wxEventTypeTag<wxCommandEvent> wxEVT_SET_TABLE_PROPERTY( wxEVT_USER_FIRST + 1 );
const wxEventTypeTag<wxCommandEvent> wxEVT_SET_FIELD_PROPERTY( wxEVT_USER_FIRST + 2 );
const wxEventTypeTag<wxCommandEvent> wxEVT_CHANGE_QUERY( wxEVT_USER_FIRST + 3 );
const wxEventTypeTag<wxCommandEvent> wxEVT_FIELD_SHUFFLED( wxEVT_USER_FIRST + 4 );

typedef int (*TABLESELECTION)(wxDocMDIChildFrame *, Database *, std::map<wxString, std::vector<TableDefinition> > &, std::vector<std::wstring> &, bool, const int, bool);
typedef int (*CREATEINDEX)(wxWindow *, DatabaseTable *, Database *, wxString &, wxString &);
typedef int (*CREATEPROPERTIESDIALOG)(wxWindow *parent, PropertiesHandler *, const wxString &, wxString &, DatabaseTable *, bool &);
typedef int (*CREATEPROPERTIESDIALOGFRPRJECT)(wxWindow *parent, std::unique_ptr<PropertiesHandler> &, const wxString &);
typedef int (*CREATEFOREIGNKEY)(wxWindow *parent, DatabaseTable *, std::vector<FKField *> &, Database *, bool &, bool, std::vector<FKField *> &, int &);
typedef void (*TABLE)(wxWindow *, wxDocManager *, Database *, DatabaseTable *, const wxString &);
typedef int (*CHOOSEOBJECT)(wxWindow *, int, std::vector<QueryInfo> &, wxString &, std::vector<LibrariesInfo> &path, bool &update);
typedef int (*NEWQUERY)(wxWindow *, int &, int &);
typedef int (*QUICKSELECT)(wxWindow *, const Database *, std::vector<DatabaseTable *> &, const std::vector<TableField *> &, std::vector<FieldSorter> &allSorted, std::vector<FieldSorter> &qerySorted);
typedef Database *(*DBPROFILE)(wxWindow *, const wxString &, wxString &, const std::wstring &);
typedef int (*RETRIEVEARGUMENTS)(wxWindow *, std::vector<QueryArguments> &arguments, const wxString &, const wxString &);
typedef int (*GOTOLINE)(wxWindow *, int &);
typedef void (*DATAEDITWINDOW)(wxWindow *parent, wxDocManager *docManager, Database *db, const wxString &);
typedef int (*GETDATASOURCE)(wxWindow *parent, wxString &sorce, const std::vector<Profile> &);
typedef int (*CREATEVIEWOPTIONS)(wxWindow *, const Database *, NewViewOptions &);
typedef int (*SAVENEWVIEW)(wxWindow *, wxString &);
typedef int (*CREATETABLESPACE)(wxWindow *);

#if _MSC_VER >= 1900 || !(defined __WXMSW__)
std::mutex Impl::my_mutex;
#endif

#define wxID_NEWOBJECT 1000

// ----------------------------------------------------------------------------
// DrawingView implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(DrawingView, wxView);

wxBEGIN_EVENT_TABLE(DrawingView, wxView)
    EVT_MENU(wxID_CLOSE, DrawingView::OnClose)
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
    EVT_UPDATE_UI(wxID_DATASOURCE, DrawingView::OnDataSourceUpdateUI)
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
    EVT_MENU(wxID_OBJECTNEWVIEW, DrawingView::OnDatabaseCreateView)
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
    EVT_MENU(wxID_CUSTOMCOLORS, DrawingView::OnCustmColors)
    EVT_MENU(wxID_DATABASEOPTIONS, DrawingView::OnDatabasePreferences)
    EVT_MENU(wxID_CREATETABLESPACE, DrawingView::OnCreateTableSpace)
    EVT_MENU(wxID_EDITTABLEOBJECT, DrawingView::OnEditTableObject)
wxEND_EVENT_TABLE()

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool DrawingView::OnCreate(wxDocument *doc, long flags)
{
    m_designCanvas = nullptr;
    m_edit = nullptr;
    m_searchDirection = 1;
    m_log = nullptr;
    m_searchPos = 0;
    m_isActive = false;
    m_tb = m_styleBar = nullptr;
    m_isCreated = false;
    m_fields = nullptr;
    m_queryBook = nullptr;
    m_page2 = m_page4 = nullptr;
    m_page1 = m_page3 = nullptr;
    m_page6 = nullptr;
    m_fieldText = nullptr;
    m_fontName = nullptr;
    m_fontSize = nullptr;
    m_dbFrame = nullptr;
    if( !wxView::OnCreate( doc, flags ) )
        return false;
    if( m_type != DatabaseView )
    {
        m_fontSizes.push_back( "8" );
        m_fontSizes.push_back( "9" );
        m_fontSizes.push_back( "10" );
        m_fontSizes.push_back( "11" );
        m_fontSizes.push_back( "12" );
        m_fontSizes.push_back( "14" );
        m_fontSizes.push_back( "16" );
        m_fontSizes.push_back( "18" );
        m_fontSizes.push_back( "20" );
        m_fontSizes.push_back( "22" );
        m_fontSizes.push_back( "24" );
        m_fontSizes.push_back( "26" );
        m_fontSizes.push_back( "28" );
        m_fontSizes.push_back( "36" );
        m_fontSizes.push_back( "48" );
        m_fontSizes.push_back( "72" );
    }
    wxRect clientRect = m_parent->GetClientRect();
    wxString title;
    if( m_type == QueryView )
    {
        title = _( "Query - (untitled)" );
    }
    else if( m_type == NewViewView )
    {
        title = _( "View -" );
    }
    else
    {
        title = L"Database - " + wxDynamicCast( GetDocument(), DrawingDocument )->GetDatabase()->GetTableVector().m_dbName;
    }
    wxPoint pos;
#ifdef __WXOSX__
    pos.y = ( m_parent->GetClientWindow()->GetClientRect().GetHeight() - m_parent->GetClientRect().GetHeight() ) - m_parent->GetToolBar()->GetRect().GetHeight();
#else
    pos = wxDefaultPosition;
#endif
    m_frame = new wxDocMDIChildFrame( doc, this, m_parent, wxID_ANY, title, pos, wxSize( clientRect.GetWidth(), clientRect.GetHeight() ) );
//    m_frame->SetMenuBar( m_parent->GetMenuBar() );
    auto id = wxPlatformInfo::Get().GetOperatingSystemId();
    if( m_type == DatabaseView )
    {
        m_log = new wxFrame( m_frame, wxID_ANY, _( "Activity Log" ), wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT );
        m_text = new wxTextCtrl( m_log, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY );
        auto font = m_text->GetFont();
        if( id & wxOS_WINDOWS )
        {
            font.SetFaceName( "MS Sans Seriff" );
            font.SetPointSize( 8 );
        }
        else if( id & wxOS_MAC )
        {
            font.SetFaceName( "MS Sans Seriff" );
            font.SetPointSize( 8 );
        }
        else
        {
#ifdef __WXGTK__
            font.SetFaceName( "Cantarell" );
            font.SetPointSize( 11 );
#elif __WXQT__
            font.SetFaceName( "Mono Seriff" );
            font.SetPointSize( 10 );
#endif
        }
        m_text->SetFont( font );
    }
    sizer = new wxBoxSizer( wxVERTICAL );
//    sizer->Layout();
//    m_frame->Layout();
//    m_frame->Show();
    wxPoint ptCanvas;
#ifndef __WXOSX__
    ptCanvas = wxDefaultPosition;
#else
    ptCanvas.x = 0;
    ptCanvas.y = ( m_parent->GetClientWindow()->GetClientRect().GetWidth() - m_parent->GetClientRect().GetWidth() );
    if( m_styleBar )
        ptCanvas.y += m_styleBar->GetSize().y;
#endif
    wxASSERT( m_frame == GetFrame() );
    if( m_type != DatabaseView )
    {
        m_fields = new FieldWindow( m_frame, false, wxDefaultPosition, wxDefaultCoord );
        m_fields->SetCursor( wxCURSOR_HAND );
        sizer->Add( m_fields, 0, wxEXPAND, 0 );
    }
    auto db = ((DrawingDocument *) GetDocument() )->GetDatabase();
    m_canvas = new DatabaseCanvas( this, ptCanvas, db->GetTableVector().m_dbName, db->GetTableVector().m_type, m_conf, m_type );
    m_canvas->SetName( "dbcanvas" );
    sizer->Add( m_canvas, 2, wxEXPAND, 0 );
    if( m_type != DatabaseView )
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
    }
    if( m_type == QueryView )
    {
        m_designCanvas = new DesignCanvas( this, ptCanvas, m_conf );
        sizer->Add( m_designCanvas, 1, wxEXPAND, 0 );
        m_canvas->Show( false );
        m_edit = new wxStyledTextCtrl( m_frame );
        std::wstring type = GetDocument()->GetDatabase()->GetTableVector().GetDatabaseType();
        std::wstring subtype = GetDocument()->GetDatabase()->GetTableVector().GetDatabaseSubtype();
        if( type == L"MySQL" || ( type == L"ODBC" && subtype == L"MySQL" ) )
            m_edit->SetLexer( wxSTC_LEX_MYSQL );
        if( type == L"Microsoft SQL Server" || ( type == L"ODBC" && subtype == L"Microsoft SQL Server" ) )
            m_edit->SetLexer( wxSTC_LEX_MSSQL );
        if( type == L"SQLite" )
            m_edit->SetLexer( wxSTC_LEX_SQL );
        sizer->Add( m_edit, 1, wxEXPAND, 0 );
        m_edit->Show( false );
    }
    m_frame->SetSizer( sizer );
    CreateViewToolBar();
    if( m_log )
        m_log->Bind( wxEVT_CLOSE_WINDOW, &DrawingView::OnCloseLogWindow, this );
    Bind( wxEVT_SET_TABLE_PROPERTY, &DrawingView::OnSetProperties, this );
    Bind( wxEVT_SET_FIELD_PROPERTY, &DrawingView::OnSetProperties, this );
    Bind( wxEVT_MENU, &DatabaseCanvas::OnDropTable, m_canvas, wxID_DROPOBJECT );
    Bind( wxEVT_FIELD_SHUFFLED, &DrawingView::OnFieldShuffle, this );
    m_frame->Bind( wxEVT_CHANGE_QUERY, &DrawingView::OnQueryChange, this );
    m_frame->Bind( wxEVT_ICONIZE, &DrawingView::OnIconise, this );
    if( m_fontName )
        m_fontName->Bind( wxEVT_COMBOBOX, &DrawingView::OnFontSeectionChange, this );
    sizer->Layout();
    m_frame->Layout();
    m_frame->Show();
    m_initialized = true;
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

void DrawingView::OnClose(wxCommandEvent &WXUNUSED(event))
{
    if( m_type == NewViewView )
    {
        if( GetDocument()->GetQueryFields().size() > 0 )
        {
            auto viewCommand = m_page6->GetSyntaxCtrl()->GetValue();
            wxString name, libName;
            wxDynamicLibrary lib1;
#ifdef __WXMSW__
            libName = m_libPath + "dialogs";
#elif __WXOSX__
            libName = m_libPath + "liblibdialogs.dylib" ;
#else
            libName = m_libPath + "libdialogs";
#endif
            lib1.Load( libName );
            if( lib1.IsLoaded() )
            {
                SAVENEWVIEW func = (SAVENEWVIEW) lib1.GetSymbol( "SaveNewView" );
                int res = func( m_parent, name );
                if( !name.IsEmpty() )
                    viewCommand.Replace( "Untitled", name );
                if( res == wxID_OK )
                {
                    wxMessageBox( "Executing " + viewCommand );
                }
                else if( res != wxID_CANCEL )
                {
                    m_text->AppendText( viewCommand );
                }
            }
        }
        m_dbFrame->Show( true );
    }
    else
    {
        if( OnClose( true ) )
            m_frame->Close();
    }
}

void DrawingView::CreateViewToolBar()
{
    auto size = m_parent->GetClientSize();
    long styleViewBar = wxNO_BORDER | wxTB_FLAT, styleStyleBar = wxNO_BORDER | wxTB_FLAT;
    switch( m_tbSetup[0].m_orientation )
    {
        case 0:
            styleViewBar |= wxTB_VERTICAL;
            break;
        case 1:
            styleViewBar |= wxTB_HORIZONTAL;
            break;
        case 2:
            styleViewBar |= wxTB_RIGHT;
            break;
        case 3:
            styleViewBar |= wxTB_BOTTOM;
            break;
    }
    if( !m_tbSetup[0].m_showTooltips )
        styleViewBar |= wxTB_NO_TOOLTIPS;
    if( m_tbSetup[0].m_showText )
        styleViewBar |= wxTB_TEXT ;
    switch( m_tbSetup[1].m_orientation )
    {
        case 0:
            styleStyleBar |= wxTB_VERTICAL;
            break;
        case 1:
            styleStyleBar |= wxTB_HORIZONTAL;
            break;
        case 2:
            styleStyleBar |= wxTB_RIGHT;
            break;
        case 3:
            styleStyleBar |= wxTB_BOTTOM;
            break;
    }
    if( !m_tbSetup[1].m_showTooltips )
        styleStyleBar |= wxTB_NO_TOOLTIPS;
    if( m_tbSetup[1].m_showText )
        styleStyleBar |= wxTB_TEXT ;
    wxWindow *parent = nullptr;
#ifdef __WXOSX__
    parent = m_frame;
#else
    parent = m_parent;
#endif
#ifdef __WXOSX__
    m_tb = m_frame->CreateToolBar();
    m_tb->SetName( "ViewBar" );
    if( m_type == QueryView )
    {
        m_styleBar = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, styleStyleBar, "StyleBar" );
    }
#else
    if( m_type != NewViewView )
    {
        if( !m_tb )
        {
            m_tb = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, styleViewBar, "ViewBar" );
        }
        else
            m_tb->ClearTools();
    }
    if( m_type == QueryView )
    {
        if( !m_styleBar )
            m_styleBar = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, styleStyleBar, "StyleBar" );
        else
            m_styleBar->ClearTools();
    }
    if( m_type == NewViewView )
    {
        m_tb->ClearTools();
    }
#endif
    if( m_type == DatabaseView )
    {
        CreateDBMenu();
        m_tb->AddTool( wxID_DATABASEWINDOW, _( "Database Profile" ), wxBitmap( database_profile ), wxBitmap( database_profile ), wxITEM_NORMAL, _( "DB Profile" ), _( "Select database profile" ) );
        wxBitmapBundle tableSVG, createviewSVG;
#ifdef __WXMSW__
        HANDLE gs_wxMainThread = NULL;
        const HINSTANCE inst = wxDynamicLibrary::MSWGetModuleHandle( "dbwindow", &gs_wxMainThread );
        const void* dataTable = nullptr, *createview = nullptr;
        size_t sizeTable = 0, createView = 0;
        if( !wxLoadUserResource( &dataTable, &sizeTable, "table", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            tableSVG = wxBitmapBundle::FromSVG( (const char *) dataTable, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &createview, &createView, "createview", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            createviewSVG = wxBitmapBundle::FromSVG( (const char *) createview, wxSize( 16, 16 ) );
        }
#elif __WXOSX__
        tableSVG = wxBitmapBundle::FromSVGResource( "table", wxSize( 16, 16 ) );
        createviewSVG = wxBitmapBundle::FromSVGResource( "addview", wxSize( 16, 16 ) );
#else
        tableSVG = wxBitmapBundle::FromSVG( table, wxSize( 16, 16 ) );
        createviewSVG = wxBitmapBundle::FromSVG( createview, wxSize( 16, 16 ) );
#endif
        m_tb->AddTool( wxID_SELECTTABLE, _( "Select Table" ), tableSVG, tableSVG, wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" ) );
        m_tb->AddTool( wxID_OBJECTNEWVIEW, _( "Create View" ), createviewSVG, createviewSVG, wxITEM_NORMAL, _( "Create View" ), _( "Creatre a New View" ) );
        m_tb->AddTool( wxID_DROPOBJECT, _( "Drop" ), wxArtProvider::GetBitmapBundle( wxART_DELETE ), wxArtProvider::GetBitmapBundle( wxART_DELETE ), wxITEM_NORMAL, _( "Drop" ), _( "Drop database Object" ) );
        m_tb->AddTool( wxID_PROPERTIES, _( "Properties" ), wxBitmap( properties ), wxBitmap( properties ), wxITEM_NORMAL, _( "Properties" ), _( "Proerties" ) );
        m_tb->AddTool( wxID_CLOSE, _( "Close View" ), wxBitmap( quit_xpm ), wxBitmap( quit_xpm ), wxITEM_NORMAL, _( "Close" ), _( "Close Database View" ) );
    }
    else if( m_type == QueryView )
    {
        if( m_type == QueryView )
            CreateQueryMenu( QuickQueryMenu );
        else
            CreateQueryMenu( SQLSelectMenu );
        wxBitmapBundle save, tableSVG, boldSVG, italicSVG, underlineSVG, leftalignSVG, centeralignSVG, rightalignSVG, commentSVG, fontnameSVG, fontsizeSVG, joinsSVG;
#ifdef __WXMSW__
        HANDLE gs_wxMainThread = NULL;
        const HINSTANCE inst = wxDynamicLibrary::MSWGetModuleHandle( "dbwindow", &gs_wxMainThread );
        const void *data = nullptr, *dataTable = nullptr, *data1 = nullptr, *data2 = nullptr, *data3 = nullptr, *data4 = nullptr, *data5 = nullptr, *data6 = nullptr;
        const void *data7 = nullptr, *data8 = nullptr, *data9 = nullptr, *data10 = nullptr;
        size_t sizeSave = 0, sizeTable = 0, size1 = 0, size2 = 0, size3 = 0, size4 = 0, size5 = 0, size6 = 0;
        size_t size7 = 0, size8 = 0, size9 = 0, size10 = 0;
        if( !wxLoadUserResource( &data, &sizeSave, "save", RT_RCDATA, inst ) )
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
        if( !wxLoadUserResource( &data4, &size4, "leftalign", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            leftalignSVG = wxBitmapBundle::FromSVG( (const char *) data4, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &data5, &size5, "centeralign", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            centeralignSVG = wxBitmapBundle::FromSVG( (const char *) data5, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &data6, &size6, "rightalign", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            rightalignSVG = wxBitmapBundle::FromSVG( (const char *) data6, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &data7, &size7, "comment", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            commentSVG = wxBitmapBundle::FromSVG( (const char *) data7, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &data8, &size8, "fontname", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            fontnameSVG = wxBitmapBundle::FromSVG( (const char *) data8, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &data9, &size9, "fontsize", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            fontsizeSVG = wxBitmapBundle::FromSVG( (const char *) data9, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &data10, &size10, "joins", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            joinsSVG = wxBitmapBundle::FromSVG( (const char *) data10, wxSize( 16, 16 ) );
        }
#elif __WXOSX__
        save = wxBitmapBundle::FromSVGResource( "save", wxSize( 16, 16 ) );
        tableSVG = wxBitmapBundle::FromSVGResource( "table", wxSize( 16, 16 ) );
        boldSVG = wxBitmapBundle::FromSVGResource( "bold", wxSize( 16, 16 ) );
        italicSVG = wxBitmapBundle::FromSVGResource( "italic", wxSize( 16, 16 ) );
        underlineSVG = wxBitmapBundle::FromSVGResource( "underline", wxSize( 16, 16 ) );
        leftalignSVG = wxBitmapBundle::FromSVGResource( "leftalign", wxSize( 16, 16 ) );
        centeralignSVG = wxBitmapBundle::FromSVGResource( "centeralign", wxSize( 16, 16 ) );
        rightalignSVG = wxBitmapBundle::FromSVGResource( "rightalign", wxSize( 16, 16 ) );
        commentSVG = wxBitmapBundle::FromSVGResource( "commenttext", wxSize( 16, 16 ) );
        fontnameSVG = wxBitmapBundle::FromSVGResource( "fontname", wxSize( 16, 16 ) );
        fontsizeSVG = wxBitmapBundle::FromSVGResource( "fontsize", wxSize( 16, 16 ) );
        joinsSVG = wxBitmapBundle::FromSVGResource( "joins", wxSize( 16, 16 ) );
#else
        save = wxArtProvider::GetBitmapBundle( wxART_FLOPPY, wxART_TOOLBAR );
        tableSVG = wxBitmapBundle::FromSVG( table, wxSize( 16, 16 ) );
        boldSVG = wxBitmapBundle::FromSVG( bold, wxSize( 16, 16 ) );
        italicSVG = wxBitmapBundle::FromSVG( italic, wxSize( 16, 16 ) );
        underlineSVG = wxBitmapBundle::FromSVG( underline, wxSize( 16, 16 ) );
        leftalignSVG = wxBitmapBundle::FromSVG( leftalign, wxSize( 16, 16 ) );
        centeralignSVG = wxBitmapBundle::FromSVG( centeralign, wxSize( 16, 16 ) );
        rightalignSVG = wxBitmapBundle::FromSVG( rightalign, wxSize( 16, 16 ) );
        commentSVG = wxBitmapBundle::FromSVG( comment, wxSize( 16, 16 ) );
        fontnameSVG = wxBitmapBundle::FromSVG( fontname, wxSize( 16, 16 ) );
        fontsizeSVG = wxBitmapBundle::FromSVG( fontsize, wxSize( 16, 16 ) );
        joinsSVG = wxBitmapBundle::FromSVG( joins, wxSize( 16, 16 ) );
#endif
        if( m_type != NewViewView )
        {
            m_tb->AddTool( wxID_NEW, _( "New" ), wxArtProvider::GetBitmapBundle( wxART_NEW, wxART_TOOLBAR ), wxArtProvider::GetBitmapBundle( wxART_NEW, wxART_TOOLBAR ), wxITEM_NORMAL, _( "New" ), _( "New Query" ) );
            m_tb->AddTool( wxID_OPEN, _( "Open" ), wxArtProvider::GetBitmapBundle( wxART_FILE_OPEN, wxART_TOOLBAR ), wxArtProvider::GetBitmapBundle( wxART_FILE_OPEN, wxART_TOOLBAR ), wxITEM_NORMAL, _( "Open" ), _( "Open Query" ) );
            m_tb->AddTool( wxID_SAVEQUERY, _( "Save" ), save, save, wxITEM_NORMAL, _( "Save" ), _( "Save Query" ) );
            m_tb->AddTool( wxID_SHOWSQLTOOLBOX, _( "Show ToolBox" ), wxBitmap( toolbox), wxBitmap( toolbox ), wxITEM_CHECK, _( "Toolbox" ), _( "Hide/Show SQL Toolbox" ) );
			m_tb->AddTool( wxID_JOINS, _( "Joins" ), joinsSVG, joinsSVG, wxITEM_NORMAL, _( "Joins" ), _( "Jois" ) );
            m_tb->AddTool( wxID_DATASOURCE, _( "Preview SQL" ), wxBitmap::NewFromPNGData( sql, WXSIZEOF( sql ) ), wxNullBitmap, wxITEM_CHECK, _( "Data Source" ), _( "" ) );
            m_tb->AddTool( wxID_CLOSE, _( "Close View" ), wxBitmap( quit_xpm ), wxBitmap( quit_xpm ), wxITEM_NORMAL, _( "Close" ), _( "Close Query View" ) );
            m_tb->ToggleTool( wxID_SHOWSQLTOOLBOX, true );
            m_tb->InsertTool( 4, wxID_SELECTTABLE, _( "Select Table" ), tableSVG, tableSVG, wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" )  );
            m_tb->InsertTool( 5, wxID_PREVIEDWQUERY, _( "Preview" ), wxBitmap::NewFromPNGData( previewIcon, WXSIZEOF( previewIcon ) ), wxNullBitmap, wxITEM_CHECK, ( "Preview" ) );
            m_tb->ToggleTool( wxID_DATASOURCE, true );
        }
        else
        {
            // CREATE VIEW toolbar
        }
        if( m_styleBar )
        {
            if( m_tbSetup[1].m_orientation == 1 || m_tbSetup[1].m_orientation == 3  )
            {
                m_fieldText = new wxTextCtrl( m_styleBar, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
                m_fieldText->Bind( wxEVT_KILL_FOCUS, &DrawingView::OnLabelTextChanged, this );
                m_fieldText->Disable();
                m_styleBar->AddControl( m_fieldText );
                m_fontName = new FontComboBox( m_styleBar );
                m_fontName->Bind( wxEVT_KILL_FOCUS, &DrawingView::OnFontNameChange, this );
                m_styleBar->AddControl( m_fontName );
                m_fontSize = new wxComboBox( m_styleBar, wxID_ANY, "" );
                ChangeFontEement();
                m_fontSize->SetSelection( 0 );
                m_styleBar->AddControl( m_fontSize );
            }
            else
            {
                m_styleBar->AddTool( wxID_COMMENTFIELD, _( "Comment" ), commentSVG, commentSVG, wxITEM_NORMAL, _( "" ), _( "" ) );
                m_styleBar->AddTool( wxID_FONTNAME, _( "FontName" ), fontnameSVG, fontnameSVG, wxITEM_NORMAL, _( "" ), _( "" ) );
                m_styleBar->AddTool( wxID_FONTSIZE, _( "FontSize" ), fontsizeSVG, fontsizeSVG, wxITEM_NORMAL, _( "" ), _( "" ) );
            }
            m_styleBar->AddSeparator();
            m_styleBar->AddTool( wxID_BOLD, _( "Bold" ), boldSVG, boldSVG, wxITEM_CHECK, _( "Bold" ), _( "Make the font bold" ) );
            m_styleBar->AddTool( wxID_ITALIC, _( "Italic" ), italicSVG, italicSVG, wxITEM_CHECK, _( "Italic" ), _( "Make the font italic" ) );
            m_styleBar->AddTool( wxID_UNDERLINE, _( "Underline" ), underlineSVG, underlineSVG, wxITEM_CHECK, _( "Make the font underlined" ), _( "Make the font underlined" ) );
            m_styleBar->AddTool( wxID_LEFTALIGN, _( "LeftAlign" ),  leftalignSVG, leftalignSVG, wxITEM_CHECK, _( "" ), _( "" ) );
            m_styleBar->AddTool( wxID_CENTERALIGN, _( "CenterAlign" ),  centeralignSVG, centeralignSVG, wxITEM_CHECK, _( "" ), _( "" ) );
            m_styleBar->AddTool( wxID_RIGHTALIGN, _( "RightAlign" ),  rightalignSVG, rightalignSVG, wxITEM_CHECK, _( "" ), _( "" ) );
        }
    }
    else
    {
        CreateViewMenu();
        wxBitmapBundle tableSVG, joinsSVG;
#ifdef __WXMSW__
        HANDLE gs_wxMainThread = NULL;
        const HINSTANCE inst = wxDynamicLibrary::MSWGetModuleHandle( "dbwindow", &gs_wxMainThread );
        const void *dataTable = nullptr, *data10 = nullptr;
        size_t sizeTable = 0, size10 = 0;
        if( !wxLoadUserResource( &dataTable, &sizeTable, "table", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            tableSVG = wxBitmapBundle::FromSVG( (const char *) dataTable, wxSize( 16, 16 ) );
        }
        if( !wxLoadUserResource( &data10, &size10, "joins", RT_RCDATA, inst ) )
        {
            auto err = ::GetLastError();
            wxMessageBox( wxString::Format( "Error: %d!!", err ) );
        }
        else
        {
            joinsSVG = wxBitmapBundle::FromSVG( (const char *) data10, wxSize( 16, 16 ) );
        }
#elif __WXOSX__
        tableSVG = wxBitmapBundle::FromSVGResource( "table", wxSize( 16, 16 ) );
        joinsSVG = wxBitmapBundle::FromSVGResource( "joins", wxSize( 16, 16 ) );
#else
        tableSVG = wxBitmapBundle::FromSVG( table, wxSize( 16, 16 ) );
        joinsSVG = wxBitmapBundle::FromSVG( joins, wxSize( 16, 16 ) );
#endif
        m_tb->AddTool( wxID_SELECTTABLE, _( "Select Table" ), tableSVG, tableSVG, wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" )  );
        m_tb->AddTool( wxID_JOINS, _( "Joins" ), joinsSVG, joinsSVG, wxITEM_NORMAL, _( "Joins" ), _( "Jois" ) );
    }
    m_tb->Realize();
    if( m_styleBar )
    {
        m_styleBar->Realize();
    }
    LayoutChildren( size );
}

void DrawingView::LayoutChildren(const wxSize &size)
{
    int offset = 0;
    auto posFrame = wxPoint( 0, 0 );
    auto sizeFrame = wxSize( size.x, size.y );
    if( m_tbSetup[0].m_hideShow && m_tb->IsShown() )
    {
        switch( m_tbSetup[0].m_orientation )
        {
        case 0:
            m_tb->SetSize( 0, 0,  wxDefaultCoord, size.y );
            offset = m_tb->GetSize().x;
            posFrame.x = offset;
            sizeFrame.SetWidth ( ( size.x - offset ) );
            break;
        case 1:
            m_tb->SetSize( 0, 0,  size.x, wxDefaultCoord );
            offset = m_tb->GetSize().y;
            posFrame.y = offset;
            sizeFrame.SetHeight( ( size.y - offset ) );
            break;
        case 2:
            offset = m_tb->GetSize().x;
            m_tb->SetSize( size.x - offset, 0,  offset, size.y );
            sizeFrame.SetWidth( size.x - offset );
            sizeFrame.SetHeight( size.y );
            break;
        case 3:
            offset = m_tb->GetSize().y;
            sizeFrame.SetWidth( size.x );
            sizeFrame.SetHeight( ( size.y - offset ) );
            m_tb->SetSize( 0, size.y - offset, size.x, wxDefaultCoord );
            break;
        default:
            break;
        }
    }
    else
        m_tb->Hide();
    if( m_styleBar && ( m_tbSetup[1].m_hideShow && m_styleBar->IsShown() ) )
    {
        switch( m_tbSetup[1].m_orientation )
        {
        case 0:
#ifdef __WXOSX__
            m_styleBar->SetSize( 0, 0,  wxDefaultCoord, size.y );
#else
            m_styleBar->SetSize( offset, 0,  wxDefaultCoord, size.y );
#endif
            if( m_tbSetup[0].m_orientation == 0 )
                offset += m_styleBar->GetSize().x;
            posFrame.x = offset;
            sizeFrame.SetWidth( size.x - offset );
            break;
        case 1:
#ifdef __WXOSX__
            m_styleBar->SetSize( 0, 0,  size.x, wxDefaultCoord );
#else
            m_styleBar->SetSize( 0, offset,  size.x, wxDefaultCoord );
#endif
            if( m_tbSetup[0].m_orientation == 1 )
                offset += m_styleBar->GetSize().y;
            posFrame.y = offset;
            sizeFrame.SetHeight( size.y - offset );
            break;
        case 2:
            if( m_tbSetup[0].m_orientation == 2 )
                offset += m_styleBar->GetSize().x;
            m_styleBar->SetSize( size.x - offset, 0,  wxDefaultCoord, size.y );
            sizeFrame.SetWidth(  size.x - offset );
            break;
        case 3:
            if( m_tbSetup[0].m_orientation == 3 )
                offset += m_styleBar->GetSize().y;
            sizeFrame.SetHeight( sizeFrame.GetHeight() - ( size.y - offset ) );
            m_styleBar->SetSize( 0, size.y - offset, size.x, wxDefaultCoord );
            break;
        default:
            break;
        }
    }
    else if( m_styleBar && !m_tbSetup[1].m_hideShow )
        m_styleBar->Hide();
#if defined( __WXMSW__ ) || defined( __WXGTK__ )
    m_frame->SetSize( posFrame.x, posFrame.y, sizeFrame.GetWidth(), sizeFrame.GetHeight() );
#else
    m_canvas->SetSize( posFrame.x, posFrame.y, sizeFrame.GetWidth(), sizeFrame.GetHeight() );
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
    wxString libName;
    wxDynamicLibrary lib;
    bool quickSelect = false;
    m_queries = queries;
    m_path = path;
    m_canvas->SetQueryInfo( queries );
    m_canvas->SetObjectPath( path );
#ifdef __WXMSW__
    libName = m_libPath + "dialogs";
#elif __WXMAC__
    libName = m_libPath + "liblibdialogs.dylib";
#else
    libName = m_libPath + "libdialogs";
#endif
    lib.Load( libName );
    auto frameSize = m_frame->GetSize();
    if( lib.IsLoaded() )
    {
        if( m_type == QueryView || m_type == NewViewView )
        {
            if( m_type == QueryView )
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
                        wxString configPath = config->GetPath();
                        config->SetPath( "Query" );
                        config->Write( "QuerySource", m_source );
                        config->Write( "QueryPresentation", m_presentation );
                        config->SetPath( configPath );
                        if( m_source != 0 )
                        {
                            auto res2 = SelectTable( false, m_tables, query, quickSelect );
                            if( res2 == wxID_CANCEL )
                                return;
                        }
                    }
                    else
                        return;
                }
                if( res == wxID_OK )
                {
                    wxBusyCursor wait;
                    GetDocument()->SetFilename( documentName );
                    if( GetDocument()->OnOpenDocument( documentName ) && ((DrawingDocument *) GetDocument() )->IsLoadSuccessful() )
                    {
                        GetDatabaseCanvas()->LoadQuery( GetDocument()->GetDatabase()->GetTableVector().m_tables );
                        m_frame->SetTitle( "Query - " + documentName );
                    }
                    else
                    {
                        wxMessageBox( _( "Error loading the query from the disk" ) );
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
                        if( m_fields )
                            m_fields->Show( true );
                        m_canvas->Show( true );
                        if( m_queryBook )
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
                        LayoutChildren( m_parent->GetClientSize() );
                    }
                    else
                    {
                        QUICKSELECT func2 = (QUICKSELECT) lib.GetSymbol( "QuickSelectDlg" );
                        res = func2( m_frame, db, m_selectTableName, GetDocument()->GetQueryFields(), GetDocument()->GetAllSorted(), GetDocument()->GetQuerySorted() );
                        quickSelect = true;
                        SelectTable( false, m_tables, query, quickSelect );
                    }
                }
            }
            else
            {
                int heightStyleBar = 0;
                m_frame->Freeze();
                wxPoint framePosition;
#ifndef __WXOSX__
                if( m_styleBar )
                    heightStyleBar = m_styleBar->GetSize().y;
#endif
                framePosition = m_frame->GetPosition();
                frameSize = m_frame->GetSize();
                m_canvas->Show( true );
                if( m_fields )
                    m_fields->Show( true );
                if( m_queryBook )
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
                options.isTemp = false;
                options.schema = "";
                options.options = 0;
                CREATEVIEWOPTIONS func = (CREATEVIEWOPTIONS) lib.GetSymbol( "CreateViewOptionsFunc" );
                res = func( m_frame, GetDocument()->GetDatabase(), options );
                if( res == wxID_CANCEL )
                {
                    m_dbFrame->Show( true );
                    m_frame->Close();
                    return;
                }
                else
                {
                    res = SelectTable( false, m_tables, query, quickSelect, true );
                    if( res == wxID_CANCEL )
                    {
                        m_dbFrame->Show( true );
                        m_frame->Close();
                    }
                }
            }
        }
        else
        {
            SelectTable( false, m_tables, query, false );
            sizer->Layout();
            m_canvas->Show( true );
        }
    }
    if( m_type != NewViewView && m_tables.size() > 0 )
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
                        long item = m_page3->GetSourceList()->InsertItem( i++, L"\"" + dbTable->GetTableName() + L"\".\"" + (*it1)->GetFieldName() + L"\"" );
                        m_page3->GetSourceList()->SetItemData( item, item );
                        GetDocument()->AddGroupByAvailableField( L"\"" + dbTable->GetTableName() + L"\".\"" + (*it1)->GetFieldName() + L"\"", item );
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
                m_frame->Layout();
                sizer->Layout();
            }
        }
    }
    m_frame->SendSizeEvent();
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
        delete m_tb;
        m_tb = nullptr;
        delete m_styleBar;
        m_styleBar = nullptr;
        frame->SetSize( 0, 0, mainWin->GetClientSize().GetWidth(), mainWin->GetClientSize().GetHeight() );
    }
    else
        m_tb->ClearTools();
    if( !wxView::OnClose( deleteWindow ) )
        return false;
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
    wxString libName;
    wxDynamicLibrary lib;
#ifdef __WXMSW__
    libName = m_libPath + "dialogs";
#elif __WXMAC__
    libName =  m_libPath + "liblibdialogs.dylib";
#else
    libName = m_libPath + "libdialogs";
#endif
    ::wxPGInitResourceModule();
    lib.Load( libName );
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
        wxMessageBox( _( "Error loading the library" ) );
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
    if( lib.IsLoaded() )
    {
        CREATEFOREIGNKEY func = (CREATEFOREIGNKEY) lib.GetSymbol( "CreateForeignKey" );
        result = func( m_frame, dbTable, newFK, GetDocument()->GetDatabase(),  logOnly, true, newFK, match );
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
        wxMessageBox( _( "Error loading the library" ) );
}

void DrawingView::OnViewSelectedTables(wxCommandEvent &WXUNUSED(event))
{
    wxString query = "";
    if( m_page6 )
        query = m_page6->GetSyntaxCtrl()->GetValue();
    SelectTable( false, m_tables, query, false );
}

int DrawingView::SelectTable(bool isTableView, std::map<wxString, std::vector<TableDefinition> > &tables, wxString &query, bool quickSelect, bool isNewView/* = false*/)
{
    int res = 0;
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
    if( lib.IsLoaded() && !quickSelect )
    {
        TABLESELECTION func2 = (TABLESELECTION) lib.GetSymbol( "SelectTablesForView" );
        res = func2( m_frame, GetDocument()->GetDatabase(), tables, GetDocument()->GetTableNames(), isTableView, m_type, isNewView );
    }
    if( m_type == QueryView || m_type == NewViewView )
    {
        if( res != wxID_CANCEL )
        {
            std::vector<TableField *> queryFields = GetDocument()->GetQueryFields();
            if( isNewView )
            {
                if( options.isTemp )
                    query = "CREATE TEMPORARY VIEW ";
                else
                    query = "CREATE VIEW ";
                query += "\"" + options.schema + "\".\"Untitled\"";
                if( options.options == 1 || options.options == 2 || options.options == 3)
                {
                    query += "\n\rWITH ";
                    if( options.options == 1 )
                        query += "ENCRYTION ";
                    if( options.options == 2 )
                        query += "SCHEMABINDING ";
                    if( options.options == 3 )
                        query += "VIEW_METADATA";
                }
                query += "\nAS SELECT ";
            }
            else
                query = "SELECT ";
            if( !quickSelect && queryFields.size() == 0 && !isNewView )
                query += "<unknown fields>\n";
            else if( isNewView )
                query += "<not specified>\n";
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
            wxString name = m_selectTableName[0]->GetSchemaName() + L"." + m_selectTableName[0]->GetTableName();
            tables[m_selectTableName[0]->GetSchemaName()].push_back( TableDefinition( L"", m_selectTableName[0]->GetSchemaName(), m_selectTableName[0]->GetTableName() ) );
        }
        else
            tables[m_selectTableName[0]->GetCatalog()].push_back( TableDefinition( m_selectTableName[0]->GetCatalog(), m_selectTableName[0]->GetSchemaName(), m_selectTableName[0]->GetTableName() ) );
    }
    if( res != wxID_CANCEL && m_tables.size() > 0 )
    {
        ((DatabaseCanvas *) m_canvas)->UnselectAllTables();
        ((DrawingDocument *) GetDocument())->AddTables( m_tables );
        m_selectTableName = ((DrawingDocument *) GetDocument())->GetDBTables();
        ((DatabaseCanvas *) m_canvas)->DisplayTables( m_tables, GetDocument()->GetQueryFields(), query, m_whereRelatons );
        if( m_type == QueryView || m_type == NewViewView )
        {
            if( query != L"\n" )
            {
                std::vector<MyErdTable *> dbTables = ((DrawingDocument *)GetDocument())->GetTables();
                m_page1->AddQuickSelectSortingFields( GetDocument()->GetAllSorted(), GetDocument()->GetQuerySorted() );
                if( options.options == 4 )
                    query.Replace( ";", "\nWITH CHECK OPTION;" );
                m_page6->SetSyntaxText( query );
                if( m_edit )
                    m_edit->SetText( query );
            }
            if( quickSelect )
            {
                PopuateQueryCanvas();
                auto position = m_frame->GetMenuBar()->FindMenu( _( "Design" ) );
                auto designMenu = m_frame->GetMenuBar()->GetMenu( position );
                designMenu->Check( wxID_DATASOURCE, false );
                m_tb->ToggleTool( wxID_DATASOURCE, false );
                m_frame->Layout();
                sizer->Layout();
            }
        }
    }
    return res;
}

void DrawingView::OnSetProperties(wxCommandEvent &WXUNUSED(event))
{
    ShapeList list;
    MyErdTable *shape = nullptr;
    auto found = false;;
    ((DatabaseCanvas *) m_canvas)->GetDiagramManager().GetShapes(  CLASSINFO( MyErdTable ), list );
    for( ShapeList::iterator it = list.begin(); it != list.end() && !found; ++it )
    {
        if( ( *it )->IsSelected() )
        {
            shape = dynamic_cast<MyErdTable *>(*it);
            found = true;
        }
    }
    SetProperties( shape );
}

void DrawingView::SetProperties(const wxSFShapeBase *shape)
{
    ShapeList selections;
    std::vector<std::wstring> errors;
    int type = 0;
    wxAny any;
    DatabaseTable *dbTable = nullptr;
    FieldShape *dbField = nullptr;
    Divider *divider = nullptr;
    DesignLabel *label = nullptr;
    DesignField *designField = nullptr;
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
        type = DesignPropertiesType;
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
            type = DatabaseTablePropertiesType;
        }
        else
        {
            divider = wxDynamicCast( shape, Divider );
            if( divider )
            {
                type = DividerPropertiesType;
                dbTable = nullptr;
            }
            else
            {
                label = wxDynamicCast( shape, DesignLabel );
                if( label )
                {
                    type = DesignLabelPropertiesType;
                    dbTable = nullptr;
                }
                else
                {
                    dbField = wxDynamicCast( shape, FieldShape );
                    if( dbField )
                    {
                        erdTable = wxDynamicCast( dbField->GetParentShape()->GetParentShape(), MyErdTable );
                        type = DatabaseFieldPropertiesType;
                    }
                    else
                    {
                        designField = wxDynamicCast( shape, DesignField );
                        if( designField )
                        {
                            type = DesignFieldPropertiesType;
                            dbTable = nullptr;
                        }
                        else
                        {
                            sign = wxDynamicCast( shape, ConstraintSign );
                            if( sign )
                            {
                                type = SignPropertiesType;
                            }
                        }
                    }
                }
            }
        }
    }
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
    int res = 0;
    PropertiesHandler *propertiesPtr = nullptr;
    wxString title;
    if( type == DatabaseTablePropertiesType )
    {
        auto db = GetDocument()->GetDatabase();
        {
            std::lock_guard<std::mutex> lock( GetDocument()->GetDatabase()->GetTableVector().my_mutex );
            res = db->GetTableProperties( dbTable, errors );
        }
        erdTable->SetProperties( erdTable->GetTable()->GetTableProperties() );
        erdTable->SetDatabase( db );
        propertiesPtr = erdTable;
        propertiesPtr->SetDatabase( db );
        erdTable->SetType( DatabaseTablePropertiesType );
        title = _( "Table " );
        title += schemaName + L"." + tableName;
    }
    if( type == DatabaseFieldPropertiesType )
    {
        auto db = GetDocument()->GetDatabase();
        {
            //#if _MSC_VER >= 1900
            std::lock_guard<std::mutex> lock( GetDocument()->GetDatabase()->GetTableVector().my_mutex );
            res = db->GetFieldProperties( tableName.ToStdWstring(), dbField->GetField(), errors );
        }
        dbField->SetProperties( dbField->GetField()->GetFieldProperties() );
        dbField->SetType( DatabaseFieldPropertiesType );
        propertiesPtr = dbField;
        propertiesPtr->SetDatabase( db );
        propertiesPtr->SetFieldType( dbField->GetField()->GetFieldType() );
        title = _( "Column " );
        title += dbField->GetField()->GetFullName();
    }
    if( type == DividerPropertiesType )
    {
        std::unique_ptr<PropertiesHandler> ptr( divider );
        divider->SetType( DividerPropertiesType );
        title = _( "Band Object" );
    }
    if( type == DesignPropertiesType )
    {
        auto ptr = std::unique_ptr<PropertiesHandler>( m_designCanvas );
        any = m_designCanvas;
        m_designCanvas->SetType( DesignPropertiesType );
        title = _( "Query Object" );
    }
    if( type == DesignFieldPropertiesType )
    {
        title = _( "Column Object" );
        designField->SetType( DesignFieldPropertiesType );
        auto ptr = std::unique_ptr<PropertiesHandler>( designField );
        any = field;
    }
    if( type == DesignLabelPropertiesType )
    {

    }
    if( lib.IsLoaded() )
    {
        CREATEPROPERTIESDIALOG func = (CREATEPROPERTIESDIALOG) lib.GetSymbol( "CreatePropertiesDialog" );
//        TableProperties *props = *static_cast<TableProperties *>( properties );
        res = func( m_frame, propertiesPtr, title, command, erdTable->GetTable(), logOnly );
        if( logOnly )
        {
            m_text->SetValue( command );
            m_log->Show();
        }
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
#if __WXGTK__
    if( m_initialized )
    {
        wxSize clientSize = m_parent->GetClientSize();
        if( activate )
            m_isActive = true;
        if( activate )
        {
            if( m_type == QueryView )
                if( m_styleBar )
                    m_styleBar->Show( true );
            m_frame->SetSize( 0, 0, clientSize.x, clientSize.y );
            CreateViewToolBar();
        }
        else
        {
            if( m_tb )
                m_tb->ClearTools();
            if( m_styleBar )
            {
                m_styleBar->Show( false );
                m_styleBar->ClearTools();
            }
        }
    }
#endif
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
    wxString libName;
    wxDynamicLibrary lib1;
#ifdef __WXMSW__
    libName = m_libPath + "tabledataedit";
#elif __WXOSX__
    libName = m_libPath + "liblibtabledataedit.dylib";
#else
    libName = m_libPath + "libtabledataedit";
#endif
    lib1.Load( libName );
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
    wxString libName;
    wxDynamicLibrary lib1;
#ifdef __WXMSW__
    libName = m_libPath + "tabledataedit";
#elif __WXOSX__
    libName = m_libPath + "liblibtabledataedit.dylib";
#else
    libName = m_libPath + "libtabledataedit";
#endif
    lib1.Load( libName );
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
    wxString libName;
    wxDynamicLibrary *lib = new wxDynamicLibrary();
#ifdef __WXMSW__
    libName = m_libPath + "dbloader";
#elif __WXMAC__
    libName = m_libPath + "liblibdbloader.dylib";
#else
    libName = m_libPath + "libdbloader";
#endif
    lib->Load( libName );
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

void DrawingView::AddFieldToQuery(const FieldShape &field, QueryFieldChange isAdding, const std::wstring &tableName)
{
    TableField *fld = const_cast<FieldShape &>( field ).GetField();
    wxString name = tableName + L"." + fld->GetFieldName();
    name = "\"" + name;
    name = name + "\"";
    wxString query = m_page6->GetSyntaxCtrl()->GetValue();
    if( isAdding == ADD )
    {
        m_fields->AddField( name );
        m_page1->AddRemoveSortingField( true, name );
        GetDocument()->AddRemoveField( fld, QueryFieldChange::ADD );
        std::vector<TableField *> queryFields = GetDocument()->GetQueryFields();
        if( m_type == QueryView )
        {
            if( queryFields.size() == 1 )
                query.Replace( "<unknown fields>", name + " " );
            else
            {
                wxString temp = query.substr( query.Find( ' ' ) );
                temp = temp.substr( 0, temp.Find( "FROM" ) - 1 );
                query.Replace( temp, temp + ", " + name + " " );
            }
        }
        else
        {
            if( queryFields.size() == 1 )
            {
                query.Replace( "<not specified>", name + " " );
                query.Replace( L"\n", L"(" + fld->GetFieldName() + L")\n", false );
            }
            else
            {
                query.Replace( L")\n", L", " + fld->GetFieldName() + L")\n", false );
                query.Replace( "\nFROM", ",\n          " + name + "\nFROM" );
            }
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
    if( m_edit )
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
                AddFieldToQuery( *field2add, isAdd ? ADD : REMOVE, tableName );
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
            result += const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + L" LEFT OUTER JOIN " + type->GetRefTable() + L" ON " + const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + L"." + type->GetLocalColumn() + L" = " + type->GetRefTable() + L"." + const_cast<QueryConstraint *>( type )->GetRefColumn();
        else
            result += type->GetRefTable() + " LEFT OUTER JOIN " + const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + " ON " + type->GetRefTable() + "." + const_cast<QueryConstraint *>( type )->GetRefColumn() + " = " + const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + "." + type->GetLocalColumn();
        query = query.substr( query.find( "WHERE" ) );
        if( m_whereRelatons.size() == 1 && m_whereCondition.size() == 0 )
        {
            int pos;
            pos = query.Find( "GROUP BY" );
            if( pos == wxNOT_FOUND )
            {
                pos = query.Find( "HAVING" );
                if( pos == wxNOT_FOUND )
                {
                    pos = query.Find( "ORDER BY" );
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
            auto temp1 = query.substr( 0, query.find( ' ' ) );
            res = ( temp1 == const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + L"." + type->GetLocalColumn() ) ||
                  ( temp1 == type->GetRefTable() + L"." + const_cast<QueryConstraint *>( type )->GetRefColumn() );
            if( res )
            {
                result += temp1;
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
        int pos = query.Find( "WHERE" );
        if( pos != wxNOT_FOUND )
        {
            wherePart = query.substr( pos + 6 );
            pos = wherePart.Find( "GROUP BY" );
            if( pos == wxNOT_FOUND )
            {
                pos = wherePart.Find( "HAVING " );
                if( pos == wxNOT_FOUND )
                {
                    pos = wherePart.Find( "ORDER BY" );
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
            pos = query.Find( "GROUP BY" );
            if( pos == wxNOT_FOUND )
            {
                pos = query.Find( "HAVING " );
                if( pos == wxNOT_FOUND )
                {
                    pos = query.Find( "ORDER BY" );
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
        int pos = query.Find( "HAVING " );
        if( pos != wxNOT_FOUND )
        {
            wxString havingPart = query.substr( pos + 7 );
            havingPart = havingPart.substr( 0, ( pos = havingPart.Find( "ORDER BY" ) ) == wxNOT_FOUND ? havingPart.length() - 1 : pos );
            m_havingCondition[event.GetInt()] = event.GetString();
        }
        else
        {

        }
    }
    if( event.GetEventObject() == m_page1 || event.GetEventObject() == m_page3 )
    {
        SortGroupByHandling( event.GetInt(), event.GetString(), m_queryBook->GetSelection(), query, (Positions *)( event.GetClientData() ), event.GetExtraLong() );
    }
}

void DrawingView::SortGroupByHandling(const int type, const wxString &fieldName, const int queryType, wxString &query, const Positions *pos, long operation)
{
    int start, end;
    bool isInserting = false;
    wxString queryString;
    if( queryType == 2 )
    {
        start = query.Find( "GROUP BY" );
        end = query.Find( "HAVING" );
        if( end == wxNOT_FOUND )
            end = query.Find( "ORDER BY" );
        queryString = "GROUP BY ";
    }
    else
    {
        start = query.Find( "ORDER BY" );
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
    else if( type == CHANGEFIELD )
    {
        if( queryType == 2 )
        {
            replace = "GROUP BY ";
            GetDocument()->ShuffleGroupByFields( fieldName, pos->position, replace );
        }
    }
    else
    {
        if( queryType == 0 )
        {
            wxString fieldReplace;
            auto fieldCut = str.substr( str.find( fieldName ) );
            if( operation == 0 )
                fieldReplace = fieldName + " DESC";
            else
                fieldReplace = fieldName + " ASC";
            auto dotPos = fieldCut.Find( ',' );
            if( dotPos == wxNOT_FOUND )
            {
                fieldReplace += ";";
                fieldCut += ";";
            }
            else
            {
                fieldReplace += ",";
                fieldCut = fieldCut.substr( 0, dotPos );
            }
            query.Replace( fieldCut, fieldReplace );
        }
    }
    query.Replace( str, replace );
    m_page6->GetSyntaxCtrl()->SetValue( query );
    dynamic_cast<QueryRoot *>( m_canvas->GetDiagramManager().GetRootItem() )->SetQuery( query );
}

void DrawingView::OnRetrievalArguments(wxCommandEvent &WXUNUSED(event))
{
    std::vector<QueryArguments> arguments;
    wxString libName;
    wxDynamicLibrary *lib = new wxDynamicLibrary();
#ifdef __WXMSW__
    libName = m_libPath + "dialogs";
#elif __WXMAC__
    libName = m_libPath + "liblibdialogs.dylib";
#else
    libName = m_libPath + "libdialogs";
#endif
    lib->Load( libName );
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

void DrawingView::OnDataSource(wxCommandEvent &event)
{
    bool emptyQuery = true;
    int heightStyleBar = m_styleBar->GetSize().y;
    wxPoint framePosition = m_frame->GetPosition();
    wxSize frameSize = m_frame->GetSize();
    wxMDIClientWindow *parent = (wxMDIClientWindow *) m_parent->GetClientWindow();
    wxSize parentSize = parent->GetSize();
    wxPoint parentPos = parent->GetPosition();
    if( m_type == QueryView )
    {
        if( event.IsChecked() )
        {
            CreateQueryMenu( SQLSelectMenu );
            if( framePosition.y == 0 )
            {
                parent->SetSize( parentPos.x, parentPos.y - heightStyleBar, parentSize.GetWidth(), parentSize.GetHeight() + heightStyleBar );
                m_frame->SetSize( frameSize.GetWidth(), frameSize.GetHeight() + heightStyleBar );
            }
            m_styleBar->Show( false );
            m_designCanvas->GetDiagramManager()->Clear();
            m_designCanvas->ClearDesignCanvas();
            m_designCanvas->Show( false );
            m_fields->Show( true );
            m_canvas->Show( true );
            m_queryBook->Show( true );
            parent->Layout();
            m_parent->Layout();
            sizer->Layout();
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
                CreateQueryMenu( QuickQueryMenu );
                m_designCanvas->Show( true );
                m_fields->Show( false );
                m_canvas->Show( false );
                m_queryBook->Show( false );
                sizer->Layout();
                parent->Layout();
                m_parent->Layout();
                PopuateQueryCanvas();
            }
        }
    }
    LayoutChildren( m_parent->GetClientSize() );
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
    lfHeight = lfHeight;
	return 1;
#endif
}

void DrawingView::ChangeFontEement()
{
    ShapeList list;
    wxFont font = wxNullFont, f;
    m_designCanvas->GetSelectedShapes( list );
    if( list.GetCount() == 0 )
        m_fontName->SetSelection( 0 );
    else
    {
        auto multiple = false;
        for( ShapeList::iterator it = list.begin(); it != list.end(); ++it )
        {
            auto label = wxDynamicCast( (*it), DesignLabel );
            if( label )
                f = label->GetProperties().As<DesignLabelProperties>().m_font;
            else
            {
                auto field = wxDynamicCast( (*it), DesignField );
                if( field )
                    f = field->GetProperties().As<DesignFieldProperties>().m_font;
            }
            if( font != f )
            {
                if( font != wxNullFont )
                    multiple = true;
                font = f;
            }
        }
        if( multiple )
        {
            m_fontName->Disable();
            m_fontSize->Disable();
        }
        else
        {
            m_fontName->SetValue( font.GetFaceName() );
            m_fontSize->SetValue( wxString::Format( "%d", font.GetPointSize() ) );
        }
        return;
    }
#ifdef __WXMSW__
    m_fontSize->Clear();
    wxString strFaceName = m_fontName->GetStringSelection();
    if( (int) m_fontName->GetClientData( m_fontName->GetSelection() ) != RASTER_FONTTYPE )
    {
        for( auto it = m_fontSizes.begin(); it < m_fontSizes.end(); ++it )
        {
            AddSize( std::stoi( (*it) ), 0 );
        }
    }
    else
    {
        HDC dc = ::GetDC( NULL );
        EnumFontFamilies( dc, strFaceName, (FONTENUMPROC) DrawingView::EnumFontFamiliesCallback2, (LPARAM) this );
        ::ReleaseDC( NULL, dc );
    }
#endif
#ifdef __WXOSX__
    m_fontSize->Append( "9" );
    m_fontSize->Append( "10" );
    m_fontSize->Append( "11" );
    m_fontSize->Append( "12" );
    m_fontSize->Append( "13" );
    m_fontSize->Append( "14" );
    m_fontSize->Append( "18" );
    m_fontSize->Append( "24" );
    m_fontSize->Append( "36" );
    m_fontSize->Append( "48" );
    m_fontSize->Append( "64" );
    m_fontSize->Append( "72" );
    m_fontSize->Append( "96" );
    m_fontSize->Append( "144" );
    m_fontSize->Append( "288" );
#endif
#ifdef __WXGTK__
    auto context = pango_context_new();
    auto fontMap = pango_context_get_font_map( context );
    if( fontMap )
    {

    }
    g_object_unref( context );
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
    item->Check( event.IsChecked() );
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
//    if( dlg )*/
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
    auto type = GetDocument()->GetDatabase()->GetTableVector().GetDatabaseType();
    auto subtype = GetDocument()->GetDatabase()->GetTableVector().GetDatabaseSubtype();
    auto mbar = new wxMenuBar;
    auto fileMenu = new wxMenu;
    fileMenu->Append( wxID_CLOSE, _( "&Close\tCtrl+W" ), _( "Close Database Window" ) );
    fileMenu->AppendSeparator();
    fileMenu->Append( wxID_CREATEDATABASE, _( "Create Database..." ), _( "Create Database" ) );
    fileMenu->Append( wxID_ATTACHDATABASE, _( "Attach Database..." ), _( "Attach Database" ) );
    fileMenu->Append( wxID_DELETEDATABASE, _( "Delete Database..." ), _( "Delete Database" ) );
    if( type  == L"SQLite" )
        fileMenu->Append( wxID_DETACHDATABASE, _( "Detach Database" ), _( "Detach Database" ) );
    fileMenu->AppendSeparator();
    if( type == L"PostgreSQL" || ( type == L"ODBC" && subtype == L"PostgreSQL" ) )
        fileMenu->Append( wxID_CREATETABLESPACE, _( "Create Tablespace" ), _( "Create Tablespace" ) );
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
    menuDesign->Append( wxID_CUSTOMCOLORS, _( "Custom Colors.." ) );
    menuDesign->Append( wxID_DATABASEOPTIONS, _( "Options..." ), _( "Database preferences" ) );
    mbar->Insert( 2, menuDesign, _( "&Design" ) );
    auto helpMenu = new wxMenu;
    helpMenu->Append( wxID_HELP, _( "Help" ), _( "Help" ) );
    mbar->Append( helpMenu, _( "Help" ) );
    m_frame->SetMenuBar( mbar );
}

void DrawingView::CreateQueryMenu(const int queryType)
{
    wxMenu *edit = nullptr, *object = nullptr, *design = nullptr, *rows = nullptr;
    auto mbar = new wxMenuBar;
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
    if( queryType == QuickQueryMenu )
    {
        edit = new wxMenu;
        edit->Append( wxID_UNDO, _( "Undo" ), _( "Perform undo operation" ) );
        edit->AppendSeparator();
        edit->Append( wxID_CUT, _( "Cut" ), _( "Perform cut operation" ) );
        edit->Append( wxID_COPY, _( "Copy" ), _( "Perform copy operation" ) );
        edit->Append( wxID_PASTE, _( "Paste" ), _( "Perform paste operation" ) );
        edit->AppendSeparator();
        auto select = new wxMenu;
        select->Append( wxID_SELECTALL, _( "Select All" ), _( "Perform Select All operation" ) );
        edit->AppendSubMenu( select, _( "Select" ) );
        object = new wxMenu;
        object->Append( wxID_SELECTOBJECT, _( "Select Object" ), _( "Select Object" ) );
        object->Append( wxID_TEXTOBJECT, _( "Text" ), _( "Creates a text object" ) );
        design = new wxMenu;
        design->Append( wxID_DATASOURCE, _( "Data Source" ), _( "Modify SQL Select syntax" ), wxITEM_CHECK );
        design->Append( wxID_PREVIEW, _( "Preview" ), _( "Show data" ) );
        design->AppendSeparator();
        rows = new wxMenu;
        rows->Append( wxID_COLUMNSPEC, _( "Column Specification..." ), _( "Ready" ) );
//        rows->Append( );
        mbar->Insert( 0, edit, _( "Edit" ) );
        mbar->Insert( 1, object, _( "Object" ) );
        mbar->Insert( 2, design, _( "Design" ) );
        mbar->Insert( 3, rows, _( "Rows" ) );
    }
    if( queryType == SQLSelectMenu )
    {
        design = new wxMenu;
        design->Append( wxID_DATASOURCE, _( "Data Source" ), _( "Data Source" ), wxITEM_CHECK );
        design->Append( wxID_PREVIEDWQUERY, _( "Preview" ), _( "Preview" ) );
        design->AppendSeparator();
        design->Append( wxID_SELECTTABLE, _( "Select Table..." ) );
        design->AppendSeparator();
        design->Append( wxID_RETRIEVEARGS, _( "Retieval Arguments..." ), _( "Define Retrieval Arguments" ) );
        design->Append( wxID_DISTINCT, _( "Distinct" ), _( "Return distinct rows only" ), wxITEM_CHECK );
        design->AppendSeparator();
        design->Append( wxID_CONVERTTOSYNTAX, _( "Convert To Syntax" ), _( "Convert To Syntax" ) );
        auto show = new wxMenu;
        show->Append( wxID_SHOWDATATYPES, _( "Datatypes" ), _( "Show Datatypes" ), wxITEM_CHECK );
        show->Append( wxID_SHOWLABELS, _( "Labels" ), _( "Show Labels" ), wxITEM_CHECK );
        show->Append( wxID_SHOWCOMMENTS, _( "Comments" ), _( "Show Comments" ), wxITEM_CHECK );
        show->Append( wxID_SHOWSQLTOOLBOX, _( "SQL Toolbox" ), _( "SQL Toolbox" ), wxITEM_CHECK );
        show->Append( wxID_SHOWJOINS, _( "Joins" ), _( "Show Joins" ), wxITEM_CHECK );
        design->AppendSubMenu( show, _( "Show" ) );
        design->Check( wxID_DATASOURCE, true );
        show->Check( wxID_SHOWDATATYPES, true );
        show->Check( wxID_SHOWLABELS, true );
        show->Check( wxID_SHOWCOMMENTS, true );
        show->Check( wxID_SHOWSQLTOOLBOX, true );
        show->Check( wxID_SHOWJOINS, true );
        if( GetSyntaxPage() && GetSyntaxPage()->GetSyntaxCtrl()->GetValue().Find( "DISTNCT" ) != wxNOT_FOUND )
            design->Check( wxID_DISTINCT, true );
        else
            design->Check( wxID_DISTINCT, false );
        design->AppendSeparator();
        design->Append( wxID_CUSTOMCOLORS, _( "Custom Colors.." ) );
        design->Append( wxID_DATABASEOPTIONS, _( "Options..." ), _( "Database preferences" ) );
        mbar->Insert( 0, design, _( "Design" ) );
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
        designMenu->AppendSeparator();
        designMenu->Append( wxID_CUSTOMCOLORS, _( "Custom Colors.." ) );
        designMenu->Append( wxID_DATABASEOPTIONS, _( "Options..." ), _( "Database preferences" ) );
        mbar->Append( editMenu, _( "Edit" ) );
        mbar->Append( searchMenu, _( "Search" ) );
        mbar->Append( designMenu, _( "Design" ) );
    }
    mbar->Insert( 0, fileMenu, _( "File" ) );
    mbar->Append( helpMenu, _( "Help" ) );
    if( queryType == QuerySyntaxMenu )
    {
        mbar->EnableTop( 0, false );
        mbar->EnableTop( 1, false );
        mbar->EnableTop( 2, false );
        mbar->EnableTop( 3, false );
        mbar->EnableTop( 4, false );
    }
    m_frame->SetMenuBar( mbar );
}

void DrawingView::CreateViewMenu()
{
    auto mbar = new wxMenuBar;
    auto filemenu = new wxMenu;
    filemenu->Append( wxID_NEW, _( "&New...\tCtrl+N" ) );
    filemenu->Append( wxID_OPEN, _( "&Open Query..\tCtrl+O" ) );
    filemenu->Append( wxID_CLOSE, _( "&Close\tCtrl+W" ) );
    filemenu->AppendSeparator();
    filemenu->Append( wxID_SAVE, _( "&Save Query...\tCtrl+S" ) );
    filemenu->Append( wxID_SAVEAS, _( "Save Query &As..." ) );
    filemenu->AppendSeparator();
    filemenu->Append( wxID_EXIT, "E&xit\tCtrl+Q" );
    mbar->Append( filemenu, _( "File" ) );
    auto design = new wxMenu;
    design->Append( wxID_SELECTTABLE, _( "Select Table..." ) );
    design->Append( wxID_ARRANGETABLES, _( "Arrange Tables" ) );
    design->AppendSeparator();
    design->AppendCheckItem( wxID_CHECKOPTION, _( "Check Option" ), _( "Check Option" ) );
    design->AppendCheckItem( wxID_DISTINCT, _( "Distinct" ), _( "Distinct" ) );
    design->AppendSeparator();
    auto show = new wxMenu;
    show->Append( wxID_SHOWDATATYPES, _( "Datatypes" ), _( "Show Datatypes" ), wxITEM_CHECK );
    show->Append( wxID_SHOWLABELS, _( "Labels" ), _( "Show Labels" ), wxITEM_CHECK );
    show->Append( wxID_SHOWCOMMENTS, _( "Comments" ), _( "Show Comments" ), wxITEM_CHECK );
    show->Append( wxID_SHOWSQLTOOLBOX, _( "SQL Toolbox" ), _( "SQL Toolbox" ), wxITEM_CHECK );
    show->Append( wxID_SHOWJOINS, _( "Joins" ), _( "Show Joins" ), wxITEM_CHECK );
    design->AppendSubMenu( show, _( "Show" ) );
    design->AppendSeparator();
    design->Append( wxID_PREVIEW, _( "&Preview" ) );
    design->Append( wxID_CUSTOMCOLORS, _( "C&ustom Colors..." ) );
    design->Append( wxID_DATABASEOPTIONS, _( "&Options..." ) );
    mbar->Append( design, _( "&Design" ) );
    m_frame->SetMenuBar( mbar );
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

void DrawingView::OnExportSyntax(wxCommandEvent &WXUNUSED(event))
{
    auto tables = dynamic_cast<DrawingDocument *>( GetDocument() )->GetDBTables();
    wxString source;
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
    wxString libName;
    wxDynamicLibrary lib;
    wxString documentName = "";
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
        bool update;
        CHOOSEOBJECT func = (CHOOSEOBJECT) lib.GetSymbol( "ChooseObject" );
        int res = func( m_frame, -1, m_queries, documentName, m_path, update );
        if( res == wxID_OK )
        {
            wxString libraryName;
            auto found = false;
            for( std::vector<LibrariesInfo>::iterator it = m_path.begin(); it < m_path.end() && !found; ++it )
            {
                if( (*it).m_isActive )
                {
                    libraryName = (*it).m_path;
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
            if( GetDocument()->SaveNewQuery( libraryName, m_queries, documentName + ".qry", update ) ) 
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

FontComboBox *DrawingView::GetFontName() const
{
    return m_fontName;
}

wxComboBox *DrawingView::GetFontSize()
{
    return m_fontSize;
}

void DrawingView::SetFontBold(bool bold)
{
    auto tool = m_styleBar->FindById( 303 );
    if( tool )
        tool->Toggle( bold );
}

void DrawingView::SetFontItalic(bool italic)
{
    auto tool = m_styleBar->FindById( 304 );
    if( tool )
        tool->Toggle( italic );
}

void DrawingView::SetFontUnderline(bool underline)
{
    auto tool = m_styleBar->FindById( 305 );
    if( tool )
        tool->Toggle( underline );
}

void DrawingView::OnLabelTextChanged(wxFocusEvent &WXUNUSED(event))
{
    m_designCanvas->ChangeLabel( m_fieldText->GetValue() );
    m_fieldText->SetValue( "" );
    m_fieldText->Disable();
}

void DrawingView::OnFontNameChange(wxFocusEvent &WXUNUSED(event))
{
    if( m_designCanvas )
        m_designCanvas->ChangeFontName( m_fontName->GetValue() );
}

void DrawingView::OnDataSourceUpdateUI(wxUpdateUIEvent &event)
{
    if( GetDocument()->GetDBTables().size() == 0 )
        event.Enable( false );
    else
        event.Enable( true );
}

void DrawingView::OnDatabaseCreateView(wxCommandEvent &WXUNUSED(event))
{
    std::vector<QueryInfo> queries;
    std::vector<LibrariesInfo> path;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( m_parent );
#endif
    m_frame->Show( false );
    auto docTemplate = (DatabaseTemplate *) GetDocumentManager()->FindTemplate( CLASSINFO( DrawingDocument ) );
    docTemplate->CreateDatabaseDocument( m_parent, "*.qrv", NewViewView, GetDocument()->GetDatabase(), m_conf, wxDOC_NEW | wxDOC_SILENT );
    dynamic_cast<DrawingDocument *>( GetDocumentManager()->GetCurrentDocument() )->SetDatabase( GetDocument()->GetDatabase()  );
    dynamic_cast<DrawingView *>( docTemplate->GetDocumentManager()->GetCurrentView() )->SetDatabaseChildWindow( m_frame );
    dynamic_cast<DrawingView *>( docTemplate->GetDocumentManager()->GetCurrentView() )->SetLogWindow( m_log );
    dynamic_cast<DrawingView *>( GetDocumentManager()->GetCurrentDocument()->GetFirstView() )->GetTablesForView( GetDocument()->GetDatabase(), true, queries, path );
}

void DrawingView::SetDatabaseChildWindow(wxDocMDIChildFrame *frame)
{
    m_dbFrame = frame;
}

void DrawingView::SetToolbarOptions(Configuration *conf)
{
    m_conf = conf;
    m_tbSetup[0].m_hideShow = conf->m_tbSettings["ViewBar"].m_hideShow;
    m_tbSetup[0].m_showTooltips = conf->m_tbSettings["ViewBar"].m_showTooltips;
    m_tbSetup[0].m_showText = conf->m_tbSettings["ViewBar"].m_showText;
    m_tbSetup[0].m_orientation = conf->m_tbSettings["ViewBar"].m_orientation;
    m_tbSetup[1].m_hideShow = conf->m_tbSettings["StyleBar"].m_hideShow;
    m_tbSetup[1].m_showTooltips = conf->m_tbSettings["StyleBar"].m_showTooltips;
    m_tbSetup[1].m_showText = conf->m_tbSettings["StyleBar"].m_showText;
    m_tbSetup[1].m_orientation = conf->m_tbSettings["StyleBar"].m_orientation;
    m_source = conf->m_querySource;
    m_presentation = conf->m_queryPresentation;
}

void DrawingView::SetParentWindow(wxWindow *window)
{
    m_parent = wxStaticCast( window, wxDocMDIParentFrame );
}

void DrawingView::OnCustmColors(wxCommandEvent &WXUNUSED(event))
{
    wxColourData clrData;
    clrData.SetColour( m_canvas->GetBackgroundColour() );
    clrData.SetChooseAlpha( true );
    clrData.SetChooseFull( true );

    wxColourDialog dlg( m_parent, &clrData );
    dlg.SetTitle( _( "Color" ) );
    dlg.ShowModal();
}

void DrawingView::OnDatabasePreferences(wxCommandEvent &WXUNUSED(event))
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
    int res = 0;
    wxString title;
    PropertiesHandler *handler;
    if( m_type == DatabaseView )
        handler = m_canvas;
    else
        handler = m_designCanvas;
    std::lock_guard<std::mutex> lock( GetDocument()->GetDatabase()->GetTableVector().my_mutex );
    auto ptr = std::unique_ptr<PropertiesHandler>( handler );
    wxAny any = m_canvas;
    handler->SetType( m_type == DatabaseView ? DatabasePropertiesType : QueryPropertiesType );
    title = _( "Database Preferences" );
    if( lib.IsLoaded() )
    {
        CREATEPROPERTIESDIALOGFRPRJECT func = (CREATEPROPERTIESDIALOGFRPRJECT) lib.GetSymbol( "CreatePropertiesDialogForObject" );
        res = func( m_frame, ptr, title );
    }
}

void DrawingView::OnCreateTableSpace(wxCommandEvent &event)
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
    int res = 0;
    wxString title;
    if( lib.IsLoaded() )
    {
        CREATETABLESPACE func = (CREATETABLESPACE) lib.GetSymbol( "CreateTableSpace" );
        res = func( m_frame );
    }
}

void DrawingView::OnEditTableObject(wxCommandEvent &event)
{

}

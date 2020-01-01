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
#include "../dbhandler/res/quit.xpm"
#include "res/gui/toolbox.xpm"
#include "../dbhandler/res/new.xpm"
#include "../dbhandler/res/open.xpm"
#include "../dbhandler/res/save.xpm"
#include "../dbhandler/res/cut.xpm"
//#include "./res/gui/bold_png.c"
//#endif

#include "res/gui/bold.c"
#include "res/gui/italic.c"
#include "res/gui/underline.c"
#include "res/gui/preview.c"
#include "res/gui/sql.h"

#include <string>
#if _MSC_VER >= 1900 || !(defined __WXMSW__)
#include <mutex>
#endif

#include "wx/file.h"

#include "wx/docview.h"
#include "wx/fontenum.h"
#include "wx/notebook.h"
#include "wx/fdrepdlg.h"
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/cmdproc.h"
#include "wx/bmpcbox.h"
#include "wx/grid.h"
#include "wx/stc/stc.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "designlabel.h"
#include "constraint.h"
#include "constraintsign.h"
#include "table.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "FieldShape.h"
#include "commenttableshape.h"
#include "fontcombobox.h"
#include "MyErdTable.h"
#include "fieldwindow.h"
#include "syntaxproppage.h"
#include "wherehavingpage.h"
#include "databasecanvas.h"
#include "databasedoc.h"
#include "designcanvas.h"
#include "databaseview.h"

const wxEventTypeTag<wxCommandEvent> wxEVT_SET_TABLE_PROPERTY( wxEVT_USER_FIRST + 1 );
const wxEventTypeTag<wxCommandEvent> wxEVT_SET_FIELD_PROPERTY( wxEVT_USER_FIRST + 2 );
const wxEventTypeTag<wxCommandEvent> wxEVT_CHANGE_QUERY( wxEVT_USER_FIRST + 3 );

typedef int (*TABLESELECTION)(wxDocMDIChildFrame *, Database *, std::vector<wxString> &, std::vector<std::wstring> &, bool, const int);
typedef int (*CREATEINDEX)(wxWindow *, DatabaseTable *, Database *, wxString &, wxString &);
typedef int (*CREATEPROPERTIESDIALOG)(wxWindow *parent, Database *, int type, void *object, wxString &, bool, const wxString &, const wxString &, const wxString &, wxCriticalSection &);
typedef int (*CREATEFOREIGNKEY)(wxWindow *parent, wxString &, DatabaseTable *, std::vector<std::wstring> &, std::vector<std::wstring> &, std::wstring &, int &, int &, Database *, bool &, bool, std::vector<FKField *> &, int &);
typedef void (*TABLE)(wxWindow *, wxDocManager *, Database *, DatabaseTable *, const wxString &);
typedef int (*CHOOSEOBJECT)(wxWindow *, int);
typedef int (*NEWQUERY)(wxWindow *, int &, int &);
typedef int (*QUICKSELECT)(wxWindow *, const Database *, std::vector<DatabaseTable *> &, std::vector<Field *> &);
typedef Database *(*DBPROFILE)(wxWindow *, const wxString &, wxString &, const std::wstring &);
typedef int (*RETRIEVEARGUMENTS)(wxWindow *, std::vector<QueryArguments> &arguments, const wxString &, const wxString &);
typedef int (*GOTOLINE)(wxWindow *, int &);

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
    EVT_MENU(wxID_DISTINCT, DrawingView::OnDistinct)
    EVT_MENU(wxID_RETRIEVEARGS, DrawingView::OnRetrievalArguments)
    EVT_MENU(wxID_DATASOURCE, DrawingView::OnDataSource)
    EVT_MENU(wxID_DESIGNTABORDER, DrawingView::OnTabOrder)
    EVT_UPDATE_UI(wxID_PREVIEDWQUERY, DrawingView::OnQueryPreviewUpdateUI)
    EVT_MENU(wxID_SHOWSQLTOOLBOX, DrawingView::OnShowSQLBox)
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
    m_page6 = nullptr;
    m_fieldText = nullptr;
    m_fontName = nullptr;
    m_fontSize = nullptr;
    if( !wxView::OnCreate( doc, flags ) )
        return false;
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
    wxPoint ptCanvas;
    mainSizer = new wxBoxSizer( wxHORIZONTAL );
    sizer = new wxBoxSizer( wxVERTICAL );
#ifdef __WXOSX__
    wxBoxSizer *macTBSizer = new wxBoxSizer( wxVERTICAL );
    wxRect parentRect = m_parent->GetRect();
    wxSize parentClientSize = m_parent->GetClientSize();
    wxPoint pt;
    pt.x = -1;
    pt.y = parentRect.height - parentClientSize.GetHeight();
    m_frame->SetSize( pt.x, pt.y, parentRect.GetWidth(), /*parentRect.GetHeight() - parent->GetToolBar()->GetSize().GetHeight()*/parentClientSize.GetHeight() );
    m_tb = new wxToolBar( m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_TOP, "Second Toolbar" );
    if( m_type == QueryView )
    {
        m_styleBar = new wxToolBar( m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_TOP, "StyleBar" );
    }
    if( m_type == DatabaseView )
    {
        m_tb->AddTool( wxID_DATABASEWINDOW, _( "Database Profile" ), wxBitmap( database_profile ), wxBitmap( database_profile ), wxITEM_NORMAL, _( "DB Profile" ), _( "Select database profile" ) );
        m_tb->AddTool( wxID_SELECTTABLE, _( "Select Table" ), wxBitmap( table ), wxBitmap( table ), wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" ) );
        m_tb->AddTool( wxID_DROPOBJECT, _( "Drop" ), wxBitmap( cut_xpm ), wxBitmap( cut_xpm ), wxITEM_NORMAL, _( "Drop" ), _( "Drop database Object" ) );
        m_tb->AddTool( wxID_PROPERTIES, _( "Properties" ), wxBitmap( properties ), wxBitmap( properties ), wxITEM_NORMAL, _( "Properties" ), _( "Proerties" ) );
        m_tb->AddTool( wxID_CLOSE, _( "Close View" ), wxBitmap( quit_xpm ), wxBitmap( quit_xpm ), wxITEM_NORMAL, _( "Close Database View" ), _( "Close Database View" ) );
    }
    else
    {
        m_tb->AddTool( wxID_NEW, _( "New" ), wxBitmap( new_xpm ), wxBitmap( new_xpm ), wxITEM_NORMAL, _( "New" ), _( "New Query" ) );
        m_tb->AddTool( wxID_OPEN, _( "Open" ), wxBitmap( open_xpm ), wxBitmap( open_xpm ), wxITEM_NORMAL, _( "Open" ), _( "Open Query" ) );
        m_tb->AddTool( wxID_SAVE, _( "Save" ), wxBitmap( save_xpm ), wxBitmap( save_xpm ), wxITEM_NORMAL, _( "Save" ), _( "Save Query" ) );
        m_tb->AddTool( wxID_SHOWSQLTOOLBOX, _( "Show ToolBox" ), wxBitmap( toolbox), wxBitmap( toolbox ), wxITEM_CHECK, _( "Toolbox" ), _( "Hide/Show SQL Toolbox" ) );
        m_tb->AddTool( wxID_DATASOURCE, _( "Preview SQL" ), wxBitmap::NewFromPNGData( sql, WXSIZEOF( sql ) ), wxBitmap::NewFromPNGData( sql, WXSIZEOF( sql ) ), wxITEM_CHECK, _( "Data Source" ), _( "" ) );
        m_tb->AddTool( wxID_CLOSE, _( "Close View" ), wxBitmap( quit_xpm ), wxBitmap( quit_xpm ), wxITEM_NORMAL, _( "Close" ), _( "Close Query View" ) );
        m_tb->ToggleTool( wxID_SHOWSQLTOOLBOX, true );
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
        m_styleBar->AddTool( 303, _( "Bold" ), wxBitmap::NewFromPNGData( bold_png,  WXSIZEOF( bold_png ) ), wxNullBitmap, wxITEM_NORMAL );
        m_styleBar->AddTool( 303, _( "Italic" ), wxBitmap::NewFromPNGData( italic_png,  WXSIZEOF( italic_png ) ), wxNullBitmap, wxITEM_NORMAL );
        m_styleBar->AddTool( 303, _( "Underline" ), wxBitmap::NewFromPNGData( underline_png,  WXSIZEOF( underline_png ) ), wxNullBitmap, wxITEM_NORMAL );
    }
    m_tb->Realize();
    m_tb->SetSize( 0, 0, parentRect.GetWidth(), wxDefaultCoord );
    if( m_styleBar )
    {
        m_styleBar->Realize();
        m_styleBar->SetSize( 0, m_tb->GetSize().y, parentRect.GetWidth(), wxDefaultCoord );
    }
    ptCanvas.x = 0;
    ptCanvas.y = m_tb->GetSize().GetHeight();
    if( m_styleBar )
        ptCanvas.y += m_styleBar->GetSize().GetHeight();
    macTBSizer->Add( m_tb, 0, wxEXPAND, 0 );
    if( m_styleBar )
        macTBSizer->Add( m_styleBar, 0, wxEXPAND, 0 );
    sizer->Add( macTBSizer, 0, wxEXPAND, 0 );
    m_frame->SetSize( 0, ptCanvas.y, parentRect.GetWidth(), parentClientSize.GetHeight() - ptCanvas.y );
#else
    ptCanvas = wxDefaultPosition;
#endif
    wxASSERT( m_frame == GetFrame() );
    if( m_type == QueryView )
    {
        m_fields = new FieldWindow( m_frame, 1, wxDefaultPosition, wxDefaultCoord );
        sizer->Add( m_fields, 0, wxEXPAND, 0 );
        m_fields->Show( false );
    }
    m_canvas = new DatabaseCanvas( this, ptCanvas );
    sizer->Add( m_canvas, 2, wxEXPAND, 0 );
    if( m_type == QueryView )
    {
        m_queryBook = new wxNotebook( m_frame, wxID_ANY );
        m_page2 = new WhereHavingPage( m_queryBook, GetDocument()->GetDatabase()->GetTableVector().GetDatabaseType(), GetDocument()->GetDatabase()->GetTableVector().GetDatabaseSubtype() );
        m_queryBook->AddPage( m_page2, _( "Where" ) );
        m_page4 = new WhereHavingPage( m_queryBook, GetDocument()->GetDatabase()->GetTableVector().GetDatabaseType(), GetDocument()->GetDatabase()->GetTableVector().GetDatabaseSubtype() );
        m_queryBook->AddPage( m_page4, _( "Having" ) );
        m_page6 = new SyntaxPropPage( m_queryBook );
        m_queryBook->AddPage( m_page6, _( "Syntax" ), true );
        sizer->Add( m_queryBook, 0, wxEXPAND, 0 );
        m_queryBook->Show( false );
        m_queryBook->Bind( wxEVT_NOTEBOOK_PAGE_CHANGED, &DrawingView::OnSQLNotebookPageChanged, this );
        m_designCanvas = new DesignCanvas( this, ptCanvas );
        mainSizer->Add( m_designCanvas, 1, wxEXPAND, 0 );
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
        mainSizer->Add( m_edit, 1, wxEXPAND, 0 );
        m_edit->Show( false );
    }
    mainSizer->Add( sizer, 1, wxEXPAND, 0 );
    m_frame->SetSizer( mainSizer );
    if( m_log )
        m_log->Bind( wxEVT_CLOSE_WINDOW, &DrawingView::OnCloseLogWindow, this );
    Bind( wxEVT_SET_TABLE_PROPERTY, &DrawingView::OnSetProperties, this );
    Bind( wxEVT_SET_FIELD_PROPERTY, &DrawingView::OnSetProperties, this );
    Bind( wxEVT_MENU, &DatabaseCanvas::OnDropTable, m_canvas, wxID_DROPOBJECT );
    m_frame->Bind( wxEVT_CHANGE_QUERY, &DrawingView::OnQueryChange, this );
#if defined __WXMSW__ || defined __WXGTK__
    CreateViewToolBar();
#endif
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
    return true;
}

DrawingView::~DrawingView()
{
    wxMenuBar *bar = m_parent->GetMenuBar();
    for( int i = bar->GetMenuCount() - 2; i > 0; i-- )
    {
        auto menu = bar->Remove( i );
        delete menu;
    }
#ifndef __WXOSX__
    delete m_tb;
    m_tb = nullptr;
    delete m_styleBar;
    m_styleBar = nullptr;
#endif
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
    if( m_type == QueryView )
    {
        if( !m_styleBar )
            m_styleBar = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, "StyleBar" );
        else
            m_styleBar->ClearTools();
    }
    if( m_type == DatabaseView )
    {
        m_tb->AddTool( wxID_DATABASEWINDOW, _( "Database Profile" ), wxBitmap( database_profile ), wxBitmap( database_profile ), wxITEM_NORMAL, _( "DB Profile" ), _( "Select database profile" ) );
        m_tb->AddTool( wxID_SELECTTABLE, _( "Select Table" ), wxBitmap( table ), wxBitmap( table ), wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" ) );
        m_tb->AddTool( wxID_DROPOBJECT, _( "Drop" ), wxBitmap( cut_xpm ), wxBitmap( cut_xpm ), wxITEM_NORMAL, _( "Drop" ), _( "Drop database Object" ) );
        m_tb->AddTool( wxID_PROPERTIES, _( "Properties" ), wxBitmap( properties ), wxBitmap( properties ), wxITEM_NORMAL, _( "Properties" ), _( "Proerties" ) );
        m_tb->AddTool( wxID_CLOSE, _( "Close View" ), wxBitmap( quit_xpm ), wxBitmap( quit_xpm ), wxITEM_NORMAL, _( "Close" ), _( "Close Database View" ) );
    }
    else
    {
        m_tb->AddTool( wxID_NEW, _( "New" ), wxBitmap( new_xpm ), wxBitmap( new_xpm ), wxITEM_NORMAL, _( "New" ), _( "New Query" ) );
        m_tb->AddTool( wxID_OPEN, _( "Open" ), wxBitmap( open_xpm ), wxBitmap( open_xpm ), wxITEM_NORMAL, _( "Open" ), _( "Open Query" ) );
        m_tb->AddTool( wxID_SAVE, _( "Save" ), wxBitmap( save_xpm ), wxBitmap( save_xpm ), wxITEM_NORMAL, _( "Save" ), _( "Save Query" ) );
        m_tb->AddTool( wxID_SHOWSQLTOOLBOX, _( "Show ToolBox" ), wxBitmap( toolbox), wxBitmap( toolbox ), wxITEM_CHECK, _( "Toolbox" ), _( "Hide/Show SQL Toolbox" ) );
        m_tb->AddTool( wxID_DATASOURCE, _( "Preview SQL" ), wxBitmap::NewFromPNGData( sql, WXSIZEOF( sql ) ), wxNullBitmap, wxITEM_CHECK, _( "Data Source" ), _( "" ) );
        m_tb->AddTool( wxID_CLOSE, _( "Close View" ), wxBitmap( quit_xpm ), wxBitmap( quit_xpm ), wxITEM_NORMAL, _( "Close" ), _( "Close Query View" ) );
        m_tb->ToggleTool( wxID_SHOWSQLTOOLBOX, true );
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
        m_styleBar->AddTool( 303, _( "Bold" ), wxBitmap::NewFromPNGData( bold_png,  WXSIZEOF( bold_png ) ), wxNullBitmap, wxITEM_NORMAL );
        m_styleBar->AddTool( 303, _( "Italic" ), wxBitmap::NewFromPNGData( italic_png,  WXSIZEOF( italic_png ) ), wxNullBitmap, wxITEM_NORMAL );
        m_styleBar->AddTool( 303, _( "Underline" ), wxBitmap::NewFromPNGData( underline_png,  WXSIZEOF( underline_png ) ), wxNullBitmap, wxITEM_NORMAL );
    }
    m_tb->Realize();
    if( m_styleBar )
        m_styleBar->Realize();
    wxMDIClientWindow *frame = (wxMDIClientWindow *) parent->GetClientWindow();
    m_tb->SetSize( 0, 0, size.x, wxDefaultCoord );
    offset = m_tb->GetSize().y;
    if( m_styleBar )
    {
        m_styleBar->SetSize( 0, offset, size.x, wxDefaultCoord );
        offset += m_styleBar->GetSize().y;
    }
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
    TableProperties *tableProperties = NULL;
    DatabaseTable *dbTable;
    int res = -1;
    ShapeList shapes;
    bool found = false;
    wxString command;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( wxSFRectShape ), shapes );
    wxString tableName, schemaName;
    MyErdTable *erdTable = NULL;
    FieldShape *field = NULL;
    int isLogOnly = event.GetInt();
    long type = event.GetExtraLong();
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end() && !found; ++it )
    {
        if( (*it)->IsSelected() )
        {
            if( type == 0 )
            {
                tableProperties = (TableProperties *) event.GetClientData();
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
    if( type == 0 )
        res = GetDocument()->GetDatabase()->SetTableProperties( &erdTable->GetTable(), *tableProperties, isLogOnly, const_cast<std::wstring &>( command.ToStdWstring() ), errors );
    if( type == 1 )
        res = GetDocument()->GetDatabase()->SetFieldProperties( const_cast<DatabaseTable &>( erdTable->GetTable() ).GetTableName(), const_cast<DatabaseTable &>( erdTable->GetTable() ).GetTableOwner(), field->GetField()->GetFieldName(), field->GetField(), isLogOnly, const_cast<std::wstring &>( command.ToStdWstring() ), errors );
    if( res )
    {
        for( std::vector<std::wstring>::iterator it = errors.begin(); it < errors.end(); it++ )
            wxMessageBox( (*it) );
        event.SetString( "Failed" );
        return;
    }
    else
    {
        if( isLogOnly )
        {
            m_text->AppendText( command );
            m_text->AppendText( "\n\r\n\r" );
            if( !m_log->IsShown() )
                m_log->Show();
        }
        else
        {
            if( type == 0 )
            {
                dbTable = const_cast<DatabaseTable *>( &((MyErdTable *) erdTable)->GetTable() );
                Database *db = GetDocument()->GetDatabase();
                {
#if defined __WXMSW__ && _MSC_VER < 1900
                    wxCriticalSectionLocker( *pcs );
#else
//#if _MSC_VER >= 1900 || !(defined __WXMSW__)
                    std::lock_guard<std::mutex> lock( db->GetTableVector().my_mutex );
#endif
                    db->GetTableProperties( dbTable, errors );
                }
                erdTable->SetTableComment( dbTable->GetComment() );
                erdTable->Update();
                erdTable->Refresh();
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
    int res = -1;
    wxString query;
    std::vector<wxString> tables;
    wxDynamicLibrary lib;
    bool quickSelect = false;
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
                if( res == wxID_NEWOBJECT )
                {
                    NEWQUERY func1 = (NEWQUERY) lib.GetSymbol( "NewQueryDlg" );
                    res = func1( m_frame, m_source, m_presentation );
                }
                if( res != wxID_CANCEL )
                {
                    if( m_source != 1 )
                    {
                        wxMDIClientWindow *parent = (wxMDIClientWindow *) m_parent->GetClientWindow();
                        wxSize parentSize = parent->GetSize();
                        wxPoint parentPos = parent->GetPosition();
                        int heightStyleBar = m_styleBar->GetSize().y;
                        wxPoint framePosition = m_frame->GetPosition();
                        wxSize frameSize = m_frame->GetSize();
                        if( framePosition.y == 0 )
                        {
                            parent->SetSize( parentPos.x, parentPos.y - heightStyleBar, parentSize.GetWidth(), parentSize.GetHeight() + heightStyleBar );
                            m_frame->SetSize( frameSize.GetWidth(), frameSize.GetHeight() + heightStyleBar );
//                            m_frame->SetPosition( wxPoint( framePosition.x, framePosition.y - heightStyleBar ) );
//                            m_frame->SetSize( frameSize.GetWidth(), frameSize.GetHeight() + heightStyleBar );
                        }
                        SetQueryMenu( SQLSelectMenu );
                        m_frame->Freeze();
                        m_styleBar->Show( false );
                        m_designCanvas->Show( false );
                        m_fields->Show( true );
                        m_canvas->Show( true );
                        m_queryBook->Show( true );
                        m_frame->Layout();
                        sizer->Layout();
                        m_frame->Thaw();
                        TABLESELECTION func2 = (TABLESELECTION) lib.GetSymbol( "SelectTablesForView" );
                        res = func2( m_frame, db, tables, GetDocument()->GetTableNames(), false, m_type );
#ifdef __WXGTK__
						m_parent->SendSizeEvent();
						wxYield();
#endif
                    }
                    else
                    {
                        QUICKSELECT func2 = (QUICKSELECT) lib.GetSymbol( "QuickSelectDlg" );
                        res = func2( m_frame, db, m_selectTableName, m_queryFields );
                        GetDocument()->SetQueryFields( m_queryFields );
                        quickSelect = true;
                    }
/*                    if( quickSelect )
                    {
                        m_fields->Show( false );
                        m_canvas->Show( false );
                        m_queryBook->Show( false );
                        m_designCanvas->Show( true );
                    }*/
/*
#ifdef __WXGTK__
                    wxDocMDIParentFrame *parent = wxDynamicCast( m_frame->GetMDIParent(), wxDocMDIParentFrame );
                    wxSize size = parent->GetSize();
                    parent->SetSize( size.GetWidth() - 5, size.GetHeight() - 5 );
                    parent->SetSize( size.GetWidth() + 5, size.GetHeight() + 5 );
#endif
*/                }
            }
            else
            {
                TABLESELECTION func2 = (TABLESELECTION) lib.GetSymbol( "SelectTablesForView" );
                res = func2( m_frame, db, tables, GetDocument()->GetTableNames(), false, m_type );
            }
        }
        else
        {
            TABLESELECTION func2 = (TABLESELECTION) lib.GetSymbol( "SelectTablesForView" );
            res = func2( m_frame, db, tables, GetDocument()->GetTableNames(), false, m_type );
            m_canvas->Show( true );
            sizer->Layout();
        }
        if( m_type == QueryView )
        {
            if( res != wxID_CANCEL )
            {
                std::vector<std::wstring> queryFields = GetDocument()->GetQueryFields();
                query = "SELECT ";
                if( !quickSelect && queryFields.size() == 0 )
                    query += "<unknown fields>\n";
                else
                {
                    if( !quickSelect )
                    {
                        for( std::vector<std::wstring>::iterator it = queryFields.begin(); it < queryFields.end(); it++ )
                        {
                            query += (*it);
                            if( it != queryFields.end() - 1 )
                                query += ",";
                        }
                        query += "\n";
                    }
                    else
                    {
                        for( std::vector<Field *>::iterator it = m_queryFields.begin(); it < m_queryFields.end(); ++it )
                        {
                            query += (*it)->GetFieldName();
                            if( it != m_queryFields.end() - 1 )
                                query += ",";
                        }
                        query += "\n";
                    }
                }
                query += "FROM ";
            }
        }
        if( quickSelect && m_selectTableName.size() > 0 )
        {
            if( db->GetTableVector().GetDatabaseType() == L"SQLite" )
            {
                wxString name = m_selectTableName[0]->GetTableName();
                tables.push_back( name.substr( name.find_last_of( '.' ) + 1 ) );
            }
            else
                tables.push_back( m_selectTableName[0]->GetTableName() );
        }
    }
    ((DrawingDocument *) GetDocument())->AddTables( tables );
    ((DatabaseCanvas *) m_canvas)->DisplayTables( tables, query );
    if( m_type == QueryView )
    {
        if( query != L"\n" )
        {
            m_page6->SetSyntaxText( query );
            m_edit->SetText( query );
        }
        if( quickSelect && m_selectTableName.size() == 1 )
            m_canvas->AddQuickQueryFields( m_selectTableName[0]->GetTableName(), m_queryFields, quickSelect );
        if( quickSelect && m_selectTableName.size() > 0 )
        {
            wxBeginBusyCursor();
            wxFontStyle labelStyle = m_selectTableName[0]->GetLabelFontItalic() == 0 ? wxFONTSTYLE_NORMAL : wxFONTSTYLE_ITALIC;
            wxFontWeight labelWeight = ( ( m_selectTableName[0]->GetLabelFontWeight() == 0 ) ? wxFONTWEIGHT_NORMAL : wxFONTWEIGHT_BOLD );
            wxFontStyle dataStyle = m_selectTableName[0]->GetDataFontItalic()  == 0 ? wxFONTSTYLE_NORMAL : wxFONTSTYLE_ITALIC;
            wxFontWeight dataWeight = m_selectTableName[0]->GetDataFontWeight() == 0 ? wxFONTWEIGHT_NORMAL : wxFONTWEIGHT_BOLD;
            for( std::vector<Field *>::iterator it = m_queryFields.begin(); it < m_queryFields.end(); ++it )
                m_designCanvas->AddFieldLabelToCanvas( *wxFont::New( m_selectTableName[0]->GetLabelFontSize(), wxFONTFAMILY_DEFAULT, labelStyle, labelWeight, m_selectTableName[0]->GetLabelFontUnderline(), m_selectTableName[0]->GetLabelFontName() ),
                                                  (*it) );
            m_designCanvas->AddHeaderDivider();
            for( std::vector<Field *>::iterator it = m_queryFields.begin(); it < m_queryFields.end(); ++it )
                m_designCanvas->AddFieldToCanvas( *wxFont::New( m_selectTableName[0]->GetDataFontSize(), wxFONTFAMILY_DEFAULT, dataStyle, dataWeight, m_selectTableName[0]->GetDataFontUnderline(), m_selectTableName[0]->GetDataFontName() ), (*it) );
            m_designCanvas->AddDataDivider();
            m_designCanvas->Update();
            m_designCanvas->InitialFieldSizing();
            m_designCanvas->Refresh();
            wxEndBusyCursor();
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
    if( !wxView::OnClose( deleteWindow ) )
        return false;

    Activate( false );

    if( deleteWindow )
    {
        GetFrame()->Destroy();
        SetFrame( NULL );
    }
    wxMDIClientWindow *parent = dynamic_cast<wxMDIClientWindow *>( mainWin->GetClientWindow() );
    wxWindowList children = parent->GetChildren();
    if( parent->GetChildren().size() == 0 )
    {
        delete m_tb;
        m_tb = NULL;
        wxSize clientSize = mainWin->GetClientSize();
        parent->SetSize( 0, 0, clientSize.x, clientSize.y );
    }
    else
        m_tb->ClearTools();
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
            dbTable = const_cast<DatabaseTable *>( &((MyErdTable *) *it)->GetTable() );
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
                db->CreateIndex( command.ToStdWstring(), indexName.ToStdWstring(), dbTable->GetSchemaName(), dbTable->GetTableName(), errors );
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
            dbTable = const_cast<DatabaseTable *>( &((MyErdTable *) *it)->GetTable() );
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
    GetTablesForView( GetDocument()->GetDatabase(), false );
}

void DrawingView::OnFieldProperties(wxCommandEvent &event)
{
    std::vector<std::wstring> errors;
    bool found = false;
    int type = 0;
    DatabaseTable *dbTable = NULL;
    Field *field = NULL;
    ShapeList shapes;
    wxString command = "";
    bool logOnly = false;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( wxSFRectShape ), shapes );
    wxString tableName, schemaName, ownerName;
    MyErdTable *erdTable = NULL;
    ConstraintSign *sign = NULL;
    Constraint *constraint = NULL;
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end() && !found; ++it )
    {
        if( event.GetId() == wxID_PROPERTIES )
        {
            if( (*it)->IsSelected() )
            {
                erdTable = (MyErdTable *)(*it);
                if( erdTable )
                {
                    dbTable = const_cast<DatabaseTable *>( &((MyErdTable *) *it)->GetTable() );
                    type = 0;
                    found = true;
                }
            }
        }
        if( event.GetId() == wxID_FIELDPROPERTIES )
        {
            field = ((FieldShape *) event.GetEventObject())->GetField();
            if( (*it)->IsSelected() )
            {
                MyErdTable *my_table = dynamic_cast<MyErdTable *>( *it );
                if( my_table )
                {
                    erdTable = my_table;
                    tableName = const_cast<DatabaseTable *>( &erdTable->GetTable() )->GetTableName();
                    schemaName = const_cast<DatabaseTable *>( &erdTable->GetTable() )->GetSchemaName();
                    ownerName = const_cast<DatabaseTable *>( &erdTable->GetTable() )->GetTableOwner();
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
            res = func( m_frame, GetDocument()->GetDatabase(), type, dbTable, command, logOnly, wxEmptyString, wxEmptyString, wxEmptyString, *pcs );
        if( type == 1 )
            res = func( m_frame, GetDocument()->GetDatabase(), type, field, command, logOnly, tableName, schemaName, ownerName, *pcs );
        if( res != wxID_CANCEL && logOnly )
        {
            m_text->AppendText( command );
            m_text->AppendText( "\n\r\n\r" );
            if( !m_log->IsShown() )
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
            if( m_tb )
            {
                m_tb->ClearTools();
                m_tb->Hide();
            }
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

void DrawingView::AddFieldToQuery(const FieldShape &field, bool isAdding, const std::wstring &tableName, bool quickSelect)
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
        if( !quickSelect )
            m_queryFields.push_back( fld );
        m_page6->SetSyntaxText( query );
        m_edit->SetText( query );
    }
    else
    {
        wxString temp1;
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
            temp1 = query.substr( query.Find( name ) + name.length() );
            if( temp == query )
            {
                temp = "SELECT ";
                temp1 = temp1.substr( 2 );
            }
            query = temp + temp1;
        }
        m_queryFields.erase( std::remove( m_queryFields.begin(), m_queryFields.end(), fld ), m_queryFields.end() );
        m_page6->SetSyntaxText( query );
        m_edit->SetText( query );
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
            FieldShape *field2add = (FieldShape *) node->GetData();
            if( field2add && isAdd ? !field2add->IsSelected() : field2add->IsSelected() )
            {
                field2add->Select( isAdd );
                AddFieldToQuery( *field2add, isAdd, tableName, false );
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
}

void DrawingView::OnDistinct(wxCommandEvent &event)
{
    wxString qry;
    wxString query = m_page6->GetSyntaxCtrl()->GetValue();
    wxTextCtrl *queryText = const_cast<wxTextCtrl *>( m_page6->GetSyntaxCtrl() );
    if( !dynamic_cast<wxMenu *>( event.GetEventObject() )->IsChecked( wxID_DISTINCT ) )
    {
        query.Replace( "SELECT ", "SELECT DISTINCT " );
    }
    else
    {
        query.Replace( "SELECT DISTINCT ", "SELECT " );
    }
    queryText->SetValue( query );
    m_edit->SetText( query );
}

wxFrame *DrawingView::GetLogWindow() const
{
    return m_log;
}

wxTextCtrl *DrawingView::GetTextLogger() const
{
    return m_text;
}

void DrawingView::SetSynchronisationObject(wxCriticalSection &cs)
{
    pcs = &cs;
}

void DrawingView::UpdateQueryFromSignChange(const QueryConstraint *type)
{
    auto res = true;
    auto sign = type->GetSign();
    auto query = m_page6->GetSyntaxCtrl()->GetValue();
    if( sign == 1 || sign == 2 )
    {
        auto result = query.substr( 0, query.find( "\n" ) + 1 );
        query = query.substr( query.find( "\n" ) + 1 );
        result += "FROM(";
        if( sign == 1 )
            result += const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + " LEFT OUTER JOIN " + type->GetRefTable() + " ON " + const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + "." + type->GetLocalColumn() + " = " + type->GetRefTable() + "." + const_cast<QueryConstraint *>( type )->GetRefColumn();
        else
            result += type->GetRefTable() + " LEFT OUTER JOIN " + const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + " ON " + type->GetRefTable() + "." + const_cast<QueryConstraint *>( type )->GetRefColumn() + " = " + const_cast<DatabaseTable *>( type->GetFKTable() )->GetTableName() + "." + type->GetLocalColumn();
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
    auto result = query.substr( 0, query.find( " WHERE " ) + 7 );
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
    m_page6->SetSyntaxText( result );
    m_edit->SetText( result );
}

void DrawingView::OnQueryChange(wxCommandEvent &event)
{
    wxString query = m_page6->GetSyntaxCtrl()->GetValue();
    if( event.GetEventObject () == m_page2 )
    {
        wxString wherePart = query.substr( query.find( "WHERE" ) );
        wherePart = wherePart.substr( 0, wherePart.find( "HAVING" ) );
        WhereHavingLines line = *(WhereHavingLines *) event.GetClientData();
        int pos = wherePart.find( line.m_old );
    }
}

void DrawingView::OnRetrievalArguments(wxCommandEvent &WXUNUSED(event))
{
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
            m_arguments.push_back( QueryArguments( 1, "", "" ) );
        RETRIEVEARGUMENTS func = (RETRIEVEARGUMENTS) lib->GetSymbol( "GetQueryArguments" );
        int res = func( m_frame, m_arguments, GetDocument()->GetDatabase()->GetTableVector().GetDatabaseType(), GetDocument()->GetDatabase()->GetTableVector().GetDatabaseSubtype() );
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
    int heightStyleBar = m_styleBar->GetSize().y;
    wxPoint framePosition = m_frame->GetPosition();
    wxSize frameSize = m_frame->GetSize();
    wxMDIClientWindow *parent = (wxMDIClientWindow *) m_parent->GetClientWindow();
    wxSize parentSize = parent->GetSize();
    wxPoint parentPos = parent->GetPosition();
    wxMenuBar *menuBar = m_parent->GetMenuBar();
    for( unsigned int i = menuBar->GetMenuCount() - 2; i < 0; --i )
        menuBar->Remove( i );
    if( m_type == QueryView )
    {
        if( event.IsChecked () )
        {
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
        }
        else
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
            sizer->Layout();
        }
    }
    wxMenuItem *dataSourceMenu = m_parent->GetMenuBar()->FindItem( wxID_DATASOURCE );
    if( dataSourceMenu )
    {
        dataSourceMenu->GetMenu()->Check( wxID_DATASOURCE, event.IsChecked() );
        m_tb->ToggleTool( wxID_DATASOURCE, event.IsChecked() );
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

void DrawingView::SetQueryMenu(const int queryType)
{
    wxMenuBar *bar = m_parent->GetMenuBar();
    if( queryType == SQLSelectMenu )
    {
        for( int i = bar->GetMenuCount() - 2; i > 0; i-- )
        {
            auto *menu = bar->Remove( i );
            delete menu;
        }
        auto *designMenu = new wxMenu;
        designMenu->Append( wxID_DATASOURCE, _( "Data Source" ), _( "Data Source" ), wxITEM_CHECK );
        m_tb->InsertTool( 3, wxID_SELECTTABLE, _( "Select Table" ), wxBitmap( table ), wxBitmap( table ), wxITEM_NORMAL, _( "Select Table" ), _( "Select Table" ) );
        m_tb->InsertTool( 5, wxID_PREVIEDWQUERY, _( "Preview" ), wxBitmap::NewFromPNGData( preview, WXSIZEOF( preview ) ), wxNullBitmap, wxITEM_CHECK, ( "Preview" ) );
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
        m_tb->ToggleTool( wxID_DATASOURCE, true );
        bar->Insert( 1, designMenu, _( "Design" ) );
        m_tb->Realize();
    }
    if( queryType == QuerySyntaxMenu )
    {
        for( int i = bar->GetMenuCount() - 2; i > 0; i-- )
        {
            auto *menu = bar->Remove( i );
            delete menu;
        }
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
        bar->Insert( 1, editMenu, _( "Edit" ) );
        bar->Insert( 2, searchMenu, _( "Search" ) );
    }
}

void DrawingView::OnQueryPreviewUpdateUI(wxUpdateUIEvent &event)
{
    if( m_queryFields.size() > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}

void DrawingView::OnShowSQLBox (wxCommandEvent &event)
{
    HideShowSQLBox( event.IsChecked() );
}

void DrawingView::OnConvertToSyntaxUpdateUI(wxUpdateUIEvent &event)
{
    if( m_queryFields.size() > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}

void DrawingView::OnConvertToSyntax(wxCommandEvent &WXUNUSED(event))
{
    SetQueryMenu( QuerySyntaxMenu );
    m_designCanvas->Show( false );
    m_fields->Show( false );
    m_canvas->Show( false );
    m_queryBook->Show( false );
    m_edit->Show( true );
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

void DrawingView::OnFindNext(wxCommandEvent &event)
{
    if( m_searchDirection )
        m_start = m_start++;
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

void DrawingView::OnGotoLine(wxCommandEvent &event)
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
        if( res == wxID_OK && lineNo < m_edit->GetLineCount() )
            m_edit->GotoLine( lineNo );
    }
}

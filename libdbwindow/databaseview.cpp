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
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/cmdproc.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "table.h"
#include "GridTableShape.h"
#include "FieldShape.h"
#include "MyErdTable.h"
#include "databasecanvas.h"
#include "databasedoc.h"
#include "databaseview.h"

const wxEventTypeTag<wxCommandEvent> wxEVT_SET_TABLE_PROPERTY( wxEVT_USER_FIRST + 1 );

typedef int (*TABLESELECTION)(wxDocMDIChildFrame *, Database *, std::vector<wxString> &, std::vector<std::wstring> &);
typedef int (*CREATEINDEX)(wxWindow *, DatabaseTable *, Database *, wxString &);
typedef int (*CREATEPROPERTIESDIALOG)(wxWindow *parent, Database *, int type, void *object, wxString &, bool, const wxString &, const wxString &);
typedef int (*CREATEFOREIGNKEY)(wxWindow *parent, DatabaseTable *, Database *);

// ----------------------------------------------------------------------------
// DrawingView implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(DrawingView, wxView);

wxBEGIN_EVENT_TABLE(DrawingView, wxView)
    EVT_MENU(wxID_SELECTTABLE, DrawingView::OnViewSelectedTables)
    EVT_MENU(wxID_OBJECTNEWINDEX, DrawingView::OnNewIndex)
    EVT_MENU(wxID_FIELDDEFINITION, DrawingView::OnFieldDefinition)
    EVT_MENU(wxID_FIELDPROPERTIES, DrawingView::OnFieldProperties)
    EVT_MENU(wxID_PROPERTIES, DrawingView::OnFieldProperties)
    EVT_MENU(wxID_FIELDPROPERTIES, DrawingView::OnFieldProperties)
    EVT_MENU(wxID_OBJECTNEWFF, DrawingView::OnForeignKey)
wxEND_EVENT_TABLE()

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool DrawingView::OnCreate(wxDocument *doc, long flags)
{
    if( !wxView::OnCreate( doc, flags ) )
        return false;
    wxDocMDIParentFrame *parent = wxStaticCast( wxTheApp->GetTopWindow(), wxDocMDIParentFrame );
    wxWindowList children = parent->GetChildren();
    bool found = false;
    int height = 0;
    wxToolBar *tb;
    for( wxWindowList::iterator it = children.begin(); it != children.end() && !found; it++ )
    {
        tb = wxDynamicCast( *it, wxToolBar );
        if( tb && tb->GetName() == "Second Toolbar" )
        {
            found = true;
            height = tb->GetSize().GetHeight();
        }
    }
    wxPoint start( 0, height );
    wxRect clientRect = parent->GetClientRect();
    clientRect.height -= height;
    m_frame = new wxDocMDIChildFrame( doc, this, parent, wxID_ANY, _T( "Database" ), /*wxDefaultPosition*/start, wxSize( clientRect.GetWidth(), clientRect.GetHeight() ) );
    m_log = new wxFrame( m_frame, wxID_ANY, _( "Activity Log" ), wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT );
    m_text = new wxTextCtrl( m_log, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY );
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
    m_canvas = new DatabaseCanvas( this );
    m_frame->Show();
    m_log->Bind( wxEVT_CLOSE_WINDOW, &DrawingView::OnCloseLogWindow, this );
    Bind( wxEVT_SET_TABLE_PROPERTY, &DrawingView::OnSetProperties, this );
    return true;
}

// Sneakily gets used for default print/preview as well as drawing on the
// screen.
void DrawingView::OnDraw(wxDC *dc)
{
    dc->SetPen( *wxBLACK_PEN );

    // simply draw all lines of all segments
    const DoodleSegments& segments = GetDocument()->GetSegments();
    for ( DoodleSegments::const_iterator i = segments.begin(); i != segments.end(); ++i )
    {
        const DoodleLines& lines = i->GetLines();
        for ( DoodleLines::const_iterator j = lines.begin(); j != lines.end(); ++j )
        {
            const DoodleLine& line = *j;

            dc->DrawLine( line.x1, line.y1, line.x2, line.y2 );
        }
    }
}

void DrawingView::OnSetProperties(wxCommandEvent &event)
{
    std::vector<std::wstring> errors;
    DatabaseTable *table;
    int res;
    ShapeList shapes;
    bool found = false;
    m_canvas->GetDiagramManager().GetShapes( CLASSINFO( wxSFRectShape ), shapes );
    wxString tableName, schemaName;
    MyErdTable *erdTable = NULL;
    int isLogOnly = event.GetInt();
    int type = event.GetExtraLong();
    wxString *command = (wxString *) event.GetClientData();
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end() && !found; ++it )
    {
        if( type == 0 )
        {
            if( (*it)->IsSelected() )
            {
                erdTable = (MyErdTable *)(*it);
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

//std::vector<Table> &DrawingView::GetTablesForView(Database *db)
void DrawingView::GetTablesForView(Database *db)
{
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
        TABLESELECTION func = (TABLESELECTION) lib.GetSymbol( "SelectTablesForView" );
        int res = func( m_frame, db, tables, GetDocument()->GetTableNames() );
        if( res != wxID_CANCEL )
        {
            ((DrawingDocument *) GetDocument())->AddTables( tables );
            ((DatabaseCanvas *) m_canvas)->DisplayTables( tables );
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
    if( !wxView::OnClose( deleteWindow ) )
        return false;

    Activate( false );

    if( deleteWindow )
    {
        GetFrame()->Destroy();
        SetFrame( NULL );
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

void DrawingView::OnForeignKey(wxCommandEvent &event)
{
    int result;
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
        CREATEFOREIGNKEY func = (CREATEFOREIGNKEY) lib.GetSymbol( "CreateForeignKey" );
        result = func( m_frame, table, GetDocument()->GetDatabase() );
    }
    else
        wxMessageBox( _( "Error loading the DLL/so" ) );
}

void DrawingView::OnViewSelectedTables(wxCommandEvent &WXUNUSED(event))
{
    GetTablesForView( GetDocument()->GetDatabase() );
}

void DrawingView::OnFieldDefinition(wxCommandEvent &WXUNUSED(event))
{
    wxMessageBox( "Field definition" );
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
            if( (*it)->IsSelected() )
            {
                MyErdTable *table = dynamic_cast<MyErdTable *>( *it );
                if( table )
                {
                    erdTable = table;
                    tableName = const_cast<DatabaseTable *>( &erdTable->GetTable() )->GetTableName();
                    schemaName = const_cast<DatabaseTable *>( &erdTable->GetTable() )->GetSchemaName();
                    continue;
                }
                if( !table )
                {
                    field = dynamic_cast<Field *>( ((FieldShape *) *it)->GetField() );
                    type = 1;
                    found = true;
                    (*it)->Select( false );
                    erdTable->UpdateTable();
//                    m_canvas->GetDiagramManager().UpdateAll();
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
        int res;
        CREATEPROPERTIESDIALOG func = (CREATEPROPERTIESDIALOG) lib.GetSymbol( "CreatePropertiesDialog" );
        if( type == 0 )
            res = func( m_frame, GetDocument()->GetDatabase(), type, table, command, logOnly, wxEmptyString, wxEmptyString );
        if( type == 1 )
            res = func( m_frame, GetDocument()->GetDatabase(), type, field, command, logOnly, tableName, schemaName );
/*        if( res != wxID_CANCEL && logOnly )
        {
            m_text->AppendText( command );
            m_text->AppendText( "\n\r\n\r" );
            if( !m_log->IsShown() )
                m_log->Show();
        }*/
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

// ----------------------------------------------------------------------------
// MyCanvas implementation
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
wxEND_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxView *view, wxWindow *parent) : wxScrolledWindow(parent ? parent : view->GetFrame())
{
    m_view = view;
    m_currentSegment = NULL;
    m_lastMousePos = wxDefaultPosition;

    SetCursor( wxCursor( wxCURSOR_PENCIL ) );

    // this is completely arbitrary and is done just for illustration purposes
    SetVirtualSize( 1000, 1000 );
    SetScrollRate( 20, 20 );

    SetBackgroundColour( *wxWHITE );
}

MyCanvas::~MyCanvas()
{
    delete m_currentSegment;
}

// Define the repainting behaviour
void MyCanvas::OnDraw(wxDC& dc)
{
    if( m_view )
        m_view->OnDraw( &dc );
}

// This implements a tiny doodling program. Drag the mouse using the left
// button.
void MyCanvas::OnMouseEvent(wxMouseEvent& event)
{
    if( !m_view )
        return;

    wxClientDC dc( this );
    PrepareDC( dc );

    dc.SetPen( *wxBLACK_PEN );

    const wxPoint pt( event.GetLogicalPosition( dc ) );

    // is this the end of the current segment?
    if( m_currentSegment && event.LeftUp() )
    {
        if( !m_currentSegment->IsEmpty() )
        {
            // We've got a valid segment on mouse left up, so store it.
            DrawingDocument *const doc = wxStaticCast( m_view->GetDocument(), DrawingDocument );
            doc->GetCommandProcessor()->Submit( new DrawingAddSegmentCommand( doc, *m_currentSegment ) );
            doc->Modify( true );
        }
        wxDELETE( m_currentSegment );
    }

    // is this the start of a new segment?
    if( m_lastMousePos != wxDefaultPosition && event.Dragging() )
    {
        if( !m_currentSegment )
            m_currentSegment = new DoodleSegment;

        m_currentSegment->AddLine( m_lastMousePos, pt );

        dc.DrawLine( m_lastMousePos, pt );
    }

    m_lastMousePos = pt;
}

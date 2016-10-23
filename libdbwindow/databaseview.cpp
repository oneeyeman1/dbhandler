// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include <string>
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/cmdproc.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "table.h"
#include "GUIColumn.h"
#include "MyErdTable.h"
#include "databasecanvas.h"
#include "databasedoc.h"
#include "databaseview.h"

typedef void (*TABLESELECTION)(wxDocMDIChildFrame *, Database *, std::vector<wxString> &);

// ----------------------------------------------------------------------------
// DrawingView implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(DrawingView, wxView);

wxBEGIN_EVENT_TABLE(DrawingView, wxView)
    EVT_MENU(wxID_CUT, DrawingView::OnCut)
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
    for( wxWindowList::iterator it = children.begin(); it != children.end() && !found; it++ )
    {
        wxToolBar *tb = wxDynamicCast( *it, wxToolBar );
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
#ifdef __WXOSX__
    wxRect parentRect = parent->GetRect();
    wxSize parentClientSize = parent->GetClientSize();
    wxPoint pt;
    pt.x = -1;
    pt.y = parentRect.height - parentClientSize.GetHeight();
    m_frame->Move( pt.x, pt.y );
#endif
    wxASSERT( m_frame == GetFrame() );
    m_canvas = new DatabaseCanvas( this );
    m_frame->Show();
	Bind( wxEVT_CONTEXT_MENU, &DrawingView::OnContextMenu, this );
    return true;
}

void DrawingView::OnContextMenu(wxContextMenuEvent &event)
{
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
        func( m_frame, db, tables );
        ((DrawingDocument *) GetDocument())->AddTables( tables );
		((DatabaseCanvas *) m_canvas)->DisplayTables( tables );
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

void DrawingView::OnCut(wxCommandEvent& WXUNUSED(event))
{
    DrawingDocument * const doc = GetDocument();

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


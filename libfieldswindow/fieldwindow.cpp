/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/mainframe.cpp
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/window.h"
#include "wx/sizer.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/TextShape.h"
#include "field.h"
#include "fieldwindow.h"

FieldWindow::FieldWindow(wxWindow *parent, int type, const wxPoint &pos, int width) : wxSFShapeCanvas()
{
    m_isDragging = false;
    Create( parent, wxID_ANY, pos == wxDefaultPosition ? wxDefaultPosition : pos, wxSize( width == -1 ? parent->GetSize().GetWidth() : width, 55 ), wxBORDER_SIMPLE | wxHSCROLL | wxALWAYS_SHOW_SB );
    m_startPoint.x = 10;
    m_startPoint.y = 10;
    m_manager.SetRootItem( new xsSerializable() );
    SetDiagramManager( &m_manager );
    SetVirtualSize( 1000, 30 );
    SetScrollRate( 20, 0 );
    SetCanvasColour( *wxWHITE );
//    m_win->SetStyle( 0 );
    Bind( wxEVT_LEFT_DOWN, &FieldWindow::OnLeftDown, this );
}

FieldWindow::~FieldWindow(void)
{
}

void FieldWindow::AddField(const wxString &fieldName)
{
    FieldWin *field = new FieldWin( wxRealPoint( m_startPoint.x, m_startPoint.y ), fieldName, m_manager );
    m_manager.AddShape( field, NULL, m_startPoint, sfINITIALIZE );
    Refresh();
    m_startPoint.x += field->GetBoundingBox().GetWidth() + 5;
}

void FieldWindow::RemoveField(const std::vector<std::wstring> &names)
{
// tried to implement this with shifting the fields but it didn't work
// hopefully someone can make it work and improve this algorythm
    m_startPoint.x = 10;
    m_manager.Clear();
    for( std::vector<std::wstring>::const_iterator it = names.begin(); it < names.end(); it++ )
    {
        FieldWin *field = new FieldWin( wxRealPoint( m_startPoint.x, m_startPoint.y ), (*it), m_manager );
        m_manager.AddShape( field, NULL, m_startPoint, sfINITIALIZE );
        m_startPoint.x += field->GetBoundingBox().GetWidth() + 5;
    }
    Refresh();
}

void FieldWindow::RemoveField(const std::vector<wxString> &names)
{
// tried to implement this with shifting the fields but it didn't work
// hopefully someone can make it work and improve this algorythm
    m_startPoint.x = 10;
    m_manager.Clear();
    for( std::vector<wxString>::const_iterator it = names.begin(); it < names.end(); it++ )
    {
        FieldWin *field = new FieldWin( wxRealPoint( m_startPoint.x, m_startPoint.y ), (*it), m_manager );
        m_manager.AddShape( field, NULL, m_startPoint, sfINITIALIZE );
        m_startPoint.x += field->GetBoundingBox().GetWidth() + 5;
    }
    Refresh();
}

void FieldWindow::Clear()
{
    m_manager.Clear();
    m_startPoint.x = 10;
    m_startPoint.y = 10;
    Refresh();
}

void FieldWindow::OnLeftDown(wxMouseEvent &event)
{
    ShapeList shapes, allShapes;
    GetSelectedShapes( allShapes );
    GetShapesAtPosition( event.GetPosition(), shapes );
    for( ShapeList::iterator it = allShapes.begin (); it != allShapes.end (); ++it )
    {
        FieldWin *field = wxDynamicCast( *it, FieldWin );
        if( field )
            field->Select( false );
    }
    Refresh();
    for( ShapeList::iterator it = shapes.begin();  it != shapes.end(); ++it )
    {
        FieldWin *field = wxDynamicCast( *it, FieldWin );
        if( field )
            field->Select( true );
    }
}

void FieldWindow::OnMouseMove(wxMouseEvent &event)
{
    if( event.Dragging() )
    {
        m_draggingField = dynamic_cast<FieldWin *>( GetShapeAtPosition( event.GetPosition() ) );
        if( m_draggingField )
        {
            m_initialDraggerPosition = m_draggingField->GetBoundingBox();
            m_isDragging = true;
            wxLogDebug( "Dragging" );
        }
    }
}

void FieldWindow::OnLeftUp(wxMouseEvent &event)
{
    FieldWin *field = dynamic_cast<FieldWin *>( GetShapeAtPosition( event.GetPosition() ) );
    m_isDragging = false;
}
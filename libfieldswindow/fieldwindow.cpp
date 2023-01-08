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
#include "wx/docmdi.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/TextShape.h"
#include "field.h"
#include "fieldwindow.h"

const wxEventTypeTag<wxCommandEvent> wxEVT_FIELD_SHUFFLED( wxEVT_USER_FIRST + 4 );

FieldWindow::FieldWindow(wxWindow *parent, int type, const wxPoint &pos, int width) : wxSFShapeCanvas()
{
    m_draggingField = nullptr;
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
    m_selectedFields.push_back( fieldName );
}

void FieldWindow::RemoveField(const wxString &name)
{
    // tried to implement this with shifting the fields but it didn't work
    // hopefully someone can make it work and improve this algorythm
    m_startPoint.x = 10;
    m_manager.Clear();
    m_selectedFields.erase( std::remove_if( m_selectedFields.begin(), m_selectedFields.end(), 
                           [name](wxString element)
                           {
                               return element == name;
                           } ) );
    for( auto it = 0; it < m_selectedFields.size(); ++it )
    {
        FieldWin *field = new FieldWin( wxRealPoint( m_startPoint.x, m_startPoint.y ), ( m_selectedFields.at( it ) ), m_manager );
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
        FieldWin *shape = dynamic_cast<FieldWin *>( GetShapeAtPosition( event.GetPosition() ) );
        if( shape && !m_draggingField )
        {
            m_draggingField = shape;
            m_initialDraggerPosition = m_draggingField->GetBoundingBox();
        }
        if( m_draggingField )
            m_isDragging = true;
    }
}

void FieldWindow::OnLeftUp(wxMouseEvent &event)
{
    Direction dirs = FieldWindow::UNINITIALIZED;
    wxPoint pos = event.GetPosition();
    FieldWin *shape = dynamic_cast<FieldWin *>( GetShapeAtPosition( pos ) );
    if( !m_draggingField && shape )
    {
        shape->Select( false );
        Refresh();
        return;
    }
    if( m_isDragging )
    {
        if( !shape )
        {
            if( m_draggingField->GetFieldName() == m_selectedFields.at( m_selectedFields.size() - 1 ) )
            {
                m_draggingField->Select( false );
                Refresh();
                return;
            }
            else
            {
                m_selectedFields.erase( std::remove( m_selectedFields.begin(), m_selectedFields.end(), m_draggingField->GetFieldName() ), m_selectedFields.end() );
                m_startPoint.x = 10;
                wxString draggingField = m_draggingField->GetFieldName();
                m_manager.Clear();
                for( std::vector<wxString>::iterator it = m_selectedFields.begin (); it < m_selectedFields.end (); ++it )
                {
                    FieldWin *field = new FieldWin( wxRealPoint( m_startPoint.x, m_startPoint.y ), (*it), m_manager );
                    m_manager.AddShape( field, NULL, m_startPoint, sfINITIALIZE );
                    m_startPoint.x += field->GetBoundingBox().GetWidth() + 5;
                }
                FieldWin *field = new FieldWin( wxRealPoint( m_startPoint.x, m_startPoint.y ), draggingField, m_manager );
                m_manager.AddShape( field, NULL, m_startPoint, sfINITIALIZE );
                m_selectedFields.push_back( field->GetFieldName() );
                Refresh();
            }
        }
        else
        {
            wxRect shapeRect = shape->GetBoundingBox();
            int shapeMiddle = ( ( shapeRect.GetRight() - shapeRect.GetLeft() ) / 2 ) + shape->GetBoundingBox().GetLeft();
            if( ( pos.x > shapeRect.GetLeft() && pos.x < shapeMiddle && m_initialDraggerPosition.GetRight() + 6 == shapeRect.GetLeft() ) ||
                ( pos.x < shapeRect.GetRight() && pos.x > shapeMiddle && shapeRect.GetRight() + 6 == m_initialDraggerPosition.GetLeft() ) )
            {
                m_draggingField->Select( false );
                Refresh();
                return;
            }
            else
            {
                if( event.GetPosition().x > shapeMiddle && event.GetPosition().x < shapeRect.GetRight() + 5 )
                    dirs = FieldWindow::AFTER;
                if( event.GetPosition().x < shapeMiddle && event.GetPosition().x > shapeRect.GetLeft() - 5 )
                    dirs = FieldWindow::BEFORE;
                m_selectedFields.erase( std::remove( m_selectedFields.begin(), m_selectedFields.end(), m_draggingField->GetFieldName() ), m_selectedFields.end() );
                std::vector<wxString>::iterator it = std::find( m_selectedFields.begin(), m_selectedFields.end(), shape->GetFieldName() );
                if( dirs == FieldWindow::AFTER )
                    m_selectedFields.insert( it + 1, m_draggingField->GetFieldName() );
                if( dirs == FieldWindow::BEFORE )
                    m_selectedFields.insert( it - 1, m_draggingField->GetFieldName() );
                m_startPoint.x = 10;
                m_manager.Clear();
                for( std::vector<wxString>::iterator it = m_selectedFields.begin (); it < m_selectedFields.end (); ++it )
                {
                    FieldWin *field = new FieldWin( wxRealPoint( m_startPoint.x, m_startPoint.y ), (*it), m_manager );
                    m_manager.AddShape( field, NULL, m_startPoint, sfINITIALIZE );
                    m_startPoint.x += field->GetBoundingBox().GetWidth() + 5;
                }
            }
//            shape->Select( false );
        }
        wxCommandEvent event( wxEVT_FIELD_SHUFFLED );
        event.SetClientObject( (wxClientData *) &m_selectedFields );
        ProcessEvent( event );
        m_isDragging = false;
        m_draggingField = nullptr;
    }
    else
        if( shape )
            shape->Select( false );
    Refresh( false );
}

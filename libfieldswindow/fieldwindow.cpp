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

FieldWindow::FieldWindow(wxWindow *parent, int type)
{
    m_win = new wxSFShapeCanvas();
    m_win->Create( parent, wxID_ANY, wxDefaultPosition, wxSize( parent->GetSize().GetWidth(), 55 ), wxBORDER_SIMPLE | wxHSCROLL );
    m_startPoint.x = 10;
    m_startPoint.y = 10;
    m_manager.SetRootItem( new xsSerializable() );
    m_win->SetDiagramManager( &m_manager );
    m_win->SetVirtualSize( 1000, 1000 );
    m_win->SetScrollRate( 20, 20 );
    m_win->SetCanvasColour( *wxWHITE );
}

FieldWindow::~FieldWindow(void)
{
}

wxSFShapeCanvas *FieldWindow::GetFieldsWindow()
{
    return m_win;
}

void FieldWindow::AddField(const wxString &fieldName)
{
    FieldWin *field = new FieldWin( wxRealPoint( m_startPoint.x, m_startPoint.y ), fieldName, m_manager );
    m_manager.AddShape( field, NULL, m_startPoint, sfINITIALIZE );
    m_win->Refresh();
    m_startPoint.x += field->GetBoundingBox().GetWidth() + 5;
}

void FieldWindow::RemoveField(const wxString &fieldName)
{
    ShapeList shapes;
    m_manager.GetShapes( CLASSINFO( FieldWin ), shapes );
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end(); ++it )
    {
        if( ((FieldWin *) *it)->GetFieldName() == fieldName )
        {
            wxRect rect = (*it)->GetBoundingBox();
            m_manager.RemoveShape( (*it) );
            for( ShapeList::iterator it1 = ++it; it1 != shapes.end(); ++it1 )
            {
                m_manager.AddShape( (*it1), NULL, wxPoint( rect.GetLeft(), rect.GetTop() ), sfINITIALIZE );
                m_win->Refresh();
				rect.x += (*it1)->GetBoundingBox().GetWidth() + 5;
            }
        }
    }
}

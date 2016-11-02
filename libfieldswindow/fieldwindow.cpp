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
#include "fieldwindow.h"

FieldWindow::FieldWindow(wxWindow *parent, int type)
{
	m_win = new wxWindow( parent, wxID_ANY, wxDefaultPosition, wxSize( parent->GetSize().GetWidth(), 50 ), wxBORDER_SIMPLE/* | wxHSCROLL*/ );
    m_win->SetBackgroundColour( *wxWHITE );
    m_startPoint.x = 10;
    m_startPoint.y = 10;
    if( type == 1 )
    {
        m_sizer = new wxBoxSizer( wxVERTICAL );
        m_sizer->Add( m_win, 0, wxEXPAND, 0 );
        m_columnsScrollbar = new wxScrollBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_HORIZONTAL );
        m_sizer->Add( m_columnsScrollbar, 0, wxEXPAND, 0 );
    }
}

FieldWindow::~FieldWindow(void)
{
    delete m_win;
    m_win = NULL;
}

wxBoxSizer *FieldWindow::GetSizer()
{
    return m_sizer;
}

/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tableeditview.cc
 * Copyright (C) 2020 Igor Korot <igor@IgorReinCloud>
 *
 * dbhandler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * dbhandler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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

#include "wx/docview.h"
#include "wx/docmdi.h"
#include "tableeditview.h"

bool TableEditView::OnCreate(wxDocument *doc, long flags)
{
    wxToolBar *tb = nullptr;
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
    m_frame = new wxDocMDIChildFrame( doc, this, m_parent, wxID_ANY, "Data Management for ", wxDefaultPosition, wxSize( clientRect.GetWidth(), clientRect.GetHeight() ) );
    wxPoint ptCanvas;
    sizer = new wxBoxSizer( wxVERTICAL );
#ifdef __WXOSX__
    wxRect parentRect = m_parent->GetRect();
    wxSize parentClientSize = m_parent->GetClientSize();
    m_frame->Move( 0, parentRect.y - parentClientSize.y );
    wxPoint pt;
    pt.x = -1;
    pt.y = parentRect.height - parentClientSize.GetHeight();
    m_frame->SetSize( pt.x, pt.y, parentRect.GetWidth(), parentClientSize.GetHeight() );
    m_tb = m_frame->CreateToolBar();
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
        m_fieldText = new wxTextCtrl( m_styleBar, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
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
    int offset = m_tb->GetSize().y;
    if( m_styleBar )
    {
        m_styleBar->Realize();
        sizer->Add( m_styleBar, 1, wxEXPAND, 0 );
    }
    ptCanvas.x = 0;
    ptCanvas.y = offset;
#else
    ptCanvas = wxDefaultPosition;
#endif
    return true;
}

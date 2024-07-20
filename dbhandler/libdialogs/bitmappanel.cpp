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

#ifdef __WXGTK__
#include "gtk/gtk.h"
#if !GTK_CHECK_VERSION(3,6,0)
#include "wx/generic/stattextg.h"
#endif
#endif
#include "bitmappanel.h"

BitmapPanel::BitmapPanel(wxWindow *parent, const wxBitmap &bitmap, const wxString &label, int index) : wxPanel(parent), m_index(index)
{
    m_bitmap = new wxStaticBitmap( this, wxID_ANY, bitmap );
#ifdef __WXGTK__
#if GTK_CHECK_VERSION(3,6,0)
    m_label = new wxStaticText( this, wxID_ANY, label );
#else
    m_label = new wxGenericStaticText( this, wxID_ANY, label );
#endif
#else
    m_label = new wxStaticText( this, wxID_ANY, label );
#endif
    do_layout();
    m_bitmap->Bind( wxEVT_LEFT_DOWN, &BitmapPanel::OnBitmapClicked, this );
    m_label->Bind( wxEVT_LEFT_DOWN, &BitmapPanel::OnLabelClicked, this );
}

void BitmapPanel::do_layout()
{
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer2 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer3 = new wxBoxSizer( wxVERTICAL );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 20, 0, wxEXPAND, 0 );
    sizer3->Add( m_bitmap, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 0 );
    sizer3->Add( 5, 5, wxEXPAND, 0 );
    sizer3->Add( m_label, 0, wxALIGN_CENTER_HORIZONTAL, 0 );
    sizer3->Add( 5, 20, wxEXPAND, 0 );
    sizer2->Add( sizer3, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer1 );
}

#ifdef __WXGTK__
#if GTK_CHECK_VERSION(3,6,0)
wxStaticText *BitmapPanel::GetLabel()
#else
wxGenericStaticText *BitmapPanel::GetLabel()
#endif
#else
wxStaticText *BitmapPanel::GetLabel()
#endif
{
    return m_label;
}

void BitmapPanel::OnBitmapClicked(wxMouseEvent &event)
{
    event.ResumePropagation( wxEVENT_PROPAGATE_MAX );
    event.SetEventObject( this );
    event.Skip();
}

void BitmapPanel::OnLabelClicked(wxMouseEvent &event)
{
    event.ResumePropagation( wxEVENT_PROPAGATE_MAX );
    event.SetEventObject( this );
    event.Skip();
}

int BitmapPanel::GetIndex()
{
    return m_index;
}

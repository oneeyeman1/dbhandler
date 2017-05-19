/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/docview.cpp
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/stockitem.h"
#endif

#include "queryoptions.h"

QueryOptionsDialog::QueryOptionsDialog(wxWindow *parent, int optionsType) : wxDialog(parent, wxID_ANY, "")
{
    m_panel = new wxPanel( this );
    m_options = new wxListBox( m_panel, wxID_ANY );
    m_ok = new wxButton( m_panel, wxID_ANY, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_ANY, _( "Cancel" ) );
    m_help = new wxButton( m_panel, wxID_ANY, _( "Help" ) );
    m_ok->SetDefault();
    wxBoxSizer *main = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *hmargin = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *vmargin = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *buttons = new wxBoxSizer( wxVERTICAL );
    vmargin->Add( 5, 5, 0, wxEXPAND, 0 );
    hmargin->Add( 5, 5, 0, wxEXPAND, 0 );
    hmargin->Add( m_options, 0, wxEXPAND, 0 );
    buttons->Add( m_ok, 0, wxEXPAND, 0 );
    buttons->Add( 5, 5, 0, wxEXPAND, 0 );
    buttons->Add( m_cancel, 0, wxEXPAND, 0 );
    buttons->Add( 5, 5, 0, wxEXPAND, 0 );
    buttons->Add( m_help, 0, wxEXPAND, 0 );
    hmargin->Add( 5, 5, 0, wxEXPAND, 0 );
    hmargin->Add( buttons, 0, wxEXPAND, 0 );
    hmargin->Add( 5, 5, 0, wxEXPAND, 0 );
    vmargin->Add( hmargin, 0, wxEXPAND, 0 );
    vmargin->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( hmargin );
    main->Add( m_panel, 0, wxEXPAND, 0 );
    SetSizer( main );
    main->Fit( this );
    Layout();
}
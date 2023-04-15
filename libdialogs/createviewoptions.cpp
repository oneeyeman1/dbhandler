//
//  createviewoptions.cpp
//  libdialogs
//
//  Created by Igor Korot on 4/15/23.
//  Copyright © 2023 Igor Korot. All rights reserved.
//
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "createviewoptions.h"

CreateViewOptions::CreateViewOptions(wxWindow *parent) : wxDialog( parent, wxID_ANY, "")
{
    SetTitle( _( "Create View" ) );
    auto mainSizer = new wxBoxSizer( wxHORIZONTAL );
    auto rightleft = new wxBoxSizer( wxHORIZONTAL );
    auto topbottom = new wxBoxSizer( wxVERTICAL );
    auto sizer     = new wxBoxSizer( wxVERTICAL );
    auto buttons   = new wxBoxSizer( wxVERTICAL );
    m_panel = new wxPanel( this );
    m_temp = new wxCheckBox( m_panel, wxID_ANY, _( "Temp" ) );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _( "Help" ) );
    rightleft->Add( 5, 5, 0, wxEXPAND, 0 );
    topbottom->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer->Add( m_temp, 0, wxEXPAND, 0 );
    topbottom->Add( sizer, 0, wxEXPAND, 0 );
    buttons->Add( m_ok, 0, wxEXPAND, 0 );
    buttons->Add( 5, 5, 0, wxEXPAND, 0 );
    buttons->Add( m_cancel, 0, wxEXPAND, 0 );
    buttons->Add( 5, 5, 0, wxEXPAND, 0 );
    buttons->Add( m_help, 0, wxEXPAND, 0 );
    topbottom->Add( buttons, 0, wxEXPAND, 0 );
    topbottom->Add( sizer, 0, wxEXPAND, 0 );
    topbottom->Add( 5, 5, 0, wxEXPAND, 0 );
    rightleft->Add( topbottom, 0, wxEXPAND, 0 );
    rightleft->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( rightleft );
    mainSizer->Add( m_panel, 0, wxEXPAND, 0 );
    SetSizer( mainSizer );
    mainSizer->Fit( this );
    Layout();
}

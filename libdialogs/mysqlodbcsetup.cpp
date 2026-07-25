//
//  mysqlodbcsetup.cpp
//  libdialogs
//
//  Created by Igor Korot on 10/8/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if defined( __WXGTK__ ) || defined( __WXQT__ )
#include "logomysql.h"
#endif

mySQLODBCSetupDialog(wxWindow *parent, wxWindowID id, const wxString &title) : wxDialog( parent, wxID_ANY, "" )
{
    SetTitle( "MySQL ODBC Data Source Configuration" );
    auto sizer = new wxBoxSizer( wxHORIZONTAL );
    m_panel = new wxPanel( this );
    sizer->Add( m_panel, 0, wxEXPAND, 0 );
    auto sizer2 = new wxBoxSizer( wxHORIZONTAL );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer3 = new wxBoxSizer( wxVERTICAL );
    sizer2->Add( sizer3, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
#if defined( __WXGTK__ ) || defined( __WXQT__ )
    m_logo = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmapBundle::FromSVG() );
#else
    m_logo = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmapBundle::FromSVGResource() );
#endif
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer2 );
    SetSizer( sizer );
    sizer->Fit();
    Layout();
}


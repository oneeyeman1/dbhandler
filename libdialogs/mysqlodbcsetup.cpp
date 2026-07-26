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

#include "wx/statline.h"
#include "wx/collpane.h"
#include "mysqlodbcsetup.h"

mySQLODBCSetupDialog::mySQLODBCSetupDialog(wxWindow *parent, wxWindowID id, const wxString &title) : wxDialog( parent, wxID_ANY, "" )
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
    m_logo = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmapBundle::FromSVG( logomysql, wxSize( 16, 16 ) ) );
#else
    m_logo = new wxStaticBitmap( m_panel, wxID_ANY, wxBitmapBundle::FromSVGResource( "logomysql", wxSize( 16, 16 ) ) );
#endif
    sizer3->Add( m_logo, 1, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    m_line = new wxStaticLine( m_panel, wxID_ANY );
    sizer3->Add( m_line, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    m_label1 = new wxStaticText( m_panel, wxID_ANY, "Connection Parameters" );
    sizer3->Add( m_label1, 0, wxEXPAND, 0 );
    auto sizer4 = new wxFlexGridSizer( 2, 7, 5, 5 );
    sizer3->Add( sizer4, 0, wxEXPAND, 0 );
    m_label2 = new wxStaticText( m_panel, wxID_ANY, "Data Source Name:" );
    sizer4->Add( m_label2, 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_name = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer4->Add( m_name, 1, wxEXPAND, 0 );
    m_label3 = new wxStaticText( m_panel, wxID_ANY, "Description:" );
    sizer4->Add( m_label3, 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_desc = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer4->Add( m_desc, 1, wxEXPAND, 0 );
    m_network = new wxRadioButton( m_panel, wxID_ANY, "TCP/IP Server" );
    sizer4->Add( m_network, 0, wxALIGN_CENTER_VERTICAL, 0 );
    auto sizer5 = new wxBoxSizer( wxHORIZONTAL );
    sizer4->Add( sizer5, 0, wxRIGHT, 0 );
    m_serverName = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer5->Add( m_serverName, 0, wxEXPAND, 0 );
    sizer5->Add( 5, 5, 0, wxEXPAND, 0 );
    m_label4 = new wxStaticText( m_panel, wxID_ANY, "Port" );
    sizer5->Add( m_label4, 0, wxEXPAND, 0 );
    sizer5->Add( 5, 5, 0, wxEXPAND, 0 );
    m_port = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer5->Add( m_port, 0, wxEXPAND, 0 );
    m_pipe = new wxRadioButton( m_panel, wxID_ANY, "Named Pipe:" );
    sizer4->Add( m_pipe, 0, wxEXPAND, 0 );
    m_pipeName = new wxTextCtrl( m_panel, wxID_ANY, "" );
    m_pipeName->Enable( false );
    sizer4->Add( m_pipeName, 0, wxEXPAND, 0 );
    m_label5 = new wxStaticText( m_panel, wxID_ANY, "User" );
    sizer4->Add( m_label5, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 0 );
    m_user = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer4->Add( m_user, 1, wxEXPAND, 0 );
    m_label6 = new wxStaticText( m_panel, wxID_ANY, "Password" );
    sizer4->Add( m_label6, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 0 );
    m_password = new wxTextCtrl( m_panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    sizer4->Add( m_password, 1, wxEXPAND, 0 );
    m_label7 = new wxStaticText( m_panel, wxID_ANY, "Database Name" );
    sizer4->Add( m_label7, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 0 );
    auto sizer6 = new wxBoxSizer( wxHORIZONTAL );
    sizer4->Add( sizer6, 1, wxEXPAND, 0 );
    m_dbName = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer6->Add( m_dbName, 1, wxEXPAND, 0 );
    sizer6->Add( 5, 5, 0, wxEXPAND, 0 );
    m_test = new wxButton( m_panel, wxID_ANY, _( "Test" ) );
    sizer6->Add( m_test, 0, wxEXPAND, 0 );
    auto sizer7 = new wxBoxSizer( wxHORIZONTAL );
    sizer3->Add( sizer7, 0, wxEXPAND, 0 );
    m_details = new wxCollapsiblePane( m_panel, wxID_ANY, "Details" );
    sizer7->Add( m_details, 0, wxEXPAND, 0 );
    m_details->Bind( wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent &) { Layout(); } );
    auto win = m_details->GetPane();
    auto buttons = CreateStdDialogButtonSizer();
    sizer7->Add( buttons, 0, wxRIGHT, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer2 );
    SetSizer( sizer );
    sizer->Fit( this );
    Layout();
}


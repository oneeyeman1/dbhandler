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
#include "wx/notebook.h"
#include "mysqlodbcsetupconnection.h"
#include "mysqlodbcsetup.h"

mySQLODBCSetupDialog::mySQLODBCSetupDialog(wxWindow *parent, wxWindowID id, const wxString &title) : wxDialog( parent, wxID_ANY, "" )
{
    SetTitle( "MySQL ODBC Data Source Configuration" );
    auto sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer_2 = new wxBoxSizer( wxVERTICAL );
    sizer_1->Add( sizer_2, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer3 = new wxBoxSizer( wxVERTICAL );
    sizer_2->Add( sizer3, 0, wxEXPAND, 0 );
#if defined( __WXGTK__ ) || defined( __WXQT__ )
    m_logo = new wxStaticBitmap( this, wxID_ANY, wxBitmapBundle::FromSVG( logomysql, wxSize( 16, 16 ) ) );
#else
    m_logo = new wxStaticBitmap( this, wxID_ANY, wxBitmapBundle::FromSVGResource( "logomysql", wxSize( 16, 16 ) ) );
#endif
    sizer3->Add( m_logo, 1, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    m_line = new wxStaticLine( this, wxID_ANY );
    m_line->SetForegroundColour( wxColour( 0, 255, 255 ) );
    sizer3->Add( m_line, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, "Connection Parameters" ), wxVERTICAL );
    sizer3->Add( sizer4, 0, wxEXPAND, 0 );
    auto sizer5 = new wxFlexGridSizer( 7, 2, 5, 5 );
	sizer4->Add( sizer5, 0, wxEXPAND, 0 );
    m_label1 = new wxStaticText( sizer4->GetStaticBox(), wxID_ANY, "Data Source Name:" );
    sizer5->Add( m_label1, 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_name = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, "" );
    sizer5->Add( m_name, 1, wxEXPAND, 0 );
    m_label2 = new wxStaticText( sizer4->GetStaticBox(), wxID_ANY, "Description:" );
    sizer5->Add( m_label2, 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_desc = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, "" );
    sizer5->Add( m_desc, 1, wxEXPAND, 0 );
    m_network = new wxRadioButton( sizer4->GetStaticBox(), wxID_ANY, "TCP/IP Server" );
    sizer5->Add( m_network, 0, wxALIGN_CENTER_VERTICAL, 0 );
    auto sizer6 = new wxBoxSizer( wxHORIZONTAL );
    sizer5->Add( sizer6, 0, wxRIGHT, 0 );
    m_serverName = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, "" );
    sizer6->Add( m_serverName, 0, wxEXPAND, 0 );
    sizer6->Add( 5, 5, 0, wxEXPAND, 0 );
    m_label4 = new wxStaticText( sizer4->GetStaticBox(), wxID_ANY, "Port" );
    sizer6->Add( m_label4, 0, wxEXPAND, 0 );
    sizer6->Add( 5, 5, 0, wxEXPAND, 0 );
    m_port = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, "" );
    sizer6->Add( m_port, 0, wxEXPAND, 0 );
    m_pipe = new wxRadioButton( sizer4->GetStaticBox(), wxID_ANY, "Named Pipe:" );
    sizer5->Add( m_pipe, 0, wxEXPAND, 0 );
    m_pipeName = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, "" );
    m_pipeName->Enable( false );
    sizer5->Add( m_pipeName, 0, wxEXPAND, 0 );
    m_label5 = new wxStaticText( sizer4->GetStaticBox(), wxID_ANY, "User" );
    sizer5->Add( m_label5, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 0 );
    m_user = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, "" );
    sizer5->Add( m_user, 1, wxEXPAND, 0 );
    m_label6 = new wxStaticText( sizer4->GetStaticBox(), wxID_ANY, "Password" );
    sizer5->Add( m_label6, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 0 );
    m_password = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    sizer5->Add( m_password, 1, wxEXPAND, 0 );
    m_label7 = new wxStaticText( sizer4->GetStaticBox(), wxID_ANY, "Database Name" );
    sizer5->Add( m_label7, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 0 );
    auto sizer7 = new wxBoxSizer( wxHORIZONTAL );
    sizer5->Add( sizer7, 1, wxEXPAND, 0 );
    m_dbName = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, "" );
    sizer7->Add( m_dbName, 1, wxEXPAND, 0 );
    sizer7->Add( 5, 5, 0, wxEXPAND, 0 );
    m_test = new wxButton( sizer4->GetStaticBox(), wxID_ANY, _( "Test" ) );
    sizer7->Add( m_test, 0, wxEXPAND, 0 );
    m_detailsOptions = new wxNotebook( this, wxID_ANY );
    auto page1 = new MySQLODBCSetupConnection( m_detailsOptions );
    m_detailsOptions->AddPage( page1, _( "Connection" ) );
    m_detailsOptions->Hide();
    sizer3->Add( m_detailsOptions, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer8 = new wxBoxSizer( wxHORIZONTAL );
    sizer3->Add( sizer8, 0, wxEXPAND, 0 );
    m_details = new wxButton( this, 0, _( "Details >>" ) );
    sizer8->Add( m_details, 0, wxEXPAND, 0 );
    sizer8->AddStretchSpacer();
    auto *buttons = CreateStdDialogButtonSizer( wxOK | wxCANCEL | wxHELP );
	sizer8->Add( buttons, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    Layout();
}


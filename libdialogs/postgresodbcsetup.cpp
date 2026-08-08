//
//  mysqlodbcsetup.cpp
//  libdialogs
//
//  Created by Igor Korot on 10/8/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <map>
#include "postgresodbcsetup.h"

PostgresODBCSetupDialog::PostgresODBCSetupDialog(wxWindow *parent, std::map<std::wstring, std::wstring> &values) : wxDialog( parent, wxID_ANY, _( "PostgreSQL ODBC Driver (psqlODBC) Setup" ) )
{
    m_panel = new wxPanel( this );
    auto sizer = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add( m_panel, 0, wxEXPAND, 0 );
    auto sizer1 = new wxBoxSizer( wxHORIZONTAL );
    m_panel->SetSizer( sizer1 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer2 = new wxBoxSizer( wxVERTICAL );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer3 = new wxFlexGridSizer( 4, 4, 5, 5 );
    sizer2->Add( sizer3, 0, wxEXPAND, 0 );
    m_label1 = new wxStaticText( m_panel, wxID_ANY, _( "Data Source" ) );
    sizer3->Add( m_label1, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_dataSource = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer3->Add( m_dataSource, 0, wxEXPAND, 0 );
    m_label2 = new wxStaticText( m_panel, wxID_ANY, _( "Description" ) );
    sizer3->Add( m_label2, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_desc = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer3->Add( m_desc, 0, wxEXPAND, 0 );
    m_label3 = new wxStaticText( m_panel, wxID_ANY, _( "Database" ) );
    sizer3->Add( m_label3, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_dbName = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer3->Add( m_dbName, 0, wxEXPAND, 0 );
    m_label4 = new wxStaticText( m_panel, wxID_ANY, _( "SSL mode" ) );
    sizer3->Add( m_label4, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_ssl = new wxComboBox( m_panel, wxID_ANY );
    sizer3->Add( m_ssl, 0, wxEXPAND, 0 );
    m_label5 = new wxStaticText( m_panel, wxID_ANY, _( "Server" ) );
    sizer3->Add( m_label5, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_server = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer3->Add( m_server, 0, wxEXPAND, 0 );
    m_label6 = new wxStaticText( m_panel, wxID_ANY, _( "Port" ) );
    sizer3->Add( m_label6, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_port = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer3->Add( m_port, 0, wxEXPAND, 0 );
    m_label7 = new wxStaticText( m_panel, wxID_ANY, _( "User Name" ) );
    sizer3->Add( m_label7, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_name = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer3->Add( m_name, 0, wxEXPAND, 0 );
    m_label8 = new wxStaticText( m_panel, wxID_ANY, _( "Password" ) );
    sizer3->Add( m_label8, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_password = new wxTextCtrl( m_panel, wxID_ANY, "" );
    sizer3->Add( m_password, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer4 = new wxBoxSizer( wxHORIZONTAL );
    sizer2->Add( sizer4, 0, wxEXPAND, 0 );
    auto sizer5 = new wxStaticBoxSizer( new wxStaticBox( m_panel, wxID_ANY, _( "Options" ) ), wxHORIZONTAL );
    sizer4->Add( sizer5, 0, wxEXPAND, 0 );
    m_datasrc = new wxButton( sizer5->GetStaticBox(), wxID_ANY, _( "Datasource" ) );
    sizer5->Add( m_datasrc, 0, wxEXPAND, 0 );
    m_global = new wxButton( sizer5->GetStaticBox(), wxID_ANY, _( "Global" ) );
    sizer5->Add( m_global, 0, wxEXPAND, 0 );
    auto sizer6 = new wxBoxSizer( wxVERTICAL );
    sizer4->Add( sizer6, 0, wxEXPAND, 0 );
    m_test = new wxButton( m_panel, wxID_ANY, _( "Test" ) );
    sizer6->Add( m_test, 0, wxRIGHT, 0 );
    auto sizer7 = new wxBoxSizer( wxHORIZONTAL );
    sizer6->Add( sizer6, 0, wxRIGHT, 0 );
    m_save = new wxButton( m_panel, wxID_OK, _( "Save" ) );
    m_save->SetDefault();
    sizer7->Add( m_save, 0, wxEXPAND, 0 );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    sizer7->Add( m_cancel, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer );
    sizer->Fit( this );
    sizer->SetSizeHints( this );
    Layout();    
}


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

#include <map>
#include "wx/statline.h"
#include "wx/valnum.h"
#include "wx/collpane.h"
#include "wx/notebook.h"
#include <wx/filepicker.h>
#include "wx/process.h"
#include "mysqlodbcsetupconnection.h"
#include "mysqlodbcsetupmeta.h"
#include "mysqlodbcsetupcursor.h"
#include "mysqlodbcsetupdebug.h"
#include "mysqlodbcsetupssl.h"
#include "mysqlodbcsetupmisc.h"
#include "mysqlodbcsetup.h"

mySQLODBCSetupDialog::mySQLODBCSetupDialog(wxWindow *parent, std::map<std::wstring, std::wstring> &values) : wxDialog( parent, wxID_ANY, "" )
{
    SetTitle( "MySQL ODBC Data Source Configuration" );
    wxIntegerValidator<unsigned long> val( &m_value );
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
    m_name = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, values[L"Name"] );
    sizer5->Add( m_name, 1, wxEXPAND, 0 );
    m_label2 = new wxStaticText( sizer4->GetStaticBox(), wxID_ANY, "Description:" );
    sizer5->Add( m_label2, 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_desc = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, values[L"Description"] );
    sizer5->Add( m_desc, 1, wxEXPAND, 0 );
    m_network = new wxRadioButton( sizer4->GetStaticBox(), wxID_ANY, "TCP/IP Server" );
    m_network->Bind( wxEVT_RADIOBUTTON, &mySQLODBCSetupDialog::OnServer, this );
    sizer5->Add( m_network, 0, wxALIGN_CENTER_VERTICAL, 0 );
    auto sizer6 = new wxBoxSizer( wxHORIZONTAL );
    sizer5->Add( sizer6, 0, wxRIGHT, 0 );
    m_serverName = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, values[L"Server"] );
    sizer6->Add( m_serverName, 0, wxEXPAND, 0 );
    sizer6->Add( 5, 5, 0, wxEXPAND, 0 );
    m_label4 = new wxStaticText( sizer4->GetStaticBox(), wxID_ANY, "Port" );
    sizer6->Add( m_label4, 0, wxALIGN_CENTER_VERTICAL, 0 );
    sizer6->Add( 5, 5, 0, wxEXPAND, 0 );
    m_port = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val );
    sizer6->Add( m_port, 0, wxEXPAND, 0 );
    m_pipe = new wxRadioButton( sizer4->GetStaticBox(), wxID_ANY, "Named Pipe:" );
    m_pipe->Bind( wxEVT_RADIOBUTTON, &mySQLODBCSetupDialog::OnPipe, this );
    sizer5->Add( m_pipe, 0, wxEXPAND, 0 );
    m_pipeName = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, "" );
    m_pipeName->Enable( false );
    sizer5->Add( m_pipeName, 0, wxEXPAND, 0 );
    m_label5 = new wxStaticText( sizer4->GetStaticBox(), wxID_ANY, "User" );
    sizer5->Add( m_label5, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 0 );
    m_user = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, values[L"User"] == L"" ? values[L"UID"] : values[L"User"] );
    sizer5->Add( m_user, 1, wxEXPAND, 0 );
    m_label6 = new wxStaticText( sizer4->GetStaticBox(), wxID_ANY, "Password" );
    sizer5->Add( m_label6, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 0 );
    m_password = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, values[L"PWD"], wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    sizer5->Add( m_password, 1, wxEXPAND, 0 );
    m_label7 = new wxStaticText( sizer4->GetStaticBox(), wxID_ANY, "Database Name" );
    sizer5->Add( m_label7, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 0 );
    auto sizer7 = new wxBoxSizer( wxHORIZONTAL );
    sizer5->Add( sizer7, 1, wxEXPAND, 0 );
    m_dbName = new wxTextCtrl( sizer4->GetStaticBox(), wxID_ANY, values[L"Database"] );
    sizer7->Add( m_dbName, 1, wxEXPAND, 0 );
    sizer7->Add( 5, 5, 0, wxEXPAND, 0 );
    m_test = new wxButton( sizer4->GetStaticBox(), wxID_ANY, _( "Test" ) );
    m_test->Disable();
    m_test->Bind( wxEVT_UPDATE_UI, &mySQLODBCSetupDialog::OnTestUpdateUI, this );
    m_test->Bind( wxEVT_BUTTON, &mySQLODBCSetupDialog::OnTest, this );
    sizer7->Add( m_test, 0, wxEXPAND, 0 );
    m_detailsOptions = new wxNotebook( this, wxID_ANY );
    m_page1 = new MySQLODBCSetupConnection( m_detailsOptions, values );
    m_detailsOptions->AddPage( m_page1, _( "Connection" ) );
    m_page2 = new MySQLODBCSetupMeta( m_detailsOptions, values );
    m_detailsOptions->AddPage( m_page2, _( "Metadata" ) );
    m_page3 = new MySQLODBCSetupCursor( m_detailsOptions, values );
    m_detailsOptions->AddPage( m_page3, _( "Cursors/Results" ) );
    m_page4 = new MySQLODBCSetupDebug( m_detailsOptions, values );
    m_detailsOptions->AddPage( m_page4, _( "Debug" ) );
    m_page5 = new MySQLODBCSetupSSL( m_detailsOptions, values );
    m_detailsOptions->AddPage( m_page5, "SSL" );
    m_page6 = new MySQLODBCSetupMisc( m_detailsOptions, values );
    m_detailsOptions->AddPage( m_page6, "Misc" );
    m_detailsOptions->Hide();
    sizer3->Add( m_detailsOptions, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer8 = new wxBoxSizer( wxHORIZONTAL );
    sizer3->Add( sizer8, 0, wxEXPAND, 0 );
    m_details = new wxButton( this, 0, _( "Details >>" ) );
    m_details->Bind( wxEVT_BUTTON, &mySQLODBCSetupDialog::OnDetails, this );
    sizer8->Add( m_details, 0, wxEXPAND, 0 );
    sizer8->AddStretchSpacer();
    auto *buttons = CreateStdDialogButtonSizer( wxOK | wxCANCEL | wxHELP );
    FindWindowById( wxID_OK, this )->Bind( wxEVT_BUTTON, &mySQLODBCSetupDialog::OnOK, this );
	sizer8->Add( buttons, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    Layout();
}

void mySQLODBCSetupDialog::OnTestUpdateUI(wxUpdateUIEvent &event)
{
    if( m_name->GetValue().Length() > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}

void mySQLODBCSetupDialog::OnDetails(wxCommandEvent &event)
{
    if( !m_detailsShown )
	{
        m_detailsOptions->Show();
        m_details->SetLabel( _( "Details <<" ) );
    }
    else
    {
        m_detailsOptions->Hide();
        m_details->SetLabel( _( "Details >>" ) );
    }
    Fit();
    Layout();
    m_detailsShown = !m_detailsShown;
}

void mySQLODBCSetupDialog::OnTest(wxCommandEvent &event)
{
    wxString command = "mysql ";
    auto user = m_user->GetValue();
    command += "-u " + user;
    auto password = m_password->GetValue();
    command += " -p" + password;
    auto database = m_dbName->GetValue();
    command += " -D " + database;
    command += " -e do 1";
    auto process = new wxProcess();
    auto res = wxExecute( command, wxEXEC_SYNC, process );
}

void mySQLODBCSetupDialog::OnPipe(wxCommandEvent &event)
{
    m_pipeName->Enable( true );
    m_serverName->Enable( false );
    m_label4->Enable( false );
    m_port->Enable( false );
}

void mySQLODBCSetupDialog::OnServer(wxCommandEvent &event)
{
    m_pipeName->Enable( false );
    m_serverName->Enable( true );
    m_label4->Enable( true );
    m_port->Enable( true );
}

void mySQLODBCSetupDialog::OnOK(wxCommandEvent &event)
{
    m_data[L"Description"] = m_desc->GetValue().ToStdWstring();
    if( m_network->GetValue() )
    {
        m_data[L"Server"] = m_serverName->GetValue().ToStdWstring();
        m_data[L"Port"] = m_port->GetValue().ToStdWstring();
    }
    else
        m_data[L"Socket"] = m_pipeName->GetValue().ToStdWstring();
    m_data[L"UID"] = m_user->GetValue().ToStdWstring();
    m_data[L"PWD"] = m_password->GetValue().ToStdWstring();
    m_data[L"DATABASE"] = m_dbName->GetValue().ToStdWstring();
    if( m_page1->IsChanged() )
    {
        m_data[L"BIG_PACKETS"] = m_page1->Get1Check()->GetValue() ? L"1" : L"0";
        m_data[L"CAN_HANDLE_EXP_PWD"] = m_page1->Get3Check()->GetValue() ? L"1" : L"0";
        m_data[L"COMPRESSED_PROTO"] = m_page1->Get2Check()->GetValue() ? L"1" : L"0";
        m_data[L"ENABLE_CLEARTEXT_PLUGIN"] = m_page1->Get4Check()->GetValue() ? L"1" : L"0";
        m_data[L"AUTO_RECONNECT"] = m_page1->Get5Check()->GetValue() ? L"1" : L"0";
        m_data[L"GET_SERVER_PUBLIC_KEY"] = m_page1->Get6Check()->GetValue() ? L"1" : L"0";
        m_data[L"NO_PROMPT"] = m_page1->Get7Check()->GetValue() ? L"1" : L"0";
        m_data[L"ENABLE_DNS_SRV"] = m_page1->Get8Check()->GetValue() ? L"1" : L"0";
        m_data[L"MULTI_STATEMENTS"] = m_page1->Get9Check()->GetValue() ? L"1" : L"0";
        m_data[L"MULTI_HOST"] = m_page1->Get10Check()->GetValue() ? L"1" : L"0";
        m_data[L"INTERACTIVE"] = m_page1->Get11Check()->GetValue() ? L"1" : L"0";
        m_data[L"CHARSET"] = m_page1->GetCharSet()->GetValue().ToStdWstring();
        m_data[L"INITSTMT"] = m_page1->GetInitStmt()->GetValue().ToStdWstring();
        m_data[L"PLUGIN_DIR"] = m_page1->GetPluginDir()->GetPath().ToStdWstring();
        m_data[L"DEFAULT_AUTH"] = m_page1->GetAuth()->GetValue().ToStdWstring();
        m_data[L"OCI_CONFIG_FILE"] = m_page1->GetOCI()->GetFileName().GetFullPath().ToStdWstring();
    }
    if( m_page2->IsChanged() )
    {
        m_data[L"NO_BIGINT"] = m_page2->GetBigInt()->GetValue() ? L"1" : L"0";
        m_data[L"NO_BINARY_RESULT"] = m_page2->GetNoBinary()->GetValue() ? L"1" : L"0";
        m_data[L"FULL_COLUMN_NAMES"] = m_page2->GetFullName()->GetValue() ? L"1" : L"0";
        m_data[L"NO_CATALOG"] = m_page2->GetNoCatalog()->GetValue() ? L"1" : L"0";
        m_data[L"NO_SCHEMA"] = m_page2->GetNoSchema()->GetValue() ? L"1" : L"0";
        m_data[L"COLUMN_SIZE_S32"] = m_page2->GetLimitColumn()->GetValue() ? L"1" : L"0";
    }
    if( m_page3->IsChanged() )
    {
        m_data[L"DYNAMIC_CURSOR"] = m_page3->GetDynamicCursor()->GetValue() ? L"1" : L"0";
        m_data[L"NO_DEFAULT_CURSOR"] = m_page3->GetDriverCursor()->GetValue() ? L"1" : L"0";
        m_data[L"NO_CACHE"] = m_page3->GetCacheCursor()->GetValue() ? L"1" : L"0";
        m_data[L"FORWARD_CURSOR"] = m_page3->GetForwardOnlyCursor()->GetValue() ? L"1" : L"0";
        if( m_page3->GetPrefetchRows()->IsEnabled() )
            m_data[L"PREFETCH"] = m_page3->GetPrefetchRows()->GetValue().ToStdWstring();
        else
            m_data[L"PREFETCH"] = L"0";
        m_data[L"FOUND_ROWS"] = m_page3->GetMatchedRows()->GetValue() ? L"1" : L"0";
        m_data[L"AUTO_IS_NULL"] = m_page3->GetIsNULL()->GetValue() ? L"1" : L"0";
        m_data[L"PAD_SPACE"] = m_page3->GetPadSpace()->GetValue() ? L"1" : L"0";
        m_data[L"ZERO_DATE_TO_MIN"] = m_page3->GetZeroDate()->GetValue() ? L"1" : L"0";
    }
    if( m_page4->IsChanged() )
        m_data[L"LOG_QUERY"] = m_page4->GetLogQueries()->GetValue() ? L"1" : L"0";
    if( m_page5->IsChanged() )
    {
        m_data[L"SSLKEY"] = m_page5->GetSSLKey()->GetPath().ToStdWstring();
    }
    EndModal( wxID_OK );
}

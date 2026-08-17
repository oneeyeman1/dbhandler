/***************************************************************************
 *   Copyright (C) 2005 by Igor Korot                                      *
 *   igor@IgorsGentoo                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <map>
#include "wx/wx.h"
#include "wx/filepicker.h"
#include "mysqlodbcsetupssl.h"

MySQLODBCSetupSSL::MySQLODBCSetupSSL(wxWindow *parent, const std::map<std::wstring, std::wstring> &values) : wxPanel( parent )
{
    auto sizer = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer1 = new wxBoxSizer( wxVERTICAL );
    sizer->Add( sizer1, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer2 = new wxFlexGridSizer( 9, 2, 5, 5 );
    sizer1->Add( sizer2, 0, wxALIGN_CENTER_HORIZONTAL, 0 );
    m_label1 = new wxStaticText( this, wxID_ANY, _( "SSL Key" ) );
    sizer2->Add( m_label1, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_ssl = new wxFilePickerCtrl( this, wxID_ANY, "", _( "Select File" ) );
    if( values.find( L"SSLKEY" ) != values.end() )
        m_ssl->SetFileName( wxFileName( values.at( L"SSLKEY" ) ) );
    m_ssl->Bind( wxEVT_FILEPICKER_CHANGED, &MySQLODBCSetupSSL::DataChanged, this );
    sizer2->Add( m_ssl, 1, wxEXPAND, 0 );
    m_label2 = new wxStaticText( this, wxID_ANY, _( "SSL Certificate" ) );
    sizer2->Add( m_label2, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_sslCert = new wxFilePickerCtrl( this, wxID_ANY, "" );
    if( values.find( L"SSLCA" ) != values.end() )
        m_sslCert->SetFileName( wxFileName( values.at( L"SSLCA" ) ) );
    m_sslCert->Bind( wxEVT_FILEPICKER_CHANGED, &MySQLODBCSetupSSL::DataChanged, this );
    sizer2->Add( m_sslCert, 1, wxEXPAND, 0 );
    m_label3 = new wxStaticText( this, wxID_ANY, _( "SSL CA file" ) );
    sizer2->Add( m_label3, 0, wxEXPAND, 0 );
    m_sslCA = new wxFilePickerCtrl( this, wxID_ANY, "" );
    if( values.find( L"SSLCA" ) != values.end() )
        m_sslCA->SetFileName( wxFileName( values.at( L"SSLCA" ) ) );
    sizer2->Add( m_sslCA, 1, wxEXPAND, 0 );
    m_label4 = new wxStaticText( this, wxID_ANY, _( "SSL CA Path" ) );
    sizer2->Add( m_label4, 0, wxEXPAND, 0 );
    m_sslPath = new wxDirPickerCtrl( this, wxID_ANY, "" );
    sizer2->Add( m_sslPath, 1, wxEXPAND, 0 );
    m_label5 = new wxStaticText( this, wxID_ANY, _( "SSL Cipher" ) );
    sizer2->Add( m_label5, 0, wxEXPAND, 0 );
    m_sslCipher = new wxTextCtrl( this, wxID_ANY, "" );
    m_sslCipher->Bind( wxEVT_TEXT, &MySQLODBCSetupSSL::DataChanged, this );
    sizer2->Add( m_sslCipher, 0, wxEXPAND, 0 );
    m_label6 = new wxStaticText( this, wxID_ANY, _( "SSL Mode" ) );
    sizer2->Add( m_label6, 0, wxEXPAND, 0 );
    const wxString choices[] = {
        _( "DISABLED" ),
        _( "PREFERRED" ),
        _( "REQUIRED" ),
        _( "VERIFY_CA" ),
        _( "VERIFY_IDENTITY" )
    };
    m_sslMode = new wxComboBox( this, wxID_ANY, "" );
    m_sslMode->Bind( wxEVT_COMBOBOX, &MySQLODBCSetupSSL::DataChanged, this );
    sizer2->Add( m_sslMode, 1, wxEXPAND, 0 );
    m_label7 = new wxStaticText( this, wxID_ANY, _( "RSA Public key" ) );
    sizer2->Add( m_label7, 0, wxEXPAND, 0 );
    m_rsaKey = new wxFilePickerCtrl( this, wxID_ANY, "" );
    m_rsaKey->Bind( wxEVT_FILEPICKER_CHANGED, &MySQLODBCSetupSSL::DataChanged, this );
    sizer2->Add( m_rsaKey, 1, wxEXPAND, 0 );
    m_label8 = new wxStaticText( this, wxID_ANY, _( "SSL CRL File" ) );
    sizer2->Add( m_label8, 0, wxEXPAND, 0 );
    m_sslCRKFile = new wxFilePickerCtrl( this, wxID_ANY, "" );
    m_sslCRKFile->Bind( wxEVT_FILEPICKER_CHANGED, &MySQLODBCSetupSSL::DataChanged, this );
    sizer2->Add( m_sslCRKFile, 1, wxEXPAND, 0 );
    m_label9 = new wxStaticText( this, wxID_ANY, _( "SSL CRL Path" ) );
    sizer2->Add( m_label9, 0, wxEXPAND, 0 );
    m_sslCRLPath = new wxDirPickerCtrl( this, wxID_ANY, "" );
    sizer2->Add( m_sslCRLPath, 1, wxEXPAND, 0 );
    auto sizer3 = new wxBoxSizer( wxHORIZONTAL );
    sizer1->Add( sizer3, 0, wxEXPAND, 0 );
    m_disableTLS2 = new wxCheckBox( this, wxID_ANY, _( "Disable TLS version 1.2" ) );
    m_disableTLS2->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupSSL::DataChanged, this );
    sizer3->AddStretchSpacer();
    sizer3->Add( m_disableTLS2, 0, wxEXPAND, 0 );
    sizer3->AddStretchSpacer();
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer4 = new wxBoxSizer( wxHORIZONTAL );
    sizer1->Add( sizer4, 0, wxEXPAND, 0 );
    m_disableTLS3 = new wxCheckBox( this, wxID_ANY, _( "Disable TLS version 1.3" ) );
    m_disableTLS3->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupSSL::DataChanged, this );
    sizer4->AddStretchSpacer();
    sizer4->Add( m_disableTLS3, 0, wxEXPAND, 0 );
    sizer4->AddStretchSpacer();
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer );
    sizer->Fit( this );
}

void MySQLODBCSetupSSL::DataChanged(wxCommandEvent &event)
{
    m_changed = true;
}

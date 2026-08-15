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
    m_label1 = new wxStaticText( this, wxID_ANY, _( "SSL Key" ) );
    sizer2->Add( m_label1, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_ssl = new wxFilePickerCtrl( this, wxID_ANY, "", _( "Select File" ) );
    if( values.find( L"SSLKEY" ) != values.end() )
        m_ssl->SetFileName( wxFileName( values.at( L"SSLKEY" ) ) );
    m_ssl->Bind( wxEVT_FILEPICKER_CHANGED, &MySQLODBCSetupSSL::DataChanged, this );
    sizer2->Add( m_ssl, 0, wxEXPAND, 0 );
    m_label2 = new wxStaticText( this, wxID_ANY, _( "SSL Certificate" ) );
    sizer2->Add( m_label2, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 0 );
    m_sslCert = new wxFilePickerCtrl( this, wxID_ANY, "" );
    if( values.find( L"SSLKEY" ) != values.end() )
        m_sslCert->SetFileName( wxFileName( values.at( L"SSLKEY" ) ) );
    m_sslCert->Bind( wxEVT_FILEPICKER_CHANGED, &MySQLODBCSetupSSL::DataChanged, this );
    sizer2->Add( m_sslCert, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer );
    sizer->Fit( this );
}

void MySQLODBCSetupSSL::DataChanged(wxCommandEvent &event)
{
    m_changed = true;
}

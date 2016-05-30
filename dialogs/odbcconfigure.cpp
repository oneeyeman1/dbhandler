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
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <map>
#include <vector>
#ifdef _IODBCUNIX_H
#include "iODBC/sqlext.h"
#include "iODBC/odbcinst.h"
#else
#include <sqlext.h>
#include <odbcinst.h>
#endif
#include "wx/config.h"
#include "wx/dynlib.h"
#include "wx/vector.h"
#include "odbcconfigure.h"
/*#include "resource.h"
#include "table.h"*/

CODBCConfigure::CODBCConfigure(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
#ifdef __WXMSW__
    m_lib = new wxDynamicLibrary( "dbloader" );
#else
    m_lib = new wxDynamicLibrary( "libdbloader" );
#endif
    if( m_lib->IsLoaded() )
    {
        func = (CONVERTFROMSQLWCHAR) m_lib->GetSymbol( "ConvertFromSQLWCHAR" );
        func1 = (CONVERTFROMWXSTRING) m_lib->GetSymbol( "ConvertFromString" );
    }
    else
    {
        func = NULL;
        func1 = NULL;
    }
    style = style;
    panel_1 = new wxPanel( this, -1 );
    label_1 = new wxStaticText( panel_1, wxID_ANY, _( "Installed Drivers:" ) );
    m_drivers = new wxListBox( panel_1, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE | wxLB_SORT );
    m_OK = new wxButton( panel_1, wxID_OK, _( "&Close" ) );
    m_Help = new wxButton( panel_1, wxID_HELP, _( "&Help" ) );
    label_2 = new wxStaticText( panel_1, wxID_ANY, _( "Data Sources for Selected Drivers:" ) );
    m_dsn = new wxListBox( panel_1, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE | wxLB_ALWAYS_SB );
    m_createdsn = new wxButton( panel_1, wxID_ANY, _( "&Create..." ) );
    m_editdsn = new wxButton( panel_1, wxID_ANY, _( "&Edit..." ) );
    m_removedsn = new wxButton( panel_1, wxID_ANY, _( "&Remove..." ) );
	
    set_properties();
    do_layout();
}

CODBCConfigure::~CODBCConfigure()
{
    delete m_lib;
}

void CODBCConfigure::set_properties()
{
	std::vector<std::wstring> errorMsg;
    bool res = GetDriverList();
    if( !res )
    {
		for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
            wxMessageBox( (*it), _( "ODBC Configuration Error" ) );
        m_createdsn->Disable();
        m_editdsn->Disable();
        m_removedsn->Disable();
        return;
    }
    for( std::map<wxString,std::vector<wxString> >::iterator it1 = m_driversDSN.begin(); it1 != m_driversDSN.end(); it1++ )
 		m_drivers->Append( (*it1).first );
    if( m_drivers->GetCount() != 0 )
    {
        m_drivers->SetSelection( 0 );
        FillDSNComboBox();
    }
    if( m_dsn->GetCount() == 0 || m_dsn->GetSelection() == wxNOT_FOUND )
    {
        m_editdsn->Disable();
        m_removedsn->Disable();
    }
    m_OK->SetDefault();
    m_createdsn->Bind( wxEVT_BUTTON, &CODBCConfigure::OnCreateDSN, this );
    m_editdsn->Bind( wxEVT_BUTTON, &CODBCConfigure::OnEditDSN, this );
    m_removedsn->Bind( wxEVT_BUTTON, &CODBCConfigure::OnRemoveDSN, this );
    m_drivers->Bind( wxEVT_LISTBOX, &CODBCConfigure::OnDriverNameChange, this );
    m_dsn->Bind( wxEVT_LISTBOX, &CODBCConfigure::OnDsnNameChange, this );
}

void CODBCConfigure::do_layout()
{
    wxBoxSizer* sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer( 7, 5, 0, 0 );
    wxBoxSizer* sizer_3 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* sizer_2 = new wxBoxSizer( wxVERTICAL );
    grid_sizer_1->Add( 5, 5, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 20, 5, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 10, 5, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 20, 5, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 5, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( label_1, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 10, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 20, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( m_drivers, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 10, 20, 0, wxGROW | wxALL, 0 );
    sizer_2->Add( m_OK, 0, wxGROW | wxALL, 0 );
    sizer_2->Add( m_Help, 0, wxGROW | wxALL, 0 );
    sizer_2->Add( 20, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( sizer_2, 1, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 20, 15, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 10, 15, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 20, 15, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( label_2, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 10, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 20, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( m_dsn, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 10, 20, 0, wxGROW | wxALL, 0 );
    sizer_3->Add( m_createdsn, 0, wxGROW | wxALL, 0 );
    sizer_3->Add( m_editdsn, 0, wxGROW | wxALL, 0 );
    sizer_3->Add( m_removedsn, 0, wxGROW | wxALL, 0 );
    sizer_3->Add( 20, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( sizer_3, 1, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 20, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 5, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 20, 5, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 10, 5, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 20, 5, 0, wxGROW | wxALL, 0 );
    grid_sizer_1->Add( 5, 5, 0, wxGROW | wxALL, 0 );
    panel_1->SetAutoLayout( true );
    panel_1->SetSizer( grid_sizer_1 );
    grid_sizer_1->Fit( panel_1 );
    grid_sizer_1->SetSizeHints( panel_1 );
    sizer_1->Add( panel_1, 1, wxEXPAND, 0 );
    SetAutoLayout( true );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    sizer_1->SetSizeHints( this );
    Layout();
}

bool CODBCConfigure::GetDriverList()
{
    bool result = true;
    wxString s1, s2, error, dsn_result;
    SQLWCHAR driverDesc[1024], driverAttributes[256], dsn[SQL_MAX_DSN_LENGTH+1], sqlState[20], errMsg[SQL_MAX_MESSAGE_LENGTH], dsnDescr[255];
    SQLSMALLINT i = 1, attr_ret;
    SQLINTEGER nativeError;
    SWORD pcbDSN, pcbDesc;
    SQLSMALLINT cbErrorMsg, descriptionLength;
    SQLUSMALLINT direction = SQL_FETCH_FIRST, direct = SQL_FETCH_FIRST;
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HENV, &m_henv ), ret1, ret2;
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        result = false;
        while( ( ret = SQLGetDiagRec( SQL_HANDLE_ENV, m_henv, i, sqlState, &nativeError, errMsg, sizeof( errMsg ), &cbErrorMsg ) ) == SQL_SUCCESS )
        {
            func( errMsg, error );
            wxMessageBox( error );
            i++;
        }
    }
    else
    {
        ret = SQLSetEnvAttr( m_henv, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0 );
        if( ret != SQL_SUCCESS )
        {
            result = false;
            while( ( ret = SQLGetDiagRec( SQL_HANDLE_ENV, m_henv, i, sqlState, &nativeError, errMsg, sizeof( errMsg ), &cbErrorMsg ) ) == SQL_SUCCESS )
            {
                func( errMsg, error );
                wxMessageBox( error );
                i++;
            }
        }
        else
        {
            while( ( ret1 = SQLDrivers( m_henv, direction, driverDesc, sizeof( driverDesc ) - 1, &descriptionLength, driverAttributes, sizeof( driverAttributes ), &attr_ret ) ) == SQL_SUCCESS )
            {
                func( driverDesc, s1 );
                while( ( ret2 = SQLDataSources( m_henv, direct, dsn, SQL_MAX_DSN_LENGTH, &pcbDSN, dsnDescr, 254, &pcbDesc ) ) == SQL_SUCCESS )
                {
                    func( dsnDescr, s2 );
                    if( s1 == s2 )
                    {
                        func( dsn, dsn_result );
                        m_driversDSN[s1].push_back( dsn_result );
                    }
                    direct = SQL_FETCH_NEXT;
                }
                if( ret2 != SQL_SUCCESS && ret2 != SQL_NO_DATA )
                {
                    result = false;
                    while( ( ret = SQLGetDiagRec( SQL_HANDLE_ENV, m_henv, i, sqlState, &nativeError, errMsg, sizeof( errMsg ), &cbErrorMsg ) ) == SQL_SUCCESS )
                    {
                        func( errMsg, error );
                        wxMessageBox( error );
                        i++;
                    }
                    break;
                }
                else
                {
                    direction = SQL_FETCH_FIRST;
                    direct = SQL_FETCH_FIRST;
                }
                if( m_driversDSN.find( s1 ) == m_driversDSN.end() )
                    m_driversDSN[s1].push_back( L"" );
                if( ret2 == SQL_NO_DATA )
                    ret2 = SQL_SUCCESS;
                direction = SQL_FETCH_NEXT;
            }
            if( ret1 != SQL_SUCCESS && ret1 != SQL_NO_DATA )
            {
                result = false;
                while( ( ret = SQLGetDiagRec( SQL_HANDLE_ENV, m_henv, i, sqlState, &nativeError, errMsg, sizeof( errMsg ), &cbErrorMsg ) ) == SQL_SUCCESS )
                {
                    func( errMsg, error );
                    wxMessageBox( error );
                    i++;
                }
            }
        }
    }
    return result;
}

bool CODBCConfigure::AddDsn(WXWidget hwnd, const wxString &driver)
{
    bool result = true;
    wxString error;
    SQLWCHAR buf[256];
    SQLSMALLINT i = 1;
    DWORD nativeError;
    SQLWCHAR errMsg[SQL_MAX_MESSAGE_LENGTH];
    WORD cbErrorMsg;
    SQLWCHAR *temp1;
//    ConvertFromString( driver, temp1 );
temp1 = func1( driver );
    BOOL ret = SQLConfigDataSource( hwnd, ODBC_ADD_DSN, temp1, buf );
    if( !ret )
    {
        result = false;
        while( ( ret = SQLInstallerError( i, &nativeError, errMsg, SQL_MAX_MESSAGE_LENGTH - 1, &cbErrorMsg ) ) == SQL_SUCCESS )
        {
            func( errMsg, error );
            wxMessageBox( error );
            i++;
        }
    }
    else
        FillDSNComboBox();
    delete temp1;
    return result;
}

bool CODBCConfigure::EditDsn(WXWidget hwnd, const wxString &driver, const wxString &dsn)
{
    bool result = true;
    wxString error;
    SQLSMALLINT i = 1;
    DWORD nativeError;
    SQLWCHAR errMsg[SQL_MAX_MESSAGE_LENGTH];
    WORD cbErrorMsg;
	wxString temp = wxString::Format( "Driver = %s;%s", driver.c_str(), dsn.c_str() );
SQLWCHAR *temp1 = NULL;// = const_cast<SQLWCHAR *>( driver.wc_str() );
SQLWCHAR *temp2 = NULL;// = const_cast<SQLWCHAR *>( temp.wc_str() );
//    ConvertFromString( driver, temp1 );
//    ConvertFromString( temp, temp2 );
	BOOL ret= SQLConfigDataSource( hwnd, ODBC_CONFIG_DSN, temp1, temp2 );
    if( !ret )
    {
        result = false;
        while( ( ret = SQLInstallerError( i, &nativeError, errMsg, SQL_MAX_MESSAGE_LENGTH - 1, &cbErrorMsg ) ) == SQL_SUCCESS )
        {
            func( errMsg, error );
            wxMessageBox( error );
            i++;
        }
    }
    return result;
}

void CODBCConfigure::OnCreateDSN(wxCommandEvent &WXUNUSED(event))
{
    std::vector<char *> errorMsg;
    wxString driver = m_drivers->GetStringSelection();
    if( driver.IsEmpty() )
    {
        wxMessageBox( _( "No driver specified to create a Data Source Name!" ), _( "Error Creating DSN!" ), wxOK | wxICON_ERROR );
        return;
    };
	if( !AddDsn( this->GetHandle(), driver.c_str() ) )
    {
        for( std::vector<char *>::iterator it = errorMsg.begin(); it != errorMsg.end(); it++ )
        {
            wxString temp = wxMBConvUTF16().cMB2WC( (const char *) (*it) );
            wxMessageBox( temp );
//            char *temp = *(it);
//            wxString str( temp, wxConvLocal );
//            std::string s( temp );
            wxMessageBox( temp );
        }
        return;
    }
    if( m_dsn->GetCount() > 0 )
        m_dsn->Clear();
    FillDSNComboBox();
}

void CODBCConfigure::OnEditDSN(wxCommandEvent &WXUNUSED(event))
{
    std::vector<char *> errorMsg;
    wxString driver = m_drivers->GetStringSelection();
    wxString dsnStr = m_dsn->GetStringSelection();
    dsnStr = _T( "DSN=" ) + dsnStr + '\0';
    driver = driver + '\0';
	if( !EditDsn( this->GetHandle(), driver.c_str(), dsnStr.c_str() ) )
    {
        for( std::vector<char *>::iterator it = errorMsg.begin(); it != errorMsg.end(); it++ )
        {
            wxString temp = wxMBConvUTF16().cMB2WC( (const char *) (*it) );
            wxMessageBox( temp );
        }
        return;
    }
}

void CODBCConfigure::OnRemoveDSN(wxCommandEvent &WXUNUSED(event))
{
    std::vector<char *> errorMsg;
    wxString driver = m_drivers->GetStringSelection();
    wxString dsnStr = m_dsn->GetStringSelection();
    int res = wxMessageBox( _( "OK to delete " ) + dsnStr, _( "Configure ODBC" ), wxOK | wxCANCEL, this );
    if( res == wxOK )
    {
        dsnStr = _T( "DSN=" ) + dsnStr + '\0';
/*        if( !m_db->RemoveDSN( dsnStr, driver, (HWND ) this->GetHandle() ) )
        {
            event.Skip();
            return;
        }
        else*/
        {
			m_dsn->Delete( m_dsn->FindString( dsnStr ) );
            m_editdsn->Disable();
            m_removedsn->Disable();
        }
    }
}

void CODBCConfigure::OnDriverNameChange(wxCommandEvent &WXUNUSED(event))
{
    if( m_dsn->GetCount() > 0 )
        m_dsn->Clear();
    FillDSNComboBox();
}

void CODBCConfigure::OnDsnNameChange(wxCommandEvent &WXUNUSED(event))
{
    m_editdsn->Enable();
    m_removedsn->Enable();
}

void CODBCConfigure::FillDSNComboBox()
{
    wxString selDriver = m_drivers->GetStringSelection();
    for( std::map<wxString, std::vector<wxString> >::iterator it = m_driversDSN.begin(); it != m_driversDSN.end(); it++ )
    {
        wxString driver = (*it).first;
        if( selDriver == driver )
        {
            for( std::vector<wxString>::iterator it1 = (*it).second.begin(); it1 < (*it).second.end(); it1++ )
            {
//                wxString temp( (const char *) (*it1).c_str() );
                if( (*it1) != wxEmptyString )
                    m_dsn->Append( (*it1) );
            }
        }
    }
}
/*
SQLWCHAR *CODBCConfigure::ConvertFromString(wxString str)
{
	SQLWCHAR *s = new SQLWCHAR[str.length() * sizeof( SQLWCHAR )];
    SQLWCHAR *tmp;
#if defined(__WXMSW__) || defined( SQL_ATTR_UNIXODBC_VERSION )
    tmp = const_cast<SQLWCHAR *>( str.wc_str() );
#elif defined(  _IODBCUNIX_H )
    tmp = str.wc_str();
#endif
    wcscpy( s, tmp, sizeof( temp ) );
    return s;
}

void CODBCConfigure::ConvertFromString(const wxString &str, SQLWCHAR *string)
{
#if defined(__WXMSW__) || defined( SQL_ATTR_UNIXODBC_VERSION )
    string = const_cast<SQLWCHAR *>( str.wc_str() );
#elif defined(  _IODBCUNIX_H )
    string = str.wc_str();
#endif
}

wxString CODBCConfigure::ConvertFromSQLWCHAR(SQLWCHAR *str)
{
    wxString string;
#if defined(__WXMSW__) || defined( SQL_ATTR_UNIXODBC_VERSION )
    string = str;
#elif defined(  _IODBCUNIX_H )
    string = str.wc_str();
#endif
    return string;
}
*/
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
#include "wx/config.h"
#include "wx/dynlib.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "wx/vector.h"
#include "database.h"
#include "odbcconfigure.h"
/*#include "resource.h"
#include "table.h"*/

typedef Database *(*GETDRIVERLIST)(std::map<std::wstring, std::vector<std::wstring> > &, std::vector<std::wstring> &);
typedef int (*ADDNEWDSN)(Database *, wxWindow *, const wxString &);
typedef int (*EDITDSN)(Database *, wxWindow *, const wxString &, const wxString &);
typedef int (*DELETEDSN)(Database *, const wxString &, const wxString &);

CODBCConfigure::CODBCConfigure(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
    m_db = NULL;
	m_lib = new wxDynamicLibrary();
#ifdef __WXMSW__
    m_lib->Load( "dbloader" );
#elif __WXOSX__
    wxFileName fn( wxStandardPaths::Get().GetExecutablePath() );
    auto path = fn.GetPath() + wxFileName::GetPathSeparator() + ".." + wxFileName::GetPathSeparator() + "Frameworks" + wxFileName::GetPathSeparator();
    m_lib->Load( path + "liblibdbloader.dylib" );
#else
    m_lib->Load( "libdbloader" );
#endif
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
    delete m_db;
    m_db = NULL;
    delete m_lib;
    m_lib = NULL;
}

void CODBCConfigure::set_properties()
{
    std::vector<std::wstring> errorMsg;
    GETDRIVERLIST func = (GETDRIVERLIST) m_lib->GetSymbol( "GetDriverList" );
    m_db = func( m_driversDSN, errorMsg );
    if( !m_db )
    {
        m_createdsn->Disable();
        m_editdsn->Disable();
        m_removedsn->Disable();
        return;
    }
    for( std::map<std::wstring,std::vector<std::wstring> >::iterator it1 = m_driversDSN.begin(); it1 != m_driversDSN.end(); it1++ )
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

void CODBCConfigure::OnCreateDSN(wxCommandEvent &WXUNUSED(event))
{
    std::vector<char *> errorMsg;
    wxString driver = m_drivers->GetStringSelection();
    if( driver.IsEmpty() )
    {
        wxMessageBox( _( "No driver specified to create a Data Source Name!" ), _( "Error Creating DSN!" ), wxOK | wxICON_ERROR );
        return;
    };
    ADDNEWDSN func = (ADDNEWDSN) m_lib->GetSymbol( "AddNewDSN" );
    if( func( m_db, this, driver ) )
    {
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
    EDITDSN func = (EDITDSN) m_lib->GetSymbol( "EditDSN" );
    if( func( m_db, this, driver, dsnStr ) )
    {
        return;
    }
}

void CODBCConfigure::OnRemoveDSN(wxCommandEvent &event)
{
    std::vector<char *> errorMsg;
    wxString driver = m_drivers->GetStringSelection();
    wxString dsnStr = m_dsn->GetStringSelection();
    int res = wxMessageBox( _( "OK to delete " ) + dsnStr, _( "Configure ODBC" ), wxOK | wxCANCEL, this );
    if( res == wxOK )
    {
        DELETEDSN func = (DELETEDSN) m_lib->GetSymbol( "RemoveDSN" );
        dsnStr = "DSN=" + dsnStr + '\0';
        if( !func( m_db, driver, dsnStr ) )
        {
            event.Skip();
            return;
        }
        else
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
    for( std::map<std::wstring, std::vector<std::wstring> >::iterator it = m_driversDSN.begin(); it != m_driversDSN.end(); it++ )
    {
        wxString driver = (*it).first;
        if( selDriver == driver )
        {
            for( std::vector<std::wstring>::iterator it1 = (*it).second.begin(); it1 < (*it).second.end(); it1++ )
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

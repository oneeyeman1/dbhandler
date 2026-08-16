/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * mysqlodbcsetupmisc.cpp
 * Copyright (C) 2026 Igor Korot <igor@IgorReinCloud>
 *
 * dbhandler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * dbhandler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <map>
#include "wx/wx.h"
#include "wx/stdpaths.h"
#include "wx/filepicker.h"
#include "mysqlodbcsetupmisc.h"

MySQLODBCSetupMisc::MySQLODBCSetupMisc(wxWindow *parent, const std::map<std::wstring, std::wstring> &values) : wxPanel( parent )
{
    auto sizer = new wxBoxSizer( wxHORIZONTAL );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer1 = new wxBoxSizer( wxVERTICAL );
    sizer->Add( sizer1, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer2 = new wxFlexGridSizer( 9, 2, 5, 5 );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    m_safeOptions = new wxCheckBox( this, wxID_ANY, _( "Enable safe options (see documentation)" ) );
    if( values.find( L"SAFE" ) != values.end() )
        m_safeOptions->SetValue( values.at( L"SAFE" ) == L"1" ? true : false );
    m_safeOptions->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupMisc::DataChanged, this );
    sizer2->Add( m_safeOptions, 0, wxEXPAND, 0 );
    m_bindBigInt = new wxCheckBox( this, wxID_ANY, _( "Bind BIGINT parameters as strings" ) );
    if( values.find( L"DFLT_BIGINT_BIND_STR" ) != values.end() )
        m_bindBigInt->SetValue( values.at( L"DFLT_BIGINT_BIND_STR" ) == L"1" ? true : false );
    m_bindBigInt->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupMisc::DataChanged, this );
    sizer2->Add( m_bindBigInt, 0, wxEXPAND, 0 );
    m_noSetLocale = new wxCheckBox( this, wxID_ANY, _( "Don't use setlocale()" ) );
    if( values.find( L"NO_LOCALE" ) != values.end() )
        m_noSetLocale->SetValue( values.at( L"NO_LOCALE" ) == L"1" ? true : false );
    m_noSetLocale->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupMisc::DataChanged, this );
    sizer2->Add( m_noSetLocale, 0, wxEXPAND, 0 );
    m_disableDateOverflow = new wxCheckBox( this, wxID_ANY, _( "Disable Date Overflow error" ) );
    if( values.find( L"NO_DATE_OVERFLOW" ) != values.end() )
        m_disableDateOverflow->SetValue( values.at( L"NO_DATE_OVERFLOW" ) == L"1" ? true : false );
    m_disableDateOverflow->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupMisc::DataChanged, this );
    sizer2->Add( m_disableDateOverflow, 0, wxEXPAND, 0 );
    m_ignoreSpace = new wxCheckBox( this, wxID_ANY, _( "Ignore space after function names" ) );
    if( values.find( L"IGNORE_SPACE" ) != values.end() )
        m_ignoreSpace->SetValue( values.at( L"IGNORE_SPACE" ) == L"1" ? true : false );
    m_ignoreSpace->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupMisc::DataChanged, this );
    sizer2->Add( m_ignoreSpace, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_useConfigFile = new wxCheckBox( this, wxID_ANY, _( "Read options from my.cnf" ) );
    if( values.find( L"USE_MYCNF" ) != values.end() )
        m_useConfigFile->SetValue( values.at( L"USE_MYCNF" ) == L"1" ? true : false );
    m_useConfigFile->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupMisc::DataChanged, this );
    sizer2->Add( m_useConfigFile, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_noTrans = new wxCheckBox( this, wxID_ANY, _( "Disable transaction support" ) );
    if( values.find( L"NO_TRANSACTIONS" ) != values.end() )
        m_noTrans->SetValue( values.at( L"NO_TRANSACTIONS" ) == L"1" ? true : false );
    m_noTrans->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupMisc::DataChanged, this );
    sizer2->Add( m_noTrans, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_bindMinDate = new wxCheckBox( this, wxID_ANY, _( "Bind minimal date as zero date" ) );
    if( values.find( L"MIN_DATE_TO_ZERO" ) != values.end() )
        m_bindMinDate->SetValue( values.at( L"MIN_DATE_TO_ZERO" ) == L"1" ? true : false );
    m_bindMinDate->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupMisc::DataChanged, this );
    sizer2->Add( m_bindMinDate, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_prepareOnClient = new wxCheckBox( this, wxID_ANY, _( "Prepare statements on the client" ) );
    if( values.find( L"NO_SSPS" ) != values.end() )
        m_prepareOnClient->SetValue( values.at( L"NO_SSPS" ) == L"1" ? true : false );
    m_prepareOnClient->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupMisc::DataChanged, this );
    sizer2->Add( m_prepareOnClient, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_enableLoadData = new wxCheckBox( this, wxID_ANY, _( "Enable LOAD DATA LOCAL INFILE statements" ) );
    if( values.find( L"ENABLE_LOCAL_INFILE" ) != values.end() )
        m_enableLoadData->SetValue( values.at( L"ENABLE_LOCAL_INFILE" ) == L"1" ? true : false );
    m_enableLoadData->Bind( wxEVT_CHECKBOX, &MySQLODBCSetupMisc::DataChanged, this );
    sizer2->Add( m_enableLoadData, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer3 = new wxBoxSizer( wxHORIZONTAL );
    sizer1->Add( sizer3, 0, wxEXPAND, 0 );
    m_label1 = new wxStaticText( this, wxID_ANY, _( "LOAD DATA LOCAL Directory" ) );
    sizer3->Add( m_label1, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    m_dataLocalDir = new wxDirPickerCtrl( this, wxID_ANY, wxStandardPaths::Get().GetDocumentsDir() );
    if( values.find( L"LOAD_DATA_LOCAL_DIR" ) != values.end() )
        m_dataLocalDir->SetPath( values.at( L"LOAD_DATA_LOCAL_DIR" ) );
    m_dataLocalDir->Bind( wxEVT_DIRPICKER_CHANGED, &MySQLODBCSetupMisc::DataChanged, this );
    sizer3->Add( m_dataLocalDir, 1, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer );
    sizer->Fit( this );
}

void MySQLODBCSetupMisc::DataChanged(wxCommandEvent &event)
{
    m_changed = true;
}


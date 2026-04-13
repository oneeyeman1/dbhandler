//
//  createdatabase.cpp
//  libdialogs
//
//  Created by Igor Korot on 6/28/17.
//  Copyright (c) 2017 Igor Korot. All rights reserved.
//

#include "wx/wx.h"
#include <wx/filepicker.h>
#include <wx/collpane.h>
#include "database.h"
#include "createdatabase.h"

CreateDatabase::CreateDatabase(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, CreateDBOptions *options) : wxDialog( parent, wxID_ANY, _( "Create Database" ) )
{
    m_opts = options;
    auto main = new wxBoxSizer( wxHORIZONTAL );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    auto second = new wxBoxSizer( wxVERTICAL );
    second->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer1 = new wxBoxSizer( wxHORIZONTAL );
    m_label1 = new wxStaticText( this, wxID_ANY, _( "Database Name" ) );
    sizer1->Add( m_label1, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    if( type == L"SQLite" )
    {
        m_SQLiteName = new wxFilePickerCtrl( this, wxID_ANY );
        sizer1->Add( m_SQLiteName, 0, wxEXPAND, 0 );
    }
    else
    {
        m_name = new wxTextCtrl( this, wxID_ANY );
        sizer1->Add( m_name, 0, wxEXPAND, 0 );
    }
    second->Add( sizer1, 0, wxEXPAND, 0 );
    second->Add( 5, 5, 0, wxEXPAND, 0 );
    if( type != L"SQLite" )
    {
        auto sizer2 = new wxBoxSizer( wxHORIZONTAL );
        m_options = new wxCollapsiblePane( this, wxID_ANY, _( "Options" ) );
        m_options->Bind( wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent &) { Layout(); } );
        sizer2->Add( m_options, 0, wxEXPAND, 0 );
        second->Add( sizer2, 0, wxEXPAND, 0 );
        auto win = m_options->GetPane();
        auto sizer3 = new wxBoxSizer( wxHORIZONTAL );
        sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
        auto sizer4 = new wxBoxSizer( wxVERTICAL );
        sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
        if( type == L"MySQL" || subtype == L"MySQL" )
        {
            MySQLCreateDBOptions *opts = dynamic_cast<MySQLCreateDBOptions *>( options );
            auto paneSizer1 = new wxFlexGridSizer( 3, 2, 5, 5 );
            m_label2 = new wxStaticText( win, wxID_ANY, _( "Character Set:" ) );
            paneSizer1->Add( m_label2, 0, wxEXPAND, 0 );
            m_characterSet = new wxComboBox( win, wxID_ANY );
            for( auto charSet : opts->m_charSets )
            {
                int row = m_characterSet->Append( std::get<2>( charSet ) );
                m_characterSet->SetClientObject( row, (wxClientData *) &charSet );
            }
            m_characterSet->SetValue( "Default" );
            m_characterSet->Bind( wxEVT_COMBOBOX, &CreateDatabase::OnCharacterSetChanged, this );
            paneSizer1->Add( m_characterSet, 0, wxEXPAND, 0 );
            m_label3 = new wxStaticText( win, wxID_ANY, _( "Collations:" ) );
            paneSizer1->Add( m_label3, 0, wxEXPAND, 0 );
            m_collations = new wxComboBox( win, wxID_ANY );
            m_collations->Append( "Default" );
            m_collations->SetValue( "Default" );
            paneSizer1->Add( m_collations, 0, wxEXPAND, 0 );
            m_encrypted = new wxCheckBox( win, wxID_ANY, _( "Encrypted:" ) );
            paneSizer1->Add( m_encrypted, 0, wxEXPAND, 0 );
            paneSizer1->Add( 5, 5, 0, wxEXPAND, 0 );
            sizer4->Add( paneSizer1, 0, wxEXPAND, 0 );
        }
        sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
        sizer3->Add( sizer4, 0, wxEXPAND, 0 );
        sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
        win->SetSizer( sizer3 );
        sizer3->SetSizeHints( win );
    }
    second->Add( 5, 5, 0, wxEXPAND, 0 );
    second->Add( CreateStdDialogButtonSizer( wxOK | wxCANCEL | wxHELP ) );
    second->Add( 5, 5, 0, wxEXPAND, 0 );
    main->Add( second, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( main );
    Layout();
}

void CreateDatabase::OnCharacterSetChanged(wxCommandEvent &event)
{
    MySQLCreateDBOptions *opts = dynamic_cast<MySQLCreateDBOptions *>( m_opts );
    auto charSet = reinterpret_cast<std::tuple<std::wstring, std::wstring, std::wstring> *>( m_characterSet->GetClientObject( m_characterSet->GetSelection() ) );
    m_collations->Clear();
    std::wstring charset = std::get<1>( charSet );
    for( auto collation : opts->m_collations[charset] )
    {
        m_collations->Append( std::get<0>( collation ) );
    }
}


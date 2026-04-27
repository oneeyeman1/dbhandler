//
//  createdatabase.cpp
//  libdialogs
//
//  Created by Igor Korot on 6/28/17.
//  Copyright (c) 2017 Igor Korot. All rights reserved.
//

#include "wx/wx.h"
#include "wx/filepicker.h"
#include "wx/collpane.h"
#include "wx/spinctrl.h"
#include "database.h"
#include "createdatabase.h"

CreateDatabase::CreateDatabase(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, std::shared_ptr<CreateDBOptions> options) : wxDialog( parent, wxID_ANY, _( "Create Database" ) )
{
    m_opts = options;
    m_type = type;
    m_subtype = subtype;
    wxFlexGridSizer *paneSizer1 = nullptr;
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
    m_exist = new wxCheckBox( this, wxID_ANY, "IF NOT EXIST" );
    second->Add( m_exist, 0, wxEXPAND, 0 );
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
            auto opts = std::dynamic_pointer_cast<MySQLCreateDBOptions>( options );
            paneSizer1 = new wxFlexGridSizer( 3, 2, 5, 5 );
            m_label2 = new wxStaticText( win, wxID_ANY, _( "Character Set:" ) );
            paneSizer1->Add( m_label2, 0, wxEXPAND, 0 );
            m_characterSet = new wxComboBox( win, wxID_ANY );
            for( auto &charSet : opts->m_charSets )
            {
                int row = m_characterSet->Append( std::get<2>( *charSet ) );
                m_characterSet->SetClientData( row, charSet.get() );
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
        }
        if( type == L"PostgreSQL" || subtype == L"PostgreSQL" )
        {
            auto opts = std::dynamic_pointer_cast<PostgresCreateDBOptions>( options );
            paneSizer1 = new wxFlexGridSizer( 7, 2, 5, 5 );
            m_label1 = new wxStaticText( win, wxID_ANY, _( "OWNER" ) );
            paneSizer1->Add( m_label1, 0, wxALIGN_CENTER_VERTICAL, 0 );
            m_owner = new wxComboBox( win, wxID_ANY );
            paneSizer1->Add( m_owner, 0, wxEXPAND, 0 );
            for( auto user : opts->m_roles )
                m_owner->Append( user );
            m_owner->SetValue( "Default" );
            m_label2 = new wxStaticText( win, wxID_ANY, "TEMPLATE" );
            paneSizer1->Add( m_label2, 0, wxALIGN_CENTER_VERTICAL, 0 );
            m_template = new wxComboBox( win, wxID_ANY );
            for( auto tmplate : opts->m_templates )
                m_template->Append( tmplate );
            paneSizer1->Add( m_template, 0, wxEXPAND, 0 );
            m_template->SetValue( "Default" );
            m_label3 = new wxStaticText( win, wxID_ANY, "TABLESPACE" );
            m_tablespace = new wxComboBox( win, wxID_ANY, "" );
            paneSizer1->Add( m_label3, 0, wxALIGN_CENTER_VERTICAL, 0 );
            paneSizer1->Add( m_tablespace, 0, wxEXPAND, 0 );
            m_label4 = new wxStaticText( win, wxID_ANY, "CONNECTION LIMIT" );
            m_connlimit = new wxSpinCtrl( win, wxID_ANY, "-1", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -1, 32676 );
            paneSizer1->Add( m_label4, 0, wxALIGN_CENTER_VERTICAL, 0 );
            paneSizer1->Add( m_connlimit, 0, wxEXPAND, 0 );
        }
        if( type == L"Microsoft SQL Server" || subtype == L"Microsoft SQL Server" )
        {
            auto opts = std::dynamic_pointer_cast<SQLServerCreateDBOptions>( options );
            paneSizer1 = new wxFlexGridSizer( 5, 5, 5, 5 );
            const wxString data[] =
            {
                "NONE",
                "PARTIAL"
            };
            m_label1 = new wxStaticText( win, wxID_ANY, "CONTAINMENT" );
            m_containment = new wxComboBox( win, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 2, 0 );
            paneSizer1->Add( m_label1, 0, wxEXPAND, 0 );
            paneSizer1->Add( m_containment, 0, wxEXPAND, 0 );
        }
        sizer4->Add( paneSizer1, 0, wxEXPAND, 0 );
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
    FindWindowById( wxID_OK )->Bind( wxEVT_UPDATE_UI, &CreateDatabase::OnOKUpdateUI, this );
    dynamic_cast<wxButton *>( FindWindowById( wxID_OK ) )->Bind( wxEVT_BUTTON, &CreateDatabase::OnOK, this );
}

void CreateDatabase::OnCharacterSetChanged(wxCommandEvent &WXUNUSED(event))
{
    auto opts = std::dynamic_pointer_cast<MySQLCreateDBOptions>( m_opts );
    CharSet *charSet = static_cast<CharSet *>( m_characterSet->GetClientData( m_characterSet->GetSelection() ) );
    wxString defValue = "";
    std::wstring charset = std::get<0>( *charSet );
    for( auto collation : opts->m_collations[charset] )
    {
        auto value = std::get<0>( collation );
        m_collations->Append( value );
        if( std::get<1>( collation ) )
            defValue = value;
    }
    m_collations->SetValue( defValue );
}

void CreateDatabase::OnOKUpdateUI(wxUpdateUIEvent &event)
{
    if( m_name->GetValue() == wxEmptyString )
        event.Enable( false );
    else
        event.Enable( true );
}

void CreateDatabase::OnOK(wxCommandEvent &WXUNUSED(event))
{
    m_opts->m_name = m_name->GetValue();
    m_opts->m_exist = m_exist->GetValue();
    if( m_type == L"MySQL" || m_subtype == L"MySQL" )
    {
        auto opts = std::dynamic_pointer_cast<MySQLCreateDBOptions>( m_opts );
        CharSet *charSet = static_cast<CharSet *>( m_characterSet->GetClientData( m_characterSet->GetSelection() ) );
        wxString defValue = "";
        std::wstring charset = std::get<0>( *charSet );
        opts->m_charSet = charset;
        opts->m_collation = m_collations->GetValue();
        opts->m_encrypted = m_encrypted->GetValue();
    }
    if( m_type == L"Microsoft SQL Server" || m_subtype == L"Microsoft SQL Server" )
    {
        auto opts = std::dynamic_pointer_cast<SQLServerCreateDBOptions>( m_opts );
        opts->m_containment = m_containment->GetValue();
    }
    EndModal( wxID_OK );
}


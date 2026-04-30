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
#include "sqlserveraddfilespec.h"
#include "createdatabase.h"

CreateDatabase::CreateDatabase(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, int serverVersion, std::shared_ptr<CreateDBOptions> options) : wxDialog( parent, wxID_ANY, _( "Create Database" ) )
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
    if( type == L"Microsoft SQL Server" || subtype == L"Microsoft SQL Server" )
    {
        const wxString data[] =
        {
            "NONE",
            "PARTIAL"
        };
        m_label1 = new wxStaticText( this, wxID_ANY, "CONTAINMENT" );
        m_containment = new wxComboBox( this, wxID_ANY, "NONE", wxDefaultPosition, wxDefaultSize, 2, data );
        auto sizer2 = new wxBoxSizer( wxHORIZONTAL );
        sizer2->Add( m_label1, 0, wxEXPAND, 0 );
        sizer2->Add( m_containment, 0, wxEXPAND, 0 );
        second->Add( sizer2, 0, wxEXPAND, 0 );
        second->Add( 5, 5, 0, wxEXPAND, 0 );
    }
    if( type != L"SQLite" )
    {
        auto sizer2 = new wxBoxSizer( wxHORIZONTAL );
        wxString title;
        if( type != L"Microsoft SQL Server" || subtype != L"Microsoft SQL Server" )
            title = _( "Options" );
        else
            title = _( "ON" );
        m_options = new wxCollapsiblePane( this, wxID_ANY, title );
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
            auto paneSizer = new wxBoxSizer( wxVERTICAL );
            auto scrolled = new wxScrolledWindow( win, wxID_ANY );
            paneSizer->Add( scrolled, 1, wxEXPAND, 0 );
            paneSizer1 = new wxFlexGridSizer( 2, 5, 5, 5 );
            auto label_1 = new wxStaticText( scrolled, wxID_ANY, "Name" );
            paneSizer1->Add( label_1, 0, wxEXPAND, 0 );
            auto label_2 = new wxStaticText( scrolled, wxID_ANY, "FileName" );
            paneSizer1->Add( label_2, 0, wxEXPAND, 0 );
            auto label_3 = new wxStaticText( scrolled, wxID_ANY, "Size" );
            paneSizer1->Add( label_3, 0, wxEXPAND, 0 );
            auto label_4 = new wxStaticText( scrolled, wxID_ANY, "MaxSize" );
            paneSizer1->Add( label_4, 0, wxEXPAND, 0 );
            auto label_5 = new wxStaticText( scrolled, wxID_ANY, "FileGrowth" );
            paneSizer1->Add( label_5, 0, wxEXPAND, 0 );
            auto name = new wxTextCtrl( scrolled, wxID_ANY, "Default" );
            name->Enable( false );
            paneSizer1->Add( name, 0, wxEXPAND, 0 );
            auto fileName = new wxTextCtrl( scrolled, wxID_ANY, "Default" );
            fileName->Enable( false );
            paneSizer1->Add( fileName, 0, wxEXPAND, 0 );
            auto sizer5 = new wxBoxSizer( wxHORIZONTAL );
            paneSizer1->Add( sizer5, 0, wxEXPAND, 0 );
            auto size1 = new wxTextCtrl( scrolled, wxID_ANY, "" );
            size1->Enable( false );
            sizer5->Add( size1, 0, wxEXPAND, 0 );
            sizer5->Add( 5, 5, 0, wxEXPAND, 0 );
            auto label1 = new wxStaticText( scrolled, wxID_ANY, "GB" );
            sizer5->Add( label1, 0, wxEXPAND, 0 );
            auto sizer6 = new wxBoxSizer( wxHORIZONTAL );
            paneSizer1->Add( sizer6, 0, wxBOTTOM, 0 );
            auto size2 = new wxTextCtrl( scrolled, wxID_ANY, "" );
            size2->Enable( false );
            sizer6->Add( size2, 0, wxEXPAND, 0 );
            sizer6->Add( 5, 5, 0, wxEXPAND, 0 );
            auto label2 = new wxStaticText( scrolled, wxID_ANY, "GN" );
            sizer6->Add( label2, 0, wxEXPAND, 0 );
            auto sizer7 = new wxBoxSizer( wxHORIZONTAL );
            paneSizer1->Add( sizer7, 0, wxEXPAND, 0 );
            auto size3 = new wxTextCtrl( scrolled, wxID_ANY, "Default" );
            size3->Enable( false );
            sizer7->Add( size3, 0, wxEXPAND, 0 );
            sizer7->Add( 5, 5, 0, wxEXPAND, 0 );
            auto label3 = new wxStaticText( scrolled, wxID_ANY, "%" );
            sizer7->Add( label3, 0, wxEXPAND, 0 );
            paneSizer->Add( 5, 5, 0, wxEXPAND, 0 );
            auto sizer4 = new wxBoxSizer( wxHORIZONTAL );
            paneSizer->Add( sizer4, 0, wxEXPAND, 0 );
            auto sizer3 = new wxBoxSizer( wxHORIZONTAL );
            sizer4->Add( sizer3, 0, wxEXPAND, 0 );
            m_add = new wxButton( win, wxID_ANY, "Add" );
            m_add->Bind( wxEVT_BUTTON, &CreateDatabase::OnSQLServerFileSecAdd, this );
            sizer3->Add( m_add, 0, wxEXPAND, 0 );
            sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
            m_delete = new wxButton( win, wxID_ANY, "Delete" );
            m_delete->Bind( wxEVT_BUTTON, &CreateDatabase::OnSQLServerFileSecDelete, this );
            sizer3->Add( m_delete, 0, wxEXPAND, 0 );
            paneSizer->Add( 5, 5, 0, wxEXPAND, 0 );
            scrolled->SetSizer( paneSizer1 );
            win->SetSizer( paneSizer );
        }
        else
        {
            sizer4->Add( paneSizer1, 0, wxEXPAND, 0 );
            sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
            sizer3->Add( sizer4, 0, wxEXPAND, 0 );
            sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
            win->SetSizer( sizer3 );
            sizer3->SetSizeHints( win );
        }
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

void CreateDatabase::OnSQLServerFileSecAdd(wxCommandEvent &event)
{
    SQLServerAddFileSpec dlg( GetParent(), wxID_ANY, "Add FileSpec" );
    if( dlg.ShowModal() == wxID_OK )
    {
    }
}

void CreateDatabase::OnSQLServerFileSecDelete(wxCommandEvent &event)
{

}

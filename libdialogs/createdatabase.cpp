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

CreateDatabase::CreateDatabase(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, int serverVersionMajor, int serverVersionMinor, std::shared_ptr<CreateDBOptions> options) : wxDialog( parent, wxID_ANY, _( "Create Database" ) )
{
    m_opts = options;
    m_type = type;
    m_subtype = subtype;
    m_versionMajor = serverVersionMajor;
    m_versionMinor = serverVersionMinor;
    wxWindow *withPane = nullptr;
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
    if( ( type == L"Microsoft SQL Server" || subtype == L"Microsoft SQL Server" ) && m_versionMajor >= 11 )
    {
        const wxString data[] =
        {
            "NONE",
            "PARTIAL"
        };
        m_label1 = new wxStaticText( this, wxID_ANY, "CONTAINMENT" );
        m_containment = new wxComboBox( this, wxID_ANY, "NONE", wxDefaultPosition, wxDefaultSize, 2, data );
        m_containment->SetToolTip( "Specifies the containment status of the database" );
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
        if( type == L"Microsoft SQL Server" || subtype == L"Microsoft SQL Server" )
        {
            auto opts = std::dynamic_pointer_cast<SQLServerCreateDBOptions>( options );
            m_label12 = new wxStaticText( this, wxID_ANY, "COLLATE" );
            second->Add( 5, 5, 0, wxEXPAND, 0 );
            auto sizer20 = new wxBoxSizer( wxHORIZONTAL );
            sizer20->Add( m_label12, 0, wxEXPAND, 0 );
            sizer20->Add( 5, 5, 0, wxEXPAND, 0 );
            m_collations = new wxComboBox( this, wxID_ANY, "Default" );
            sizer20->Add( m_collations, 0, wxEXPAND, 0 );
            second->Add( sizer20, 0, wxEXPAND, 0 );
            m_with = new wxCollapsiblePane( this, wxID_ANY, "WITH" );
            m_with->Bind( wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent &) { Layout(); } );
            second->Add( m_with, 0, wxEXPAND, 0 );
            withPane = m_with->GetPane();
            auto sizer_1 = new wxBoxSizer( wxHORIZONTAL );
            sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
            auto sizer_2 = new wxBoxSizer( wxVERTICAL );
            sizer_1->Add( sizer_2, 0, wxEXPAND, 0 );
            sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
            auto sizer_3 = new wxBoxSizer( wxVERTICAL );
            sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
            auto sizer_5 = new wxStaticBoxSizer( new wxStaticBox( withPane, wxID_ANY, "FILESTREAM" ), wxHORIZONTAL );
            sizer_3->Add( sizer_5, 0, wxEXPAND, 0 );
            auto grid_sizer_2 = new wxFlexGridSizer( 2, 2, 5, 5 );
            sizer_5->Add( grid_sizer_2, 1, wxEXPAND, 0 );
            m_label7 = new wxStaticText( sizer_5->GetStaticBox(), wxID_ANY, "NON_TRANSACTED_ACCESS" );
            grid_sizer_2->Add( m_label7, 0, wxALIGN_CENTER_VERTICAL, 0 );
            const wxString m_access_choices[] = {
                "OFF",
                "READ ONLY",
                "FULL",
            };
            m_access = new wxChoice( sizer_5->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 3, m_access_choices );
            m_access->SetSelection( 2 );
            grid_sizer_2->Add( m_access, 0, wxEXPAND, 0 );
            m_label8 = new wxStaticText( sizer_5->GetStaticBox(), wxID_ANY, "DIRECTORY_NAME" );
            grid_sizer_2->Add( m_label8, 0, wxALIGN_CENTER_VERTICAL, 0 );
            m_dirName1 = new wxDirPickerCtrl( sizer_5->GetStaticBox(), wxID_ANY, wxEmptyString );
            grid_sizer_2->Add( m_dirName1, 0, wxEXPAND, 0 );
            sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
            auto grid_sizer_1 = new wxFlexGridSizer( 9, 2, 5, 5 );
            sizer_3->Add( grid_sizer_1, 0, wxEXPAND, 0 );
            m_label9 = new wxStaticText( withPane, wxID_ANY, "DEFAULT_FULLTEXT_LANGUAGE" );
            grid_sizer_1->Add( m_label9, 0, wxALIGN_CENTER_VERTICAL, 0 );
            m_fulltext = new wxChoice( withPane, wxID_ANY );
            grid_sizer_1->Add( m_fulltext, 0, wxEXPAND, 0 );
            m_label10 = new wxStaticText( withPane, wxID_ANY, "DEFAULT_LANGUAGE" );
            grid_sizer_1->Add( m_label10, 0, wxALIGN_CENTER_VERTICAL, 0 );
            m_lang = new wxChoice( withPane, wxID_ANY );
            grid_sizer_1->Add( m_lang, 0, wxEXPAND, 0 );
            m_triggers = new wxCheckBox( withPane, wxID_ANY, "NESTED_TRIGGERS" );
            m_triggers->SetValue( 1 );
            grid_sizer_1->Add( m_triggers, 0, wxEXPAND, 0 );
            grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
            m_noise = new wxCheckBox( withPane, wxID_ANY, "TRANSFORM_NOISE_WORDS" );
            grid_sizer_1->Add( m_noise, 0, wxEXPAND, 0 );
            grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
            m_label11 = new wxStaticText( withPane, wxID_ANY, "TWO_DIGIT_YEAR_CUTOFF" );
            grid_sizer_1->Add( m_label11, 0, wxALIGN_CENTER_VERTICAL, 0 );
            m_yearCutoff = new wxSpinCtrl( withPane, wxID_ANY, "2049", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS|wxSP_WRAP, 1753, 9999 );
            grid_sizer_1->Add( m_yearCutoff, 0, wxEXPAND, 0 );
            m_dbChain = new wxCheckBox( withPane, wxID_ANY, "DB_CHAINING" );
            grid_sizer_1->Add( m_dbChain, 0, wxALIGN_CENTER_VERTICAL, 0 );
            grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
            m_trust = new wxCheckBox( withPane, wxID_ANY, "TRUSTWORTHY" );
            grid_sizer_1->Add( m_trust, 0, wxALIGN_CENTER_VERTICAL, 0 );
            grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
            m_persistantLog = new wxCheckBox( withPane, wxID_ANY, "PERSISTENT_LOG_BUFFER" );
            grid_sizer_1->Add( m_persistantLog, 0, wxALIGN_CENTER_VERTICAL, 0 );
            m_dirName2 = new wxDirPickerCtrl( withPane, wxID_ANY, wxEmptyString );
            m_dirName2->Enable( false );
            grid_sizer_1->Add( m_dirName2, 0, wxEXPAND, 0 );
            m_ledger = new wxCheckBox( withPane, wxID_ANY, "LEDGER" );
            grid_sizer_1->Add( m_ledger, 0, wxEXPAND, 0 );
            grid_sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
            sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
            sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
            m_persistantLog->Bind( wxEVT_CHECKBOX, &CreateDatabase::OnPersistentLog, this );
            withPane->SetSizer( sizer_1 );
        }
        auto win = m_options->GetPane();
        auto sizer3 = new wxBoxSizer( wxHORIZONTAL );
        sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
        wxBoxSizer *sizer4 = nullptr;
        if( type != L"Microsoft SQL Server" && subtype != L"Microsoft SQL Server" )
        {
            sizer4 = new wxBoxSizer( wxVERTICAL );
            sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
        }
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
            paneSizer1 = new wxFlexGridSizer( 9, 2, 5, 5 );
            m_label1 = new wxStaticText( win, wxID_ANY, _( "OWNER" ) );
            paneSizer1->Add( m_label1, 0, wxALIGN_CENTER_VERTICAL, 0 );
            m_owner = new wxComboBox( win, wxID_ANY );
            m_owner->SetToolTip( "The role name of the user who will own the new database, or DEFAULT to use the default (namely, the user executing the command)" );
            paneSizer1->Add( m_owner, 0, wxEXPAND, 0 );
            for( auto user : opts->m_roles )
                m_owner->Append( user );
            m_owner->SetValue( "Default" );
            m_label2 = new wxStaticText( win, wxID_ANY, "TEMPLATE" );
            paneSizer1->Add( m_label2, 0, wxALIGN_CENTER_VERTICAL, 0 );
            m_template = new wxComboBox( win, wxID_ANY );
            m_template->SetToolTip( "The name of the template from which to create the new database, or DEFAULT to use the default template (template1)" );
            for( auto tmplate : opts->m_templates )
                m_template->Append( tmplate );
            paneSizer1->Add( m_template, 0, wxEXPAND, 0 );
            m_template->SetValue( "Default" );
            m_label3 = new wxStaticText( win, wxID_ANY, "ENCODING" );
            m_characterSet = new wxComboBox( win, wxID_ANY );
            m_characterSet->SetToolTip( "Character set encoding to use in the new database. Specify a string constant (e.g., 'SQL_ASCII'), or an integer encoding number, or DEFAULT to use the default encoding (namely, the encoding of the template database)" );
            for( auto encoding : opts->m_encodings )
                m_characterSet->Append( encoding );
            m_characterSet->SetValue( "Default" );
            paneSizer1->Add( m_label3, 0, wxALIGN_CENTER_VERTICAL, 0 );
            paneSizer1->Add( m_characterSet, 0, wxEXPAND, 0 );
            m_label5 = new wxStaticText( win, wxID_ANY, "LC_COLLATE" );
            paneSizer1->Add( m_label5, 0, wxALIGN_CENTER_VERTICAL, 0 );
            m_collations = new wxComboBox( win, wxID_ANY, "" );
            m_collations->SetToolTip( "Collation order (LC_COLLATE) to use in the new database. This affects the sort order applied to strings" );
            for( auto collation : opts->m_collations )
                m_collations->Append( std::get<0>( collation ) );
            m_collations->SetValue( "Default" );
            paneSizer1->Add( m_collations, 0, wxEXPAND, 0 );
            m_label6 = new wxStaticText( win, wxID_ANY, "LC_CTYPE" );
            paneSizer1->Add( m_label6, 0, wxALIGN_CENTER_VERTICAL, 0 );
            m_ctype = new wxComboBox( win, wxID_ANY, "" );
            m_ctype->SetToolTip( "Character classification (LC_CTYPE) to use in the new database. This affects the categorization of characters" );
            for( auto ctype : opts->m_ctypes )
                m_ctype->Append( std::get<0>( ctype ) );
            m_ctype->SetValue( "Default" );
            paneSizer1->Add( m_ctype, 0, wxEXPAND, 0 );
            m_label4 = new wxStaticText( win, wxID_ANY, "TABLESPACE" );
            m_tablespace = new wxComboBox( win, wxID_ANY, "" );
            m_tablespace->SetToolTip( "The name of the tablespace that will be associated with the new database, or DEFAULT to use the template database's tablespace" );
            for( auto space: opts->m_tablespaces )
                m_tablespace->Append( space );
            m_tablespace->SetValue( "Default" );
            paneSizer1->Add( m_label4, 0, wxALIGN_CENTER_VERTICAL, 0 );
            paneSizer1->Add( m_tablespace, 0, wxEXPAND, 0 );
            if( ( m_versionMajor > 9 && m_versionMinor >= 5 ) || ( m_versionMajor >= 10 ) )
            {
                m_allowConn = new wxCheckBox( win, wxID_ANY, "ALLOW_CONNECTION" );
                m_allowConn->SetToolTip( "If false then no one can connect to this database. The default is true, allowing connections" );
                m_allowConn->SetValue( true );
                paneSizer1->Add( m_allowConn, 0, wxEXPAND, 0 );
                paneSizer1->Add( 5, 5, 0, wxEXPAND, 0 );
            }
            m_label4 = new wxStaticText( win, wxID_ANY, "CONNECTION LIMIT" );
            m_connlimit = new wxSpinCtrl( win, wxID_ANY, "-1", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -1, 32676 );
            m_connlimit->SetToolTip( "How many concurrent connections can be made to this database. -1 (the default) means no limit" );
            paneSizer1->Add( m_label4, 0, wxALIGN_CENTER_VERTICAL, 0 );
            paneSizer1->Add( m_connlimit, 0, wxEXPAND, 0 );
            if( ( m_versionMajor > 9 && m_versionMinor >= 5 ) || ( m_versionMajor >= 10 ) )
            {
                m_istemplate = new wxCheckBox( win, wxID_ANY, "IS_TEMPLATE" );
                m_istemplate->SetToolTip( "If true, then this database can be cloned by any user with CREATEDB privileges; if false (the default), then only superusers or the owner of the database can clone it" );
                paneSizer1->Add( m_istemplate, 0, wxEXPAND, 0 );
                paneSizer1->Add( 5, 5, 0, wxEXPAND, 0 );
            }
        }
        if( type == L"Microsoft SQL Server" || subtype == L"Microsoft SQL Server" )
        {
            auto opts = std::dynamic_pointer_cast<SQLServerCreateDBOptions>( options );
            auto paneSizer = new wxBoxSizer( wxVERTICAL );
            auto scrolled = new wxScrolledWindow( win, wxID_ANY );
            paneSizer->Add( scrolled, 1, wxEXPAND, 0 );
            paneSizer1 = new wxFlexGridSizer( 2, 6, 5, 5 );
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
            auto label_6 = new wxStaticText( scrolled, wxID_ANY, "Primary" );
            paneSizer1->Add( label_6, 0, wxEXPAND, 0 );
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
            auto label2 = new wxStaticText( scrolled, wxID_ANY, "GB" );
            sizer6->Add( label2, 0, wxEXPAND, 0 );
            auto sizer7 = new wxBoxSizer( wxHORIZONTAL );
            paneSizer1->Add( sizer7, 0, wxEXPAND, 0 );
            auto size3 = new wxTextCtrl( scrolled, wxID_ANY, "Default" );
            size3->Enable( false );
            sizer7->Add( size3, 0, wxEXPAND, 0 );
            sizer7->Add( 5, 5, 0, wxEXPAND, 0 );
            auto label3 = new wxStaticText( scrolled, wxID_ANY, "%" );
            sizer7->Add( label3, 0, wxEXPAND, 0 );
            auto primary = new wxCheckBox( scrolled, wxID_ANY, "" );
            primary->Enable( false );
            paneSizer1->Add( primary, 0, wxALIGN_CENTER_HORIZONTAL, 0 );
            paneSizer->Add( 5, 5, 0, wxEXPAND, 0 );
            auto sizer9 = new wxBoxSizer( wxHORIZONTAL );
            paneSizer->Add( sizer9, 0, wxEXPAND, 0 );
            auto sizer8 = new wxBoxSizer( wxHORIZONTAL );
            sizer9->AddStretchSpacer();
            sizer9->Add( sizer8, 0, wxEXPAND, 0 );
            sizer9->AddStretchSpacer();
            m_add = new wxButton( win, wxID_ANY, "Add" );
            m_add->Bind( wxEVT_BUTTON, &CreateDatabase::OnSQLServerFileSecAdd, this );
            sizer8->Add( m_add, 0, wxEXPAND, 0 );
            sizer8->Add( 5, 5, 0, wxEXPAND, 0 );
            m_delete = new wxButton( win, wxID_ANY, "Delete" );
            m_delete->Bind( wxEVT_BUTTON, &CreateDatabase::OnSQLServerFileSecDelete, this );
            sizer8->Add( m_delete, 0, wxEXPAND, 0 );
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
    if( m_type == L"PostgreSQL" || m_subtype == L"PostgreSQL" )
    {
        auto opts = std::dynamic_pointer_cast<PostgresCreateDBOptions>( m_opts );
        opts->m_role = m_owner->GetValue();
        opts->m_template = m_template->GetValue();
        opts->m_encoding = m_characterSet->GetValue();
        opts->m_collation = m_collations->GetValue();
        opts->m_ctype = m_ctype->GetValue();
        opts->m_tablespace = m_tablespace->GetValue();
        if( ( m_versionMajor > 9 && m_versionMinor >= 5 ) || ( m_versionMajor >= 10 ) )
            opts->m_allowConn = m_allowConn->GetValue();
        opts->m_connlimit = m_connlimit->GetValue();
        if( ( m_versionMajor > 9 && m_versionMinor >= 5 ) || ( m_versionMajor >= 10 ) )
            opts->m_isTemplate = m_istemplate->GetValue();
    }
    EndModal( wxID_OK );
}

void CreateDatabase::OnSQLServerFileSecAdd(wxCommandEvent &WXUNUSED(event))
{
    SQLServerAddFileSpec dlg( GetParent(), wxID_ANY, "Add FileSpec", m_versionMajor );
    if( dlg.ShowModal() == wxID_OK )
    {
        auto spec = dlg.GetFileSpec();
        if( paneSizer1->GetRows() == 2 )
        {
            dynamic_cast<wxTextCtrl *>( paneSizer1->GetItem( 7 ) )->SetValue( spec.m_name );
        }
    }
}

void CreateDatabase::OnSQLServerFileSecDelete(wxCommandEvent &WXUNUSED(event))
{

}

void CreateDatabase::OnPersistentLog(wxCommandEvent &WXUNUSED(event))
{
    if( m_persistantLog->IsChecked() )
        m_dirName2->Enable( true );
    else
        m_dirName2->Enable( false );
}

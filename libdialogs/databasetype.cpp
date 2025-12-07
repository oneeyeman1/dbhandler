// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/mdi.h"
#endif

#include <map>
/*#ifdef _IODBCUNIX_H
#include "iODBC/sqlext.h"
#else
#include <sqlext.h>
#endif*/
#include "wx/valnum.h"
#include "wx/spinctrl.h"
#include "wx/wizard.h"
#include "wx/filepicker.h"
#include "wx/dynlib.h"
#include "wx/statline.h"
#include "database.h"
#include "dialogs.h"
//#include "maskededit.h"
#include "databasetype.h"

//typedef void (*CONNECTTODB)(const wxString &, const wxString &, void *&db, wxString &, WXHWND);
typedef Database *(*CONNECTTODB)(const wxString &, const wxString &, Database *db, wxString &, WXWidget, bool);

DatabaseType::DatabaseType(wxWindow *parent, const wxString &title, const wxString &name, const wxString &engine, const std::vector<std::wstring> &dsn, const std::vector<Profile> &profiles) : wxWizard(parent, wxID_ANY, title, wxNullBitmap, wxDefaultPosition, wxWIZARD_EX_HELPBUTTON | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX )
{
    m_profiles = profiles;
    m_newCurrent = "";
    button = FindWindowById( wxID_FORWARD );
    page1 = new DBType( this );
    page2 = new SQLiteConnect( this );
    page3 = new ODBCConnect( this, dsn );
    page4 = new PostgresConnect( this );
    page5 = new mySQLConnect( this );
    page6 = new SybaseSQLServer( this, true );
    page7 = new SybaseSQLServer( this, false );
    GetPageAreaSizer()->Add( page1 );
    GetPageAreaSizer()->Add( page2 );
    GetPageAreaSizer()->Add( page3 );
    GetPageAreaSizer()->Add( page4 );
    GetPageAreaSizer()->Add( page5 );
    GetPageAreaSizer()->Add( page6 );
    GetPageAreaSizer()->Add( page7 );
    if( engine == "SQLite" )
    {
        page1->GetComboBoxTypes()->SetValue( "SQLite" );
        page2->GetFileCtrl()->SetPath( name );
    }
    else if( engine == "ODBC" )
    {
        page1->GetComboBoxTypes()->SetValue( "ODBC" );
        page1->GetProfilesCtrl()->Enable( false );
        page3->GetDSNTypesCtrl()->SetStringSelection( name );
    }
    else
    {

    }
    Bind( wxEVT_WIZARD_PAGE_CHANGED, &DatabaseType::OnPageChanged, this );
    Bind( wxEVT_WIZARD_PAGE_CHANGING, &DatabaseType::OnPageChanging, this );
    Bind( wxEVT_WIZARD_FINISHED, &DatabaseType::OnConnect, this );
    button->Bind( wxEVT_UPDATE_UI, &DatabaseType::OnButtonUpdateUI, this );
}

void DatabaseType::OnButtonUpdateUI(wxUpdateUIEvent &event)
{
    if( GetCurrentPage() == page1 )
    {
        if( page1->GetProfilesCtrl()->GetValue() == "" && page1->GetComboBoxTypes()->GetValue() != _( "ODBC" ) )
            event.Enable( false );
        else
            event.Enable( true );
    }
    if( GetCurrentPage() == page2 )
    {
        if( page2->GetFileCtrl()->GetFileName() == wxEmptyString )
            event.Enable( false );
        else
            event.Enable( true );
    }
    else if( GetCurrentPage() == page3 )
    {
        if( dynamic_cast<ODBCConnect *>( page3 )->GetDSNTypesCtrl()->GetSelection() != wxNOT_FOUND )
            event.Enable( true );
        else
            event.Enable( false );
    }
    else if( GetCurrentPage() == page4 )
    {
        if( dynamic_cast<PostgresConnect *>( page4 )->GetPassword()->IsEmpty() )
            event.Enable( false );
        else
            event.Enable( true );
    }
    else if( GetCurrentPage() == page5 )
    {
        if( dynamic_cast<mySQLConnect *>( page5 )->GetPassword()->IsEmpty() )
            event.Enable( false );
        else
            event.Enable( true );
    }
}

void DatabaseType::OnPageChanging(wxWizardEvent &event)
{
    if( event.GetPage() == page1 )
    {
        if( page1->GetComboBoxTypes()->GetValue() != "ODBC" )
        {
            auto profile = page1->GetProfilesCtrl()->GetValue();
            if( std::find_if( m_profiles.begin(), m_profiles.end(), 
               [profile](Profile prof)
               {
                   return prof.m_name == profile;
               } ) != m_profiles.end() )
            {
                page1->GetErrorCtrl()->Show( true );
                page1->GetProfilesCtrl()->SetFocus();
                page1->GetProfilesCtrl()->SetSelection( -1, -1 );
                page1->Layout();
                Layout();
                event.Veto();
            }
        }
    }
}

void DatabaseType::OnPageChanged(wxWizardEvent &event)
{
    if( event.GetPage() == page1 )
    {
        button = FindWindowById( wxID_FORWARD );
        if( button )
        {
            button->SetLabel( _( "Next" ) );
            button->Enable( true );
        }
    }
    else
    {
        button = FindWindowById( wxID_FORWARD );
        if( button )
        {
            button->SetLabel( _( "Connect" ) );
            dynamic_cast<wxButton *>( button )->SetDefault();
            button->Enable( false );
        }
        m_newCurrent = page1->GetProfilesCtrl()->GetValue();
    }
    event.Skip();
}

void DatabaseType::OnConnect(wxWizardEvent &WXUNUSED(event))
{
    if( m_dbEngine == "SQLite" )
    {
        m_dbEngine = "SQLite";
        m_dbName = page2->GetFileCtrl()->GetPath();
        m_askForConnectParameter = false;
    }
    if( m_dbEngine == "ODBC" )
    {
        m_dbEngine = "ODBC";
        wxListBox *lbox = page3->GetDSNTypesCtrl();
        m_dbName = lbox->GetString( lbox->GetSelection() );
        wxCheckBox *check = page3->GetAskForParameters();
        m_askForConnectParameter = check->GetValue();
    }
    if( m_dbEngine == "PostgreSQL" )
    {
        m_dbEngine = "PostgreSQL";
        wxString host = page4->GetHost()->GetValue();
        if( !host.empty() )
            m_connStr = "host = " + host + " ";
        wxString hostAddr = page4->GetHostAddr()->GetValue();
        if( !hostAddr.empty() )
            m_connStr += "hostaddr = " + hostAddr + " ";
        wxString port = page4->GetPort()->GetValue();
        if( !port.empty() )
            m_connStr += "port = " + port + " ";
        else
            m_connStr += "port = " + wxString::Format( "%ld", 5432 ) + " ";
        m_connStr += "user = " + page4->GetUserID()->GetValue() + " ";
        m_connStr += "password = " + page4->GetPassword()->GetValue() + " ";
        wxString dbName = page4->GetDBName()->GetValue();
        if( !dbName.empty() )
            m_connStr += "dbname = " + dbName + " ";
        m_connStr += "connect_timeout = " + wxString::Format( "%d", page4->GetTimeout()->GetValue() ) + " ";
    }
    if( m_dbEngine == "mySQL" )
    {
        m_dbEngine = "MySQL";
        wxString host = page5->GetHost()->GetValue();
        if( !host.empty() )
            m_connStr = "host=" + host + " ";
        else
            m_connStr = "host=localhost ";
        wxString port = page5->GetPort()->GetValue();
        if( !port.empty() )
            m_connStr += "port=" + port + " ";
        else
            m_connStr += "port=" + wxString::Format( "%d", 3306 ) + " ";
        m_connStr += "user=" + page5->GetUserID()->GetValue() + " ";
        m_connStr += "password=" + page5->GetPassword()->GetValue() + " ";
        wxString dbName = page5->GetDBName()->GetValue();
        if( !dbName.empty() )
            m_connStr += "dbname=" + dbName + " ";
        wxString socket = page5->GetSocket()->GetValue();
        if( !socket.IsEmpty() )
            m_connStr += "socket=" + socket + " ";
        int flags = page5->GetFlags();
        if( flags )
            m_connStr += "flags=" + wxString::Format( "%d", flags ) + " ";
        wxString options = page5->GetOptions();
        if( !options.IsEmpty() )
            m_connStr += options;
    }
}

void DatabaseType::SetDbEngine(const wxString &engine)
{
    m_dbEngine = engine;
}

void DatabaseType::GetDatabaseEngine(wxString &engineName)
{
    engineName = page1->GetComboBoxTypes()->GetStringSelection();
}

bool DatabaseType::GetODBCConnectionParam()
{
    return m_askForConnectParameter;
}

wxString DatabaseType::GetDatabaseName()
{
    return m_dbName;
}

wxString &DatabaseType::GetConnectString() const
{
    return const_cast<wxString &>( m_connStr );
}

wxTextCtrl *DatabaseType::GetUserControl() const
{
    if( m_dbEngine == "SQLite" )
        return nullptr;
    if( m_dbEngine == "PostgreSQL" )
        return page4->GetUserID();
    if( m_dbEngine == "mySQL" )
        return page5->GetUserID();
    return nullptr;
}

DBType::DBType(wxWizard *parent) : wxWizardPage( parent )
{
    auto main = new wxBoxSizer( wxHORIZONTAL );
    auto sizer1 = new wxFlexGridSizer( 3, 2, 5, 5 );
    const wxString choices[] = { "SQLite", "ODBC", "MS SQL Server", "mySQL", "PostgreSQL", "Sybase", "Oracle" };
    auto label0 = new wxStaticText( this, wxID_ANY, _( "Profile" ) );
    profile = new wxTextCtrl( this, wxID_ANY );
    auto label = new wxStaticText( this, wxID_ANY, _( "Please select the database type" ) );
    m_types = new wxComboBox( this, wxID_ANY, "SQLite", wxDefaultPosition, wxDefaultSize, 7, choices, wxCB_READONLY );
    label1 = new wxStaticText( this, wxID_ANY, _( "This profile already exist!" ) );
    wxFont font = label->GetFont();
    font.MakeBold();
    label->SetFont( font );
    label1->SetForegroundColour( *wxRED );
    label1->Show( false );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( label0, 0, wxEXPAND, 0 );
    sizer1->Add( profile, 1, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( label1, 1, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0 );
    sizer1->Add( label, 0, wxEXPAND, 0 );
    sizer1->Add( m_types, 0, wxEXPAND, 0 );
    main->Add( sizer1, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizerAndFit( main );
    m_types->Bind( wxEVT_COMBOBOX, &DBType::OnComboSelecton, this );
}

wxWizardPage *DBType::GetPrev() const
{
    return nullptr;
}

wxWizardPage *DBType::GetNext() const
{
    wxString type = m_types->GetStringSelection();
    if( type == "SQLite" )
    {
        dynamic_cast<DatabaseType *>( GetParent() )->SetDbEngine( "SQLite" );
        return dynamic_cast<DatabaseType *>( GetParent() )->GetSQLitePage();
    }
    else if( type == "ODBC" )
    {
        dynamic_cast<DatabaseType *>( GetParent() )->SetDbEngine( "ODBC" );
        return dynamic_cast<DatabaseType *>( GetParent() )->GetODBCPage();
    }
    else if( type == "PostgreSQL" )
    {
        dynamic_cast<DatabaseType *>( GetParent() )->SetDbEngine( m_types->GetValue() );
        return dynamic_cast<DatabaseType *>( GetParent() )->GetPostgresPage();
    }
    else if( type == "mySQL" )
    {
        dynamic_cast<DatabaseType *>( GetParent() )->SetDbEngine( m_types->GetValue() );
        return dynamic_cast<DatabaseType *>( GetParent() )->GetmySQLPage();
    }
    else if( type == "Sybase" )
    {
        dynamic_cast<DatabaseType *>( GetParent() )->SetDbEngine( m_types->GetValue() );
        return dynamic_cast<DatabaseType *>( GetParent() )->GetSybasePage();
    }
    else if( type == "MS SQL Server" )
    {
        dynamic_cast<DatabaseType *>( GetParent() )->SetDbEngine( m_types->GetValue() );
        return dynamic_cast<DatabaseType *>( GetParent() )->GetSQLServerPage();
    }
    return nullptr;
}

wxComboBox *DBType::GetComboBoxTypes() const
{
    return m_types;
}

void DBType::OnComboSelecton(wxCommandEvent &WXUNUSED(event))
{
    if( m_types->GetValue() == _( "ODBC" ) )
        profile->Disable();
    else
        profile->Enable();
}

SQLiteConnect::SQLiteConnect(wxWizard *parent) : wxWizardPage( parent )
{
    wxSizer *main = new wxBoxSizer( wxHORIZONTAL );
    wxSizer *sizer1 = new wxBoxSizer( wxVERTICAL );
    wxStaticText *label = new wxStaticText( this, wxID_ANY, _( "Please select or enter the database name:" ) );
    dbName = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxFileSelectorPromptStr, "*.db", wxDefaultPosition, wxDefaultSize, wxFLP_OPEN | wxFLP_USE_TEXTCTRL );
    wxFont font = label->GetFont();
    font.MakeBold();
    label->SetFont( font );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( label, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( dbName, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    main->Add( sizer1, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizerAndFit( main );
}

wxWizardPage *SQLiteConnect::GetPrev() const
{
    return dynamic_cast<DatabaseType *>( GetParent() )->GetFirstPage();
}

wxWizardPage *SQLiteConnect::GetNext() const
{
    return nullptr;
}

ODBCConnect::ODBCConnect(wxWizard *parent, const std::vector<std::wstring> &dsn) : wxWizardPage( parent )
{
    wxSizer *main = new wxBoxSizer( wxHORIZONTAL );
    wxSizer *sizer1 = new wxBoxSizer( wxVERTICAL );
    wxStaticText *label = new wxStaticText( this, wxID_ANY, _( "Please select the database type" ) );
    m_types = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_SINGLE );
    wxFont font = label->GetFont();
    font.MakeBold();
    label->SetFont( font );
    m_ask = new wxCheckBox( this, wxID_ANY, _( "Prompt for Database Information" ) );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( label, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( m_types, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( m_ask, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    main->Add( sizer1, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    for( std::vector<std::wstring>::const_iterator it = dsn.begin(); it < dsn.end(); it++ )
    {
        m_types->Append( (*it) );
    }
    SetSizerAndFit( main );
    m_types->Bind( wxEVT_LISTBOX_DCLICK, &ODBCConnect::OnSelectionDClick, this );
}

wxWizardPage *ODBCConnect::GetPrev() const
{
    return dynamic_cast<DatabaseType *>( GetParent() )->GetFirstPage();
}

wxWizardPage *ODBCConnect::GetNext() const
{
    return nullptr;
}

void ODBCConnect::OnSelectionDClick(wxCommandEvent &WXUNUSED(event))
{
    dynamic_cast<DatabaseType *>( GetParent() )->EndModal( wxID_FORWARD );
}
/*
void ODBCConnect::AppendDSNsToList(const std::vector<std::string> &dsns)
{
    for( std::vector<std::string>::const_iterator it = dsns.begin(); it < dsns.end(); it++ )
    {
		m_types->Append( (*it) );
    }
}
*/
wxListBox *ODBCConnect::GetDSNTypesCtrl() const
{
    return const_cast<wxListBox *>( m_types );
}

wxCheckBox *ODBCConnect::GetAskForParameters() const
{
    return const_cast<wxCheckBox *>( m_ask );
}

PostgresConnect::PostgresConnect(wxWizard *parent) : wxWizardPage( parent )
{
    dynamic_cast<DatabaseType *>( GetParent() )->GetDatabaseEngine( m_engine );
    m_value = 5432;
    wxIntegerValidator<unsigned long> val( &m_value );
    val.SetRange( 1, 65535 );
    m_label1 = new wxStaticText( this, wxID_ANY, _( "Host" ) );
    m_host = new wxTextCtrl( this, wxID_ANY, "localhost" );
    m_label2 = new wxStaticText( this, wxID_ANY, _( "Host Address" ) );
//    m_hostAddr = new wxMaskEdit( this, wxID_ANY, "127.0.0.1" );
    m_hostAddr = new wxTextCtrl( this, wxID_ANY, "127.0.0.1" );
    m_label3 = new wxStaticText( this, wxID_ANY, _( "Port" ) );
    m_port = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val );
    m_label4 = new wxStaticText( this, wxID_ANY, _( "User ID" ) );
    m_userID = new wxTextCtrl( this, wxID_ANY, "postgres" );
    m_label5 = new wxStaticText( this, wxID_ANY, _( "Password" ) );
    m_password = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    m_label6 = new wxStaticText( this, wxID_ANY, _( "Database Name" ) );
    m_dbName = new wxTextCtrl( this, wxID_ANY, "" );
    m_label7 = new wxStaticText( this, wxID_ANY, _( "Connection Timeout" ) );
    m_timeout = new wxSpinCtrl( this, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxSP_WRAP, 0, 300 );
    m_advanced = new wxButton( this, wxID_ANY, _( "Advanced Options" ) );
    wxBoxSizer *main = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer1 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *sizer2 = new wxFlexGridSizer( 7, 2, 5, 5 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( m_label1, 0, wxEXPAND, 0 );
    sizer2->Add( m_host, 0, wxEXPAND, 0 );
    sizer2->Add( m_label2, 0, wxEXPAND, 0 );
    sizer2->Add( m_hostAddr, 0, wxEXPAND, 0 );
    sizer2->Add( m_label3, 0, wxEXPAND, 0 );
    sizer2->Add( m_port, 0, wxEXPAND, 0 );
    sizer2->Add( m_label4, 0, wxEXPAND, 0 );
    sizer2->Add( m_userID, 0, wxEXPAND, 0 );
    sizer2->Add( m_label5, 0, wxEXPAND, 0 );
    sizer2->Add( m_password, 0, wxEXPAND, 0 );
    sizer2->Add( m_label6, 0, wxEXPAND, 0 );
    sizer2->Add( m_dbName, 0, wxEXPAND, 0 );
    sizer2->Add( m_label7, 0, wxEXPAND, 0 );
    sizer2->Add( m_timeout, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( m_advanced, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    main->Add( sizer1, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizerAndFit( main );
    m_advanced->Bind( wxEVT_BUTTON, &PostgresConnect::OnAdvanced, this );
}

wxWizardPage *PostgresConnect::GetPrev() const
{
    return dynamic_cast<DatabaseType *>( GetParent() )->GetFirstPage();
}

wxWizardPage *PostgresConnect::GetNext() const
{
    return nullptr;
}

wxTextCtrl *PostgresConnect::GetDatabaseName()
{
    return m_dbName;
}

wxTextCtrl *PostgresConnect::GetHost() const
{
    return m_host;
}

wxTextCtrl *PostgresConnect::GetHostAddr() const
{
    return m_hostAddr;
}

wxTextCtrl *PostgresConnect::GetPort() const
{
    return m_port;
}

wxTextCtrl *PostgresConnect::GetUserID() const
{
    return m_userID;
}

wxTextCtrl *PostgresConnect::GetPassword() const
{
    return m_password;
}

wxTextCtrl *PostgresConnect::GetDBName() const
{
    return m_dbName;
}

wxSpinCtrl *PostgresConnect::GetTimeout() const
{
    return m_timeout;
}

void PostgresConnect::OnAdvanced(wxCommandEvent &WXUNUSED(event))
{
    PostgresAdvanced dlg( NULL );
    dlg.Centre();
    dlg.ShowModal();
}

PostgresAdvanced::PostgresAdvanced(wxWindow *parent) : wxDialog( parent, wxID_ANY, _( "Postgres Advanced Options" ) )
{
    m_panel = new wxPanel( this );
    m_label1 = new wxStaticText( m_panel, wxID_ANY, _( "Client Encoding" ) );
    m_encoding = new wxComboBox( m_panel, wxID_ANY );
}

mySQLConnect::mySQLConnect(wxWizard *parent) : wxWizardPage( parent )
{
    m_flags = 0;
    dynamic_cast<DatabaseType *>( GetParent() )->GetDatabaseEngine( m_engine );
    m_value = 3306;
    wxIntegerValidator<unsigned long> val( &m_value );
    val.SetRange( 1, 65535 );
    m_label1 = new wxStaticText( this, wxID_ANY, _( "Host" ) );
    m_host = new wxTextCtrl( this, wxID_ANY, "localhost" );
    m_label3 = new wxStaticText( this, wxID_ANY, _( "Port" ) );
    m_port = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val );
    m_label4 = new wxStaticText( this, wxID_ANY, _( "User ID" ) );
    m_userID = new wxTextCtrl( this, wxID_ANY, "root" );
    m_label5 = new wxStaticText( this, wxID_ANY, _( "Password" ) );
    m_password = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    m_label6 = new wxStaticText( this, wxID_ANY, _( "Database Name" ) );
    m_dbName = new wxTextCtrl( this, wxID_ANY, "" );
    m_label7 = new wxStaticText( this, wxID_ANY, _( "Unix Socket" ) );
    m_socket = new wxTextCtrl( this, wxID_ANY, "" );
    m_advanced = new wxButton( this, wxID_ANY, _( "Advaned Option" ) );
    wxBoxSizer *main = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer1 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *sizer2 = new wxFlexGridSizer( 6, 2, 5, 5 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( m_label1, 0, wxEXPAND, 0 );
    sizer2->Add( m_host, 0, wxEXPAND, 0 );
    sizer2->Add( m_label3, 0, wxEXPAND, 0 );
    sizer2->Add( m_port, 0, wxEXPAND, 0 );
    sizer2->Add( m_label4, 0, wxEXPAND, 0 );
    sizer2->Add( m_userID, 0, wxEXPAND, 0 );
    sizer2->Add( m_label5, 0, wxEXPAND, 0 );
    sizer2->Add( m_password, 0, wxEXPAND, 0 );
    sizer2->Add( m_label6, 0, wxEXPAND, 0 );
    sizer2->Add( m_dbName, 0, wxEXPAND, 0 );
    sizer2->Add( m_label7, 0, wxEXPAND, 0 );
    sizer2->Add( m_socket, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( m_advanced, 0, wxALIGN_CENTER_HORIZONTAL, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    main->Add( sizer1, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizerAndFit( main );
    m_advanced->Bind( wxEVT_BUTTON, &mySQLConnect::OnAdvanced, this );
}

wxWizardPage *mySQLConnect::GetPrev() const
{
    return dynamic_cast<DatabaseType *>( GetParent() )->GetFirstPage();
}

wxWizardPage *mySQLConnect::GetNext() const
{
    return nullptr;
}

wxTextCtrl *mySQLConnect::GetDatabaseName()
{
    return m_dbName;
}

wxTextCtrl *mySQLConnect::GetHost() const
{
    return m_host;
}

wxTextCtrl *mySQLConnect::GetPort() const
{
    return m_port;
}

wxTextCtrl *mySQLConnect::GetUserID() const
{
    return m_userID;
}

wxTextCtrl *mySQLConnect::GetPassword() const
{
    return m_password;
}

wxTextCtrl *mySQLConnect::GetDBName() const
{
    return m_dbName;
}

wxTextCtrl *mySQLConnect::GetSocket() const
{
    return m_socket;
}

int mySQLConnect::GetFlags()
{
    return m_flags;
}

wxString mySQLConnect::GetOptions() const
{
    return m_options;
}

void mySQLConnect::OnAdvanced(wxCommandEvent &WXUNUSED(event))
{
    mySQLAdvanced dlg( nullptr, m_flags );
    dlg.Centre();
    if( dlg.ShowModal() == wxID_OK )
    {
        if( dlg.m_expPass->IsChecked() )
            m_flags |= 1;
        if( dlg.m_clientCompress->IsChecked() )
            m_flags |= 2;
        if( dlg.m_foundRows->IsChecked() )
            m_flags |= 4;
        if( dlg.m_ignoreSigPipe->IsChecked() )
            m_flags |= 8;
        if( dlg.m_ignoreSpace->IsChecked() )
            m_flags |= 16;
        if( dlg.m_interactive->IsChecked() )
            m_flags |= 32;
        if( dlg.m_localFiles->IsChecked() )
            m_flags |= 64;
        if( dlg.m_multiResults->IsChecked() )
            m_flags |= 128;
        if( dlg.m_multiStat->IsChecked() )
            m_flags |= 256;
        if( dlg.m_noSchema->IsChecked() )
            m_flags |= 512;
        if( dlg.m_odbc->IsChecked() )
            m_flags |= 1024;
        if( dlg.m_ssl->IsChecked() )
            m_flags |= 2048;
        if( dlg.m_remember->IsChecked() )
            m_flags |= 4096;
        wxString defaultAuth = dlg.m_defaultAuth->GetValue();
        if( !defaultAuth.IsEmpty() )
            m_options += "MYSQL_DEFAULT_AUTH=" + defaultAuth + " ";
        bool cleartextPlugin = dlg.m_clearText->IsChecked();
        if( cleartextPlugin )
        {
            wxString temp = "MYSQL_ENABLE_CLEARTEXT_PLUGIN=1";
            temp += " ";
            m_options += temp;
        }
        wxString initCommand = dlg.m_initCommand->GetValue();
        if( !initCommand.IsEmpty() )
            m_options += "MYSQL_INIT_COMMAND=" + initCommand + " ";
        wxString optionBind = dlg.m_optBind->GetValue();
        if( !optionBind.IsEmpty() )
            m_options += "MYSQL_OPT_BIND=" + optionBind + " ";
        bool handleExpiredPass = dlg.m_handleExpiredPass->IsChecked();
        if( handleExpiredPass )
        {
            wxString temp = "MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORD=1";
            temp += " ";
            m_options += temp;
        }
        bool compress = dlg.m_optCompress->IsChecked();
        if( compress )
        {
            wxString temp = "MYSQL_OPT_COMPRESS=0";
            temp += " ";
            m_options +=  temp;
        }
        wxString attrDelete = dlg.m_connectAttrDelete->GetValue();
        if( !attrDelete.IsEmpty() )
            m_options += "MYSQL_OPT_CONNECT_ATTR_DELETE=" + attrDelete + " ";
    }
}

mySQLAdvanced::mySQLAdvanced(wxWindow *parent, int flags) : wxDialog( parent, wxID_ANY, _( "mySQL Advanced Options" ) ) 
{
    m_panel = new wxPanel( this );
    m_expPass = new wxCheckBox( m_panel, wxID_ANY, _( "Handle Expired Password" ) );
    m_clientCompress = new wxCheckBox( m_panel, wxID_ANY, _( "Client Compress" ) );
    m_foundRows = new wxCheckBox( m_panel, wxID_ANY, _( "Found Rows" ) );
    m_ignoreSigPipe = new wxCheckBox( m_panel, wxID_ANY, _( "Ignore SIGPIPE" ) );
    m_ignoreSpace = new wxCheckBox( m_panel, wxID_ANY, _( "Ignore Space" ) );
    m_interactive = new wxCheckBox( m_panel, wxID_ANY, _( "Interactive" ) );
    m_localFiles = new wxCheckBox( m_panel, wxID_ANY, _( "Local Files" ) );
    m_multiResults = new wxCheckBox( m_panel, wxID_ANY, _("Multi Results" ) );
    m_multiStat = new wxCheckBox( m_panel, wxID_ANY, _( "Multi Statements" ) );
    m_noSchema = new wxCheckBox( m_panel, wxID_ANY, _( "No Schema" ) );
    m_odbc = new wxCheckBox( m_panel, wxID_ANY, _( "ODBC" ) );
    m_ssl = new wxCheckBox( m_panel, wxID_ANY, _( "SSL" ) );
    m_remember = new wxCheckBox( m_panel, wxID_ANY, _( "Remember Options" ) );
    m_label1 = new wxStaticText( m_panel, wxID_ANY, _( "Default Authentication Plugin:" ) );
    m_defaultAuth = new wxTextCtrl( m_panel, wxID_ANY );
    m_clearText = new wxCheckBox( m_panel, wxID_ANY, _( "Clear Text Plugin" ) );
    m_label2 = new wxStaticText( m_panel, wxID_ANY, _( "Init Command:" ) );
    m_initCommand = new wxTextCtrl( m_panel, wxID_ANY );
    m_label3 = new wxStaticText( m_panel, wxID_ANY, _( "Opt Bind" ) );
    m_optBind = new wxTextCtrl( m_panel, wxID_ANY );
    m_handleExpiredPass = new wxCheckBox( m_panel, wxID_ANY, _( "Can Handle Expired Password" ) );
    m_optCompress = new wxCheckBox( m_panel, wxID_ANY, _( "Opt Compress" ) );
    m_label4 = new wxStaticText( m_panel, wxID_ANY, _( "Delete Connection Attribute:" ) );
    m_connectAttrDelete = new wxTextCtrl( m_panel, wxID_ANY );
    wxIntegerValidator<unsigned long> val;
    m_label5 = new wxStaticText( m_panel, wxID_ANY, _( "Connection Timeout:" ) );
    m_connectTimeout = new wxTextCtrl( m_panel, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, 0, val );
    m_guessConnect = new wxCheckBox( m_panel, wxID_ANY, _( "Guess Connection" ) );
    if( flags & 1 )
        m_expPass->SetValue( true );
    if( flags & 2 )
        m_clientCompress->SetValue( true );
    if( flags & 4 )
        m_foundRows->SetValue( true );
    if( flags & 8 )
        m_ignoreSigPipe->SetValue( true );
    if( flags & 16 )
        m_ignoreSpace->SetValue( true );
    if( flags & 32 )
        m_interactive->SetValue( true );
    if( flags & 64 )
        m_localFiles->SetValue( true );
    if( flags & 128 )
        m_multiResults->SetValue( true );
    if( flags & 256 )
        m_multiStat->SetValue( true );
    if( flags & 512 )
        m_noSchema->SetValue( true );
    if( flags & 1024 )
        m_odbc->SetValue( true );
    if( flags & 2048 )
        m_ssl->SetValue( true );
    if( flags & 4096 )
        m_remember->SetValue( true );
    wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer2 = new wxBoxSizer( wxVERTICAL );
    wxStdDialogButtonSizer *sizer3 = new wxStdDialogButtonSizer();
    sizer3->AddButton( new wxButton( m_panel, wxID_OK, _( "OK" ) ) );
    sizer3->AddButton( new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) ) );
    sizer3->AddButton( new wxButton( m_panel, wxID_HELP, _( "Help" ) ) );
    sizer3->Realize();
    wxFlexGridSizer *sizer4 = new wxFlexGridSizer( 5, 3, 5, 5 );
    wxFlexGridSizer *sizer5 = new wxFlexGridSizer( 5, 3, 5, 5 );
    wxBoxSizer *sizer6 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer7 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer8 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer9 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer10 = new wxBoxSizer( wxHORIZONTAL );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer4->Add( m_expPass, 0, wxEXPAND, 0 );
    sizer4->Add( m_clientCompress, 0, wxEXPAND, 0 );
    sizer4->Add( m_foundRows, 0, wxEXPAND, 0 );
    sizer4->Add( m_ignoreSigPipe, 0, wxEXPAND, 0 );
    sizer4->Add( m_ignoreSpace, 0, wxEXPAND, 0 );
    sizer4->Add( m_interactive, 0, wxEXPAND, 0 );
    sizer4->Add( m_localFiles, 0, wxEXPAND, 0 );
    sizer4->Add( m_multiResults, 0, wxEXPAND, 0 );
    sizer4->Add( m_multiStat, 0, wxEXPAND, 0 );
    sizer4->Add( m_noSchema, 0, wxEXPAND, 0 );
    sizer4->Add( m_odbc, 0, wxEXPAND, 0 );
    sizer4->Add( m_ssl, 0, wxEXPAND, 0 );
    sizer4->Add( m_remember, 0, wxEXPAND, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( sizer4, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( new wxStaticLine( m_panel ), 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer6->Add( m_label1, 0, wxEXPAND, 0 );
    sizer6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer6->Add( m_defaultAuth, 0, wxEXPAND, 0 );
    sizer5->Add( sizer6, 0, wxEXPAND, 0 );
    sizer5->Add( m_clearText, 0, wxEXPAND, 0 );
    sizer7->Add( m_label2, 0, wxEXPAND, 0 );
    sizer7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer7->Add( m_initCommand, 0, wxEXPAND, 0 );
    sizer5->Add( sizer7, 0, wxEXPAND, 0 );
    sizer8->Add( m_label3, 0, wxEXPAND, 0 );
    sizer8->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer8->Add( m_optBind, 0, wxEXPAND, 0 );
    sizer5->Add( sizer8, 0, wxEXPAND, 0 );
    sizer5->Add( m_handleExpiredPass, 0, wxEXPAND, 0 );
    sizer5->Add( m_optCompress, 0, wxEXPAND, 0 );
    sizer9->Add( m_label4, 0, wxEXPAND, 0 );
    sizer9->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer9->Add( m_connectAttrDelete, 0, wxEXPAND, 0 );
    sizer5->Add( sizer9, 0, wxEXPAND, 0 );
    sizer10->Add( m_label5, 0, wxEXPAND, 0 );
    sizer10->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer10->Add( m_connectTimeout, 0, wxEXPAND, 0 );
    sizer5->Add( sizer10, 0, wxEXPAND, 0 );
    sizer5->Add( m_guessConnect, 0, wxEXPAND, 0 );
    sizer2->Add( sizer5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( sizer3, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer1 );
    sizer->Add( m_panel, 0, wxEXPAND, 0 );
    SetSizer( sizer );
    sizer->Fit( this );
    Layout();
}

SybaseSQLServer::SybaseSQLServer(wxWizard* parent, bool isSQLServer) : wxWizardPage( parent )
{
    auto osId = wxPlatformInfo::Get().GetOperatingSystemId();
    // begin wxGlade: MyDialog::MyDialog
    auto sizer_1 = new wxBoxSizer( wxVERTICAL );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    const wxString m_api_choices[] = {
        "Native",
        "FreeTDS",
    };
    m_api = new wxRadioBox( this, wxID_ANY, "API", wxDefaultPosition, wxDefaultSize, 2, m_api_choices, 0, wxRA_SPECIFY_ROWS );
    if( isSQLServer )
    {
        m_api->Enable( 0, false );
        m_api->SetSelection( 1 );
    }
    else
        m_api->SetSelection( 0 );
    sizer_1->Add( m_api, 0, wxALIGN_CENTER_HORIZONTAL, 0 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    const wxString m_library_choices[] = {
        "ct-lib",
        "db-lib",
    };
    m_library = new wxRadioBox( this, wxID_ANY, "Library", wxDefaultPosition, wxDefaultSize, 2, m_library_choices, 0, wxRA_SPECIFY_ROWS );
    m_library->SetSelection( 0 );
    sizer_1->Add( m_library, 0, wxALIGN_CENTER_HORIZONTAL, 0 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    m_divider = new wxStaticLine( this, wxID_ANY );
    sizer_1->Add( m_divider, 0, wxEXPAND, 0 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    m_label1 = new wxStaticText( this, wxID_ANY, "User Name:" );
    m_label2 = new wxStaticText( this, wxID_ANY, "Password:" );
    m_label3 = new wxStaticText( this, wxID_ANY, "Server Name:" );
    m_user = new wxTextCtrl( this, wxID_ANY );
    m_password = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    m_serverName = new wxTextCtrl( this, wxID_ANY );
    auto sizer = new wxFlexGridSizer( 3, 2, 5, 5 );
    sizer->Add( m_label1, 0, wxEXPAND, 0 );
    sizer->Add( m_user, 0, wxEXPAND, 0 );
    sizer->Add( m_label2, 0, wxEXPAND, 0 );
    sizer->Add( m_password, 0, wxEXPAND, 0 );
    sizer->Add( m_label3, 0, wxEXPAND, 0 );
    sizer->Add( m_serverName, 0, wxEXPAND, 0 );
    sizer_1->Add( sizer, 0, wxEXPAND, 0 );
    sizer_1->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    // end wxGlade
}

wxWizardPage *SybaseSQLServer::GetPrev() const
{
    return dynamic_cast<DatabaseType *>( GetParent() )->GetFirstPage();
}

wxWizardPage *SybaseSQLServer::GetNext() const
{
    return nullptr;
}

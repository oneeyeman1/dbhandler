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
#include "wx/wizard.h"
#include "wx/filepicker.h"
#include "wx/dynlib.h"
#include "database.h"
#include "databasetype.h"

//typedef void (*CONNECTTODB)(const wxString &, const wxString &, void *&db, wxString &, WXHWND);
typedef Database *(*CONNECTTODB)(const wxString &, const wxString &, Database *db, wxString &, WXWidget, bool);

DatabaseType::DatabaseType(wxWindow *parent, const wxString &title, const wxString &name, const wxString &engine, const std::vector<std::wstring> &dsn) : wxWizard(parent, wxID_ANY, title, wxNullBitmap, wxDefaultPosition, wxWIZARD_EX_HELPBUTTON | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX )
{
    button = FindWindowById( wxID_FORWARD );
    page1 = new DBType( this );
    page2 = new SQLiteConnect( this );
    page3 = new ODBCConnect( this, dsn );
	page4 = new PostgresConnect( this );
    page5 = new mySQLConnect( this );
    GetPageAreaSizer()->Add( page1 );
    GetPageAreaSizer()->Add( page2 );
    GetPageAreaSizer()->Add( page3 );
    GetPageAreaSizer()->Add( page4 );
    GetPageAreaSizer()->Add( page5 );
    if( engine == "SQLite" )
    {
        page1->GetComboBoxTypes()->SetValue( "SQLite" );
        page2->GetFileCtrl()->SetPath( name );
    }
    else if( engine == "ODBC" )
    {
        page1->GetComboBoxTypes()->SetValue( "ODBC" );
        page3->GetDSNTypesCtrl()->SetStringSelection( name );
    }
    else
    {

    }
    Bind( wxEVT_WIZARD_PAGE_CHANGED, &DatabaseType::OnPageChanged, this );
    Bind( wxEVT_WIZARD_FINISHED, &DatabaseType::OnConnect, this );
    button->Bind( wxEVT_UPDATE_UI, &DatabaseType::OnButtonUpdateUI, this );
}

void DatabaseType::OnButtonUpdateUI(wxUpdateUIEvent &event)
{
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
    }
/*    if( event.GetPage() == page3 )
    {
        std::vector<std::string> dsns;
        std::vector<SQLWCHAR *> errorMsg;
        if( static_cast<ODBCDatabase *>( m_db )->GetDsnList( dsns, errorMsg ) )
        {
            for( std::vector<SQLWCHAR *>::iterator it = errorMsg.begin(); it != errorMsg.end(); it++ )
            {
                wxString temp = wxMBConvUTF16().cMB2WC( (const char *) (*it) );
                wxMessageBox( temp, "Error!" );
            }
        }
        else
            dynamic_cast<ODBCConnect *>( page3 )->AppendDSNsToList( dsns );
    }*/
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
        m_connStr += "dbname = " + page4->GetDBName()->GetValue() + " ";
    }
    if( m_dbEngine == "mySQL" )
    {
        m_dbEngine = "MySQL";
        wxString host = page5->GetHost()->GetValue();
        if( !host.empty() )
            m_connStr = "host=" + host + " ";
        wxString port = page5->GetPort()->GetValue();
        if( !port.empty() )
            m_connStr += "port=" + port + " ";
		else
            m_connStr += "port=" + wxString::Format( "%d", 3306 ) + " ";
        m_connStr += "user=" + page5->GetUserID()->GetValue() + " ";
        m_connStr += "password=" + page5->GetPassword()->GetValue() + " ";
        m_connStr += "dbname=" + page5->GetDBName()->GetValue() + " ";
        
    }
/*    WXWidget hwnd = 0;
    wxString driver;
    bool askForConnectParameter = false;
    if( m_dbEngine == "SQLite" )
    wxDynamicLibrary lib;
#ifdef __WXMSW__
    lib.Load( "dbloader" );
#else
    lib.Load( "libdbloader" );
#endif
    if( lib.IsLoaded() )
    {
        CONNECTTODB func = (CONNECTTODB) lib.GetSymbol( "ConnectToDb" );
        m_db = func( m_dbName, m_dbEngine, m_db, driver, hwnd, askForConnectParameter );
    }*/
/*    wxString dbName;
	std::vector<std::wstring> errorMsg;
    if( event.GetPage() == page2 )
    {
        dbName = dynamic_cast<SQLiteConnect *>( page2 )->GetFileCtrl()->GetPath();
    }
    if( event.GetPage() == page3 )
    {
        dbName = dynamic_cast<ODBCConnect *>( page3 )->GetDSNTypesCtrl().GetStringSelection();
        wxWindow *parent = GetParent();
        dynamic_cast<ODBCDatabase *>( m_db )->SetWindowHandle( parent->GetHandle() );
    }
    if( !m_db->Connect( dbName, errorMsg ) )
    {
        for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it != errorMsg.end(); it++ )
        {
            wxMessageBox( (*it) );
        }
    }*/
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

DBType::DBType(wxWizard *parent) : wxWizardPage( parent )
{
    wxSizer *main = new wxBoxSizer( wxHORIZONTAL );
    wxSizer *sizer1 = new wxBoxSizer( wxVERTICAL );
    const wxString choices[] = { "SQLite", "ODBC", "MS SQL Server", "mySQL", "PostgreSQL", "Sybase", "Oracle" };
    wxStaticText *label = new wxStaticText( this, wxID_ANY, _( "Please select the database type" ) );
    m_types = new wxComboBox( this, wxID_ANY, "SQLite", wxDefaultPosition, wxDefaultSize, 7, choices, wxCB_READONLY );
    wxFont font = label->GetFont();
    font.MakeBold();
    label->SetFont( font );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( label, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( m_types, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    main->Add( sizer1, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizerAndFit( main );
}

wxWizardPage *DBType::GetPrev() const
{
    return NULL;
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
    return NULL;
}

wxComboBox *DBType::GetComboBoxTypes() const
{
    return m_types;
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
    return NULL;
}

ODBCConnect::ODBCConnect(wxWizard *parent, const std::vector<std::wstring> &dsn) : wxWizardPage( parent )
{
    wxSizer *main = new wxBoxSizer( wxHORIZONTAL );
    wxSizer *sizer1 = new wxBoxSizer( wxVERTICAL );
    wxStaticText *label = new wxStaticText( this, wxID_ANY, _( "Please select the database type" ) );
    m_types = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
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
}

wxWizardPage *ODBCConnect::GetPrev() const
{
    return dynamic_cast<DatabaseType *>( GetParent() )->GetFirstPage();
}

wxWizardPage *ODBCConnect::GetNext() const
{
    return NULL;
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
    m_hostAddr = new wxTextCtrl( this, wxID_ANY, "127.0.0.1" );
    m_label3 = new wxStaticText( this, wxID_ANY, _( "Port" ) );
    m_port = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val );
    m_label4 = new wxStaticText( this, wxID_ANY, _( "User ID" ) );
    m_userID = new wxTextCtrl( this, wxID_ANY, "postgres" );
    m_label5 = new wxStaticText( this, wxID_ANY, _( "Password" ) );
    m_password = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    m_label6 = new wxStaticText( this, wxID_ANY, _( "Database Name" ) );
    m_dbName = new wxTextCtrl( this, wxID_ANY, "" );
    wxBoxSizer *main = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer1 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *sizer2 = new wxFlexGridSizer( 6, 2, 5, 5 );
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
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    main->Add( sizer1, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizerAndFit( main );
}

wxWizardPage *PostgresConnect::GetPrev() const
{
    return dynamic_cast<DatabaseType *>( GetParent() )->GetFirstPage();
}

wxWizardPage *PostgresConnect::GetNext() const
{
    return NULL;
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

mySQLConnect::mySQLConnect(wxWizard *parent) : wxWizardPage( parent )
{
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
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    main->Add( sizer1, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizerAndFit( main );
}

wxWizardPage *mySQLConnect::GetPrev() const
{
    return dynamic_cast<DatabaseType *>( GetParent() )->GetFirstPage();
}

wxWizardPage *mySQLConnect::GetNext() const
{
    return NULL;
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

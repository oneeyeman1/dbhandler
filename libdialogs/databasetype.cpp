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
#ifdef _IODBCUNIX_H
#include "iODBC/sqlext.h"
#else
#include <sqlext.h>
#endif
#include "wx/wizard.h"
#include "wx/filepicker.h"
#include "wx/dynlib.h"
#include "database.h"
#include "databasetype.h"

//typedef void (*CONNECTTODB)(const wxString &, const wxString &, void *&db, wxString &, WXHWND);
typedef Database *(*CONNECTTODB)(const wxString &, const wxString &, Database *db, wxString &, WXWidget, bool);

DatabaseType::DatabaseType(wxWindow *parent, const wxString &title, const wxString &name, const wxString &engine) : wxWizard(parent, wxID_ANY, title, wxNullBitmap, wxDefaultPosition, wxWIZARD_EX_HELPBUTTON | wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX )
{
    button = FindWindowById( wxID_FORWARD );
    page1 = new DBType( this );
    page2 = new SQLiteConnect( this );
    page3 = new ODBCConnect( this );
    GetPageAreaSizer()->Add( page1 );
    GetPageAreaSizer()->Add( page2 );
    GetPageAreaSizer()->Add( page3 );
    if( engine == "SQLite" )
    {
        page1->GetComboBoxTypes()->SetValue( "SQLite" );
        page2->GetFileCtrl()->SetPath( name );
    }
    if( engine == "ODBC" )
    {
        page1->GetComboBoxTypes()->SetValue( "ODBC" );
        page3->GetDSNTypesCtrl()->SetStringSelection( name );
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
			button->Enable( false );
        }
    }
//    m_lib = new wxDynamicLibrary;
	if( event.GetPage() == page2 )
    {
        m_dbEngine = "SQLite";
/*		m_lib->Load( "sqlite3" );
        if( !m_db )
            m_db = new SQLiteDatabase( m_lib );*/
    }
    if( event.GetPage() == page3 )
    {
        m_dbEngine = "ODBC";
/*		m_lib->Load( "odbc32" );
        if( !m_db )
            m_db = new ODBCDatabase( m_lib );
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
            dynamic_cast<ODBCConnect *>( page3 )->AppendDSNsToList( dsns );*/
    }
    event.Skip();
}

void DatabaseType::OnConnect(wxWizardEvent &WXUNUSED(event))
{
    if( m_dbEngine == "SQLite" )
    {
        m_dbName = page2->GetFileCtrl()->GetPath();
        m_askForConnectParameter = false;
    }
    if( m_dbEngine == "ODBC" )
    {
        wxListBox *lbox = page3->GetDSNTypesCtrl();
        m_dbName = lbox->GetString( lbox->GetSelection() );
        wxCheckBox *check = page3->GetAskForParameters();
        m_askForConnectParameter = check->GetValue();
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

DBType::DBType(wxWizard *parent) : wxWizardPage( parent )
{
    wxSizer *main = new wxBoxSizer( wxHORIZONTAL );
    wxSizer *sizer1 = new wxBoxSizer( wxVERTICAL );
	const wxString choices[] = { "SQLite", "ODBC", "MS SQL Server", "Sybase", "Oracle" };
    wxStaticText *label = new wxStaticText( this, wxID_ANY, _( "Please select the database type" ) );
    m_types = new wxComboBox( this, wxID_ANY, "SQLite", wxDefaultPosition, wxDefaultSize, 5, choices, wxCB_READONLY );
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
        return dynamic_cast<DatabaseType *>( GetParent() )->GetSQLitePage();
    }
	else if( type == "ODBC" )
    {
		return dynamic_cast<DatabaseType *>( GetParent() )->GetODBCPage();
    }
	else
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

ODBCConnect::ODBCConnect(wxWizard *parent) : wxWizardPage( parent )
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
    GetDSNList();
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

void ODBCConnect::GetDSNList()
{
    wxDynamicLibrary lib;
#ifdef __WXMSW__
	lib.Load( "dbloader" );
#else
    lib.Load( "libdbloader" );
#endif
    if( lib.IsLoaded() )
    {
        wxString error;
        CONVERTFROMSQLWCHAR func = (CONVERTFROMSQLWCHAR) lib.GetSymbol( "ConvertFromSQLWCHAR" );
        SQLSMALLINT i = 1, cbErrorMsg, direct = SQL_FETCH_FIRST;
        SQLWCHAR dsn[SQL_MAX_DSN_LENGTH+1], sqlState[20], errMsg[SQL_MAX_MESSAGE_LENGTH], dsnDescr[255];
        SWORD pcbDSN, pcbDesc;
        SQLINTEGER nativeError;
        RETCODE ret = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HENV, &m_henv );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
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
                while( ( ret = SQLGetDiagRec( SQL_HANDLE_ENV, m_henv, i, sqlState, &nativeError, errMsg, sizeof( errMsg ), &cbErrorMsg ) ) == SQL_SUCCESS )
                {
                    func( errMsg, error );
                    wxMessageBox( error );
                    i++;
                }
            }
            else
            {
                while( ( ret = SQLDataSources( m_henv, direct, dsn, SQL_MAX_DSN_LENGTH, &pcbDSN, dsnDescr, 254, &pcbDesc ) ) == SQL_SUCCESS )
                {
//                wxString s1 = func( dsnDescr );
//                wxString s2 = func( dsn );
                    wxString s1, s2;
                    func( dsnDescr, s1 );
                    func( dsn, s2 );
                    m_types->Append( s2, new wxStringClientData( s1 ) );
                    direct = SQL_FETCH_NEXT;
                }
                if( ret != SQL_SUCCESS && ret != SQL_NO_DATA )
                {
                    while( ( ret = SQLGetDiagRec( SQL_HANDLE_ENV, m_henv, i, sqlState, &nativeError, errMsg, sizeof( errMsg ), &cbErrorMsg ) ) == SQL_SUCCESS )
                    {
                        func( errMsg, error );
                        wxMessageBox( error );
                        i++;
                    }
                }
            }
            SQLFreeHandle( SQL_HANDLE_ENV, m_henv );
        }
    }
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

#if _MSC_VER >= 1900
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif
#ifdef WIN32
#include <windows.h>
#endif
#ifndef WIN32
#include <sstream>
#endif

#if defined __WXMSW__ && defined __MEMORYLEAKS__
#include <vld.h>
#endif

#include <map>
#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#ifdef _IODBCUNIX_FRAMEWORK
#include "iODBC/sql.h"
#include "iODBC/sqlext.h"
#include "iODBCinst/odbcinst.h"
#else
#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>
#endif
#include "string.h"
#include "database.h"
#include "database_odbc.h"

std::mutex Impl::my_mutex;

ODBCDatabase::ODBCDatabase(const int osId, const std::wstring &desktop) : Database( osId, desktop )
{
    m_env = 0;
    m_hdbc = 0;
    m_hstmt = 0;
    odbc_pimpl = nullptr;
    m_oneStatement = false;
    m_connectString = nullptr;
    m_isConnected = false;
    connectToDatabase = false;
    m_fieldsInRecordSet = 0;
}

ODBCDatabase::~ODBCDatabase()
{
    RETCODE ret;
    std::vector<std::wstring> errorMsg;
    delete[] m_connectString;
    m_connectString = nullptr;
    delete odbc_pimpl;
    odbc_pimpl = nullptr;
    if( m_hdbc != 0 )
    {
        ret = SQLFreeHandle( SQL_HANDLE_DBC, m_hdbc );
        if( ret != SQL_SUCCESS )
            GetErrorMessage( errorMsg, CONN_ERROR );
        m_hdbc = 0;
        if( m_env != 0 )
        {
            ret = SQLFreeHandle( SQL_HANDLE_ENV, m_env );
            if( ret != SQL_SUCCESS )
                GetErrorMessage( errorMsg, ENV_ERROR );
            m_env = 0;
        }
    }
}

bool ODBCDatabase::GetDriverList(std::map<std::wstring, std::vector<std::wstring> > &driversDSN, std::vector<std::wstring> &errMsg)
{
    bool result = true;
    std::wstring s1, s2, error, dsn_result;
    SQLWCHAR driverDesc[1024], driverAttributes[256], dsn[SQL_MAX_DSN_LENGTH+1], dsnDescr[255];
    SQLSMALLINT attr_ret;
    SWORD pcbDSN, pcbDesc;
    SQLSMALLINT descriptionLength;
    SQLUSMALLINT direction = SQL_FETCH_FIRST, direct = SQL_FETCH_FIRST;
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HENV, &m_env ), ret1, ret2;
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        result = false;
        GetErrorMessage( errMsg, ENV_ERROR );
    }
    else
    {
        ret = SQLSetEnvAttr( m_env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0 );
        if( ret != SQL_SUCCESS )
        {
            result = false;
            GetErrorMessage( errMsg, ENV_ERROR );
        }
        else
        {
            while( ( ret1 = SQLDrivers( m_env, direction, driverDesc, sizeof( driverDesc ) - 1, &descriptionLength, driverAttributes, sizeof( driverAttributes ), &attr_ret ) ) == SQL_SUCCESS )
            {
                str_to_uc_cpy( s1, driverDesc );
                while( ( ret2 = SQLDataSources( m_env, direct, dsn, SQL_MAX_DSN_LENGTH, &pcbDSN, dsnDescr, 254, &pcbDesc ) ) == SQL_SUCCESS )
                {
                    str_to_uc_cpy( s2, dsnDescr );
                    if( s1 == s2 )
                    {
                        str_to_uc_cpy( dsn_result, dsn  );
                        driversDSN[s1].push_back( dsn_result );
                        dsn_result = L"";
                    }
                    direct = SQL_FETCH_NEXT;
                    s2 = L"";
                }
                if( ret2 != SQL_SUCCESS && ret2 != SQL_NO_DATA )
                {
                    result = false;
                    GetErrorMessage( errMsg, ENV_ERROR, m_env );
                    driversDSN.clear();
                    break;
                }
                else
                {
                    direction = SQL_FETCH_FIRST;
                    direct = SQL_FETCH_FIRST;
                }
                if( driversDSN.find( s1 ) == driversDSN.end() )
                    driversDSN[s1].push_back( L"" );
                if( ret2 == SQL_NO_DATA )
                    ret2 = SQL_SUCCESS;
                direction = SQL_FETCH_NEXT;
                s1 = L"";
            }
            if( ret1 != SQL_SUCCESS && ret1 != SQL_NO_DATA )
            {
                result = false;
                driversDSN.clear();
                GetErrorMessage( errMsg, ENV_ERROR );
            }
        }
    }
    return result;
}

bool ODBCDatabase::GetDSNList(std::vector<std::wstring> &dsns, std::vector<std::wstring> &errorMsg)
{
    bool result = false;
    SQLWCHAR dsn[SQL_MAX_DSN_LENGTH+1], dsnDescr[255];
    SQLSMALLINT direct = SQL_FETCH_FIRST;
    SWORD pcbDSN, pcbDesc;
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HENV, &m_env );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, ENV_ERROR );
        result = true;
    }
    else
    {
        ret = SQLSetEnvAttr( m_env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0 );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, ENV_ERROR );
            result = true;
        }
        else
        {
            ret = SQLDataSources( m_env, direct, dsn, SQL_MAX_DSN_LENGTH, &pcbDSN, dsnDescr, 254, &pcbDesc );
            while( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
            {
                std::wstring s1;
                str_to_uc_cpy( s1, dsn );
                dsns.push_back( s1 );
                direct = SQL_FETCH_NEXT;
                ret = SQLDataSources( m_env, direct, dsn, SQL_MAX_DSN_LENGTH, &pcbDSN, dsnDescr, 254, &pcbDesc );
            }
            if( ret != SQL_SUCCESS && ret != SQL_NO_DATA )
            {
                GetErrorMessage( errorMsg, ENV_ERROR );
                result = true;
            }
        }
    }
    ret = SQLFreeHandle( SQL_HANDLE_ENV, m_env );
    if( ret != SQL_SUCCESS && ret != SQL_NO_DATA )
    {
        GetErrorMessage( errorMsg, ENV_ERROR );
        result = true;
    }
    return result;
}

bool ODBCDatabase::AddDsn(SQLHWND hwnd, const std::wstring &driver, std::vector<std::wstring> &errorMsg)
{
    bool result = true;
    std::wstring error;
    SQLWCHAR buf[256];
    SQLWCHAR temp1[512];
    memset( temp1, 0, 512 );
    memset( buf, 0, 256 );
    uc_to_str_cpy( temp1, driver );
    BOOL ret = SQLConfigDataSource( hwnd, ODBC_ADD_DSN, temp1, buf );
    if( !ret )
    {
        GetDSNErrorMessage( errorMsg );
        if( !errorMsg.empty() )
            result = false;
    }
    return result;
}

bool ODBCDatabase::EditDsn(SQLHWND hwnd, const std::wstring &driver, const std::wstring &dsn, std::vector<std::wstring> &errorMsg)
{
    bool result = true;
    SQLWCHAR temp1[1024];
    SQLWCHAR temp2[1024];
    memset( temp1, 0, 1024 );
    memset( temp2, 0, 1024 );
    uc_to_str_cpy( temp1, driver );
    uc_to_str_cpy( temp2, L"DSN=" );
    uc_to_str_cpy( temp2, dsn );
    BOOL ret= SQLConfigDataSource( hwnd, ODBC_CONFIG_DSN, temp1, temp2 );
    if( !ret )
    {
        GetDSNErrorMessage( errorMsg );
        if( !errorMsg.empty() )
            result = false;
    }
    return result;
}

bool ODBCDatabase::RemoveDsn(const std::wstring &driver, const std::wstring &dsn, std::vector<std::wstring> &errorMsg)
{
    bool result = true;
    SQLWCHAR temp1[1024];
    SQLWCHAR temp2[1024];
    memset( temp1, 0, 1024 );
    memset( temp2, 0, 1024 );
    uc_to_str_cpy( temp1, driver );
    uc_to_str_cpy( temp2, L"DSN=" );
    uc_to_str_cpy( temp2, dsn );
    BOOL ret= SQLConfigDataSource( NULL, ODBC_REMOVE_DSN, temp1, temp2 );
    if( !ret )
    {
        GetDSNErrorMessage( errorMsg );
        if( !errorMsg.empty() )
            result = false;
    }
    return result;
}

void ODBCDatabase::SetWindowHandle(SQLHWND handle)
{
    m_handle = handle;
}

void ODBCDatabase::AskForConnectionParameter(bool ask)
{
    m_ask = ask;
}

int ODBCDatabase::GetErrorMessage(std::vector<std::wstring> &errorMsg, int type, SQLHSTMT stmt)
{
    RETCODE ret;
    SQLWCHAR sqlstate[20], msg[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER native_error;
    SQLSMALLINT i = 1, msglen, option = SQL_HANDLE_STMT;
    SQLHANDLE handle = m_hstmt;
    switch( type )
    {
        case ENV_ERROR:
            option = SQL_HANDLE_ENV;
            handle = m_env;
            break;
        case STMT_ERROR:
            option = SQL_HANDLE_STMT;
            handle = stmt == 0 ? m_hstmt : stmt;
            break;
        case CONN_ERROR:
            option = SQL_HANDLE_DBC;
            handle = stmt == 0 ? m_hdbc : stmt;
            break;
    }
    while( ( ret = SQLGetDiagRec( option, handle, i, sqlstate, &native_error, msg, sizeof( msg ), &msglen ) ) == SQL_SUCCESS )
    {
        std::wstring state;
        str_to_uc_cpy( state, sqlstate );
        if( state != L"HYC00" )
        {
            std::wstring strMsg;
            str_to_uc_cpy( strMsg, msg );
            errorMsg.push_back( strMsg );
        }
        i++;
    }
    return 0;
}

int ODBCDatabase::GetDSNErrorMessage(std::vector<std::wstring> &errorMsg)
{
    WORD i = 1, cbErrorMsgMax = SQL_MAX_MESSAGE_LENGTH, pcbErrorMsg;
    SQLWCHAR errorMessage[512];
    DWORD pfErrorCode;
    RETCODE ret;
    while( ( ret = SQLInstallerError( i, &pfErrorCode, errorMessage, cbErrorMsgMax, &pcbErrorMsg ) ) == SQL_SUCCESS )
    {
        if( pfErrorCode != ODBC_ERROR_USER_CANCELED )
        {
            std::wstring strMsg;
            i++;
            str_to_uc_cpy( strMsg, errorMessage );
            errorMsg.push_back( strMsg );
        }
        else break;
    }
    return 0;
}

void ODBCDatabase::uc_to_str_cpy(SQLWCHAR *dest, const std::wstring &src)
{
    const wchar_t *temp = src.c_str();
    while( *dest )
    {
        dest++;
    }
    while( *temp )
    {
        *dest = *temp;
        dest++;
        temp++;
    }
    *dest++ = '\0';
    *dest = '\0';
}

void ODBCDatabase::str_to_uc_cpy(std::wstring &dest, const SQLWCHAR *src)
{
    while( src && *src )
    {
        dest += *src++;
    }
}

bool ODBCDatabase::equal(SQLWCHAR *dest, SQLWCHAR *src)
{
    bool eq = true;
    while( *dest && eq )
        if( *src++ != *dest++ )
            eq = false;
    return eq;
}

void ODBCDatabase::copy_uc_to_uc(SQLWCHAR *dest, SQLWCHAR *src)
{
    while( *dest )
        dest++;
    while( *src )
        *dest++ = *src++;
    *dest = 0;
}

int ODBCDatabase::GetSQLStringSize(SQLWCHAR *str)
{
    int ret = 1;
    while( *str++ )
        ret++;
    return ret;
}

int ODBCDatabase::GetDriverForDSN(SQLWCHAR *dsn, SQLWCHAR *driver, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    bool found = false;
    RETCODE ret1;
    SQLUSMALLINT direct = SQL_FETCH_FIRST;
    SWORD pcbDSN, pcbDesc;
    SQLWCHAR dsnDescr[255], dsnRead[SQL_MAX_DSN_LENGTH + 1];
    ret1 = SQLDataSources( m_env, direct, dsnRead, SQL_MAX_DSN_LENGTH, &pcbDSN, dsnDescr, 254, &pcbDesc );
    while( ( ret1 == SQL_SUCCESS || ret1 == SQL_SUCCESS_WITH_INFO ) )
    {
        if( equal( dsn, dsnRead ) )
        {
            found = true;
            break;
        }
        direct = SQL_FETCH_NEXT;
        ret1 = SQLDataSources( m_env, direct, dsnRead, SQL_MAX_DSN_LENGTH, &pcbDSN, dsnDescr, 254, &pcbDesc );
    }
    if( ret1 != SQL_SUCCESS && ret1 != SQL_NO_DATA )
    {
        GetErrorMessage( errorMsg, ENV_ERROR );
        result = 1;
    }
    else
    {
        if( found )
            copy_uc_to_uc( driver, dsnDescr );
    }
    return result;
}

int ODBCDatabase::CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    SQLWCHAR *query = nullptr;
    std::wstring qry = L"CREATE DATABASE " + name + L" ";
    if( pimpl.m_subtype == L"Microsoft SQL Server" )
        qry = L"CREATE DATABASE " + name + L"";
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    if( !result )
    {
        query = new SQLWCHAR[name.length() + 2];
        memset( query, '\0', name.length() + 2 );
        uc_to_str_cpy( query, name );
        ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
        delete[] query;
        query = nullptr;
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( m_hstmt )
    {
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        m_hstmt = 0;
    }
    delete[] query;
    query = nullptr;
    return result;
}

int ODBCDatabase::DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring qry = L"DROP DATABASE " + name;
    SQLWCHAR *query = nullptr;
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    if( !result )
    {
        query = new SQLWCHAR[qry.length() + 2];
        memset( query, '\0', qry.length() + 2 );
        uc_to_str_cpy( query, qry );
        ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
        delete[] query;
        query = nullptr;
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( m_hstmt )
    {
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        m_hstmt = 0;
    }
    return result;
}

int ODBCDatabase::Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg)
{
    int result = 0, bufferSize = 1024;
    std::vector<SQLWCHAR *> errorMessage;
    SQLWCHAR connectStrIn[sizeof(SQLWCHAR) * 255], driver[1024], dsn[1024], dbType[1024], *query = nullptr;
    SQLSMALLINT OutConnStrLen;
    SQLRETURN ret;
    SQLUSMALLINT options;
    SQLWCHAR *user = nullptr, *password = nullptr;
    SQLWCHAR driverName[1024];
    SQLWCHAR dbName[1024];
    SQLWCHAR userName[1024];
    std::wstring connectingDSN, connectingUser = L"", connectingPassword = L"";
    pimpl.m_type = L"ODBC";
    pimpl.m_pgLogFile = L"";
    if( !odbc_pimpl )
        odbc_pimpl = new ODBCImpl;
    std::wstring::size_type pos = selectedDSN.find( L';' );
    if( pos == std::wstring::npos )
        connectingDSN = selectedDSN;
    else
    {
        std::wstring temp = selectedDSN.substr( selectedDSN.find( L"DSN=" ) + 4 );
        connectingDSN = temp.substr( 0, temp.find( L";") );
//        connectingDSN = selectedDSN.substr( 0, pos );
        temp = temp.substr( temp.find( L"UID=" ) + 4 );
        connectingUser = temp.substr( 0, temp.find( L";" ) );
        temp = temp.substr( temp.find( L"PWD=" ) + 4 );
        connectingPassword = temp.substr( 0, temp.find( L";" ) );
        user = new SQLWCHAR[connectingUser.length() + 2];
        password = new SQLWCHAR[connectingPassword.length() + 2];
        memset( user, '\0', connectingUser.length() + 2 );
        memset( password, '\0', connectingPassword.length() + 2 );
        uc_to_str_cpy( user, connectingUser );
        uc_to_str_cpy( password, connectingPassword );
    }
    m_connectString = new SQLWCHAR[sizeof(SQLWCHAR) * 1024];
    memset( dsn, 0, sizeof( dsn ) );
    memset( connectStrIn, 0, sizeof( connectStrIn ) );
    memset( driver, 0, sizeof( driver ) );
    uc_to_str_cpy( dsn, connectingDSN.c_str() );
    ret = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HENV, &m_env );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        errorMsg.push_back( L"Failed to allocate memory for the connection" );
        return 1;
    }
    ret = SQLSetEnvAttr( m_env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, ENV_ERROR );
        result = 1;
    }
    else
    {
        ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &m_hdbc );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, ENV_ERROR );
            return 1;
        }
        else
        {
            if( !GetDriverForDSN( dsn, driver, errorMsg ) )
            {
                SQLWCHAR *tempPostgres = new SQLWCHAR[13];
                memset( tempPostgres, '\0', 13 );
                uc_to_str_cpy( tempPostgres, L"PostgreSQL " );
                if( connectStrIn[0] == '\0' )
                {
                    uc_to_str_cpy( connectStrIn, L"DSN=" );
                    uc_to_str_cpy( connectStrIn, connectingDSN.c_str() );
                }
                if( equal( tempPostgres, driver ) )
                    uc_to_str_cpy( connectStrIn, L";UseServerSidePrepare=1;ShowSystemTables=1;" );
                delete[] tempPostgres;
                tempPostgres = nullptr;
                if( user && password )
                {
                    uc_to_str_cpy( connectStrIn, L";UID=" );
                    copy_uc_to_uc( connectStrIn, user );
                    uc_to_str_cpy( connectStrIn, L";PWD=" );
                    copy_uc_to_uc( connectStrIn, password );
                }
                delete[] user;
                user = nullptr;
                delete[] password;
                password = nullptr;
                ret = SQLSetConnectAttr( m_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)50, 0 );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                {
                    GetErrorMessage( errorMsg, 2 );
                    result = 1;
                }
                if( !result )
                {
#ifdef _WIN32
                    options = m_ask ? SQL_DRIVER_COMPLETE : SQL_DRIVER_NOPROMPT;
#else
                    options = SQL_DRIVER_NOPROMPT;
#endif
                    ret = SQLDriverConnect( m_hdbc, m_handle, connectStrIn, SQL_NTS, m_connectString, 1024, &OutConnStrLen, options );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                    {
                        GetErrorMessage( errorMsg, CONN_ERROR );
                        result = 1;
                    }
                    if( ret == SQL_NO_DATA )
                    {
                        errorMsg.push_back( L"Connection cancelled" );
                        return 0;
                    }
                }
                if( !result )
                {
                    str_to_uc_cpy( pimpl.m_connectString, m_connectString );
                    ret = SQLGetInfo( m_hdbc, SQL_DBMS_NAME, dbType, (SQLSMALLINT) bufferSize, (SQLSMALLINT *) &bufferSize );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, CONN_ERROR );
                        result = 1;
                    }
                }
                if( !result )
                {
                    str_to_uc_cpy( pimpl.m_subtype, dbType );
                    ret = SQLGetInfo( m_hdbc, SQL_DRIVER_NAME, driverName, 1024, (SQLSMALLINT *) &bufferSize );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, CONN_ERROR );
                        result = 1;
                    }
                }
                if( !result )
                {
                    str_to_uc_cpy( odbc_pimpl->m_driverName, driverName );
                    bufferSize = 1024;
                    if( pimpl.m_subtype != L"Oracle" )
                    {
                        ret = SQLGetInfo( m_hdbc, SQL_DATABASE_NAME, dbName, (SQLSMALLINT) bufferSize, (SQLSMALLINT *) &bufferSize );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, CONN_ERROR );
                            result = 1;
                        }
                    }
                    else
                    {
                        std::wstring dbNameQuery = L"SELECT sys_context( 'userenv', 'current_schema' ) FROM dual";
                        SQLWCHAR *qry = new SQLWCHAR[dbNameQuery.length() + 2];
                        memset( qry, '\0', dbNameQuery.length() + 2 );
                        uc_to_str_cpy( qry, dbNameQuery );
                        ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, CONN_ERROR );
                            result = 1;
                        }
                        else
                        {
                            ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
                            delete[] qry;
                            qry = nullptr;
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errorMsg, 1, m_hstmt );
                                result = 1;
                            }
                            else
                            {
                                SQLULEN columnSizePtr = 0;
                                SQLLEN cDatabaseName;
                                ret = SQLBindCol( m_hstmt, 1, SQL_C_WCHAR, dbName, columnSizePtr, &cDatabaseName );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage(errorMsg, 1, m_hstmt);
                                    result = 1;
                                }
                                else
                                {
                                    ret = SQLFetch( m_hstmt );
                                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                    {
                                        GetErrorMessage(errorMsg, 1, m_hstmt);
                                        result = 1;
                                    }
                                }
                            }
                            if( m_hstmt )
                            {
                                ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errorMsg, STMT_ERROR );
                                    result = 1;
                                }
                                m_hstmt = 0;
                            }
                        }
                    }
                }
                if( !result )
                {
                    str_to_uc_cpy( pimpl.m_dbName, dbName );
                    if( pimpl.m_subtype != L"Oracle" )
                    {
                        ret = SQLGetInfo( m_hdbc, SQL_USER_NAME, userName, (SQLSMALLINT) bufferSize, (SQLSMALLINT *) &bufferSize );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, CONN_ERROR );
                            result = 1;
                        }
                    }
                    else
                    {
                        std::wstring userQuery = L"SELECT user FROM dual";
                        SQLWCHAR *qry = new SQLWCHAR[userQuery.length() + 2];
                        memset( qry, '\0', userQuery.length() + 2 );
                        uc_to_str_cpy( qry, userQuery );
                        ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, CONN_ERROR );
                            delete[] qry;
                            qry = nullptr;
                            result = 1;
                        }
                        else
                        {
                            ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
                            delete[] qry;
                            qry = nullptr;
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errorMsg, STMT_ERROR );
                                result = 1;
                            }
                            else
                            {
                                SQLLEN cbUserName;
                                ret = SQLBindCol( m_hstmt, 1, SQL_C_WCHAR, userName, 1024, &cbUserName );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errorMsg, STMT_ERROR );
                                    result = 1;
                                }
                                else
                                {
                                    ret = SQLFetch( m_hstmt );
                                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                    {
                                        GetErrorMessage( errorMsg, STMT_ERROR );
                                        result = 1;
                                    }
                                }
                            }
                            if( m_hstmt )
                            {
                                ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errorMsg, STMT_ERROR );
                                    result = 1;
                                }
                                m_hstmt = 0;
                            }
                        }
                    }
                }
                if( !result )
                    str_to_uc_cpy( pimpl.m_connectedUser, userName );
                if( pimpl.m_subtype == L"ACCESS" )
                {
                    pimpl.m_dbName = pimpl.m_dbName.substr( pimpl.m_dbName.find_last_of( L'\\' ) + 1 );
                    pimpl.m_dbName = pimpl.m_dbName.substr( 0, pimpl.m_dbName.find( L'.' ) );
                }
                if( !pimpl.m_dbName.empty() )
                    connectToDatabase = true;
                if( GetServerVersion( errorMsg ) )
                {
                    result = 1;
                }
                if( !result )
                {
                    SQLRETURN retcode = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
                    if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, CONN_ERROR );
                        result = 1;
                    }
                    if( !result )
                    {
                        result = MonitorSchemaChanges( errorMsg );
                    }
                    if( !result )
                        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
                    else
                        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                        result = 1;
                    }
                    m_hstmt = 0;
                }
/*****************************************/
                if( !result )
                {
                    ret = SQLSetConnectAttr( m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) FALSE, 0 );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, CONN_ERROR );
                        result = 1;
                    }
                }
                if( !result )
                {
                    if( !connectToDatabase )
                    {
                        if( pimpl.m_subtype != L"Oracle" || pimpl.m_versionMajor > 11 )
                        {
                            if( ServerConnect( dbList, errorMsg ) )
                            {
                                result = 1;
                            }
                        }
                    }
                }
                if( !result )
                {
                    if( CreateSystemObjectsAndGetDatabaseInfo( errorMsg ) )
                    {
                        result = 1;
                    }
                }
                if( !result && PopulateValdators( errorMsg ) )
                    result = 1;
            }
        }
    }
    if( result )
    {
        for( std::vector<SQLWCHAR *>::iterator it = errorMessage.begin(); it != errorMessage.end(); it++ )
        {
            std::wstring strMsg;
            str_to_uc_cpy( strMsg, (*it) );
            errorMsg.push_back( strMsg );
        }
    }
    else
        m_isConnected = true;
    delete[] query;
    query = nullptr;
    return result;
}

int ODBCDatabase::CreateSystemObjectsAndGetDatabaseInfo(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::vector<std::wstring> queries;
    if( pimpl.m_subtype == L"Microsoft SQL Server" ) // MS SQL SERVER
    {
        queries.push_back( L"BEGIN TRANSACTION" );
        queries.push_back( L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatcol' AND xtype='U') CREATE TABLE \"abcatcol\"(abc_tnam nvarchar(129) NOT NULL, abc_tid integer, abc_ownr nvarchar(129) NOT NULL, abc_cnam nvarchar(129) NOT NULL, abc_cid smallint, abc_labl varchar(254), abc_lpos smallint, abc_hdr varchar(254), abc_hpos smallint, abc_itfy smallint, abc_mask varchar(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn varchar(31), abc_bmap char(1), abc_init varchar(254), abc_cmnt nvarchar(254), abc_edit varchar(31), abc_tag varchar(254) );" );
        queries.push_back( L"IF NOT EXISTS (SELECT * FROM sys.indexes WHERE name = \'abcatc_x\' AND object_id = OBJECT_ID( \'abcatcol') ) CREATE UNIQUE INDEX abcatc_x ON abcatcol( abc_tnam ASC, abc_ownr ASC, abc_cnam ASC ) WITH (IGNORE_DUP_KEY = ON);" );
        queries.push_back( L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatedt' AND xtype='U') CREATE TABLE \"abcatedt\"(abe_name varchar(30) NOT NULL, abe_edit varchar(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work varchar(32) );" );
        queries.push_back( L"IF NOT EXISTS (SELECT * FROM sys.indexes WHERE name = \'abcate_x\' AND object_id = OBJECT_ID( \'abcatedt') ) CREATE UNIQUE INDEX abcate_x ON abcatedt( abe_name ASC, abe_seqn ASC ) WITH (IGNORE_DUP_KEY = ON);" );
        queries.push_back( L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatfmt' AND xtype='U') CREATE TABLE \"abcatfmt\"(abf_name varchar(30) NOT NULL, abf_frmt varchar(254), abf_type smallint, abf_cntr integer );" );
        queries.push_back( L"IF NOT EXISTS (SELECT * FROM sys.indexes WHERE name = \'abcatf_x\' AND object_id = OBJECT_ID( \'abcatfmt\') ) CREATE UNIQUE INDEX abcatf_x ON abcatfmt( abf_name ASC ) WITH (IGNORE_DUP_KEY = ON);" );
        queries.push_back( L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcattbl' AND xtype='U') CREATE TABLE \"abcattbl\"(abt_os tinyint, abt_tnam nchar(129) NOT NULL, abt_tid integer, abt_ownr nchar(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl integer, abd_fstr integer, abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl integer, abh_fstr integer, abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl integer, abl_fstr integer, abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt nchar(254) );" );
        queries.push_back( L"IF NOT EXISTS (SELECT * FROM sys.indexes WHERE name = \'abcatt_x\' AND object_id = OBJECT_ID( \'abcattbl\' )) CREATE UNIQUE INDEX abcatt_x ON abcattbl( abt_os ASC, abt_tnam ASC, abt_ownr ASC ) WITH (IGNORE_DUP_KEY = ON);");
        queries.push_back( L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatvld' AND xtype='U') CREATE TABLE \"abcatvld\"(abv_name nvarchar(30) NOT NULL, abv_vald nvarchar(254), abv_type smallint, abv_cntr integer, abv_msg nvarchar(254) );" );
        queries.push_back( L"IF NOT EXISTS (SELECT * FROM sys.indexes WHERE name = \'abcatv_x\' AND object_id = OBJECT_ID( \'abcatvld\' ) ) CREATE UNIQUE INDEX abcatv_x ON abcatvld( abv_name ASC ) WITH (IGNORE_DUP_KEY = ON);" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'(General)\') INSERT INTO \"abcatfmt\" VALUES( \'(General)\', \'(General)\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'0\') INSERT INTO \"abcatfmt\" VALUES( \'0\', \'0\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'0.00\') INSERT INTO \"abcatfmt\" VALUES( \'0.00\', \'0.00\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'#.##0\') INSERT INTO \"abcatfmt\" VALUES( \'#.##0\', \'#.##0\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'#.##0,00\') INSERT INTO \"abcatfmt\" VALUES( \'#.##0,00\', \'#.##0,00\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'$#.##0;[$#.##0]\') INSERT INTO \"abcatfmt\" VALUES( \'$#.##0;[$#.##0]\', \'$#.##0;[$#.##0]\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'$#.##0;[RED][$#.##0]\') INSERT INTO \"abcatfmt\" VALUES( \'$#.##0;[RED][$#.##0]\', \'$#.##0;|RED|[$#.##0]\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'$#.##0,00;[$#.##0,00]\') INSERT INTO \"abcatfmt\" VALUES( \'$#.##0,00;[$#.##0,00]\', \'$#.##0,00;[$#.##0,00]\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'$#.##0,00;[RED][$#.##0,00]\') INSERT INTO \"abcatfmt\" VALUES( \'$#.##0,00;[RED][$#.##0,00]\', \'$#.##0,00;|RED|[$#.##0,00]\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'0%\') INSERT INTO \"abcatfmt\" VALUES( \'0%\', \'0%\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'0.00%\') INSERT INTO \"abcatfmt\" VALUES( \'0.00%\', \'0.00%\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'0.00E+00\') INSERT INTO \"abcatfmt\" VALUES( \'0.00E+00\', \'0.00E+00\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'m/d/yy\') INSERT INTO \"abcatfmt\" VALUES( \'m/d/yy\', \'m/d/yy\', 84, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'d-mmm-yy\') INSERT INTO \"abcatfmt\" VALUES( \'d-mmm-yy\', \'d-mmm-yy\', 84, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'d-mmm\') INSERT INTO \"abcatfmt\" VALUES( \'d-mmm\', \'d-mmm\', 84, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'mmm-yy\') INSERT INTO \"abcatfmt\" VALUES( \'mmm-yy\', \'mmm-yy\', 84, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'h:mm AM/PM\') INSERT INTO \"abcatfmt\" VALUES( \'h:mm AM/PM\', \'h:mm AM/PM\', 84, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'h:mm:ss AM/PM\') INSERT INTO \"abcatfmt\" VALUES( \'h:mm:ss AM/PM\', \'h:mm:ss AM/PM\', 84, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'h:mm:ss\') INSERT INTO \"abcatfmt\" VALUES( \'h:mm:ss\', \'h:mm:ss\', 84, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'Phone_format\') INSERT INTO \"abcatfmt\" VALUES( \'Phone_format\', \'(@@@)) @@@-@@@@\', 80, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'m-d-yy\') INSERT INTO \"abcatfmt\" VALUES( \'m-d-yy\', \'m-d-yy\', 84, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'soc_sec_number\') INSERT INTO \"abcatfmt\" VALUES( \'soc_sec_number\', \'@@@-@@-@@@@\', 80, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'mm/dd/yyyy\') INSERT INTO \"abcatfmt\" VALUES( \'mm/dd/yyyy\', \'mm/dd/yyyy\', 82, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'salary\') INSERT INTO \"abcatfmt\" VALUES( \'salary\', \'$###,##0.00\', 81, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatfmt\" WHERE abf_name=\'mm-dd-yyyy\') INSERT INTO \"abcatfmt\" VALUES( \'mm-dd-yyyy\', \'mm-dd-yyyy\', 82, 0 );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatvld\" WHERE abv_name=\'Multiple_of_100\') INSERT INTO \"abcatvld\" VALUES( \'Multiple_of_100\', \'CHECK( mod( @column, 100 ) = 0 )\', 81, 3, \'The department number must be \');" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatvld\" WHERE abv_name=\'Positive_number\') INSERT INTO \"abcatvld\" VALUES( \'Positive_number\', \'CHECK( @column > 0 )\', 81, 6, \'Sorry! The value must be greater than 0\');");
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatvld\" WHERE abv_name=\'Y_or_N\') INSERT INTO \"abcatvld\" VALUES( \'Y_or_N\', \'CHECK( @column IN ( \"Y\", \"y\", \"N\", \"n\" )\', 81, 6, \'\');");
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatvld\" WHERE abv_name=\'must_be_number\') INSERT INTO \"abcatvld\" VALUES( \'must_be_number\', \'CHECK( isNumer( @column )\', 80, 0, \'\');");
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatvld\" WHERE abv_name=\'valid status\') INSERT INTO \"abcatvld\" VALUES( \'valid status\', \'CHECK( @status == \"ALT\" )\', 80, 3, \'\');");
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'###-##-####\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'###-##-####\', \'###-##-####\', 90, 1, 1, 32, \'00\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'###,###.00\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'###,###.00\', \'###,###.00\', 90, 1, 1, 32, \'10\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'#####\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'#####\', \'#####\', 90, 1, 1, 32, \'10\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'BenefitsCheckBox\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', NULL, 85, 4, 1, 536870916, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'BenefitsCheckBox\' AND abe_seqn=2) INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'Y\', 85, 4, 2, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'BenefitsCheckBox\' AND abe_seqn=3) INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'N\', 85, 4, 3, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'0\', 87, 3, 1, -201326590, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=2) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 2, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=3) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 3, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=4) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 4, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=5) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 5, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=6) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 6, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=7) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 7, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=8) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 8, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=9) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 9, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=10) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 10, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=11) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 11, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=12) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 12, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=13) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 13, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=14) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yellow\', 87, 3, 14, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=15) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yrllow\', 87, 3, 15, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=16) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 16, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Color List\' AND abe_seqn=17) INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 17, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Customers\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'d_dddw_cust\', 88, 2, 1, 536870928, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Customers\' AND abe_seqn=2) INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 2, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Customers\' AND abe_seqn=3) INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 3, 0, \'400\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Day Care\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Day Care\', 85, 4, 1, 536870916, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Day Care\' AND abe_seqn=2) INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Y\', 85, 4, 2, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Day Care\' AND abe_seqn=3) INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'N\', 85, 4, 3, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'DD/MM/YY\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY\', \'DD/MM/YY\', 90, 1, 1, 32, \'20\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'DD/MM/YY HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS\', \'DD/MM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'DD/MM/YY HH:MM:SS:FFFFFF\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS:FFFFFF\', \'DD/MM/YY HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'40\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'DD/MM/YYYY\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY\', \'DD/MM/YYYY\', 90, 1, 1, 32, \'20\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'DD/MM/YYYY HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY HH:MM:SS\', \'DD/MM/YY HH::MM:SS\', 90, 1, 1, 32, \'40\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'DD/MMM/YY\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY\', \'DD/MMM/YY\', 90, 1, 1, 32, \'20\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'DD/MMM/YY HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY HH:MM:SS\', \'DD/MMM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'DDD/YY\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'DDD/YY\', \'DDD/YY\', 90, 1, 1, 32, \'20\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'DDD/YY HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'DDD/YY HH:MM:SS\', \'DDD/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'DDD/YYYY\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY\', \'DDD/YYYY\', 90, 1, 1, 32, \'20\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'DDD/YYYY HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY HH:MM:SS\', \'DDD/YYYY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Department List\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'d_dddw_dept\', 88, 10, 1, 536870928, \'0\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Department List\' AND abe_seqn=2) INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_id\', 88, 10, 2, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Department List\' AND abe_seqn=3) INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_d\', 88, 10, 3, 0, \'300\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Dollars with cents\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'Dollars with cents\', \'$###,###,###.00\', 90, 2, 1, 32, \'00\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Employee Status\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'1\', 86, 3, 1, 1073741832, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Employee Status\' AND abe_seqn=2) INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Active\', 86, 3, 2, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Employee Status\' AND abe_seqn=3) INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'A\', 86, 3, 3, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Employee Status\' AND abe_seqn=4) INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Terminated\', 86, 3, 4, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Employee Status\' AND abe_seqn=5) INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'T\', 86, 3, 5, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Employee Status\' AND abe_seqn=6) INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'On Leave\', 86, 3, 6, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Employee Status\' AND abe_seqn=7) INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'L\', 86, 3, 7, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'employees\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'employees\', \'d_dddw_sales_reps\', 88, 3, 1, 536870928, \'0\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'employees\' AND abe_seqn=2) INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 2, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'employees\' AND abe_seqn=3) INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 3, 0, \'400\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Financial Codes\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'d_dddw_fin_code\', 88, 3, 1, 536870928, \'0\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Financial Codes\' AND abe_seqn=2) INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 2, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Financial Codes\' AND abe_seqn=3) INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 3, 0, \'700\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Health Insurance\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'health insurance\', 85, 3, 1, 536870928, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Health Insurance\' AND abe_seqn=2) INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'Y\', 85, 3, 2, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'Health Insurance\' AND abe_seqn=3) INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'N\', 85, 3, 3, 0, NULL );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS:FFF\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFF\', \'HH:MM:SS:FFF\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS:FFFFFF\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFFFFF\', \'HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'JJJ/YY\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY\', \'JJJ/YY\', 90, 1, 1, 32, \'20\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'JJJ/YY HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY HH:MM:SS\', \'JJJ/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'JJJ/YYYY\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'JJJ/YYYY\', \'JJJ/YYYY\', 90, 1, 1, 32, \'40\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        queries.push_back( L"IF NOT EXISTS( SELECT * FROM \"abcatedt\" WHERE abe_name=\'HH:MM:SS\' AND abe_seqn=1) INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    }
    if( pimpl.m_subtype == L"ACCESS" )
    {
        queries.push_back( L"SELECT name FROM MSysObjects WHERE type IN (1, 4, 6) AND name = 'abcatcol';" );
        queries.push_back( L"CREATE TABLE abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));" );
        queries.push_back( L"SELECT name FROM MSysObjects WHERE type IN (1, 4, 6) AND name = 'abcatedt';" );
        queries.push_back( L"CREATE TABLE abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));" );
    }
    if( pimpl.m_subtype == L"MySQL" || pimpl.m_subtype == L"PostgreSQL" )
    {
        queries.push_back( L"BEGN" );
        queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));" );
        queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));" );
        queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));" );
        queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));" );
        if( pimpl.m_subtype == L"MySQL" )
        {
            queries.push_back( L"CREATE TABLE IF NOT EXISTS abcattbl(abt_os tinyint, abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl integer, abd_fstr integer, abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl integer, abh_fstr integer, abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl integer, abl_fstr integer, abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ));" );
            queries.push_back( L"SELECT( IF( ( SELECT 1 FROM information_schema.statistics WHERE index_name=\'abcatc_x\' AND table_name=\'abcatcol\' ) > 0, \"SELECT 0\", \"CREATE UNIQUE INDEX abcatc_x ON abcatcol(abc_tnam ASC, abc_ownr ASC, abc_cnam ASC)\"));" );
            queries.push_back( L"SELECT( IF( ( SELECT 1 FROM information_schema.statistics WHERE index_name=\'abcate_x\' AND table_name=\'abcatedt\' ) > 0, \"SELECT 0\", \"CREATE UNIQUE INDEX abcate_x ON abcatedt(abe_name ASC, abe_seqn ASC)\"));" );
            queries.push_back( L"SELECT( IF( ( SELECT 1 FROM information_schema.statistics WHERE index_name=\'abcatf_x\' AND table_name=\'abcatfmt\' ) > 0, \"SELECT 0\", \"CREATE UNIQUE INDEX abcatf_x ON abcatfmt(abf_name ASC)\"));" );
            queries.push_back( L"SELECT( IF( ( SELECT 1 FROM information_schema.statistics WHERE index_name=\'abcatt_x\' AND table_name=\'abcattbl\' ) > 0, \"SELECT 0\", \"CREATE UNIQUE INDEX abcatt_x ON abcattbl(abt_os ASC, abt_tnam ASC, abt_ownr ASC)\"));" );
            queries.push_back( L"SELECT( IF( ( SELECT 1 FROM information_schema.statistics WHERE index_name=\'abcatv_x\' AND table_name=\'abcatvld\' ) > 0, \"SELECT 0\", \"CREATE UNIQUE INDEX abcatv_x ON abcatvld(abv_name ASC)\"));" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'(General)\', \'(General)\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'0\', \'0\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'0.00\', \'0.00\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'#.##0\', \'#.##0\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'#.##0,00\', \'#.##0,00\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'$#.##0;[$#.##0]\', \'$#.##0;[$#.##0]\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'$#.##0;|RED|[$#.##0]\', \'$#.##0;|RED|[$#.##0]\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'$#.##0,00;[$#.##0,00]\', \'$#.##0,00;[$#.##0,00]\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'$#.##0,00;|RED|[$#.##0,00]\', \'$#.##0,00;|RED|[$#.##0,00]\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'0%\', \'0%\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'0.00%\', \'0.00%\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'0.00E+00\', \'0.00E+00\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'m/d/yy\', \'m/d/yy\', 84, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'d-mmm-yy\', \'d-mmm-yy\', 84, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'d-mmm\', \'d-mmm\', 84, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'mmm-yy\', \'mmm-yy\', 84, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'h:mm AM/PM\', \'h:mm AM/PM\', 84, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'h:mm:ss AM/PM\', \'h:mm:ss AM/PM\', 84, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'h:mm:ss\', \'h:mm:ss\', 84, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'Phone_format\', \'(@@@)) @@@-@@@@\', 80, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'m-d-yy\', \'m-d-yy\', 84, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'soc_sec_number\', \'@@@-@@-@@@@\', 80, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'mm/dd/yyyy\', \'mm/dd/yyyy\', 82, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'salary\', \'$###,##0.00\', 81, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatfmt\" VALUES( \'mm-dd-yyyy\', \'mm-dd-yyyy\', 82, 0 );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatvld\" VALUES( \'Multiple_of_100\', \'CHECK( mod( @column, 100 ) = 0 )\', 81, 3, \'The department number must be \');" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatvld\" VALUES( \'Positive_number\', \'CHECK( @column > 0 )\', 81, 6, \'Sorry! The value must be greater than 0\');");
            queries.push_back( L"INSERT IGNORE INTO \"abcatvld\" VALUES( \'Y_or_N\', \'CHECK( @column IN ( \"Y\", \"y\", \"N\", \"n\" )\', 81, 6, \'\');");
            queries.push_back( L"INSERT IGNORE INTO \"abcatvld\" VALUES( \'must_be_numer\', \'CHECK( isNumer( @column )\', 80, 0, \'\');");
            queries.push_back( L"INSERT IGNORE INTO \"abcatvld\" VALUES( \'valid status\', \'CHECK( @status == \"ALT\" )\', 80, 3, \'\');");
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'###-##-####\', \'###-##-####\', 90, 1, 1, 32, \'00\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'###,###.00\', \'###,###.00\', 90, 1, 1, 32, \'10\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'#####\', \'#####\', 90, 1, 1, 32, \'10\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', NULL, 85, 4, 1, 536870916, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'Y\', 85, 4, 2, 0, NULL );" );
            queries.push_back( L"INSERT IGNRE INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'N\', 85, 4, 3, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'0\', 87, 3, 1, -201326590, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 2, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 3, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 4, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 5, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 6, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 7, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 8, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 9, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 10, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 11, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 12, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 13, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Yellow\', 87, 3, 14, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Yrllow\', 87, 3, 15, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 16, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 17, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Customers\', \'d_dddw_cust\', 88, 2, 1, 536870928, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 2, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 3, 0, \'400\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Day Care\', \'Day Care\', 85, 4, 1, 536870916, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Day Care\', \'Y\', 85, 4, 2, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Day Care\', \'N\', 85, 4, 3, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'DD/MM/YY\', \'DD/MM/YY\', 90, 1, 1, 32, \'20\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS\', \'DD/MM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS:FFFFFF\', \'DD/MM/YY HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'40\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'DD/MM/YYYY\', \'DD/MM/YYYY\', 90, 1, 1, 32, \'20\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'DD/MM/YYYY HH:MM:SS\', \'DD/MM/YY HH::MM:SS\', 90, 1, 1, 32, \'40\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'DD/MMM/YY\', \'DD/MMM/YY\', 90, 1, 1, 32, \'20\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'DD/MMM/YY HH:MM:SS\', \'DD/MMM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'DDD/YY\', \'DDD/YY\', 90, 1, 1, 32, \'20\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'DDD/YY HH:MM:SS\', \'DDD/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'DDD/YYYY\', \'DDD/YYYY\', 90, 1, 1, 32, \'20\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'DDD/YYYY HH:MM:SS\', \'DDD/YYYY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Department List\', \'d_dddw_dept\', 88, 10, 1, 536870928, \'0\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Department List\', \'dept_id\', 88, 10, 2, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Department List\', \'dept_d\', 88, 10, 3, 0, \'300\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Dollars with cents\', \'$###,###,###.00\', 90, 2, 1, 32, \'00\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Employee Status\', \'1\', 86, 3, 1, 1073741832, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Employee Status\', \'Active\', 86, 3, 2, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Employee Status\', \'A\', 86, 3, 3, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Employee Status\', \'Terminated\', 86, 3, 4, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Employee Status\', \'T\', 86, 3, 5, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Employee Status\', \'On Leave\', 86, 3, 6, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Employee Status\', \'L\', 86, 3, 7, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'employees\', \'d_dddw_sales_reps\', 88, 3, 1, 536870928, \'0\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 2, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 3, 0, \'400\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Financial Codes\', \'d_dddw_fin_code\', 88, 3, 1, 536870928, \'0\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 2, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 3, 0, \'700\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Health Insurance\', \'health insurance\', 85, 3, 1, 536870928, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Health Insurance\', \'Y\', 85, 3, 2, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'Health Insurance\', \'N\', 85, 3, 3, 0, NULL );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFF\', \'HH:MM:SS:FFF\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFFFFF\', \'HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'JJJ/YY\', \'JJJ/YY\', 90, 1, 1, 32, \'20\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'JJJ/YY HH:MM:SS\', \'JJJ/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'JJJ/YYYY\', \'JJJ/YYYY\', 90, 1, 1, 32, \'40\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
            queries.push_back( L"INSERT IGNORE INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
        }
        else
        {
            queries.push_back( L"CREATE TABLE IF NOT EXISTS abcattbl(abt_os smallint, abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl integer, abd_fstr integer, abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl integer, abh_fstr integer, abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl integer, abl_fstr integer, abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ));" );
            if( pimpl.m_versionMajor >= 9 && pimpl.m_versionMinor >= 5 )
            {
                queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcatc_x ON abcatcol( abc_tnam, abc_ownr, abc_cnam );" );
                queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcate_x ON abcatedt( abe_name, abe_seqn );" );
                queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcatf_x ON abcatfmt( abf_name );" );
                queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcatt_x ON abcattbl( abt_os, abt_name, abt_ownr );" );
                queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcatv_x ON abcatvld( abv_name );" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'(General)\', \'(General)\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'0\', \'0\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'0.00\', \'0.00\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'#.##0\', \'#.##0\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'#.##0,00\', \'#.##0,00\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'$#.##0;[$#.##0]\', \'$#.##0;[$#.##0]\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'$#.##0;|RED|[$#.##0]\', \'$#.##0;|RED|[$#.##0]\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'$#.##0,00;[$#.##0,00]\', \'$#.##0,00;[$#.##0,00]\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'$#.##0,00;|RED|[$#.##0,00]\', \'$#.##0,00;|RED|[$#.##0,00]\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'0%\', \'0%\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'0.00%\', \'0.00%\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'0.00E+00\', \'0.00E+00\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'m/d/yy\', \'m/d/yy\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'d-mmm-yy\', \'d-mmm-yy\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'d-mmm\', \'d-mmm\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'mmm-yy\', \'mmm-yy\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'h:mm AM/PM\', \'h:mm AM/PM\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'h:mm:ss AM/PM\', \'h:mm:ss AM/PM\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'h:mm:ss\', \'h:mm:ss\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'Phone_format\', \'(@@@)) @@@-@@@@\', 80, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'m-d-yy\', \'m-d-yy\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'soc_sec_number\', \'@@@-@@-@@@@\', 80, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'mm/dd/yyyy\', \'mm/dd/yyyy\', 82, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'salary\', \'$###,##0.00\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'mm-dd-yyyy\', \'mm-dd-yyyy\', 82, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatvld\" VALUES( \'Multiple_of_100\', \'CHECK( mod( @column, 100 ) = 0 )\', 81, 3, \'The department number must be \') ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatvld\" VALUES( \'Positive_number\', \'CHECK( @column > 0 )\', 81, 6, \'Sorry! The value must be greater than 0\') ON CONFLICT DO NOTHING;");
                queries.push_back( L"INSERT INTO \"abcatvld\" VALUES( \'Y_or_N\', \'CHECK( @column IN ( \"Y\", \"y\", \"N\", \"n\" )\', 81, 6, \'\') ON CONFLICT DO NOTHING;");
                queries.push_back( L"INSERT INTO \"abcatvld\" VALUES( \'must_be_numer\', \'CHECK( isNumer( @column )\', 80, 0, \'\') ON CONFLICT DO NOTHING;");
                queries.push_back( L"INSERT INTO \"abcatvld\" VALUES( \'valid status\', \'CHECK( @status == \"ALT\" )\', 80, 3, \'\') ON CONFLICT DO NOTHING;");
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###-##-####\', \'###-##-####\', 90, 1, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###,###.00\', \'###,###.00\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'#####\', \'#####\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', NULL, 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'0\', 87, 3, 1, -201326590, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 8, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 9, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 10, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 11, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 12, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 13, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yellow\', 87, 3, 14, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yrllow\', 87, 3, 15, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 16, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 17, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'d_dddw_cust\', 88, 2, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Day Care\', 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY\', \'DD/MM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS\', \'DD/MM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS:FFFFFF\', \'DD/MM/YY HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY\', \'DD/MM/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY HH:MM:SS\', \'DD/MM/YY HH::MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY\', \'DD/MMM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY HH:MM:SS\', \'DD/MMM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY\', \'DDD/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY HH:MM:SS\', \'DDD/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY\', \'DDD/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY HH:MM:SS\', \'DDD/YYYY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'d_dddw_dept\', 88, 10, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_id\', 88, 10, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_d\', 88, 10, 3, 0, \'300\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Dollars with cents\', \'$###,###,###.00\', 90, 2, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'1\', 86, 3, 1, 1073741832, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Active\', 86, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'A\', 86, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Terminated\', 86, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'T\', 86, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'On Leave\', 86, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'L\', 86, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'d_dddw_sales_reps\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'d_dddw_fin_code\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 3, 0, \'700\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'health insurance\', 85, 3, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'Y\', 85, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'N\', 85, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFF\', \'HH:MM:SS:FFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFFFFF\', \'HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY\', \'JJJ/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY HH:MM:SS\', \'JJJ/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YYYY\', \'JJJ/YYYY\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            }
            else
            {
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcatc_x\' AND n.nspname = \'public\' ) THEN CREATE UNIQUE INDEX \"abcatc_x\" ON \"abcatcol\"(\"abc_tnam\" ASC, \"abc_ownr\" ASC, \"abc_cnam\" ASC); END IF; END;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcate_x\' AND n.nspname = \'public\' ) THEN CREATE UNIQUE INDEX \"abcate_x\" ON \"abcatedt\"(\"abe_name\" ASC, \"abe_seqn\" ASC); END IF; END;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcatf_x\' AND n.nspname = \'public\' ) THEN CREATE UNIQUE INDEX \"abcatf_x\" ON \"abcatfmt\"(\"abf_tnam\" ASC); END IF; END;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcatt_x\' AND n.nspname = \'public\' ) THEN CREATE UNIQUE INDEX \"abcatt_x\" ON \"abcattbl\"(\"abt_os\" ASC, \"abt_tnam\" ASC, \"abt_ownr\" ASC); END IF; END;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcatv_x\' AND n.nspname = \'public\' ) THEN CREATE UNIQUE INDEX \"abcatv_x\" ON \"abcatvld\"(\"abv_name\" ASC); END IF; END;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'(General)\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'(General)\', \'(General)\', 81, 0 ); END IF; END;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'0\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'0\', \'0\', 81, 0 ); END IF; END;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'0.00\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'0.00\', \'0.00\', 81, 0 ); END IF; END;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'#.##0\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'#.##0\', \'#.##0\', 81, 0 ); END IF; END;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'#.##0,00\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'#.##0,00\', \'#.##0,00\', 81, 0 ); END IF; END;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'$#.##0;[$#.##0]\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'$#.##0;[$#.##0]\', \'$#.##0;[$#.##0]\', 81, 0 ); END IF; END;;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'$#.##0;|RED|[$#.##0]\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'$#.##0;|RED|[$#.##0]\', \'$#.##0;|RED|[$#.##0]\', 81, 0 ); END IF; END;" );
                queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'$#.##0,00;[$#.##0,00]\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'$#.##0,00;[$#.##0,00]\', \'$#.##0,00;[$#.##0,00]\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'$#.##0,00;|RED|[$#.##0,00]\', \'$#.##0,00;|RED|[$#.##0,00]\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'0%\', \'0%\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'0.00%\', \'0.00%\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'0.00E+00\', \'0.00E+00\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'m/d/yy\', \'m/d/yy\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'d-mmm-yy\', \'d-mmm-yy\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'d-mmm\', \'d-mmm\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'mmm-yy\', \'mmm-yy\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'h:mm AM/PM\', \'h:mm AM/PM\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'h:mm:ss AM/PM\', \'h:mm:ss AM/PM\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'h:mm:ss\', \'h:mm:ss\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'Phone_format\', \'(@@@)) @@@-@@@@\', 80, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'m-d-yy\', \'m-d-yy\', 84, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'soc_sec_number\', \'@@@-@@-@@@@\', 80, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'mm/dd/yyyy\', \'mm/dd/yyyy\', 82, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'salary\', \'$###,##0.00\', 81, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatfmt\" VALUES( \'mm-dd-yyyy\', \'mm-dd-yyyy\', 82, 0 ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatvld\" VALUES( \'Multiple_of_100\', \'CHECK( mod( @column, 100 ) = 0 )\', 81, 3, \'The department number must be \') ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatvld\" VALUES( \'Positive_number\', \'CHECK( @column > 0 )\', 81, 6, \'Sorry! The value must be greater than 0\') ON CONFLICT DO NOTHING;");
                queries.push_back( L"INSERT INTO \"abcatvld\" VALUES( \'Y_or_N\', \'CHECK( @column IN ( \"Y\", \"y\", \"N\", \"n\" )\', 81, 6, \'\') ON CONFLICT DO NOTHING;");
                queries.push_back( L"INSERT INTO \"abcatvld\" VALUES( \'must_be_numer\', \'CHECK( isNumer( @column )\', 80, 0, \'\') ON CONFLICT DO NOTHING;");
                queries.push_back( L"INSERT INTO \"abcatvld\" VALUES( \'valid status\', \'CHECK( @status == \"ALT\" )\', 80, 3, \'\') ON CONFLICT DO NOTHING;");
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###-##-####\', \'###-##-####\', 90, 1, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###,###.00\', \'###,###.00\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'#####\', \'#####\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', NULL, 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'0\', 87, 3, 1, -201326590, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 8, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 9, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 10, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 11, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 12, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 13, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yellow\', 87, 3, 14, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yrllow\', 87, 3, 15, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 16, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 17, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'d_dddw_cust\', 88, 2, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Day Care\', 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY\', \'DD/MM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS\', \'DD/MM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS:FFFFFF\', \'DD/MM/YY HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY\', \'DD/MM/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY HH:MM:SS\', \'DD/MM/YY HH::MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY\', \'DD/MMM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY HH:MM:SS\', \'DD/MMM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY\', \'DDD/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY HH:MM:SS\', \'DDD/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY\', \'DDD/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY HH:MM:SS\', \'DDD/YYYY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'d_dddw_dept\', 88, 10, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_id\', 88, 10, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_d\', 88, 10, 3, 0, \'300\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Dollars with cents\', \'$###,###,###.00\', 90, 2, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'1\', 86, 3, 1, 1073741832, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Active\', 86, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'A\', 86, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Terminated\', 86, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'T\', 86, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'On Leave\', 86, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'L\', 86, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'d_dddw_sales_reps\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'d_dddw_fin_code\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 3, 0, \'700\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'health insurance\', 85, 3, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'Y\', 85, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'N\', 85, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFF\', \'HH:MM:SS:FFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFFFFF\', \'HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY\', \'JJJ/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY HH:MM:SS\', \'JJJ/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YYYY\', \'JJJ/YYYY\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
                queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            }
        }
    }
    if( pimpl.m_subtype == L"Sybase" || pimpl.m_subtype == L"ASE" )
    {
        queries.push_back( L"BEGIN TRANSACTION" );
        queries.push_back( L"IF NOT EXISTS(SELECT 1 FROM sysobjects WHERE name = 'abcatcol' AND type = 'U') EXECUTE(\"CREATE TABLE abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ))\")" );
        queries.push_back( L"IF NOT EXISTS(SELECT 1 FROM sysobjects WHERE name = 'abcatedt' AND type = 'U') EXECUTE(\"CREATE TABLE abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ))\")" );
        queries.push_back( L"IF NOT EXISTS(SELECT 1 FROM sysobjects WHERE name = 'abcatfmt' AND type = 'U') EXECUTE(\"CREATE TABLE abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ))\")" );
        queries.push_back( L"IF NOT EXISTS(SELECT 1 FROM sysobjects WHERE name = 'abcattbl' AND type = 'U') EXECUTE(\"CREATE TABLE abcattbl(abt_os integer, abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ))\")" );
        queries.push_back( L"IF NOT EXISTS(SELECT 1 FROM sysobjects WHERE name = 'abcatvld' AND type = 'U') EXECUTE(\"CREATE TABLE abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ))\")" );
        queries.push_back( L"IF NOT EXISTS(SELECT o.name, i.name FROM sysobjects o, sysindexes i WHERE o.id = i.id AND o.name='abcattbl' AND i.name='abcattbl_tnam_ownr') EXECUTE(\"CREATE INDEX abcattbl_tnam_ownr ON abcattbl(abt_tnam ASC, abt_ownr ASC)\")" );
        queries.push_back( L"IF NOT EXISTS(SELECT o.name, i.name FROM sysobjects o, sysindexes i WHERE o.id = i.id AND o.name='abcatcol' AND i.name='abcatcol_tnam_ownr_cnam') EXECUTE(\"CREATE INDEX abcatcol_tnam_ownr_cnam ON abcatcol(abc_tnam ASC, abc_ownr ASC, abc_cnam ASC)\")" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###-##-####\', \'###-##-####\', 90, 1, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###,###.00\', \'###,###.00\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'#####\', \'#####\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', NULL, 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'0\', 87, 3, 1, -201326590, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 8, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 9, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 10, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 11, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 12, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 13, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yellow\', 87, 3, 14, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yrllow\', 87, 3, 15, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 16, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 17, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'d_dddw_cust\', 88, 2, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Day Care\', 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY\', \'DD/MM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS\', \'DD/MM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS:FFFFFF\', \'DD/MM/YY HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY\', \'DD/MM/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY HH:MM:SS\', \'DD/MM/YY HH::MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY\', \'DD/MMM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY HH:MM:SS\', \'DD/MMM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY\', \'DDD/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY HH:MM:SS\', \'DDD/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY\', \'DDD/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY HH:MM:SS\', \'DDD/YYYY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'d_dddw_dept\', 88, 10, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_id\', 88, 10, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_d\', 88, 10, 3, 0, \'300\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Dollars with cents\', \'$###,###,###.00\', 90, 2, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'1\', 86, 3, 1, 1073741832, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Active\', 86, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'A\', 86, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Terminated\', 86, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'T\', 86, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'On Leave\', 86, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'L\', 86, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'d_dddw_sales_reps\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'d_dddw_fin_code\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 3, 0, \'700\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'health insurance\', 85, 3, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'Y\', 85, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'N\', 85, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFF\', \'HH:MM:SS:FFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFFFFF\', \'HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY\', \'JJJ/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY HH:MM:SS\', \'JJJ/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YYYY\', \'JJJ/YYYY\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
    }
    if( pimpl.m_subtype == L"Sybase SQL Anywhere" )
    {
        queries.push_back( L"BEGIN TRANSACTION" );
        if( pimpl.m_versionMajor >= 12 )
        {
            // According to the Sybase (SAP) documentation "IF NOT EXIST" is supported from version 12 up.
            queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ))" );
            queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ))" );
            queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ))" );
            queries.push_back( L"CREATE TABLE IF NOT EXISTS abcattbl(abt_os integer, abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fstr integer, abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fstr integer, abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fstr integer, abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ))" );
            queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ))" );
            queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcatc_x ON abcatcol(abc_tnam ASC, abc_ownr ASC, abc_cnam ASC);" );
            queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcate_x ON abcatedt(abe_tnam ASC, abe_seqn ASC);" );
            queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcatf_x ON abcatfmt(abf_name ASC);" );
            queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcatt_x ON abcattbl(abt_os ASC, abt_tnam ASC, abt_ownr ASC);" );
            queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcatv_x ON abcatvld(abv_name ASC);" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###-##-####\', \'###-##-####\', 90, 1, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###,###.00\', \'###,###.00\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'#####\', \'#####\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', NULL, 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'0\', 87, 3, 1, -201326590, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 8, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 9, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 10, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 11, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 12, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 13, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yellow\', 87, 3, 14, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yrllow\', 87, 3, 15, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 16, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 17, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'d_dddw_cust\', 88, 2, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Day Care\', 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY\', \'DD/MM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS\', \'DD/MM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS:FFFFFF\', \'DD/MM/YY HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY\', \'DD/MM/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY HH:MM:SS\', \'DD/MM/YY HH::MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY\', \'DD/MMM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY HH:MM:SS\', \'DD/MMM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY\', \'DDD/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY HH:MM:SS\', \'DDD/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY\', \'DDD/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY HH:MM:SS\', \'DDD/YYYY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'d_dddw_dept\', 88, 10, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_id\', 88, 10, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_d\', 88, 10, 3, 0, \'300\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Dollars with cents\', \'$###,###,###.00\', 90, 2, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'1\', 86, 3, 1, 1073741832, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Active\', 86, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'A\', 86, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Terminated\', 86, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'T\', 86, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'On Leave\', 86, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'L\', 86, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'d_dddw_sales_reps\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'d_dddw_fin_code\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 3, 0, \'700\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'health insurance\', 85, 3, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'Y\', 85, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'N\', 85, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFF\', \'HH:MM:SS:FFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFFFFF\', \'HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY\', \'JJJ/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY HH:MM:SS\', \'JJJ/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YYYY\', \'JJJ/YYYY\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        }
        else
        {
            queries.push_back( L"SELECT 1 FROM dbo.sysobjects WHERE name = 'abcatcol'" );
            queries.push_back( L"CREATE TABLE abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ))" );
            queries.push_back( L"SELECT 1 FROM dbo.sysobjects WHERE name = 'abcatedt'" );
            queries.push_back( L"CREATE TABLE abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ))" );
            queries.push_back( L"SELECT 1 FROM dbo.sysobjects WHERE name = 'abcatfmt'" );
            queries.push_back( L"CREATE TABLE abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ))" );
            queries.push_back( L"SELECT 1 FROM dbo.sysobjects WHERE name = 'abcattbl'" );
            queries.push_back( L"CREATE TABLE abcattbl(abt_os, abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fstr integer, abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fstr integer, abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fstr integer, abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ))" );
            queries.push_back( L"SELECT 1 FROM dbo.sysobjects WHERE name = 'abcatvld'" );
            queries.push_back( L"CREATE TABLE abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ))" );
            queries.push_back( L"SELECT 1 FROM dbo.sysindexes WHERE name = \'pbcatc_x\' AND tname = \'abcatcol\';" );
            queries.push_back( L"CREATE UNIQUE INDEX abcatc_x ON abcatcol( abc_tnam ASC, abc_ownr ASC, abc_cnam ASC );" );
            queries.push_back( L"SELECT 1 FROM dbo.sysindexes WHERE name = \'pbcate_x\' AND tname = \'abcatedt\';" );
            queries.push_back( L"CREATE UNIQUE INDEX abcate_x ON abcatedt( abe_name ASC, abe_seqn ASC );" );
            queries.push_back( L"SELECT 1 FROM dbo.sysindexes WHERE name = \'pbcatf_x\' AND tname = \'abcatfmt\';" );
            queries.push_back( L"CREATE UNIQUE INDEX abcatf_x ON abcatfmt( abf_name ASC );" );
            queries.push_back( L"SELECT 1 FROM sysindexes WHERE tname = \'abcattbl\' AND iname = \'abcatt_x\';" );
            queries.push_back( L"CREATE UNIQUE INDEX abcatt_x ON abcattbl( abt_os ASC, abt_tnam ASC, abt_ownr ASC );" );
            queries.push_back( L"SELECT 1 FROM sysindexes WHERE tname = \'abcatvld\' AND iname = \'abcatv_x\';" );
            queries.push_back( L"CREATE UNIQUE INDEX abcatv_x ON abcatvld( abv_name ASC, abt_ownr ASC );" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###-##-####\', \'###-##-####\', 90, 1, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###,###.00\', \'###,###.00\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'#####\', \'#####\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', NULL, 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'0\', 87, 3, 1, -201326590, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 8, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 9, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 10, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 11, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 12, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 13, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yellow\', 87, 3, 14, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yrllow\', 87, 3, 15, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 16, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 17, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'d_dddw_cust\', 88, 2, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Day Care\', 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY\', \'DD/MM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS\', \'DD/MM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS:FFFFFF\', \'DD/MM/YY HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY\', \'DD/MM/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY HH:MM:SS\', \'DD/MM/YY HH::MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY\', \'DD/MMM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY HH:MM:SS\', \'DD/MMM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY\', \'DDD/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY HH:MM:SS\', \'DDD/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY\', \'DDD/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY HH:MM:SS\', \'DDD/YYYY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'d_dddw_dept\', 88, 10, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_id\', 88, 10, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_d\', 88, 10, 3, 0, \'300\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Dollars with cents\', \'$###,###,###.00\', 90, 2, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'1\', 86, 3, 1, 1073741832, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Active\', 86, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'A\', 86, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Terminated\', 86, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'T\', 86, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'On Leave\', 86, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'L\', 86, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'d_dddw_sales_reps\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'d_dddw_fin_code\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 3, 0, \'700\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'health insurance\', 85, 3, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'Y\', 85, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'N\', 85, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFF\', \'HH:MM:SS:FFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFFFFF\', \'HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY\', \'JJJ/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY HH:MM:SS\', \'JJJ/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YYYY\', \'JJJ/YYYY\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
            queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        }
    }
    if( pimpl.m_subtype == L"Oracle" )
    {

        queries.push_back( L"SELECT 1 FROM all_objects WHERE object_name = UPPER( 'abcatcol' )" );
        queries.push_back( L"CREATE TABLE abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));" );
        queries.push_back( L"SELECT 1 FROM all_objects WHERE object_name = UPPER( 'abcatedt' )" );
        queries.push_back( L"CREATE TABLE abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));" );
        queries.push_back( L"SELECT 1 FROM all_objects WHERE object_name = UPPER( 'abcatfmt' )" );
        queries.push_back( L"CREATE TABLE abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));" );
        queries.push_back( L"SELECT 1 FROM all_objects WHERE object_name = UPPER( 'abcattbl' )" );
        queries.push_back( L"CREATE TABLE abcattbl(abt_os integer, abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl integer, abd_fstr integer, abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl integer, abh_fstr integer, abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl integer, abl_fstr integer, abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ));" );
        queries.push_back( L"SELECT 1 FROM all_objects WHERE object_name = UPPER( 'abcatvld' )" );
        queries.push_back( L"CREATE TABLE abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));" );
        queries.push_back( L"SELECT 1 FROM all_indexes WHERE table_name = UPPER( 'abcattbl' ) AND index_name = UPPER( 'abcattbl_tnam_ownr' );" );
        queries.push_back( L"CREATE INDEX abcattbl_tnam_ownr ON abcattbl(abt_tnam, abt_ownr);" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###-##-####\', \'###-##-####\', 90, 1, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'###,###.00\', \'###,###.00\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'#####\', \'#####\', 90, 1, 1, 32, \'10\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', NULL, 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'BenefitsCheckBox\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'0\', 87, 3, 1, -201326590, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 8, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 9, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 10, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 11, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 12, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 13, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yellow\', 87, 3, 14, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Yrllow\', 87, 3, 15, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 16, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 17, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'d_dddw_cust\', 88, 2, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Day Care\', 85, 4, 1, 536870916, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'Y\', 85, 4, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Day Care\', \'N\', 85, 4, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY\', \'DD/MM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS\', \'DD/MM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS:FFFFFF\', \'DD/MM/YY HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY\', \'DD/MM/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MM/YYYY HH:MM:SS\', \'DD/MM/YY HH::MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY\', \'DD/MMM/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DD/MMM/YY HH:MM:SS\', \'DD/MMM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY\', \'DDD/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YY HH:MM:SS\', \'DDD/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY\', \'DDD/YYYY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'DDD/YYYY HH:MM:SS\', \'DDD/YYYY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'d_dddw_dept\', 88, 10, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_id\', 88, 10, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Department List\', \'dept_d\', 88, 10, 3, 0, \'300\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Dollars with cents\', \'$###,###,###.00\', 90, 2, 1, 32, \'00\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'1\', 86, 3, 1, 1073741832, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Active\', 86, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'A\', 86, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'Terminated\', 86, 3, 4, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'T\', 86, 3, 5, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'On Leave\', 86, 3, 6, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Employee Status\', \'L\', 86, 3, 7, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'d_dddw_sales_reps\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'employees\', \'emp_id\', 88, 3, 3, 0, \'400\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'d_dddw_fin_code\', 88, 3, 1, 536870928, \'0\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 3, 0, \'700\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'health insurance\', 85, 3, 1, 536870928, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'Y\', 85, 3, 2, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'Health Insurance\', \'N\', 85, 3, 3, 0, NULL ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFF\', \'HH:MM:SS:FFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS:FFFFFF\', \'HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY\', \'JJJ/YY\', 90, 1, 1, 32, \'20\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YY HH:MM:SS\', \'JJJ/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'JJJ/YYYY\', \'JJJ/YYYY\', 90, 1, 1, 32, \'40\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
        queries.push_back( L"INSERT INTO \"abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' ) ON CONFLICT DO NOTHING;" );
    }
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        if( pimpl.m_subtype != L"Oracle" || ( pimpl.m_subtype != L"Sybase SQL Anywhere" && pimpl.m_versionMajor >= 12 ) )
        {
            SQLWCHAR *qry = nullptr;
            for( std::vector<std::wstring>::iterator it = queries.begin(); it < queries.end(); ++it )
            {
                qry = new SQLWCHAR[(*it).length() + 2];
                memset( qry, '\0', (*it).length() + 2 );
                uc_to_str_cpy( qry, (*it) );
                ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
                delete[] qry;
                qry = nullptr;
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                    ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                    result = 1;
                    break;
                }
            }
        }
        else
        {
            int i = 1;
            SQLWCHAR *qry = nullptr;
            for( std::vector<std::wstring>::iterator it = queries.begin(); it < queries.end(); ++it )
            {
                qry = new SQLWCHAR[(*it).length() + 2];
                memset( qry, '\0', (*it).length() + 2 );
                uc_to_str_cpy( qry, (*it) );
                ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
                if( i % 2 != 0 )
                {
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                        result = 1;
                        break;
                    }
                    else if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                    {
                        ret = SQLFetch( m_hstmt );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                            result = 1;
                            break;
                        }
                        else if( ret == SQL_NO_DATA )
                        {
                            i++;
                            continue;
                        }
                        else
                        {
                            ++it;
                            i += 2;
                            continue;
                        }
                    }
                    else
                    {
                        ++it;
                        continue;
                    }
                }
                else
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                        result = 1;
                        break;
                    }
            }
            delete[] qry;
            qry = nullptr;
        }
        if( !result )
        {
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
        }
        else
        {
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
        }
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        m_hstmt = 0;
        if( !result )
            result = GetTableListFromDb( errorMsg );
    }
    else
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    return result;
}

int ODBCDatabase::ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg)
{
    SQLWCHAR *qry = nullptr, *dbName = nullptr;
    SQLSMALLINT nameBufLength, dataTypePtr, decimalDigitsPtr, isNullable;
    SQLULEN columnSizePtr = 0;
    SQLLEN cbDatabaseName;
    std::wstring query;
    int result = 0;
    if( pimpl.m_subtype == L"Microsoft SQL Server" )
        query = L"SELECT name AS Database FROM master.dbo.sysdatabases;";
    if( pimpl.m_subtype == L"Sybase" || pimpl.m_subtype == L"ASE" )
        query = L"SELECT name FROM sp_helpdb";
    if( pimpl.m_subtype == L"MySQL"  )
        query = L"SELECT schema_name AS Database FROM information_schema.schemata;";
    if( pimpl.m_subtype == L"PostgreSQL" )
        query = L"SELECT datname AS Database FROM pg_database;";
    qry = new SQLWCHAR[query.length() + 2];
    memset( qry, '\0', query.length() + 2 );
    uc_to_str_cpy( qry, query );
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    if( !result )
    {
        ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLDescribeCol( m_hstmt, 1, NULL, 0, &nameBufLength, &dataTypePtr, &columnSizePtr, &decimalDigitsPtr, &isNullable );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        dbName = new SQLWCHAR[columnSizePtr + 2];
        ret = SQLBindCol( m_hstmt, 1, SQL_C_WCHAR, dbName, columnSizePtr, &cbDatabaseName );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ); ret = SQLFetch( m_hstmt ) )
        {
            std::wstring databaseName;
            str_to_uc_cpy( databaseName, dbName );
            dbList.push_back( databaseName );
            memset( dbName, '\0', columnSizePtr + 2 );
        }
    }
    if( !result )
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    else
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    m_hstmt = 0;
    delete[] qry;
    qry = nullptr;
    delete[] dbName;
    dbName = nullptr;
    return result;
}

int ODBCDatabase::Disconnect(std::vector<std::wstring> &errorMsg)
{
    std::lock_guard<std::mutex> locker( GetTableVector().my_mutex );
    int result = 0;
    RETCODE ret;
    long connectionAlive;
    SQLINTEGER pcbValue;
    if( !m_isConnected )
    {
        ret = SQLFreeHandle( SQL_HANDLE_DBC, m_hdbc );
        ret = SQLFreeHandle( SQL_HANDLE_ENV, m_env );
        return 0;
    }
    ret = SQLGetConnectAttr( m_hdbc, SQL_ATTR_CONNECTION_DEAD, (SQLPOINTER) &connectionAlive, 0, &pcbValue );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        if( errorMsg.empty() )
            connectionAlive = SQL_CD_TRUE;
    }
    if( connectionAlive == SQL_CD_FALSE )
        m_isConnected = true;
/*    int count;
    SQLLEN ptr;
    ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    ret = SQLExecDirect( m_hstmt, L"SELECT @@trancount", SQL_NTS );
    ret = SQLBindCol( m_hstmt, 1, SQL_C_SLONG, &count, 0, &ptr );
    ret = SQLFetch( m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }*/
    if( pimpl.m_subtype == L"PostgreSQL" && connectionAlive != SQL_CD_TRUE )
    {
        ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
        else
        {
            if( pimpl.m_versionMajor >= 9 && pimpl.m_versionMinor > 3 )
            {
                std::wstring query8, query9;
                SQLWCHAR *query = nullptr;
                query9 = L"DROP FUNCTION __watch_schema_changes() CASCADE;";
                query8 = L"DROP EVENT TRIGGER schema_change_notify CASCADE;";
                query = new SQLWCHAR[query8.length() + 2];
                memset( query, '\0', query8.length() + 2 );
                uc_to_str_cpy( query, query8 );
                ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
                delete[] query;
                query = nullptr;
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 1 );
                    result = 1;
                }
                else
                {
                    query = new SQLWCHAR[query9.length() + 2];
                    memset( query, '\0', query9.length() + 2 );
                    uc_to_str_cpy( query, query9 );
                    ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
                    delete[] query;
                    query = nullptr;
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, 1 );
                        result = 1;
                    }
                }
            }
        }
    }
    if( m_hstmt != 0 )
    {
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS )
        {
            GetErrorMessage( errorMsg, 2 );
            result = 1;
        }
        else
            m_hstmt = 0;
    }
    if( m_hdbc != 0 )
    {
        if( m_isConnected )
        {
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
            if( ret != SQL_SUCCESS )
            {
                GetErrorMessage( errorMsg, CONN_ERROR );
                result = 1;
            }
            if( !result )
            {
                ret = SQLDisconnect( m_hdbc );
                if( ret != SQL_SUCCESS )
                {
                    GetErrorMessage( errorMsg, CONN_ERROR );
                    result = 1;
                }
            }
        }
        ret = SQLFreeHandle( SQL_HANDLE_DBC, m_hdbc );
        if( ret != SQL_SUCCESS )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
        else
            m_hdbc = 0;
    }
    if( m_env != 0 )
    {
        ret = SQLFreeHandle( SQL_HANDLE_ENV, m_env );
        if( ret != SQL_SUCCESS )
        {
            GetErrorMessage( errorMsg, ENV_ERROR );
            result = 1;
        }
        else
            m_env = 0;
    }
    for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it = pimpl.m_tables.begin(); it != pimpl.m_tables.end(); it++ )
    {
        std::vector<DatabaseTable *> tableVec = (*it).second;
        for( std::vector<DatabaseTable *>::iterator it1 = tableVec.begin(); it1 < tableVec.end(); it1++ )
        {
            delete (*it1);
            (*it1) = nullptr;
        }
        (*it).second.clear();
    }
    delete odbc_pimpl;
    odbc_pimpl = nullptr;
    delete[] m_connectString;
    m_connectString = nullptr;
    return result;
}

int ODBCDatabase::GetTableListFromDb(std::vector<std::wstring> &errorMsg)
{
    short osid = 0;
    auto ret = SQL_SUCCESS;
    std::wstring query4, query2, owner;
    SQLLEN cbParam[6] = {0, 0, 0, 0, 0, 0};
    int result = 0, bufferSize = 1024, count = 0;
    std::vector<TableField *> fields;
    std::wstring fieldName, fieldType, defaultValue, primaryKey, fkSchema, fkName, fkTable, schema, table, origSchema, origTable, origCol, refSchema, refTable, refCol, cat;
    std::vector<std::wstring> pk_fields, fk_fieldNames;
    std::vector<std::wstring> indexes;
    std::map<int,std::vector<FKField *> > foreign_keys;
    if( m_osId & ( 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 ) ) // Windows
    {
        osid = WINDOWS;
    }
    else if( m_osId & ( 1 << 0 | 1 << 1 ) ) // Mac
    {
        osid = OSX;
    }
    else // *nix
    {
#ifdef __DBGTK__
        osid = GTK;
#elif __DBQT__
        osid = QT;
#endif // __DBGTK__
    }
    std::wstring qry2;
    if( pimpl.m_subtype == L"Microsoft SQL Server" ) // MS SQL SERVER
        qry2 = L"INSERT INTO abcattbl SELECT ?, ?, (SELECT object_id FROM sys.objects o, sys.schemas s WHERE s.schema_id = o.schema_id AND o.name = ? AND s.name = ?),  \'\', 8, 400, \'N\', 0, 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', 0, 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', 0, 0, 34, 0, \'MS Sans Serif\', \'\' WHERE NOT EXISTS(SELECT * FROM dbo.abcattbl WHERE abt_tnam=? AND abt_ownr=?);";
    if( pimpl.m_subtype == L"MySQL" )
        qry2 = L"INSERT IGNORE INTO \"abcattbl\" VALUES( ?, ?, (SELECT CASE WHEN t.engine = 'InnoDB' THEN (SELECT st.table_id FROM information_schema.INNODB_SYS_TABLES st WHERE CONCAT(t.table_schema,'/', t.table_name) = st.name) ELSE (SELECT 0) END AS id FROM information_schema.tables t WHERE t.table_name = ? AND t.table_schema = ?), \'\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', \'\' );";
    if( pimpl.m_subtype == L"PostgreSQL" )
    {
        if( pimpl.m_versionMajor >= 9 && pimpl.m_versionMinor >= 5 )
            qry2 = L"INSERT INTO \"abcattbl\" VALUES( ?, ?, (SELECT c.oid FROM pg_class c, pg_namespace nc WHERE nc.oid = c.relnamespace AND c.relname = ? AND nc.nspname = ?), \'\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', \'\' ) ON CONFLICT DO NOTHING;";
        else
            qry2 = L"INSERT INTO \"abcattbl\" VALUES( ?, ?, (SELECT c.oid FROM pg_class c, pg_namespace nc WHERE nc.oid = c.relnamespace AND c.relname = ? AND nc.nspname = ?), \'\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', \'\' );";
    }
    SQLWCHAR *qry = new SQLWCHAR[qry2.length() + 2];
    memset( qry, '\0', qry2.length() + 2 );
    uc_to_str_cpy( qry, qry2 );
    SQLWCHAR *catalogName = nullptr, *schemaName = nullptr, *tableName = nullptr;
    SQLTablesDataBinding *catalog = (SQLTablesDataBinding *) malloc( 5 * sizeof( SQLTablesDataBinding ) );
    if( !result )
    {
        ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        auto catalogDB = new SQLWCHAR[pimpl.m_dbName.length() + 2];
        memset( catalogDB, '\0', pimpl.m_dbName.length() + 2 );
        uc_to_str_cpy( catalogDB, pimpl.m_dbName );
        for( int i = 0; i < 5; i++ )
        {
            catalog[i].TargetType = SQL_C_WCHAR;
            catalog[i].BufferLength = ( bufferSize + 1 );
            catalog[i].TargetValuePtr = malloc( sizeof( unsigned char ) * catalog[i].BufferLength );
            ret = SQLBindCol( m_hstmt, (SQLUSMALLINT) i + 1, catalog[i].TargetType, catalog[i].TargetValuePtr, catalog[i].BufferLength, &( catalog[i].StrLen_or_Ind ) );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
                break;
            }
        }
        if( !result )
        {
            ret = SQLTables( m_hstmt, catalogDB, SQL_NTS, NULL, 0, NULL, 0, NULL, 0 );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            else
            {
                delete[] catalogDB;
                catalogDB = nullptr;
                for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( m_hstmt ) )
                {
                    if( catalog[0].StrLen_or_Ind != SQL_NULL_DATA )
                        catalogName = (SQLWCHAR *) catalog[0].TargetValuePtr;
                    if( catalog[1].StrLen_or_Ind != SQL_NULL_DATA )
                        schemaName = (SQLWCHAR *) catalog[1].TargetValuePtr;
                    if( catalog[2].StrLen_or_Ind != SQL_NULL_DATA )
                        tableName = (SQLWCHAR *) catalog[2].TargetValuePtr;
                    cat = L"";
                    schema = L"";
                    table = L"";
                    str_to_uc_cpy( cat, catalogName );
                    str_to_uc_cpy( schema, schemaName );
                    str_to_uc_cpy( table, tableName );
                    if( schema == L"" && cat != L"" && schemaName != NULL )
                    {
                        schema = cat;
                        copy_uc_to_uc( schemaName, catalogName );
                    }
                    pimpl.m_tableDefinitions[cat].push_back( TableDefinition( cat, schema, table ) );
                    count++;
                }
            }
        }
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        if( !result )
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
        else
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        if( !result )
        {
            ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            m_hstmt = 0;
        }
        if( !result )
        {
            ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, CONN_ERROR );
                result = 1;
            }
        }
        if( !result )
        {
            auto qry1 = new SQLWCHAR[30];
            memset( qry1, '\0', 30 );
            uc_to_str_cpy( qry1, L"BEGIN TRANSACTION" );
            ret = SQLExecDirect( m_hstmt, qry1, SQL_NTS );
            delete[] qry1;
            qry1 = nullptr;
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
        }
        if( !result )
        {
            if( pimpl.m_subtype == L"Microsoft SQL Server" ) // MS SQL SERVER
            {
                auto qry1 = new SQLWCHAR[30];
                memset( qry1, '\0', 30 );
                uc_to_str_cpy( qry1, L"SET NOCOUNT ON" );
                ret = SQLExecDirect( m_hstmt, qry1, SQL_NTS );
                delete[] qry1;
                qry1 = nullptr;
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                    result = 1;
                }
                if( !result )
                {
                    qry1 = new SQLWCHAR[50];
                    memset( qry1, '\0', 50 );
                    uc_to_str_cpy( qry1, L"SET TRANSACTION ISOLATION LEVEL SERIALIZABLE" );
                    ret = SQLExecDirect( m_hstmt, qry1, SQL_NTS );
                    delete[] qry1;
                    qry1 = nullptr;
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                        result = 1;
                    }
                }
            }
        }
        if( !result )
        {
            ret = SQLPrepare( m_hstmt, qry, SQL_NTS );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
        }
        if( !result )
        {
            ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_TINYINT, 0, 0, &osid, 0, &cbParam[2] );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
        }
        if( !result )
        {
            for( auto it = pimpl.m_tableDefinitions[cat].begin(); it < pimpl.m_tableDefinitions[cat].end(); ++it )
            {
                schemaName = new SQLWCHAR[(*it).schemaName.length() + 2];
                tableName = new SQLWCHAR[(*it).tableName.length() + 2];
                memset( schemaName, '\0', (*it).schemaName.length() + 2 );
                memset( tableName, '\0', (*it).tableName.length() + 2 );
                uc_to_str_cpy( schemaName, (*it).schemaName );
                uc_to_str_cpy( tableName, (*it).tableName );
                if( pimpl.m_subtype == L"Microsoft SQL Server" ) // MS SQL SERVER
                {
                    if( !result )
                    {
                        ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_TINYINT, 0, 0, &osid, 0, &cbParam[2] );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, table.length(), 0, tableName, 0, &cbParam[0] );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindParameter( m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, table.length(), 0, tableName, 0, &cbParam[2] );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindParameter( m_hstmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, schema.length(), 0, schemaName, 0, &cbParam[3] );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindParameter( m_hstmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, table.length(), 0, tableName, 0, &cbParam[4] );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindParameter( m_hstmt, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, schema.length(), 0, schemaName, 0, &cbParam[5] );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                    }
                }
                if( pimpl.m_subtype == L"MySQL" )
                {
                    ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_TINYINT, 0, 0, &osid, 0, &cbParam[0] );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                        result = 1;
                        break;
                    }
                    if( !result )
                    {
                        ret = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, table.length(), 0, tableName, 0, &cbParam[1] );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindParameter( m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, table.length(), 0, tableName, 0, &cbParam[2] );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindParameter( m_hstmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, schema.length(), 0, schemaName, 0, &cbParam[3] );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                    }
                }
                if( !result )
                {
                    ret = SQLExecute( m_hstmt );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                        result = 1;
                        break;
                    }
                }
                if( !result )
                {
                    ret = SQLFreeStmt( m_hstmt, SQL_RESET_PARAMS );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                        result = 1;
                        break;
                    }
                }
                delete[] schemaName;
                delete[] tableName;
                schemaName = nullptr;
                tableName = nullptr;
            }
        }
    }
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    if( !result )
    {
        if( pimpl.m_subtype == L"Microsoft SQL Server" ) // MS SQL SERVER
        {
            auto qry1 = new SQLWCHAR[30];
            memset( qry1, '\0', 30 );
            uc_to_str_cpy( qry1, L"SET NOCOUNT OFF" );
            ret = SQLExecDirect( m_hstmt, qry1, SQL_NTS );
            delete[] qry1;
            qry1 = nullptr;
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
        }
    }
    for( int i = 0; i < 5; i++ )
    {
        free( catalog[i].TargetValuePtr );
        catalog[i].TargetValuePtr = nullptr;
    }
    free( catalog );
    catalog = nullptr;
    if( !result )
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
    else
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    else
        m_hstmt = 0;
    delete[] qry;
    qry = nullptr;
    if( !result )
        m_numOfTables = count;
    return result;
}

int ODBCDatabase::CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &catalogName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    SQLRETURN ret;
    int result = 0;
    SQLWCHAR *query = nullptr;
    ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS || ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, m_hstmt );
        result = 1;
    }
    if( !result )
    {
        auto qry = new SQLWCHAR[30];
        memset( qry, '\0', 30 );
        uc_to_str_cpy( qry, L"BEGIN TRANSACTION" );
        ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        delete[] qry;
        qry = nullptr;
        if( ret != SQL_SUCCESS || ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 1, m_hstmt );
            result = 1;
        }
    }
    if( !result )
    {
        query = new SQLWCHAR[command.length() + 2];
        memset( query, '\0', command.length() + 2 );
        uc_to_str_cpy( query, command );
        bool exists = IsIndexExists( index_name, catalogName, schemaName, tableName, errorMsg );
        if( exists )
        {
            std::wstring temp1 = L"Index ";
            temp1 += index_name;
            temp1 += L" already exists.";
            errorMsg.push_back( temp1 );
            result = 1;
        }
        else if( !errorMsg.empty() )
            result = 1;
        else
        {
            ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
            delete[] query;
            query = nullptr;
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 1, m_hstmt );
                result = 1;
            }
            if( result == 1 )
            {
                ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
            }
            else
            {
                ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
            }
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 1, m_hstmt );
                result = 1;
            }
            ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 1, m_hstmt );
                result = 1;
            }
            m_hstmt = 0;
        }
    }
    else
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    return result;
}

bool ODBCDatabase::IsIndexExists(const std::wstring &indexName, const std::wstring &catalogName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    SQLRETURN ret;
    SQLWCHAR *query = nullptr;
    bool exists = false;
    std::wstring query1;
    SQLWCHAR *index_name = nullptr, *table_name = nullptr, *schema_name = nullptr;
    SQLLEN cbIndexName = SQL_NTS, cbTableName = SQL_NTS, cbSchemaName = SQL_NTS;
    if( pimpl.m_subtype == L"Microsoft SQL Server" )
        query1 = L"SELECT 1 FROM " + catalogName + L".sys.indexes WHERE name = ? AND object_id = OBJECT_ID( ? ) );";
    if( pimpl.m_subtype == L"MySQL" )
        query1 = L"SELECT 1 FROM information_schema.statistics WHERE index_name = ? AND table_name = ? AND schema_name = ? AND table_catalog = ?;";
    if( pimpl.m_subtype == L"PostgreSQL" )
        query1 = L"SELECT 1 FROM " + catalogName + L".pg_catalog.pg_indexes WHERE indexname = $1 AND tablename = $2 AND schemaname = $3;";
    if( pimpl.m_subtype == L"Sybase" || pimpl.m_subtype == L"ASE" )
        query1 = L"SELECT 1 FROM sysindexes ind, sysobjects obj WHERE obj.id = ind.id AND ind.name = ? AND obj.name = ?";
    if( pimpl.m_subtype == L"Oracle" )
        query1 = L"SELECT 1 FROM user_indexes WHERE index_name = ?;";
    index_name = new SQLWCHAR[indexName.length() + 2];
    memset( index_name, '\0', indexName.length() + 2 );
    uc_to_str_cpy( index_name, indexName );
    if( pimpl.m_subtype == L"MySQL" || pimpl.m_subtype == L"PostgreSQL" )
    {
        table_name = new SQLWCHAR[tableName.length() + 2];
        schema_name = new SQLWCHAR[schemaName.length() + 2];
        memset( table_name, '\0', tableName.length() + 2 );
        memset( schema_name, '\0', schemaName.length() + 2 );
        uc_to_str_cpy( schema_name, schemaName );
        uc_to_str_cpy( table_name, tableName );
    }
    else
    {
        table_name = new SQLWCHAR[tableName.length() + schemaName.length() + 2];
        memset( table_name, '\0', tableName.length() + schemaName.length() + 2 );
        uc_to_str_cpy( table_name, schemaName );
        uc_to_str_cpy( table_name, L"." );
        uc_to_str_cpy( table_name, tableName );
    }
    ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        query = new SQLWCHAR[query1.length() + 2];
        memset( query, '\0', query1.size() + 2 );
        uc_to_str_cpy( query, query1 );
        ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, indexName.length(), 0, index_name, 0, &cbIndexName );
        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
        {
            ret = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, tableName.length(), 0, table_name, 0, &cbTableName );
            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
            {
                if( pimpl.m_subtype == L"MySQL" || pimpl.m_subtype == L"PostgreSQL" )
                    ret = SQLBindParameter( m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, schemaName.length(), 0, schema_name, 0, &cbSchemaName );
                if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                {
                    ret = SQLPrepare( m_hstmt, query, SQL_NTS );
                    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                    {
                        ret = SQLExecute( m_hstmt );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            ret = SQLFetch( m_hstmt );
                            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                            {
                                exists = true;
                            }
                            else if( ret != SQL_NO_DATA )
                            {
                                GetErrorMessage( errorMsg, STMT_ERROR );
                            }
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                        }
                    }
                    else
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                    }
                }
                else
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                }
            }
            else
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
            }
        }
        else
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
        }
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        }
        else
        {
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        }
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 1, m_hstmt );
        }
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
        }
        else
            m_hstmt = 0;
    }
    else
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
    }
    delete[] index_name;
    index_name = nullptr;
    delete[] table_name;
    table_name = nullptr;
    delete[] schema_name;
    schema_name = nullptr;
    delete[] query;
    query = nullptr;
    return exists;
}

int ODBCDatabase::GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    int result = 0, id;
    SQLWCHAR *qry = nullptr;
    unsigned short dataFontSize = 0, dataFontWeight = 0, dataFontUnderline = 0, dataFontStriken = 0, headingFontSize = 0, headingFontWeight = 0, headingFontUnderline = 0, headingFontStriken = 0, labelFontSize = 0, labelFontWeight = 0, labelFontUnderline = 0, labelFontStriken = 0;
    unsigned short dataFontCharacterSet = 0, headingFontCharacterSet = 0, labelFontCharacterSet = 0, dataFontPixelSize = 0, headingFontPixelSize = 0, labelFontPixelSize = 0;
    SQLWCHAR dataFontItalic[2], headingFontItalic[2], labelFontItalic[2], dataFontName[20], headingFontName[20], labelFontName[20];
    SQLWCHAR comments[225];
    SQLLEN cbDataFontSize = 0, cbDataFontWeight = 0, cbDataFontItalic = SQL_NTS, cbDataFontUnderline = SQL_NTS, cbDataFontStriken = SQL_NTS, cbDataFontName = 0, cbHeadingFontSize = 0, cbHeadingFontWeight = 0;
    SQLLEN cbHeadingFontItalic = 0,  cbHeadingFontUnderline = 0, cbHeadingFontStriken = 0, cbHeadingFontName = 0, cbComment;
    SQLLEN cbLabelFontSize = 0, cbLabelFontWeight = 0, cbLabelFontItalic = 0, cbLabelFontUnderline = 0, cbLabelFontStriken = 0, cbLabelFontName = 0;
    SQLLEN cbDataFontCharacterSet = 0, cbHeadingFontCharacterSet = 0, cbLabelFontCharacterSet = 0, cbDataFontPixelSize = 0, cbHeadingFontPixelSize = 0, cbLabelFontPixelSize = 0;
    std::wstring query = L"SELECT rtrim(abt_tnam), abt_tid, rtrim(abt_ownr), abd_fhgt, abd_fwgt, abd_fitl, abd_funl, abd_fstr, abd_fchr, abd_fptc, rtrim(abd_ffce), abh_fhgt, abh_fwgt, abh_fitl, abh_funl, abh_fstr, abh_fchr, abh_fptc, rtrim(abh_ffce), abl_fhgt, abl_fwgt, abl_fitl, abl_funl, abl_fstr, abl_fchr, abl_fptc, rtrim(abl_ffce), rtrim(abt_cmnt) FROM abcattbl WHERE rtrim(abt_tnam) = ";
    std::wstring tableName = table->GetTableName(), schemaName = table->GetSchemaName(), ownerName = table->GetTableOwner();
    if( m_osId & ( 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 ) ) // Windows
        id = WINDOWS;
    else if( m_osId & ( 1 << 0 | 1 << 1 ) ) // Mac
        id = OSX;
    else // *nix
    {
#ifdef __DBGTK__
        id = GTK;
#else
        id = QT;
#endif
    }
    if( ownerName.empty() && !schemaName.empty() )
        ownerName = schemaName;
    std::wstring t = schemaName + L".";
    t += tableName;
    query += L"\'";
    query += t;
    query += L"\'";
    query += L" AND rtrim(abt_ownr) = ";
    query += L"\'";
    query += ownerName;
    query += L"\' AND abt_os = ";
    query += std::to_wstring( id );
    query += L";";
    qry = new SQLWCHAR[query.length() + 2];
    memset( qry, '\0', query.size() + 2 );
    uc_to_str_cpy( qry, query );
    auto ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR  );
        result = 1;
    }
    if( !result )
    {
        ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 5, SQL_C_SSHORT, &dataFontSize, 0, &cbDataFontSize );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 6, SQL_C_SSHORT, &dataFontWeight, 0, &cbDataFontWeight );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 7, SQL_C_CHAR, &dataFontItalic, 2, &cbDataFontItalic );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 8, SQL_C_SSHORT, &dataFontUnderline, 2, &cbDataFontUnderline );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 9, SQL_C_SSHORT, &dataFontStriken, 2, &cbDataFontStriken );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 10, SQL_C_SSHORT, &dataFontCharacterSet, 0, &cbDataFontCharacterSet );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 11, SQL_C_SSHORT, &dataFontPixelSize, 0, &cbDataFontPixelSize );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 12, SQL_C_WCHAR, &dataFontName, 44, &cbDataFontName );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 13, SQL_C_SSHORT, &headingFontSize, 0, &cbHeadingFontSize );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 14, SQL_C_SSHORT, &headingFontWeight, 0, &cbHeadingFontWeight );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 15, SQL_C_CHAR, &headingFontItalic, 2, &cbHeadingFontItalic );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 16, SQL_C_SSHORT, &headingFontUnderline, 2, &cbHeadingFontUnderline );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 17, SQL_C_SSHORT, &headingFontStriken, 2, &cbHeadingFontStriken );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 18, SQL_C_SSHORT, &headingFontCharacterSet, 0, &cbHeadingFontCharacterSet );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 19, SQL_C_SSHORT, &headingFontPixelSize, 0, &cbHeadingFontPixelSize );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 20, SQL_C_WCHAR, &headingFontName, 44, &cbHeadingFontName );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 21, SQL_C_SSHORT, &labelFontSize, 0, &cbLabelFontSize );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 22, SQL_C_SSHORT, &labelFontWeight, 0, &cbLabelFontWeight );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 23, SQL_C_CHAR, &labelFontItalic, 3, &cbLabelFontItalic );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 24, SQL_C_SSHORT, &labelFontUnderline, 3, &cbLabelFontUnderline );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
               }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 25, SQL_C_SSHORT, &labelFontStriken, 3, &cbLabelFontStriken );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 26, SQL_C_SSHORT, &labelFontCharacterSet, 0, &cbLabelFontCharacterSet );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 27, SQL_C_SSHORT, &labelFontPixelSize, 0, &cbLabelFontPixelSize );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 28, SQL_C_WCHAR, &labelFontName, 44, &cbLabelFontName );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 29, SQL_C_WCHAR, &comments, 225, &cbComment );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLFetch( m_hstmt );
        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
        {
            TableProperties prop;
            std::wstring name;
            prop.m_dataFontSize = dataFontSize;
            prop.m_dataFontWeight = dataFontWeight;
            prop.m_dataFontItalic = dataFontItalic[0] == 'Y';
            prop.m_dataFontUnderline = dataFontUnderline == 1;
            prop.m_dataFontStrikethrough = dataFontStriken == 1;
            prop.m_dataFontCharacterSet = dataFontCharacterSet;
            prop.m_dataFontPixelSize = dataFontPixelSize;
            str_to_uc_cpy( name, dataFontName );
            prop.m_dataFontName = name;
            name = L"";
            prop.m_headingFontSize = headingFontSize;
            prop.m_headingFontWeight = headingFontWeight;
            prop.m_headingFontItalic = headingFontItalic[0] == 'Y';
            prop.m_headingFontUnderline = headingFontUnderline == 1;
            prop.m_headingFontStrikethrough = headingFontStriken == 1 ? true : false;
            prop.m_headingFontCharacterSet = headingFontCharacterSet;
            prop.m_headingFontPixelSize = headingFontPixelSize;
            str_to_uc_cpy( name, headingFontName );
            prop.m_headingFontName = name;
            name = L"";
            prop.m_labelFontSize = labelFontSize;
            prop.m_labelFontWeight = labelFontWeight;
            prop.m_labelFontItalic = labelFontItalic[0] == 'Y';
            prop.m_labelFontUnderline = labelFontUnderline == 1;
            prop.m_labelFontStrikethrough = labelFontStriken == 1;
            prop.m_labelFontCharacterSer = labelFontCharacterSet;
            prop.m_labelFontPixelSize = labelFontPixelSize;
            str_to_uc_cpy( name, labelFontName );
            prop.m_labelFontName = name;
            name = L"";
            str_to_uc_cpy( name, comments );
            prop.m_comment = name;
            table->SetTableProperties( prop );
            name = L"";
        }
        else if( ret != SQL_NO_DATA )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        if( result == 1 )
        {
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        }
        else
        {
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        }
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 1, m_hstmt );
            result = 1;
        }
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        m_hstmt = 0;
    }
    delete[] qry;
    qry = nullptr;
    table->SetFullName( table->GetCatalog() + L"." + table->GetSchemaName() + L"." + table->GetTableName() );
    return 0;
}

int ODBCDatabase::SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    bool exist;
    std::wostringstream istr;
    std::wstring query = L"BEGIN TRANSACTION";
    SQLWCHAR *qry = new SQLWCHAR[query.length() + 2];
    memset( qry, '\0', query.length() + 2 );
    uc_to_str_cpy( qry, query );
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, m_hstmt );
        result = 1;
    }
    if( !result )
    {
        ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        delete[] qry;
        qry = nullptr;
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        std::wstring tableName = const_cast<DatabaseTable *>( table )->GetTableName();
        std::wstring schemaName = const_cast<DatabaseTable *>( table )->GetSchemaName();
        std::wstring comment = const_cast<DatabaseTable *>( table )->GetTableProperties().m_comment;
        std::wstring tableOwner = const_cast<DatabaseTable *>( table )->GetTableOwner();
        unsigned long tableId = const_cast<DatabaseTable *>( table )->GetTableId();
        exist = IsTablePropertiesExist( table, errorMsg );
        if( errorMsg.size() != 0 )
            result = 1;
        else
        {
            if( exist )
            {
                command = L"UPDATE \"abcattbl\" SET \n\"abt_tnam\" = \'";
                command += schemaName;
                command += L".";
                command += tableName;
                command += L"\',\n \"abt_tid\" = ";
                istr << tableId;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abt_ownr\" = \'";
                command += tableOwner;
                command += L"\',\n  \"abd_fhgt\" = ";
                istr << properties.m_dataFontSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abd_fwgt\" = ";
                istr << properties.m_dataFontWeight;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abd_fitl\" = \'";
                command += properties.m_dataFontItalic ? L"Y" : L"N";
                command += L"\',\n \"abd_funl\" = ";
                istr << ( properties.m_dataFontUnderline ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abd_fstr\" = ";
                istr << ( properties.m_dataFontStrikethrough ? 1: 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abd_fchr\" = ";
                istr << properties.m_dataFontEncoding;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abd_fptc\" = ";
                istr << properties.m_dataFontPixelSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abd_ffce\" = \'";
                command += properties.m_dataFontName;
                command += L"\',\n  \"abh_fhgt\" = ";
                istr << properties.m_headingFontSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abh_fwgt\" = ";
                istr << properties.m_headingFontWeight;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abh_fitl\" = \'";
                command += properties.m_headingFontItalic ? L"Y" : L"N";
                command += L"\',\n \"abh_funl\" = ";
                istr << ( properties.m_headingFontUnderline ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abh_fstr\" = ";
                istr << ( properties.m_headingFontStrikethrough ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abh_fchr\" = ";
                istr << properties.m_headingFontEncoding;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abh_fptc\" = ";
                istr << properties.m_headingFontPixelSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abh_ffce\" = \'";
                command += properties.m_headingFontName;
                command += L"\',\n  \"abl_fhgt\" = ";
                istr << properties.m_labelFontSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abl_fwgt\" = ";
                istr << properties.m_labelFontWeight;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abl_fitl\" = \'";
                command += properties.m_labelFontItalic ? L"Y" : L"N";
                command += L"\',\n \"abl_funl\" = ";
                istr << ( properties.m_labelFontUnderline ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                comment += L",\n \"abl_fstr\" = ";
                istr << ( properties.m_labelFontStrikethrough ? 1 : 0 );
                comment += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abl_fchr\" = ";
                istr << properties.m_labelFontEncoding;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abl_fptc\" = ";
                istr << properties.m_labelFontPixelSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L",\n \"abl_ffce\" = \'";
                command += properties.m_labelFontName;
                command += L"\',\n \"abt_cmnt\" = \'";
                command += properties.m_comment;
                command += L"\'\n WHERE \"abt_tnam\" = \'";
                command += schemaName;
                command += L".";
                command += tableName;
                command += L"\' AND \"abt_tid\" = ";
                istr << tableId;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L" AND \"abt_ownr\" = \'";
                command += pimpl.m_connectedUser;
                command += L"\';";
            }
            else
            {
                command = L"INSERT INTO \"abcattbl\" VALUES( \'";
                command += schemaName;
                command += L".";
                command += tableName;
                command += L"\', ";
                istr << tableId;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \'";
                command += tableOwner;
                command += L"\', ";
                istr << properties.m_dataFontSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << properties.m_dataFontWeight;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \'";
                command += properties.m_dataFontItalic ? L"Y" : L"N";
                command += L"\', ";
                istr << ( properties.m_dataFontUnderline ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << ( properties.m_dataFontStrikethrough ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << properties.m_dataFontEncoding;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << properties.m_dataFontPixelSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \'";
                command += properties.m_dataFontName;
                command += L"\', ";
                istr << properties.m_headingFontSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << properties.m_headingFontWeight;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \'";
                command += properties.m_headingFontItalic ? L"Y" : L"N";
                command += L"\', ";
                istr << ( properties.m_headingFontUnderline ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << ( properties.m_headingFontStrikethrough ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << properties.m_headingFontEncoding;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << properties.m_headingFontPixelSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \'";
                command += properties.m_headingFontName;
                command += L"\', ";
                istr << properties.m_labelFontSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << properties.m_labelFontWeight;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \'";
                command += properties.m_labelFontItalic ? L"Y" : L"N";
                command += L"\', ";
                istr << ( properties.m_labelFontUnderline ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << ( properties.m_labelFontStrikethrough ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << properties.m_labelFontEncoding;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", ";
                istr << properties.m_labelFontPixelSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \'";
                command += properties.m_labelFontName;
                command += L"\', \'";
                command += properties.m_comment;
                command += L"\' );";
            }
            if( !isLog )
            {
                qry = new SQLWCHAR[command.length() + 2];
                memset( qry, '\0', command.length() + 2 );
                uc_to_str_cpy( qry, command );
                ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, CONN_ERROR );
                    result = 1;
                }
                if( !result )
                {
                    ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                        result = 1;
                    }
                }
                if( result == 1 )
                {
                    ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                }
                else
                {
                    ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                }
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                    result = 1;
                }
                ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                    result = 1;
                }
                m_hstmt = 0;
            }
        }
    }
    else
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    delete[] qry;
    qry = nullptr;
    return result;
}

bool ODBCDatabase::IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    bool result = false;
    SQLLEN cbTableName = SQL_NTS, cbSchemaName = SQL_NTS;
    std::wstring query = L"SELECT 1 FROM abcattbl WHERE abt_tnam = ? AND abt_ownr = ?;";
    std::wstring tname = const_cast<DatabaseTable *>( table )->GetSchemaName() + L".";
    tname += const_cast<DatabaseTable *>( table )->GetTableName();
    std::wstring ownerName = const_cast<DatabaseTable *>( table )->GetTableOwner();
    SQLWCHAR *qry = new SQLWCHAR[query.length() + 2], *table_name = new SQLWCHAR[tname.length() + 2], *owner_name = new SQLWCHAR[ownerName.length() + 2];
    memset( owner_name, '\0', ownerName.length() + 2 );
    memset( table_name, '\0', tname.length() + 2 );
    uc_to_str_cpy( owner_name, ownerName );
    uc_to_str_cpy( table_name, tname );
    memset( qry, '\0', query.size() + 2 );
    uc_to_str_cpy( qry, query );
    SQLRETURN ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
    }
    else
    {
        ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, tname.length(), 0, table_name, 0, &cbTableName );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
        }
        else
        {
            ret = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, ownerName.length(), 0, owner_name, 0, &cbSchemaName );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
            }
            else
            {
                ret = SQLPrepare( m_hstmt, qry, SQL_NTS );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                }
                else
                {
                    ret = SQLExecute( m_hstmt );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                    }
                    else
                    {
                        ret = SQLFetch( m_hstmt );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                            result = true;
                        else if( ret != SQL_NO_DATA )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                        }
                        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                        }
                        m_hstmt = 0;
                    }
                }
            }
        }
    }
    if( result == 1 )
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    else
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    delete[] qry;
    qry = nullptr;
    delete[] table_name;
    table_name = nullptr;
    delete[] owner_name;
    owner_name = nullptr;
    return result;
}

int ODBCDatabase::GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &ownerName, const std::wstring &fieldName, TableField *field, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    short justify = 0;
    std::wstring fieldFormat = L"";
    SQLWCHAR *commentField = nullptr, *label = nullptr, *heading = nullptr, *formatNameField = nullptr, *formatField = nullptr, *fF = nullptr;
    SQLWCHAR *qry = nullptr;
    unsigned short labelAlignment = 0, headingAlignment = 0;
    SQLWCHAR *table = new SQLWCHAR[schemaName.length() + tableName.length() + 3], *owner = new SQLWCHAR[ownerName.length() + 2], *fieldNameReq = new SQLWCHAR[fieldName.length() + 2];
    memset( table, '\0', schemaName.length() + tableName.length() + 3 );
    memset( owner, '0', ownerName.length() + 2 );
    memset( fieldNameReq, '\0', fieldName.length() + 2);
    uc_to_str_cpy( table, schemaName );
    uc_to_str_cpy( table, L"." );
    uc_to_str_cpy( table, tableName );
    uc_to_str_cpy( owner, ownerName );
    uc_to_str_cpy( fieldNameReq, fieldName );
    SQLLEN cbSchemaName = SQL_NTS, cbTableName = SQL_NTS, cbFieldName = SQL_NTS, cbCommentField = SQL_NTS, cbLabelField = SQL_NTS, cbHeadingField = SQL_NTS;
    SQLLEN cbLabelAlignment = 0, cbHeadingAlignment = 0, cbJustify = 0, cbFormatName = 0, cbFormat = 0, cbFieldFormat = 0;
    SQLSMALLINT dataType, decimalDigits = 0, nullable;
    SQLULEN paramSize = 0;
    std::wstring query = L"SELECT * FROM \"abcatcol\" WHERE \"abc_tnam\" = ? AND \"abc_ownr\" = ? AND \"abc_cnam\" = ?;";
    std::wstring query1 = L"SELECT * FROM \"abcatfmt\" WHERE \"abc_type\" = ?";
    qry = new SQLWCHAR[query.length() + 2];
    memset( qry, '\0', query.length() + 2 );
    uc_to_str_cpy( qry, query );
    auto ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    if( !result )
    {
        ret = SQLDescribeParam( m_hstmt, 1, &dataType, &paramSize, &decimalDigits, &nullable );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, paramSize, decimalDigits, &table, 0, &cbTableName );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLDescribeParam( m_hstmt, 2, &dataType, &paramSize, &decimalDigits, &nullable );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, paramSize, decimalDigits, &owner, 0, &cbSchemaName );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLDescribeParam( m_hstmt, 1, &dataType, &paramSize, &decimalDigits, &nullable );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindParameter( m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, paramSize, decimalDigits, &fieldNameReq, 0, &cbFieldName );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLPrepare( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLExecute( m_hstmt );
        delete[] qry;
        qry = nullptr;
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 18, SQL_C_WCHAR, &commentField, 3, &cbCommentField );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 6, SQL_C_WCHAR, &label, 254, &cbLabelField );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 7, SQL_C_SSHORT, &labelAlignment, 0, &cbLabelAlignment );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 8, SQL_C_WCHAR, &heading, 254, &cbHeadingField );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 9, SQL_C_SSHORT, &headingAlignment, 0, &cbHeadingAlignment );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 10, SQL_C_SHORT, &justify, 0, &cbJustify );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 11, SQL_C_WCHAR, &fF, 31, &cbFieldFormat );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLFetch( m_hstmt );
        if( ret == SQL_NO_DATA )
        {
            field->GetFieldProperties().m_comment = L"";
            field->GetFieldProperties().m_heading.m_label = fieldName;
            field->GetFieldProperties().m_heading.m_heading = fieldName;
            field->GetFieldProperties().m_heading.m_labelAlignment = 0;
            field->GetFieldProperties().m_heading.m_headingAlignment = 1;
            field->GetFieldProperties().m_display.m_justify = 0;
            fieldFormat = L"";
        }
        else if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        else if( ret != SQL_NO_DATA )
        {
            std::wstring comment, lbl, head;
            str_to_uc_cpy( comment, commentField );
            str_to_uc_cpy( lbl, label );
            str_to_uc_cpy( head, heading );
            str_to_uc_cpy( fieldFormat, fF );
            field->GetFieldProperties().m_comment = comment;
            field->GetFieldProperties().m_heading.m_label = lbl;
            field->GetFieldProperties().m_heading.m_heading = head;
            field->GetFieldProperties().m_heading.m_labelAlignment = labelAlignment;
            field->GetFieldProperties().m_heading.m_headingAlignment = headingAlignment;
            field->GetFieldProperties().m_display.m_justify = justify;
        }
    }
    if( result == 1 )
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    else
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    m_hstmt = 0;
    qry = new SQLWCHAR[query1.length() + 2];
    memset( qry, '\0', query1.length() + 2 );
    uc_to_str_cpy( qry, query1 );
    ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    if( !result )
    {
        int type;
        if( field->GetFieldType() == L"date" )
            type = 82;
        if( field->GetFieldType() == L"datetime" || 
            field->GetFieldType() == L"datwtime2" || 
            field->GetFieldType() == L"smalldatetime" ||
            field->GetFieldType() == L"timestamp" )
            type = 84;
        if( field->GetFieldType() == L"real" ||
            field->GetFieldType() == L"float" ||
            field->GetFieldType() == L"numeric" )
            type = 81;
        if( field->GetFieldType() == L"mpney" )
            type = 81;
        else
            type = 80;
        ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_TINYINT, 0, 0, &type, 0, &cbTableName );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLPrepare( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLExecute( m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 1, SQL_C_WCHAR, &formatNameField, 30, &cbFormatName );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 2, SQL_C_WCHAR, &formatField, 254, &cbFormat );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ); ret = SQLFetch( m_hstmt ) )
        {
            std::wstring formatName, format;
            str_to_uc_cpy( formatName, formatNameField );
            str_to_uc_cpy( format, formatField );
            if( fieldFormat == formatName )
                field->GetFieldProperties().m_display.m_format[1].push_back( std::make_pair( formatName, format ) );
            else
                field->GetFieldProperties().m_display.m_format[0].push_back( std::make_pair( formatName, format ) );
        }
    }
    if( result == 1 )
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    else
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    m_hstmt = 0;
    delete[] qry;
    qry = nullptr;
    delete[] table;
    table = nullptr;
    delete[] owner;
    owner = nullptr;
    delete[] fieldNameReq;
    fieldNameReq = nullptr;
    return result;
}

int ODBCDatabase::GetFieldProperties (const std::wstring &table, TableField *field, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    bool found = false;
    std::wstring schemaName, ownerName;
    for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it = pimpl.m_tables.begin(); it != pimpl.m_tables.end(); ++it )
    {
        if( ( *it ).first == pimpl.m_dbName )
        {
            for( std::vector<DatabaseTable *>::iterator it1 = (*it).second.begin(); it1 < (*it).second.end() || !found; ++it1 )
            {
                if( ( *it1 )->GetTableName () == table )
                {
                    found = true;
                    schemaName = (*it1)->GetSchemaName();
                    ownerName = (*it1)->GetTableOwner();
                }
            }
        }
    }
    result = GetFieldProperties( table, schemaName, ownerName, field->GetFieldName(), field, errorMsg );
    return result;
}

int ODBCDatabase::ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &foreignKeyFields, const std::wstring &refTableName, const std::vector<std::wstring> &refKeyFields, int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"ALTER TABLE ";
    query += tableName.GetSchemaName() + L"." + tableName.GetTableName() + L" ";
    query += L"ADD CONSTRAINT " + keyName + L" ";
    query += L"FOREIGN KEY(";
    std::vector<std::wstring> origFields, refFields;
    for( std::vector<FKField *>::const_iterator it1 = newFK.begin(); it1 < newFK.end(); it1++ )
    {
        query += (*it1)->GetOriginalFieldName();
        if( it1 == newFK.end() - 1 )
            query += L") ";
        else
            query += L",";
        origFields.push_back( (*it1)->GetOriginalFieldName() );
    }
    if( newFK.size() > 0 )
        query += L"REFERENCES " + newFK.at ( 0 )->GetReferencedTableName() + L"(";
    for( std::vector<FKField *>::const_iterator it1 = newFK.begin(); it1 < newFK.end(); it1++ )
    {
        query += (*it1)->GetReferencedFieldName();
        if( it1 == newFK.end() - 1 )
            query += L") ";
        else
            query += L",";
        refFields.push_back( (*it1)->GetReferencedFieldName() );
    }
    query += L"ON DELETE ";
    FK_ONUPDATE updProp = NO_ACTION_UPDATE;
    FK_ONDELETE delProp = NO_ACTION_DELETE;
    switch( deleteProp )
    {
    case 0:
        query += L"NO ACTION ";
        delProp = NO_ACTION_DELETE;
        break;
    case 1:
        if( pimpl.m_subtype == L"Microsoft SQL Server" )
        {
            query += L"NO ACTION ";
            delProp = NO_ACTION_DELETE;
        }
        else
        {
            query += L"RESTRICT ";
            delProp = RESTRICT_DELETE;
        }
        break;
    case 2:
        query += L"CASCADE ";
        delProp = CASCADE_DELETE;
        break;
    case 3:
        query += L"SET NULL ";
        delProp = SET_NULL_DELETE;
        break;
    case 4:
        query += L"SET DEFAULT ";
        delProp = SET_DEFAULT_DELETE;
        break;
    }
    query += L"ON UPDATE ";
    switch( updateProp )
    {
    case 0:
        query += L"NO ACTION";
        updProp = NO_ACTION_UPDATE;
        break;
    case 1:
        if( pimpl.m_subtype == L"Microsoft SQL Server" )
        {
            query += L"NO ACTION ";
            updProp = NO_ACTION_UPDATE;
        }
        else
        {
            query += L"RESTRICT ";
            updProp = RESTRICT_UPDATE;
        }
        query += L"RESTRICT";
        updProp = RESTRICT_UPDATE;
        break;
    case 2:
        query += L"CASCADE";
        updProp = CASCADE_UPDATE;
        break;
    case 3:
        query += L"SET NULL";
        updProp = SET_NULL_UPDATE;
        break;
    case 4:
        query += L"SET DEFAULT";
        updProp = SET_DEFAULT_UPDATE;
        break;
    }
    if( !isNew )
        result = DropForeignKey( command, tableName, keyName, logOnly, errorMsg );
    if( !result )
    {
        if( !logOnly && !newFK.empty() )
        {
            SQLRETURN ret;
            SQLWCHAR *qry = nullptr;
            ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, CONN_ERROR );
                result = 1;
            }
            if( !result )
            {
                qry = new SQLWCHAR[query.length() + 2];
                memset( qry, '\0', query.size() + 2 );
                uc_to_str_cpy( qry, query );
                ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                    result = 1;
                }
            }
            if( !result )
            {
                std::map<unsigned long, std::vector<FKField *> > fKeys = tableName.GetForeignKeyVector();
                unsigned long size = fKeys.size();
                for( unsigned int i = 0; i < foreignKeyFields.size(); i++ )
                    fKeys[size].push_back( new FKField( i, keyName, L"", tableName.GetTableName(), foreignKeyFields.at( i ), L"", refTableName, refKeyFields.at( i ), origFields, refFields, updProp, delProp ) );
            }
            if( result == 1 )
            {
                ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
            }
            else
            {
                ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
            }
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            m_hstmt = 0;
            delete[] qry;
            qry = nullptr;
        }
        else if( logOnly )
        {
            command = query;
        }
    }
    return result;
}

int ODBCDatabase::DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    SQLWCHAR *qry = nullptr;
    std::wstring query = L"DROP TABLE ";
    query += tableName;
    SQLRETURN ret;
    ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    if( !result )
    {
        qry = new SQLWCHAR[query.length() + 2];
        memset( qry, '\0', query.length() + 2 );
        uc_to_str_cpy( qry, query );
        ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( result == 1 )
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    else
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 2, m_hstmt );
        result = 1;
    }
    m_hstmt = 0;
    delete[] qry;
    qry = nullptr;
    return result;
}

int ODBCDatabase::SetFieldProperties(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, const FieldProperties &prop, bool isLogOnly, std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    SQLRETURN result = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        res = 1;
    }
    else
    {
        bool exist = IsFieldPropertiesExist( tableName, ownerName, fieldName, errorMsg );
        if( exist )
        {
            command = L"INSERT INTO abcatcol(abc_tnam, abc_ownr, abc_cnam, abc_labl, abc_lpos, abc_hdr, abc_hpos, abc_cmnt) VALUES(";
            command += tableName;
            command += L", " + ownerName;
            command += L", " + fieldName;
            command += L", " + prop.m_heading.m_label;
            command += L", " + std::to_wstring( prop.m_heading.m_labelAlignment );
            command += L", " + prop.m_heading.m_heading;
            command += L", " + std::to_wstring( prop.m_heading.m_headingAlignment );
            command += L", " + prop.m_comment + L");";
        }
        else
        {
            command = L"UPDATE abcatcol SET abc_labl = ";
            command += prop.m_heading.m_label + L", abc_hdr = ";
            command += prop.m_heading.m_heading + L", abc_cmnt = ";
            command += prop.m_comment + L", abc_labl = ";
            command += prop.m_heading.m_label + L", abc_lpos = ";
            command += std::to_wstring( prop.m_heading.m_labelAlignment ) + L", abc_hdr = ";
            command += prop.m_heading.m_heading + L", abc_hpos = ";
            command += std::to_wstring( prop.m_heading.m_headingAlignment ) + L"WHERE abc_tnam = ";
            command += tableName + L" AND abc_ownr = ";
            command += ownerName + L" AND abc_cnam = ";
            command += fieldName + L";";
        }
        if( !isLogOnly )
        {
            SQLWCHAR *query = new SQLWCHAR[command.length() + 2];
            memset( query, '\0', command.length() + 2 );
            uc_to_str_cpy( query, command );
            result = SQLExecDirect( m_hstmt, query, /*command.length() + 2*/SQL_NTS );
            if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                res = 1;
            }
            delete[] query;
            query = nullptr;
        }
        if( result == 1 )
        {
            result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        }
        else
        {
            result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        }
        if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
        result = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            res = 1;
        }
        m_hstmt = 0;
    }
    return res;
}

int ODBCDatabase::GetTableId(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, long &tableId, std::vector<std::wstring> &errorMsg)
{
    SQLSMALLINT dataType[2], decimalDigit[2], nullable[2];
    SQLULEN parameterSize[2];
    SQLLEN cbName, cbTableName = SQL_NTS, cbSchemaName = SQL_NTS;
    int result = 0;
    std::wstring query;
    SQLWCHAR *qry;
    if( pimpl.m_subtype == L"Microsoft SQL Server" || pimpl.m_subtype == L"Sybase" || pimpl.m_subtype == L"ASE" )
        query = L"SELECT object_id FROM sys.objects o, sys.schemas s WHERE s.schema_id = o.schema_id AND o.name = ? AND s.name = ?";
//        query = L"SELECT object_id FROM sys.objects o, sys.schemas s WHERE s.schema_id = o.schema_id AND o.name = 'abcatcol' AND s.name = 'dbo';";
    if( pimpl.m_subtype == L"PostgreSQL" )
        query = L"SELECT c.oid FROM pg_class c, pg_namespace nc WHERE nc.oid = c.relnamespace AND c.relname = ? AND nc.nspname = ?;";
    if( pimpl.m_subtype == L"MySQL" )
        query = L"SELECT CASE WHEN t.engine = 'InnoDB' THEN (SELECT st.table_id FROM information_schema.INNODB_SYS_TABLES st WHERE CONCAT(t.table_schema,'/', t.table_name) = st.name) ELSE (SELECT 0) END AS id FROM information_schema.tables t WHERE t.table_name = ? AND t.table_schema = ?;";
    if( pimpl.m_subtype == L"Oracle" )
        query = L"SELECT object_id FROM all_objects WHERE object_name = UPPER(?) AND owner = UPPER(?)";
    if( pimpl.m_subtype == L"Sybase SQL Anywhere" )
        query = L"SELECT id FROM dbo.sysobjects WHERE name = ?";
    qry = new SQLWCHAR[query.length() + 2];
    auto tname = new SQLWCHAR[tableName.length() + 2];
    auto sname = new SQLWCHAR[schemaName.length() + 2];
    memset( qry, '\0', query.length() + 2 );
    memset( tname, '\0', tableName.length() + 2 );
    memset( sname, '\0', schemaName.length() + 2);
    uc_to_str_cpy( qry, query );
    uc_to_str_cpy( sname, schemaName );
    uc_to_str_cpy( tname, tableName );
    auto dbName = new SQLWCHAR[pimpl.m_dbName.length() + 2];
    memset( dbName, '\0', pimpl.m_dbName.length() + 2 );
    uc_to_str_cpy( dbName, pimpl.m_dbName );
    auto retcode = SQLSetConnectAttr( m_hdbc, SQL_ATTR_CURRENT_CATALOG, dbName, SQL_NTS );
    delete[]  dbName;
    dbName = nullptr;
    if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    if( !result )
    {
        retcode = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLPrepare( m_hstmt, qry, SQL_NTS );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLDescribeParam( m_hstmt, 1, &dataType[0], &parameterSize[0], &decimalDigit[0], &nullable[0] );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, dataType[0], parameterSize[0], decimalDigit[0], tname, 0, &cbTableName );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result  )
    {
        retcode = SQLDescribeParam( m_hstmt, 2, &dataType[1], &parameterSize[1], &decimalDigit[1], &nullable[1] );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, dataType[1], parameterSize[1], decimalDigit[1], sname, 0, &cbSchemaName );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLExecute( m_hstmt );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLBindCol( m_hstmt, 1, SQL_C_SLONG, &tableId, 100, &cbName );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLFetch( m_hstmt );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO && retcode != SQL_NO_DATA )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        else if( retcode == SQL_NO_DATA )
            tableId = 0;
    }
    if( result == 1 )
    {
        result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    else
    {
        result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    delete[] tname;
    tname = nullptr;
    delete[] sname;
    sname = nullptr;
    delete[] qry;
    if( m_hstmt )
    {
        retcode = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        m_hstmt = 0;
    }
    return result;
}

int ODBCDatabase::GetTableOwner(const std::wstring &UNUSED(catalog), const std::wstring &schemaName, const std::wstring &tableName, std::wstring &tableOwner, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    SQLLEN cbTableName = SQL_NTS, cbSchemaName = SQL_NTS;
    SQLWCHAR *table_name = nullptr, *schema_name = nullptr, *qry = nullptr, *owner = nullptr;
    SQLSMALLINT dataType[2], decimalDigits[2], nullable[2];
    SQLULEN paramSize[2];
    SQLSMALLINT nameBufLength, dataTypePtr, decimalDigitsPtr, isNullable;
    SQLULEN columnSizePtr = 0;
    SQLLEN cbTableOwner;
    std::wstring query;
    table_name = new SQLWCHAR[tableName.length() + 2];
    schema_name = new SQLWCHAR[schemaName.length() + 2];
    memset( table_name, '\0', tableName.length() + 2 );
    memset( schema_name, '\0', schemaName.length() + 2 );
    uc_to_str_cpy( table_name, tableName );
    uc_to_str_cpy( schema_name, schemaName );
    if( pimpl.m_subtype == L"Microsoft SQL Server" )
        query = L"SELECT cast(su.name AS nvarchar(128)) FROM sysobjects so, sysusers su, sys.tables t, sys.schemas s WHERE so.uid = su.uid AND t.object_id = so.id AND t.schema_id = s.schema_id AND so.name = ? AND s.name = ?;";
    if( pimpl.m_subtype == L"PostgreSQL" )
        query = L"SELECT u.usename FROM pg_class c, pg_user u, pg_namespace n WHERE n.oid = c.relnamespace AND u.usesysid = c.relowner AND relname = ? AND n.nspname = ?";
    if( pimpl.m_subtype == L"Sybase" || pimpl.m_subtype == L"ASE" || pimpl.m_subtype == L"Sybase SQL Anywhere" )
        query = L"SELECT su.name FROM sysobjects so, sysusers su WHERE su.uid = so.uid AND so.name = ?";
    if( pimpl.m_subtype == L"MySQL" )
    {
        odbc_pimpl->m_currentTableOwner = L"";
        tableOwner = L"";
        return result;
    }
    if( pimpl.m_subtype == L"Oracle" )
    {
        odbc_pimpl->m_currentTableOwner = schemaName;
        tableOwner = schemaName;
        return result;
    }
    auto retcode = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    if( !result )
    {
        qry = new SQLWCHAR[query.length() + 2];
        memset( qry, '\0', query.length() + 2 );
        uc_to_str_cpy( qry, query );
        retcode = SQLPrepare( m_hstmt, qry, SQL_NTS );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLDescribeParam( m_hstmt, 1, &dataType[0], &paramSize[0], &decimalDigits[0], &nullable[0] );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, dataType[0], paramSize[0], decimalDigits[0], table_name, 0, &cbTableName );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result && ( pimpl.m_subtype != L"Sybase SQL Anywhere" && pimpl.m_subtype != L"ASE" && pimpl.m_subtype != L"Sybase" ) )
    {
        retcode = SQLDescribeParam( m_hstmt, 2, &dataType[1], &paramSize[1], &decimalDigits[1], &nullable[1] );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result && ( pimpl.m_subtype != L"Sybase SQL Anywhere" && pimpl.m_subtype != L"ASE" && pimpl.m_subtype != L"Sybase" ) )
    {
        retcode = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, dataType[1], paramSize[1], decimalDigits[1], schema_name, 0, &cbSchemaName );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLExecute( m_hstmt );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLDescribeCol( m_hstmt, 1, NULL, 0, &nameBufLength, &dataTypePtr, &columnSizePtr, &decimalDigitsPtr, &isNullable );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        owner = new SQLWCHAR[columnSizePtr + 1];
        retcode = SQLBindCol( m_hstmt, 1, SQL_C_WCHAR, owner, columnSizePtr, &cbTableOwner );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        retcode = SQLFetch( m_hstmt );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO && retcode != SQL_NO_DATA )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
            str_to_uc_cpy( tableOwner, owner );
        if( retcode == SQL_NO_DATA )
            tableOwner = schemaName;
    }
    if( result == 1 )
    {
        result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    else
    {
        result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
    }
    if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    retcode = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    m_hstmt = 0;
    delete[] qry;
    qry = nullptr;
    delete[] table_name;
    table_name = nullptr;
    delete[] schema_name;
    schema_name = nullptr;
    delete[] owner;
    owner = nullptr;
    return result;
}

void ODBCDatabase::SetFullType(TableField *field)
{
    std::wostringstream ss;
    std::wstring fieldType = field->GetFieldType();
    if( pimpl.m_subtype == L"Microsoft SQL Server" )
    {
        if( fieldType == L"bigint" || fieldType == L"bit" || fieldType == L"int" || fieldType == L"smallint" || fieldType == L"tinyint" || fieldType == L"date" || fieldType == L"datetime" || fieldType == L"smalldatetime" || fieldType == L"text" || fieldType == L"image" || fieldType == L"ntext" )
            field->SetFullType( fieldType );
        if( fieldType == L"decimal" || fieldType == L"numeric" )
        {
            std::wstring type = fieldType + L"(";
            ss << field->GetFieldSize();
            type += ss.str();
            ss.clear();
            ss.str( L"" );
            type += L",";
            ss << field->GetPrecision();
            type += ss.str();
            type += L")";
            field->SetFullType( type );
        }
        if( fieldType == L"float" || fieldType == L"real" || fieldType == L"datetime2" || fieldType == L"datetimeoffset" || fieldType == L"time" || fieldType == L"char" || fieldType == L"varchar" || fieldType == L"binary" || fieldType == L"varbinary" )
        {
            std::wstring type = fieldType + L"(";
            ss << field->GetFieldSize();
            type += ss.str();
            ss.clear();
            ss.str( L"" );
            type += L")";
        }
    }
}

int ODBCDatabase::GetServerVersion(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query;
    unsigned long versionMajor = 0, versionMinor = 0;
    SQLLEN cbVersion = SQL_NTS;
    SQLWCHAR *qry = nullptr, version[1024];
    if( pimpl.m_subtype == L"Microsoft SQL Server" ) // MS SQL SERVER
    {
        query = L"SELECT SERVERPROPERTY('productversion') AS version, COALESCE(SERVERPROPERTY('ProductMajorVersion'), PARSENAME(CAST(SERVERPROPERTY('productversion') AS varchar(20)), 4)) AS major, COALESCE(SERVERPROPERTY('ProductMinorVersion'), PARSENAME(CAST(SERVERPROPERTY('productversion') AS varchar(20)), 3)) AS minor;";
    }
    if( pimpl.m_subtype == L"MySQL" )
    {
        query = L"SELECT version(), LEFT( version(), LOCATE( '.', version() ) - 1 ) AS major, LEFT( SUBSTR( version(), LOCATE( '.', version() ) + 1 ), LOCATE( '.', SUBSTR( version(), LOCATE( '.', version() ) + 1 ) ) - 1 ) AS minor;";
    }
    if( pimpl.m_subtype == L"PostgreSQL" )
    {
        query = L"SELECT version() AS version, split_part( split_part( version(), ' ', 2 ) , '.', 1 ) AS major, split_part( split_part( version(), ' ', 2 ), '.', 2 ) AS minor;";
    }
    if( pimpl.m_subtype == L"Sybase" || pimpl.m_subtype == L"ASE" )
    {
        query = L"SELECT @@version_number AS version, @@version_as_integer / 1000 AS major";
    }
    if( pimpl.m_subtype == L"Sybase SQL Anywhere" || pimpl.m_subtype == L"SQL Anywhere" )
    {
        query = L"SELECT @@version AS version";
    }
    if( pimpl.m_subtype == L"Oracle" )
    {
        query = L"SELECT version FROM v$instance";
    }
    if( pimpl.m_subtype != L"ACCESS" )
    {
        SQLRETURN retcode = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
        else
        {
            qry = new SQLWCHAR[query.length() + 2];
            memset( qry, '\0', query.length() + 2 );
            uc_to_str_cpy( qry, query );
            retcode = SQLExecDirect( m_hstmt, qry, SQL_NTS );
            delete[] qry;
            qry = nullptr;
            if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            else
            {
                retcode = SQLBindCol( m_hstmt, 1, SQL_C_WCHAR, &version, 1024, &cbVersion );
                if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                    result = 1;
                }
                else
                {
                    if( pimpl.m_subtype != L"Sybase SQL Anywhere" && pimpl.m_subtype != L"Oracle" )
                    {
                        retcode = SQLBindCol( m_hstmt, 2, SQL_C_SLONG, &versionMajor, 0, 0 );
                        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                        }
                        else
                        {
                            retcode = SQLBindCol( m_hstmt, 3, SQL_C_SLONG, &versionMinor, 0, 0 );
                            if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errorMsg, STMT_ERROR );
                                result = 1;
                            }
                        }
                    }
                    if( !result )
                    {
                        retcode = SQLFetch( m_hstmt );
                        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                        }
                        else
                        {
                            if( pimpl.m_subtype != L"Sybase SQL Anywhere" && pimpl.m_subtype != L"Oracle" )
                            {
                                str_to_uc_cpy( pimpl.m_serverVersion, version );
                                if( pimpl.m_subtype == L"SQL Anywhere" )
                                {
                                    std::string::size_type pos = pimpl.m_serverVersion.find( L"." );
                                    pimpl.m_versionMajor = std::stoi( pimpl.m_serverVersion.substr( 0, pos ) );
                                    std::string::size_type posSec = pimpl.m_serverVersion.find( L".", pos );
                                    pimpl.m_versionMinor = std::stoi( pimpl.m_serverVersion.substr( pos, posSec ) );
                                }
                                else
                                {
                                    pimpl.m_versionMajor = (int) versionMajor;
                                    pimpl.m_versionMinor = (int) versionMinor;
                                }
                            }
                            else
                            {
                                std::wstring temp;
                                str_to_uc_cpy( temp, version );
                                pimpl.m_serverVersion = temp;
                                pimpl.m_versionMajor = std::stoi( temp.substr( 0, temp.find( '.' ) ) );
                                temp = temp.substr( temp.find( '.' ) + 1 );
                                pimpl.m_versionMinor = std::stoi( temp.substr( 0, temp.find( '.' ) ) );
                                temp = temp.substr( temp.find( '.' ) + 1 );
                                pimpl.m_versionRevision = std::stoi( temp.substr( 0, temp.find( '.' ) ) );
                            }
                        }
                    }
                }
            }
            if( result == 1 )
            {
                result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
            }
            else
            {
                result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
            }
            if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, CONN_ERROR );
                result = 1;
            }
            retcode = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
            if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            m_hstmt = 0;
        }
        int bufferSize = 1024;
        std::wstring clientVersion;
        SQLRETURN ret = SQLGetInfo( m_hdbc, SQL_DRIVER_VER, version, 1024, (SQLSMALLINT *) &bufferSize );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2 );
            result = 1;
        }
        else
            str_to_uc_cpy( clientVersion, version );
        pimpl.m_clientVersionMajor = 0;
        pimpl.m_clientVersionMinor = 0;
    }
    return result;
}

int ODBCDatabase::CreateIndexesOnPostgreConnection(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
/*    SQLWCHAR *qry1 = NULL, *qry2 = NULL, *qry3 = NULL, *qry4 = NULL;
    std::wstring query1 = L"SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcattbl_tnam_ownr\' AND n.nspname = \'public\';";
    std::wstring query3 = L"CREATE INDEX  \"abcattbl_tnam_ownr\" ON \"abcattbl\"(\"abt_tnam\" ASC, \"abt_ownr\" ASC);";
    std::wstring query2 = L"SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcatcol_tnam_ownr_cnam\' AND n.nspname = \'public\'";
    std::wstring query4 = L"CREATE INDEX \"abcatcol_tnam_ownr_cnam\" ON \"abcatcol\"(\"abc_tnam\" ASC, \"abc_ownr\" ASC, \"abc_cnam\" ASC);";
    qry1 = new SQLWCHAR[query1.length() + 2];
    qry2 = new SQLWCHAR[query2.length() + 2];
    qry3 = new SQLWCHAR[query3.length() + 2];
    qry4 = new SQLWCHAR[query4.length() + 2];
    memset( qry1, '\0', query1.length() + 2 );
    memset( qry2, '\0', query2.length() + 2 );
    memset( qry3, '\0', query3.length() + 2 );
    memset( qry4, '\0', query4.length() + 2 );
    uc_to_str_cpy( qry1, query1 );
    uc_to_str_cpy( qry2, query2 );
    uc_to_str_cpy( qry3, query3 );
    uc_to_str_cpy( qry4, query4 );
    RETCODE ret = SQLExecDirect( m_hstmt, qry1, SQL_NTS );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        ret = SQLFetch( m_hstmt );
        if( ret == SQL_NO_DATA )
        {
            ret = SQLFreeStmt( m_hstmt, SQL_CLOSE );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 1, m_hstmt );
                result = 1;
            }
            else
            {
                ret = SQLExecDirect( m_hstmt, qry3, SQL_NTS );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 1, m_hstmt );
                    result = 1;
                }
            }
        }
        else if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 1, m_hstmt );
            result = 1;
        }
    }
    else
    {
        GetErrorMessage( errorMsg, 1, m_hstmt );
        result = 1;
    }
    if( !result )
    {
        ret = SQLFreeStmt( m_hstmt, SQL_CLOSE );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 1, m_hstmt );
            result = 1;
        }
        else
        {
            ret = SQLExecDirect( m_hstmt, qry2, SQL_NTS );
            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
            {
                ret = SQLFetch( m_hstmt );
                if( ret == SQL_NO_DATA )
                {
                    ret = SQLFreeStmt( m_hstmt, SQL_CLOSE );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, 1, m_hstmt );
                        result = 1;
                    }
                    else
                    {
                        ret = SQLExecDirect( m_hstmt, qry4, SQL_NTS );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, m_hstmt );
                            result = 1;
                        }
                    }
                }
                else if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 1, m_hstmt );
                    result = 1;
                }
            }
            else if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 1, m_hstmt );
                result = 1;
            }
        }
    }
    delete[] qry1;
    qry1 = NULL;
    delete[] qry2;
    qry2 = NULL;
    delete[] qry3;
    qry3 = NULL;
    delete[] qry4;
    qry4 = NULL;
*/    return result;
}

int ODBCDatabase::DropForeignKey(std::wstring &command, const DatabaseTable &tableName, const std::wstring &keyName, bool logOnly, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query;
    query = L"ALTER TABLE ";
    query += tableName.GetSchemaName() + L"." + tableName.GetTableName() + L" ";
    query += L"DROP CONSTRAINT " + keyName + L" ";
    if( !logOnly )
    {
        SQLRETURN ret;
        SQLWCHAR *qry = nullptr;
        ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
        if( !result )
        {
            qry = new SQLWCHAR[query.length() + 2];
            memset( qry, '\0', query.size() + 2 );
            uc_to_str_cpy( qry, query );
            ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
        }
        if( !result )
        {
            bool found = false;
            std::map<unsigned long, std::vector<FKField *> > fKeys = const_cast<DatabaseTable &>( tableName ).GetForeignKeyVector();
            for( std::map<unsigned long, std::vector<FKField *> >::iterator it = fKeys.begin(); it != fKeys.end() && !found; ++it )
                for( std::vector<FKField *>::iterator it1 = (*it).second.begin(); it1 != (*it).second.end() && !found;  )
                {
                    if( (*it1)->GetFKName() == keyName )
                    {
                        found = true;
                        delete (*it1);
                        (*it1) = nullptr;
                        it1 = (*it).second.erase( it1 );
                    }
                    else
                        ++it1;
                }
        }
        if( result == 1 )
        {
            result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        }
        else
        {
            result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        }
        if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        m_hstmt = 0;
        delete[] qry;
        qry = nullptr;
    }
    else
    {
        command = query;
    }
    return result;
}

int ODBCDatabase::NewTableCreation(std::vector<std::wstring> &errorMsg)
{
    SQLHDBC dbc;
    SQLWCHAR outConnect[1024];
    SQLWCHAR *command = new SQLWCHAR[1024];
    int result = 0, ret;
    if( !m_isConnected )
        return result;
    SQLSMALLINT OutConnStrLen;
    SQLHSTMT stmt = 0;
    std::wstring query, query1, strCommand;
    unsigned int count = 0;
    auto bufferSize = 1024;
    if( pimpl.m_connectString[0] == '\0' )
        return 0;
    if( pimpl.m_subtype == L"Microsoft SQL Server" )
    {
        query = L"SELECT TOP 1 convert(xml, message_body) FROM EventNotificationQueue;";
        query1 = L"RECEIVE TOP( 1 ) convert(xml, message_body) FROM EventNotificationQueue;";
    }
    if( pimpl.m_subtype == L"PostgreSQL" || pimpl.m_subtype == L"MySQL" )
    {
        query = L"SELECT count(*) FROM information_schema.tables";
        query1 = L"SELECT table_schema, table_name FROM information_schema.table ;";
    }
    if( pimpl.m_subtype == L"Oracle" )
    {
        query = L"SELECT count(*) FROM all_tables;";
        query1 = L"SELECT owner, table_name FROM all_tables;";
    }
    ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &dbc );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, ENV_ERROR );
        result = 1;
    }
    ret = SQLSetConnectAttr( m_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)50, 0 );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
    {
        GetErrorMessage( errorMsg, 2 );
        result = 1;
    }
    if( !result )
    {
        ret = SQLDriverConnect( dbc, m_handle, m_connectString, SQL_NTS, outConnect, 1024, &OutConnStrLen, SQL_DRIVER_NOPROMPT );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
        {
            GetErrorMessage( errorMsg, CONN_ERROR, dbc );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLAllocHandle( SQL_HANDLE_STMT, dbc, &stmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR, dbc );
            result = 1;
        }
    }
    if( !result )
    {
        SQLWCHAR *qry = new SQLWCHAR[query.length() + 2];
        memset( qry, '\0', query.length() + 2 );
        uc_to_str_cpy( qry, query );
        if( pimpl.m_subtype == L"Microsoft SQL Server" )
        {
            ret = SQLExecDirect( stmt, qry, SQL_NTS );
            delete[] qry;
            qry = nullptr;
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                result = 1;
            }
            if( !result )
            {
                ret = SQLFetch( stmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA  )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                    result = 1;
                }
            }
            if( !result && ret != SQL_NO_DATA )
            {
                SQLLEN indicator;
                ret = SQLGetData( stmt, 1, SQL_C_WCHAR, command, 1024, &indicator );
                while( ret == SQL_SUCCESS_WITH_INFO )
                {
                    str_to_uc_cpy( strCommand, command );
                    if( indicator == SQL_NO_TOTAL )
                        indicator = 1024;
                    memset( command, '\0', 1024 );
                    ret = SQLGetData( stmt, 1, SQL_C_WCHAR, command, 1024, &indicator );
                    count++;
                }
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                    result = 1;
                }
                delete[] command;
                command = nullptr;
            }
            if( !result )
            {
                ret = SQLFreeHandle( SQL_HANDLE_STMT, stmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                    result = 1;
                }
            }
            if( !result )
            {
                ret = SQLAllocHandle( SQL_HANDLE_STMT, dbc, &stmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                    result = 1;
                }
            }
            if( !result )
            {
                qry = new SQLWCHAR[query1.length() + 2];
                memset( qry, '\0', query1.length() + 2 );
                uc_to_str_cpy( qry, query1 );
                ret = SQLExecDirect( stmt, qry, SQL_NTS );
                delete[] qry;
                qry = nullptr;
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                    result = 1;
                }
            }
        }
/*        else
        {
            ret = SQLPrepare( stmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR, stmt );
            result = 1;
        }
        if( !result )
        {
            ret = SQLExecute( stmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                result = 1;
            }
        }
        if( !result )
        {
            SQLLEN indicator;
            ret = SQLBindCol( stmt, 1, SQL_C_LONG, &count, 0, &indicator );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                result = 1;
            }
        }
        if( !result )
        {
            ret = SQLFetch( stmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                result = 1;
            }
        }
        delete[] qry;
        qry = nullptr;
        if( result == 1 )
        {
            result = SQLEndTran( SQL_HANDLE_DBC, dbc, SQL_ROLLBACK );
        }
        else
        {
            result = SQLEndTran( SQL_HANDLE_DBC, dbc, SQL_ROLLBACK );
        }
        if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR, dbc );
            result = 1;
        }
        ret = SQLFreeHandle( SQL_HANDLE_STMT, stmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR, stmt );
            result = 1;
        }
        ret = 0;
        if( count != m_numOfTables )
        {
            ret = SQLAllocHandle( SQL_HANDLE_STMT, dbc, &stmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, CONN_ERROR, dbc );
                result = 1;
            }
            else
            {
                SQLTablesDataBinding *catalog = (SQLTablesDataBinding *) malloc( 5 * sizeof( SQLTablesDataBinding ) );
                SQLWCHAR *catalogName = nullptr, *schemaName = nullptr, *tableName = nullptr;
                std::wstring cat, schema, table;
                auto catalogDB = new SQLWCHAR[pimpl.m_dbName.length() + 2];
                memset( catalogDB, '\0', pimpl.m_dbName.length() + 2 );
                uc_to_str_cpy( catalogDB, pimpl.m_dbName );
                for( int i = 0; i < 5; i++ )
                {
                    catalog[i].TargetType = SQL_C_WCHAR;
                    catalog[i].BufferLength = ( bufferSize + 1 );
                    catalog[i].TargetValuePtr = malloc( sizeof( unsigned char ) * catalog[i].BufferLength );
                    ret = SQLBindCol( stmt, (SQLUSMALLINT) i + 1, catalog[i].TargetType, catalog[i].TargetValuePtr, catalog[i].BufferLength, &( catalog[i].StrLen_or_Ind ) );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                        result = 1;
                        break;
                    }
                }
                if( !result )
                {
                    std::map<std::wstring, std::vector<TableDefinition> > temp;
                    ret = SQLTables( stmt, catalogDB, SQL_NTS, NULL, 0, NULL, 0, NULL, 0 );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                        result = 1;
                    }
                    else
                    {
                        for( ret = SQLFetch( stmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( stmt ) )
                        {
                            if( catalog[0].StrLen_or_Ind != SQL_NULL_DATA )
                                catalogName = (SQLWCHAR *) catalog[0].TargetValuePtr;
                            if( catalog[1].StrLen_or_Ind != SQL_NULL_DATA )
                                schemaName = (SQLWCHAR *) catalog[1].TargetValuePtr;
                            if( catalog[2].StrLen_or_Ind != SQL_NULL_DATA )
                                tableName = (SQLWCHAR *) catalog[2].TargetValuePtr;
                            cat = L"";
                            schema = L"";
                            table = L"";
                            str_to_uc_cpy( cat, catalogName );
                            str_to_uc_cpy( schema, schemaName );
                            str_to_uc_cpy( table, tableName );
                            if( schema == L"" && cat != L"" )
                            {
                                schema = cat;
                                copy_uc_to_uc( schemaName, catalogName );
                            }
                            schema = cat + L"." + schema;
                            if( pimpl.m_tableDefinitions.find( cat ) != pimpl.m_tableDefinitions.end() )
                                temp[cat].push_back( TableDefinition( cat, schema, table ) );
                            count++;
                        }
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            std::lock_guard<std::mutex> locker( GetTableVector().my_mutex );
                            pimpl.m_tableDefinitions.clear();
                            pimpl.m_tableDefinitions = temp;
                            m_numOfTables = count;
                        }
                        if( result == 1 )
                        {
                            result = SQLEndTran( SQL_HANDLE_DBC, dbc, SQL_ROLLBACK );
                        }
                        else
                        {
                            result = SQLEndTran( SQL_HANDLE_DBC, dbc, SQL_ROLLBACK );
                        }
                        if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, CONN_ERROR, dbc );
                            result = 1;
                        }
                        ret = SQLFreeHandle( SQL_HANDLE_STMT, stmt );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR, stmt );
                            result = 1;
                        }
                        stmt = 0;
                    }
                    for( int i = 0; i < 5; i++ )
                    {
                        free( catalog[i].TargetValuePtr );
                         catalog[i].TargetValuePtr = nullptr;
                    }
                    free( catalog );
                    catalog = nullptr;
                }
                delete[] catalogDB;
                catalogDB = nullptr;
            }
        }*/
    }
    if( stmt != 0 )
    {
        ret = SQLFreeHandle( SQL_HANDLE_STMT, stmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR, stmt );
            result = 1;
        }
        stmt = 0;
    }
    ret = SQLDisconnect( dbc );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR, dbc );
        result = 1;
    }
    ret = SQLFreeHandle( SQL_HANDLE_DBC, dbc );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR, dbc );
        result = 1;
    }
    delete[] command;
    command = nullptr;
    return result;
}

int ODBCDatabase::AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, bool tableAdded, std::vector<std::wstring> &errors)
{
    SQLRETURN ret;
    SQLWCHAR *table_name = new SQLWCHAR[tableName.length() + 2], *schema_name = new SQLWCHAR[schemaName.length() + 2], *catalog_name = new SQLWCHAR[catalog.size() + 2];
    std::wstring owner;
    std::vector<std::wstring> autoinc_fields, indexes;
    SQLWCHAR **columnNames = nullptr;
    std::wstring query4;
    SQLWCHAR *qry = nullptr;
    int result = 0;
    std::vector<TableField *> fields;
    std::wstring fieldName, fieldType, defaultValue, primaryKey, fkSchema, fkName, fkTable, origSchema, origTable, origCol, refSchema, refTable, refCol;
    std::vector<std::wstring> pk_fields, fk_fieldNames;
    std::map<unsigned long,std::vector<FKField *> > foreign_keys;
    SQLWCHAR szColumnName[256], szTypeName[256], szRemarks[256], szColumnDefault[256], szIsNullable[256], pkName[SQL_MAX_COLUMN_NAME_LEN + 1];
    SQLWCHAR szFkTable[SQL_MAX_COLUMN_NAME_LEN + 1], szPkCol[SQL_MAX_COLUMN_NAME_LEN + 1], szPkTable[SQL_MAX_COLUMN_NAME_LEN + 1], szPkSchema[SQL_MAX_COLUMN_NAME_LEN + 1], szFkTableSchema[SQL_MAX_SCHEMA_NAME_LEN + 1], szFkCol[SQL_MAX_COLUMN_NAME_LEN + 1], szFkName[256], szFkCatalog[SQL_MAX_CATALOG_NAME_LEN + 1];
    SQLSMALLINT updateRule = 0, deleteRule = 0, keySequence = 0;
    SQLLEN cbPkCol, cbFkTable, cbFkCol, cbFkName, cbFkSchemaName, cbUpdateRule, cbDeleteRule, cbKeySequence, cbFkCatalog, fkNameLength = 256;
    SQLLEN cbColumnName, cbDataType, cbTypeName, cbColumnSize, cbBufferLength, cbDecimalDigits, cbNumPrecRadix, pkLength;
    SQLLEN cbNullable, cbRemarks, cbColumnDefault, cbSQLDataType, cbDatetimeSubtypeCode, cbCharOctetLength, cbOrdinalPosition, cbIsNullable;
    SQLSMALLINT DataType, DecimalDigits = 0, NumPrecRadix, Nullable = 0, SQLDataType, DatetimeSubtypeCode, numCols = 0;
    SQLINTEGER ColumnSize = 0, BufferLength, CharOctetLength, OrdinalPosition;
    if( pimpl.m_subtype == L"PostgreSQL" )
        query4 = L"SELECT indexname FROM " + catalog + L".pg_catalog.pg_indexes WHERE tablename = ? AND schemaname = ?";
    if( pimpl.m_subtype == L"MySQL" )
        query4 = L"SELECT index_name FROM information_schema.statistics WHERE table_name = ? AND table_schema = ? AND table_catalog = ?;";
    if( pimpl.m_subtype == L"Microsoft SQL Server" )
        query4 = L"SELECT i.name FROM " + catalog + L".sys.indexes i, " + catalog + L".sys.tables t WHERE i.object_id = t.object_id AND SCHEMA_NAME(t.schema_id) = ? AND t.name = ?;";
    if( pimpl.m_subtype == L"Sybase" || pimpl.m_subtype == L"ASE" )
        query4 = L"SELECT o.name, i.name FROM sysobjects o, sysindexes i, sysusers u WHERE o.id = i.id AND o.uid = u.uid AND u.name = ? AND o.name= ?";
    if( pimpl.m_subtype == L"Sybase SQL Anywhere" )
        query4 = L"SELECT iname FROM sysindexes WHERE tname = ?";
    if( pimpl.m_subtype == L"Oracle" )
        query4 = L"SELECT index_name FROM all_indexes WHERE table_name = UPPER(?)";
    std::wstring schema, table;
    auto pos = schemaName.rfind( '.' );
    if( pos != std::wstring::npos )
        schema = schemaName.substr( pos + 1 );
    else
        schema = schemaName;
    pos = tableName.rfind( '.' );
    if( pos != std::wstring::npos )
        table = tableName.substr( pos + 1 );
    else
        table = tableName;
    memset( table_name, '\0', table.length() + 2 );
    memset( schema_name, '\0', schema.length() + 2 );
    memset( catalog_name, '\0', catalog.length() + 2 );
    uc_to_str_cpy( table_name, table );
    uc_to_str_cpy( schema_name, schemaName );
    uc_to_str_cpy( catalog_name, catalog );
    if( tableAdded )
    {
        if( GetTableOwner( catalog, schemaName, tableName, owner, errors ) )
        {
            result = 1;
            ret = SQL_ERROR;
        }
        else
        {
            if( schemaName != L"\"sys\"" )
            {
                ret = SQLAllocHandle(  SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errors, STMT_ERROR );
                    result = 1;
                }
                if( !result )
                {
                    std::wstring name;
                    std::wstring query = L"SELECT * FROM ";
                    if( catalog.rfind( '.' ) == std::wstring::npos )
                        name = catalog;
                    else
                        name = L"\"" + catalog + L"\'";
                    query += name + L".";
                    if( schemaName.rfind( '.' ) == std::wstring::npos )
                        name = schemaName;
                    else
                        name = L"\"" + schemaName + L"\'";
                    query += name + L".";
                    pos = tableName.rfind( '.' );
                    if( pos != std::wstring::npos )
                        name = tableName.substr( pos + 1 );
                    else
                        name = tableName;
                    query += name;
                    SQLWCHAR *szTableName = new SQLWCHAR[query.size() + 2];
                    memset( szTableName, '\0', query.size() + 2 );
                    uc_to_str_cpy( szTableName, query );
                    ret = SQLExecDirect( m_hstmt, szTableName, SQL_NTS );
                    delete[] szTableName;
                    szTableName = nullptr;
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errors, STMT_ERROR );
                        result = 1;
                    }
                    if( !result )
                    {
                        SQLSMALLINT lenUsed;
                        SQLSMALLINT bufferSize = 1024;
                        ret = SQLNumResultCols( m_hstmt, &numCols );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            result = 1;
                        }
                        if( !result )
                        {
                            columnNames = new SQLWCHAR *[numCols];
                            SQLLEN autoincrement;
                            for( unsigned short i = 0; i < numCols; i++ )
                            {
                                autoincrement = 0;
                                columnNames[i] = new SQLWCHAR[sizeof( SQLWCHAR ) * SQL_MAX_COLUMN_NAME_LEN + 1];
                                ret = SQLColAttribute( m_hstmt, i + 1, SQL_DESC_LABEL, columnNames[i], bufferSize, &lenUsed, NULL );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errors, STMT_ERROR );
                                    result = 1;
                                    autoinc_fields.clear();
                                    break;
                                }
                                if( result )
                                    break;
                                ret = SQLColAttribute( m_hstmt, (SQLUSMALLINT) i + 1, SQL_DESC_AUTO_UNIQUE_VALUE, NULL, 0, NULL, &autoincrement );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errors, STMT_ERROR );
                                    result = 1;
                                    autoinc_fields.clear();
                                    break;
                                }
                                if( autoincrement == SQL_TRUE )
                                {
                                    std::wstring colName;
                                    str_to_uc_cpy( colName, columnNames[i] );
                                    autoinc_fields.push_back( colName );
                                }
                                delete[] columnNames[i];
                            }
                            delete[] columnNames;
                            if( result == 1 )
                            {
                                result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                            }
                            else
                            {
                                result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                            }
                            if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errors, CONN_ERROR );
                                result = 1;
                            }
                            ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errors, STMT_ERROR );
                                result = 1;
                                autoinc_fields.clear();
                            }
                            m_hstmt = 0;
                        }
                    }
                }
            }
            if( !result )
            {
                ret = SQLAllocHandle(  SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errors, CONN_ERROR );
                    autoinc_fields.clear();
                    result = 1;
                }
                if( !result )
                {
                    ret = SQLBindCol( m_hstmt, 2, SQL_C_WCHAR, szPkSchema, SQL_MAX_COLUMN_NAME_LEN + 1, &cbPkCol );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errors, STMT_ERROR );
                        autoinc_fields.clear();
                        result = 1;
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 3, SQL_C_WCHAR, szPkTable, SQL_MAX_COLUMN_NAME_LEN + 1, &cbPkCol );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 4, SQL_C_WCHAR, szPkCol, SQL_MAX_COLUMN_NAME_LEN + 1, &cbPkCol );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 5, SQL_C_WCHAR, szFkCatalog, SQL_MAX_CATALOG_NAME_LEN + 1, &cbFkCatalog );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 6, SQL_C_WCHAR, szFkTableSchema, SQL_MAX_TABLE_NAME_LEN + 1, &cbFkSchemaName );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 7, SQL_C_WCHAR, szFkTable, SQL_MAX_TABLE_NAME_LEN + 1, &cbFkTable );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 8, SQL_C_WCHAR, szFkCol, SQL_MAX_COLUMN_NAME_LEN + 1, &cbFkCol );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 9, SQL_C_SSHORT, &keySequence, SQL_MAX_TABLE_NAME_LEN + 1, &cbKeySequence );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 10, SQL_C_SSHORT, &updateRule, SQL_MAX_COLUMN_NAME_LEN + 1, &cbUpdateRule );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 11, SQL_C_SSHORT, &deleteRule, SQL_MAX_COLUMN_NAME_LEN + 1, &cbDeleteRule );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 12, SQL_C_WCHAR, szFkName, fkNameLength, &cbFkName );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        if( pimpl.m_subtype != L"Oracle" )
                            ret = SQLForeignKeys( m_hstmt, NULL, 0, NULL, 0, NULL, 0, catalog_name, SQL_NTS, schema_name, SQL_NTS, table_name, SQL_NTS );
                        else
                            ret = SQLForeignKeys( m_hstmt, NULL, 0, NULL, 0, NULL, 0, NULL, SQL_NTS, schema_name, SQL_NTS, table_name, SQL_NTS );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                        else
                        {
                            bool fkFound = false;
                            std::map<int, std::vector<std::wstring> >origFields, refFields;
                            memset( szFkName, '\0', fkNameLength );
                            for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( m_hstmt ) )
                            {
                                str_to_uc_cpy( origCol, szPkCol );
                                str_to_uc_cpy( refCol, szFkCol );
                                origFields[keySequence].push_back( origCol );
                                refFields[keySequence].push_back( refCol );
                                fkFound = true;
                                origCol = L"";
                                refCol = L"";
                            }
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                            {
                                GetErrorMessage( errors, STMT_ERROR );
                                autoinc_fields.clear();
                                origFields.clear();
                                refFields.clear();
                                result = 1;
                            }
                            else if( fkFound )
                            {
                                ret = SQLCloseCursor( m_hstmt );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errors, STMT_ERROR );
                                    autoinc_fields.clear();
                                    origFields.clear();
                                    refFields.clear();
                                    result = 1;
                                }
                                else
                                {
                                    if( pimpl.m_subtype != L"Oracle" )
                                        ret = SQLForeignKeys( m_hstmt, NULL, 0, NULL, 0, NULL, 0, catalog_name, SQL_NTS, schema_name, SQL_NTS, table_name, SQL_NTS );
                                    else
                                        ret = SQLForeignKeys( m_hstmt, NULL, 0, NULL, 0, NULL, 0, NULL, SQL_NTS, schema_name, SQL_NTS, table_name, SQL_NTS );
                                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                    {
                                        GetErrorMessage( errors, STMT_ERROR );
                                        autoinc_fields.clear();
                                        origFields.clear();
                                        refFields.clear();
                                        result = 1;
                                    }
                                    else
                                    {
                                        for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( m_hstmt ) )
                                        {
                                            FK_ONUPDATE update_constraint = NO_ACTION_UPDATE;
                                            FK_ONDELETE delete_constraint = NO_ACTION_DELETE;
                                            if( szFkName[0] == '\0' )
                                                fkName = L"";
                                            else
                                                str_to_uc_cpy( fkName, szFkName );
                                            str_to_uc_cpy( origSchema, schema_name );
                                            str_to_uc_cpy( origTable, table_name );
                                            str_to_uc_cpy( origCol, szPkCol );
                                            str_to_uc_cpy( refSchema, szPkSchema );
                                            str_to_uc_cpy( refTable, szPkTable );
                                            str_to_uc_cpy( refCol, szFkCol );
                                            switch( updateRule )
                                            {
                                                case SQL_NO_ACTION:
                                                case SQL_RESTRICT:
                                                    update_constraint = NO_ACTION_UPDATE;
                                                    break;
                                                case SQL_SET_NULL:
                                                    update_constraint = SET_NULL_UPDATE;
                                                    break;
                                                case SQL_SET_DEFAULT:
                                                    update_constraint = SET_DEFAULT_UPDATE;
                                                    break;
                                                case SQL_CASCADE:
                                                    update_constraint = CASCADE_UPDATE;
                                                    break;
                                            }
                                            if( pimpl.m_subtype == L"Microsoft SQL Server" && updateRule == SQL_RESTRICT )
                                                update_constraint = NO_ACTION_UPDATE;
                                            switch( deleteRule )
                                            {
                                                case SQL_NO_ACTION:
                                                case SQL_RESTRICT:
                                                    delete_constraint = NO_ACTION_DELETE;
                                                    break;
                                                case SQL_SET_NULL:
                                                    delete_constraint = SET_NULL_DELETE;
                                                    break;
                                                case SQL_SET_DEFAULT:
                                                    delete_constraint = SET_DEFAULT_DELETE;
                                                    break;
                                                case SQL_CASCADE:
                                                    delete_constraint = CASCADE_DELETE;
                                                    break;
                                            }
                                            if( pimpl.m_subtype == L"Microsoft SQL Server" && deleteRule == SQL_RESTRICT )
                                                delete_constraint = NO_ACTION_DELETE;
                                                                                             //id,         name,   orig_schema,  table_name,  orig_field,  ref_schema, ref_table, ref_field, update_constraint, delete_constraint
                                            foreign_keys[keySequence].push_back( new FKField( keySequence, fkName, origSchema, origTable, origCol, refSchema,  refTable,  refCol, origFields[keySequence], refFields[keySequence], update_constraint, delete_constraint ) );
                                            fk_fieldNames.push_back( origCol );
                                            primaryKey = L"";
                                            fkSchema = L"";
                                            fkTable = L"";
                                            fkName = L"";
                                            memset( szFkName, '\0', fkNameLength );
                                            memset( szFkTable, '\0', SQL_MAX_TABLE_NAME_LEN + 1 );
                                            memset( szPkTable, '\0', SQL_MAX_TABLE_NAME_LEN + 1 );
                                            origSchema = L"";
                                            origTable = L"";
                                            origCol = L"";
                                            refSchema = L"";
                                            refTable = L"";
                                            refCol = L"";
                                        }
                                    }
                                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                                    {
                                        GetErrorMessage( errors, STMT_ERROR );
                                        autoinc_fields.clear();
                                        origFields.clear();
                                        refFields.clear();
                                        result = 1;
                                    }
                                }
                            }
                        }
                        if( result == 1 )
                        {
                            result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                        }
                        else
                        {
                            result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                        }
                        if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, CONN_ERROR );
                            result = 1;
                        }
                        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, CONN_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                    }
                }
            }
            if( !result )
            {
                ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errors, CONN_ERROR );
                    autoinc_fields.clear();
                    result = 1;
                }
                else
                {
                    ret = SQLBindCol( m_hstmt, 4, SQL_C_WCHAR, pkName, SQL_MAX_COLUMN_NAME_LEN + 1, &pkLength );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errors, STMT_ERROR );
                        autoinc_fields.clear();
                        result = 1;
                    }
                    if( !result )
                    {
                        if( pimpl.m_subtype != L"Oracle" )
                            ret = SQLPrimaryKeys( m_hstmt, catalog_name, SQL_NTS, schema_name, SQL_NTS, table_name, SQL_NTS );
                        else
                            ret = SQLPrimaryKeys( m_hstmt, NULL, SQL_NTS, schema_name, SQL_NTS, table_name, SQL_NTS );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            result = 1;
                        }
                        if( !result )
                        {
                            for( ret = SQLFetch( m_hstmt ); ( ret != SQL_SUCCESS || ret != SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( m_hstmt ) )
                            {
                                std::wstring pkFieldName;
                                str_to_uc_cpy( pkFieldName, pkName );
                                pk_fields.push_back( pkFieldName );
                            }
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                            {
                                GetErrorMessage( errors, STMT_ERROR );
                                autoinc_fields.clear();
                                pk_fields.clear();
                                result = 1;
                            }
                            if( result == 1 )
                            {
                                result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                            }
                            else
                            {
                                result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                            }
                            if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errors, CONN_ERROR );
                                result = 1;
                            }
                            ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errors, STMT_ERROR );
                                autoinc_fields.clear();
                                pk_fields.clear();
                                result = 1;
                            }
                            else
                            {
                                m_hstmt = 0;
                            }
                        }
                    }
                }
            }
            if( !result )
            {
                ret = SQLAllocHandle(  SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errors, CONN_ERROR );
                    autoinc_fields.clear();
                    pk_fields.clear();
                    result = 1;
                }
                if( !result )
                {
                    SQLSMALLINT numColumns;
                    if( pimpl.m_subtype != L"Oracle" )
                        ret = SQLColumns( m_hstmt, catalog_name, SQL_NTS, schema_name, SQL_NTS, table_name, SQL_NTS, NULL, 0);
                    else
                        ret = SQLColumns( m_hstmt, NULL, SQL_NTS, schema_name, SQL_NTS, table_name, SQL_NTS, NULL, 0 );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errors, STMT_ERROR );
                        autoinc_fields.clear();
                        pk_fields.clear();
                        result = 1;
                    }
                    if( !result )
                    {
                        ret = SQLNumResultCols( m_hstmt, &numColumns );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 4, SQL_C_WCHAR, szColumnName, 256, &cbColumnName );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 5, SQL_C_SSHORT, &DataType, 0, &cbDataType );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 6, SQL_C_WCHAR, szTypeName, 256, &cbTypeName );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 7, SQL_C_SLONG, &ColumnSize, 0, &cbColumnSize );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 8, SQL_C_SLONG, &BufferLength, 0, &cbBufferLength );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 9, SQL_C_SSHORT, &DecimalDigits, 0, &cbDecimalDigits );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 10, SQL_C_SSHORT, &NumPrecRadix, 0, &cbNumPrecRadix );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 11, SQL_C_SSHORT, &Nullable, 0, &cbNullable );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        ret = SQLBindCol( m_hstmt, 12, SQL_C_WCHAR, szRemarks, 256, &cbRemarks );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            result = 1;
                        }
                    }
                    if( !result && ( pimpl.m_subtype != L"Sybase SQL Anywhere" || ( pimpl.m_subtype == L"Sybase SQL Anywhere" && pimpl.m_versionMajor > 6 ) ) )
                    {
                        ret = SQLBindCol( m_hstmt, 13, SQL_C_WCHAR, szColumnDefault, 256, &cbColumnDefault );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result  && ( pimpl.m_subtype != L"Sybase SQL Anywhere" || ( pimpl.m_subtype == L"Sybase SQL Anywhere" && pimpl.m_versionMajor > 6 ) ) )
                    {
                        ret = SQLBindCol( m_hstmt, 14, SQL_C_SSHORT, &SQLDataType, 0, &cbSQLDataType );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result  && ( pimpl.m_subtype != L"Sybase SQL Anywhere" || ( pimpl.m_subtype == L"Sybase SQL Anywhere" && pimpl.m_versionMajor > 6 ) ) )
                    {
                        ret = SQLBindCol( m_hstmt, 15, SQL_C_SSHORT, &DatetimeSubtypeCode, 0, &cbDatetimeSubtypeCode );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result  && ( pimpl.m_subtype != L"Sybase SQL Anywhere" || ( pimpl.m_subtype == L"Sybase SQL Anywhere" && pimpl.m_versionMajor > 6 ) ) )
                    {
                        ret = SQLBindCol( m_hstmt, 16, SQL_C_SLONG, &CharOctetLength, 0, &cbCharOctetLength );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result  && ( pimpl.m_subtype != L"Sybase SQL Anywhere" || ( pimpl.m_subtype == L"Sybase SQL Anywhere" && pimpl.m_versionMajor > 6 ) ) )
                    {
                        ret = SQLBindCol( m_hstmt, 17, SQL_C_SLONG, &OrdinalPosition, 0, &cbOrdinalPosition );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result  && ( pimpl.m_subtype != L"Sybase SQL Anywhere" || ( pimpl.m_subtype == L"Sybase SQL Anywhere" && pimpl.m_versionMajor > 6 ) ) )
                    {
                        ret = SQLBindCol( m_hstmt, 18, SQL_C_WCHAR, szIsNullable, 256, &cbIsNullable );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            result = 1;
                        }
                    }
                    if( !result )
                    {
                        int i = 0;
                        for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( m_hstmt ) )
                        {
                            str_to_uc_cpy( fieldName, szColumnName );
                            str_to_uc_cpy( fieldType, szTypeName );
                            str_to_uc_cpy( defaultValue, szColumnDefault );
                            std::wstring tempTableName;
                            str_to_uc_cpy( tempTableName, table_name );
                            TableField *field = new TableField( fieldName, fieldType, ColumnSize, DecimalDigits, catalog + L"." + schemaName + L"" + tempTableName + L"." + fieldName, defaultValue, Nullable == 1, std::find( autoinc_fields.begin(), autoinc_fields.end(), fieldName ) != autoinc_fields.end() ? true : false, std::find( pk_fields.begin(), pk_fields.end(), fieldName ) != pk_fields.end(), std::find( fk_fieldNames.begin(), fk_fieldNames.end(), fieldName ) != fk_fieldNames.end() );
/*                                    if( GetFieldProperties( fieldName.c_str(), schemaName, odbc_pimpl->m_currentTableOwner, szColumnName, field, errorMsg ) )
                                    {
                                        GetErrorMessage( errorMsg, 2 );
                                        result = 1;
                                        break;
                                    }*/
                            fields.push_back( field );
                            fieldName = L"";
                            fieldType = L"";
                            defaultValue = L"";
                            i++;
                        }
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            fields.clear();
                            result = 1;
                        }
                    }
                }
                if( result == 1 )
                {
                    result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                }
                else
                {
                    result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                }
                if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errors, CONN_ERROR );
                    result = 1;
                }
                ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errors, STMT_ERROR );
                    autoinc_fields.clear();
                    pk_fields.clear();
                    fields.clear();
                    result = 1;
                }
                else
                {
                    m_hstmt = 0;
                }
            }
            if( !result )
            {
                ret = SQLAllocHandle(  SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errors, CONN_ERROR );
                    autoinc_fields.clear();
                    pk_fields.clear();
                    fields.clear();
                    result = 1;
                }
            }
            if( !result )
            {
                qry = new SQLWCHAR[query4.length() + 2];
                memset( qry, '\0', query4.length() + 2 );
                uc_to_str_cpy( qry, query4 );
                ret = SQLPrepare( m_hstmt, qry, SQL_NTS );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errors, STMT_ERROR );
                    autoinc_fields.clear();
                    pk_fields.clear();
                    fields.clear();
                    result = 1;
                }
                if( !result )
                {
                    SQLLEN cbSchemaName = SQL_NTS, cbTableName = SQL_NTS;
                    SQLULEN ParamSize;
                    ret = SQLDescribeParam( m_hstmt, 1, &DataType, &ParamSize, &DecimalDigits, &Nullable);
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errors, STMT_ERROR );
                        autoinc_fields.clear();
                        pk_fields.clear();
                        fields.clear();
                        result = 1;
                    }
                    if( !result )
                    {
                        ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, DataType, ParamSize, DecimalDigits, table_name, 0, &cbSchemaName );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errors, STMT_ERROR );
                            autoinc_fields.clear();
                            pk_fields.clear();
                            fields.clear();
                            result = 1;
                        }
                    }
                    if( pimpl.m_subtype != L"Oracle" )
                    {
                        if( !result )
                        {
                             ret = SQLDescribeParam( m_hstmt, 2, &DataType, &ParamSize, &DecimalDigits, &Nullable);
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errors, STMT_ERROR );
                                autoinc_fields.clear();
                                pk_fields.clear();
                                fields.clear();
                                result = 1;
                            }
                        }
                        if( !result )
                        {
                            ret = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, DataType, ParamSize, DecimalDigits, schema_name, 0, &cbTableName );
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errors, STMT_ERROR );
                                autoinc_fields.clear();
                                pk_fields.clear();
                                fields.clear();
                                result = 1;
                            }
                        }
                        if( !result )
                        {
                            ret = SQLExecute( m_hstmt );
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errors, STMT_ERROR );
                                autoinc_fields.clear();
                                pk_fields.clear();
                                fields.clear();
                                result = 1;
                            }
                            if( !result )
                            {
                                SQLWCHAR name[SQL_MAX_COLUMN_NAME_LEN];
                                SQLLEN cbIndexName;
                                ret = SQLBindCol( m_hstmt, 1, SQL_C_WCHAR, &name, SQL_MAX_COLUMN_NAME_LEN, &cbIndexName );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errors, STMT_ERROR );
                                    autoinc_fields.clear();
                                    pk_fields.clear();
                                    fields.clear();
                                    result = 1;
                                }
                                if( !result )
                                {
                                    for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( m_hstmt ) )
                                    {
                                        std::wstring temp;
                                        str_to_uc_cpy( temp, name );
                                        indexes.push_back( temp );
                                    }
                                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                                    {
                                        GetErrorMessage( errors, STMT_ERROR );
                                        autoinc_fields.clear();
                                        pk_fields.clear();
                                        fields.clear();
                                        indexes.clear();
                                        result = 1;
                                    }
                                }
                            }
                        }
                    }
                }
                if( result == 1 )
                {
                    result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                }
                else
                {
                    result = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                }
                if( result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errors, CONN_ERROR );
                    result = 1;
                }
                ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errors, STMT_ERROR );
                    autoinc_fields.clear();
                    pk_fields.clear();
                    fields.clear();
                    indexes.clear();
                    result = 1;
                }
                else
                {
                    m_hstmt = 0;
                }
            }
            delete[] qry;
            qry = nullptr;
            if( !result )
            {
                std::wstring tempSchemaName, tempTableName;
                str_to_uc_cpy( tempSchemaName, schema_name );
                str_to_uc_cpy( tempTableName, table_name );
                if( pimpl.m_subtype == L"Microsoft SQL Server" && tempSchemaName == L"sys" )
                    tempTableName = tempSchemaName + L"." + tempTableName;
                std::vector<std::wstring> pk;
                for( std::vector<TableField *>::iterator it = fields.begin (); it < fields.end (); ++it )
                {
                    if( (*it)->IsPrimaryKey() )
                        pk.push_back( (*it)->GetFieldName() );
                }
                DatabaseTable *new_table = new DatabaseTable( tempTableName, tempSchemaName, fields, pk, foreign_keys );
                new_table->SetTableOwner( owner );
                new_table->SetCatalog( catalog );
                new_table->SetNumberOfFields( fields.size() );
/*                if( GetTableId( new_table, errorMsg ) )
                {
                    result = 1;
                }
                else*/
                {
                    if( GetTableProperties( new_table, errors ) )
                    {
                        result = 1;
                    }
                    else
                    {
                        new_table->SetIndexNames( indexes );
                        pimpl.m_tables[catalog].push_back( new_table );
                        fields.erase( fields.begin(), fields.end() );
                        foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                        fields.clear();
                        foreign_keys.clear();
                        pk_fields.clear();
                        fk_fieldNames.clear();
                        indexes.clear();
                    }
                }
            }
        }
        delete[] table_name;
        table_name = nullptr;
        delete[] schema_name;
        schema_name = nullptr;
        delete[] catalog_name;
        catalog_name = nullptr;
    }
    else
    {
        pimpl.m_tableNames.erase( std::remove( pimpl.m_tableNames.begin(), pimpl.m_tableNames.end(), tableName ), pimpl.m_tableNames.end() );
        for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it = pimpl.m_tables.begin(); it != pimpl.m_tables.end(); it++ )
        {
            std::vector<DatabaseTable *> tableVec = (*it).second;
            for( std::vector<DatabaseTable *>::iterator it1 = tableVec.begin(); it1 < tableVec.end(); it1++ )
            {
                if( (*it1)->GetTableName() == tableName )
                {
                    delete (*it1);
                    (*it1) = nullptr;
                }
            }
            (*it).second.clear();
        }
    }
    return result;
}

void ODBCDatabase::GetConnectedUser(const std::wstring &dsn, std::wstring &connectedUser)
{
    SQLWCHAR *connectDSN = new SQLWCHAR[dsn.length() + 2];
    SQLWCHAR *entry = new SQLWCHAR[50];
    SQLWCHAR *retBuffer = new SQLWCHAR[256];
    SQLWCHAR fileName[16];
    SQLWCHAR defValue[50];
    memset( fileName, '\0', 16 );
    memset( retBuffer, '\0', 256 );
    memset( entry, '\0', 52 );
    memset( connectDSN, '\0', dsn.length() + 2 );
    memset( defValue, '\0', 50 );
    uc_to_str_cpy( fileName, L"odbc.ini" );
    uc_to_str_cpy( retBuffer, L" " );
    uc_to_str_cpy( entry, L"UserID" );
    uc_to_str_cpy( connectDSN, dsn );
    uc_to_str_cpy( defValue, L" " );
    int ret = SQLGetPrivateProfileString( connectDSN, entry, defValue, retBuffer, 256, fileName );
    if( ret < 0 )
        connectedUser = L"";
    else if( ret == 0 )
    {
        uc_to_str_cpy( entry, L"UserName" );
        ret = SQLGetPrivateProfileString( connectDSN, entry, defValue, retBuffer, 256, fileName );
        if( ret < 0 )
            connectedUser = L"";
        else if( ret == 0 )
        {
            uc_to_str_cpy( entry, L"User" );
            ret = SQLGetPrivateProfileString( connectDSN, entry, defValue, retBuffer, 256, fileName );
            if( ret < 0 )
                connectedUser = L"";
            else
                str_to_uc_cpy( connectedUser, retBuffer );
        }
        else
            str_to_uc_cpy( connectedUser, retBuffer );
    }
    else
        str_to_uc_cpy( connectedUser, retBuffer );
    delete[] connectDSN;
    delete[] entry;
    delete[] retBuffer;
}

void ODBCDatabase::GetConnectionPassword(const std::wstring &dsn, std::wstring &connectionPassword)
{
    SQLWCHAR *connectDSN = new SQLWCHAR[dsn.length() + 2];
    SQLWCHAR *entry = new SQLWCHAR[50];
    SQLWCHAR *retBuffer = new SQLWCHAR[256];
    SQLWCHAR fileName[16];
    SQLWCHAR defValue[50];
    memset( fileName, '\0', 16 );
    memset( retBuffer, '\0', 256 );
    memset( entry, '\0', 52 );
    memset( connectDSN, '\0', dsn.length() + 2 );
    memset( defValue, '\0', 50 );
    uc_to_str_cpy( fileName, L"odbc.ini" );
    uc_to_str_cpy( retBuffer, L" " );
    uc_to_str_cpy( entry, L"Password" );
    uc_to_str_cpy( connectDSN, dsn );
    uc_to_str_cpy( defValue, L" " );
    int ret = SQLGetPrivateProfileString( connectDSN, entry, defValue, retBuffer, 256, fileName );
    if( ret < 0 )
        connectionPassword = L"";
    else
        str_to_uc_cpy( connectionPassword, retBuffer );
    delete[] connectDSN;
    delete[] entry;
    delete[] retBuffer;
}

bool ODBCDatabase::IsFieldPropertiesExist (const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, std::vector<std::wstring> &errorMsg)
{
    bool exist = false;
    SQLWCHAR *qry = nullptr;
    SQLLEN cbTableName = SQL_NTS, cbOwnerName = SQL_NTS, cbFieldName = SQL_NTS;
    std::wstring query = L"SELECT 1 FROM abcatcol WHERE abc_tnam = ? AND abc_ownr = ? AND abc_cnam = ?;";
    SQLWCHAR *table_name = new SQLWCHAR[tableName.length() + 2], *owner_name = new SQLWCHAR[ownerName.length() + 2], *field_name = new SQLWCHAR[fieldName.length() + 2];
    memset( table_name, '\0', tableName.length() + 2 );
    memset( owner_name, '\0', ownerName.length() + 2 );
    memset( field_name, '\0', fieldName.length() + 2 );
    uc_to_str_cpy( table_name, tableName );
    uc_to_str_cpy( owner_name, ownerName );
    uc_to_str_cpy( field_name, fieldName );
    SQLRETURN ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        qry = new SQLWCHAR[query.length() + 2];
        memset( qry, '\0', query.length() + 2 );
        uc_to_str_cpy( qry, query );
        ret = SQLPrepare( m_hstmt, qry, SQL_NTS );
        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
        {
            ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, tableName.length(), 0, table_name, 0, &cbTableName );
            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
            {
                ret = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, ownerName.length(), 0, owner_name, 0, &cbOwnerName );
                if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                {
                    ret = SQLBindParameter( m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, fieldName.length(), 0, field_name, 0, &cbFieldName );
                    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                    {
                        ret = SQLExecute( m_hstmt );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            ret = SQLFetch( m_hstmt );
                            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                                exist = true;
                            else if( ret != SQL_NO_DATA )
                                GetErrorMessage( errorMsg, 1, m_hstmt );
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                    }
                }
                else
                {
                }
            }
            else
            {
            }
        }
        else
        {
        }
        if( ret == 1 )
        {
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        }
        else
        {
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        }
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
        }
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        m_hstmt = 0;
    }
    delete[] qry;
    qry = nullptr;
    delete[] table_name;
    table_name = nullptr;
    delete[] owner_name;
    owner_name = nullptr;
    delete[] field_name;
    field_name = nullptr;
    return exist;
}

int ODBCDatabase::GetFieldHeader(const std::wstring &tableName, const std::wstring &fieldName, std::wstring &headerStr, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    SQLWCHAR *qry = nullptr;
    SQLLEN cbTableName = SQL_NTS, cbFieldName = SQL_NTS, cbHeaderStr = SQL_NTS;
    SQLWCHAR name[SQL_MAX_COLUMN_NAME_LEN];
    headerStr = fieldName;
    SQLWCHAR *table_name = new SQLWCHAR[tableName.length() + 2], *field_name = new SQLWCHAR[fieldName.length() + 2];
    memset( table_name, '\0', tableName.length() + 2 );
    memset( field_name, '\0', fieldName.length() + 2 );
    uc_to_str_cpy( table_name, tableName );
    uc_to_str_cpy( field_name, fieldName );
    std::wstring query = L"SEECT pbc_hdr FROM \"abcatcol\" WHERE \"abc_tnam\" = ? AND \"abc_cnam\" = ?";
    SQLRETURN ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS || ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    if( !result )
    {
        qry = new SQLWCHAR[query.length() + 2];
        memset( qry, '\0', query.length() + 2 );
        uc_to_str_cpy( qry, query );
        ret = SQLPrepare( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS || ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, tableName.length(), 0, table_name, 0, &cbTableName );
        if( ret != SQL_SUCCESS || ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindParameter( m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, fieldName.length(), 0, field_name, 0, &cbFieldName );
        if( ret != SQL_SUCCESS || ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 1, SQL_C_WCHAR, &name, SQL_MAX_COLUMN_NAME_LEN, &cbHeaderStr );
        if( ret != SQL_SUCCESS || ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLExecute( m_hstmt );
        if( ret != SQL_SUCCESS || ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLFetch( m_hstmt );
        if( ret != SQL_SUCCESS || ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
    }
    if( !result )
        str_to_uc_cpy( headerStr, name );
    else
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    delete[] table_name;
    table_name = nullptr;
    delete[] field_name;
    field_name = nullptr;
    delete[] qry;
    qry = nullptr;
    if( ret == 1 )
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    else
    {
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    }
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
    }
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    m_hstmt = 0;
    return result;
}
/*
int ODBCDatabase::EditTableData(const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"SELECT * FROM " + schemaName + L"." + tableName + L";";
    SQLWCHAR *qry = new SQLWCHAR[query.length() + 2];
    memset( qry, '\0', query.length() + 2 );
    uc_to_str_cpy( qry, query );
    SQLRETURN ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
        {
            for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ); ret = SQLFetch( m_hstmt ) )
            {
                SQLSMALLINT count;
                ret = SQLNumResultCols( m_hstmt, &count );
            }
        }
    }
    delete[] qry;
    qry = nullptr;
    return result;
}
*/
int ODBCDatabase::PrepareStatement(const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"SELECT * FROM " + schemaName + L"." + tableName + L";";
    SQLWCHAR *qry = new SQLWCHAR[query.length() + 2];
    memset( qry, '\0', query.length() + 2 );
    uc_to_str_cpy( qry, query );
    SQLRETURN ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    else
    {
        ret = SQLPrepare( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        else
        {
            ret = SQLExecute( m_hstmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
        }
    }
    return result;
}

int ODBCDatabase::EditTableData(std::vector<DataEditFiield> &row, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    SQLSMALLINT columnCount, *nameLen = nullptr, *dataType = nullptr, *decimal = nullptr, *nullable = nullptr;
    SQLULEN  *columnSize = nullptr;
    SQLRETURN ret;
    SQLWCHAR *fieldString = nullptr;
    SQLWCHAR *fieldStr = nullptr;
    char *fieldBlob = nullptr;
    SQLINTEGER fieldInt;
    SQLFLOAT fieldFloat;
    SQLLEN *columnDataLen = nullptr;
    std::wstring strCol;
    std::wstring stringCol;
    if( m_fieldsInRecordSet == 0 )
    {
        ret = SQLNumResultCols( m_hstmt, &columnCount );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        else
            m_fieldsInRecordSet = columnCount;
    }
    if( !result )
    {
        ret = SQLFetch( m_hstmt );
        if( ret == SQL_NO_DATA )
        {
            result = NO_MORE_ROWS;
        }
        else if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        if( !result )
        {
            nameLen = new SQLSMALLINT[m_fieldsInRecordSet];
            dataType = new SQLSMALLINT[m_fieldsInRecordSet];
            columnSize = new SQLULEN[m_fieldsInRecordSet];
            decimal = new SQLSMALLINT[m_fieldsInRecordSet];
            nullable = new SQLSMALLINT[m_fieldsInRecordSet];
            columnDataLen = new SQLLEN[m_fieldsInRecordSet];
            for( unsigned short i = 0; i < m_fieldsInRecordSet; i++ )
            {
                SQLSMALLINT ctype;
                int valueType;
                ret = SQLDescribeCol( m_hstmt, i + 1, NULL, 0, &nameLen[i], &dataType[i], &columnSize[i], &decimal[i], &nullable[i] );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                    result = 1;
                    break;
                }
                else
                {
                    switch( dataType[i] )
                    {
                    case SQL_CHAR:
                    case SQL_VARCHAR:
                    case SQL_LONGVARCHAR:
                        ctype = SQL_C_CHAR;
                        valueType = 3;
                        fieldString = new SQLWCHAR[columnSize[i] + 2];
                        memset( fieldString, '\0', columnSize[i] + 2 );
                        ret = SQLGetData( m_hstmt, i + 1, ctype, fieldString, columnSize[i], &columnDataLen[i] );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            str_to_uc_cpy( stringCol, fieldString );
                            row.push_back( DataEditFiield( stringCol ) );
                            delete[] fieldString;
                            fieldString = nullptr;
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                        break;
                    case SQL_WCHAR:
                    case SQL_WVARCHAR:
                    case SQL_WLONGVARCHAR:
                        ctype = SQL_C_WCHAR;
                        valueType = 3;
                        fieldStr = new SQLWCHAR[columnSize[i] + 2];
                        memset( fieldStr, '\0', columnSize[i] + 2 );
                        ret = SQLGetData( m_hstmt, i + 1, ctype, fieldString, columnSize[i], &columnDataLen[i] );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            str_to_uc_cpy( strCol, fieldStr );
                            row.push_back( DataEditFiield( strCol ) );
                            delete fieldString;
                            fieldString = nullptr;
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                        break;
                    case SQL_DECIMAL:
                    case SQL_NUMERIC:
                    case SQL_SMALLINT:
                        if( dataType[i] == SQL_NUMERIC )
                            ctype = SQL_C_NUMERIC;
                        else
                            ctype = SQL_C_SSHORT;
                        valueType = 1;
                        ret = SQLGetData( m_hstmt, i + 1, ctype, &fieldInt, 0, &columnDataLen[i] );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            row.push_back( DataEditFiield( std::to_wstring( fieldInt ) ) );
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                        break;
                    case SQL_INTEGER:
                        ctype = SQL_C_SLONG;
                        valueType = 1;
                        ret = SQLGetData( m_hstmt, i + 1, ctype, &fieldInt, columnSize[i], &columnDataLen[i] );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            row.push_back( DataEditFiield( std::to_wstring( fieldInt ) ) );
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                        break;
                    case SQL_REAL:
                        ctype = SQL_C_FLOAT;
                        valueType = 2;
                        ret = SQLBindCol( m_hstmt, i + 1, ctype, &fieldFloat, columnSize[i], &columnDataLen[i] );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            row.push_back( DataEditFiield( fieldFloat, columnSize[i], decimal[i] ) );
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                        break;
                    case SQL_FLOAT:
                    case SQL_DOUBLE:
                        ctype = SQL_C_DOUBLE;
                        valueType = 2;
                        ret = SQLBindCol( m_hstmt, i + 1, ctype, &fieldFloat, columnSize[i], &columnDataLen[i] );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            row.push_back( DataEditFiield( fieldFloat, columnSize[i], decimal[i] ) );
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                        break;
                    case SQL_BIT:
                        ctype = SQL_C_BIT;
                        valueType = 3;
                        ret = SQLGetData( m_hstmt, i + 1, ctype, &fieldInt, columnSize[i], &columnDataLen[i] );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            row.push_back( DataEditFiield( std::to_wstring( fieldInt ) ) );
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                        break;
                    case SQL_TINYINT:
                        ctype = SQL_C_STINYINT;
                        valueType = 1;
                        ret = SQLGetData( m_hstmt, i + 1, ctype, &fieldInt, columnSize[i], &columnDataLen[i] );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            row.push_back( DataEditFiield( std::to_wstring( fieldInt ) ) );
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                        break;
                    case SQL_BIGINT:
                        ctype = SQL_C_SBIGINT;
                        valueType = 1;
                        ret = SQLGetData( m_hstmt, i + 1, ctype, &fieldInt, columnSize[i], &columnDataLen[i] );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            row.push_back( DataEditFiield( std::to_wstring( fieldInt ) ) );
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                        break;
                    case SQL_BINARY:
                    case SQL_VARBINARY:
                    case SQL_LONGVARBINARY:
                        ctype = SQL_C_BINARY;
                        valueType = 3;
                        fieldBlob = new char[columnSize[i]];
                        ret = SQLGetData( m_hstmt, i + 1, ctype, &fieldBlob, columnSize[i], &columnDataLen[i] );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                        break;
                    case SQL_TYPE_DATE:
                    case SQL_TYPE_TIME:
                    case SQL_TYPE_TIMESTAMP:
                    case SQL_INTERVAL_MONTH:
                    case SQL_INTERVAL_YEAR:
                    case SQL_INTERVAL_YEAR_TO_MONTH:
                    case SQL_INTERVAL_DAY:
                    case SQL_INTERVAL_HOUR:
                    case SQL_INTERVAL_MINUTE:
                    case SQL_INTERVAL_SECOND:
                    case SQL_INTERVAL_DAY_TO_HOUR:
                    case SQL_INTERVAL_DAY_TO_MINUTE:
                    case SQL_INTERVAL_DAY_TO_SECOND:
                    case SQL_INTERVAL_HOUR_TO_MINUTE:
                    case SQL_INTERVAL_HOUR_TO_SECOND:
                    case SQL_INTERVAL_MINUTE_TO_SECOND:
                        ctype = SQL_C_WCHAR;
                        valueType = 3;
                        fieldStr = new SQLWCHAR[columnSize[i] + 2];
                        memset( fieldStr, '\0', columnSize[i] + 2 );
                        ret = SQLGetData( m_hstmt, i + 1, ctype, fieldString, columnSize[i], &columnDataLen[i] );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            str_to_uc_cpy( strCol, fieldStr );
                            row.push_back( DataEditFiield( strCol ) );
                            delete fieldString;
                            fieldString = nullptr;
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, STMT_ERROR );
                            result = 1;
                            break;
                        }
                        break;
                    }
                    if( result )
                    {
                        GetErrorMessage( errorMsg, 1, m_hstmt );
                       result = 1;
                       break;
                    }
                }
            }
        }
    }
    delete[] nameLen;
    nameLen = nullptr;
    delete[] dataType;
    dataType = nullptr;
    delete[] columnSize;
    columnSize = nullptr;
    delete[] decimal;
    decimal = nullptr;
    delete[] nullable;
    nullable = nullptr;
    delete[] columnDataLen;
    columnDataLen = nullptr;
    return result;
}

int ODBCDatabase::FinalizeStatement(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    SQLRETURN ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    m_fieldsInRecordSet = 0;
    return result;
}

int ODBCDatabase::GetTableCreationSyntax(const std::wstring tableName, std::wstring &syntax, std::vector<std::wstring> &errorMsg)
{
    std::wstring query;
    if( pimpl.m_subtype == L"Microsoft SQL Server" )
        query = L"SELECT  'CREATE TABLE [' + so.name + '] (' + o.list + ')' + CASE WHEN tc.Constraint_Name IS NULL THEN '' ELSE 'ALTER TABLE ' + so.Name + ' ADD CONSTRAINT ' + tc.Constraint_Name  + ' PRIMARY KEY ' + ' (' + LEFT(j.List, Len(j.List)-1) + ')' END " \
        "from    sysobjects so" \
        "cross apply" \
        " (SELECT '  ['+column_name+'] ' + "\
         "data_type + case data_type" \
         "when 'sql_variant' then ''" \
         "when 'text' then ''" \
         "when 'ntext' then ''" \
         "when 'xml' then ''"  \
         "when 'decimal' then '(' + cast(numeric_precision as varchar) + ', ' + cast(numeric_scale as varchar) + ')'" \
            "else coalesce('('+case when character_maximum_length = -1 then 'MAX' else cast(character_maximum_length as varchar) end +')','') end + ' ' +" \
            "case when exists ( " \
            "select id from syscolumns" \
            "where object_name(id)=so.name" \
            "and name=column_name" \
                "and columnproperty(id,name,'IsIdentity') = 1 " \
                ") then" \
                "'IDENTITY(' + " \
                "cast(ident_seed(so.name) as varchar) + ',' + " \
                "cast(ident_incr(so.name) as varchar) + ')'" \
        "else ''" \
        "end + ' ' +" \
        "(case when UPPER(IS_NULLABLE) = 'NO' then 'NOT ' else '' end ) + 'NULL ' + " \
        "case when information_schema.columns.COLUMN_DEFAULT IS NOT NULL THEN 'DEFAULT '+ information_schema.columns.COLUMN_DEFAULT ELSE '' END + ', ' " \
        "from information_schema.columns where table_name = so.name "
        "order by ordinal_position" \
        "FOR XML PATH('')) o (list)" \
        "left join information_schema.table_constraints tc " \
        "on  tc.Table_name       = so.Name AND tc.Constraint_Type  = 'PRIMARY KEY'" \
        "cross apply" \
        "(select '[' + Column_Name + '], '" \
        "FROM   information_schema.key_column_usage kcu" \
        "WHERE  kcu.Constraint_Name = tc.Constraint_Name" \
        "ORDER BY" \
        "ORDINAL_POSITION  FOR XML PATH('')) j (list)" \
        "where   xtype = 'U'" \
            "AND name    NOT IN ('dtproperties')";
    if( pimpl.m_subtype == L"MySQL" )
        query = L"SHOW CREATE TABLE " + tableName;
    int result = 0;
    auto qry = new SQLWCHAR[query.length() + 2];
    memset( qry, '\0', query.size() + 2 );
    uc_to_str_cpy( qry, query );
    auto ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    else
    {
        ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        else
        {
            SQLLEN cSyntax;
            SQLSMALLINT dataTypePtr, decimalDigitsPtr, isNullable, nameLengthPtr;
            SQLULEN columnSizePtr;
            ret = SQLDescribeCol( m_hstmt, 1, NULL, 0, &nameLengthPtr, &dataTypePtr, &columnSizePtr, &decimalDigitsPtr, &isNullable );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            else
            {
                SQLWCHAR *tableCreation = new SQLWCHAR[columnSizePtr + 1];
                ret = SQLBindCol( m_hstmt, 1, SQL_C_WCHAR, &tableCreation, columnSizePtr, &cSyntax );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                    result = 1;
                }
                else
                {
                    ret = SQLFetch( m_hstmt );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                        result = 1;
                    }
                    else
                        str_to_uc_cpy( syntax, tableCreation );
                }
            }
            ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            m_hstmt = 0;
        }
    }
    delete[] qry;
    qry = NULL;
    return result;
}

int ODBCDatabase::AttachDatabase(const std::wstring &catalog, const std::wstring &, std::vector<std::wstring> &errorMsg)
{
    int result = 0, bufferSize = 1024;
    SQLHDBC hdbc;
    SQLSMALLINT OutConnStrLen;
    SQLWCHAR *catalogName = nullptr, *schemaName = nullptr, *tableName = nullptr;
    std::wstring cat, schema, table;
    SQLTablesDataBinding *catalogSQL = (SQLTablesDataBinding *) malloc( 5 * sizeof( SQLTablesDataBinding ) );
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &hdbc );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, ENV_ERROR );
        result = 1;
    }
    else
    {
        ret = SQLDriverConnect( hdbc, NULL, m_connectString, SQL_NTS, NULL, 0, &OutConnStrLen, SQL_DRIVER_NOPROMPT );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2, hdbc );
            result = 1;
        }
        else
        {
            ret = SQLAllocHandle( SQL_HANDLE_STMT, hdbc, &m_hstmt );
            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
            {
                auto catalogDB = new SQLWCHAR[pimpl.m_dbName.length() + 2];
                memset( catalogDB, '\0', pimpl.m_dbName.length() + 2 );
                uc_to_str_cpy( catalogDB, catalog );
                for( int i = 0; i < 5; i++ )
                {
                    catalogSQL[i].TargetType = SQL_C_WCHAR;
                    catalogSQL[i].BufferLength = ( bufferSize + 1 );
                    catalogSQL[i].TargetValuePtr = malloc( sizeof( unsigned char ) * catalogSQL[i].BufferLength );
                    ret = SQLBindCol( m_hstmt, (SQLUSMALLINT) i + 1, catalogSQL[i].TargetType, catalogSQL[i].TargetValuePtr, catalogSQL[i].BufferLength, &( catalogSQL[i].StrLen_or_Ind ) );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, 1 );
                        result = 1;
                        break;
                    }
                }
                if( !result )
                {
                    ret = SQLTables( m_hstmt, catalogDB, SQL_NTS, NULL, 0, NULL, 0, NULL, 0 );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, 1 );
                        result = 1;
                    }
                    else
                    {
                        for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( m_hstmt ) )
                        {
                            if( catalogSQL[0].StrLen_or_Ind != SQL_NULL_DATA )
                                catalogName = (SQLWCHAR *) catalogSQL[0].TargetValuePtr;
                            if( catalogSQL[1].StrLen_or_Ind != SQL_NULL_DATA )
                                schemaName = (SQLWCHAR *) catalogSQL[1].TargetValuePtr;
                            if( catalogSQL[2].StrLen_or_Ind != SQL_NULL_DATA )
                                tableName = (SQLWCHAR *) catalogSQL[2].TargetValuePtr;
                            cat = L"";
                            schema = L"";
                            table = L"";
                            str_to_uc_cpy( cat, catalogName );
                            str_to_uc_cpy( schema, schemaName );
                            str_to_uc_cpy( table, tableName );
                            if( schema == L"" && cat != L"" && schemaName != NULL )
                            {
                                schema = cat;
                                copy_uc_to_uc( schemaName, catalogName );
                            }
                            pimpl.m_tableDefinitions[cat].push_back( TableDefinition( cat, schema, table ) );
                        }
                    }
                }
            }
            for( int i = 0; i < 5; i++ )
            {
                free( catalogSQL[i].TargetValuePtr );
                catalogSQL[i].TargetValuePtr = nullptr;
            }
            free( catalogSQL );
            catalogSQL = nullptr;
            ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 1, m_hstmt );
                result = 1;
            }
            m_hstmt = 0;
            ret = SQLDisconnect( hdbc );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 2, hdbc );
                result = 1;
            }
            ret = SQLFreeHandle( SQL_HANDLE_DBC, hdbc );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 2, hdbc );
                result = 1;
            }
        }
    }
    return result;
}

int ODBCDatabase::GetDatabaseNameList(std::vector<std::wstring> &names, std::vector<std::wstring> &errorMsg)
{
    std::wstring query;
    SQLWCHAR *qry = nullptr;
    int result = 0;
    SQLWCHAR *dbName = nullptr;
    if( pimpl.m_subtype == L"Microsoft SQL Server" )
        query = L"SELECT name FROM sys.databases;";
    if( pimpl.m_subtype == L"PostgreSQL" )
        query = L"SELECT datname FROM pg_database;";
    if( pimpl.m_subtype == L"Sybase" || pimpl.m_subtype == L"ASE" )
        query = L"SELECT name FROM sp_helpdb";
    if( pimpl.m_subtype == L"mySQL" )
        query = L"SELECT schema_name FROM information_schema.schemata;";
    auto ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    else
    {
        qry = new SQLWCHAR[query.length() + 2];
        memset( qry, '\0', query.size() + 2 );
        uc_to_str_cpy( qry, query );
        ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        else
        {
            SQLSMALLINT nameBufLength, dataTypePtr, decimalDigitsPtr, isNullable;
            SQLULEN columnSizePtr;
            SQLLEN cDatabaseName;
            ret = SQLDescribeCol( m_hstmt, 1, NULL, 0, &nameBufLength, &dataTypePtr, &columnSizePtr, &decimalDigitsPtr, &isNullable );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            dbName = new SQLWCHAR[columnSizePtr + 1];
            if( !result )
            {
                ret = SQLBindCol( m_hstmt, 1, SQL_C_WCHAR, dbName, columnSizePtr, &cDatabaseName );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                        GetErrorMessage( errorMsg, STMT_ERROR );
                        result = 1;
                }
            }
            if( !result )
            {
                ret = SQLExecute( m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                    result = 1;
                }
            }
            if( !result )
            {
                for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ); ret = SQLFetch( m_hstmt ) )
                {
                    std::wstring databaseName;
                    str_to_uc_cpy( databaseName, dbName );
                    names.push_back( databaseName );
                }
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, STMT_ERROR );
                    result = 1;
                }
            }
            ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            m_hstmt = 0;
        }
    }
    delete[] qry;
    qry = nullptr;
    return result;
}

int ODBCDatabase::GetQueryRow(const std::wstring &query, std::vector<std::wstring> &values)
{
    auto result = 0;
    return result;
}

int ODBCDatabase::AddUpdateFormat()
{
    return 0;
}

int ODBCDatabase::PopulateValdators(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    SQLWCHAR *qry = nullptr;
    std::wstring query;
    SQLLEN colDataLen;
    SQLWCHAR name[32], rule[256], message[256];
    short int validType;
    int control;
    std::wstring validName, validRule, validMessage;
    if( pimpl.m_subtype == L"Microsoft SQL Server" )
        query = L"SELECT * FROM abcatvld;";
    if( pimpl.m_subtype == L"PostgreSQL" )
        if( pimpl.m_versionMajor >= 9 && pimpl.m_versionMinor >= 5 )
            query = L"SELECT * FROM abcatvld FOR UPDATE;";
        else
            query = L"SELECT * FROM abcatvld FOR UPDATE;";
    auto ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    if( !result )
    {
        qry = new SQLWCHAR[query.length() + 2];
        memset( qry, '\0', query.length() + 2 );
        uc_to_str_cpy( qry, query );
        ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 1, SQL_C_WCHAR, &name, 32, &colDataLen );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 2, SQL_C_WCHAR, &rule, 256, &colDataLen );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 3, SQL_C_SSHORT, &validType, 0, &colDataLen );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 4, SQL_C_SLONG, &control, 0, &colDataLen );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindCol( m_hstmt, 5, SQL_C_WCHAR, &message, 256, &colDataLen );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ); ret = SQLFetch( m_hstmt ) )
        {
            str_to_uc_cpy( validName, name );
            str_to_uc_cpy( validRule, rule );
            str_to_uc_cpy( validMessage, message );
            pimpl.m_validators.push_back( std::make_tuple( validName, validRule, validType, control, validMessage ) );
            validName = L"";
            validRule = L"";
            validMessage = L"";
        }
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
        if( !result )
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
        else
            ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
        if( !result )
        {
            ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );;
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            m_hstmt = 0;
        }
    }
    delete[] qry;
    qry = nullptr;
    return result;
}

int ODBCDatabase::CreateUpdateValidationRule(bool isNew, const std::wstring &name, const std::wstring &rule, const int type, const std::wstring &message, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring errorMessage;
    std::wstring query = L"";
    SQLLEN val1, val2, val3, val4, val5;
    if( isNew )
        query = L"INSERT INTO \"abcatvld\"(\"abv_name\", \"abv_vald\", \"abv_type\", \"abv_msg\") VALUES( ?, ?, ?, ?)";
    else
        query = L"UPDATE \"abcatvld\" SET \"abv_name\" = ?, \"abv_vald\" = ?, \"abv_type\" = ?, \"abv_msg\" = ? WHERE \"abv_name\" = ?";
    auto qry = new SQLWCHAR[query.length() + 2];
    memset( qry, '\0', query.length() + 2 );
    uc_to_str_cpy( qry, query );
    auto qryName = new SQLWCHAR[32];
    memset( qryName, '\0', 32 );
    uc_to_str_cpy( qryName, name );
    auto qryRule = new SQLWCHAR[258];
    memset( qryRule, '\0', 258 );
    uc_to_str_cpy( qryRule, rule );
    auto qryType = (int) type;
    auto qryMessage = new SQLWCHAR[256];
    memset( qryMessage, '\0', 256 );
    uc_to_str_cpy( qryMessage, message );
    auto ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, CONN_ERROR );
        result = 1;
    }
    if( !result )
    {
        ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 32, 0, qryName, 0, &val1 );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 258, 0, qryRule, 0, &val2 );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindParameter( m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_INTEGER, 0, 0, &qryType, 0, &val3 );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLBindParameter( m_hstmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 254, 0, &qryMessage, 0, &val4 );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( isNew && !result )
    {
        ret = SQLBindParameter( m_hstmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 32, 0, qryName, 0, &val5 );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLPrepare( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
    {
        ret = SQLExecute( m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
        }
    }
    if( !result )
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
    else
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, STMT_ERROR );
        result = 1;
    }
    m_hstmt = 0;
    delete[] qry;
    qry = nullptr;
    delete[] qryName;
    qryName = nullptr;
    delete[] qryRule;
    qryRule = nullptr;
    delete[] qryMessage;
    qryMessage = nullptr;
    return result;
}

int ODBCDatabase::MonitorSchemaChanges(std::vector<std::wstring> &errorMsg)
{
    std::vector<std::wstring> queries;
    auto result = 0;
    SQLRETURN ret = SQL_SUCCESS;
    if( pimpl.m_subtype == L"Microsoft SQL Server" )
    {
//        queries.push_back( L"BEGIN TRANSACTION" );
        queries.push_back( L"IF EXISTS(SELECT * FROM sys.databases WHERE name = \'" +  pimpl.m_dbName + L"\' AND is_broker_enabled = 0) ALTER DATABASE " + pimpl.m_dbName + L" SET ENABLE_BROKER" );
        queries.push_back( L"IF NOT EXISTS(SELECT * FROM sys.service_queues WHERE name = \'EventNotificationQueue\') CREATE QUEUE dbo.EventNotificationQueue" );
        queries.push_back( L"IF NOT EXISTS(SELECT * FROM sys.services WHERE name = \'//" + pimpl.m_dbName + L"/EventNotificationService\') CREATE SERVICE [//" + pimpl.m_dbName +L"/EventNotificationService] ON QUEUE dbo.EventNotificationQueue([http://schemas.microsoft.com/SQL/Notifications/PostEventNotification])" );
        queries.push_back( L"IF NOT EXISTS(SELECT * FROM sys.event_notifications WHERE name = \'SchemaChangeEventsTable\') CREATE EVENT NOTIFICATION SchemaChangeEventsTable ON DATABASE FOR DDL_TABLE_EVENTS TO SERVICE \'//" + pimpl.m_dbName + L"/EventNotificationService\' , \'current database\'" );
        queries.push_back( L"IF NOT EXISTS(SELECT * FROM sys.event_notifications WHERE name = \'SchemaChangeEventsIndex\') CREATE EVENT NOTIFICATION SchemaChangeEventsIndex ON DATABASE FOR DDL_INDEX_EVENTS TO SERVICE \'//" + pimpl.m_dbName + L"/EventNotificationService\' , \'current database\'" );
        queries.push_back( L"IF NOT EXISTS(SELECT * FROM sys.event_notifications WHERE name = \'SchemaChangeEventsView\') CREATE EVENT NOTIFICATION SchemaChangeEventsView ON DATABASE FOR DDL_VIEW_EVENTS TO SERVICE \'//" + pimpl.m_dbName + L"/EventNotificationService\' , \'current database\'" );
    }
    if( pimpl.m_subtype == L"PostgreSQL" )
    {
        if( pimpl.m_versionMajor >= 9 && pimpl.m_versionMinor > 3 )
        {
            queries.push_back( L"BEGIN" );
            queries.push_back( L"CREATE FUNCTION __watch_schema_changes() RETURNS event_trigger LANGUAGE plpgsql AS $$ BEGIN NOTIFY tg_tag; END; $$;" );
            queries.push_back( L"CREATE EVENT TRIGGER schema_change_notify ON ddl_command_end WHEN TAG IN(\'CREATE TABLE\', \'ALTER TABLE\', \'DROP TABLE\', \'CREATE INDEX\', \'DROP INDEX\') EXECUTE PROCEDURE __watch_schema_changes();" );
        }
    }
    if( pimpl.m_subtype == L"ACCESS" )
    {
        queries.push_back( L"BEGIN TRANSACTION" );
        queries.push_back( L"GRANT SELECT ON MSysObjects TO Admin;" );
    }
    for( auto it = queries.begin(); it < queries.end(); ++it )
    {
        auto qry = new SQLWCHAR[(*it).size() + 2];
        memset( qry, '\0', (*it).size() + 2 );
        uc_to_str_cpy( qry, (*it) );
        ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        delete[] qry;
        qry = nullptr;
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, STMT_ERROR );
            result = 1;
            break;
        }
    }
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
    else
        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
    return result;
}

int ODBCDatabase::GetTablespacesList(std::vector<std::wstring> &list, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    if( pimpl.m_subtype == L"PostgreSQL" )
    {
        std::wstring query = L"SELECT * FROM pg_tablespaces;";
        auto ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, CONN_ERROR );
            result = 1;
        }
        if( !result )
        {
            auto qry = new SQLWCHAR[query.size() + 2];
            memset( qry, '\0', query.size() + 2 );
            uc_to_str_cpy( qry, query );
            ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, STMT_ERROR );
                result = 1;
            }
            delete[] qry;
            qry = nullptr;
        }
    }
    else
    {
        UNUSED( list );
        UNUSED( errorMsg );
    }
    return result;
}


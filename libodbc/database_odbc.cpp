#ifdef WIN32
#include <windows.h>
#endif
#ifndef WIN32
#include <sstream>
#endif
#include <map>
#include <set>
#include <vector>
#include <string>
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

ODBCDatabase::ODBCDatabase() : Database()
{
    m_env = 0;
    m_hdbc = 0;
    m_hstmt = 0;
    pimpl = new Impl;
    pimpl->m_type = L"ODBC";
    m_oneStatement = false;
    m_connectString = NULL;
}

ODBCDatabase::~ODBCDatabase()
{
    RETCODE ret;
    std::vector<std::wstring> errorMsg;
    delete m_connectString;
    m_connectString = 0;
    if( m_hstmt != 0 )
    {
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS )
            GetErrorMessage( errorMsg, 1 );
        m_hstmt = 0;
        if( m_hdbc != 0 )
        {
            ret = SQLFreeHandle( SQL_HANDLE_DBC, m_hdbc );
            if( ret != SQL_SUCCESS )
                GetErrorMessage( errorMsg, 2 );
            m_hdbc = 0;
            if( m_env != 0 )
            {
                ret = SQLFreeHandle( SQL_HANDLE_ENV, m_env );
                if( ret != SQL_SUCCESS )
                    GetErrorMessage( errorMsg, 0 );
                m_env = 0;
            }
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
        GetErrorMessage( errMsg, 0, m_env );
    }
    else
    {
        ret = SQLSetEnvAttr( m_env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0 );
        if( ret != SQL_SUCCESS )
        {
            result = false;
            GetErrorMessage( errMsg, 0, m_env );
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
                    GetErrorMessage( errMsg, 0, m_env );
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
                GetErrorMessage( errMsg, 0, m_env );
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
        GetErrorMessage( errorMsg, 0, m_env );
        result = true;
    }
    else
    {
        ret = SQLSetEnvAttr( m_env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0 );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 0, m_env );
            result = true;
        }
        else
        {
            while( ( ret = SQLDataSources( m_env, direct, dsn, SQL_MAX_DSN_LENGTH, &pcbDSN, dsnDescr, 254, &pcbDesc ) ) == SQL_SUCCESS )
            {
                std::wstring s1;
                str_to_uc_cpy( s1, dsn );
                dsns.push_back( s1 );
                direct = SQL_FETCH_NEXT;
            }
            if( ret != SQL_SUCCESS && ret != SQL_NO_DATA )
            {
                GetErrorMessage( errorMsg, 0, m_env );
                result = true;
            }
        }
    }
    ret = SQLFreeHandle( SQL_HANDLE_ENV, m_env );
    if( ret != SQL_SUCCESS && ret != SQL_NO_DATA )
    {
        GetErrorMessage( errorMsg, 0, m_env );
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
    BOOL ret= SQLConfigDataSource( NULL, ODBC_CONFIG_DSN, temp1, temp2 );
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
    case 0:
        option = SQL_HANDLE_ENV;
        handle = m_env;
        break;
    case 1:
        option = SQL_HANDLE_STMT;
        handle = stmt == 0 ? m_hstmt : stmt;
        break;
    case 2:
        option = SQL_HANDLE_DBC;
        handle = m_hdbc == 0 ? m_hdbc : stmt;
        break;
    }
    while( ( ret = SQLGetDiagRec( option, handle, i, sqlstate, &native_error, msg, sizeof( msg ), &msglen ) ) == SQL_SUCCESS )
    {
        std::wstring strMsg;
        i++;
        str_to_uc_cpy( strMsg, msg );
        errorMsg.push_back( strMsg );
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
        *dest++;
    }
    while( *temp )
    {
        *dest = *temp;
        *dest++;
        *temp++;
    }
    *dest++ = 0;
    *dest = 0;
}

void ODBCDatabase::str_to_uc_cpy(std::wstring &dest, const SQLWCHAR *src)
{
    while( *src )
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
        *dest++;
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
    while( ( ret1 = SQLDataSources( m_env, direct, dsnRead, SQL_MAX_DSN_LENGTH, &pcbDSN, dsnDescr, 254, &pcbDesc ) ) == SQL_SUCCESS && !found )
    {
        if( equal( dsn, dsnRead ) )
            found = true;
        direct = SQL_FETCH_NEXT;
	}
    if( ret1 != SQL_SUCCESS && ret1 != SQL_NO_DATA )
    {
        GetErrorMessage( errorMsg, 0 );
        result = 1;
    }
    else
    {
        if( found )
            copy_uc_to_uc( driver, dsnDescr );
    }
    return result;
}

int ODBCDatabase::Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::vector<SQLWCHAR *> errorMessage;
    SQLWCHAR connectStrIn[sizeof(SQLWCHAR) * 255], driver[1024], dsn[1024];
    SQLSMALLINT OutConnStrLen;
    SQLRETURN ret;
    SQLUSMALLINT options;
    m_connectString = new SQLWCHAR[sizeof(SQLWCHAR) * 1024];
    memset( dsn, 0, sizeof( dsn ) );
    memset( connectStrIn, 0, sizeof( connectStrIn ) );
    memset( driver, 0, sizeof( driver ) );
    uc_to_str_cpy( dsn, selectedDSN.c_str() );
    ret = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HENV, &m_env );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        ret = SQLSetEnvAttr( m_env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER );
        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
        {
            ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &m_hdbc );
            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
            {
                if( !GetDriverForDSN( dsn, driver, errorMsg ) )
                {
                    uc_to_str_cpy( connectStrIn, L"DSN=" );
                    uc_to_str_cpy( connectStrIn, selectedDSN.c_str() );
                    uc_to_str_cpy( connectStrIn, L";Driver=" );
                    copy_uc_to_uc( connectStrIn, driver );
                    SQLSetConnectAttr( m_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0 );
                    options = m_ask ? SQL_DRIVER_COMPLETE_REQUIRED : SQL_DRIVER_COMPLETE;
                    ret = SQLDriverConnect( m_hdbc, m_handle, connectStrIn, SQL_NTS, m_connectString, 1024, &OutConnStrLen, options );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, 2 );
                        result = 1;
                    }
                    else
                    {
                        ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 2 );
                            result = 1;
                        }
                        else
                        {
                            result = GetTableListFromDb( errorMsg );
                        }
                    }
                }
                else
                {
                    GetErrorMessage( errorMsg, 2 );
                    result = 1;
                }
            }
		}
        else
        {
            GetErrorMessage( errorMsg, 0 );
            result = 1;
        }
    }
    else
    {
        GetErrorMessage( errorMsg, 0 );
        result = 1;
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
    delete m_connectString;
    m_connectString = 0;
    return result;
}

int ODBCDatabase::Disconnect(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    RETCODE ret;
    if( m_hstmt != 0 )
    {
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS )
        {
            GetErrorMessage( errorMsg, 1 );
            result = 1;
        }
        else
        {
            m_hstmt = 0;
            ret = SQLDisconnect( m_hdbc );
            if( ret != SQL_SUCCESS )
            {
                GetErrorMessage( errorMsg, 2 );
                result = 1;
            }
            else
            {
                if( m_hdbc != 0 )
                {
                    ret = SQLFreeHandle( SQL_HANDLE_DBC, m_hdbc );
                    if( ret != SQL_SUCCESS )
                    {
                        GetErrorMessage( errorMsg, 2 );
                        result = 1;
                    }
                    else
                    {
                        m_hdbc = 0;
                        if( m_env != 0 )
                        {
                            ret = SQLFreeHandle( SQL_HANDLE_ENV, m_env );
                            if( ret != SQL_SUCCESS )
                            {
                                GetErrorMessage( errorMsg, 0 );
                                return 1;
                            }
                            else
                                m_env = 0;
                        }
                    }
                }
            }
        }
    }
    return result;
}

int ODBCDatabase::GetTableListFromDb(std::vector<std::wstring> &errorMsg)
{
    RETCODE ret;
    int result = 0, bufferSize = 1024;
    std::vector<Field *> fields;
    std::wstring fieldName, fieldType, defaultValue, primaryKey, fkSchema, fkTable, fkName;
    std::set<SQLWCHAR *> pk_fields;
    std::set<SQLWCHAR *> autoinc_fields;
    std::map<int,std::vector<FKField *> > foreign_keys;
    SQLWCHAR *catalogName, *schemaName, *tableName;
    SQLHSTMT stmt_col = 0, stmt_pk = 0, stmt_colattr = 0, stmt_fk = 0;
    SQLHDBC hdbc_col = 0, hdbc_pk = 0, hdbc_colattr = 0, hdbc_fk = 0;
    SQLWCHAR szColumnName[256], szTypeName[256], szRemarks[256], szColumnDefault[256], szIsNullable[256], pkName[SQL_MAX_COLUMN_NAME_LEN + 1], dbName[1024], dbType[1024];
    SQLWCHAR szFkTable[SQL_MAX_COLUMN_NAME_LEN + 1], szPkCol[SQL_MAX_COLUMN_NAME_LEN + 1], szFkTableSchema[SQL_MAX_SCHEMA_NAME_LEN + 1], szFkCol[SQL_MAX_COLUMN_NAME_LEN + 1], szFkCatalog[SQL_MAX_CATALOG_NAME_LEN + 1];
    SQLSMALLINT updateRule, deleteRule, keySequence;
    SQLWCHAR **columnNames = NULL;
    SQLLEN cbPkCol, cbFkTable, cbFkCol, cbFkSchemaName, cbUpdateRule, cbDeleteRule, cbKeySequence, cbFkCatalog;
    SQLLEN cbColumnName, cbDataType, cbTypeName, cbColumnSize, cbBufferLength, cbDecimalDigits, cbNumPrecRadix, pkLength;
    SQLLEN cbNullable, cbRemarks, cbColumnDefault, cbSQLDataType, cbDatetimeSubtypeCode, cbCharOctetLength, cbOrdinalPosition, cbIsNullable;
    SQLSMALLINT DataType, DecimalDigits, NumPrecRadix, Nullable, SQLDataType, DatetimeSubtypeCode, numCols = 0;
    SQLINTEGER ColumnSize, BufferLength, CharOctetLength, OrdinalPosition;
    SQLTablesDataBinding *catalog = (SQLTablesDataBinding *) malloc( 5 * sizeof( SQLTablesDataBinding ) );
    for( int i = 0; i < 5; i++ )
    {
        catalog[i].TargetType = SQL_C_WCHAR;
        catalog[i].BufferLength = ( bufferSize + 1 );
        catalog[i].TargetValuePtr = malloc( sizeof( unsigned char ) * catalog[i].BufferLength );
        ret = SQLBindCol( m_hstmt, (SQLUSMALLINT) i + 1, catalog[i].TargetType, catalog[i].TargetValuePtr, catalog[i].BufferLength, &( catalog[i].StrLen_or_Ind ) );
    }
    ret = SQLTables( m_hstmt, NULL, 0, NULL, 0, NULL, 0, NULL, 0 );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1 );
        result = 1;
    }
    else
    {
        for( ret = SQLFetch( m_hstmt ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( m_hstmt ) )
        {
            if( catalog[0].StrLen_or_Ind != SQL_NULL_DATA )
                catalogName = (SQLWCHAR *) catalog[0].TargetValuePtr;
            if( catalog[1].StrLen_or_Ind != SQL_NULL_DATA )
                schemaName = (SQLWCHAR *) catalog[1].TargetValuePtr;
            if( catalog[2].StrLen_or_Ind != SQL_NULL_DATA )
                tableName = (SQLWCHAR *) catalog[2].TargetValuePtr;
            ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &hdbc_colattr );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 0 );
                result = 1;
                fields.clear();
                pk_fields.clear();
                break;
            }
            else
            {
                SQLSMALLINT OutConnStrLen;
                ret = SQLDriverConnect( hdbc_colattr, NULL, m_connectString, SQL_NTS, NULL, 0, &OutConnStrLen, SQL_DRIVER_NOPROMPT );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 2, hdbc_colattr );
                    result = 1;
                    fields.clear();
                    pk_fields.clear();
                    SQLFreeHandle( SQL_HANDLE_DBC, hdbc_colattr );
                    hdbc_colattr = 0;
                    break;
                }
                else
                {
                    ret = SQLAllocHandle(  SQL_HANDLE_STMT, hdbc_colattr, &stmt_colattr );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, 2, hdbc_colattr );
                        result = 1;
                        fields.clear();
                        pk_fields.clear();
                        SQLDisconnect( hdbc_colattr );
                        SQLFreeHandle( SQL_HANDLE_DBC, hdbc_colattr );
                        hdbc_colattr = 0;
                        break;
                    }
                    else
                    {
                        int size = GetSQLStringSize( tableName );
                        size += GetSQLStringSize( schemaName );
                        SQLWCHAR *szTableName = new SQLWCHAR[size + 16];
                        memset( szTableName, 0, size + 16 );
                        uc_to_str_cpy( szTableName, L"SELECT * FROM " );
                        copy_uc_to_uc( szTableName, schemaName );
                        uc_to_str_cpy( szTableName, L"." );
                        copy_uc_to_uc( szTableName, tableName );
                        ret = SQLExecDirect( stmt_colattr, szTableName, SQL_NTS );
                        delete szTableName;
                        szTableName = NULL;
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NULL_DATA )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_colattr );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLDisconnect( hdbc_colattr );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_colattr );
                            hdbc_colattr = 0;
                            break;
                        }
                        else if( ret != SQL_NULL_DATA )
                        {
                            SQLSMALLINT lenUsed;
                            int bufferSize = 1024;
                            ret = SQLNumResultCols( stmt_colattr, &numCols );
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errorMsg, 1, stmt_colattr );
                                result = 1;
                                fields.clear();
                                pk_fields.clear();
                                SQLDisconnect( hdbc_colattr );
                                SQLFreeHandle( SQL_HANDLE_DBC, hdbc_colattr );
                                hdbc_colattr = 0;
                                break;
                            }
                            columnNames = new SQLWCHAR *[numCols];
                            SQLLEN autoincrement;
                            for( int i = 0; i < numCols; i++ )
                            {
                                autoincrement = 0;
                                columnNames[i] = new SQLWCHAR[sizeof( SQLWCHAR ) * SQL_MAX_COLUMN_NAME_LEN + 1];
                                ret = SQLColAttribute( stmt_colattr, (SQLUSMALLINT) i + 1, SQL_DESC_LABEL, columnNames[i], bufferSize, &lenUsed, NULL );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errorMsg, 1, stmt_colattr );
                                    result = 1;
                                    fields.clear();
                                    pk_fields.clear();
                                    SQLDisconnect( hdbc_colattr );
                                    SQLFreeHandle( SQL_HANDLE_DBC, hdbc_colattr );
                                    hdbc_colattr = 0;
                                    break;
                                }
                                ret = SQLColAttribute( stmt_colattr, (SQLUSMALLINT) i + 1, SQL_DESC_AUTO_UNIQUE_VALUE, NULL, 0, NULL, &autoincrement );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errorMsg, 1, stmt_colattr );
                                    result = 1;
                                    fields.clear();
                                    pk_fields.clear();
                                    autoinc_fields.clear();
                                    SQLDisconnect( hdbc_colattr );
                                    SQLFreeHandle( SQL_HANDLE_DBC, hdbc_colattr );
                                    hdbc_colattr = 0;
                                    break;
                                }
                                if( autoincrement == SQL_TRUE )
                                    autoinc_fields.insert( columnNames[i] );
                            }
                        }
                        ret = SQLFreeHandle( SQL_HANDLE_STMT, stmt_colattr );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_colattr );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            autoinc_fields.clear();
                            SQLDisconnect( hdbc_colattr );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_colattr );
                            stmt_colattr = 0;
                            hdbc_colattr = 0;
                            break;
                        }
                        stmt_colattr = 0;
                        ret = SQLDisconnect( hdbc_colattr );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 2, hdbc_colattr );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            autoinc_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_colattr );
                            hdbc_colattr = 0;
                            break;
                        }
                        ret = SQLFreeHandle( SQL_HANDLE_DBC, hdbc_colattr );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 2, hdbc_colattr );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            autoinc_fields.clear();
                            hdbc_colattr = 0;
                            break;
                        }
                    }
                }
            }
            ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &hdbc_pk );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 0 );
                result = 1;
                fields.clear();
                pk_fields.clear();
                break;
            }
            else
            {
                SQLSMALLINT OutConnStrLen;
                ret = SQLDriverConnect( hdbc_pk, NULL, m_connectString, SQL_NTS, NULL, 0, &OutConnStrLen, SQL_DRIVER_NOPROMPT );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 2 );
                    result = 1;
                    fields.clear();
                    pk_fields.clear();
                    ret = SQLFreeHandle( SQL_HANDLE_DBC, hdbc_pk );
                    hdbc_pk = 0;
                    break;
                }
                else
                {
                    ret = SQLAllocHandle(  SQL_HANDLE_STMT, hdbc_pk, &stmt_pk );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, 2 );
                        result = 1;
                        fields.clear();
                        pk_fields.clear();
                        SQLDisconnect( hdbc_pk );
                        SQLFreeHandle( SQL_HANDLE_DBC, hdbc_pk );
                        hdbc_pk = 0;
                        break;
                    }
                    else
                    {
                        ret = SQLBindCol( stmt_pk, 4, SQL_C_WCHAR, pkName, SQL_MAX_COLUMN_NAME_LEN + 1, &pkLength );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1 );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_pk );
                            SQLDisconnect( hdbc_pk );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_pk );
                            stmt_pk = 0;
                            hdbc_pk = 0;
                            break;
                        }
                        ret = SQLPrimaryKeys( stmt_pk, catalogName, SQL_NTS, schemaName, SQL_NTS, tableName, SQL_NTS );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1 );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_pk );
                            SQLDisconnect( hdbc_pk );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_pk );
                            stmt_pk = 0;
                            hdbc_pk = 0;
                            break;
                        }
                        else
                        {
                            for( ret = SQLFetch( stmt_pk ); ( ret != SQL_SUCCESS || ret != SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( stmt_pk ) )
                                pk_fields.insert( pkName );
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                            {
                                GetErrorMessage( errorMsg, 1 );
                                result = 1;
                                pk_fields.clear();
                                fields.clear();
                                SQLFreeHandle( SQL_HANDLE_STMT, stmt_pk );
                                SQLDisconnect( hdbc_pk );
                                SQLFreeHandle( SQL_HANDLE_DBC, hdbc_pk );
                                stmt_pk = 0;
                                hdbc_pk = 0;
                                break;
                            }
                            ret = SQLFreeHandle( SQL_HANDLE_STMT, stmt_pk );
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errorMsg, 1, stmt_pk );
                                result = 1;
                                fields.clear();
                                pk_fields.clear();
                                SQLDisconnect( hdbc_pk );
                                SQLFreeHandle( SQL_HANDLE_DBC, hdbc_pk );
                                stmt_pk = 0;
                                hdbc_pk = 0;
                                break;
                            }
                            else
                            {
                                stmt_pk = 0;
                                ret = SQLDisconnect( hdbc_pk );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errorMsg, 2, hdbc_pk );
                                    result = 1;
                                    fields.clear();
                                    pk_fields.clear();
                                    SQLFreeHandle( SQL_HANDLE_DBC, hdbc_pk );
                                    hdbc_pk = 0;
                                    break;
                                }
                                else
                                {
                                    ret = SQLFreeHandle( SQL_HANDLE_DBC, hdbc_pk );
                                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                    {
                                        GetErrorMessage( errorMsg, 2, hdbc_pk );
                                        result = 1;
                                        fields.clear();
                                        pk_fields.clear();
                                        break;
                                    }
                                    else
                                        hdbc_pk = 0;
                                }
                            }
                        }
                    }
                }
            }
            ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &hdbc_col );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 0 );
                result = 1;
                fields.clear();
                pk_fields.clear();
                break;
            }
            else
            {
                SQLSMALLINT OutConnStrLen;
                ret = SQLDriverConnect( hdbc_col, NULL, m_connectString, SQL_NTS, NULL, 0, &OutConnStrLen, SQL_DRIVER_NOPROMPT );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 2, hdbc_col );
                    result = 1;
                    fields.clear();
                    pk_fields.clear();
                    SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                    hdbc_col = 0;
                    break;
                }
                else
                {
                    ret = SQLAllocHandle(  SQL_HANDLE_STMT, hdbc_col, &stmt_col );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, 2, hdbc_col );
                        result = 1;
                        fields.clear();
                        pk_fields.clear();
                        SQLDisconnect( hdbc_col );
                        SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                        hdbc_col = 0;
                        break;
                    }
                    else
                    {
                        ret = SQLColumns( stmt_col, catalogName, SQL_NTS, schemaName, SQL_NTS, tableName, SQL_NTS, NULL, 0);
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 4, SQL_C_WCHAR, szColumnName, 256, &cbColumnName );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 5, SQL_C_SSHORT, &DataType, 0, &cbDataType );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 6, SQL_C_WCHAR, szTypeName, 256, &cbTypeName );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 7, SQL_C_SLONG, &ColumnSize, 0, &cbColumnSize );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 8, SQL_C_SLONG, &BufferLength, 0, &cbBufferLength );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 9, SQL_C_SSHORT, &DecimalDigits, 0, &cbDecimalDigits );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 10, SQL_C_SSHORT, &NumPrecRadix, 0, &cbNumPrecRadix );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 11, SQL_C_SSHORT, &Nullable, 0, &cbNullable );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 12, SQL_C_WCHAR, szRemarks, 256, &cbRemarks );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 13, SQL_C_WCHAR, szColumnDefault, 256, &cbColumnDefault );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 14, SQL_C_SSHORT, &SQLDataType, 0, &cbSQLDataType );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 15, SQL_C_SSHORT, &DatetimeSubtypeCode, 0, &cbDatetimeSubtypeCode );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 16, SQL_C_SLONG, &CharOctetLength, 0, &cbCharOctetLength );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 17, SQL_C_SLONG, &OrdinalPosition, 0, &cbOrdinalPosition );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        ret = SQLBindCol( stmt_col, 18, SQL_C_WCHAR, szIsNullable, 256, &cbIsNullable );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_col );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
                            stmt_col = 0;
                            SQLDisconnect( hdbc_col );
                            SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                            hdbc_col = 0;
                            break;
                        }
                        int i = 0;
                        for( ret = SQLFetch( stmt_col ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( stmt_col ) )
                        {
                            str_to_uc_cpy( fieldName, szColumnName );
                            str_to_uc_cpy( fieldType, szTypeName );
                            str_to_uc_cpy( defaultValue, szColumnDefault );
                            fields.push_back( new Field( fieldName, fieldType, ColumnSize, DecimalDigits, defaultValue, Nullable == 1, autoinc_fields.find( szColumnName ) == autoinc_fields.end(), pk_fields.find( szColumnName ) == pk_fields.end() ) );
                            fieldName = L"";
                            fieldType = L"";
                            defaultValue = L"";
                            i++;
                        }
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                        {
                            GetErrorMessage( errorMsg, 2 );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
                    }
				}
            }
            if( columnNames )
            {
                for( int i = 0; i < numCols; i++ )
                    delete columnNames[i];
                delete columnNames;
                columnNames = NULL;
            }
            ret = SQLFreeHandle( SQL_HANDLE_STMT, stmt_col );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 1, stmt_col );
                result = 1;
                fields.clear();
                pk_fields.clear();
                break;
            }
            else
            {
                stmt_col = 0;
                ret = SQLDisconnect( hdbc_col );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 2, hdbc_col );
                    result = 1;
                    fields.clear();
                    pk_fields.clear();
                    break;
                }
                else
                {
                    ret = SQLFreeHandle( SQL_HANDLE_DBC, hdbc_col );
                    if( ret != SQL_SUCCESS )
                    {
                        GetErrorMessage( errorMsg, 2, hdbc_col );
                        result = 1;
                        fields.clear();
                        pk_fields.clear();
                        break;
                    }
                    else
                        hdbc_col = 0;
                }
            }
            ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &hdbc_fk );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 0 );
                result = 1;
                fields.clear();
                pk_fields.clear();
                break;
            }
            else
            {
                SQLSMALLINT OutConnStrLen;
                ret = SQLDriverConnect( hdbc_fk, NULL, m_connectString, SQL_NTS, NULL, 0, &OutConnStrLen, SQL_DRIVER_NOPROMPT );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 2, hdbc_fk );
                    result = 1;
                    fields.clear();
                    pk_fields.clear();
                    break;
                }
                else
                {
                    ret = SQLAllocHandle(  SQL_HANDLE_STMT, hdbc_fk, &stmt_fk );
                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, 2, hdbc_fk );
                        result = 1;
                        fields.clear();
                        pk_fields.clear();
                        break;
                    }
                    else
                    {
                        ret = SQLBindCol( stmt_fk, 4, SQL_C_WCHAR, szPkCol, SQL_MAX_COLUMN_NAME_LEN + 1, &cbPkCol );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
                        ret = SQLBindCol( stmt_fk, 5, SQL_C_WCHAR, szFkCatalog, SQL_MAX_CATALOG_NAME_LEN + 1, &cbFkCatalog );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
                        ret = SQLBindCol( stmt_fk, 6, SQL_C_WCHAR, szFkTableSchema, SQL_MAX_TABLE_NAME_LEN + 1, &cbFkSchemaName );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
                        ret = SQLBindCol( stmt_fk, 7, SQL_C_WCHAR, szFkTable, SQL_MAX_TABLE_NAME_LEN + 1, &cbFkTable );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
                        ret = SQLBindCol( stmt_fk, 8, SQL_C_WCHAR, szFkCol, SQL_MAX_COLUMN_NAME_LEN + 1, &cbFkCol );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
                        ret = SQLBindCol( stmt_fk, 9, SQL_C_SSHORT, &keySequence, SQL_MAX_TABLE_NAME_LEN + 1, &cbKeySequence );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
                        ret = SQLBindCol( stmt_fk, 10, SQL_C_SSHORT, &updateRule, SQL_MAX_COLUMN_NAME_LEN + 1, &cbUpdateRule );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
                        ret = SQLBindCol( stmt_fk, 11, SQL_C_SSHORT, &deleteRule, SQL_MAX_COLUMN_NAME_LEN + 1, &cbDeleteRule );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
                        ret = SQLForeignKeys( stmt_fk, catalogName, SQL_NTS, schemaName, SQL_NTS, tableName, SQL_NTS, NULL, 0, NULL, 0, NULL, 0 );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
                        for( ret = SQLFetch( stmt_fk ); ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA; ret = SQLFetch( stmt_fk ) )
                        {
                            FK_ONUPDATE update_constraint;
                            FK_ONDELETE delete_constraint;
                            str_to_uc_cpy( primaryKey, szPkCol );
                            str_to_uc_cpy( fkSchema, szFkTableSchema );
                            str_to_uc_cpy( fkTable, szFkTable );
                            str_to_uc_cpy( fkName, szFkCol );
                            switch( updateRule )
                            {
                                case SQL_NO_ACTION:
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
                            switch( deleteRule )
                            {
                                case SQL_NO_ACTION:
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
                            foreign_keys[keySequence].push_back( new FKField( fkTable, primaryKey, fkName, fkSchema, update_constraint, delete_constraint ) );
                            primaryKey = L"";
                            fkSchema = L"";
                            fkTable = L"";
                            fkName = L"";
                        }
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            foreign_keys.clear();
                        }
                        else
                        {
                            ret = SQLFreeHandle( SQL_HANDLE_STMT, stmt_fk );
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errorMsg, 2, hdbc_fk );
                                result = 1;
                                fields.clear();
                                pk_fields.clear();
                                foreign_keys.clear();
                            }
                            else
                            {
                                stmt_fk = 0;
                                ret = SQLDisconnect( hdbc_fk );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errorMsg, 2, hdbc_fk );
                                    result = 1;
                                    fields.clear();
                                    pk_fields.clear();
                                    foreign_keys.clear();
                                }
                                else
                                {
                                    ret = SQLFreeHandle( SQL_HANDLE_DBC, hdbc_fk );
                                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                    {
                                        GetErrorMessage( errorMsg, 2, hdbc_fk );
                                        result = 1;
                                        fields.clear();
                                        pk_fields.clear();
                                        foreign_keys.clear();
                                    }
                                    else
                                        hdbc_fk = 0;
                                }
                            }
                        }
                    }
                }
            }
            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO || ret == SQL_NO_DATA )
            {
                std::wstring catalog_name, schema_name, table_name;
                str_to_uc_cpy( catalog_name, catalogName );
                str_to_uc_cpy( schema_name, schemaName );
                schema_name += L".";
                str_to_uc_cpy( table_name, tableName );
                schema_name += table_name;
                pimpl->m_tables[catalog_name].push_back( new DatabaseTable( schema_name, fields, foreign_keys ) );
				fields.erase( fields.begin(), fields.end() );
				foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                fields.clear();
                foreign_keys.clear();
            }
            else
            {
                GetErrorMessage( errorMsg, 1, stmt_fk );
                result = 1;
                fields.clear();
                pk_fields.clear();
                break;
            }
        }
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
        {
            if( errorMsg.empty() )
            {
                GetErrorMessage( errorMsg, 1 );
                result = 1;
                fields.clear();
                pk_fields.clear();
            }
        }
        bufferSize = 1024;
        ret = SQLGetInfo( m_hdbc, SQL_DATABASE_NAME, dbName, (SQLSMALLINT) bufferSize, (SQLSMALLINT *) &bufferSize );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2 );
            result = 1;
            fields.clear();
            pk_fields.clear();
        }
        else
        {
            str_to_uc_cpy( pimpl->m_dbName, dbName );
        }
        bufferSize = 1024;
        ret = SQLGetInfo( m_hdbc, SQL_DBMS_NAME, dbType, (SQLSMALLINT) bufferSize, (SQLSMALLINT *) &bufferSize );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2 );
            result = 1;
            fields.clear();
            pk_fields.clear();
        }
        else
        {
            str_to_uc_cpy( pimpl->m_subtype, dbType );
        }
    }
    for( int i = 0; i < 5; i++ )
    {
        free( catalog[i].TargetValuePtr );
    }
    free( catalog );
    return result;
}
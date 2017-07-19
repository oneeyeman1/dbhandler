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
            handle = stmt == 0 ? m_hdbc : stmt;
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

int ODBCDatabase::CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    SQLWCHAR *query;
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        query = new SQLWCHAR[name.length() + 2];
        memset( query, '\0', name.length() + 2 );
        uc_to_str_cpy( query, name );
        ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
        delete query;
        query = NULL;
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2 );
            result = 1;
        }
        else
        {
            if( m_hstmt )
            {
                ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 2 );
                    result = 1;
                }
                else
                    m_hstmt = 0;
            }
        }
    }
    else
    {
        GetErrorMessage( errorMsg, 2 );
        result = 1;
    }
    return result;
}

int ODBCDatabase::DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    SQLWCHAR *query;
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        query = new SQLWCHAR[name.length() + 2];
        memset( query, '\0', name.length() + 2 );
        uc_to_str_cpy( query, name );
        ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
        delete query;
        query = NULL;
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2 );
            result = 1;
        }
        else
        {
            if( m_hstmt )
            {
                ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 2 );
                    result = 1;
                }
                else
                    m_hstmt = 0;
            }
        }
    }
    else
    {
        GetErrorMessage( errorMsg, 2 );
        result = 1;
    }
    return result;
}

int ODBCDatabase::Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg)
{
    int result = 0, bufferSize = 1024;
    std::vector<SQLWCHAR *> errorMessage;
    SQLWCHAR connectStrIn[sizeof(SQLWCHAR) * 255], driver[1024], dsn[1024], dbType[1024], *query = NULL;
    SQLSMALLINT OutConnStrLen;
    SQLRETURN ret;
    SQLUSMALLINT options;
    std::wstring query1, query2, query3, query4, query5, query6, query7;
    if( !pimpl )
        pimpl = new Impl;
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
                        str_to_uc_cpy( pimpl->m_connectString, m_connectString );
                        ret = SQLGetInfo( m_hdbc, SQL_DBMS_NAME, dbType, (SQLSMALLINT) bufferSize, (SQLSMALLINT *) &bufferSize );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 2 );
                            result = 1;
                        }
                        else
                        {
                            str_to_uc_cpy( pimpl->m_subtype, dbType );
                            ret = SQLSetConnectAttr( m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) FALSE, 0 );
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errorMsg, 2 );
                                result = 1;
                                ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                            }
                            else
                            {
                                if( pimpl->m_subtype == L"Microsoft SQL Server" ) // MS SQL SERVER
                                {
                                    query1 = L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatcol' AND xtype='U') CREATE TABLE \"abcatcol\"(abc_tnam varchar(129) NOT NULL, abc_tid integer, abc_ownr varchar(129) NOT NULL, abc_cnam varchar(129) NOT NULL, abc_cid smallint, abc_labl varchar(254), abc_lpos smallint, abc_hdr varchar(254), abc_hpos smallint, abc_itfy smallint, abc_mask varchar(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn varchar(31), abc_bmap char(1), abc_init varchar(254), abc_cmnt varchar(254), abc_edit varchar(31), abc_tag varchar(254) PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));";
                                    query2 = L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatedt' AND xtype='U') CREATE TABLE \"abcatedt\"(abe_name varchar(30) NOT NULL, abe_edit varchar(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work varchar(32) PRIMARY KEY( abe_name, abe_seqn ));";
                                    query3 = L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatfmt' AND xtype='U') CREATE TABLE \"abcatfmt\"(abf_name varchar(30) NOT NULL, abf_frmt varchar(254), abf_type smallint, abf_cntr integer PRIMARY KEY( abf_name ));";
                                    query4 = L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcattbl' AND xtype='U') CREATE TABLE \"abcattbl\"(abt_tnam varchar(129) NOT NULL, abt_tid integer, abt_ownr varchar(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce varchar(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce varchar(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce varchar(18), abt_cmnt varchar(254) PRIMARY KEY( abt_tnam, abt_ownr ));";
                                    query5 = L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatvld' AND xtype='U') CREATE TABLE \"abcatvld\"(abv_name varchar(30) NOT NULL, abv_vald varchar(254), abv_type smallint, abv_cntr integer, abv_msg varchar(254) PRIMARY KEY( abv_name ));";
                                    query6 = L"IF NOT EXISTS (SELECT * FROM sys.indexes WHERE name='abcattbl_tnam_ownr' AND object_id = OBJECT_ID('abcattbl')) CREATE INDEX \"abcattbl_tnam_ownr\" ON \"abcattbl\"(\"abt_tnam\" ASC, \"abt_ownr\" ASC);";
                                    query7 = L"IF NOT EXISTS (SELECT * FROM sys.indexes WHERE name='abcatcol_tnam_ownr_cnam' AND object_id = OBJECT_ID('abcatcol')) CREATE INDEX \"abcatcol_tnam_ownr_cnam\" ON \"abcatcol\"(\"abc_tnam\" ASC, \"abc_ownr\" ASC, \"abc_cnam\" ASC);";
                                }
                                if( pimpl->m_subtype == L"MySQL" || pimpl->m_subtype == L"PostgreSQL" )
                                {
                                    query1 = L"CREATE TABLE IF NOT EXISTS \"abcatcol\"(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));";
                                    query2 = L"CREATE TABLE IF NOT EXISTS \"abcatedt\"(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));";
                                    query3 = L"CREATE TABLE IF NOT EXISTS \"abcatfmt\"(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));";
                                    query4 = L"CREATE TABLE IF NOT EXISTS \"abcattbl\"(abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ));";
                                    query5 = L"CREATE TABLE IF NOT EXISTS \"abcatvld\"(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));";
                                    if( pimpl->m_subtype == L"MySQL" )
                                    {
                                        query6 = L"IF NOT EXISTS (SELECT * FROM information_schema.statistics WHERE \"index_name\"='abcattbl_tnam_ownr' AND \"table_name\"='abcattbl') CREATE INDEX \"abcattbl_tnam_ownr\" ON \"abcattbl\"(\"abt_tnam\" ASC, \"abt_ownr\" ASC);";
                                        query7 = L"IF NOT EXISTS (SELECT * FROM information_schema.statistics WHERE \"index_name\"='abcatcol_tnam_ownr_cnam' AND \"table_name\"='abcatcol') CREATE INDEX \"abcatcol_tnam_ownr_cnam\" ON \"abcattbl\"(\"abc_tnam\" ASC, \"abc_ownr\" ASC, \"abc_cnam\" ASC);";
                                    }
                                    else
                                    {
                                        query6 = L"CREATE INDEX IF NOT EXISTS \"abcattbl_tnam_ownr\" ON \"abcattbl\"(\"abt_tnam\" ASC, \"abt_ownr\" ASC);";
                                        query7 = L"CREATE INDEX IF NOT EXISTS \"abcatcol_tnam_ownr_cnam\" ON \"abcatcol\"(\"abc_tnam\" ASC, \"abc_ownr\" ASC, \"abc_cnam\" ASC);";
                                    }
                                }
                                if( pimpl->m_subtype == L"Sybase" )
                                {
                                    query1 = L"IF NOT EXISTS(SELECT 1 FROM sysobjects WHERE name = 'abcatcol' AND type = 'U') EXECUTE(CREATE TABLE abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam )));";
                                    query2 = L"IF NOT EXISTS(SELECT 1 FROM sysobjects WHERE name = 'abcatedt' AND type = 'U') EXECUTE(CREATE TABLE abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn )));";
                                    query3 = L"IF NOT EXISTS(SELECT 1 FROM sysobjects WHERE name = 'abcatfmt' AND type = 'U') EXECUTE(CREATE TABLE abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name )));";
                                    query4 = L"IF NOT EXISTS(SELECT 1 FROM sysobjects WHERE name = 'abcattbl' AND type = 'U') EXECUTE(CREATE TABLE abcattbl(abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr )));";
                                    query5 = L"IF NOT EXISTS(SELECT 1 FROM sysobjects WHERE name = 'abcatvld' AND type = 'U') EXECUTE(CREATE TABLE abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name )));";
                                    query6 = L"IF NOT EXISTS(SELECT o.name, i.name FROM tempdb..sysobjects o, tempdb..sysindexes i WHERE o.id = i.id AND o.name='abcattbl' AND i.name='abcattbl_tnam_ownr') CREATE INDEX \"abcattbl_tnam_ownr\" ON \"abcattbl\"(\"abt_tnam\" ASC, \"abt_ownr\" ASC)";
                                    query7 = L"IF NOT EXISTS(SELECT o.name, i.name FROM tempdb..sysobjects o, tempdb..sysindexes i WHERE o.id = i.id AND o.name='abcatcol' AND i.name='abcatcol_tnam_ownr_cnam') CREATE INDEX \"abcatcol_tnam_ownr_cnam\" ON \"abcatcol\"(\"abc_tnam\" ASC, \"abc_ownr\" ASC, \"abc_cnam\" ASC)";
                                }
                                if( pimpl->m_subtype == L"Oracle" )
                                {
                                    query1 = L"IF( SELECT count(*) FROM all_tables WHERE table_name = 'abcatcol' ) <= 0 CREATE TABLE abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));";
                                    query2 = L"IF( SELECT count(*) FROM all_tables WHERE table_name = 'abcatedt' ) <= 0 CREATE TABLE abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));";
                                    query3 = L"IF( SELECT count(*) FROM all_tables WHERE table_name = 'abcatfmt' ) <= 0 CREATE TABLE abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));";
                                    query4 = L"IF( SELECT count(*) FROM all_tables WHERE table_name = 'abcattbl' ) <= 0 CREATE TABLE abcattbl(abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abl_tnam, abl_ownr ));";
                                    query5 = L"IF( SELECT count(*) FROM all_tables WHERE table_name = 'abcatvld' ) <= 0 CREATE TABLE abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));";
                                    query6 = L"";
                                    query7 = L"";
                                }
                                ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
                                if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                                {
                                    query = new SQLWCHAR[query1.length() + 2];
                                    memset( query, '\0', query1.size() + 2 );
                                    uc_to_str_cpy( query, query1 );
                                    ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
                                    delete query;
                                    query = NULL;
                                    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                                    {
                                        query = new SQLWCHAR[query2.length() + 2];
                                        memset( query, '\0', query2.size() + 2 );
                                        uc_to_str_cpy( query, query2 );
                                        ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
                                        delete query;
                                        query = NULL;
                                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                                        {
                                            query = new SQLWCHAR[query3.length() + 2];
                                            memset( query, '\0', query3.size() + 2 );
                                            uc_to_str_cpy( query, query3 );
                                            ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
                                            delete query;
                                            query = NULL;
                                            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                                            {
                                                query = new SQLWCHAR[query4.length() + 2];
                                                memset( query, '\0', query4.size() + 2 );
                                                uc_to_str_cpy( query, query4 );
                                                ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
                                                delete query;
                                                query = NULL;
                                                if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                                                {
                                                    query = new SQLWCHAR[query5.length() + 2];
                                                    memset( query, '\0', query5.size() + 2 );
                                                    uc_to_str_cpy( query, query5 );
                                                    ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
                                                    delete query;
                                                    query = NULL;
                                                    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                                                    {
                                                        query = new SQLWCHAR[query6.length() + 2];
                                                        memset( query, '\0', query6.size() + 2 );
                                                        uc_to_str_cpy( query, query6 );
                                                        ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
                                                        delete query;
                                                        query = NULL;
                                                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                                                        {
                                                            query = new SQLWCHAR[query7.length() + 2];
                                                            memset( query, '\0', query7.size() + 2 );
                                                            uc_to_str_cpy( query, query7 );
                                                            ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
                                                            delete query;
                                                            query = NULL;
                                                            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                                                            {
                                                                ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                    {
                                        GetErrorMessage( errorMsg, 1 );
                                        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                                        result = 1;
                                    }
                                }
                                else
                                {
                                    GetErrorMessage( errorMsg, 2 );
                                    ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                                    result = 1;
                                }
                            }
                            if( m_hstmt )
                            {
                                ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errorMsg, 2 );
                                    result = 1;
                                }
                                else
                                    m_hstmt = 0;
                            }
                            if( !result )
                            {
                                ret = SQLSetConnectAttr( m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) TRUE, 0 );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errorMsg, 2 );
                                    result = 1;
                                }
                                result = GetTableListFromDb( errorMsg );
                            }
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
    delete query;
    query = NULL;
    return result;
}

int ODBCDatabase::Disconnect(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    RETCODE ret;
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
    for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it = pimpl->m_tables.begin(); it != pimpl->m_tables.end(); it++ )
    {
        std::vector<DatabaseTable *> tableVec = (*it).second;
        for( std::vector<DatabaseTable *>::iterator it1 = tableVec.begin(); it1 < tableVec.end(); it1++ )
        {
            std::vector<Field *> fields = (*it1)->GetFields();
            for( std::vector<Field *>::iterator it2 = fields.begin(); it2 < fields.end(); it2++ )
            {
                delete (*it2);
                (*it2) = NULL;
            }
            std::map<int,std::vector<FKField *> > fk_fields = (*it1)->GetForeignKeyVector();
            for( std::map<int, std::vector<FKField *> >::iterator it2 = fk_fields.begin(); it2 != fk_fields.end(); it2++ )
            {
                for( std::vector<FKField *>::iterator it3 = (*it2).second.begin(); it3 < (*it2).second.end(); it3++ )
                {
                    delete (*it3);
                    (*it3) = NULL;
                }
            }
        }
        for( std::vector<DatabaseTable *>::iterator it1 = tableVec.begin(); it1 < tableVec.end(); it1++ )
        {
            delete (*it1);
            (*it1) = NULL;
        }
    }
    delete pimpl;
    pimpl = NULL;
    return result;
}

int ODBCDatabase::GetTableListFromDb(std::vector<std::wstring> &errorMsg)
{
    RETCODE ret;
    int result = 0, bufferSize = 1024;
    std::vector<Field *> fields;
    std::wstring fieldName, fieldType, defaultValue, primaryKey, fkSchema, fkTable, fkName;
    std::vector<std::wstring> pk_fields, fk_fieldNames;
    std::vector<std::wstring> autoinc_fields;
    std::map<int,std::vector<FKField *> > foreign_keys;
    SQLWCHAR *catalogName = NULL, *schemaName = NULL, *tableName = NULL;
    SQLHSTMT stmt_col = 0, stmt_pk = 0, stmt_colattr = 0, stmt_fk = 0;
    SQLHDBC hdbc_col = 0, hdbc_pk = 0, hdbc_colattr = 0, hdbc_fk = 0;
    SQLWCHAR szColumnName[256], szTypeName[256], szRemarks[256], szColumnDefault[256], szIsNullable[256], pkName[SQL_MAX_COLUMN_NAME_LEN + 1], dbName[1024], userName[1024];
    SQLWCHAR szFkTable[SQL_MAX_COLUMN_NAME_LEN + 1], szPkCol[SQL_MAX_COLUMN_NAME_LEN + 1], szPkTable[SQL_MAX_COLUMN_NAME_LEN + 1], szPkSchema[SQL_MAX_COLUMN_NAME_LEN + 1], szFkTableSchema[SQL_MAX_SCHEMA_NAME_LEN + 1], szFkCol[SQL_MAX_COLUMN_NAME_LEN + 1], szFkCatalog[SQL_MAX_CATALOG_NAME_LEN + 1];
    SQLSMALLINT updateRule, deleteRule, keySequence;
    SQLWCHAR **columnNames = NULL;
    SQLLEN cbPkCol, cbFkTable, cbFkCol, cbFkSchemaName, cbUpdateRule, cbDeleteRule, cbKeySequence, cbFkCatalog;
    SQLLEN cbColumnName, cbDataType, cbTypeName, cbColumnSize, cbBufferLength, cbDecimalDigits, cbNumPrecRadix, pkLength;
    SQLLEN cbNullable, cbRemarks, cbColumnDefault, cbSQLDataType, cbDatetimeSubtypeCode, cbCharOctetLength, cbOrdinalPosition, cbIsNullable;
    SQLSMALLINT DataType, DecimalDigits, NumPrecRadix, Nullable, SQLDataType, DatetimeSubtypeCode, numCols = 0;
    SQLINTEGER ColumnSize, BufferLength, CharOctetLength, OrdinalPosition;
    SQLTablesDataBinding *catalog = (SQLTablesDataBinding *) malloc( 5 * sizeof( SQLTablesDataBinding ) );
    ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1 );
        return 1;
    }
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
                        if( *schemaName != 0 )
                        {
                            copy_uc_to_uc( szTableName, schemaName );
                            uc_to_str_cpy( szTableName, L"." );
                        }
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
                                {
                                    std::wstring autoincFieldName;
                                    str_to_uc_cpy( autoincFieldName, columnNames[i] );
                                    autoinc_fields.push_back( autoincFieldName );
                                }
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
                            GetErrorMessage( errorMsg, 1, stmt_pk );
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
                            {
                                std::wstring pkFieldName;
                                str_to_uc_cpy( pkFieldName, pkName );
                                pk_fields.push_back( pkFieldName );
                            }
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
                        ret = SQLBindCol( stmt_fk, 2, SQL_C_WCHAR, szPkSchema, SQL_MAX_COLUMN_NAME_LEN + 1, &cbPkCol );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
                        ret = SQLBindCol( stmt_fk, 3, SQL_C_WCHAR, szPkTable, SQL_MAX_COLUMN_NAME_LEN + 1, &cbPkCol );
                        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 1, stmt_fk );
                            result = 1;
                            fields.clear();
                            pk_fields.clear();
                            break;
                        }
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
                        ret = SQLForeignKeys( stmt_fk, NULL, 0, NULL, 0, NULL, 0, catalogName, SQL_NTS, schemaName, SQL_NTS, tableName, SQL_NTS );
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
                            FK_ONUPDATE update_constraint = NO_ACTION_UPDATE;
                            FK_ONDELETE delete_constraint = NO_ACTION_DELETE;
                            str_to_uc_cpy( primaryKey, szPkCol );
                            str_to_uc_cpy( fkSchema, szPkSchema );
                            str_to_uc_cpy( fkTable, szPkTable );
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
                            foreign_keys[keySequence].push_back( new FKField( keySequence, fkTable, primaryKey, fkName, fkSchema, update_constraint, delete_constraint ) );
                            fk_fieldNames.push_back( primaryKey );
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
                            std::wstring schema_name, table_name;
                            str_to_uc_cpy( schema_name, schemaName );
                            str_to_uc_cpy( table_name, tableName );
                            str_to_uc_cpy( fieldName, szColumnName );
                            str_to_uc_cpy( fieldType, szTypeName );
                            str_to_uc_cpy( defaultValue, szColumnDefault );
                            Field *field = new Field( fieldName, fieldType, ColumnSize, DecimalDigits, defaultValue, Nullable == 1, std::find( autoinc_fields.begin(), autoinc_fields.end(), fieldName ) == autoinc_fields.end(), std::find( pk_fields.begin(), pk_fields.end(), fieldName ) != pk_fields.end(), std::find( fk_fieldNames.begin(), fk_fieldNames.end(), fieldName ) != fk_fieldNames.end() );
                            if( GetFieldProperties( table_name, schema_name, fieldName, field, errorMsg ) )
                            {
                                GetErrorMessage( errorMsg, 2 );
                                result = 1;
                                fields.clear();
                                pk_fields.clear();
                                break;
                            }
                            fields.push_back( field );
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
            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO || ret == SQL_NO_DATA )
            {
                std::wstring catalog_name, schema_name, table_name;
                str_to_uc_cpy( catalog_name, catalogName );
                str_to_uc_cpy( schema_name, schemaName );
                str_to_uc_cpy( table_name, tableName );
                if( pimpl->m_subtype == L"Microsoft SQL Server" && schema_name == L"sys" )
                    table_name = schema_name + L"." + table_name;
                DatabaseTable *table = new DatabaseTable( table_name, schema_name, fields, foreign_keys );
                if( SetTableOwner( table, errorMsg ) )
                {
                    fields.erase( fields.begin(), fields.end() );
                    foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                    fields.clear();
                    foreign_keys.clear();
                    autoinc_fields.clear();
                    pk_fields.clear();
                    fk_fieldNames.clear();
                    return 1;
                }
                if( GetTableId( table, errorMsg ) )
                {
                    fields.erase( fields.begin(), fields.end() );
                    foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                    fields.clear();
                    foreign_keys.clear();
                    autoinc_fields.clear();
                    pk_fields.clear();
                    fk_fieldNames.clear();
                    return 1;
                }
                if( GetTableProperties( table, errorMsg ) )
                {
                    fields.erase( fields.begin(), fields.end() );
                    foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                    fields.clear();
                    foreign_keys.clear();
                    autoinc_fields.clear();
                    pk_fields.clear();
                    fk_fieldNames.clear();
                    return 1;
                }
                pimpl->m_tables[catalog_name].push_back( table );
                fields.erase( fields.begin(), fields.end() );
                foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                fields.clear();
                foreign_keys.clear();
                autoinc_fields.clear();
                pk_fields.clear();
                fk_fieldNames.clear();
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
        ret = SQLGetInfo( m_hdbc, SQL_USER_NAME, userName, (SQLSMALLINT) bufferSize, (SQLSMALLINT *) &bufferSize );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2 );
            result = 1;
            fields.clear();
            pk_fields.clear();
        }
        else
        {
            str_to_uc_cpy( pimpl->m_connectedUser, dbName );
        }
    }
    for( int i = 0; i < 5; i++ )
    {
        free( catalog[i].TargetValuePtr );
    }
    free( catalog );
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 2 );
        result = 1;
        fields.clear();
        pk_fields.clear();
    }
    else
        m_hstmt = 0;
    return result;
}

int ODBCDatabase::CreateIndex(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    SQLRETURN ret;
    int result = 0;
    SQLWCHAR *query = NULL;
    query = new SQLWCHAR[command.length() + 2];
    memset( query, '\0', command.length() + 2 );
    uc_to_str_cpy( query, command );
    ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
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
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, m_hstmt );
        result = 1;
    }
    else
        m_hstmt = 0;
    delete query;
    query = NULL;
    return result;
}

int ODBCDatabase::SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg)
{
    bool found = false;
    SQLWCHAR *table_name = NULL, *field_name = NULL, *owner_name = NULL, *value = NULL, *query = NULL;
    SQLLEN cbTableName = SQL_NTS, cbFieldName = SQL_NTS, cbOwnerName = SQL_NTS, cbValue = SQL_NTS;
    int result = 0;
    std::wstring temp;
    std::wstring query1 = L"SELECT count(*) FROM abcatcol WHERE abc_tnam = ? AND abc_cnam = ? AND abc_ownr = ?;", query2;
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        temp = L"BEGIN TRANSACTION";
        query = new SQLWCHAR[temp.length() + 2];
        memset( query, '\0', temp.length() + 2 );
        uc_to_str_cpy( query, temp );
        ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 1, m_hstmt );
            return 1;
        }
        delete query;
        query = NULL;
        query = new SQLWCHAR[query1.length() + 2];
        table_name = new SQLWCHAR[tableName.length() + 2];
        field_name = new SQLWCHAR[fieldName.length() + 2];
        owner_name = new SQLWCHAR[pimpl->m_connectedUser.length() + 2];
        value = new SQLWCHAR[comment.length() + 2];
        memset( query, '\0', query1.size() + 2 );
        memset( table_name, '\0', tableName.length() + 2 );
        memset( field_name, '\0', fieldName.length() + 2 );
        memset( owner_name, '\0', pimpl->m_connectedUser.length() + 2 );
        memset( value, '\0', comment.length() + 2 );
        uc_to_str_cpy( query, query1 );
        uc_to_str_cpy( field_name, fieldName );
        uc_to_str_cpy( table_name, tableName );
        uc_to_str_cpy( owner_name, pimpl->m_connectedUser );
        uc_to_str_cpy( value, comment );
        ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, tableName.length(), 0, table_name, 0, &cbTableName );
        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
        {
            ret = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, fieldName.length(), 0, field_name, 0, &cbFieldName );
            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
            {
                ret = SQLBindParameter( m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, pimpl->m_connectedUser.length(), 0, owner_name, 0, &cbOwnerName );
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
                                found = true;
                            }
                            else if( ret != SQL_NO_DATA )
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
                    }
                    else
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
            }
            else
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
    }
    else
    {
        GetErrorMessage( errorMsg, 1, m_hstmt );
        result = 1;
    }
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, m_hstmt );
        return 1;
    }
    else
        m_hstmt = 0;
    delete query;
    query = NULL;
    if( !result )
    {
        if( found )
            query2 = L"UPDATE abcatcol SET abc_cmnt = ? WHERE abc_tnam = ? AND abc_ownr == ? AND abc_cnam = ?;";
        else
            query2 = L"INSERT INTO abcattbl(abc_cmnt, abc_tnam, abc_ownr, abc_cnam ) VALUES( ?, ?, ?, ? );";
        ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
        {
            query = new SQLWCHAR[query2.length() + 2];
            memset( query, '\0', query2.size() + 2 );
            uc_to_str_cpy( query, query2 );
            ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, comment.length(), 0, value, 0, &cbValue );
            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
            {
                ret = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, tableName.length(), 0, table_name, 0, &cbTableName );
                if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                {
                    ret = SQLBindParameter( m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, fieldName.length(), 0, field_name, 0, &cbFieldName );
                    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                    {
                        ret = SQLBindParameter( m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, pimpl->m_connectedUser.length(), 0, owner_name, 0, &cbOwnerName );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            ret = SQLPrepare( m_hstmt, query, SQL_NTS );
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                ret = SQLExecute( m_hstmt );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
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
                        }
                        else
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
                }
                else
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
        }
        delete query;
        query = NULL;
        if( !result )
        {
            temp = L"COMMIT";
        }
        else
        {
            temp = L"ROLLBACK";
        }
        query = new SQLWCHAR[temp.length() + 2];
        memset( query, '\0', temp.length() + 2 );
        uc_to_str_cpy( query, temp );
        ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 1, m_hstmt );
            result = 1;
        }
        else
        {
            if( m_hstmt )
            {
                ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 1, m_hstmt );
                    result = 1;
                }
                else
                    m_hstmt = 0;
            }
        }
    }
    return result;
}

bool ODBCDatabase::IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    SQLRETURN ret;
    SQLWCHAR *query = NULL;
    bool exists = false;
    std::wstring query1;
    SQLWCHAR *index_name = NULL, *table_name = NULL, *schema_name = NULL;
    SQLLEN cbIndexName = SQL_NTS, cbTableName = SQL_NTS, cbSchemaName = SQL_NTS;
    if( pimpl->m_subtype == L"Microsoft SQL Server" )
        query1 = L"SELECT count(*) FROM sys.indexes WHERE name = ? AND object_id = OBJECT_ID( ? ) );";
    if( pimpl->m_subtype == L"MySQL" )
        query1 = L"SELECT count(*) FROM information_schema.statistics WHERE index_name = ? AND table_name = ? AND schema_name = ?;";
    if( pimpl->m_subtype == L"PostgreSQL" )
        query1 = L"SELECT count(*) FROM pg_indexes WHERE indexname = $1 AND tablename = $2 AND schemaname = $3;";
    index_name = new SQLWCHAR[indexName.length() + 2];
    memset( index_name, '\0', indexName.length() + 2 );
    uc_to_str_cpy( index_name, indexName );
    if( pimpl->m_subtype == L"MySQL" || pimpl->m_subtype == L"PostgreSQL" )
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
                if( pimpl->m_subtype == L"MySQL" || pimpl->m_subtype == L"PostgreSQL" )
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
                                GetErrorMessage( errorMsg, 1, m_hstmt );
                            }
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, 1, m_hstmt );
                        }
                    }
                    else
                    {
                        GetErrorMessage( errorMsg, 1, m_hstmt );
                    }
                }
                else
                {
                    GetErrorMessage( errorMsg, 1, m_hstmt );
                }
            }
            else
            {
                GetErrorMessage( errorMsg, 1, m_hstmt );
            }
        }
        else
        {
            GetErrorMessage( errorMsg, 1, m_hstmt );
        }
    }
    else
    {
        GetErrorMessage( errorMsg, 1, m_hstmt );
    }
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, m_hstmt );
    }
    else
        m_hstmt = 0;
    delete index_name;
    index_name = NULL;
    delete table_name;
    table_name = NULL;
    delete schema_name;
    schema_name = NULL;
    delete query;
    query = NULL;
    return exists;
}

int ODBCDatabase::GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    SQLSMALLINT OutConnStrLen;
    SQLHDBC hdbc_tableProp;
    SQLHSTMT stmt_tableProp;
    unsigned short dataFontSize, dataFontWeight, headingFontSize, headingFontWeight, labelFontSize, labelFontWeight;
    SQLWCHAR dataFontItalic[2], headingFontItalic[2], labelFontItalic[2], dataFontUnderline[2], headingFontUnderline[2], labelFontUnderline[2], dataFontName[20], headingFontName[20], labelFontName[20];
    SQLWCHAR comments[225];
    SQLLEN cbDataFontSize = 0, cbDataFontWeight = 0, cbDataFontItalic = 0, cbDataFontUnderline = 0, cbDataFontName = 0, cbHeadingFontSize = 0, cbHeadingFontWeight = 0;
    SQLLEN cbSchemaName = SQL_NTS, cbTableName = SQL_NTS, cbHeadingFontItalic = 0,  cbHeadingFontUnderline = 0, cbHeadingFontName = 0, cbComment;
    SQLLEN cbLabelFontSize = 0, cbLabelFontWeight = 0, cbLabelFontItalic = 0, cbLabelFontUnderline = 0, cbLabelFontName = 0;
    std::wstring query = L"SELECT * FROM abcattbl WHERE \"abt_tnam\" = ? AND \"abt_ownr\" = ?;";
    std::wstring tableName = table->GetTableName(), ownerName = table->GetTableOwner();
    int tableNameLen = tableName.length(), ownerNameLen = ownerName.length();
    SQLLEN cbOwnerName = ownerNameLen == 0 ? SQL_NULL_DATA : SQL_NTS;
    SQLWCHAR *qry = new SQLWCHAR[query.length() + 2], *table_name = new SQLWCHAR[tableNameLen + 2], *owner_name = new SQLWCHAR[ownerNameLen + 2];
    memset( owner_name, '\0', ownerNameLen + 2 );
    memset( table_name, '\0', tableNameLen + 2 );
    uc_to_str_cpy( owner_name, ownerName );
    uc_to_str_cpy( table_name, tableName );
    memset( qry, '\0', query.size() + 2 );
    uc_to_str_cpy( qry, query );
    SQLRETURN ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &hdbc_tableProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 0, m_env );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLDriverConnect( hdbc_tableProp, NULL, m_connectString, SQL_NTS, NULL, 0, &OutConnStrLen, SQL_DRIVER_NOPROMPT );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 2, m_env );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLAllocHandle( SQL_HANDLE_STMT, hdbc_tableProp, &stmt_tableProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 2, m_hdbc );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindParameter( stmt_tableProp, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, tableName.length(), 0, table_name, 0, &cbTableName );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindParameter( stmt_tableProp, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, ownerNameLen, 0, owner_name, 0, &cbOwnerName );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLPrepare( stmt_tableProp, qry, SQL_NTS );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLExecute( stmt_tableProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 4, SQL_C_SSHORT, &dataFontSize, 0, &cbDataFontSize );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 5, SQL_C_SSHORT, &dataFontWeight, 0, &cbDataFontWeight );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 6, SQL_C_WCHAR, &dataFontItalic, 3, &cbDataFontItalic );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 7, SQL_C_WCHAR, &dataFontUnderline, 3, &cbDataFontUnderline );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 10, SQL_C_WCHAR, &dataFontName, 22, &cbDataFontName );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 11, SQL_C_SSHORT, &headingFontSize, 0, &cbHeadingFontSize );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 12, SQL_C_SSHORT, &headingFontWeight, 0, &cbHeadingFontWeight );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 13, SQL_C_SSHORT, &headingFontItalic, 3, &cbHeadingFontItalic );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 14, SQL_C_SSHORT, &headingFontUnderline, 3, &cbHeadingFontUnderline );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 17, SQL_C_WCHAR, &headingFontName, 22, &cbHeadingFontName );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 18, SQL_C_SSHORT, &labelFontSize, 0, &cbLabelFontSize );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 19, SQL_C_SSHORT, &labelFontWeight, 0, &cbLabelFontWeight );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 20, SQL_C_SSHORT, &labelFontItalic, 3, &cbLabelFontItalic );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 21, SQL_C_SSHORT, &labelFontUnderline, 3, &cbLabelFontUnderline );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 24, SQL_C_WCHAR, &labelFontName, 22, &cbLabelFontName );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_tableProp, 25, SQL_C_WCHAR, &comments, 225, &cbComment );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLFetch( stmt_tableProp );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        std::wstring name;
        table->SetDataFontSize( dataFontSize );
        table->SetDataFontWeight( dataFontWeight );
        table->SetDataFontItalic( dataFontItalic[0] == 'Y' );
        table->SetDataFontUnderline( dataFontUnderline[0] == 'Y' );
        str_to_uc_cpy( name, dataFontName );
        table->SetDataFontName( name );
        name = L"";
        table->SetHeadingFontSize( headingFontSize );
        table->SetHeadingFontWeight( headingFontWeight );
        table->SetHeadingFontItalic( headingFontItalic[0] == 'Y' );
        table->SetHeadingFontUnderline( headingFontUnderline[0] == 'Y' );
        str_to_uc_cpy( name, headingFontName );
        table->SetHeadingFontName( name );
        name = L"";
        table->SetLabelFontSize( labelFontSize );
        table->SetLabelFontWeight( labelFontWeight );
        table->SetLabelFontItalic( labelFontItalic[0] == 'Y' );
        table->SetLabelFontUnderline( labelFontUnderline[0] == 'Y' );
        str_to_uc_cpy( name, labelFontName );
        table->SetLabelFontName( name );
        name = L"";
        str_to_uc_cpy( name, comments );
        table->SetComment( name );
        name = L"";
    }
    else if( ret != SQL_NO_DATA )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLFreeHandle( SQL_HANDLE_STMT, stmt_tableProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLDisconnect( hdbc_tableProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    ret = SQLFreeHandle( SQL_HANDLE_DBC, hdbc_tableProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_tableProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return 1;
    }
    delete qry;
    qry = NULL;
    delete table_name;
    table_name = NULL;
    delete owner_name;
    owner_name = NULL;
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
    RETCODE rets = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( rets == SQL_SUCCESS || rets == SQL_SUCCESS_WITH_INFO )
    {
        SQLRETURN ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 1, m_hstmt );
            result = 1;
        }
        else
        {
            rets = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
            if( rets != SQL_SUCCESS && rets != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 1, m_hstmt );
                result = 1;
            }
            else
            {
                std::wstring tableName = const_cast<DatabaseTable *>( table )->GetTableName();
                std::wstring schemaName = const_cast<DatabaseTable *>( table )->GetSchemaName();
                std::wstring comment = const_cast<DatabaseTable *>( table )->GetComment();
                std::wstring tableOwner = const_cast<DatabaseTable *>( table )->GetTableOwner();
                int tableId = const_cast<DatabaseTable *>( table )->GetTableId();
                delete qry;
                qry = NULL;
                exist = IsTablePropertiesExist( table, errorMsg );
                if( errorMsg.size() != 0 )
                    result = 1;
                else
                {
                    if( exist )
                    {
                        command = L"UPDATE \"abcattbl\" SET \"abt_tnam\" = \'";
                        command += tableName;
                        command += L"\', \"abt_tid\" = ";
                        istr << tableId;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abt_ownr\" = \'";
                        command += tableOwner;
                        command += L"\',  \"abd_fhgt\" = ";
                        istr << properties.m_dataFontSize;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abd_fwgt\" = ";
                        istr << properties.m_isDataFontBold;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abd_fitl\" = \'";
                        command += properties.m_isDataFontItalic ? L"Y" : L"N";
                        command += L"\', \"abd_funl\" = \'";
                        command += properties.m_isDataFontUnderlined ? L"Y" : L"N";
                        command += L"\', \"abd_fchr\" = ";
                        istr << properties.m_dataFontEncoding;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abd_fptc\" = ";
                        istr << properties.m_dataFontSize;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abd_ffce\" = \'";
                        command += properties.m_dataFontName;
                        command += L"\',  \"abh_fhgt\" = ";
                        istr << properties.m_headingFontSize;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abd_fwgt\" = ";
                        istr << properties.m_isHeadingFontBold;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abh_fitl\" = \'";
                        command += properties.m_isHeadingFontItalic ? L"Y" : L"N";
                        command += L"\', \"abh_funl\" = \'";
                        command += properties.m_isHeadingFontUnderlined ? L"Y" : L"N";
                        command += L"\', \"abh_fchr\" = ";
                        istr << properties.m_headingFontEncoding;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abh_fptc\" = ";
                        istr << properties.m_headingFontSize;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abh_ffce\" = \'";
                        command += properties.m_headingFontName;
                        command += L"\',  \"abl_fhgt\" = ";
                        istr << properties.m_labelFontSize;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abl_fwgt\" = ";
                        istr << properties.m_isLabelFontBold;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abl_fitl\" = \'";
                        command += properties.m_isLabelFontItalic ? L"Y" : L"N";
                        command += L"\', \"abl_funl\" = \'";
                        command += properties.m_isLabelFontUnderlined ? L"Y" : L"N";
                        command += L"\', \"abl_fchr\" = ";
                        istr << properties.m_labelFontEncoding;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abl_fptc\" = ";
                        istr << properties.m_labelFontSize;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \"abl_ffce\" = \'";
                        command += properties.m_labelFontName;
                        command += L"\', \"abt_cmnt\" = \'";
                        command += comment;
                        command += L"\' WHERE \"abt_tnam\" = \'";
                        command += tableName;
                        command += L"\' AND \"abt_tid\" = ";
                        istr << tableId;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L" AND \"abt_ownr\" = \'";
                        command += pimpl->m_connectedUser;
                        command += L"\';";
                    }
                    else
                    {
                        command = L"INSERT INTO \"abcattbl\" VALUES( \'";
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
                        istr << properties.m_isDataFontBold;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \'";
                        command += properties.m_isDataFontItalic ? L"Y" : L"N";
                        command += L"\', \'";
                        command += properties.m_isDataFontUnderlined ? L"Y" : L"N";
                        command += L"\', ";
                        istr << properties.m_dataFontEncoding;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", ";
                        istr << properties.m_dataFontSize;
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
                        istr << properties.m_isHeadingFontBold;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \'";
                        command += properties.m_isHeadingFontItalic ? L"Y" : L"N";
                        command += L"\', \'";
                        command += properties.m_isHeadingFontUnderlined ? L"Y" : L"N";
                        command += L"\', ";
                        istr << properties.m_headingFontEncoding;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", ";
                        istr << properties.m_headingFontSize;
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
                        istr << properties.m_isLabelFontBold;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \'";
                        command += properties.m_isLabelFontItalic ? L"Y" : L"N";
                        command += L"\', \'";
                        command += properties.m_isLabelFontUnderlined ? L"Y" : L"N";
                        command += L"\', ";
                        istr << properties.m_labelFontEncoding;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", ";
                        istr << properties.m_labelFontSize;
                        command += istr.str();
                        istr.clear();
                        istr.str( L"" );
                        command += L", \'";
                        command += properties.m_labelFontName;
                        command += L"\', \'";
                        command += comment;
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
                            GetErrorMessage( errorMsg, 1, m_hstmt );
                            delete qry;
                            qry = NULL;
                            result = 1;
                        }
                        else
                        {
                            ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errorMsg, 1, m_hstmt );
                                delete qry;
                                qry = NULL;
                                result = 1;
                            }
                        }
                        delete qry;
                        qry = NULL;
                    }
                }
            }
        }
    }
    else
    {
        GetErrorMessage( errorMsg, 2 );
        result = 1;
    }
    delete qry;
    qry = NULL;
    if( result == 1 )
        query = L"ROLLBACK";
	else
        query = L"COMMIT";
    qry = new SQLWCHAR[query.length() + 2];
    memset( qry, '\0', query.length() + 2 );
    uc_to_str_cpy( qry, query );
    SQLRETURN ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
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
    else
        m_hstmt = 0;
    return result;
}

bool ODBCDatabase::IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    bool result = false;
    SQLLEN cbTableName = SQL_NTS, cbSchemaName = SQL_NTS, cbTableId = 0;
    std::wstring query = L"SELECT 1 FROM abcattbl WHERE abt_tnam = ? AND abt_ownr = ? AND \"abt_tid\" = ?;";
    std::wstring tname = const_cast<DatabaseTable *>( table )->GetSchemaName() + L".";
    tname += const_cast<DatabaseTable *>( table )->GetTableName();
    std::wstring ownerName = const_cast<DatabaseTable *>( table )->GetTableOwner();
    int tableId = const_cast<DatabaseTable *>( table )->GetTableId();
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
        GetErrorMessage( errorMsg, 1, m_hstmt );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
        return false;
    }
    ret = SQLBindParameter( m_hstmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, tname.length(), 0, table_name, 0, &cbTableName );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, m_hstmt );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete owner_name;
        owner_name = NULL;
    }
    else
    {
        ret = SQLBindParameter( m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, ownerName.length(), 0, owner_name, 0, &cbSchemaName );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 1, m_hstmt );
            delete qry;
            qry = NULL;
            delete table_name;
            table_name = NULL;
            delete owner_name;
            owner_name = NULL;
        }
        else
        {
            ret = SQLBindParameter( m_hstmt, 3, SQL_PARAM_INPUT, SQL_C_ULONG, SQL_INTEGER, 0, 0, &tableId, 0, &cbTableId );
            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 1, m_hstmt );
                delete qry;
                qry = NULL;
                delete table_name;
                table_name = NULL;
                delete owner_name;
                owner_name = NULL;
            }
            else
            {
                ret = SQLPrepare( m_hstmt, qry, SQL_NTS );
                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 1, m_hstmt );
                    delete qry;
                    qry = NULL;
                    delete table_name;
                    table_name = NULL;
                    delete owner_name;
                    owner_name = NULL;
                }
                else
                {
                    ret = SQLExecute( m_hstmt );
                    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                    {
                        ret = SQLFetch( m_hstmt );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                            result = true;
                        else if( ret != SQL_NO_DATA )
                        {
                            GetErrorMessage( errorMsg, 1, m_hstmt );
                            delete qry;
                            qry = NULL;
                            delete table_name;
                            table_name = NULL;
                            delete owner_name;
                            owner_name = NULL;
                        }
                    }
                    else if( ret != SQL_NO_DATA )
                    {
                        GetErrorMessage( errorMsg, 1, m_hstmt );
                        delete qry;
                        qry = NULL;
                        delete table_name;
                        table_name = NULL;
                        delete owner_name;
                        owner_name = NULL;
                    }
                }
            }
        }
    }
    delete qry;
    qry = NULL;
    delete table_name;
    table_name = NULL;
    delete owner_name;
    owner_name = NULL;
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, m_hstmt );
    }
    m_hstmt = 0;
    return result;
}

int ODBCDatabase::GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &fieldName, Field *field, std::vector<std::wstring> &errorMsg)
{
    SQLHDBC hdbc_fieldProp;
    SQLHSTMT stmt_fieldProp;
    std::wstring query;
    int result = 0;
    SQLWCHAR *commentField;
    SQLWCHAR *table_name = NULL, *schema_name = NULL, *field_name = NULL, *qry = NULL;
    SQLLEN cbSchemaName = SQL_NTS, cbTableName = SQL_NTS, cbFieldName = SQL_NTS, cbDataFontItalic;
    SQLSMALLINT OutConnStrLen;
    if( pimpl->m_subtype == L"MySQL" )
        query = L"SELECT * FROM abcatcol WHERE \"abc_tnam\" = ? AND \"abc_ownr\" = ? AND \"abc_cnam\" = ?;";
    else
        query = L"SELECT * FROM \"abcatcol\" WHERE \"abc_tnam\" = ? AND \"abc_ownr\" = ? AND \"abc_cnam\" = ?;";
    table_name = new SQLWCHAR[tableName.length() + 2];
    schema_name = new SQLWCHAR[schemaName.length() + 2];
    field_name = new SQLWCHAR[fieldName.length() + 2];
    qry = new SQLWCHAR[query.length() + 2];
    memset( table_name, '\0', tableName.length() + 2 );
    memset( schema_name, '\0', schemaName.length() + 2 );
    memset( field_name, '\0', fieldName.length() + 2 );
    memset( qry, '\0', query.length() + 2 );
    uc_to_str_cpy( table_name, tableName );
    uc_to_str_cpy( schema_name, schemaName );
    uc_to_str_cpy( field_name, fieldName );
    uc_to_str_cpy( qry, query );
    SQLRETURN ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &hdbc_fieldProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 0, m_env );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        delete field_name;
        field_name = NULL;
        return 1;
    }
    ret = SQLDriverConnect( hdbc_fieldProp, NULL, m_connectString, SQL_NTS, NULL, 0, &OutConnStrLen, SQL_DRIVER_NOPROMPT );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 2, m_env );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        delete field_name;
        field_name = NULL;
        return 1;
    }
    ret = SQLAllocHandle( SQL_HANDLE_STMT, hdbc_fieldProp, &stmt_fieldProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 2, m_hdbc );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        delete field_name;
        field_name = NULL;
        return 1;
    }
    ret = SQLBindParameter( stmt_fieldProp, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, tableName.length(), 0, table_name, 0, &cbTableName );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_fieldProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        delete field_name;
        field_name = NULL;
        return 1;
    }
    ret = SQLBindParameter( stmt_fieldProp, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, schemaName.length(), 0, schema_name, 0, &cbSchemaName );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_fieldProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        delete field_name;
        field_name = NULL;
        return 1;
    }
    ret = SQLBindParameter( stmt_fieldProp, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, fieldName.length(), 0, field_name, 0, &cbFieldName );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_fieldProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        delete field_name;
        field_name = NULL;
        return 1;
    }
    ret = SQLPrepare( stmt_fieldProp, qry, SQL_NTS );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_fieldProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        delete field_name;
        field_name = NULL;
        return 1;
    }
    ret = SQLExecute( stmt_fieldProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_fieldProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        delete field_name;
        field_name = NULL;
        return 1;
    }
    ret = SQLBindCol( stmt_fieldProp, 18, SQL_C_WCHAR, &commentField, 3, &cbDataFontItalic );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_fieldProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        delete field_name;
        field_name = NULL;
        return 1;
    }
    ret = SQLFetch( stmt_fieldProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA )
    {
        GetErrorMessage( errorMsg, 1, stmt_fieldProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        delete field_name;
        field_name = NULL;
        return 1;
    }
    else if( ret != SQL_NO_DATA )
    {
        std::wstring comment;
        str_to_uc_cpy( comment, commentField );
        field->SetComment( comment );
    }
    ret = SQLFreeHandle( SQL_HANDLE_STMT, stmt_fieldProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt_fieldProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        return 1;
    }
    ret = SQLDisconnect( hdbc_fieldProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, hdbc_fieldProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        return 1;
    }
    ret = SQLFreeHandle( SQL_HANDLE_DBC, hdbc_fieldProp );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, hdbc_fieldProp );
        delete qry;
        qry = NULL;
        delete table_name;
        table_name = NULL;
        delete schema_name;
        schema_name = NULL;
        return 1;
    }
    delete table_name;
    delete schema_name;
    delete field_name;
    delete qry;
    return result;
}

int ODBCDatabase::ApplyForeignKey(const std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, std::vector<std::wstring> &errorMsg)
{
    tableName = tableName;
    int result = 0;
    SQLRETURN ret;
    ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 2, m_hstmt );
        result = 1;
    }
    else
    {
        SQLWCHAR *query = new SQLWCHAR[command.length() + 2];
        memset( query, '\0', command.size() + 2 );
        uc_to_str_cpy( query, command );
        ret = SQLExecDirect( m_hstmt, query, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2, m_hstmt );
            result = 1;
        }
        delete query;
        query = NULL;
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2, m_hstmt );
            result = 1;
        }
        else
            m_hstmt = 0;
    }
    return result;
}

int ODBCDatabase::DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"DROP TABLE ";
    query += tableName;
    SQLRETURN ret;
    ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &m_hstmt );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 2, m_hstmt );
        result = 1;
    }
    else
    {
        SQLWCHAR *qry = new SQLWCHAR[query.length() + 2];
        memset( qry, '\0', query.length() + 2 );
        uc_to_str_cpy( qry, query );
        ret = SQLExecDirect( m_hstmt, qry, SQL_NTS );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2, m_hstmt );
            result = 1;
        }
        delete qry;
        qry = NULL;
        ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
        if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2, m_hstmt );
            result = 1;
        }
        else
            m_hstmt = 0;
    }
    return result;
}

int ODBCDatabase::SetFieldProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    return res;
}

int ODBCDatabase::GetTableId(const DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    SQLHSTMT stmt;
    SQLHDBC hdbc;
    SQLLEN cbName, cbTableName = SQL_NTS;
    long id;
    int result = 0;
    std::wstring query;
    if( pimpl->m_subtype == L"Microsoft SQL Server" )
        query = L"SELECT OBJECT_ID(?);";
    if( pimpl->m_subtype == L"PostgreSQL" )
        query = L"SELECT oid FROM pg_class WHERE relname = ?";
    std::wstring tableName = const_cast<DatabaseTable *>( table )->GetTableName();
    SQLWCHAR *qry = new SQLWCHAR[query.length() + 2];
    SQLWCHAR *tname = new SQLWCHAR[tableName.length() + 2];
    memset( qry, '\0', query.length() + 2 );
    memset( tname, '\0', tableName.length() + 2 );
    uc_to_str_cpy( qry, query );
    uc_to_str_cpy( tname, tableName );
    SQLRETURN retcode = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &hdbc );
    if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 0 );
        result = 1;
    }
    else
    {
        SQLSMALLINT OutConnStrLen;
        retcode = SQLDriverConnect( hdbc, NULL, m_connectString, SQL_NTS, NULL, 0, &OutConnStrLen, SQL_DRIVER_NOPROMPT );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2, hdbc );
            result = 1;
        }
        else
        {
            retcode = SQLAllocHandle( SQL_HANDLE_STMT, hdbc, &stmt );
            if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 2, hdbc );
                result = 1;
            }
            else
            {
                retcode = SQLBindParameter( stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, tableName.length(), 0, tname, 0, &cbTableName );
                if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 2, hdbc );
                    result = 1;
                }
                else
                {
                    retcode = SQLPrepare( stmt, qry, SQL_NTS );
                    if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
                    {
                        GetErrorMessage( errorMsg, 2, hdbc );
                        result = 1;
                    }
                    else
                    {
                        retcode = SQLExecute( stmt );
                        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
                        {
                            GetErrorMessage( errorMsg, 2, hdbc );
                            result = 1;
                        }
                        else
                        {
                            retcode = SQLFetch( stmt );
                            if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
                            {
                                GetErrorMessage( errorMsg, 1, stmt );
                                result = 1;
                            }
                            else
                            {
                                retcode = SQLGetData( stmt, 1, SQL_C_SLONG, &id, 0, &cbName );
                                if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
                                {
                                    const_cast<DatabaseTable *>( table )->SetTableId( id );
                                }
                                else
                                {
                                    GetErrorMessage( errorMsg, 1, stmt );
                                    result = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    delete qry;
    qry = NULL;
    delete tname;
    tname = NULL;
    if( stmt )
    {
        retcode = SQLFreeHandle( SQL_HANDLE_STMT, stmt );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 1, stmt );
            result = 1;
        }
        else
        {
            stmt = 0;
            retcode = SQLDisconnect( hdbc );
            if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 2, hdbc );
                result = 1;
            }
            else
            {
                retcode = SQLFreeHandle( SQL_HANDLE_DBC, hdbc );
                if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
                {
                    GetErrorMessage( errorMsg, 2, hdbc );
                    result = 1;
                }
                else
                    hdbc = 0;
            }
        }
    }
    return result;
}

int ODBCDatabase::SetTableOwner(DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    SQLHSTMT stmt;
    SQLHDBC hdbc;
    SQLLEN cbTableName = SQL_NTS;
    SQLLEN cbName;
    SQLWCHAR owner[1024];
    SQLWCHAR *table_name = NULL, *qry;
    int result = 0;
    std::wstring query, name = table->GetTableName();
    if( pimpl->m_subtype == L"Microsoft SQL Server" )
        query = L"SELECT su.name FROM sysobjects so, sysusers su WHERE so.uid = su.uid AND so.name = ?";
    if( pimpl->m_subtype == L"PostgreSQL" )
        query = L"SELECT u.usename FROM pg_class c, pg_user u WHERE u.usesysid = c.relowner AND relname = ?";
    SQLRETURN retcode = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &hdbc );
    if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 0 );
        result = 1;
    }
    else
    {
        SQLSMALLINT OutConnStrLen;
        retcode = SQLDriverConnect( hdbc, NULL, m_connectString, SQL_NTS, NULL, 0, &OutConnStrLen, SQL_DRIVER_NOPROMPT );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 2, hdbc );
            result = 1;
        }
        else
        {
            retcode = SQLAllocHandle( SQL_HANDLE_STMT, hdbc, &stmt );
            if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 2, hdbc );
                result = 1;
            }
            else
            {
                table_name = new SQLWCHAR[name.length() + 2];
                qry = new SQLWCHAR[query.length() + 2];
                memset( qry, '\0', query.size() + 2 );
                memset( table_name, '\0', name.length() + 2 );
                uc_to_str_cpy( qry, query );
                uc_to_str_cpy( table_name, name );
                retcode = SQLBindParameter( stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, name.length(), 0, table_name, 0, &cbTableName );
                if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
                {
                    retcode = SQLPrepare( stmt, qry, SQL_NTS );
                    if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
                    {
                        retcode = SQLExecute( stmt );
                        if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
                        {
                            retcode = SQLFetch( stmt );
                            if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
                            {
                                retcode = SQLGetData( stmt, 1, SQL_C_WCHAR, owner, 1024, &cbName );
                                if( retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
                                {
                                    std::wstring tableOwner;
                                    str_to_uc_cpy( tableOwner, owner );
                                    table->SetTableOwner( tableOwner );
                                }
                                else
                                {
                                    GetErrorMessage( errorMsg, 1, stmt );
                                    result = 1;
                                }
                            }
                            else if( retcode != SQL_NO_DATA )
                            {
                                GetErrorMessage( errorMsg, 1, stmt );
                                result = 1;
                            }
                            else
                                table->SetTableOwner( table->GetSchemaName() );
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, 1, stmt );
                            result = 1;
                        }
                    }
                    else
                    {
                        GetErrorMessage( errorMsg, 1, stmt );
                        result = 1;
                    }
                }
                else
                {
                    GetErrorMessage( errorMsg, 1, stmt );
                    result = 1;
                }
                delete table_name;
                table_name = NULL;
                delete qry;
                qry = NULL;
            }
        }
    }
    retcode = SQLFreeHandle( SQL_HANDLE_STMT, stmt );
    if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
    {
        GetErrorMessage( errorMsg, 1, stmt );
        result = 1;
    }
    else
    {
        stmt = 0;
        retcode = SQLDisconnect( hdbc );
        if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
        {
            GetErrorMessage( errorMsg, 1, stmt );
            result = 1;
        }
        else
        {
            retcode = SQLFreeHandle( SQL_HANDLE_DBC, hdbc );
            if( retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO )
            {
                GetErrorMessage( errorMsg, 1, stmt );
                result = 1;
            }
            else
                hdbc = 0;
        }
    }
    return result;
}


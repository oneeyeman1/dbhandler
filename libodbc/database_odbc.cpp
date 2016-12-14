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
#include <algorithm>
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

int ODBCDatabase::Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg)
{
    int result = 0, bufferSize = 1024;
    std::vector<SQLWCHAR *> errorMessage;
    SQLWCHAR connectStrIn[sizeof(SQLWCHAR) * 255], driver[1024], dsn[1024], dbType[1024], *query = NULL;
    SQLSMALLINT OutConnStrLen;
    SQLRETURN ret;
    SQLUSMALLINT options;
    std::wstring query1, query2, query3, query4, query5;
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
                                    query1 = L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatcol' AND xtype='U') CREATE TABLE abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254) PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));";
                                    query2 = L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatedt' AND xtype='U') CREATE TABLE abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32) PRIMARY KEY( abe_name, abe_seqn ));";
                                    query3 = L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatfmt' AND xtype='U') CREATE TABLE abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer PRIMARY KEY( abf_name ));";
                                    query4 = L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcattbl' AND xtype='U') CREATE TABLE abcattbl(abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254) PRIMARY KEY( abt_tnam, abt_ownr ));";
                                    query5 = L"IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='abcatvld' AND xtype='U') CREATE TABLE abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254) PRIMARY KEY( abv_name ));";
                                }
                                if( pimpl->m_subtype == L"MySQL" || pimpl->m_subtype == L"PostgreSQL" )
                                {
                                    query1 = L"CREATE TABLE IF NOT EXISTS abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));";
                                    query2 = L"CREATE TABLE IF NOT EXISTS abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));";
                                    query3 = L"CREATE TABLE IF NOT EXISTS abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));";
                                    query4 = L"CREATE TABLE IF NOT EXISTS abcattbl(abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ));";
                                    query5 = L"CREATE TABLE IF NOT EXISTS abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));";
                                }
                                if( pimpl->m_subtype == L"Oracle" )
                                {
                                    query1 = L"IF( SELECT count(*) FROM all_tables WHERE table_name = 'abcatcol' ) <= 0 CREATE TABLE abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));";
                                    query2 = L"IF( SELECT count(*) FROM all_tables WHERE table_name = 'abcatedt' ) <= 0 CREATE TABLE abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));";
                                    query3 = L"IF( SELECT count(*) FROM all_tables WHERE table_name = 'abcatfmt' ) <= 0 CREATE TABLE abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));";
                                    query4 = L"IF( SELECT count(*) FROM all_tables WHERE table_name = 'abcattbl' ) <= 0 CREATE TABLE abcattbl(abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abl_tnam, abl_ownr ));";
                                    query5 = L"IF( SELECT count(*) FROM all_tables WHERE table_name = 'abcatvld' ) <= 0 CREATE TABLE abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));";
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
                                                        ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT );
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
/*                            if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                            {
                                if( !result )
                                {
                                    GetErrorMessage( errorMsg, 2 );
                                    result = 1;
                                }
                                ret = SQLEndTran( SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK );
                            }*/
                            if( m_hstmt )
                            {
                                ret = SQLFreeHandle( SQL_HANDLE_STMT, m_hstmt );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errorMsg, 2 );
                                    result = 1;
                                }
                            }
                            if( !result )
                            {
                                ret = SQLSetConnectAttr( m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) TRUE, 0 );
                                if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
                                {
                                    GetErrorMessage( errorMsg, 2 );
                                    result = 1;
                                }
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
    delete query;
    query = NULL;
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
    SQLWCHAR *catalogName, *schemaName, *tableName;
    SQLHSTMT stmt_col = 0, stmt_pk = 0, stmt_colattr = 0, stmt_fk = 0;
    SQLHDBC hdbc_col = 0, hdbc_pk = 0, hdbc_colattr = 0, hdbc_fk = 0;
    SQLWCHAR szColumnName[256], szTypeName[256], szRemarks[256], szColumnDefault[256], szIsNullable[256], pkName[SQL_MAX_COLUMN_NAME_LEN + 1], dbName[1024];
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
                            str_to_uc_cpy( fieldName, szColumnName );
                            str_to_uc_cpy( fieldType, szTypeName );
                            str_to_uc_cpy( defaultValue, szColumnDefault );
                            Field *field = new Field( fieldName, fieldType, ColumnSize, DecimalDigits, defaultValue, Nullable == 1, std::find( autoinc_fields.begin(), autoinc_fields.end(), fieldName ) == autoinc_fields.end(), std::find( pk_fields.begin(), pk_fields.end(), fieldName ) != pk_fields.end(), std::find( fk_fieldNames.begin(), fk_fieldNames.end(), fieldName ) != fk_fieldNames.end() );
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
                std::wstring catalog_name, schema_name, table_name, comment = L"";
                str_to_uc_cpy( catalog_name, catalogName );
                str_to_uc_cpy( schema_name, schemaName );
                str_to_uc_cpy( table_name, tableName );
                if( pimpl->m_subtype == L"Microsoft SQL Server" && schema_name == L"sys" )
                    table_name = schema_name + L"." + table_name;
                DatabaseTable *table = new DatabaseTable( table_name, schema_name, fields, foreign_keys );
                GetTableComments( table_name, comment, errorMsg );
                table->SetComment( comment );
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
    }
    for( int i = 0; i < 5; i++ )
    {
        free( catalog[i].TargetValuePtr );
    }
    free( catalog );
    return result;
}

int ODBCDatabase::CreateIndex(std::wstring &command, bool isUnique, bool isAscending, const std::wstring &indexName, const std::wstring &tableName, const std::vector<std::wstring> &fields, bool logOnly, std::vector<std::wstring> &errorMsg)
{
    command = L"CREATE ";
    if( isUnique )
        command += L"UNIQUE ";
    command += L"INDEX ";
    command += indexName;
    command += L" ON " + tableName + L"(";
    for( std::vector<std::wstring>::const_iterator it = fields.begin(); it < fields.end(); it++ )
    {
        command += (*it);
        if( isAscending )
            command += L" ASC ";
        else
            command += L" DESC ";
        if( it < fields.end() - 1 )
            command += L",";
        else
            command += L")";
    }
    if( !logOnly )
    {
    }
    return 0;
}

void ODBCDatabase::GetTableComments(const std::wstring &tableName, std::wstring &comment, std::vector<std::wstring> &errorMsg)
{
    SQLLEN owner_size = SQL_NTS, name_size = SQL_NTS;
    SQLHDBC dbc_comment;
    SQLHSTMT stmt_comment = 0;
    SQLWCHAR sql_comment[254], *temp = NULL, *sql_name = NULL, *sql_owner = NULL;
    SQLLEN sql_comment_length = 0;
    int pos = tableName.find( '.' );
    std::wstring owner = tableName.substr( 0, pos );
    std::wstring name = tableName.substr( pos + 1 );
    std::wstring query = L"SELECT abt_cmnt FROM abcattbl WHERE abt_tnam = ? AND abt_ownr = ?;";
    RETCODE ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &dbc_comment );
    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
    {
        SQLSMALLINT OutConnStrLen;
        ret = SQLDriverConnect( dbc_comment, NULL, m_connectString, SQL_NTS, NULL, 0, &OutConnStrLen, SQL_DRIVER_NOPROMPT );
        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
        {
            ret = SQLAllocHandle( SQL_HANDLE_STMT, dbc_comment, &stmt_comment );
            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
            {
                temp = new SQLWCHAR[query.length() + 2];
                memset( temp, '\0', query.length() + 2 );
                uc_to_str_cpy( temp, query );
                sql_owner = new SQLWCHAR[owner.length() + 2];
                memset( sql_owner, '\0', owner.length() + 2 );
                uc_to_str_cpy( sql_owner, owner );
                sql_name = new SQLWCHAR[name.length() + 2];
                memset( sql_name, '\0', name.length() + 2 );
                uc_to_str_cpy( sql_name, name );
                ret = SQLPrepare( stmt_comment, temp, SQL_NTS );
                if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                {
                    ret = SQLBindParameter( stmt_comment, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, name.length() + 2, 0, sql_name, 0, &name_size );
                    if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                    {
                        ret = SQLBindParameter( stmt_comment, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, owner.length() + 2, 0, sql_owner, 0, &owner_size );
                        if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                        {
                            ret = SQLExecute( stmt_comment );
                            if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                            {
                                ret = SQLBindCol( stmt_comment, 1, SQL_C_WCHAR, &sql_comment, 254 * 2, &sql_comment_length );
                                if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
                                {
                                    ret = SQLFetch( stmt_comment );
                                    if( ret != SQL_NO_DATA )
                                    {
                                        if( ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) && ret != SQL_NO_DATA )
                                        {
                                            str_to_uc_cpy( comment, sql_comment );
                                        }
                                        else
                                        {
                                            GetErrorMessage( errorMsg, 1, stmt_comment );
                                        }
                                    }
                                }
                                else
                                {
                                    GetErrorMessage( errorMsg, 1, stmt_comment );
                                }
                            }
                            else
                            {
                                GetErrorMessage( errorMsg, 1, stmt_comment );
                            }
                        }
                        else
                        {
                            GetErrorMessage( errorMsg, 1, stmt_comment );
                        }
                    }
                    else
                    {
                        GetErrorMessage( errorMsg, 1, stmt_comment );
                    }
                }
                else
                {
                    GetErrorMessage( errorMsg, 1, stmt_comment );
                }
            }
            else
            {
                GetErrorMessage( errorMsg, 2, dbc_comment );
            }
        }
        else
        {
            GetErrorMessage( errorMsg, 2, dbc_comment );
        }
    }
    else
    {
        GetErrorMessage( errorMsg, 0, m_env );
    }
    delete temp;
    delete sql_name;
    delete sql_owner;
    ret = SQLFreeHandle( SQL_HANDLE_STMT, stmt_comment );
    ret = SQLDisconnect( dbc_comment );
    ret = SQLFreeHandle( SQL_HANDLE_DBC, dbc_comment );
}

void ODBCDatabase::SetTableComments(const std::wstring &tableName, const std::wstring &comment, std::vector<std::wstring> &errorMsg)
{
}

void ODBCDatabase::GetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, std::wstring &comment, std::vector<std::wstring> &errorMsg)
{
}

void ODBCDatabase::SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg)
{
}

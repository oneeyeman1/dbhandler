#ifdef WIN32
#include <windows.h>
#pragma execution_character_set("utf-8")
#endif

#if defined __WXMSW__ && defined __MEMORYLEAKS__
#include <vld.h>
#endif

#include <stdio.h>
#include <map>
#include <memory>
#include <vector>
#include <string.h>
#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>
#if defined( __DBGTK__ ) || defined( __DBQT__ )
#include <arpa/inet.h>
#endif
#include <sstream>
#include <libpq-fe.h>
#include "database.h"
#include "database_postgres.h"

std::mutex Impl::my_mutex;

PostgresDatabase::PostgresDatabase(const int osId, const std::wstring &desktop) : Database( osId, desktop )
{
    m_db = NULL;
    m_pimpl = NULL;
    connectToDatabase = false;
    m_isConnected = false;
    m_fieldsInRecordSet = 0;
}

PostgresDatabase::~PostgresDatabase()
{
    delete m_pimpl;
    m_pimpl = NULL;
}

int PostgresDatabase::CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::vector<std::wstring> dbList;
    result = Disconnect( errorMsg );
//    if( !result )
//        result = Connect( name, dbList, errorMsg );
    return result;
}

int PostgresDatabase::DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    PGresult *res;
    std::wstring query = L"DROP DATABASE " + name;
    if( pimpl.m_dbName == name )
        result = Disconnect( errorMsg );
    if( !result )
    {
        res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
        if( PQresultStatus( res ) != PGRES_COMMAND_OK )
        {
            result = 1;
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error dropping database: " + err );
        }
        PQclear( res );
    }
    return result;
}

int PostgresDatabase::Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring err;
    std::size_t found = selectedDSN.find( L"dbname" );
    if( found != std::wstring::npos )
        connectToDatabase = true;
    pimpl.m_type = L"PostgreSQL";
    pimpl.m_subtype = L"";
    if( !m_pimpl )
        m_pimpl = new PostgresImpl;
    m_db = PQconnectdb( m_pimpl->m_myconv.to_bytes( selectedDSN.c_str() ).c_str() );
    if( PQstatus( m_db ) != CONNECTION_OK )
    {
        err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Connection to database failed: " + err );
        result = 1;
    }
    else
    {
        m_isConnected = true;
        result = GetServerVersion( errorMsg );
        if( result )
        {
            PQfinish( m_db );
            errorMsg.push_back( L"Problem during connection. Please fix and restart the application" );
        }
        else
        {
            pimpl.m_connectString = selectedDSN;
            pimpl.m_connectedUser = m_pimpl->m_myconv.from_bytes( PQuser( m_db ) );
            pimpl.m_dbName = m_pimpl->m_myconv.from_bytes( PQdb( m_db ) );
            if( !connectToDatabase )
            {
                if( ServerConnect( dbList, errorMsg ) )
                {
                    PQfinish( m_db );
                    errorMsg.push_back( L"Problem during connection. Please fix and restart the application" );
                    result = 1;
                }
            }
            else
            {
                if( CreateSystemObjectsAndGetDatabaseInfo( errorMsg ) )
                {
                    result = 1;
                }
                if( !result && PopulateValdators( errorMsg ) )
                    result = 1;
                if( !result && GetTablespacesList( m_pimpl->m_tablespaces, errorMsg ) )
                    result = 1;
            }
        }
    }
    return result;
}

int PostgresDatabase::ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg)
{
    PGresult *res;
    std::wstring err;
    int result = 0;
    std::wstring query = L"SELECT datname FROM pg_database";
    res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
    ExecStatusType status = PQresultStatus( res );
    if( status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK )
    {
        err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Connection to database failed: " + err );
        result = 1;
    }
    else if( status == PGRES_TUPLES_OK )
    {
        for( int i = 0; i < PQntuples( res ); i++ )
        {
            dbList.push_back( m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 0 ) ) );
        }
    }
    PQclear( res );
    return result;
}

int PostgresDatabase::CreateSystemObjectsAndGetDatabaseInfo(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    PGresult *res;
    std::vector<std::wstring> queries;
    queries.push_back( L"SET client_min_messages = WARNING;" );
    queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));" );
    queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));" );
    queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));" );
    queries.push_back( L"CREATE TABLE IF NOT EXISTS abcattbl(abt_os smallint, abt_tnam char(129), abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl integer, abd_strke integer, abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl integer, abh_strke integer, abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl integer, abl_strke integer, abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_os, abt_tnam, abt_ownr ));" );
    queries.push_back( L"CREATE TABLE IF NOT EXISTS abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));" );
    if( pimpl.m_versionMajor >= 10 )
    {
        queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcatc_x ON abcatcol( abc_tnam, abc_ownr, abc_cnam );" );
        queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcate_x ON abcatedt( abe_name, abe_seqn );" );
        queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcatf_x ON abcatfmt( abf_name );" );
        queries.push_back( L"CREATE UNIQUE INDEX IF NOT EXISTS abcatt_x ON abcattbl( abt_os, abt_tnam, abt_ownr );" );
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
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcatc_x\' AND n.nspname = \'public\' ) THEN CREATE UNIQUE INDEX \"abcatc_x\" ON \"abcatcol\"(\"abc_tnam\" ASC, \"abc_ownr\" ASC, \"abc_cnam\" ASC); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcate_x\' AND n.nspname = \'public\' ) THEN CREATE UNIQUE INDEX \"abcate_x\" ON \"abcatedt\"(\"abe_name\" ASC, \"abe_seqn\" ASC); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcatf_x\' AND n.nspname = \'public\' ) THEN CREATE UNIQUE INDEX \"abcatf_x\" ON \"abcatfmt\"(\"abf_tnam\" ASC); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcatt_x\' AND n.nspname = \'public\' ) THEN CREATE UNIQUE INDEX \"abcatt_x\" ON \"abcattbl\"(\"abt_tnam\" ASC, \"abt_ownr\" ASC); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM pg_class c, pg_namespace n WHERE n.oid = c.relnamespace AND c.relname = \'abcatv_x\' AND n.nspname = \'public\' ) THEN CREATE UNIQUE INDEX \"abcatv_x\" ON \"abcatvld\"(\"abv_name\" ASC); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'(General)\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'(General)\', \'(General)\', 81, 0 ); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'0\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'0\', \'0\', 81, 0 ); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'0.00\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'0.00\', \'0.00\', 81, 0 ); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'#.##0\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'#.##0\', \'#.##0\', 81, 0 ); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'#.##0,00\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'#.##0,00\', \'#.##0,00\', 81, 0 ); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'$#.##0;[$#.##0]\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'$#.##0;[$#.##0]\', \'$#.##0;[$#.##0]\', 81, 0 ); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'$#.##0;|RED|[$#.##0]\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'$#.##0;|RED|[$#.##0]\', \'$#.##0;|RED|[$#.##0]\', 81, 0 ); END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'$#.##0,00;[$#.##0,00]\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'$#.##0,00;[$#.##0,00]\', \'$#.##0,00;[$#.##0,00]\', 81, 0 ) END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'$#.##0,00;|RED|[$#.##0,00]\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'$#.##0,00;|RED|[$#.##0,00]\', \'$#.##0,00;|RED|[$#.##0,00]\', 81, 0 ) END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'0%\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'0%\', \'0%\', 81, 0 ) END IF; END; $$;" );
        queries.push_back( L"DO $$ BEGIN IF NOT EXISTS( SELECT 1 FROM \"abcatfmt\" WHERE abf_name = \'0.00%\' ) THEN INSERT INTO \"abcatfmt\" VALUES( \'0.00%\', \'0.00%\', 81, 0 ) END IF; END; $$;" );

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
    std::wstring err;
    res = PQexec( m_db, "START TRANSACTION" );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        result = 1;
        PQclear( res );
    }
    else
    {
        PQclear( res );
        for( std::vector<std::wstring>::iterator it = queries.begin(); it < queries.end() && !result; ++it )
        {
            res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( (*it).c_str() ).c_str() );
            if( PQresultStatus( res ) != PGRES_COMMAND_OK )
            {
                err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                errorMsg.push_back( err );
                result = 1;
            }
            PQclear( res );
        }
    }
    if( !result )
    {
        res = PQexec( m_db, "COMMIT" );
        if( PQresultStatus( res ) != PGRES_COMMAND_OK )
        {
            PQclear( res );
            err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( err );
            res = PQexec( m_db, "ROLLBACK" );
            result = 1;
        }
        else
        {
            PQclear( res );
            result = GetTableListFromDb( errorMsg );
            if( result )
            {
                errorMsg.push_back( L"Problem during connection. Please fix the problem and restart the application" );
                result = 1;
            }
        }
    }
    else
    {
        res = PQexec( m_db, "ROLLBACK" );
        if( PQresultStatus( res ) != PGRES_COMMAND_OK )
        {
            err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error during ROLLBACK: " + err );
            result = 1;
        }
        PQclear( res );
    }
    if( result )
        Disconnect( errorMsg );
    else
    {
        pimpl.m_dbName = pimpl.m_tableDefinitions.begin()->first;
    }
    return result;
}
	
int PostgresDatabase::Disconnect(std::vector<std::wstring> &UNUSED(errorMsg))
{
    std::lock_guard<std::mutex> locker( GetTableVector().my_mutex );
    int result = 0;
    if( m_isConnected )
    {
        PQfinish( m_db );
        m_isConnected = false;
    }
    for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it = pimpl.m_tables.begin(); it != pimpl.m_tables.end(); it++ )
    {
        std::vector<DatabaseTable *> tableVec = (*it).second;
        for( std::vector<DatabaseTable *>::iterator it1 = tableVec.begin(); it1 < tableVec.end(); it1++ )
        {
            delete (*it1);
            (*it1) = NULL;
        }
    }
    delete m_pimpl;
    m_pimpl = NULL;
    return result;
}

int PostgresDatabase::GetTableListFromDb(std::vector<std::wstring> &errorMsg)
{
    int osid = 0;
    PGresult *res  = nullptr, *res1  = nullptr;
    std::vector<TableField *> fields;
    std::map<int,std::vector<FKField *> > foreign_keys;
    std::wstring errorMessage;
    std::wstring fieldName, fieldType, fieldDefaultValue, origSchema, origTable, origField, refSchema, refTable, refField, fkName, fkTableField, fkUpdateConstraint, fkDeleteConstraint;
    int result = 0, count = 0;
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
    std::wstring query1 = L"SELECT t.table_catalog AS catalog, t.table_schema AS schema, t.table_name AS table, u.usename AS owner, c.oid AS table_id FROM information_schema.tables t, pg_catalog.pg_class c, pg_catalog.pg_user u WHERE t.table_name = c.relname AND c.relowner = usesysid AND (t.table_type = 'BASE TABLE' OR t.table_type = 'VIEW' OR t.table_type = 'LOCAL TEMPORARY') ORDER BY table_name;";
    std::wstring query2 = L"SELECT DISTINCT column_name, data_type, character_maximum_length, character_octet_length, numeric_precision, numeric_precision_radix, numeric_scale, is_nullable, column_default, CASE WHEN column_name IN (SELECT ccu.column_name FROM information_schema.constraint_column_usage ccu, information_schema.table_constraints tc WHERE ccu.constraint_name = tc.constraint_name AND tc.constraint_type = 'PRIMARY KEY' AND ccu.table_name = $2) THEN 'YES' ELSE 'NO' END AS is_pk, ordinal_position FROM information_schema.columns col, information_schema.table_constraints tc WHERE tc.table_schema = col.table_schema AND tc.table_name = col.table_name AND col.table_schema = $1 AND col.table_name = $2 ORDER BY ordinal_position;";
    std::wstring query3 = L"SELECT (SELECT con.oid FROM pg_constraint con WHERE con.conname = c.constraint_name) AS id, x.ordinal_position AS pos, c.constraint_name AS name, x.table_schema as schema, x.table_name AS table, x.column_name AS column, y.table_schema as ref_schema, y.table_name as ref_table, y.column_name as ref_column, c.update_rule, c.delete_rule, c.match_option FROM information_schema.referential_constraints c, information_schema.key_column_usage x, information_schema.key_column_usage y WHERE x.constraint_name = c.constraint_name AND y.ordinal_position = x.position_in_unique_constraint AND y.constraint_name = c.unique_constraint_name AND x.table_schema = $1 AND x.table_name = $2 ORDER BY c.constraint_name, x.ordinal_position;";
    std::wstring query4 = L"SELECT indexname FROM pg_indexes pi, information_schema.table_constraints tc WHERE pi.tablename = tc.table_name AND pi.indexname = tc.constraint_name AND tc.constraint_type NOT IN ('PRIMARY KEY') AND pi.schemaname = $1 AND pi.tablename = $2;";
    std::wstring query5 = L"SELECT abd_fhgt, abd_fwgt, abd_fitl, abd_funl, abd_strke, abd_fchr, abd_fptc, rtrim(abd_ffce), abh_fhgt, abh_fwgt, abh_fitl, abh_funl, abh_strke, abh_fchr, abh_fptc, rtrim(abh_ffce), abl_fhgt, abl_fwgt, abl_fitl, abl_funl, abl_strke, abl_fchr, abl_fptc, rtrim(abl_ffce), rtrim(abt_cmnt) FROM abcattbl WHERE abt_tnam = $1 AND abt_ownr = $2 AND abt_os = $3;";
    std::wstring query6 = L"SELECT * FROM \"abcatcol\" WHERE \"abc_tnam\" = $1 AND \"abc_ownr\" = $2 AND \"abc_cnam\" = $3;";
    std::wstring query7;
    if( pimpl.m_versionMajor <= 9 && pimpl.m_versionMinor < 5 )
    {
        if( osid == WINDOWS )
            query7 = L"INSERT INTO \"abcattbl\" VALUES( 0, $1, (SELECT c.oid FROM pg_class c, pg_namespace nc WHERE nc.oid = c.relnamespace AND c.relname = $2 AND nc.nspname = $3), $4, 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', \'\' );";
        else if( osid == OSX )
            query7 = L"INSERT INTO \"abcattbl\" VALUES( 4, $1, (SELECT c.oid FROM pg_class c, pg_namespace nc WHERE nc.oid = c.relnamespace AND c.relname = $2 AND nc.nspname = $3), $4, 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', \'\' );";
        else if( osid == GTK )
            query7 = L"INSERT INTO \"abcattbl\" VALUES( 1, $1, (SELECT c.oid FROM pg_class c, pg_namespace nc WHERE nc.oid = c.relnamespace AND c.relname = $2 AND nc.nspname = $3), $4, 8, 400, \'N\', \'N\', 0, 1, 0, \'Serif\', 8, 400, \'N\', \'N\', 0, 1, 0, \'Serif\', 8, 400, \'N\', \'N\', 0, 1, 0, \'Serif\', \'\' ) ON CONFLICT DO NOTHING;";
        else if( osid == QT )
            query7 = L"INSERT INTO \"abcattbl\" VALUES( 2, $1, (SELECT c.oid FROM pg_class c, pg_namespace nc WHERE nc.oid = c.relnamespace AND c.relname = $2 AND nc.nspname = $3), $4, 8, 400, \'N\', \'N\', 0, 1, 0, \'Cantrell\', 8, 400, \'N\', \'N\', 0, 1, 0, \'Cantrell\', 8, 400, \'N\', \'N\', 0, 1, 0, \'Cantrell\', \'\' ) ON CONFLICT DO NOTHING;";
    }
    else
    {
        if( osid == WINDOWS )
            query7 = L"INSERT INTO \"abcattbl\" VALUES( 0, $1, (SELECT c.oid FROM pg_class c, pg_namespace nc WHERE nc.oid = c.relnamespace AND c.relname = $2 AND nc.nspname = $3), $4, 8, 400, \'N\', \'N\', 0, 1, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 1, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 1, 0, \'MS Sans Serif\', \'\' ) ON CONFLICT DO NOTHING;";
        else if( osid == OSX )
            query7 = L"INSERT INTO \"abcattbl\" VALUES( 4, $1, (SELECT c.oid FROM pg_class c, pg_namespace nc WHERE nc.oid = c.relnamespace AND c.relname = $2 AND nc.nspname = $3), $4, 8, 400, \'N\', \'N\', 0, 1, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 1, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 1, 0, \'MS Sans Serif\', \'\' ) ON CONFLICT DO NOTHING;";
        else if( osid == GTK )
            query7 = L"INSERT INTO \"abcattbl\" VALUES( 1, $1, (SELECT c.oid FROM pg_class c, pg_namespace nc WHERE nc.oid = c.relnamespace AND c.relname = $2 AND nc.nspname = $3), $4, 8, 400, \'N\', \'N\', 0, 1, 0, \'Serif\', 8, 400, \'N\', \'N\', 0, 1, 0, \'Serif\', 8, 400, \'N\', \'N\', 0, 1, 0, \'Serif\', \'\' ) ON CONFLICT DO NOTHING;";
        else if( osid == QT )
            query7 = L"INSERT INTO \"abcattbl\" VALUES( 2, $1, (SELECT c.oid FROM pg_class c, pg_namespace nc WHERE nc.oid = c.relnamespace AND c.relname = $2 AND nc.nspname = $3), $4, 8, 400, \'N\', \'N\', 0, 1, 0, \'Cantrell\', 8, 400, \'N\', \'N\', 0, 1, 0, \'Cantrell\', 8, 400, \'N\', \'N\', 0, 1, 0, \'Cantrell\', \'\' ) ON CONFLICT DO NOTHING;";
    }
    res = PQexec( m_db, "BEGIN" );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    PQclear( res );
    if( !result )
    {
        res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str() );
        ExecStatusType status = PQresultStatus( res ); 
        if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( err );
            result = 1;
        }
    }
    if( !result )
    {
        res1 = PQprepare( m_db, "set_table_prop", m_pimpl->m_myconv.to_bytes( query7 ).c_str(), 3, nullptr );
        auto status = PQresultStatus( res1 );
        if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( err );
            result = 1;
        }
    }
    PQclear( res1 );
    if( !result )
    {
        res1 = PQprepare( m_db, "get_fkeys", m_pimpl->m_myconv.to_bytes( query3.c_str() ).c_str(), 2, NULL );
        auto status = PQresultStatus( res1 );
        if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( err );
            result = 1;
        }
    }
    PQclear( res1 );
    if( !result )
    {
        res1 = PQprepare( m_db, "get_columns", m_pimpl->m_myconv.to_bytes( query2.c_str() ).c_str(), 2, NULL );
        if( PQresultStatus( res1 ) != PGRES_COMMAND_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error executing query: " + err );
            result = 1;
        }
    }
    PQclear( res1 );
    if( !result )
    {
        res1 = PQprepare( m_db, "get_indexes", m_pimpl->m_myconv.to_bytes( query4.c_str() ).c_str(), 2, NULL );
        if( PQresultStatus( res1 ) != PGRES_COMMAND_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error executing query: " + err );
            fields.erase( fields.begin(), fields.end() );
            foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
            result = 1;
        }
    }
    PQclear( res1 );
    if( !result )
    {
        res1 = PQprepare( m_db, "get_table_prop", m_pimpl->m_myconv.to_bytes( query5.c_str() ).c_str(), 3, NULL );
        if( PQresultStatus( res1 ) != PGRES_COMMAND_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error executing query: " + err );
            result = 1;
        }
    }
    PQclear( res1 );
    if( !result )
    {
        res1 = PQprepare( m_db, "get_field_properties", m_pimpl->m_myconv.to_bytes( query6.c_str() ).c_str(), 3, NULL );
        if( PQresultStatus( res1 ) != PGRES_COMMAND_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error executing query: " + err );
            result = 1;
        }
    }
    if( !result )
    {
        res1 = PQprepare( m_db, "set_table_param", m_pimpl->m_myconv.to_bytes( query7.c_str() ).c_str(), 3, NULL );
        if( PQresultStatus( res1 ) != PGRES_COMMAND_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error executing query: " + err );
            result = 1;
        }
    }
    PQclear( res1 );
    if( !result )
     {
         char *params[4];
         std::wstring paramValues;
         int paramLength[4];
         int paramFormat[4];
         for( int i = 0; i < PQntuples( res ) && !result; i++ )
         {
             auto category = PQgetvalue( res, i, 0 );
             std::wstring cat = m_pimpl->m_myconv.from_bytes( category );
             char *schema = PQgetvalue( res, i, 1 );
             char *table = PQgetvalue( res, i, 2 );
             char *table_owner = PQgetvalue( res, i, 3 );
             pimpl.m_tableDefinitions[cat].push_back( TableDefinition( cat, m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 1 ) ),  m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 2 ) ) ) );
             count++;
             params[0] = new char[strlen( schema ) + strlen( table ) + 2];
             memset( params[0], '\0', strlen( schema ) + strlen( table ) + 2 );
             strcpy( params[0], schema );
             strcat( params[0], "." );
             strcat( params[0], table );
             params[1] = new char[strlen( table ) + 2];
             memset( params[1], '\0', strlen( table ) + 2 );
             strcpy( params[1], table );
             params[2] = new char[strlen( schema ) + 2];
             memset( params[2], '\0', strlen( schema ) + 2 );
             strcpy( params[2], schema );
             params[3] = new char[strlen( table_owner ) + 2];
             memset( params[3], '\0', strlen( table_owner ) + 2 );
             strcpy( params[3], table_owner );
             paramFormat[0] = paramFormat[1] = paramFormat[2] = paramFormat[3] = 0;
             paramLength[0] = strlen( schema ) + strlen( table ) + 1;
             paramLength[1] = strlen( table );
             paramLength[2] = strlen( schema );
             paramLength[3] = strlen( table_owner );
             res1 = PQexecPrepared( m_db, "set_table_prop", 4, params, paramLength, paramFormat, 0 );
             if( PQresultStatus( res1 ) != PGRES_COMMAND_OK )
             {
                 std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                 errorMsg.push_back( L"Error executing query: " + err );
                 result = 1;
             }
             PQclear( res1 );
             delete[] params[0];
             params[0] = nullptr;
             delete[] params[1];
             params[1] = nullptr;
             delete[] params[2];
             params[2] = nullptr;
             delete[] params[3];
             params[3] = nullptr;
         }
    }
    PQclear( res );
    if( !result )
        res = PQexec( m_db, "COMMIT" );
    else
        res = PQexec( m_db, "ROLLBACK" );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Error executing query: " + err );
        result = 1;
    }
    m_numOfTables = count;
    return result;
}

int PostgresDatabase::CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &catalogName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    PGresult *res;
    std::wstring query;
    int result = 0;
    res = PQexec( m_db, "BEGIN TRANSACTION" );
    ExecStatusType status = PQresultStatus( res );
    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        PQclear( res );
        result = 1;
    }
    else
    {
        bool exists = IsIndexExists( index_name, catalogName, schemaName, tableName, errorMsg );
        if( exists )
        {
            std::wstring temp = L"Index ";
            temp += index_name;
            temp += L" already exists.";
            errorMsg.push_back( temp );
            result = 1;
        }
        else if( !errorMsg.empty() )
            result = 1;
        else
        {
            res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() );
            status = PQresultStatus( res );
            if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                errorMsg.push_back( err );
                PQclear( res );
                result = 1;
            }
            if( result == 1 )
                query = L"ROLLBACK";
            else
                query = L"COMMIT";
            res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
            if( PQresultStatus( res ) != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                errorMsg.push_back( err );
                PQclear( res );
                result = 1;
            }
        }
    }
    return result;
}

int PostgresDatabase::DropIndex(const std::wstring &fullTableName, const std::wstring &indexName, const DropIndexOption &options, std::vector<std::wstring> &errorMsg)
{
    PGresult *res;
    int result = 0;
    std::wstring query = L"";
    query = L"DROP INDEX ";
    if( options.m_concurrent )
        query += L"CONCURRENTLY ";
    query += indexName;
    if( options.m_cascade )
        query += L" CASCADE";
    res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
    auto status = PQresultStatus( res );
    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        PQclear( res );
        result = 1;
    }
    return result;
}

bool PostgresDatabase::IsIndexExists(const std::wstring &indexName, const std::wstring &catalogName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    PGresult *res;
    bool exists = false;
    std::wstring query = L"SELECT 1 FROM " + catalogName + L".pg_catalog.pg_indexes WHERE schemaname = $1 AND tablename = $2 AND indexname = $3;";
    char *values[3];
    values[0] = NULL, values[1] = NULL, values[2] = NULL;
    values[0] = new char[schemaName.length() * sizeof( wchar_t ) + 1];
    values[1] = new char[tableName.length() * sizeof( wchar_t ) + 1];
    values[2] = new char[indexName.length() * sizeof( wchar_t ) + 1];
    memset( values[0], '\0', schemaName.length() * sizeof( wchar_t ) + 1 );
    memset( values[1], '\0', tableName.length() * sizeof( wchar_t ) + 1 );
    memset( values[2], '\0', indexName.length() * sizeof( wchar_t ) + 1 );
    strcpy( values[0], m_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str() );
    strcpy( values[1], m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str() );
    strcpy( values[2], m_pimpl->m_myconv.to_bytes( indexName.c_str() ).c_str() );
    int len1 = (int) schemaName.length() * sizeof( wchar_t );
    int len2 = (int) tableName.length() * sizeof( wchar_t );
    int len3 = (int) indexName.length() * sizeof( wchar_t );
    int length[3] = { len1, len2, len3 };
    int formats[3] = { 1, 1, 1 };
    res = PQexecParams( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), 3, NULL, values, length, formats, 0 );
    ExecStatusType status = PQresultStatus( res ); 
    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Error executing query: " + err );
        PQclear( res );
    }
    else if( status == PGRES_TUPLES_OK )
    {
        exists = 1;
    }
    delete[] values[0];
    values[0] = NULL;
    delete[] values[1];
    values[1] = NULL;
    delete[] values[2];
    values[2] = NULL;
    return exists;
}

int PostgresDatabase::GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    int result = 0, osid;
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
    std::wstring schemaName = table->GetSchemaName(), tableName = table->GetTableName(), ownerName = table->GetTableOwner();
    std::wstring t = schemaName + L".";
    t += tableName;
    char *values[3];
    unsigned long len1 = t.length() * sizeof( wchar_t ) + 1;
    unsigned long len2 = ownerName.length() * sizeof( wchar_t ) + 1;
    values[0] = new char[len1 * 2];
    values[1] = new char[len2 * 2];
    values[2] = new char[2];
    memset( values[0], '\0', len1 * 2 );
    memset( values[1], '\0', len2 * 2 );
    memset( values[2], '\0', 2 );
    strcpy( values[0], m_pimpl->m_myconv.to_bytes( t.c_str() ).c_str() );
    strcpy( values[1], m_pimpl->m_myconv.to_bytes( ownerName.c_str() ).c_str() );
    values[2] = const_cast<char *>( std::to_string( osid ).c_str() );
    int length[3] = { static_cast<int>( strlen( values[0] ) ), static_cast<int>( strlen( values[1] ) ), 2 };
    int formats[3] = { 0, 0, 0 };
    PGresult *res = PQexecPrepared( m_db, "get_table_prop", 3, values, length, formats, 0 );
    ExecStatusType status = PQresultStatus( res );
    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Error executing query: " + err );
        result = 1;
    }
    else if( status == PGRES_TUPLES_OK )
    {
        for( int i = 0; i < PQntuples( res ); i++ )
        {
            TableProperties prop;
            prop.m_dataFontSize = atoi( PQgetvalue( res, i, 0 ) );
            prop.m_dataFontWeight = atoi( PQgetvalue( res, i, 1 ) );
            prop.m_dataFontItalic = m_pimpl->m_myconv.from_bytes( (const char *) PQgetvalue( res, i, 2 ) ) == L"Y" ? true : false;
            prop.m_dataFontUnderline = atoi( PQgetvalue( res, i, 3 ) );
            prop.m_dataFontStrikethrough = atoi( PQgetvalue( res, i, 4 ) );
            prop.m_dataFontCharacterSet = atoi( PQgetvalue( res, i, 5 ) );
            prop.m_dataFontPixelSize = atoi( PQgetvalue( res, i, 6 ) );
            prop.m_dataFontName = m_pimpl->m_myconv.from_bytes( (const char *) PQgetvalue( res, i, 7 ) );
            prop.m_headingFontSize = atoi( PQgetvalue( res, i, 8 ) );
            prop.m_headingFontWeight = atoi( PQgetvalue( res, i, 9 ) );
            prop.m_headingFontItalic = m_pimpl->m_myconv.from_bytes( (const char *) PQgetvalue( res, i, 10 ) ) == L"Y" ? true : false;
            prop.m_headingFontUnderline = atoi( PQgetvalue( res, i, 11 ) );
            prop.m_headingFontStrikethrough = atoi( PQgetvalue( res, i, 12 ) );
            prop.m_headingFontCharacterSet = atoi( PQgetvalue( res, i, 13 ) );
            prop.m_headingFontPixelSize = atoi( PQgetvalue( res, i, 14 ) );
            prop.m_headingFontName = m_pimpl->m_myconv.from_bytes( (const char *) PQgetvalue( res, i, 15 ) );
            prop.m_labelFontSize = atoi( PQgetvalue( res, i, 16 ) );
            prop.m_labelFontWeight = atoi( PQgetvalue( res, i, 17 ) );
            prop.m_labelFontItalic = m_pimpl->m_myconv.from_bytes( (const char *) PQgetvalue( res, i, 18 ) ) == L"Y" ? true : false;
            prop.m_labelFontUnderline = atoi( PQgetvalue( res, i, 19 ) );
            prop.m_labelFontStrikethrough = atoi( PQgetvalue( res, i, 20 ) );
            prop.m_labelFontCharacterSer = atoi( PQgetvalue( res, i, 21 ) );
            prop.m_labelFontPixelSize = atoi( PQgetvalue( res, i, 22 ) );
            prop.m_labelFontName = m_pimpl->m_myconv.from_bytes( (const char *) PQgetvalue( res, i, 23 ) );
            prop.m_comment = m_pimpl->m_myconv.from_bytes( (const char *) PQgetvalue( res, i, 24 ) );
            table->SetTableProperties( prop );
        }
    }
    PQclear( res );
    table->SetFullName( table->GetCatalog() + L"." + table->GetSchemaName() + L"." + table->GetTableName() );
    delete[] values[0];
    values[0] = nullptr;
    delete[] values[1];
    values[1] = nullptr;
    delete[] values[2];
    values[2] = nullptr;
    return result;
}

int PostgresDatabase::SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring err;
    bool exist;
    std::wostringstream istr;
    std::wstring query = L"BEGIN TRANSACTION";
    PGresult *res;
    res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        PQclear( res );
        err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    else
    {
        std::wstring tableName = const_cast<DatabaseTable *>( table )->GetTableName();
        std::wstring schemaName = const_cast<DatabaseTable *>( table )->GetSchemaName();
        std::wstring comment = const_cast<DatabaseTable *>( table )->GetTableProperties().m_comment;
        unsigned long tableId = const_cast<DatabaseTable *>( table )->GetTableId();
        exist = IsTablePropertiesExist( table, errorMsg );
        if( errorMsg.size() != 0 )
            result = 1;
        else
        {
            if( exist )
            {
                command = L"UPDATE \"abcattbl\" SET \"abt_tnam\" = \'";
                command += schemaName;
                command += L".";
                command += tableName;
                command += L"\', \"abt_tid\" = ";
                istr << tableId;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abt_ownr\" = \'";
                command += pimpl.m_connectedUser;
                command += L"\',  \"abt_fhgt\" = ";
                istr << properties.m_dataFontSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abt_fwgt\" = ";
                istr << properties.m_dataFontWeight;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abt_fitl\" = \'";
                command += properties.m_dataFontItalic ? L"Y" : L"N";
                command += L"\', \"abt_funl\" = ";
                istr << ( properties.m_dataFontUnderline ? 1 : 0 );
                command +=  istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abt_fstr\" = ";
                istr << ( properties.m_dataFontStrikethrough ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abt_fchr\" = ";
                istr << properties.m_dataFontEncoding;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abt_fptc\" = ";
                istr << properties.m_dataFontPixelSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abt_ffce\" = \'";
                command += properties.m_dataFontName;
                command += L"\',  \"abh_fhgt\" = ";
                istr << properties.m_headingFontSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abt_fwgt\" = ";
                istr << properties.m_headingFontWeight;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abh_fitl\" = \'";
                command += properties.m_headingFontItalic ? L"Y" : L"N";
                command += L"\', \"abh_funl\" = ";
                istr << ( properties.m_headingFontUnderline ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abh_fstr\" = ";
                istr << ( properties.m_headingFontStrikethrough ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abh_fchr\" = ";
                istr << properties.m_headingFontEncoding;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abh_fptc\" = ";
                istr << properties.m_headingFontPixelSize;
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
                istr << properties.m_labelFontWeight;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abl_fitl\" = \'";
                command += properties.m_labelFontItalic ? L"Y" : L"N";
                command += L"\', \"abl_funl\" = ";
                istr << ( properties.m_labelFontUnderline ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abl_fstr\" = ";
                istr << ( properties.m_labelFontStrikethrough ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abl_fchr\" = ";
                istr << properties.m_labelFontEncoding;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abl_fptc\" = ";
                istr << properties.m_labelFontPixelSize;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L", \"abl_ffce\" = \'";
                command += properties.m_labelFontName;
                command += L"\', \"abt_cmnt\" = \'";
                command += properties.m_comment;
                command += L"\' WHERE \"abt_tnam\" = \'";
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
                command += pimpl.m_connectedUser;
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
                istr << ( properties.m_headingFontSize ? 1 : 0 );
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
                res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() );
                if( PQresultStatus( res ) != PGRES_COMMAND_OK )
                {
                    PQclear( res );
                    err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                    errorMsg.push_back( err );
                    result = 1;
                }
            }
        }
    }
    if( result == 1 )
        query = L"ROLLBACK";
    else
        query = L"COMMIT";
    res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        PQclear( res );
        err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    return result;
}

bool PostgresDatabase::IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    bool result = false;
    std::wstring query = L"SELECT 1 FROM abcattbl WHERE abt_tnam = $1 AND abt_ownr = $2;";
    std::wstring tname = const_cast<DatabaseTable *>( table )->GetSchemaName() + L".";
    tname += const_cast<DatabaseTable *>( table )->GetTableName();
    std::wstring owner = const_cast<DatabaseTable *>( table )->GetTableOwner();
    char *values[2];
    values[0] = new char[(tname.length() * sizeof( wchar_t ) + 1)];
    values[1] = new char[(owner.length() * sizeof( wchar_t ) + 1)];
    memset( values[0], '\0', tname.length() * sizeof( wchar_t ) + 1 );
    memset( values[1], '\0', owner.length() * sizeof( wchar_t ) + 1 );
    strcpy( values[0], m_pimpl->m_myconv.to_bytes( tname.c_str() ).c_str() );
    strcpy( values[1], m_pimpl->m_myconv.to_bytes( owner.c_str() ).c_str() );
//    int len1 = strlen( values[0] );
//    int len2 = strlen( values[1] );
//    int length[2] = { len1, len2 };
//    int formats[2] = { 1, 1 };
    PGresult *res = PQexecParams( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), 2, NULL, values, nullptr, nullptr, 0 );
    ExecStatusType status = PQresultStatus( res ); 
    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Error executing query: " + err );
        result = 1;
    }
    else if( status == PGRES_TUPLES_OK && PQntuples( res ) > 0 )
    {
        result = true;
    }
    PQclear( res );
    delete[] values[0];
    values[0] = NULL;
    delete[] values[1];
    values[1] = NULL;
    return result;
}

int PostgresDatabase::GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &ownerName, const std::wstring &fieldName, TableField *field, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::string table = m_pimpl->m_myconv.to_bytes( tableName.c_str() );
    std::string schema = m_pimpl->m_myconv.to_bytes( schemaName.c_str() );
    std::string owner = m_pimpl->m_myconv.to_bytes( ownerName.c_str() );
    std::string fieldNameReq = m_pimpl->m_myconv.to_bytes( fieldName.c_str() );
    std::wstring fieldFormat;
    size_t len = strlen( schema.c_str() );
    len++;
    len += strlen( table.c_str() );
    len++;
//    size_t len = tableName.length() + schemaName.length() + 2;
    char *tname = new char[len];
    memset( tname, '\0', len );
    strcpy( tname, schema.c_str() );
    strcat( tname, "." );
    strcat( tname, table.c_str() );
    char *values[3];
    values[0] = NULL, values[1] = NULL, values[2] = NULL;
    values[0] = new char[strlen( tname ) + 1];
    values[1] = new char[strlen( owner.c_str() ) + 1];
    values[2] = new char[strlen( fieldNameReq.c_str() ) + 1];
    memset( values[0], '\0', strlen( tname ) + 1 );
    memset( values[1], '\0', strlen( owner.c_str() ) + 1 );
    memset( values[2], '\0', strlen( fieldNameReq.c_str() ) + 1 );
    strcpy( values[0], tname );
    strcpy( values[1], owner.c_str() );
    strcpy( values[2], fieldNameReq.c_str() );
    PGresult *res = PQexecPrepared( m_db, "get_field_properties", 3, values, nullptr, nullptr, 0 );
    ExecStatusType status = PQresultStatus( res );
    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Error executing query: " + err );
        result = 1;
    }
    else if( status == PGRES_TUPLES_OK )
    {
        auto count = PQntuples( res );
        if( !count )
        {
            field->GetFieldProperties().m_comment = L"";
            field->GetFieldProperties().m_heading.m_heading = fieldName;
            field->GetFieldProperties().m_heading.m_headingAlignment = 1;
            field->GetFieldProperties().m_heading.m_label = fieldName;
            field->GetFieldProperties().m_heading.m_labelAlignment = 0;
            field->GetFieldProperties().m_display.m_justify = 0;
            fieldFormat = L"";
        }
        else
        {
            for( int i = 0; i < count; i++ )
            {
                field->GetFieldProperties().m_comment = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 18 ) );
                auto heading = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 8 ) );
                field->GetFieldProperties().m_heading.m_heading = heading == L"" ? fieldName : heading;
                field->GetFieldProperties().m_heading.m_headingAlignment = atoi( PQgetvalue( res, i, 9 ) );
                auto label = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 6 ) );
                field->GetFieldProperties().m_heading.m_label = label == L"" ? fieldName : label;
                field->GetFieldProperties().m_heading.m_labelAlignment = atoi( PQgetvalue( res, i, 7 ) );
                field->GetFieldProperties().m_display.m_justify = atoi( PQgetvalue( res, i, 10 ) );
                fieldFormat = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 11 ) );
            }
        }
    }
    delete[] values[0];
    values[0] = nullptr;
    delete[] values[1];
    values[1] = nullptr;
    delete[] values[2];
    values[2] = nullptr;
    delete[] tname;
    tname = nullptr;
    PQclear( res );
    int type;
    if( field->GetFieldType() == L"date" )
        type = 82;
    if( field->GetFieldType() == L"datetime" || field->GetFieldType() == L"time" || field->GetFieldType() == L"timestamp" )
        type = 84;
    if( field->GetFieldType() == L"real" )
        type = 81;
    else
        type = 80;
    uint32_t binaryIntVal;
    const char *paramValues[1];
    int paramLengths[1];
    int paramFormats[1];
    binaryIntVal = htonl( (uint32_t) type );
    paramValues[0] = (char *) &binaryIntVal;
    paramLengths[0] = sizeof( binaryIntVal );
    paramFormats[0] = 1;
    res = PQexecParams( m_db, "SELECT * FROM abcatfmt WHERE abf_type = $1::int4", 1, nullptr, paramValues, paramLengths, paramFormats, 1 );
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Error executing query: " + err );
        result = 1;
    }
    else
    {
        for( int i = 0; i < PQntuples( res ); i++ )
        {
            auto temp1 = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 1 ) );
            auto temp2 = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 1 ) );
            if( temp1 == fieldFormat )
                field->GetFieldProperties().m_display.m_format[1].push_back( std::make_pair( temp1, temp2 ) );
            else
                field->GetFieldProperties().m_display.m_format[0].push_back( std::make_pair( temp1, temp2 ) );
        }
    }
    PQclear( res );
    return result;
}

int PostgresDatabase::GetFieldProperties(const std::wstring &table, TableField *field, std::vector<std::wstring> &errorMsg)
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
                if( ( *it1 )->GetTableName() == table )
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

int PostgresDatabase::ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &foreignKeyFields, const std::wstring &refTableName, const std::vector<std::wstring> &refKeyFields, int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int match, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"ALTER TABLE ", err;
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
        query += L"REFERENCES " + newFK.at( 0 )->GetReferencedTableName() + L"(";
    for( std::vector<FKField *>::const_iterator it1 = newFK.begin(); it1 < newFK.end(); it1++ )
    {
        query += (*it1)->GetReferencedFieldName();
        if( it1 == newFK.end() - 1 )
            query += L") ";
        else
            query += L",";
        refFields.push_back( (*it1)->GetReferencedFieldName() );
    }
    if( match == 0 )
        query += L"MATCH FULL ";
    else if( match == 1 )
        query += L"MATCH PARTIAL";
	else
        query += L"MATCH SIMPLE ";
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
        query += L"RESTRICT ";
        delProp = RESTRICT_DELETE;
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
    // TODO This will need to be reimplemented
//    if( !isNew )
//        result = DropForeignKey( command, &tableName, keyName, logOnly, errorMsg );
    if( !logOnly && !result && isNew )
    {
        PGresult *res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
        if( PQresultStatus( res ) != PGRES_COMMAND_OK )
        {
            PQclear( res );
            err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Adding forign key failed: " + err );
            result = 1;
        }
        else
        {
            std::map<unsigned long, std::vector<FKField *> > fKeys = tableName.GetForeignKeyVector();
            unsigned long size = fKeys.size();
            size++;
            for( int i = 0; i < foreignKeyFields.size(); i++ )
                fKeys[size].push_back( new FKField( i, keyName, L"", tableName.GetTableName(), foreignKeyFields.at( i ), L"", refTableName, refKeyFields.at( i ), origFields, refFields, updProp, delProp, match ) );
        }
    }
    else
        command = query;
    return result;
}

int PostgresDatabase::DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"DROP TABLE ";
    query += tableName;
    query += L" CASCADE;";
    PGresult *res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Starting transaction failed during connection: " + err );
        result = 1;
    }
    PQclear( res );
    return result;
}

int PostgresDatabase::SetFieldProperties(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, const FieldProperties &prop, bool isLogOnly, std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
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
        command += prop.m_heading.m_label + L", abc_lpos = ";
        command += std::to_wstring( prop.m_heading.m_labelAlignment ) + L", abc_hdr = ";
        command += prop.m_heading.m_heading + L", abc_hpos = ";
        command += std::to_wstring( prop.m_heading.m_headingAlignment ) + L", abc_cmnt = ";
        command += prop.m_comment + L"WHERE abc_tnam = ";
        command += tableName + L" AND abc_ownr = ";
        command += ownerName + L" AND abc_cnam = ";
        command += fieldName + L";";
    }
    if( !isLogOnly )
    {
        PGresult *result = PQexec( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() );
        if( PQresultStatus (result) != PGRES_COMMAND_OK || PQresultStatus (result) != PGRES_TUPLES_OK )
        {
            res = 1;
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( err );
        }
        PQclear( result );
    }
    return res;
}

int PostgresDatabase::GetServerVersion(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    int versionInt = PQserverVersion( m_db );
    if( !versionInt )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Error executing query: " + err );
        result = 1;
    }
    else
    {
        pimpl.m_serverVersion = m_pimpl->m_myconv.from_bytes( PQparameterStatus( m_db, "server_version" ) );
        pimpl.m_versionMajor = versionInt / 10000;
        pimpl.m_versionMinor = ( versionInt - pimpl.m_versionMajor * 10000 ) / 100;
        pimpl.m_versionRevision = versionInt % 100;
    }
    versionInt = PQlibVersion();
    pimpl.m_clientVersionMajor = versionInt % 10000;
    pimpl.m_clientVersionMinor = ( versionInt - pimpl.m_clientVersionMajor ) % 100;
    return result;
}

int PostgresDatabase::DropForeignKey(std::wstring &command, DatabaseTable *table, const std::wstring &keyName, bool logOnly, const std::vector<FKField *> &fkFields, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    PGresult *res = nullptr;
    std::wstring query, err;
    query = L"ALTER TABLE ";
    query += table->GetSchemaName() + L"." + table->GetTableName() +L" ";
    query += L"DROP CONSTRAINT " + keyName;
    if( !logOnly )
    {
        res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
        int status = PQresultStatus( res );
        if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
        {
            err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Dropping foreign key failed: " + err );
            result = 1;
        }
        else
        {
            table->DropForeignKey( keyName );
/*            bool found = false;
            std::map<unsigned long, std::vector<FKField *> > fKeys = table->GetForeignKeyVector();
            for( std::map<unsigned long, std::vector<FKField *> >::iterator it = fKeys.begin(); it != fKeys.end() && !found; ++it )
                for( std::vector<FKField *>::iterator it1 = (*it).second.begin(); it1 != (*it).second.end() && !found; )
                {
                    if( (*it1)->GetFKName() == keyName )
                    {
                        found = true;
                        delete (*it1);
                        (*it1) = NULL;
                        it1 = (*it).second.erase( it1 );
                    }
                    else
                        ++it1;
                }*/
        }
    }
    else
        command = query;
    PQclear( res );
    return result;
}

int PostgresDatabase::NewTableCreation(std::vector<std::wstring> &errorMsg)
{
    int result = 0, count;
    std::wstring err, schemaName, tableName, catalogName;
    if( pimpl.m_versionMajor <= 9 && pimpl.m_versionMinor < 3 )
    {
        std::wstring query = L"SELECT count(*) FROM information_schema.tables";
        PGresult *res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
        int status = PQresultStatus( res );
        if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
        {
            err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Adding foreign key failed: " + err );
            result = 1;
        }
        else
        {
            if( PQntuples( res ) == 1 )
            {
                count = atoi( PQgetvalue( res, 0, 0 ) );
                PQclear( res );
                if( count > m_numOfTables || count < m_numOfTables )
                {
                    std::vector<TableDefinition> temp;
                    query = L"SELECT table_catalog, table_schema, table_name FROM information_schema.tables WHERE table_catalog = \'" + pimpl.m_dbName + L"\';";
                    res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
                    status = PQresultStatus( res );
                    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
                    {
                        err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                        errorMsg.push_back( L"Retrieving new database info failed " + err );
                        result = 1;
                    }
                    else
                    {
                        for( int i = 0; i < PQntuples( res ); i++ )
                        {
                            catalogName = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 0 ) );
                            schemaName = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 1 ) );
                            tableName = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 2 ) );
                            temp.push_back( TableDefinition( catalogName, schemaName, tableName ) );;
                        }
                        {
                            std::lock_guard<std::mutex> locker( GetTableVector().my_mutex );
                            pimpl.m_tableDefinitions[pimpl.m_dbName] = temp;
                        }
                        m_numOfTables = count;
                    }
                }
            }
        }
        PQclear( res );
    }
    return result;
}

int PostgresDatabase::AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, bool tableAdded, std::vector<std::wstring> &errors)
{
    PGresult *res1 = nullptr, *res2 = nullptr, *res4 = nullptr;
    int result = 0, fieldIsNull, fieldPK, fkReference, fkId, fkMatch;
    char *values1[2];
    std::vector<std::wstring> fk_names, indexes;
    std::wstring fieldName, fieldType, fieldDefaultValue, origSchema, origTable, origField, refSchema, refTable, refField, fkName, fkTableField, fkUpdateConstraint, fkDeleteConstraint;
    ExecStatusType status;
    std::vector<TableField *> fields;
    std::map<unsigned long,std::vector<FKField *> > foreign_keys;
    std::map<int, std::vector<std::wstring> > origFields, refFields;
    FK_ONUPDATE update_constraint = NO_ACTION_UPDATE;
    FK_ONDELETE delete_constraint = NO_ACTION_DELETE;
    const char *catalog_name = m_pimpl->m_myconv.to_bytes( catalog.c_str() ).c_str();
    std::wstring own = L"";
    if( GetTableOwner( schemaName, tableName, own, errors ) )
        result = 1;
    else
    {
        values1[0] = new char[schemaName.length() * sizeof( wchar_t ) + 1];
        values1[1] = new char[tableName.length() * sizeof( wchar_t ) + 1];
        memset( values1[0], '\0', schemaName.length()  * sizeof( wchar_t ) + 1 );
        memset( values1[1], '\0', tableName.length() * sizeof( wchar_t ) + 1 );
        strcpy( values1[0], m_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str() );
        strcpy( values1[1], m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str() );
        int len1 = schemaName.length();
        int len2 = tableName.length();
        int length1[2] = { len1, len2 };
        int formats1[2] = { 0, 0 };
        res1 = PQexecPrepared( m_db, "get_fkeys", 2, values1, length1, formats1, 0 );
        status = PQresultStatus( res1 );
        if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errors.push_back( L"Error executing query: " + err );
            PQclear( res1 );
            fields.erase( fields.begin(), fields.end() );
            foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
            result = 1;
        }
        if( !result  )
        {
            for( int j = 0; j < PQntuples( res1 ); j++ )
            {
                fkId = atoi( PQgetvalue( res1, j, 0 ) );
                origField = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 5 ) );
                origFields[fkId].push_back( origField );
                refField = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 8 ) );
                refFields[fkId].push_back( refField );
            }
            for( int j = 0; j < PQntuples( res1 ); j++ )
            {
                char *key_id = PQgetvalue( res1, j, 0 );
                char *fk_reference = PQgetvalue( res1, j, 1 );
                fkName = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 2 ) );
                origSchema = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 3 ) );
                origTable = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 4 ) );
                origField = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 5 ) );
                refSchema = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 6 ) );
                refTable = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 7 ) );
                refField = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 8 ) );
                fkUpdateConstraint = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 9 ) );
                fkDeleteConstraint = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 10 ) );
                std::wstring temp = m_pimpl->m_myconv.from_bytes( PQgetvalue( res1, j, 11 ) );
                fkId = atoi( key_id );
                fkReference = atoi( fk_reference );
                if( temp == L"MATCH FULL" )
                    fkMatch = 0;
                else if( temp == L"MATCH PARTIAL" )
                    fkMatch = 1;
                else
                    fkMatch = 2;
                if( fkUpdateConstraint == L"NO ACTION" )
                    update_constraint = NO_ACTION_UPDATE;
                if( fkUpdateConstraint == L"RESTRICT" )
                    update_constraint = RESTRICT_UPDATE;
                if( fkUpdateConstraint == L"SET NULL" )
                    update_constraint = SET_NULL_UPDATE;
                if( fkUpdateConstraint == L"SET DEFAULT" )
                    update_constraint = SET_DEFAULT_UPDATE;
                if( fkUpdateConstraint == L"CASCADE" )
                    update_constraint = CASCADE_UPDATE;
                if( fkDeleteConstraint == L"NO ACTION" )
                    delete_constraint = NO_ACTION_DELETE;
                if( fkDeleteConstraint == L"RESTRICT" )
                    delete_constraint = RESTRICT_DELETE;
                if( fkDeleteConstraint == L"SET NULL" )
                    delete_constraint = SET_NULL_DELETE;
                if( fkDeleteConstraint == L"SET DEFAULT" )
                    delete_constraint = SET_DEFAULT_DELETE;
                if( fkDeleteConstraint == L"CASCADE" )
                    delete_constraint = CASCADE_DELETE;
            //id,       name,   orig_schema,  table_name,  orig_field,  ref_schema, ref_table, ref_field, update_constraint, delete_constraint
                foreign_keys[fkId].push_back( new FKField( fkReference, fkName, origSchema, origTable, origField, refSchema, refTable, refField, origFields[fkId], refFields[fkId], update_constraint, delete_constraint, fkMatch ) );
                fk_names.push_back( origField );
            }
            PQclear( res1 );
            res2 = PQexecPrepared( m_db, "get_columns", 2, values1, length1, formats1, 0 );
            status = PQresultStatus( res2 );
            if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                errors.push_back( L"Error executing query: " + err );
                PQclear( res2 );
                fields.erase( fields.begin(), fields.end() );
                foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                result = 1;
            }
        }
        if( !result )
        {
            int numFields = 0;
            for( int j = 0; j < PQntuples( res2 ); j++ )
            {
                int size, precision;
                bool autoinc = false;
                const char *field_name = PQgetvalue( res2, j, 0 );
                fieldName = m_pimpl->m_myconv.from_bytes( field_name );
                fieldType = m_pimpl->m_myconv.from_bytes( PQgetvalue( res2, j, 1 ) );
                char *char_length = PQgetvalue( res2, j, 2 );
                char *numeric_length = PQgetvalue( res2, j, 4 );
                char *numeric_scale = PQgetvalue( res2, j, 6 );
                fieldDefaultValue = m_pimpl->m_myconv.from_bytes( PQgetvalue( res2, j, 8 ) );
                fieldIsNull = !strcmp( PQgetvalue( res2, j, 7 ), "YES" ) ? 1 : 0;
                fieldPK = !strcmp( PQgetvalue( res2, j, 9 ), "YES" ) ? 1 : 0;
                if( *char_length == '0' )
                {
                    size = atoi( numeric_length );
                    precision = atoi( numeric_scale );
                }
                else
                {
                    size = atoi( char_length );
                    precision = 0;
                }
                if( fieldType == L"serial" || fieldType == L"bigserial" )
                    autoinc = true;
                TableField *field = new TableField( fieldName, fieldType, size, precision, catalog + L"." + schemaName + L"." + tableName + L"." + fieldName, fieldDefaultValue, fieldIsNull, autoinc, fieldPK, std::find( fk_names.begin(), fk_names.end(), fieldName ) != fk_names.end() );
                if( GetFieldProperties( tableName, schemaName, own, m_pimpl->m_myconv.from_bytes( field_name ), field, errors ) )
                {
                    std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                    errors.push_back( err );
                    fields.erase( fields.begin(), fields.end() );
                    foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                    result = 1;
                    break;
                }
                fields.push_back( field );
                numFields++;
            }
            PQclear( res2 );
            if( !result )
            {
                std::vector<std::wstring> pk;
                for( std::vector<TableField *>::iterator it = fields.begin(); it < fields.end(); ++it )
                {
                    if( (*it)->IsPrimaryKey() )
                        pk.push_back( (*it)->GetFieldName() );
                }
                DatabaseTable *table = new DatabaseTable( tableName, schemaName, fields, pk, foreign_keys );
                table->SetFullName( catalog + L"." + schemaName + L"." + tableName );
                table->SetCatalog( catalog );
                table->SetNumberOfFields( numFields );
                table->SetTableOwner( own );
                if( GetTableProperties( table, errors ) )
                {
                    char *err = PQerrorMessage( m_db );
                    errors.push_back( m_pimpl->m_myconv.from_bytes( err ) );
                    fields.erase( fields.begin(), fields.end() );
                    foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                    result = 1;
                }
                else
                {
                    int numIndexes = 0;
                    res4 = PQexecPrepared( m_db, "get_indexes", 2, values1, length1, formats1, 1 );
                    status = PQresultStatus( res4 );
                    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
                    {
                        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                        errors.push_back( L"Error executing query: " + err );
                        PQclear( res4 );
                        fields.erase( fields.begin(), fields.end() );
                        foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                        result = 1;
                    }
                    else
                    {
                        for( int j = 0; j < PQntuples( res4 ); j++ )
                        {
                            indexes.push_back( m_pimpl->m_myconv.from_bytes( PQgetvalue( res4, j, 0 ) ) );
                            numIndexes++;
                        }
                        table->SetIndexNames( indexes );
                        table->SetNumberOfIndexes( numIndexes );
                        pimpl.m_tables[m_pimpl->m_myconv.from_bytes( catalog_name )].push_back( table );
                        fields.erase( fields.begin(), fields.end() );
                        foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                        fk_names.clear();
                        PQclear( res4 );
                    }
                }
            }
        }
    }
    delete[] values1[0];
    values1[0] = NULL;
    delete[] values1[1];
    values1[1] = NULL;
    return result;
}

int PostgresDatabase::GetTableOwner(const std::wstring &schemaName, const std::wstring &tableName, std::wstring &owner, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"SELECT u.usename FROM pg_class c, pg_user u, pg_namespace n WHERE n.oid = c.relnamespace AND u.usesysid = c.relowner AND n.nspname = $1 AND relname = $2";
    char *values[2];
    values[0] = NULL, values[1] = NULL;
    values[0] = new char[schemaName.length() * sizeof( wchar_t ) + 1];
    values[1] = new char[tableName.length() * sizeof( wchar_t ) + 1];
    memset( values[0], '\0', schemaName.length()  * sizeof( wchar_t ) + 1 );
    memset( values[1], '\0', tableName.length() * sizeof( wchar_t ) + 1 );
    strcpy( values[0], m_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str() );
    strcpy( values[1], m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str() );
    PGresult *res = PQexecParams( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), 2, NULL, values, nullptr, nullptr, 0 );
    ExecStatusType status = PQresultStatus( res ); 
    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        result = 1;
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Error getting table owner: " + err );
    }
    else
    {
        owner = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, 0, 0 ) );
    }
    delete[] values[0];
    values[0] = NULL;
    delete[] values[1];
    values[1] = NULL;
    PQclear( res );
    return result;
}

bool PostgresDatabase::IsFieldPropertiesExist(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, std::vector<std::wstring> &errorMsg)
{
    bool exist = false;
    std::wstring query = L"SELECT 1 FROM abcatcol WHERE abc_tnam = $1 AND abc_ownr = $2 AND abc_cnam = $3;";
    char *values[3];
    values[0] = new char[tableName.length() * sizeof( wchar_t ) + 1];
    values[1] = new char[ownerName.length() * sizeof( wchar_t ) + 1];
    values[2] = new char[fieldName.length() * sizeof( wchar_t ) + 1];
    strcpy( values[0], m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str() );
    strcpy( values[1], m_pimpl->m_myconv.to_bytes( ownerName.c_str() ).c_str() );
    strcpy( values[2], m_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str() );
    PGresult *res = PQexecParams( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), 3, NULL, values, nullptr, nullptr, 0 );
    ExecStatusType status = PQresultStatus( res );
    if( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK )
        exist = true;
    else
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );

    }
    PQclear( res );
    delete[] values[0];
    values[0] = NULL;
    delete[] values[1];
    values[1] = NULL;
    delete[] values[2];
    values[2] = NULL;
    return exist;
}

int PostgresDatabase::GetFieldHeader(const std::wstring &tableName, const std::wstring &fieldName, std::wstring &headerStr, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    headerStr = fieldName;
    char *values[2];
    values[0] = new char[tableName.length() * sizeof( wchar_t ) + 1];
    values[1] = new char[fieldName.length() * sizeof( wchar_t ) + 1];
    strcpy( values[0], m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str() );
    strcpy( values[1], m_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str() );
    std::wstring query = L"SEECT pbc_hdr FROM \"abcatcol\" WHERE \"abc_tnam\" = $1 AND \"abc_cnam\" = $2";
    PGresult *res = PQexecParams( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), 2, NULL, values, nullptr, nullptr, 0 );
    ExecStatusType status = PQresultStatus( res ); 
    if( status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK )
        headerStr = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, 0, 0 ) );
    delete[] values[0];
    values[0] = nullptr;
    delete[] values[1];
    values[1] = nullptr;
    return result;
}

int PostgresDatabase::PrepareStatement(const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"DECLARE editdata CURSOR FOR SELECT * FROM " + schemaName + L"." + tableName + L";";
    PGresult *res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    return result;
}

int PostgresDatabase::EditTableData(std::vector<DataEditFiield> &row, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    PGresult *res = PQexec( m_db, "FETCH 1 in editdata" );
    if( PQresultStatus( res ) != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    else
    {
        if( !m_fieldsInRecordSet )
            m_fieldsInRecordSet = PQnfields( res );
    }
    return result;
}

int PostgresDatabase::FinalizeStatement(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    PGresult *res = PQexec( m_db, "CLOSE editdata" );
    if( PQresultStatus( res ) != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    m_fieldsInRecordSet = 0;
    return result;
}

int PostgresDatabase::GetTableCreationSyntax(const std::wstring &catalog, const std::wstring &schema, const std::wstring &table, std::vector<std::wstring> &syntax, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

int PostgresDatabase::AttachDatabase(const std::wstring &catalog, const std::wstring &schema, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    char *param[1];
    param[0] = new char[catalog.length() * sizeof( wchar_t ) + 1];
    memset( param[0], '\0', catalog.length() * sizeof( wchar_t ) + 1 );
    strcpy( param[0], m_pimpl->m_myconv.to_bytes( catalog.c_str() ).c_str() );
    std::wstring query1 = L"SELECT t.table_catalog AS catalog, t.table_schema AS schema, t.table_name AS table FROM information_schema.tables t WHERE t.table_catalog = $1 AND (t.table_type = 'BASE TABLE' OR t.table_type = 'VIEW' OR t.table_type = 'LOCAL TEMPORARY') ORDER BY table_name;";
    auto res = PQexecParams( m_db, m_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str(), 1, NULL, param, NULL, NULL, 0 );
    ExecStatusType status = PQresultStatus( res ); 
    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        PQclear( res );
        result = 1;
    }
    else
    {
        for( int i = 0; i < PQntuples( res ); i++ )
        {
            char *catalog_name = PQgetvalue( res, i, 0 );
            char *schema_name = PQgetvalue( res, i, 1 );
            char *table_name = PQgetvalue( res, i, 2 );
            pimpl.m_tableDefinitions[m_pimpl->m_myconv.from_bytes( catalog_name )].push_back( TableDefinition( m_pimpl->m_myconv.from_bytes( catalog_name ), m_pimpl->m_myconv.from_bytes( schema_name ), m_pimpl->m_myconv.from_bytes( table_name ) ) );
        }
    }
    PQclear( res );
    return result;
}

int PostgresDatabase::GetDatabaseNameList(std::vector<std::wstring> &names, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"SELECT datname FROM pg_database;";
    auto res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
    ExecStatusType status = PQresultStatus( res );
    if( status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK )
    {
        auto err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Retrieve database list: " + err );
        result = 1;
    }
    else if( status == PGRES_TUPLES_OK )
    {
        for( int i = 0; i < PQntuples( res ); i++ )
        {
            names.push_back( m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 0 ) ) );
        }
    }
    PQclear( res );
    return result;
}
int PostgresDatabase::GetQueryRow(const std::wstring &query, std::vector<std::wstring> &values)
{
    auto result = 0;
    return result;
}

int PostgresDatabase::AddUpdateFormat()
{
    return 0;
}

int PostgresDatabase::PopulateValdators(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"SELECT * FROM abcatvld;";
    auto res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
    ExecStatusType status = PQresultStatus( res );
    if( status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK )
    {
        auto err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Retrieve database list: " + err );
        result = 1;
    }
    else if( status == PGRES_TUPLES_OK )
    {
        for( int i = 0; i < PQntuples( res ); i++ )
        {
            auto name = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 0 ) );
            auto rule = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 1 ) );
            unsigned short type = (unsigned short) strtoul( PQgetvalue( res, i, 2 ), nullptr, 10 );
            int control = (int) strtoul( PQgetvalue( res, i, 3 ), nullptr, 10 );
            auto message = m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 4 ) );
            pimpl.m_validators.push_back( std::make_tuple( name, rule, type, control, message ) );
        }
    }
    PQclear( res );
    return result;
}

int PostgresDatabase::CreateUpdateValidationRule(bool isNew, const std::wstring &name, const std::wstring &rule, const int type, const std::wstring &message, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring errorMessage;
    std::wstring query = L"";
    const char *paramValues[4];
    if( isNew )
        query = L"INSERT INTO \"abcatvld\"(\"abv_name\", \"abv_vald\", \"abv_type\", \"abv_cntr\", \"abv_msg\") VALUES( $1, $2, $3, 0, $4)";
    else
        query = L"UPDATE \"abcatvld\" SET \"abv_name\" = $1, \"abv_vald\" = $2, \"abv_type\" = $3, \"abv_cntr\" = 0, \"abv_msg\" = $4 WHERE \"abv_name\" = $1";
    paramValues[0] = m_pimpl->m_myconv.to_bytes( name.c_str() ).c_str();
    paramValues[1] = m_pimpl->m_myconv.to_bytes( rule.c_str() ).c_str();
    paramValues[2] = std::to_string( type ).c_str();
    paramValues[3] = m_pimpl->m_myconv.to_bytes( message.c_str() ).c_str();
    auto res = PQexecParams( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), 4, nullptr,
        paramValues, nullptr, nullptr, 1 );      /* ask for binary results */
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        auto err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Update validation table: " + err );
        result = 1;
    }
    PQclear( res );
    return result;
}

int PostgresDatabase::GetTablespacesList(std::vector<std::wstring> &list, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring errorMessage;
    std::wstring query = L"SELECT * FROM pg_tablespace;";
    auto res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );       // ask for binary results 
    if( PQresultStatus( res ) != PGRES_TUPLES_OK )
    {
        auto err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Query tablespace table: " + err );
        result = 1;
    }
    else
    {
        for( auto i = 0; i < PQntuples( res ); i++ )
            m_pimpl->m_tablespaces.push_back( m_pimpl->m_myconv.from_bytes( PQgetvalue( res, i, 1 ) ) );
    }
    PQclear( res );
    return result;
}

int PostgresDatabase::GetTableFields(const std::wstring &catalog, const std::wstring &schema, const std::wstring &table, std::vector<std::wstring> &fields, std::vector<std::wstring> &errors)
{
    int result = 0;
    char *values1[2];
    const char *catalog_name = m_pimpl->m_myconv.to_bytes( catalog.c_str() ).c_str();
    const char *schema_name = m_pimpl->m_myconv.to_bytes( schema.c_str() ).c_str();
    const char *table_name = m_pimpl->m_myconv.to_bytes( table.c_str() ).c_str();
    values1[0] = new char[strlen( schema_name ) + 1];
    values1[1] = new char[strlen( table_name ) + 1];
    strcpy( values1[0], schema_name );
    strcpy( values1[1], table_name );
    int len1 = (int) strlen( schema_name );
    int len2 = (int) strlen( table_name );
    int length1[2] = { len1, len2 };
    int formats1[2] = { 1, 1 };
    auto res2 = PQexecPrepared( m_db, "get_columns", 2, values1, length1, formats1, 0 );
    auto status = PQresultStatus( res2 );
    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errors.push_back( L"Error executing query: " + err );
        PQclear( res2 );
        result = 1;
    }
    else if( status == PGRES_TUPLES_OK )
    {
        for( int j = 0; j < PQntuples( res2 ); j++ )
        {
            if( !strcmp( PQgetvalue( res2, j, 9 ), "YES" ) )
            {
                auto fieldName = m_pimpl->m_myconv.from_bytes( PQgetvalue( res2, j, 0 ) );
                fields.push_back( fieldName );
            }
        }
    }
    PQclear( res2 );
    return result;
}

int PostgresDatabase::EditPrimaryKey(const std::wstring &catalogNamme, const std::wstring &schemaName, const std::wstring &tableName, const std::vector<std::wstring> &newKey, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}


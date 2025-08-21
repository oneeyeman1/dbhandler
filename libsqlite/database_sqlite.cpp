#ifdef WIN32
#include <windows.h>
#pragma execution_character_set("utf-8")
#endif

#if defined __WXMSW__ && defined __MEMORYLEAKS__
#include <vld.h>
#endif

#include <stdio.h>
#include <map>
#include <vector>
#include <string.h>
#include <string>
#include <locale>
#include <regex>
#include <codecvt>
#include <sstream>
#include <algorithm>
#include <cwctype>
#include "sqlite3.h"
#include "database.h"
#include "database_sqlite.h"

std::mutex Impl::my_mutex;

SQLiteDatabase::SQLiteDatabase(const int osId, const std::wstring &desktop) : Database(osId, desktop)
{
    sqlite_pimpl = NULL;
    m_db = NULL;
    connectToDatabase = false;
    m_stmt1 = m_stmt2 = m_stmt3 = NULL;
    m_fieldsInRecordSet = 0;
}

SQLiteDatabase::~SQLiteDatabase()
{
    for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it = pimpl.m_tables.begin(); it != pimpl.m_tables.end(); it++ )
    {
        std::vector<DatabaseTable *> tableVec = (*it).second;
        for( std::vector<DatabaseTable *>::iterator it1 = tableVec.begin(); it1 < tableVec.end(); it1++ )
        {
            delete (*it1);
            (*it1) = NULL;
        }
        (*it).second.clear();
    }
    delete sqlite_pimpl;
    sqlite_pimpl = NULL;
}

int SQLiteDatabase::CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    sqlite3 *db = nullptr;
    int result = 0;
    std::vector<std::wstring> dbList;
    result = Disconnect( errorMsg );
    if( result == SQLITE_OK )
        result = sqlite3_open( sqlite_pimpl->m_myconv.to_bytes( name.c_str() ).c_str(), &db );
    return result;
}

int SQLiteDatabase::DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    if( pimpl.m_dbName == name )
        result = Disconnect( errorMsg );
    if( !result )
        result = remove( sqlite_pimpl->m_myconv.to_bytes( name.c_str() ).c_str() );
    return result;
}

int SQLiteDatabase::ServerConnect(std::vector<std::wstring> &UNUSED(dbList), std::vector<std::wstring> &UNUSED(errorMsg))
{
    return 0;
}

int SQLiteDatabase::Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &UNUSED(dbList), std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    char *err;
    sqlite3_stmt *stmt;
    std::vector<std::string> queries;
    queries.push_back( "CREATE TABLE IF NOT EXISTS \"sys.abcatcol\"(\"abc_tnam\" char(129) NOT NULL, \"abc_tid\" integer, \"abc_ownr\" char(129) NOT NULL, \"abc_cnam\" char(129) NOT NULL, \"abc_cid\" smallint, \"abc_labl\" char(254), \"abc_lpos\" smallint, \"abc_hdr\" char(254), \"abc_hpos\" smallint, \"abc_itfy\" smallint, \"abc_mask\" char(31), \"abc_case\" smallint, \"abc_hght\" smallint, \"abc_wdth\" smallint, \"abc_ptrn\" char(31), \"abc_bmap\" char(1), \"abc_init\" char(254), \"abc_cmnt\" char(254), \"abc_edit\" char(31), \"abc_tag\" char(254), PRIMARY KEY( \"abc_tnam\", \"abc_ownr\", \"abc_cnam\" ));" );
    queries.push_back( "CREATE UNIQUE INDEX IF NOT EXISTS \"pbcatc_x\" ON \"sys.abcatcol\"(\"abc_tnam\" ASC, \"abc_ownr\" ASC, \"abc_cnam\" ASC);" );
    queries.push_back( "CREATE TABLE IF NOT EXISTS \"sys.abcatedt\"(\"abe_name\" char(30) NOT NULL, \"abe_edit\" char(254), \"abe_type\" smallint, \"abe_cntr\" integer, \"abe_seqn\" smallint NOT NULL, \"abe_flag\" integer, \"abe_work\" char(32), PRIMARY KEY( \"abe_name\", \"abe_seqn\" ));" );
    queries.push_back( "CREATE UNIQUE INDEX IF NOT EXISTS pbcate_x ON \"sys.abcatedt\"(\"abe_name\" ASC, \"abe_seqn\" ASC);" );
    queries.push_back( "CREATE TABLE IF NOT EXISTS \"sys.abcatfmt\"(\"abf_name\" char(30) NOT NULL, \"abf_frmt\" char(254), \"abf_type\" smallint, \"abf_cntr\" integer, PRIMARY KEY( \"abf_name\" ));" );
    queries.push_back( "CREATE UNIQUE INDEX IF NOT EXISTS pbcatf_x ON \"sys.abcatfmt\"(\"abf_name\" ASC);" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"(General)\", \"(General)\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"0\", \"0\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"0.00\", \"0.00\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"#.##0\", \"#.##0\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"#.##0,00\", \"#.##0,00\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"$#.##0;[$#.##0]\", \"$#.##0;[$#.##0]\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"$#.##0;|RED|[$#.##0]\", \"$#.##0;|RED|[$#.##0]\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"$#.##0,00;[$#.##0,00]\", \"$#.##0,00;[$#.##0,00]\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"$#.##0,00;|RED|[$#.##0,00]\", \"$#.##0,00;|RED|[$#.##0,00]\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"0%\", \"0%\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"0.00%\", \"0.00%\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"0.00E+00\", \"0.00E+00\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"m/d/yy\", \"m/d/yy\", 84, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"d-mmm-yy\", \"d-mmm-yy\", 84, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"d-mmm\", \"d-mmm\", 84, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"mmm-yy\", \"mmm-yy\", 84, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"h:mm AM/PM\", \"h:mm AM/PM\", 84, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"h:mm:ss AM/PM\", \"h:mm:ss AM/PM\", 84, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"h:mm:ss\", \"h:mm:ss\", 84, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"Phone_format\", \"(@@@)) @@@-@@@@\", 80, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"m-d-yy\", \"m-d-yy\", 84, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"soc_sec_number\", \"@@@-@@-@@@@\", 80, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"mm/dd/yyyy\", \"mm/dd/yyyy\", 82, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"salary\", \"$###,##0.00\", 81, 0 );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatfmt\" VALUES( \"mm-dd-yyyy\", \"mm-dd-yyyy\", 82, 0 );" );
    queries.push_back( "CREATE TABLE IF NOT EXISTS \"sys.abcattbl\"(\"abt_os\" integer, \"abt_tnam\" char(129) NOT NULL, \"abt_tid\" integer, \"abt_ownr\" char(129) NOT NULL, \"abd_fhgt\" smallint, \"abd_fwgt\" smallint, \"abd_fitl\" char(1), \"abd_funl\" char(1), \"abd_fstr\" integer, \"abd_fchr\" smallint, \"abd_fptc\" smallint, \"abd_ffce\" char(18), \"abh_fhgt\" smallint, \"abh_fwgt\" smallint, \"abh_fitl\" char(1), \"abh_funl\" integer, \"abh_fstr\" integer, \"abh_fchr\" smallint, \"abh_fptc\" smallint, \"abh_ffce\" char(18), \"abl_fhgt\" smallint, \"abl_fwgt\" smallint, \"abl_fitl\" char(1), \"abl_funl\" integer, \"abl_fstr\" integer, \"abl_fchr\" smallint, \"abl_fptc\" smallint, \"abl_ffce\" char(18), \"abt_cmnt\" char(254), PRIMARY KEY( \"abt_tnam\", \"abt_ownr\" ));" );
    queries.push_back( "CREATE UNIQUE INDEX IF NOT EXISTS pbcatt_x ON \"sys.abcattbl\"(\"abt_os\" ASC, \"abt_tnam\" ASC, \"abt_ownr\" ASC);" );
    queries.push_back( "CREATE TABLE IF NOT EXISTS \"sys.abcatvld\"(\"abv_name\" char(30) NOT NULL, \"abv_vald\" char(254), \"abv_type\" smallint, \"abv_cntr\" integer, \"abv_msg\" char(254), PRIMARY KEY( \"abv_name\" ));" );
    queries.push_back( "CREATE UNIQUE INDEX IF NOT EXISTS abcatv_f ON \"sys.abcatvld\"(\"abv_name\" ASC);" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatvld\" VALUES( \'Multple_of_100\', \'CHECK( mod( @column, 100 ) = 0 )\', 81, 3, \'The department number must be \');" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatvld\" VALUES( \'Positive_number\', \'CHECK( @column > 0 )\', 81, 6, \'Sorry! The value must be greater than 0\');");
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatvld\" VALUES( \'Y_or_N\', \'CHECK( @column IN ( \"Y\", \"y\", \"N\", \"n\" )\', 81, 6, '');");
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatvld\" VALUES( \'must_be_numer\', \'CHECK( isNumer( @column )\', 80, 0, \'\');");
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatvld\" VALUES( \'valid status\', \'CHECK( @status == \"ALT*\" )\', 80, 3, \'\');");
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'###-##-####\', \'###-##-####\', 90, 1, 1, 32, \'00\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'###,###.00\', \'###,###.00\', 90, 1, 1, 32, \'10\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'#####\', \'#####\', 90, 1, 1, 32, \'10\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'BenefitsCheckBox\', NULL, 85, 4, 1, 536870916, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'BenefitsCheckBox\', \'Y\', 85, 4, 2, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'BenefitsCheckBox\', \'N\', 85, 4, 3, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'0\', 87, 3, 1, -201326590, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 2, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'White\', 87, 3, 3, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 4, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Black\', 87, 3, 5, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 6, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Purple\', 87, 3, 7, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 8, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Orange\', 87, 3, 9, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 10, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Green\', 87, 3, 11, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 12, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Blue\', 87, 3, 13, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Yellow\', 87, 3, 14, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Yrllow\', 87, 3, 15, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 16, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Color List\', \'Red\', 87, 3, 17, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Customers\', \'d_dddw_cust\', 88, 2, 1, 536870928, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 2, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Customers\', \'id\', 88, 2, 3, 0, \'400\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Day Care\', \'Day Care\', 85, 4, 1, 536870916, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Day Care\', \'Y\', 85, 4, 2, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Day Care\', \'N\', 85, 4, 3, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'DD/MM/YY\', \'DD/MM/YY\', 90, 1, 1, 32, \'20\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS\', \'DD/MM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'DD/MM/YY HH:MM:SS:FFFFFF\', \'DD/MM/YY HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'40\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'DD/MM/YYYY\', \'DD/MM/YYYY\', 90, 1, 1, 32, \'20\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'DD/MM/YYYY HH:MM:SS\', \'DD/MM/YY HH::MM:SS\', 90, 1, 1, 32, \'40\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'DD/MMM/YY\', \'DD/MMM/YY\', 90, 1, 1, 32, \'20\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'DD/MMM/YY HH:MM:SS\', \'DD/MMM/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'DDD/YY\', \'DDD/YY\', 90, 1, 1, 32, \'20\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'DDD/YY HH:MM:SS\', \'DDD/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'DDD/YYYY\', \'DDD/YYYY\', 90, 1, 1, 32, \'20\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'DDD/YYYY HH:MM:SS\', \'DDD/YYYY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Department Lst\', \'d_dddw_dept\', 88, 10, 1, 536870928, \'0\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Department List\', \'dept_id\', 88, 10, 2, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Department List\', \'dept_d\', 88, 10, 3, 0, \'300\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Dollars with cents\', \'$###,###,###.00\', 90, 2, 1, 32, \'00\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Employee Status\', \'1\', 86, 3, 1, 1073741832, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Empplyee Status\', \'Active\', 86, 3, 2, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Empplyee Status\', \'A\', 86, 3, 3, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Empplyee Status\', \'Terminated\', 86, 3, 4, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Empplyee Status\', \'T\', 86, 3, 5, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Empplyee Status\', \'On Leave\', 86, 3, 6, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Empplyee Status\', \'L\', 86, 3, 7, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'empplyees\', \'d_dddw_sales_reps\', 88, 3, 1, 536870928, \'0\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'empplyees\', \'emp_id\', 88, 3, 2, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'empplyees\', \'emp_id\', 88, 3, 3, 0, \'400\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Financial Codes\', \'d_dddw_fin_code\', 88, 3, 1, 536870928, \'0\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 2, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Financial Codes\', \'code\', 88, 3, 3, 0, \'700\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Health Insurance\', \'health insurance\', 85, 3, 1, 536870928, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Health Insurance\', \'Y\', 85, 3, 2, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'Health Insurance\', \'N\', 85, 3, 3, 0, NULL );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS:FFF\', \'HH:MM:SS:FFF\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS:FFFFFF\', \'HH:MM:SS:FFFFFF\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'JJJ/YY\', \'JJJ/YY\', 90, 1, 1, 32, \'20\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'JJJ/YY HH:MM:SS\', \'JJJ/YY HH:MM:SS\', 90, 1, 1, 32, \'40\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'JJJ/YYYY\', \'JJJ/YYYY\', 90, 1, 1, 32, \'40\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    queries.push_back( "INSERT OR IGNORE INTO \"sys.abcatedt\" VALUES( \'HH:MM:SS\', \'HH:MM:SS\', 90, 1, 1, 32, \'30\' );" );
    pimpl.m_type = L"SQLite";
    pimpl.m_subtype = L"";
    pimpl.m_connectedUser = L"";
    if( !sqlite_pimpl )
    {
        sqlite_pimpl = new SQLiteImpl;
        sqlite_pimpl->m_catalog = L"";
    }
    int res = sqlite3_open( sqlite_pimpl->m_myconv.to_bytes( selectedDSN.c_str() ).c_str(), &m_db );
    if( res != SQLITE_OK )
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
    }
    else
    {
        res = sqlite3_exec( m_db, "BEGIN EXCLUSIVE TRANSACTION", NULL, NULL, &err );
        if( res != SQLITE_OK )
        {
            GetErrorMessage( res, errorMsg );
            result = 1;
            sqlite3_free( err );
        }
        else
        {
            auto error = false;
            for( auto it = queries.begin(); it < queries.end() && !error; ++it )
            {
                res = sqlite3_exec( m_db, (*it).c_str(), NULL, NULL, &err );
                if( res != SQLITE_OK )
                    error = true;
            }
            if( !error )
            {
                res = sqlite3_exec( m_db, "COMMIT", NULL, NULL, &err );
                if( res != SQLITE_OK )
                {
                    GetErrorMessage( res, errorMsg );
                    res = sqlite3_exec( m_db, "ROLLBACK", NULL, NULL, NULL );
                    result = 1;
                    sqlite3_free( err );
                }
            }
            else
            {
                GetErrorMessage( res, errorMsg );
                res = sqlite3_exec( m_db, "ROLLBACK", NULL, NULL, NULL );
                result = 1;
                sqlite3_free( err );
            }
        }
        if( res != SQLITE_OK )
        {
            GetErrorMessage( res, errorMsg );
            res = sqlite3_exec( m_db, "ROLLBACK", NULL, NULL, NULL );
            result = 1;
            sqlite3_free( err );
        }
        else
        {
            sqlite_pimpl->m_catalog = selectedDSN;
            res = GetTableListFromDb( errorMsg );
            if( !res )
            {
                res = sqlite3_exec( m_db, "PRAGMA foreign_keys = ON", NULL, NULL, NULL );
                if( res != SQLITE_OK )
                {
                    result = 1;
                    GetErrorMessage( res, errorMsg );
                }
                else
                {
                    if( sqlite3_prepare_v2( m_db, "PRAGMA schema_version", -1, &stmt, NULL ) == SQLITE_OK )
                    {
                        if( ( res = sqlite3_step( stmt ) ) == SQLITE_ROW )
                        {
                            m_schema = sqlite3_column_int( stmt, 0 );
                            pimpl.m_dbName = sqlite_pimpl->m_catalog;
                        }
                        else
                        {
                            result = 1;
                            GetErrorMessage( res, errorMsg );
                        }
                    }
                    else
                    {
                        result = 1;
                        GetErrorMessage( res, errorMsg );
                    }
                    sqlite3_finalize( stmt );
                }
            }
            else
                result = 1;
        }
        res = PopulateValdators( errorMsg );
        if( res )
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
        else
            GetServerVersion( errorMsg );
    }
    if( result && m_db )
        Disconnect( errorMsg );
    else
        pimpl.m_connectString = selectedDSN;
    sqlite3_extended_result_codes( m_db, 1 );
    return result;
}

int SQLiteDatabase::Disconnect(std::vector<std::wstring> &errorMsg)
{
    std::lock_guard<std::mutex> locker( GetTableVector().my_mutex );
    int result = 0, res;
    const char *query = NULL;
    if( !m_isConnected )
        return 0;
    if( m_stmt1 )
        res = sqlite3_finalize( m_stmt1 );
    if( m_stmt2 )
        res = sqlite3_finalize( m_stmt2 );
    if( m_stmt3 )
        res = sqlite3_finalize( m_stmt3 );
    res = sqlite3_close( m_db );
    if( res != SQLITE_OK )
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
//  For debugging purposes - helps find non-closed statements
        sqlite3_stmt *statement = sqlite3_next_stmt( m_db, NULL );
        if( statement )
            query = sqlite3_sql( statement );
//  For debugging purposes - helps find non-closed statements
    }
    delete sqlite_pimpl;
    sqlite_pimpl = NULL;
    return result;
}

void SQLiteDatabase::GetErrorMessage(int code, std::vector<std::wstring> &errorMsg)
{
    auto errorCode = sqlite3_errcode( m_db );
    auto extendedErrorCode = sqlite3_extended_errcode( m_db );
    auto stmt = sqlite3_next_stmt( m_db, nullptr );
    if( stmt )
        errorMsg.push_back( sqlite_pimpl->m_myconv.from_bytes( sqlite3_sql( stmt ) ) );
    switch( code )
    {
    case 1:
        errorMsg.push_back( L"SQL error" );
        break;
    case 2:
        errorMsg.push_back( L"Internal logic error" );
        break;
    case 3:
        errorMsg.push_back( L"Access permission denied" );
        break;
    case 4:
        errorMsg.push_back( L"Callback routine requested an abort" );
        break;
    case 5:
        errorMsg.push_back( L"The database is locked" );
        break;
    case 6:
        errorMsg.push_back( L"The table is locked" );
        break;
    case 7:
        errorMsg.push_back( L"No more memory" );
        break;
    case 8:
        errorMsg.push_back( L"Database is read-only. No write permited" );
        break;
    case 9:
        errorMsg.push_back( L"Operation terminated." );
        break;
    case 10:
        errorMsg.push_back( L"Disk I/O error occured" );
        break;
    case 11:
        errorMsg.push_back( L"The database is malformed" );
        break;
    case 12:
        errorMsg.push_back( L"Unknown opcode in sqlite3_file_control" );
        break;
    case 13:
        errorMsg.push_back( L"Insertion failed because the database is full" );
        break;
    case 14:
        errorMsg.push_back( L"Unable to open the database" );
        break;
    case 15:
        errorMsg.push_back( L"Database lock protocol error" );
        break;
    case 16:
        errorMsg.push_back( L"Database is empty" );
        break;
    case 17:
        errorMsg.push_back( L"The database schema changed" );
        break;
    case 18:
        errorMsg.push_back( L"String or BLOB exceeds size limit" );
        break;
    case 19:
        errorMsg.push_back( L"Abort due to constraint violation" );
        break;
    case 20:
        errorMsg.push_back( L"Data type mismatch" );
        break;
    case 21:
        errorMsg.push_back( L"Library used incorrectly" );
        break;
    case 22:
        errorMsg.push_back( L"Feature is not supported by OS host" );
        break;
    case 23:
        errorMsg .push_back( L"Authorization denied" );
        break;
    case 24:
        errorMsg.push_back( L"Auxiliary database format error" );
        break;
    case 25:
        errorMsg.push_back( L"Bind parameter out of range" );
        break;
    case 26:
        errorMsg.push_back( L"Not SQLite database" );
        break;
    case 27:
        errorMsg.push_back( L"Notification from the log function" );
        break;
    case 28:
        errorMsg.push_back( L"Warning from the log function" );
        break;
    }
}

int SQLiteDatabase::GetTableListFromDb(std::vector<std::wstring> &errorMsg)
{
    char *err = nullptr;
    sqlite3_stmt *stmt = nullptr, *stmt1 = nullptr;
    int result = 0, res = SQLITE_OK, count = 0;
    std::string query1 = "SELECT name FROM sqlite_master WHERE type = \'table\' OR type = \'view\';";
    /*\"abt_tnam\", \"abt_tid\", \"abt_ownr\", \"abd_fhgt\", \"abd_fwgt\", \"abd_fitl\", \"abd_funl\", \"abd_fstr\", \"abd_fchr\", \"abd_fptc\" smallint, \"abd_ffce\" char(18), \"abh_fhgt\" smallint, \"abh_fwgt\" smallint, \"abh_fitl\" char(1), \"abh_funl\" integer, \"abh_fstr\" integer, \"abh_fchr\" smallint, \"abh_fptc\" smallint, \"abh_ffce\" char(18), \"abl_fhgt\" smallint, \"abl_fwgt\" smallint, \"abl_fitl\" char(1), \"abl_funl\" integer, \"abl_fstr\" integer, \"abl_fchr\" smallint, \"abl_fptc\" smallint, \"abl_ffce\" char(18), \"abt_cmnt\" char(254), PRIMARY KEY( \"abt_tnam\", \"abt_ownr\" ))*/
    std::wstring query2;
    if( m_osId & ( 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 ) ) // Windows
        query2 = L"INSERT OR IGNORE INTO \'sys.abcattbl\' VALUES( 0, ?, 0, \'\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', \'\' );";
    else if( m_osId & ( 1 << 0 | 1 << 1 ) ) // Mac
        query2 = L"INSERT OR IGNORE INTO \'sys.abcattbl\' VALUES( 3, ?, 0, \'\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', 8, 400, \'N\', \'N\', 0, 34, 0, \'MS Sans Serif\', \'\' );";
    else // *nix
    {
#ifdef __DBGTK__
        query2 = L"INSERT OR IGNORE INTO \'sys.abcattbl\' VALUES( 2, ?, 0, \'\', 11, 400, \'N\', \'N\', 0, 34, 0, \'Cantarell\', 11, 400, \'N\', \'N\', 0, 34, 0, \'Cantarell\', 11, 400, \'N\', \'N\', 0, 34, 0, \'Cantarell\', \'\' );";
#elif __DBQT__
        query2 = L"INSERT OR IGNORE INTO \'sys.abcattbl\' VALUES( 4, ?, 0, \'\', 10, 400, \'N\', \'N\', 0, 34, 0, \'Mono Seriff\', 10, 400, \'N\', \'N\', 0, 34, 0, \'Mono Seriff\', 10, 400, \'N\', \'N\', 0, 34, 0, \'Mono Seriff\', \'\' );";
#endif // __DBGTK__
    }
    res = sqlite3_busy_timeout( m_db, 6000 );
    if( res != SQLITE_OK )
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
    }
    res = sqlite3_exec( m_db, "BEGIN EXCLUSIVE TRANSACTION", NULL, NULL, &err );
    if( res != SQLITE_OK )
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
    }
    sqlite3_free( err );
    if( !result )
    {
        res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query2.c_str() ).c_str(), (int) query2.length(), &stmt1, 0 );
        if( res != SQLITE_OK )
        {
            GetErrorMessage( res, errorMsg );
            result = 1;
        }
        if( !result )
        {
            if( ( res = sqlite3_prepare_v2( m_db, query1.c_str(), (int) query1.length(), &stmt, 0 ) ) == SQLITE_OK )
            {
                for( ; ; )
                {
                    res = sqlite3_step( stmt );
                    if( res == SQLITE_ROW  )
                    {
                        const char *tableName = (char *) sqlite3_column_text( stmt, 0 );
                        res = sqlite3_bind_text( stmt1, 1, tableName, -1, SQLITE_STATIC );
                        if( res != SQLITE_OK )
                        {
                            GetErrorMessage( res, errorMsg );
                            result = 1;
                        }
                        else
                        {
                            res = sqlite3_step( stmt1 );
                            if( res != SQLITE_DONE )
                            {
                                GetErrorMessage( res, errorMsg );
                                result = 1;
                            }
                        }
                        pimpl.m_tableDefinitions[sqlite_pimpl->m_catalog].push_back( TableDefinition( sqlite_pimpl->m_catalog, L"main", sqlite_pimpl->m_myconv.from_bytes( tableName ) ) );
                        count++;
                        res = sqlite3_reset( stmt1 );
                    }
                    else if( res == SQLITE_DONE )
                        break;
                    else
                    {
                        result = 1;
                        GetErrorMessage( res, errorMsg );
                        break;
                    }
                }
            }
            else
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    sqlite3_finalize( stmt );
    sqlite3_finalize( stmt1 );
    m_numOfTables = count;
    if( !result )
    {
        res = sqlite3_exec( m_db, "COMMIT TRANSACTION", nullptr, nullptr, &err );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
    }
    else
    {
        res = sqlite3_exec( m_db, "COMMIT TRANSACTION", nullptr, nullptr, &err );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
    }
    return result;
}

int SQLiteDatabase::CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    std::wstring query;
    int res = SQLITE_OK, result = 0;
    sqlite3_stmt *stmt = NULL;
    res = sqlite3_exec( m_db, "BEGIN EXCLUSIVE TRANSACTION", NULL, NULL, 0 );
    if( res == SQLITE_OK )
    {
        bool exists = IsIndexExists( index_name, L"", schemaName, tableName, errorMsg );
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
            if( ( res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( command.c_str() ).c_str(), -1, &stmt, 0 ) ) == SQLITE_OK )
            {
                res = sqlite3_step( stmt );
                if( res != SQLITE_DONE )
                {
                    result = 1;
                    GetErrorMessage( res, errorMsg );
                }
            }
            else
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
            sqlite3_finalize( stmt );
            if( result == 1 )
                query = L"ROLLBACK";
            else
                query = L"COMMIT";
            res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), NULL, NULL, 0 );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    else
    {
        result = 1;
        GetErrorMessage( res, errorMsg );
    }
    return result;
}

bool SQLiteDatabase::IsIndexExists(const std::wstring &indexName, const std::wstring &UNUSED(catalogName), const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    bool exists = false;
    int res = SQLITE_OK, result = 0;
    sqlite3_stmt *stmt = NULL;
    std::wstring dbIndexName;
    std::wstring query = L"PRAGMA " + schemaName;
    query += L".index_list( \"%w\" );";
    char *z = sqlite3_mprintf( sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), tableName.c_str() );
    if( ( res = sqlite3_prepare_v2( m_db, z, -1, &stmt, 0 ) ) == SQLITE_OK )
    {
        for( ; ; )
        {
            res = sqlite3_step( stmt );
            if( res == SQLITE_ROW )
            {
                dbIndexName = sqlite_pimpl->m_myconv.from_bytes( reinterpret_cast<const char *>( sqlite3_column_text( stmt, 2 ) ) );
                if( dbIndexName == indexName )
                {
                    result = 1;
                    errorMsg.push_back( L"Index already exist." );
                    exists = true;
                    break;
                }
            }
            else if( res == SQLITE_DONE )
                break;
            else
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
                break;
            }
        }
    }
    else
    {
        result = 1;
        GetErrorMessage( res, errorMsg );
    }
    sqlite3_finalize( stmt );
    return exists;
}

int SQLiteDatabase::GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    int id;
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
#endif // __DBGTK__
    }
    sqlite3_stmt *stmt = NULL;
    int result = 0;
    std::wstring query = L"SELECT * FROM \"sys.abcattbl\" WHERE \"abt_tnam\" = ? AND \"abt_ownr\" = ? AND abt_os = ?;";
    const unsigned char *dataFontName, *headingFontName, *labelFontName;
    int res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), (int) query.length(), &stmt, 0 );
    if( res == SQLITE_OK )
    {
        res = sqlite3_bind_text( stmt, 1, sqlite_pimpl->m_myconv.to_bytes( table->GetTableName().c_str() ).c_str(), -1, SQLITE_TRANSIENT );
        if( res == SQLITE_OK )
        {
            res = sqlite3_bind_text( stmt, 2, sqlite_pimpl->m_myconv.to_bytes( table->GetTableOwner().c_str() ).c_str(), -1, SQLITE_TRANSIENT );
            if( res == SQLITE_OK )
            {
                res = sqlite3_bind_int( stmt, 3, id );
                if( res == SQLITE_OK )
                {
                    for( ; ; )
                    {
                        res = sqlite3_step( stmt );
                        if( res == SQLITE_ROW )
                        {
                            TableProperties prop;
                            prop.m_dataFontSize = sqlite3_column_int( stmt, 4 );
                            prop.m_dataFontWeight = sqlite3_column_int( stmt, 5 );
                            char *italic = (char *) sqlite3_column_text( stmt, 6 );
                            if( italic )
                                prop.m_dataFontItalic = italic[0] == 'Y';
                            int underline = sqlite3_column_int( stmt, 7 );
                            if( underline > 0 )
                                prop.m_dataFontUnderline = true;
                            int strikethrough = sqlite3_column_int( stmt, 8 );
                            if( strikethrough > 0 )
                                prop.m_dataFontStrikethrough = true;
                            prop.m_dataFontCharacterSet = sqlite3_column_int( stmt, 9 );
                            prop.m_dataFontPixelSize = sqlite3_column_int( stmt, 10 );
                            dataFontName = (const unsigned char *) sqlite3_column_text( stmt, 11 );
                            if( dataFontName )
                                prop.m_dataFontName = sqlite_pimpl->m_myconv.from_bytes( (const char *) dataFontName );
                            prop.m_headingFontSize = sqlite3_column_int( stmt, 12 );
                            prop.m_headingFontWeight = sqlite3_column_int( stmt, 13 );
                            italic = (char *) sqlite3_column_text( stmt, 14 );
                            if( italic )
                                prop.m_headingFontItalic = italic[0] == 'Y';
                            underline = sqlite3_column_int( stmt, 15 );
                            if( underline > 0 )
                                prop.m_headingFontUnderline = true;
                            strikethrough = sqlite3_column_int( stmt, 16 );
                            if( strikethrough > 0 )
                                prop.m_headingFontStrikethrough = true;
                            prop.m_headingFontCharacterSet = sqlite3_column_int( stmt, 17 );
                            prop.m_headingFontPixelSize = sqlite3_column_int( stmt, 18 );
                            headingFontName = (const unsigned char *) sqlite3_column_text( stmt, 19 );
                            if( headingFontName )
                                prop.m_headingFontName = sqlite_pimpl->m_myconv.from_bytes( (const char *) headingFontName );
                            prop.m_labelFontSize = sqlite3_column_int( stmt, 20 );
                            prop.m_labelFontWeight = sqlite3_column_int( stmt, 21 );
                            italic = (char *) sqlite3_column_text( stmt, 22 );
                            if( italic )
                                prop.m_labelFontItalic = italic[0] == 'Y';
                            underline = sqlite3_column_int( stmt, 23 );
                            if( underline > 0 )
                                prop.m_labelFontUnderline = true;
                            strikethrough = sqlite3_column_int( stmt, 24 );
                            if( strikethrough > 0 )
                                prop.m_labelFontStrikethrough = true;
                            prop.m_labelFontCharacterSer = sqlite3_column_int( stmt, 25 );
                            prop.m_labelFontPixelSize = sqlite3_column_int( stmt, 26 );
                            labelFontName = (const unsigned char *) sqlite3_column_text( stmt, 27 );
                            if( labelFontName )
                                prop.m_labelFontName = sqlite_pimpl->m_myconv.from_bytes( (const char *) labelFontName );
                            prop.m_comment = sqlite_pimpl->m_myconv.from_bytes( (const char *) sqlite3_column_text( stmt, 28 ) );
                            table->SetFullName( table->GetSchemaName() + L"." + table->GetTableName() );
                            table->SetTableProperties( prop );
                        }
                        else if( res == SQLITE_DONE )
                            break;
                        else
                        {
                            result = 1;
                            GetErrorMessage( res, errorMsg );
                        }
                    }
                }
            }
            else
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
        else
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
    }
    else
    {
        result = 1;
        GetErrorMessage( res, errorMsg );
    }
    sqlite3_finalize( stmt );
    return result;
}

int SQLiteDatabase::SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int id;
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
#endif // __DBGTK__
    }
    char *error;
    std::wstring query;
    std::wostringstream istr;
    bool exist;
    sqlite3_stmt *stmt = NULL;
    int result = 0;
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
                command = L"UPDATE \"sys.abcattbl\" SET\n \"abt_os\" =";
                command += std::to_wstring( id );
                command += L",\n \"abt_tnam\" = \'";
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
                istr << ( properties.m_dataFontStrikethrough ? 1 : 0 );
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
                command += L",\n \"abd_fwgt\" = ";
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
                command += L",\n \"abl_fstr\" = ";
                istr << ( properties.m_labelFontStrikethrough ? 1 : 0 );
                command += istr.str();
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
                command += tableName;
                command += L"\' AND \"abt_tid\" = ";
                istr << tableId;
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L" AND \"abt_ownr\" = \'";
                command += tableOwner;
                command += L"\';";
            }
            else
            {
                command = L"INSERT INTO \"sys.abcattbl\" VALUES( \'";
                command += std::to_wstring( id );
                command += L"\', \'";
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
                command += L"\', \'";
                istr << ( properties.m_labelFontUnderline ? 1 : 0 );
                command += istr.str();
                istr.clear();
                istr.str( L"" );
                command += L"\', ";
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
                int res = sqlite3_exec( m_db, "BEGIN EXCLUSIVE TRANSACTION", NULL, NULL, &error );
                if( res != SQLITE_OK )
                {
                    GetErrorMessage( res, errorMsg );
                    result = 1;
                }
                else
                {
                    res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( command.c_str() ).c_str(), (int) command.length(), &stmt, 0 );
                    if( res == SQLITE_OK )
                    {
                        res = sqlite3_step( stmt );
                        if( res != SQLITE_DONE )
                        {
                            result = 1;
                            GetErrorMessage( res, errorMsg );
                        }
                    }
                    else
                    {
                        result = 1;
                        GetErrorMessage( res, errorMsg );
                    }
                    sqlite3_finalize( stmt );
                    if( result == 0 )
                        query = L"COMMIT;";
                    else
                        query = L"ROLLBACK;";
                    res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), NULL, NULL, 0 );
                    if( res != SQLITE_OK )
                    {
                        GetErrorMessage( res, errorMsg );
                        result = 1;
                    }
                }
            }
        }
    }
    return result;
}

bool SQLiteDatabase::IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    int id;
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
#endif // __DBGTK__
    }
    bool result = false;
    sqlite3_stmt *stmt = NULL;
    std::wstring name = const_cast<DatabaseTable *>( table )->GetTableName();
    std::wstring owner = const_cast<DatabaseTable *>( table )->GetTableOwner();
    std::wstring query = L"SELECT 1 FROM \"sys.abcattbl\" WHERE \"abt_tnam\" = ? AND \"abt_ownr\" = ? AND abt_os = ?;";
    int res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), (int) query.length(), &stmt, 0 );
    if( res == SQLITE_OK )
    {
        res = sqlite3_bind_text( stmt, 1, sqlite_pimpl->m_myconv.to_bytes( name.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
        if( res == SQLITE_OK )
        {
            res = sqlite3_bind_text( stmt, 2, sqlite_pimpl->m_myconv.to_bytes( owner.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
            if( res == SQLITE_OK )
            {
                res = sqlite3_bind_int( stmt, 3, id );
                if( res == SQLITE_OK )
                {
                    res = sqlite3_step( stmt );
                    if( res == SQLITE_ROW )
                        result = true;
                    else if( res != SQLITE_DONE )
                    {
                        GetErrorMessage( res, errorMsg );
                    }
                }
                else
                {
                    GetErrorMessage( res, errorMsg );
                }
            }
            else
            {
                GetErrorMessage( res, errorMsg );
            }
        }
        else
        {
            GetErrorMessage( res, errorMsg );
        }
    }
    else
    {
        GetErrorMessage( res, errorMsg );
    }
    sqlite3_finalize( stmt );
    return result;
}

int SQLiteDatabase::GetFieldProperties(const std::wstring &tableName, const std::wstring &UNUSED(schemaName), const std::wstring &ownerName, const std::wstring &fieldName, TableField *field, std::vector<std::wstring> &errorMsg)
{
    field->GetFieldProperties().m_display.m_format.clear();
    const char *fieldFormat = nullptr;
    sqlite3_stmt *stmt;
    int result = 0;
    const char *label = nullptr, *heading = nullptr;
    int labelAlignment = 0;
    std::wstring query = L"SELECT * FROM \"sys.abcatcol\" WHERE \"abc_tnam\" = ? AND \"abc_ownr\" = ? AND \"abc_cnam\" = ?;";
    std::wstring query1 = L"SELECT * FROM \"sys.abcatfmt\" WHERE \"abf_type\" = ?";
    int res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), (int) query.length(), &stmt, 0 );
    if( res == SQLITE_OK )
    {
        res = sqlite3_bind_text( stmt, 1, sqlite_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
        if( res == SQLITE_OK )
        {
            res = sqlite3_bind_text( stmt, 2, sqlite_pimpl->m_myconv.to_bytes( ownerName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
            if( res == SQLITE_OK )
            {
                res = sqlite3_bind_text( stmt, 3, sqlite_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
                if( res == SQLITE_OK )
                {
                    res = sqlite3_step( stmt );
                    if( res == SQLITE_ROW )
                    {
                        label = (const char *) sqlite3_column_text( stmt, 5 );
                        labelAlignment = sqlite3_column_int( stmt, 6 );
                        heading = (const char *) sqlite3_column_text( stmt, 7 );
                        int headingAlignment = sqlite3_column_int( stmt, 8 );
                        const char *comment = (const char *) sqlite3_column_text( stmt, 17 );
                        short justify = (short) sqlite3_column_int( stmt, 9 );
                        fieldFormat = (const char *) sqlite3_column_text( stmt, 10 );
                        if( label )
                        {
                            field->GetFieldProperties().m_heading.m_label = sqlite_pimpl->m_myconv.from_bytes( label );
                            field->GetFieldProperties().m_heading.m_labelAlignment = labelAlignment;
                        }
                        if( heading )
                        {
                            field->GetFieldProperties().m_heading.m_heading = sqlite_pimpl->m_myconv.from_bytes( heading );
                            field->GetFieldProperties().m_heading.m_headingAlignment = headingAlignment;
                        }
                        if( comment )
                            field->GetFieldProperties().m_comment = sqlite_pimpl->m_myconv.from_bytes( comment );
                        field->GetFieldProperties().m_display.m_justify = justify;
                    }
                    else if( res != SQLITE_DONE )
                    {
                        result = 1;
                        GetErrorMessage( res, errorMsg );
                    }
                    else
                    {
                        field->GetFieldProperties().m_heading.m_label = fieldName;
                        field->GetFieldProperties().m_heading.m_labelAlignment = 0;
                        field->GetFieldProperties().m_heading.m_heading = fieldName;
                        field->GetFieldProperties().m_heading.m_headingAlignment = 1;
                        field->GetFieldProperties().m_comment = L"";
                        field->GetFieldProperties().m_display.m_justify = 0;
                        fieldFormat = "";
                    }
                }
                else
                {
                    result = 1;
                    GetErrorMessage( res, errorMsg );
                }
            }
            else
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
        else
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
    }
    else
    {
        result = 1;
        GetErrorMessage( res, errorMsg );
    }
    sqlite3_finalize( stmt );
    res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str(), (int) query1.length(), &stmt, 0 );
    int type;
    if( res == SQLITE_OK )
    {
        if( field->GetFieldType() == L"date" )
            type = 82;
        if( field->GetFieldType() == L"datetime" || field->GetFieldType() == L"time" || field->GetFieldType() == L"timestamp" )
            type = 84;
        if( field->GetFieldType() == L"real" )
            type = 81;
        else
            type = 80;
        res = sqlite3_bind_int( stmt, 1, type );
        if( res == SQLITE_OK )
        {
            for( ; ; )
            {
                res = sqlite3_step( stmt );
                if( res == SQLITE_ROW )
                {
                    const char *format = (const char *) sqlite3_column_text( stmt, 1 );
                    const char *formatName = (const char *) sqlite3_column_text( stmt, 0 );
                    if( format && fieldFormat[0] != '\0' )
                    {
                        if( fieldFormat && !strcmp( fieldFormat, formatName ) )
                            field->GetFieldProperties().m_display.m_format[1].push_back( std::make_pair( sqlite_pimpl->m_myconv.from_bytes( formatName ), sqlite_pimpl->m_myconv.from_bytes( format ) ) );
                        else
                            field->GetFieldProperties().m_display.m_format[0].push_back( std::make_pair( sqlite_pimpl->m_myconv.from_bytes( formatName ), sqlite_pimpl->m_myconv.from_bytes( format ) ) );
                    }
                    else if( format )
                        field->GetFieldProperties().m_display.m_format[0].push_back( std::make_pair( sqlite_pimpl->m_myconv.from_bytes( formatName ) , sqlite_pimpl->m_myconv.from_bytes( format ) ) );
                }
                else if( res == SQLITE_DONE )
                    break;
                else if( res != SQLITE_DONE )
                {
                    result = 1;
                    GetErrorMessage( res, errorMsg );
                }
            }
        }
        else
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
    }
    else
    {
        result = 1;
        GetErrorMessage( res, errorMsg );
    }
    sqlite3_finalize( stmt );
    return result;
}

int SQLiteDatabase::ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &UNUSED(foreignKeyFields), const std::wstring &refTableName, const std::vector<std::wstring> &UNUSED(refKeyFields), int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int, std::vector<std::wstring> &errorMsg)
{
    sqlite3_stmt *stmt = NULL;
    std::wstring sql;
    char *error;
    std::vector<std::wstring> references, origFields, refFields;
    std::wstring query0 = L"PRAGMA writable_schema=true", query1 = L"PRAGMA writable_schema=false", query2 = L"BEGIN", query3;
    std::wstring query = L"SELECT sql FROM sqlite_master WHERE tbl_name = \'";
    query += tableName.GetTableName();
    query += L"\' AND type = 'table'";
    int result = 0;
    int res;
    FK_ONUPDATE updProp = NO_ACTION_UPDATE;
    FK_ONDELETE delProp = NO_ACTION_DELETE;
    // set writable schema
    if( !logOnly )
    {
        res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( query0.c_str() ).c_str(), NULL, NULL, &error );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
            sqlite3_free( error );
        }
    }
    // start transacion
    if( !logOnly && !result )
    {
        res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( query2.c_str() ).c_str(), NULL, NULL, &error );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
            sqlite3_free( error );
        }
    }
    if( !result )
    {
        // get Create Table sql
        res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), (int) query.length(), &stmt, NULL );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
        else
        {
            for( ; ; )
            {
                res = sqlite3_step( stmt );
                if( res == SQLITE_ROW )
                    sql = sqlite_pimpl->m_myconv.from_bytes( (const char *) sqlite3_column_text( stmt, 0 ) );
                else if( res == SQLITE_DONE )
                    break;
                else
                {
                    result = 1;
                    GetErrorMessage( res, errorMsg );
                    break;
                }
            }
            sqlite3_finalize( stmt );
            if( !result )
            {
                std::wstring newSQL;
                if( !isNew )
                {
                    DropForeignKey( tableName, newFK, sql, newSQL, refTableName );
                    sql = newSQL;
                }
                if( newFK.size() > 0 )
                {
                    std::wstring fk;
                    if( !keyName.empty() )
                        fk = L", CONSTRAINT " + keyName + L" FOREIGN KEY(";
                    else
                        fk = L", FOREIGN KEY(";
                    for( std::vector<FKField *>::const_iterator it = newFK.begin(); it < newFK.end(); ++it )
                    {
                        fk += (*it)->GetOriginalFieldName();
                        if( it == newFK.end() - 1 )
                            fk += L")";
                        else
                            fk += L", ";
                        origFields.push_back( (*it)->GetOriginalFieldName() );
                    }
                    fk += L" REFERENCES " + newFK.at ( 0 )->GetReferencedTableName() + L"(";
                    for( std::vector<FKField *>::const_iterator it = newFK.begin(); it < newFK.end(); ++it )
                    {
                        fk += (*it)->GetReferencedFieldName();
                        if( it == newFK.end() - 1 )
                            fk += L")";
                        else
                            fk += L", ";
                        refFields.push_back( (*it)->GetReferencedFieldName() );
                    }
                    if( deleteProp > 0 )
                    {
                        fk += L" ON DELETE ";
                        switch( deleteProp )
                        {
                            case 0:
                                fk += L"NO ACTION";
                                delProp = NO_ACTION_DELETE;
                                break;
                            case 1:
                                fk += L"RESTRICT";
                                delProp = RESTRICT_DELETE;
                                break;
                            case 2:
                                fk += L"CASCADE";
                                delProp = CASCADE_DELETE;
                                break;
                            case 3:
                                fk += L"SET NULL";
                                delProp = SET_NULL_DELETE;
                                break;
                            case 4:
                                fk += L"SET DEFAULT";
                                delProp = SET_DEFAULT_DELETE;
                                break;
                        }
                    }
                    if( updateProp > 0 )
                    {
                        fk += L" ON UPDATE ";
                        switch( updateProp )
                        {
                            case 0:
                                fk += L"NO ACTION";
                                updProp = NO_ACTION_UPDATE;
                                break;
                            case 1:
                                fk += L"RESTRICT";
                                updProp = RESTRICT_UPDATE;
                                break;
                            case 2:
                                fk += L"CASCADE";
                                updProp = CASCADE_UPDATE;
                                break;
                            case 3:
                                fk += L"SET NULL";
                                updProp = SET_NULL_UPDATE;
                                break;
                            case 4:
                                fk += L"SET DEFAULT";
                                updProp = SET_DEFAULT_UPDATE;
                                break;
                        }
                    }
                    fk += L")";
                    sql.replace( sql.length() - 1, 1, fk );
                }
                std::wstring fkQuery = L"UPDATE sqlite_master SET sql = ? WHERE type = \'table\' AND name = ?;";
                if( !logOnly )
                {
                    res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( fkQuery.c_str() ).c_str(), (int) query.length(), &stmt, NULL );
                    if( res != SQLITE_OK )
                    {
                        result = 1;
                        GetErrorMessage( res, errorMsg );
                    }
                    else
                    {
                        res = sqlite3_bind_text( stmt, 1, sqlite_pimpl->m_myconv.to_bytes( sql.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
                        if( res != SQLITE_OK )
                        {
                            result = 1;
                            GetErrorMessage( res, errorMsg );
                        }
                        else
                        {
                            res = sqlite3_bind_text( stmt, 2, sqlite_pimpl->m_myconv.to_bytes( tableName.GetTableName().c_str() ).c_str(), -1, SQLITE_TRANSIENT );
                            if( res != SQLITE_OK )
                            {
                                result = 1;
                                GetErrorMessage( res, errorMsg );
                            }
                            else
                            {
                                res = sqlite3_step( stmt );
                                if( res != SQLITE_DONE )
                                {
                                    result = 1;
                                    GetErrorMessage( res, errorMsg );
                                }
                            }
                        }
                    }
                    sqlite3_finalize( stmt );
                    if( !result )
                        query3 = L"COMMIT";
                    else
                        query3 = L"ROLLBACK";
                    // commit transaction
                    res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( query3.c_str() ).c_str(), NULL, NULL, &error );
                    if( res != SQLITE_OK )
                    {
                        result = 1;
                        GetErrorMessage( res, errorMsg );
                        sqlite3_free( error );
                    }
                }
                else
                {
                    command = fkQuery;
                    command.replace( command.find( '?' ), 1, L"\'" + sql + L"\'" );
                    command.replace( command.find( '?' ), 1, L"\'" + tableName.GetTableName() + L"\'" );
                }
                if( !logOnly && !result )
                {
                    res = sqlite3_exec( m_db, "ANALYZE sqlite_master", NULL, NULL, &error );
                    if( res != SQLITE_OK )
                    {
                        result = 1;
                        GetErrorMessage( res, errorMsg );
                        sqlite3_free( error );
                    }
                    else
                    {
                        if( newFK.size() > 0 )
                        {
                            std::map<unsigned long, std::vector<FKField *> > fKeys = tableName.GetForeignKeyVector();
                            unsigned long size = fKeys.size();
                            for( unsigned int i = 0; i < newFK.size(); i++ )
                                fKeys[size].push_back( new FKField( i, keyName, L"", tableName.GetTableName(), newFK.at( i )->GetOriginalFieldName(), L"", newFK.at( i )->GetReferencedTableName(), newFK.at( i )->GetReferencedFieldName(), origFields, refFields, newFK.at( i )->GetOnUpdateConstraint(), newFK.at( i )->GetOnDeleteConstraint() ) );
                        }
                    }
                }
            }
        }
    }
    return result;
}

int SQLiteDatabase::DropForeignKey(std::wstring &command, DatabaseTable *tableName, const std::wstring &keyName, bool logOnly, std::vector<std::wstring> &errorMsg)
{
    sqlite3_stmt *stmt = nullptr;
    int result = 0, res;
    std::vector<std::wstring> createCommands;
    auto name = tableName->GetTableName();
    // 1. Turn off foreign keys
    if( logOnly )
        command = L"PRAGMA foregn_key = OFF;\n\r";
    else
    {
        res = sqlite3_exec( m_db, "PRAGMA foreign_keys = OFF", nullptr, nullptr, nullptr );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
    }
    // 2. Start transaction
    if( !result )
    {
        if( logOnly )
            command += L"BEGIN\n\r";
        else
        {
            res = sqlite3_exec( m_db, "BEGIN", nullptr, nullptr, nullptr );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    // 3. Retrieve all commands to create the table
    if( !result )
    {
        if( logOnly )
            command += L"SELECT type, sql FROM sqlite_schema WHERE tbl_name=" + tableName->GetTableName() + L"\n\r";
        else
        {
            res = sqlite3_prepare_v2( m_db, "SELECT type, sql FROM sqlite_master WHERE tbl_name=?", -1, &stmt, NULL );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    if( !result )
    {
        if( !logOnly )
        {
            res = sqlite3_bind_text( stmt, 1, sqlite_pimpl->m_myconv.to_bytes( name ).c_str(), -1, SQLITE_TRANSIENT );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    std::wstring createCommand;
    if( !result )
    {
        if( !logOnly )
        {
            while( true )
            {
                res = sqlite3_step( stmt );
                if( res == SQLITE_ROW )
                {
                    auto temp = sqlite_pimpl->m_myconv.from_bytes( reinterpret_cast<const char *>( sqlite3_column_text( stmt, 1 ) ) );
                    auto temp1 = temp;
                    std::transform( temp.begin(), temp.end(), temp.begin(), ::toupper );
                    if( temp.find( L"TABLE" ) != std::wstring::npos && temp.find( L"CREATE" ) != std::wstring::npos )
                        createCommand = temp1;
                    createCommands.push_back( temp );
                }
                else if( res == SQLITE_DONE )
                    break;
                else
                {
                    result = 1;
                    GetErrorMessage( res, errorMsg );
                }
            }
        }
    }
    if( !result )
    {
        if( !logOnly )
        {
            res = sqlite3_finalize( stmt );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    // 4. Remove foreign key constraint from CREATE TABLE command
    std::wstring resultCommand, temp;
    std::wregex pattern( keyName, std::regex_constants::icase );
    std::wsmatch findings;
    if( std::regex_search( createCommand, findings, pattern ) )
    {
        auto start = findings[0].first - createCommand.begin();
        auto end = findings[0].second - createCommand.begin();
        auto temp = createCommand.substr( 0, start );
        temp += createCommand.substr( end );
        createCommand = temp;
    }
    // 5. Create new table with revised format
    auto pos = createCommand.find( name );
    if( pos != std::wstring::npos )
        createCommand.replace( pos, name.length(), L"new_" + name );
    if( !result )
    {
        if( logOnly )
            command += createCommand + L"\n\r";
        else
        {
            res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( createCommand.c_str() ).c_str(), nullptr, nullptr, nullptr );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    // 6. Populate data in a new table from the old one
    if( !result )
    {
        temp = L"INSERT INTO new_" + name + L" SELECT * FROM " + name;
        if( logOnly )
            command += temp + L"\n\r";
        else
        {
            res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( temp.c_str() ).c_str(), nullptr, nullptr, nullptr );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    // 7. Drop the old table
    if( !result )
    {
        temp = L"DROP TABLE " + name;
        if( logOnly )
            command += temp + L"\n\r";
        else
        {
            res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( temp.c_str() ).c_str(), nullptr, nullptr, nullptr );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    // 8. Rename the new table
    if( !result )
    {
        temp = L"ALTER TABLE new_" + name + L" RENAME TO " + name;
        if( logOnly )
            command += temp + L"\r\n";
        else
        {
            res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( temp.c_str() ).c_str(), nullptr, nullptr, nullptr );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    // 9. Run all additional commands on the table
    if( !result )
    {
        for( std::vector<std::wstring>::iterator it = createCommands.begin(); it < createCommands.end(); ++it )
        {
            if( logOnly )
                command = (*it) + L"\r\n";
            else
            {
                pattern.assign( L"CREATE\\s+TABLE", std::regex::icase );
                if( std::regex_search( (*it), findings, pattern ) )
                {
                    printf( "Hello" );
                    continue;
                }
                res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( (*it).c_str() ).c_str(), nullptr, nullptr, nullptr );
                if( res != SQLITE_OK )
                {
                    result = 1;
                    GetErrorMessage( res, errorMsg );
                }
            }
        }
    }
    if( !result )
    {
        if( logOnly )
        {
            command += L"PRAGMA foreign_key_check;";
            command += L"\n\r";
        }
        else
        {
            res = sqlite3_exec( m_db, "PRAGMA foreign_key_check", nullptr, nullptr, nullptr );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    if( !result )
    {
        if( logOnly )
        {
            command += L"COMMIT;";
            command += L"\n\r";
        }
        else
        {
            res = sqlite3_exec( m_db, "COMMIT", nullptr, nullptr, nullptr );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    else
    {
        res = sqlite3_exec( m_db, "ROLLBACK", nullptr, nullptr, nullptr );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
    }
    if( !result )
    {
        if( logOnly )
        {
            command += L"PRAGMA foreign_keys = ON";
            command += L"\r\n";
        }
        else
        {
            res = sqlite3_exec( m_db, "PRAGMA foreign_keys = ON", NULL, NULL, NULL );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    return result;
}

int SQLiteDatabase::DropForeignKey(DatabaseTable &tableName, std::vector<FKField *> &newFK, const std::wstring &sql, std::wstring &newSQL, const std::wstring &refTableName)
{
    std::wstring s, sUpper, keyTemp, constraintTemp, refTableOrig;
    std::map<unsigned long, std::vector<FKField *> > fkFields = ( tableName ).GetForeignKeyVector();
    bool isFK = false, isConstraint = false;
    refTableOrig = refTableName;
    std::wistringstream str( sql );
    bool isKeyAdded = true;
    while( std::getline( str, s, L',' ) )
    {
        sUpper = s;
        std::transform( s.begin(), s.end(), s.begin(), [](wchar_t ch) { return static_cast<wchar_t>( std::towupper( ch ) ); } );
        std::wstring temp = s.substr( s.find_first_not_of( L' ' ) );
        size_t fkPos = temp.find( L"FOREIGN KEY" );
        if( fkPos == std::wstring::npos )
            newSQL += sUpper + L',';
        else
        {
            keyTemp += sUpper + L',';
            s = s.substr( fkPos );
            isConstraint = true;
            std::wstring ref = L"REFERENCES ";
            fkPos = keyTemp.find( ref );
            while( fkPos == std::wstring::npos )
            {
                std::getline( str, s, L',' );
                sUpper = s;
                std::transform( s.begin(), s.end(), s.begin(), [](wchar_t ch) { return static_cast<wchar_t>( std::towupper( ch ) ); } );
				keyTemp += sUpper + L',';
                isFK = true;
                fkPos = keyTemp.find( ref );
                keyTemp += sUpper;
            }
            std::wstring temp1 = temp;
            std::wstring tName = sUpper.substr( fkPos + ref.length()  );
            tName = tName.substr( 0, tName.find( L'(' ) );
            tName = tName.substr( tName.find_first_not_of( L' ' ) );
            tName = tName.substr( 0, tName.find_last_not_of( L' ' ) + 1 );
            std::wstring refTable;
            if( newFK.size() > 0 )
                refTable = newFK[0]->GetReferencedTableName();
            else
                refTable = refTableName;
            if( tName == refTableName )
            {
                isKeyAdded = false;
            }
            if( isFK )
            {
                ref = L")";
                temp1 = sUpper.substr( sUpper.find( tName ) );
                while( ( fkPos = temp1.find( ref ) ) == std::wstring::npos )
                {
                    std::getline( str, s, L',' );
                    sUpper = s;
                    std::transform( s.begin(), s.end(), s.begin(), [](wchar_t ch) { return static_cast<wchar_t>( std::towupper( ch ) ); } );
                    keyTemp += sUpper + L',';
                    temp1 = s.substr( s.find_first_not_of( L' ' ) );
                }
            }
            isFK = false;
            if( keyTemp.at( keyTemp.length() - 1 ) == L')' && keyTemp.at( keyTemp.length() - 2 ) != L')' && const_cast<DatabaseTable &>( tableName ).GetForeignKeyVector().size() > 1 )
                keyTemp += L',';
            if( keyTemp.at( keyTemp.length() - 2 ) == L')' )
            {
                if( isKeyAdded )
                {
                    newSQL += keyTemp;
                }
                keyTemp = L"";
            }
            isKeyAdded = true;
        }
    }
    if( newSQL.back() == L',' )
        newSQL = newSQL.substr( 0, newSQL.length() - 1 ) + L")";
    bool found = false;
    for( std::map<unsigned long, std::vector<FKField *> >::iterator it = fkFields.begin(); it != fkFields.end() && !found; it++ )
    {
        for( std::vector<FKField *>::iterator it1 = it->second.begin(); it1 != it->second.end(); )
        {
            if( refTableOrig == (*it1)->GetReferencedTableName() )
            {
                found = true;
                delete (*it1);
                (*it1) = NULL;
                it1 = it->second.erase( it1 );
            }
            else
                ++it1;
        }
        if( found )
        {
            fkFields.erase( it );
            break;
        }
    }
    return 0;
}

int SQLiteDatabase::DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    char *error;
    std::wstring query = L"DROP TABLE ";
    query += tableName;
    int result = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( query ).c_str(), 0, 0, &error );
    if( result != SQLITE_OK )
    {
        GetErrorMessage( result, errorMsg );
        res = 1;
        sqlite3_free( error );
    }
    return res;
}

int SQLiteDatabase::SetFieldProperties(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, const FieldProperties &props, bool isLogOnly, std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    char *error;
    int result = 0;
    result = sqlite3_exec( m_db, "BEGIN", NULL, NULL, &error );
    if( result != SQLITE_OK )
    {
        GetErrorMessage( result, errorMsg );
        res = 1;
        sqlite3_free( error );
    }
    else
    {
        bool exist = IsFieldPropertiesExist( tableName, ownerName, fieldName, errorMsg );
        if( exist )
        {
            command = L"INSERT INTO \"sys.abcatcol\"(\"abc_tnam\", \"abc_ownr\", \"abc_cnam\", \"abc_labl\", \"abc_lpos\", \"abc_hdr\", \"abc_hpos\", \"abc_cmnt\") VALUES(";
            command += tableName;
            command += L", " + ownerName;
            command += L", " + fieldName;
            command += L", " + props.m_heading.m_label;
            command += L", " + std::to_wstring( props.m_heading.m_labelAlignment );
            command += L", " + props.m_heading.m_heading;
            command += L", " + std::to_wstring( props.m_heading.m_headingAlignment );
            command += L", " + props.m_comment + L");";
        }
        else
        {
            command = L"UPDATE \"sys.abcatcol\" SET \"abc_labl\" = ";
            command += props.m_heading.m_label + L", \"abc_lpos\" = ";
            command += std::to_wstring( props.m_heading.m_labelAlignment ) + L", \"abc_hdr\" = ";
            command += props.m_heading.m_heading + L", \"abc_hpos\" = ";
            command += std::to_wstring( props.m_heading.m_headingAlignment ) + L", \"abc_cmnt\" = ";
            command += props.m_comment + L"WHERE \"abc_tnam\" = ";
            command += tableName + L" AND \"abc_ownr\" = ";
            command += ownerName + L" AND \"abc_cnam\" = ";
            command += fieldName + L";";
        }
        if( !isLogOnly )
        {
            result = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( command.c_str() ).c_str(), NULL, NULL, &error );
            if( result != SQLITE_OK )
            {
                GetErrorMessage( result, errorMsg );
                res = 1;
                sqlite3_free( error );
            }
        }
        if( res )
            result = sqlite3_exec( m_db, "ROLLBACK", NULL, NULL, &error );
        else
            result = sqlite3_exec( m_db, "COMMIT", NULL, NULL, &error );
        if( result != SQLITE_OK )
        {
            GetErrorMessage( result, errorMsg );
            res = 1;
            sqlite3_free( error );
        }
    }
    return res;
}

void SQLiteDatabase::SetFullType(TableField *field, const std::wstring &type)
{
    field->SetFullType( type );
}

int SQLiteDatabase::GetServerVersion(std::vector<std::wstring> &UNUSED(errorMsg))
{
    pimpl.m_serverVersion = L"3240";
    pimpl.m_versionMajor = 3;
    pimpl.m_versionMinor = 40;
    pimpl.m_versionRevision = 0;
    pimpl.m_clientVersionMajor = 0;
    pimpl.m_clientVersionMinor = 0;
    return 0;
}

int SQLiteDatabase::NewTableCreation(std::vector<std::wstring> &errorMsg)
{
    sqlite3 *db = nullptr;
    int result = 0, schema;
    int res = sqlite3_open( sqlite_pimpl->m_myconv.to_bytes( sqlite_pimpl->m_catalog.c_str() ).c_str(), &db );
    if( res != SQLITE_OK )
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
    }
    if( !result )
    {
        unsigned int count;
        std::vector<std::wstring> tableNames = pimpl.GetTableNames();
        std::string query1 = "SELECT name FROM sqlite_master WHERE type = 'table' OR type = 'view';";
        std::string query2 = "SELECT count(name) FROM sqlite_master WHERE type = 'table' OR type = 'view';";
        std::vector<TableDefinition> temp;
        if( ( res = sqlite3_prepare_v2( db, "PRAGMA schema_version", -1, &m_stmt1, NULL ) ) == SQLITE_OK )
        {
            if( ( res = sqlite3_step( m_stmt1 ) ) == SQLITE_ROW )
            {
                schema = sqlite3_column_int( m_stmt1, 0 );
                if( schema == m_schema )
                {
                    sqlite3_finalize( m_stmt1 );
                    m_stmt1 = nullptr;
                    return result;
                }
            }
            else
            {
               GetErrorMessage( res, errorMsg );
               result = 1;
            }
        }
        else
        {
            GetErrorMessage( res, errorMsg );
            result = 1;
        }
        sqlite3_finalize( m_stmt1 );
        m_stmt1 = nullptr;
        if( !result )
        {
            if( ( res = sqlite3_prepare_v2( db, query2.c_str(), -1, &m_stmt3, NULL ) ) != SQLITE_OK )
            {
                GetErrorMessage( res, errorMsg );
                result = 1;
            }
        }
        if( !result )
        {
            if( ( res = sqlite3_step( m_stmt3 ) ) == SQLITE_ROW )
            {
                count = sqlite3_column_int( m_stmt3, 0 );
            }
            else if( res != SQLITE_DONE)
            {
                GetErrorMessage( res, errorMsg );
                result = 1;
            }
            sqlite3_finalize( m_stmt3 );
            m_stmt3 = nullptr;
        }
        if( !result )
        {
            if( count != m_numOfTables  )
            {
                if( ( res = sqlite3_prepare_v2( db, query1.c_str(), -1, &m_stmt2, NULL ) ) == SQLITE_OK )
                {
                    GetErrorMessage( res, errorMsg );
                    result = 1;
                }
                if( !result )
                {
                    for( ; ; )
                    {
                        res = sqlite3_step( m_stmt2 );
                        if( res == SQLITE_ROW )
                        {
                            temp.push_back( TableDefinition( sqlite_pimpl->m_catalog, L"Main", sqlite_pimpl->m_myconv.from_bytes( (char *) sqlite3_column_text( m_stmt2, 0 ) ) ) );
                        }
                        else if( res == SQLITE_DONE )
                            break;
                        else
                        {
                            result = 1;
                            GetErrorMessage( res, errorMsg );
                        }
                    }
                    if( !result )
                    {
                        std::lock_guard<std::mutex> locker( GetTableVector().my_mutex );
                        pimpl.m_tableDefinitions[pimpl.m_dbName] = temp;
                        m_numOfTables = count;
                    }
                }
                sqlite3_finalize( m_stmt3 );
                m_stmt3 = nullptr;
            }
        }
    }
    return result;
}

int SQLiteDatabase::AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, bool tableAdded, std::vector<std::wstring> &errors)
{
    int result = 0;
    if( tableAdded )
    {
        std::vector<TableField *> fields;
        std::vector<std::wstring> fk_names, indexes;
        std::map<unsigned long,std::vector<FKField *> > foreign_keys;
        sqlite3_stmt *stmt = NULL, *stmt2 = NULL, *stmt3 = NULL, *stmt4 = NULL;
        std::string fieldName, fieldType, fieldDefaultValue, fkTable, fkField, fkTableField, fkUpdateConstraint, fkDeleteConstraint;
        int res = SQLITE_OK, res1 = SQLITE_OK, res3 = SQLITE_OK, res4 = SQLITE_OK, fieldIsNull, fieldPK, fkReference, autoinc, fkId;
        int count1 = 0, count2 = 0;
        FK_ONUPDATE update_constraint = NO_ACTION_UPDATE;
        FK_ONDELETE delete_constraint = NO_ACTION_DELETE;
        std::map<int, std::vector<std::wstring> > origFields, refFields;
        std::string query2 = "SELECT * FROM pragma_table_info(?, ?)";
        std::string query3 = "SELECT * FROM pragma_foreign_key_list(?, ?)";
        std::string query4 = "SELECT * FROM pragma_index_list(?, ?)";
        if( ( res3 = sqlite3_prepare_v2( m_db, query3.c_str(), (int) query3.length(), &stmt3, 0 ) ) == SQLITE_OK )
        {
            res3 = sqlite3_bind_text( stmt3, 1, sqlite_pimpl->m_myconv.to_bytes( tableName ).c_str(), -1, SQLITE_TRANSIENT );
            if( res3 != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errors );
            }
            else
            {
                res3 = sqlite3_bind_text( stmt3, 2, sqlite_pimpl->m_myconv.to_bytes( schemaName ).c_str(), -1, SQLITE_TRANSIENT );
                if( res3 != SQLITE_OK )
                {
                    result = 1;
                    GetErrorMessage( res, errors );
                }
                else
                {
                    for( ; ; )
                    {
                        res3 = sqlite3_step( stmt3 );
                        if( res3 == SQLITE_ROW  )
                        {
                            fkId = sqlite3_column_int( stmt3, 0 );
                            fkField = reinterpret_cast<const char *>( sqlite3_column_text( stmt3, 3 ) );
                            fkTableField = reinterpret_cast<const char *>( sqlite3_column_text( stmt3, 4 ) );
                            origFields[fkId].push_back( sqlite_pimpl->m_myconv.from_bytes( fkField ) );
                            refFields[fkId].push_back( sqlite_pimpl->m_myconv.from_bytes( fkTableField ) );
                        }
                        else if( res3 == SQLITE_DONE )
                            break;
                        else
                        {
                            result = 1;
                            GetErrorMessage( res3, errors );
                            break;
                        }
                    }
                }
                if( !result )
                {
                    res3 = sqlite3_reset( stmt3 );
                    for( ; ; )
                    {
                        res3 = sqlite3_step( stmt3 );
                        if( res3 == SQLITE_ROW )
                        {
                            fkId = sqlite3_column_int( stmt3, 0 );
                            fkReference = sqlite3_column_int( stmt3, 1 );
                            fkTable = reinterpret_cast<const char *>( sqlite3_column_text( stmt3, 2 ) );
                            fkField = reinterpret_cast<const char *>( sqlite3_column_text( stmt3, 3 ) );
                            fkTableField = reinterpret_cast<const char *>( sqlite3_column_text( stmt3, 4 ) );
                            fkUpdateConstraint = reinterpret_cast<const char *>( sqlite3_column_text( stmt3, 5 ) );
                            if( !strcmp( fkUpdateConstraint.c_str(), "NO ACTION" ) )
                                update_constraint = NO_ACTION_UPDATE;
                            if( !strcmp( fkUpdateConstraint.c_str(), "RESTRICT" ) )
                                update_constraint = RESTRICT_UPDATE;
                            if( !strcmp( fkUpdateConstraint.c_str(), "SET NULL" ) )
                                update_constraint = SET_NULL_UPDATE;
                            if( !strcmp( fkUpdateConstraint.c_str(), "SET DEFAULT" ) )
                                update_constraint = SET_DEFAULT_UPDATE;
                            if( !strcmp( fkUpdateConstraint.c_str(), "CASCADE" ) )
                                update_constraint = CASCADE_UPDATE;
                            fkDeleteConstraint = reinterpret_cast<const char *>( sqlite3_column_text( stmt3, 6 ) );
                            if( !strcmp( fkDeleteConstraint.c_str(), "NO ACTION" ) )
                                delete_constraint = NO_ACTION_DELETE;
                            if( !strcmp( fkDeleteConstraint.c_str(), "RESTRICT" ) )
                                delete_constraint = RESTRICT_DELETE;
                            if( !strcmp( fkDeleteConstraint.c_str(), "SET NULL" ) )
                                delete_constraint = SET_NULL_DELETE;
                            if( !strcmp( fkDeleteConstraint.c_str(), "SET DEFAULT" ) )
                                delete_constraint = SET_DEFAULT_DELETE;
                            if( !strcmp( fkDeleteConstraint.c_str(), "CASCADE" ) )
                                delete_constraint = CASCADE_DELETE;
                                                                       //id, name, orig_schema,         table_name,                                  original_field,                      ref_schema,           ref_table,                              referenced_field,                          update_constraint, delete_constraint
                            foreign_keys[fkId].push_back( new FKField( fkReference, L"", L"main", tableName, sqlite_pimpl->m_myconv.from_bytes( fkField ), L"main", sqlite_pimpl->m_myconv.from_bytes( fkTable ), sqlite_pimpl->m_myconv.from_bytes( fkTableField ), origFields[fkId], refFields[fkId], update_constraint, delete_constraint ) );
                            fk_names.push_back( sqlite_pimpl->m_myconv.from_bytes( fkField ) );
                        }
                        else if( res3 == SQLITE_DONE )
                            break;
                        else
                        {
                            result = 1;
                            GetErrorMessage( res3, errors );
                            break;
                        }
                    }
                }
            }
        }
        sqlite3_finalize( stmt3 );
        if( !result )
        {
            if( ( res1 = sqlite3_prepare_v2( m_db, query2.c_str(), -1, &stmt2, 0 ) ) == SQLITE_OK )
            {
                res1 = sqlite3_bind_text( stmt2, 1, sqlite_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
                if( res1 != SQLITE_OK )
                {
                    result = 1;
                    GetErrorMessage( res, errors );
                }
                else
                {
                    res1 = sqlite3_bind_text( stmt2, 2, sqlite_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
                    if( res1 != SQLITE_OK )
                    {
                        result = 1;
                        GetErrorMessage( res, errors );
                    }
                    else
                    {
                        for( ; ; )
                        {
                            res1 = sqlite3_step( stmt2 );
                            if( res1 == SQLITE_ROW )
                            {
                                unsigned char *temp;
                                fieldName = reinterpret_cast<const char *>( sqlite3_column_text( stmt2, 1 ) );
                                if( ( temp = const_cast<unsigned char *>( sqlite3_column_text( stmt2, 2 ) ) ) == NULL )
                                    fieldType = "";
                                else
                                    fieldType = reinterpret_cast<char *>( temp );
                                fieldIsNull = sqlite3_column_int( stmt2, 3 );
                                if( ( temp = const_cast<unsigned char *>( sqlite3_column_text( stmt2, 4 ) ) ) == NULL )
                                    fieldDefaultValue = "";
                                else
                                    fieldDefaultValue = reinterpret_cast<char *>( temp );
                                fieldPK = sqlite3_column_int( stmt2, 5 );
                                res = sqlite3_table_column_metadata( m_db, NULL, sqlite_pimpl->m_myconv.to_bytes( tableName ).c_str(), fieldName.c_str(), NULL, NULL, NULL, NULL, &autoinc );
                                if( res != SQLITE_OK )
                                {
                                    result = 1;
                                    GetErrorMessage( res, errors );
                                    break;
                                }
                                std::wstring fieldComment = L"";
//                            GetColumnComment( sqlite_pimpl->m_myconv.from_bytes( (const char *) tableName ), sqlite_pimpl->m_myconv.from_bytes( fieldName ), fieldComment, errorMsg );
                                if( errors.empty() )
                                {
                                    std::wstring type = sqlite_pimpl->m_myconv.from_bytes( fieldType );
                                    std::wstring name = sqlite_pimpl->m_myconv.from_bytes( fieldName );
                                    TableField *field = new TableField( name, type, 0, 0, schemaName + L"." + tableName + L"." + name, sqlite_pimpl->m_myconv.from_bytes( fieldDefaultValue ), fieldIsNull == 0 ? false: true, autoinc == 1 ? true : false, fieldPK >= 1 ? true : false, std::find( fk_names.begin(), fk_names.end(), sqlite_pimpl->m_myconv.from_bytes( fieldName ) ) != fk_names.end() );
                                    if( GetFieldProperties( tableName, L"", L"", sqlite_pimpl->m_myconv.from_bytes( fieldName ), field, errors ) )
                                    {
                                        result = 1;
                                        GetErrorMessage( res, errors );
                                        sqlite3_finalize( stmt2 );
                                        break;
                                    }
                                    count1++;
                                    SetFullType( field, type );
                                    fields.push_back( field );
                                }
                            }
                            else if( res1 == SQLITE_DONE )
                                break;
                            else
                            {
                                result = 1;
                                GetErrorMessage( res, errors );
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                result = 1;
                GetErrorMessage( res, errors );
            }
            sqlite3_finalize( stmt2 );
        }
        if( !result )
        {
            if( ( res4 = sqlite3_prepare_v2( m_db, query4.c_str(), -1, &stmt4, 0 ) ) == SQLITE_OK )
            {
                res4 = sqlite3_bind_text( stmt4, 1, sqlite_pimpl->m_myconv.to_bytes( tableName ).c_str(), -1, SQLITE_TRANSIENT );
                if( res4 != SQLITE_OK )
                {
                    result = 1;
                    GetErrorMessage( res, errors );
                }
                else
                {
                    for( ; ; )
                    {
                        res4 = sqlite3_step( stmt4 );
                        if( res4 == SQLITE_ROW )
                        {
                            indexes.push_back( sqlite_pimpl->m_myconv.from_bytes( reinterpret_cast<const char *>( sqlite3_column_text( stmt4, 2 ) ) ) );
                            count2++;
                        }
                        else if( res4 == SQLITE_DONE )
                            break;
                        else
                        {
                            result = 1;
                            GetErrorMessage( res, errors );
                            break;
                        }
                    }
                }
            }
            else
            {
                result = 1;
                GetErrorMessage( res, errors );
            }
        }
        sqlite3_finalize( stmt4 );
        if( !result )
        {
            std::wstring comment = L"";
            std::vector<std::wstring> pk;
            for( std::vector<TableField *>::iterator it = fields.begin (); it < fields.end (); ++it )
            {
                if( (*it)->IsPrimaryKey() )
                {
                    pk.push_back( (*it)->GetFieldName() );
                }
            }
            DatabaseTable *table = new DatabaseTable( tableName, schemaName, fields, pk, foreign_keys );
            table->SetCatalog( catalog );
            table->SetNumberOfFields( count1 );
            table->SetNumberOfIndexes( count2 );
            table->SetTableId( 0 );
            if( GetTableProperties( table, errors ) )
            {
                fields.erase( fields.begin(), fields.end() );
                foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                fk_names.clear();
                sqlite3_finalize( stmt );
                result = 1;
            }
            else
            {
                table->SetIndexNames( indexes );
                pimpl.m_tables[catalog].push_back( table );
                fields.erase( fields.begin(), fields.end() );
                foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                fk_names.clear();
            }
        }
        pimpl.PushTableName( tableName );
    }
    else
    {
        std::vector<DatabaseTable *> &tableVec = pimpl.m_tables[sqlite_pimpl->m_catalog];
        for( std::vector<DatabaseTable *>::iterator it = tableVec.begin(); it != tableVec.end(); )
        {
            if( (*it)->GetTableName() == tableName )
            {
                std::vector<TableField *> fields = (*it)->GetFields();
                for( std::vector<TableField *>::iterator it1 = fields.begin(); it1 < fields.end(); it1++ )
                {
                    delete (*it1);
                    (*it1) = NULL;
                }
                fields.clear();
                std::map<unsigned long,std::vector<FKField *> > fk_fields = (*it)->GetForeignKeyVector();
                for( std::map<unsigned long, std::vector<FKField *> >::iterator it2 = fk_fields.begin(); it2 != fk_fields.end(); it2++ )
                {
                    for( std::vector<FKField *>::iterator it3 = (*it2).second.begin(); it3 < (*it2).second.end(); it3++ )
                    {
                        delete (*it3);
                        (*it3) = NULL;
                    }
                }
                fk_fields.clear();
                delete (*it);
                (*it) = NULL;
                it = tableVec.erase( it );
            }
            else
                ++it;
        }
        pimpl.m_tableNames.erase( std::remove( pimpl.m_tableNames.begin(), pimpl.m_tableNames.end(), tableName ), pimpl.m_tableNames.end() );
    }
    return result;
}

int SQLiteDatabase::GetFieldProperties (const std::wstring &table, TableField *field, std::vector<std::wstring> &errorMsg)
{
//    std::wstring schema = L"", owner = L"";
    return GetFieldProperties( table, L"", L"", field->GetFieldName(), field, errorMsg );
}

bool SQLiteDatabase::IsFieldPropertiesExist(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, std::vector<std::wstring> &errorMsg)
{
    bool exist = false;
    std::wstring query = L"SELECT 1 FROM \"sys.abcatcol\" WHERE \"abc_tnam\" = ? AND \"abc_ownr\" = ? AND \"abc_cnam\" = ?;";
    int res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), -1, &m_stmt1, NULL );
    if( res == SQLITE_OK )
    {
        res = sqlite3_bind_text( m_stmt1, 1, sqlite_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
        if( res == SQLITE_OK )
        {
            res = sqlite3_bind_text( m_stmt1, 2, sqlite_pimpl->m_myconv.to_bytes( ownerName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
            if( res == SQLITE_OK )
            {
                res = sqlite3_bind_text( m_stmt1, 3, sqlite_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
                if( res == SQLITE_OK )
                {
                    res = sqlite3_step( m_stmt1 );
                    if( res == SQLITE_ROW )
                        exist = true;
                }
                else
                {
                    GetErrorMessage( res, errorMsg );
                }
            }
            else
            {
                GetErrorMessage( res, errorMsg );
            }
        }
        else
        {
            GetErrorMessage( res, errorMsg );
        }
    }
    else
    {
        GetErrorMessage( res, errorMsg );
    }
    sqlite3_finalize( m_stmt1 );
    return exist;
}

int SQLiteDatabase::GetFieldHeader(const std::wstring &tableName, const std::wstring &fieldName, std::wstring &headerStr, std::vector<std::wstring> &errorMsg)
{
    int result = 0, res;
    sqlite3_stmt *stmt;
    headerStr = fieldName;
    std::wstring error;
    std::wstring query = L"SEECT pbc_hdr FROM \"sys.abcatcol\" WHERE \"abc_tnam\" = ? AND \"abc_cnam\" = ?";
    if( ( res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), -1, &stmt, NULL ) ) == SQLITE_OK )
    {
        res = sqlite3_bind_text( stmt, 1, sqlite_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
        if( res == SQLITE_OK )
        {
            res = sqlite3_bind_text( stmt, 2, sqlite_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
            if( res == SQLITE_OK )
            {
                for( ; ; )
                {
                    res = sqlite3_step( stmt );
                    if( res == SQLITE_ROW )
                    {
                        headerStr = sqlite_pimpl->m_myconv.from_bytes( (const char *) sqlite3_column_text( stmt, 0 ) );
                        break;
                    }
                    else
                    {
                        GetErrorMessage( res, errorMsg );
                        result = 1;
                        break;
                    }
                }
            }
        }
        else
        {
            GetErrorMessage( res, errorMsg );
            result = 1;
        }
    }
    else
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
    }
    sqlite3_finalize( stmt );
    return result;
}

int SQLiteDatabase::GetAttachedDBList(std::vector<std::wstring> &dbNames, std::vector<std::wstring> &errorMsg)
{
    int result = 0, res;
    sqlite3_stmt *stmt;
    std::wstring query = L"SELECT * FROM pragma_database_list;";
    if( ( res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), -1, &stmt, NULL ) ) == 0 )
    {
        for( ; ; )
        {
            res = sqlite3_step( stmt );
            if( res == SQLITE_ROW )
                dbNames.push_back( sqlite_pimpl->m_myconv.from_bytes( (const char *) sqlite3_column_text( stmt, 1 ) ) );
            else
                break;
        }
        sqlite3_finalize( stmt );
    }
    else
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
    }
    return result;
}

int SQLiteDatabase::PrepareStatement(const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int result = 0, res;
    std::wstring query = L"SELECT * FROM " + schemaName + L"." + tableName;
    if( ( res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), -1, &m_stmt, NULL ) ) != 0 )
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
    }
    return result;
}

int SQLiteDatabase::EditTableData(std::vector<DataEditFiield> &row, std::vector<std::wstring> &errorMsg)
{
    int result = 0, res;
    if( m_fieldsInRecordSet == 0 )
        m_fieldsInRecordSet = sqlite3_column_count( m_stmt );
    res = sqlite3_step( m_stmt );
    if( res == SQLITE_ROW )
    {
        for( unsigned int i = 0; i < m_fieldsInRecordSet; ++i )
        {
            res = sqlite3_column_type( m_stmt, i );
            switch( res )
            {
            case SQLITE_INTEGER:
                row.push_back( DataEditFiield( std::to_wstring( sqlite3_column_int64( m_stmt, i ) ) ) );
                break;
            case SQLITE_FLOAT:
                row.push_back( DataEditFiield( std::to_wstring( sqlite3_column_double( m_stmt, i ) ) ) );
                break;
            case SQLITE_TEXT:
                row.push_back( DataEditFiield( sqlite_pimpl->m_myconv.from_bytes( reinterpret_cast<const char *>( sqlite3_column_text( m_stmt, i ) ) ) ) );
                break;
            case SQLITE_BLOB:
                row.push_back( DataEditFiield( sqlite3_column_blob( m_stmt, i ), sqlite3_column_bytes( m_stmt, i ) ) );
                break;
            }
        }
    }
    else if( res == SQLITE_DONE )
        result = NO_MORE_ROWS;
    else
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
    }
    return result;
}

int SQLiteDatabase::FinalizeStatement(std::vector<std::wstring> &errorMsg)
{
    int result = 0, res;
    if( ( res = sqlite3_finalize( m_stmt ) ) != 0 )
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
    }
    m_fieldsInRecordSet = 0;
    return result;
}

int SQLiteDatabase::GetTableCreationSyntax(const std::wstring tableName, std::wstring &syntax, std::vector<std::wstring> &errorMsg)
{
    std::string query = "SELECT sql FROM sqlite_master WHERE type = 'table' OR type = 'view' AND name = ?;";
    int result = 0, res;
    if( ( res = sqlite3_prepare_v2( m_db, query.c_str(), -1, &m_stmt, NULL ) ) != 0 )
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
    }
    else
    {
        res = sqlite3_bind_text( m_stmt, 1, sqlite_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
        else
        {
            auto found = false;
            for( ; !found; )
            {
                res = sqlite3_step( m_stmt );
                if( res != SQLITE_ROW )
                {
                    result = 1;
                    GetErrorMessage( res, errorMsg );
                    found = true;
                }
                else
                {
                    syntax = sqlite_pimpl->m_myconv.from_bytes( reinterpret_cast<const char *>( sqlite3_column_text( m_stmt, 0 ) ) );
                    found = true;
                }
            }
            sqlite3_finalize( m_stmt );
        }
    }
    return result;
}

int SQLiteDatabase::AttachDatabase(const std::wstring &catalog, const std::wstring &schema, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    char *err;
    sqlite3_stmt *stmt;
    auto query = sqlite3_mprintf( "ATTACH %Q AS %w", sqlite_pimpl->m_myconv.to_bytes( catalog.c_str() ).c_str(), sqlite_pimpl->m_myconv.to_bytes( schema.c_str() ).c_str() );
    auto res = sqlite3_exec( m_db, query, NULL, NULL, &err );
    if( res != SQLITE_OK )
    {
        GetErrorMessage( res, errorMsg );
        result = 1;
        sqlite3_free( err );
    }
    else
    {
        sqlite3_free( query );
        std::wstring query1 = L"SELECT name FROM ";
        query1 += schema;
        query1 += L".sqlite_master WHERE type = 'table' OR type = 'view';";
        if( ( res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str(), (int) query1.length(), &stmt, 0 ) ) == SQLITE_OK )
        {
            for( ; ; )
            {
                res = sqlite3_step( stmt );
                if( res == SQLITE_ROW  )
                {
                    const char *tableName = (char *) sqlite3_column_text( stmt, 0 );
                    pimpl.m_tableDefinitions[catalog].push_back( TableDefinition( catalog, schema, sqlite_pimpl->m_myconv.from_bytes( tableName ) ) );
                }
                else if( res == SQLITE_DONE )
                    break;
                else
                {
                    result = 1;
                    GetErrorMessage( res, errorMsg );
                    break;
                }
            }
        }
        else
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
        sqlite3_finalize( stmt );
    }
    return result;
}

int SQLiteDatabase::GetDatabaseNameList(std::vector<std::wstring> &, std::vector<std::wstring> &)
{
    return 0;
}

int SQLiteDatabase::GetQueryRow(const std::wstring &query, std::vector<std::wstring> &values)
{
    auto result = 0;
    return result;
}

int SQLiteDatabase::AddUpdateFormat()
{
    return 0;
}

int SQLiteDatabase::PopulateValdators(std::vector<std::wstring> &errorMsg)
{
    auto result = 0;
    std::wstring query = L"SELECT * FROM \"sys.abcatvld\";";
    auto res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), (int) query.length(), &m_stmt, nullptr );
    if( res != SQLITE_OK )
    {
        result = 1;
        GetErrorMessage( res, errorMsg );
    }
    else
    {
        for( ; ; )
        {
            res = sqlite3_step( m_stmt );
            if( res == SQLITE_ROW )
            {
                auto name = sqlite_pimpl->m_myconv.from_bytes( (char *) sqlite3_column_text( m_stmt, 0 ) );
                auto rule = sqlite_pimpl->m_myconv.from_bytes( (char *) sqlite3_column_text( m_stmt, 1 ) );
                auto type = sqlite3_column_int( m_stmt, 2 );
                auto control = sqlite3_column_int( m_stmt, 3 );
                auto message = sqlite_pimpl->m_myconv.from_bytes( (char *) sqlite3_column_text( m_stmt, 4 ) );
                pimpl.m_validators.push_back( std::make_tuple( name, rule, type, control, message ) );
            }
            else if( res == SQLITE_DONE )
            {
                break;
            }
            else
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
    }
    sqlite3_finalize( m_stmt );
    return result;
}

int SQLiteDatabase::CreateUpdateValidationRule(bool isNew, const std::wstring &name, const std::wstring &rule, const int type, const std::wstring &message, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"";
    if( isNew )
        query = L"INSERT INTO \"sys.abcatvld\"(\"abv_name\", \"abv_vald\", \"abv_type\", \"abv_cntr\", \"abv_msg\") VALUES( ?, ?, ?, 0, ?)";
    else
        query = L"UPDATE \"sys.abcatvld\" SET \"abv_name\" = ?, \"abv_vald\" = ?, \"abv_type\" = ?, \"abv_cntr\" = 0, \"abv_msg\" = ? WHERE \"abv_name\" = ?1";
    auto res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), (int) query.length(), &m_stmt, nullptr );
    if( res != SQLITE_OK )
    {
        result = 1;
        GetErrorMessage( res, errorMsg );
    }
    else
    {
        res = sqlite3_bind_text( m_stmt, 1, sqlite_pimpl->m_myconv.to_bytes( name.c_str() ).c_str(), (int) name.length(), SQLITE_TRANSIENT );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
        if( !result )
        {
            res = sqlite3_bind_text( m_stmt, 2, sqlite_pimpl->m_myconv.to_bytes( rule.c_str() ).c_str(), (int) name.length(), SQLITE_TRANSIENT );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
        if( !result )
        {
            res = sqlite3_bind_text( m_stmt, 5, sqlite_pimpl->m_myconv.to_bytes( message.c_str() ).c_str(), (int) name.length(), SQLITE_TRANSIENT );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
        if( !result )
        {
            res = sqlite3_bind_int( m_stmt, 3, type );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
        if( !result )
        {
            res = sqlite3_step( m_stmt );
            if( res != SQLITE_DONE )
            {
                result = 1;
                GetErrorMessage( res, errorMsg );
            }
        }
        sqlite3_finalize( m_stmt );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMsg );
        }
    }
    return result;
}

int SQLiteDatabase::GetTablespacesList(std::vector<std::wstring> &list, std::vector<std::wstring> &errorMsg)
{
    UNUSED(list);
    UNUSED(errorMsg);
    return 0;
}

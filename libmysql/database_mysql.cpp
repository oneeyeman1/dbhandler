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
#include <codecvt>
#include <algorithm>
#include <sstream>
#include "mysql.h"
#include "database.h"
#include "database_mysql.h"

MySQLDatabase::MySQLDatabase() : Database()
{
    m_db = NULL;
    pimpl = new Impl;
    pimpl->m_type = L"MySQL";
    pimpl->m_subtype = L"";
    m_pimpl = new MySQLImpl;
    connectToDatabase = false;
    m_flags = 0;
}

MySQLDatabase::~MySQLDatabase()
{
    if( pimpl && m_pimpl )
    {
        std::vector<DatabaseTable *> tableVec = pimpl->m_tables[m_pimpl->m_catalog];
        for( std::vector<DatabaseTable *>::iterator it = tableVec.begin(); it < tableVec.end(); it++ )
        {
            std::vector<Field *> fields = (*it)->GetFields();
            for( std::vector<Field *>::iterator it1 = fields.begin(); it1 < fields.end(); it1++ )
            {
                delete (*it1);
                (*it1) = NULL;
            }
            std::map<int,std::vector<FKField *> > fk_fields = (*it)->GetForeignKeyVector();
            for( std::map<int, std::vector<FKField *> >::iterator it2 = fk_fields.begin(); it2 != fk_fields.end(); it2++ )
            {
                for( std::vector<FKField *>::iterator it3 = (*it2).second.begin(); it3 < (*it2).second.end(); it3++ )
                {
                    delete (*it3);
                    (*it3) = NULL;
                }
            }
            delete (*it);
            (*it) = NULL;
        }
    }
    delete pimpl;
    pimpl = NULL;
    delete m_pimpl;
    m_pimpl = NULL;
}

int MySQLDatabase::CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    result = Disconnect( errorMsg );
    if( !result )
    {
        result = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( name.c_str() ).c_str() );
        if( result )
        {
            result = 1;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
        }
        else
        {
            result = mysql_select_db( m_db, "" );
            if( result )
            {
                result = 1;
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( err );
            }
        }
    }
    else
        result = 1;
    return result;
}

int MySQLDatabase::DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0, res;
    std::wstring query = L"DROP DATABASE " + name;
    std::wstring query1 = L"SET FOREIGN_KEY_CHECKS = 0";
    result = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str() );
    if( !result )
    {
        res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
        if( res )
        {
            result = 1;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
        }
    }
    else
    {
        result = 1;
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
    }
    return result;
}

int MySQLDatabase::Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring err;
    std::wstring errorMessage;
    m_db = mysql_init( m_db );
    if( !m_db )
    {
        err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        errorMsg.push_back( L"Connection to database failed!" );
        result = 1;
    }
    else
    {
        if( TokenizeConnectionString( selectedDSN, errorMsg ) )
        {
            err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            errorMsg.push_back( L"Connection to database failed!" );
            result = 1;
        }
        else
        {
            m_db = mysql_real_connect( m_db, m_pimpl->m_myconv.to_bytes( m_pimpl->m_host.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_pimpl->m_user.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_pimpl->m_password.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_pimpl->m_dbName.c_str() ).c_str(), m_port, m_pimpl->m_socket == L"" ? NULL : m_pimpl->m_myconv.to_bytes( m_pimpl->m_socket.c_str() ).c_str(), m_flags );
            if( !m_db )
            {
                err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( err );
                errorMsg.push_back( L"Connection to database failed!" );
                result = 1;
            }
            else
            {
                if( !connectToDatabase )
                {
                    if( ServerConnect( dbList, errorMsg ) )
                    {
                        err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                        errorMsg.push_back( err );
                        errorMsg.push_back( L"Connection to database failed!" );
                        result = 1;
                    }
                }
                else
                {
                    if( CreateSystemObjectsAndGetDatabaseInfo( errorMsg ) )
                    {
                        result = 1;
                    }
                }
            }
        }
    }
    return result;
}

int MySQLDatabase::CreateSystemObjectsAndGetDatabaseInfo(std::vector<std::wstring> &errorMsg)
{
    int result = 0, res;
    MYSQL_STMT *res_stmt;
    std::wstring err;
    std::wstring query1 = L"CREATE TABLE IF NOT EXISTS abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));";
    std::wstring query2 = L"CREATE TABLE IF NOT EXISTS abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));";
    std::wstring query3 = L"CREATE TABLE IF NOT EXISTS abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));";
    std::wstring query4 = L"CREATE TABLE IF NOT EXISTS abcattbl(abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl integer, abd_fstr integer, abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl integer, abh_fstr integer, abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl integer, abl_fstr integer, abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ));";
    std::wstring query5 = L"CREATE TABLE IF NOT EXISTS abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));";
//    std::wstring query6 = L"SELECT( IF( ( SELECT 1 FROM information_schema.statistics WHERE index_name=\'abcattbl_tnam_ownr\' AND table_name=\'abcattbl\' ) > 0, \"SELECT 0\", \"CREATE INDEX abcattbl_tnam_ownr ON abcattbl(abt_tnam ASC, abt_ownr ASC)\"));";
//    std::wstring query7 = L"SELECT( IF( ( SELECT 1 FROM information_schema.statistics WHERE index_name=\'abcatcol_tnam_ownr_cnam\' AND table_name=\'abcatcol\' ) > 0, \"SELECT 0\", \"CREATE INDEX abcatcol_tnam_ownr_cnam ON abcatcoll(abc_tnam ASC, abc_ownr ASC, abc_cnam ASC)\"));";
    std::wstring query6 = L"CREATE INDEX abcattbl_tnam_ownr ON abcattbl(abt_tnam ASC, abt_ownr ASC);";
    std::wstring query7 = L"CREATE INDEX abcatcol_tnam_ownr_cnam ON abcatcol(abc_tnam ASC, abc_ownr ASC, abc_cnam ASC);";
    std::wstring query8 = L"SELECT USER()";
    std::wstring query = L"SELECT 1 FROM information_schema.statistics WHERE table_name = ? AND index_name = ?;";
    res_stmt = mysql_stmt_init( m_db );
    if( !res_stmt )
    {
        err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res_stmt ) );
        errorMsg.push_back( err );
    }
    res = mysql_query( m_db, "START TRANSACTION" );
    if( res )
    {
        err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        errorMsg.push_back( L"Connection to database failed!" );
        result = 1;
    }
    else
    {
        res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str() );
        if( !res )
        {
            res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query2.c_str() ).c_str() );
            if( !res )
            {
                res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query3.c_str() ).c_str() );
                if( !res )
                {
                    res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query4.c_str() ).c_str()  );
                    if( !res )
                    {
                        if( mysql_stmt_prepare( res_stmt, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), query.length() ) )
                        {
                            err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res_stmt ) );
                            errorMsg.push_back( err );
                        }
                        else
                        {
                            res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query5.c_str() ).c_str() );
                            if( !res )
                            {
                                bool index_exist = IsSystemIndexExists( res_stmt, L"abcattbl_tnam_ownr", L"abcattbl", errorMsg );
                                if( !index_exist )
                                {
                                    res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query6.c_str() ).c_str() );
                                }
                            }
                            bool index_exist = IsSystemIndexExists( res_stmt, L"abcatcol_tnam_ownr_cnam", L"abcatcol", errorMsg );
                            if( !index_exist )
                            {
                                res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query7.c_str() ).c_str() );
                                if( !res )
                                    res = mysql_commit( m_db );
                                else
                                {
                                    err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                                    errorMsg.push_back( err );
                                }
                            }
                            if( mysql_stmt_close( res_stmt ) )
                            {
                                err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res_stmt ) );
                                errorMsg.push_back( err );
                            }
                        }
                    }
                }
            }
        }
    }
    if( res || !errorMsg.empty() )
    {
        err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        errorMsg.push_back( L"Connection to database failed!" );
        res = mysql_rollback( m_db );
        result = 1;
    }
    else
    {
        result = GetTableListFromDb( errorMsg );
        if( !result )
        {
            res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query8.c_str() ).c_str() );
            if( !res )
            {
                MYSQL_RES *results = mysql_store_result( m_db );
                if( !results )
                {
                    err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                    errorMsg.push_back( err );
                    result = 1;
                }
                else
                {
                    MYSQL_ROW row;
                    while( ( row = mysql_fetch_row( results ) ) != NULL )
                    {
                        pimpl->m_connectedUser = m_pimpl->m_myconv.from_bytes( row[0] );
                    }
                }
            }
            else
            {
                err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( err );
                result = 1;
            }
        }
    }
    return result;
}

int MySQLDatabase::ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg)
{
    std::wstring err;
    int result = 0, res;
    std::wstring query = L"SELECT schema_name FROM information_schema.schemata";
    res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
    if( !res )
    {
        err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    else
    {
        MYSQL_RES *res1 = mysql_store_result(m_db);
        if( !res1 )
        {
            err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            result = 1;
        }
        else
        {
            MYSQL_ROW row;
            while( ( row = mysql_fetch_row( res1 ) ) != NULL )
            {
                dbList.push_back( m_pimpl->m_myconv.from_bytes( row[0] ) );
            }
        }
    }
    return result;
}

int MySQLDatabase::Disconnect(std::vector<std::wstring> &UNUSED(errorMsg))
{
    int result = 0;
    mysql_close( m_db );
    m_db = NULL;
    for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it = pimpl->m_tables.begin(); it != pimpl->m_tables.end(); it++ )
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

int MySQLDatabase::GetTableListFromDb(std::vector<std::wstring> &errorMsg)
{
    int res, result = 0;
    MYSQL_STMT *res1 = NULL, *res2 = NULL, *res3 = NULL;
    char *str_data1 = NULL, *str_data2 = NULL, *str_data3 = NULL;
    char fkField[64], refTableSchema[64], refTableName[64], refTableField[64], updateCon[64], deleteCon[64];
    char colName[64], colType[64], defValue[64], nullable[3], autoInc[30], indexName[64];
    MYSQL_RES *prepare_meta_result;
    std::vector<Field *> fields;
    std::vector<std::wstring> fk_names, indexes;
    std::map<int,std::vector<FKField *> > foreign_keys;
    std::wstring errorMessage;
    std::wstring fieldName, fieldType, fieldDefaultValue, fkSchema, fkTable, fkFld, fkTableField, fkUpdateConstraint, fkDeleteConstraint;
    int fkId, charLen, charOctet, numLen, numPrec, pk;
    bool is_nullable, autoincrement, is_pk;
    FK_ONUPDATE update_constraint = NO_ACTION_UPDATE;
    FK_ONDELETE delete_constraint = NO_ACTION_DELETE;
    std::wstring query1 = L"SELECT t.table_catalog AS catalog, t.table_schema, t.table_name, \"\" AS owner, CASE WHEN t.engine = 'InnoDB' THEN (SELECT st.table_id FROM information_schema.INNODB_SYS_TABLES st WHERE CONCAT(t.table_schema,'/', t.table_name) = st.name) ELSE (SELECT 0) END AS id FROM information_schema.tables t WHERE t.table_type = 'BASE TABLE' OR t.table_type = 'VIEW';";
    std::wstring query2 = L"SELECT cols.column_name, cols.data_type, cols.character_maximum_length, cols.character_octet_length, cols.numeric_precision, cols.numeric_scale, cols.column_default, cols.is_nullable, cols.extra, (CASE WHEN kcu.column_name = cols.column_name THEN 1 ELSE 0 END) as pk_flag FROM information_schema.columns cols, information_schema.key_column_usage kcu WHERE kcu.constraint_name = 'PRIMARY' AND kcu.table_schema = cols.table_schema AND kcu.table_name = cols.table_name AND cols.table_catalog = ? AND cols.table_schema = ? AND cols.table_name = ?;";
    std::wstring query3 = L"SELECT kcu.column_name, kcu.ordinal_position, kcu.referenced_table_schema, kcu.referenced_table_name, kcu.referenced_column_name, rc.update_rule, rc.delete_rule FROM information_schema.key_column_usage kcu, information_schema.referential_constraints rc WHERE kcu.constraint_name = rc.constraint_name AND kcu.table_catalog = ? AND kcu.table_schema = ? AND kcu.table_name = ?;";
    std::wstring query4 = L"SELECT index_name FROM information_schema.statistics WHERE table_catalog = ? AND table_schema = ? AND table_name = ?;";
    res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str() );
    if( res )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    else
    {
        MYSQL_RES *results = mysql_store_result( m_db );
        if( !results )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            result = 1;
        }
        else
        {
            res1 = mysql_stmt_init( m_db );
            if( !res1 )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
                errorMsg.push_back( err );
                result = 1;
            }
            else
            {
                if( mysql_stmt_prepare( res1, m_pimpl->m_myconv.to_bytes( query3.c_str() ).c_str(), query3.length() ) )
                {
                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
                    errorMsg.push_back( err );
                    result = 1;
                }
                else
                {
                    res2 = mysql_stmt_init( m_db );
                    if( !res2 )
                    {
                        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
                        errorMsg.push_back( err );
                        result = 1;
                    }
                    else
                    {
                        if( mysql_stmt_prepare( res2, m_pimpl->m_myconv.to_bytes( query2.c_str() ).c_str(), query2.length() ) )
                        {
                            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
                            errorMsg.push_back( err );
                            result = 1;
                        }
                        else
                        {
                            res3 = mysql_stmt_init( m_db );
                            if( !res3 )
                            {
                                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res3 ) );
                                errorMsg.push_back( err );
                                result = 1;
                            }
                            else
                            {
                                if( mysql_stmt_prepare( res3, m_pimpl->m_myconv.to_bytes( query4.c_str() ).c_str(), query4.length() ) )
                                {
                                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res3 ) );
                                    errorMsg.push_back( err );
                                    result = 1;
                                }
                                else
                                {
                                    MYSQL_ROW row;
                                    while( ( row = mysql_fetch_row( results ) ) != NULL )
                                    {
                                        char *end;
                                        char *catalog_name = row[0] ? row[0] : NULL;
                                        char *schema_name = row[1] ? row[1] : NULL;
                                        char *table_name = row[2] ? row[2] : NULL;
                                        long table_id = row[4] ? strtol( row[4], &end, 10 ) : 0;
                                        MYSQL_BIND params[3];
                                        unsigned long str_length1, str_length2, str_length3;
                                        str_length1 = strlen( catalog_name ) * 2;
                                        str_length2 = strlen( schema_name ) * 2;
                                        str_length3 = strlen( table_name ) * 2;
                                        str_data1 = new char[str_length1], str_data2 = new char[str_length2], str_data3 = new char[str_length3];
                                        memset( str_data1, '\0', str_length1 );
                                        memset( str_data2, '\0', str_length2 );
                                        memset( str_data3, '\0', str_length3 );
                                        memset( params, 0, sizeof( params ) );
                                        strncpy( str_data1, catalog_name, str_length1 );
                                        strncpy( str_data2, schema_name, str_length2 );
                                        strncpy( str_data3, table_name, str_length3 );
                                        params[0].buffer_type = MYSQL_TYPE_STRING;
                                        params[0].buffer = (char *) str_data1;
                                        params[0].buffer_length = strlen( str_data1 );
                                        params[0].is_null = 0;
                                        params[0].length = &str_length1;
                                        params[1].buffer_type = MYSQL_TYPE_STRING;
                                        params[1].buffer = (char *) str_data2;
                                        params[1].buffer_length = strlen( str_data2 );
                                        params[1].is_null = 0;
                                        params[1].length = &str_length2;
                                        params[2].buffer_type = MYSQL_TYPE_STRING;
                                        params[2].buffer = (char *) str_data3;
                                        params[2].buffer_length = strlen( str_data3 );
                                        params[2].is_null = 0;
                                        params[2].length = &str_length3;
                                        if( mysql_stmt_bind_param( res1, params ) )
                                        {
                                            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
                                            errorMsg.push_back( err );
                                            result = 1;
                                            break;
                                        }
                                        else
                                        {
                                            prepare_meta_result = mysql_stmt_result_metadata( res1 );
                                            if( !prepare_meta_result )
                                            {
                                                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
                                                errorMsg.push_back( err );
                                                result = 1;
                                                break;
                                            }
                                            else
                                            {
                                                if( mysql_stmt_execute( res1 ) )
                                                {
                                                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
                                                    errorMsg.push_back( err );
                                                    result = 1;
                                                    break;
                                                }
                                                else
                                                {
                                                    MYSQL_BIND results1[7];
                                                    my_bool is_null[7], error[7];
                                                    unsigned long length[7];
                                                    memset( results1, 0, sizeof( results1 ) );
                                                    results1[0].buffer_type = results1[2].buffer_type = results1[3].buffer_type = results1[4].buffer_type = results1[5].buffer_type = results1[6].buffer_type = MYSQL_TYPE_STRING;
                                                    results1[1].buffer_type = MYSQL_TYPE_LONG;

                                                    results1[1].buffer = (char *) &fkId;
                                                    results1[1].is_null = &is_null[1];
                                                    results1[1].error = &error[1];
                                                    results1[1].length = &length[1];

                                                    length[0] = 64;
                                                    results1[0].buffer = fkField;
                                                    results1[0].buffer_length = length[0];
                                                    results1[0].is_null = &is_null[0];
                                                    results1[0].length = &length[0];
                                                    results1[0].error = &error[1];

                                                    length[2] = 64;
                                                    results1[2].buffer = refTableSchema;
                                                    results1[2].buffer_length = length[2];
                                                    results1[2].is_null = &is_null[2];
                                                    results1[2].length = &length[2];
                                                    results1[2].error = &error[2];

                                                    length[3] = 64;
                                                    results1[3].buffer = refTableName;
                                                    results1[3].buffer_length = length[3];
                                                    results1[3].is_null = &is_null[3];
                                                    results1[3].length = &length[3];
                                                    results1[3].error = &error[3];

                                                    length[4] = 64;
                                                    results1[4].buffer = refTableField;
                                                    results1[4].buffer_length = length[4];
                                                    results1[4].is_null = &is_null[4];
                                                    results1[4].length = &length[4];
                                                    results1[4].error = &error[4];

                                                    length[5] = 64;
                                                    results1[5].buffer = updateCon;
                                                    results1[5].buffer_length = length[5];
                                                    results1[5].is_null = &is_null[5];
                                                    results1[5].length = &length[5];
                                                    results1[5].error = &error[5];

                                                    length[6] = 64;
                                                    results1[6].buffer = deleteCon;
                                                    results1[6].buffer_length = length[6];
                                                    results1[6].is_null = &is_null[6];
                                                    results1[6].length = &length[6];
                                                    results1[6].error = &error[6];
                                                    std::vector<std::wstring> origFields, refFields;
                                                    if( mysql_stmt_bind_result( res1, results1 ) )
                                                    {
                                                        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
                                                        errorMsg.push_back( err );
                                                        result = 1;
                                                        break;
                                                    }
                                                    else
                                                    {
                                                        while( !mysql_stmt_fetch( res1 ) )
                                                        {
                                                            fkFld = m_pimpl->m_myconv.from_bytes( fkField );
                                                            fkTableField = m_pimpl->m_myconv.from_bytes( refTableField );
                                                            origFields.push_back( fkFld );
                                                            refFields.push_back( fkTableField );
                                                        }
                                                        if( mysql_stmt_reset( res1 ) )
                                                        {
                                                            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
                                                            errorMsg.push_back( err );
                                                            result = 1;
                                                            break;
                                                        }
														else
                                                        {
                                                            fkFld = m_pimpl->m_myconv.from_bytes( fkField );
                                                            fkSchema = m_pimpl->m_myconv.from_bytes( refTableSchema );
                                                            fkTable = m_pimpl->m_myconv.from_bytes( refTableName );
                                                            fkTableField = m_pimpl->m_myconv.from_bytes( refTableField );
                                                            if( !strcmp( updateCon, "CASCADE" ) )
                                                                update_constraint = CASCADE_UPDATE;
                                                            if( !strcmp( updateCon, "SET NULL" ) )
                                                                update_constraint = SET_NULL_UPDATE;
                                                            if( !strcmp( updateCon, "SET DEFAULT" ) )
                                                                update_constraint = SET_DEFAULT_UPDATE;
                                                            if( !strcmp( updateCon, "RESTRICT" ) )
                                                                update_constraint = RESTRICT_UPDATE;
                                                            if( !strcmp( deleteCon, "CASCADE" ) )
                                                                delete_constraint = CASCADE_DELETE;
                                                            if( !strcmp( deleteCon, "SET NULL" ) )
                                                                delete_constraint = SET_NULL_DELETE;
                                                            if( !strcmp( deleteCon, "SET DEFAULT" ) )
                                                                delete_constraint = SET_DEFAULT_DELETE;
                                                            if( !strcmp( deleteCon, "RESTRICT" ) )
                                                                delete_constraint = RESTRICT_DELETE;
//                                        foreign_keys[].push_back( new FKField( , fkTable, fkFld, fkTableField, fkSchema, update_constraint, delete_constraint ) );
                                                            fk_names.push_back( fkFld );
                                                        }
                                                        if( mysql_stmt_free_result( res1 ) )
                                                        {
                                                            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                                                            errorMsg.push_back( err );
                                                            result = 1;
                                                            break;
                                                        }
                                                        else
                                                        {
                                                            if( mysql_stmt_bind_param( res2, params ) )
                                                            {
                                                                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
                                                                errorMsg.push_back( err );
                                                                result = 1;
                                                                break;
                                                            }
                                                            else
                                                            {
                                                                prepare_meta_result = mysql_stmt_result_metadata( res2 );
                                                                if( !prepare_meta_result )
                                                                {
                                                                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
                                                                    errorMsg.push_back( err );
                                                                    result = 1;
                                                                    break;
                                                                }
                                                                else
                                                                {
                                                                    if( mysql_stmt_execute( res2 ) )
                                                                    {
                                                                        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
                                                                        errorMsg.push_back( err );
                                                                        result = 1;
                                                                        break;
                                                                    }
                                                                    else
                                                                    {
                                                                        MYSQL_BIND results2[10];
                                                                        my_bool is_null1[10], error1[10];
                                                                        long unsigned int length1[10];
     
                                                                        length1[0] = 64;
                                                                        results2[0].buffer = colName;
                                                                        results2[0].buffer_type = MYSQL_TYPE_STRING;
                                                                        results2[0].buffer_length = length1[0];
                                                                        results2[0].is_null = &is_null1[0];
                                                                        results2[0].length = &length1[0];
                                                                        results2[0].error = &error1[0];

                                                                        length1[1] = 64;
                                                                        results2[1].buffer = colType;
                                                                        results2[1].buffer_type = MYSQL_TYPE_STRING;
                                                                        results2[1].buffer_length = length1[1];
                                                                        results2[1].is_null = &is_null1[1];
                                                                        results2[1].length = &length1[1];
                                                                        results2[1].error = &error1[1];

                                                                        results2[2].buffer = (char *) &charLen;
                                                                        results2[2].buffer_type = MYSQL_TYPE_LONG;
                                                                        results2[2].is_null = &is_null1[2];
                                                                        results2[2].length = &length1[2];
                                                                        results2[2].error = &error1[2];

                                                                        results2[3].buffer = (char *) &charOctet;
                                                                        results2[3].buffer_type = MYSQL_TYPE_LONG;
                                                                        results2[3].is_null = &is_null1[3];
                                                                        results2[3].length = &length1[3];
                                                                        results2[3].error = &error1[3];

                                                                        results2[4].buffer = (char *) &numLen;
                                                                        results2[4].buffer_type = MYSQL_TYPE_LONG;
                                                                        results2[4].is_null = &is_null1[4];
                                                                        results2[4].length = &length1[4];
                                                                        results2[4].error = &error1[4];

                                                                        results2[5].buffer = (char *) &numPrec;
                                                                        results2[5].buffer_type = MYSQL_TYPE_LONG;
                                                                        results2[5].is_null = &is_null1[5];
                                                                        results2[5].length = &length1[5];
                                                                        results2[5].error = &error1[5];

                                                                        length1[6] = 64;
                                                                        results2[6].buffer = defValue;
                                                                        results2[6].buffer_type = MYSQL_TYPE_STRING;
                                                                        results2[6].buffer_length = length1[6];
                                                                        results2[6].is_null = &is_null1[6];
                                                                        results2[6].length = &length1[6];
                                                                        results2[6].error = &error[6];

                                                                        length1[7] = 3;
                                                                        results2[7].buffer = nullable;
                                                                        results2[7].buffer_type = MYSQL_TYPE_STRING;
                                                                        results2[7].buffer_length = length1[7];
                                                                        results2[7].is_null = &is_null1[7];
                                                                        results2[7].length = &length1[7];
                                                                        results2[7].error = &error[7];
 
                                                                        length1[8] = 30;
                                                                        results2[8].buffer = autoInc;
                                                                        results2[8].buffer_type = MYSQL_TYPE_STRING;
                                                                        results2[8].buffer_length = length1[8];
                                                                        results2[8].is_null = &is_null1[8];
                                                                        results2[8].length = &length1[8];
                                                                        results2[8].error = &error1[8];

                                                                        results2[9].buffer = (char *) &pk;
                                                                        results2[9].buffer_type = MYSQL_TYPE_LONG;
                                                                        results2[9].is_null = &is_null1[9];
                                                                        results2[9].length = &length1[9];
                                                                        results2[9].error = &error1[9];

                                                                        if( mysql_stmt_bind_result( res2, results2 ) )
                                                                        {
                                                                            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
                                                                            errorMsg.push_back( err );
                                                                            result = 1;
                                                                            break;
                                                                        }
                                                                        else
                                                                        {
                                                                            if( mysql_stmt_store_result( res2 ) )
                                                                            {
                                                                                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
                                                                                errorMsg.push_back( err );
                                                                                result = 1;
                                                                                break;
                                                                            }
                                                                            else
                                                                            {
                                                                                int fieldSize = 0, fieldPrec = 0;
                                                                                while( !mysql_stmt_fetch( res2 ) )
                                                                                {
                                                                                    fieldName = m_pimpl->m_myconv.from_bytes( colName );
                                                                                    fieldType = m_pimpl->m_myconv.from_bytes( colType );
                                                                                    if( !is_null1[6] )
                                                                                        fieldDefaultValue = m_pimpl->m_myconv.from_bytes( defValue );
                                                                                    else
                                                                                        fieldDefaultValue = L"";
                                                                                    if( !is_null1[3] )
                                                                                    {
                                                                                        fieldSize = charLen;
                                                                                        fieldPrec = 0;
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        fieldSize = numLen;
                                                                                        fieldPrec = numPrec;
                                                                                    }
                                                                                    if( !is_null1[7] )
                                                                                    {
                                                                                        if( !strcmp( nullable, "YES" ) )
                                                                                            is_nullable = true;
                                                                                        else
                                                                                            is_nullable = false;
                                                                                    }
                                                                                    else
                                                                                        is_nullable = false;
                                                                                    if( !strcmp( autoInc, "autoincrement" ) )
                                                                                        autoincrement = true;
                                                                                    else
                                                                                        autoincrement = false;
                                                                                    is_pk = pk == 1 ? true : false;
                                                                                    Field *field = new Field( fieldName, fieldType, fieldSize, fieldPrec, fieldDefaultValue, is_nullable, autoincrement, is_pk, std::find( fk_names.begin(), fk_names.end(), fieldName ) != fk_names.end() );
                                                                                    if( GetFieldProperties( m_pimpl->m_myconv.from_bytes( table_name ), m_pimpl->m_myconv.from_bytes( schema_name ), L"", m_pimpl->m_myconv.from_bytes( colName ), field, errorMsg ) )
                                                                                    {
                                                                                        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
                                                                                        errorMsg.push_back( err );
                                                                                        result = 1;
                                                                                        break;
                                                                                    }
                                                                                    std::wstring type = fieldType;
                                                                                    if( fieldSize > 0 )
                                                                                    {
                                                                                        std::wostringstream ss;
                                                                                        ss << fieldSize;
                                                                                        type += L"(";
                                                                                        type += ss.str();
                                                                                    }
                                                                                    if( fieldPrec > 0 )
                                                                                    {
                                                                                        std::wostringstream ss;
                                                                                        ss << fieldPrec;
                                                                                        type += L",";
                                                                                        type += ss.str();
                                                                                        type += L")";
                                                                                    }
                                                                                    else
                                                                                        type += L")";
                                                                                    field->SetFullType( type );
                                                                                }
                                                                                if( result )
                                                                                    break;
                                                                                else
                                                                                {
                                                                                    mysql_free_result( prepare_meta_result );
                                                                                    if( mysql_stmt_free_result( res2 ) )
                                                                                    {
                                                                                        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                                                                                        errorMsg.push_back( err );
                                                                                        result = 1;
                                                                                        break;
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        std::wstring tableName = m_pimpl->m_myconv.from_bytes( (const char *) table_name );
                                                                                        std::wstring schemaName = m_pimpl->m_myconv.from_bytes( (const char *) schema_name );
                                                                                        DatabaseTable *table = new DatabaseTable( tableName, schemaName, fields, foreign_keys );
                                                                                        table->SetTableId( table_id );
                                                                                        if( mysql_stmt_bind_param( res3, params ) )
                                                                                        {
                                                                                            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res3 ) );
                                                                                            errorMsg.push_back( err );
                                                                                            result = 1;
                                                                                            break;
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            prepare_meta_result = mysql_stmt_result_metadata( res3 );
                                                                                            if( !prepare_meta_result )
                                                                                            {
                                                                                                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res3 ) );
                                                                                                errorMsg.push_back( err );
                                                                                                result = 1;
                                                                                                break;
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                                if( mysql_stmt_execute( res3 ) )
                                                                                                {
                                                                                                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res3 ) );
                                                                                                    errorMsg.push_back( err );
                                                                                                    result = 1;
                                                                                                    break;
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    MYSQL_BIND results3[1];
                                                                                                    my_bool is_null3[1], error3[1];
                                                                                                    unsigned long length3[1];
                                                                                                    memset( results3, 0, sizeof( results ) );
                                                                                                    results3[0].buffer_type = MYSQL_TYPE_STRING;
                                                                                                    length[0] = 64;
                                                                                                    results3[0].buffer = indexName;
                                                                                                    results3[0].buffer_length = length3[0];
                                                                                                    results3[0].is_null = &is_null3[0];
                                                                                                    results3[0].length = &length[0];
                                                                                                    results3[0].error = &error3[0];
                                                                                                    if( mysql_stmt_bind_result( res3, results3 ) )
                                                                                                    {
                                                                                                        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res3 ) );
                                                                                                        errorMsg.push_back( err );
                                                                                                        result = 1;
                                                                                                        break;
                                                                                                    }
                                                                                                    else
                                                                                                    {
                                                                                                        while( !mysql_stmt_fetch( res3 ) )
                                                                                                            indexes.push_back( m_pimpl->m_myconv.from_bytes( indexName ) );
                                                                                                        table->SetIndexNames( indexes );
                                                                                                        mysql_stmt_free_result( res3 );
                                                                                                        if( GetTableProperties( table, errorMsg ) )
                                                                                                        {
                                                                                                            result = 1;
                                                                                                            break;
                                                                                                        }
                                                                                                        pimpl->m_tables[m_pimpl->m_myconv.from_bytes( catalog_name )].push_back( table );
                                                                                                        fields.clear();
                                                                                                        foreign_keys.clear();
                                                                                                        fk_names.clear();
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    delete str_data1;
    str_data1 = NULL;
    delete str_data2;
    str_data2 = NULL;
    delete str_data3;
    str_data3 = NULL;
    if( result )
        Disconnect( errorMsg );
    return result;
}

int MySQLDatabase::CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    bool exists = IsIndexExists( index_name, schemaName, tableName, errorMsg );
    if( exists )
    {
        std::wstring temp1 = L"Index ";
        temp1 += index_name;
        temp1 += L" already exists.";
        errorMsg.push_back( temp1 );
        result = 1;
    }
    else
    {
        int res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() );
        if( res )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            result = 1;
        }
    }
    return result;
}

bool MySQLDatabase::IsSystemIndexExists(MYSQL_STMT *res, const std::wstring &indexName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    bool exists = false;
    char *str_data[2] = { NULL, NULL };
    unsigned long *str_length[3];
    MYSQL_BIND values[2];
    memset( values, 0, sizeof( values ) );
    str_data[0] = new char[tableName.length() + 1];
    str_data[1] = new char[indexName.length() + 1];
    memset( str_data[0], 0, tableName.length() + 1 );
    memset( str_data[1], 0, indexName.length() + 1 );
    str_length[0] = new unsigned long;
    str_length[1] = new unsigned long;
    values[0].buffer_type = MYSQL_TYPE_STRING;
    values[1].buffer_type = MYSQL_TYPE_STRING;
    values[0].buffer = str_data[0];
    values[1].buffer = str_data[1];
    values[0].buffer_length = tableName.length();
    values[1].buffer_length = indexName.length();
    values[0].is_null = 0;
    values[1].is_null = 0;
    values[0].length = str_length[0];
    values[1].length = str_length[1];
    strncpy( str_data[0], m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), tableName.length() );
    strncpy( str_data[1], m_pimpl->m_myconv.to_bytes( indexName.c_str() ).c_str(), indexName.length() );
    *str_length[0] = tableName.length();
    *str_length[1] = indexName.length();
    if( mysql_stmt_bind_param( res, values ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res ) );
        errorMsg.push_back( err );
    }
    else
    {
        if( mysql_stmt_execute( res ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res ) );
            errorMsg.push_back( err );
        }
        else
        {
            if( mysql_stmt_fetch( res ) != MYSQL_NO_DATA )
                exists = 1;
        }
    }
    if( mysql_stmt_free_result( res ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
    }
    if( mysql_stmt_reset( res ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
    }
    delete str_data[0];
    str_data[0] = NULL;
    delete str_data[1];
    str_data[1] = NULL;
    delete str_length[0];
    str_length[0] = NULL;
    delete str_length[1];
    str_length[1] = NULL;
    return exists;
}

bool MySQLDatabase::IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    MYSQL_STMT *res;
    bool exists = false;
    char *str_data[3];
    unsigned long *str_length[3];
    std::wstring query = L"SELECT 1 FROM information_schema.statistics WHERE table_schema = ? AND table_name = ? AND index_name = ?;";
    res = mysql_stmt_init( m_db );
    if( !res )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
    }
    else
    {
        if( mysql_stmt_prepare( res, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), query.length() ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res ) );
            errorMsg.push_back( err );
        }
        else
        {
            MYSQL_BIND values[3];
            memset( values, 0, sizeof( values ) );
            str_data[0] = new char[schemaName.length()];
            str_data[1] = new char[tableName.length()];
            str_data[2] = new char[indexName.length()];
            str_length[0] = new unsigned long;
            str_length[1] = new unsigned long;
            str_length[2] = new unsigned long;
            values[0].buffer_type = MYSQL_TYPE_STRING;
            values[1].buffer_type = MYSQL_TYPE_STRING;
            values[2].buffer_type = MYSQL_TYPE_STRING;
            values[0].buffer = str_data[0];
            values[1].buffer = str_data[1];
            values[2].buffer = str_data[2];
            values[0].buffer_length = schemaName.length();
            values[1].buffer_length = tableName.length();
            values[2].buffer_length = indexName.length();
            values[0].is_null = 0;
            values[1].is_null = 0;
            values[2].is_null = 0;
            values[0].length = str_length[0];
            values[1].length = str_length[1];
            values[2].length = str_length[2];
            strncpy( str_data[0], m_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str(), schemaName.length() );
            strncpy( str_data[1], m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), tableName.length() );
            strncpy( str_data[2], m_pimpl->m_myconv.to_bytes( indexName.c_str() ).c_str(), indexName.length() );
            *str_length[0] = schemaName.length();
            *str_length[1] = tableName.length();
            *str_length[2] = indexName.length();
            if( mysql_stmt_bind_param( res, values ) )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res ) );
                errorMsg.push_back( err );
            }
            else
            {
                if( mysql_stmt_execute( res ) )
                {
                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res ) );
                    errorMsg.push_back( err );
                }
                else
                {
                    if( mysql_stmt_fetch( res ) != MYSQL_NO_DATA )
                        exists = 1;
                }
            }
        }
        if( mysql_stmt_close( res ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
        }
    }
    delete str_data[0];
    str_data[0] = NULL;
    delete str_data[1];
    str_data[1] = NULL;
    delete str_data[2];
    str_data[2] = NULL;
    delete str_length[0];
    str_length[0] = NULL;
    delete str_length[1];
    str_length[1] = NULL;
    delete str_length[2];
    str_length[2] = NULL;
    return exists;
}

int MySQLDatabase::GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    MYSQL_STMT *stmt = NULL;
    int result = 0;
    char *str_data1 = NULL, *str_data2 = NULL, tName[129], owner[129], datafontname[18], headingfontname[18], labelfontname[18], comments[254];
    int tableId;
    short int datafontheight, datafontweight, datafontset, datafontptc, headingfontheight, headingfontweight, headingfontset, headingfontptc, labelfontheight, labelfontweight, labelfontset, labelfontptc;
    short int datafontunderline, headingfontunderline, labelfontunderline, datafontstriken, headingfontstriken, labelfontstriken;
    char datafontitalic = 'N', headingfontitalic = 'N', labelfontitalic = 'N';
    std::wstring query = L"SELECT rtrim(abt_tnam), abt_tid, rtrim(abt_ownr), abd_fhgt, abd_fwgt, abd_fitl, abd_funl, abd_fstr, abd_fchr, abd_fptc, rtrim(abd_ffce), abh_fhgt, abh_fwgt, abh_fitl, abh_funl, abh_fstr, abh_fchr, abh_fptc, rtrim(abh_ffce), abl_fhgt, abl_fwgt, abl_fitl, abl_funl, abl_fstr, abl_fchr, abl_fptc, rtrim(abl_ffce), rtrim(abt_cmnt) FROM abcattbl WHERE abt_ownr = ? AND abt_tnam = ?;";
    std::wstring schemaName = table->GetSchemaName(), tableName = table->GetTableName();
    stmt = mysql_stmt_init( m_db );
    if( !stmt )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( stmt ) );
        errorMsg.push_back( err );
        result = 1;
    }
    else
    {
        int res = mysql_stmt_prepare( stmt, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), query.length() );
        if( res )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( stmt ) );
            errorMsg.push_back( err );
            result = 1;
        }
        else
        {
            MYSQL_BIND params[2];
            unsigned long str_length1, str_length2;
            str_data1 = new char[schemaName.length()], str_data2 = new char[tableName.length()];
            memset( params, 0, sizeof( params ) );
            params[0].buffer_type = MYSQL_TYPE_STRING;
            params[0].buffer = (char *) str_data1;
            params[0].buffer_length = schemaName.length();
            params[0].is_null = 0;
            params[0].length = &str_length1;
            params[1].buffer_type = MYSQL_TYPE_STRING;
            params[1].buffer = (char *) str_data2;
            params[1].buffer_length = tableName.length();
            params[1].is_null = 0;
            params[1].length = &str_length2;
            res = mysql_stmt_bind_param( stmt, params );
            if( res )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( stmt ) );
                errorMsg.push_back( err );
                result = 1;
            }
            else
            {
                strncpy( str_data1, m_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str(), schemaName.length() );
                strncpy( str_data2, m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), tableName.length() );
                str_length1 = schemaName.length();
                str_length2 = tableName.length();
                res = mysql_stmt_execute( stmt );
                if( res )
                {
                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( stmt ) );
                    errorMsg.push_back( err );
                    result = 1;
                }
                else
                {
                    MYSQL_BIND results[28];
                    my_bool is_null[28], error[28];
                    unsigned long length[28];
                    memset( results, 0, sizeof( results ) );
                    results[0].buffer_type = MYSQL_TYPE_STRING;
                    results[0].buffer = (char *)tName;
                    results[0].buffer_length = 129;
                    results[0].is_null = &is_null[0];
                    results[0].length = &length[0];
                    results[0].error = &error[0];
                    results[1].buffer_type = MYSQL_TYPE_LONG;
                    results[1].buffer = (char *) &tableId;
                    results[1].is_null = &is_null[1];
                    results[1].length = &length[1];
                    results[1].error = &error[1];
                    results[2].buffer_type = MYSQL_TYPE_STRING;
                    results[2].buffer = (char *)owner;
                    results[2].buffer_length = 129;
                    results[2].is_null = &is_null[2];
                    results[2].length = &length[2];
                    results[2].error = &error[2];
                    results[3].buffer_type = MYSQL_TYPE_SHORT;
                    results[3].buffer = (char *) &datafontheight;
                    results[3].is_null = &is_null[3];
                    results[3].length = &length[3];
                    results[3].error = &error[3];
                    results[4].buffer_type = MYSQL_TYPE_SHORT;
                    results[4].buffer = (char *) &datafontweight;
                    results[4].is_null = &is_null[4];
                    results[4].length = &length[4];
                    results[4].error = &error[4];
                    results[5].buffer_type = MYSQL_TYPE_STRING;
                    results[5].buffer = (char *)&datafontitalic;
                    results[5].buffer_length = 1;
                    results[5].is_null = &is_null[5];
                    results[5].length = &length[5];
                    results[5].error = &error[5];
                    results[6].buffer_type = MYSQL_TYPE_SHORT;
                    results[6].buffer = (char *)&datafontunderline;
                    results[6].is_null = &is_null[6];
                    results[6].length = &length[6];
                    results[6].error = &error[6];
                    results[7].buffer_type = MYSQL_TYPE_SHORT;
                    results[7].buffer = (char *)&datafontstriken;
                    results[7].is_null = &is_null[7];
                    results[7].length = &length[7];
                    results[7].error = &error[7];
                    results[8].buffer_type = MYSQL_TYPE_SHORT;
                    results[8].buffer = (char *) &datafontset;
                    results[8].is_null = &is_null[8];
                    results[8].length = &length[8];
                    results[8].error = &error[8];
                    results[9].buffer_type = MYSQL_TYPE_SHORT;
                    results[9].buffer = (char *) &datafontptc;
                    results[9].is_null = &is_null[9];
                    results[9].length = &length[9];
                    results[9].error = &error[9];
                    results[10].buffer_type = MYSQL_TYPE_STRING;
                    results[10].buffer = (char *)datafontname;
                    results[10].buffer_length = 18;
                    results[10].is_null = &is_null[10];
                    results[10].length = &length[10];
                    results[10].error = &error[10];
                    results[11].buffer_type = MYSQL_TYPE_SHORT;
                    results[11].buffer = (char *) &headingfontheight;
                    results[11].is_null = &is_null[11];
                    results[11].length = &length[11];
                    results[11].error = &error[11];
                    results[12].buffer_type = MYSQL_TYPE_SHORT;
                    results[12].buffer = (char *) &headingfontweight;
                    results[12].is_null = &is_null[12];
                    results[12].length = &length[12];
                    results[12].error = &error[12];
                    results[13].buffer_type = MYSQL_TYPE_STRING;
                    results[13].buffer = (char *)&headingfontitalic;
                    results[13].buffer_length = 1;
                    results[13].is_null = &is_null[13];
                    results[13].length = &length[13];
                    results[13].error = &error[13];
                    results[14].buffer_type = MYSQL_TYPE_SHORT;
                    results[14].buffer = (char *)&headingfontunderline;
                    results[14].is_null = &is_null[14];
                    results[14].length = &length[14];
                    results[14].error = &error[14];
                    results[15].buffer_type = MYSQL_TYPE_SHORT;
                    results[15].buffer = (char *)&headingfontstriken;
                    results[15].is_null = &is_null[15];
                    results[15].length = &length[15];
                    results[15].error = &error[15];
                    results[16].buffer_type = MYSQL_TYPE_SHORT;
                    results[16].buffer = (char *) &headingfontset;
                    results[16].is_null = &is_null[16];
                    results[16].length = &length[16];
                    results[16].error = &error[16];
                    results[17].buffer_type = MYSQL_TYPE_SHORT;
                    results[17].buffer = (char *) &headingfontptc;
                    results[17].is_null = &is_null[17];
                    results[17].length = &length[17];
                    results[17].error = &error[17];
                    results[18].buffer_type = MYSQL_TYPE_STRING;
                    results[18].buffer = (char *)headingfontname;
                    results[18].buffer_length = 18;
                    results[18].is_null = &is_null[18];
                    results[18].length = &length[18];
                    results[18].error = &error[18];
                    results[19].buffer_type = MYSQL_TYPE_SHORT;
                    results[19].buffer = (char *) &labelfontheight;
                    results[19].is_null = &is_null[19];
                    results[19].length = &length[19];
                    results[19].error = &error[19];
                    results[20].buffer_type = MYSQL_TYPE_SHORT;
                    results[20].buffer = (char *) &labelfontweight;
                    results[20].is_null = &is_null[20];
                    results[20].length = &length[20];
                    results[20].error = &error[20];
                    results[21].buffer_type = MYSQL_TYPE_STRING;
                    results[21].buffer = (char *)&labelfontitalic;
                    results[21].buffer_length = 1;
                    results[21].is_null = &is_null[21];
                    results[21].length = &length[21];
                    results[21].error = &error[21];
                    results[22].buffer_type = MYSQL_TYPE_SHORT;
                    results[22].buffer = (char *)&labelfontunderline;
                    results[22].is_null = &is_null[22];
                    results[22].length = &length[22];
                    results[22].error = &error[22];
                    results[23].buffer_type = MYSQL_TYPE_SHORT;
                    results[23].buffer = (char *)&labelfontstriken;
                    results[23].is_null = &is_null[23];
                    results[23].length = &length[23];
                    results[23].error = &error[23];
                    results[24].buffer_type = MYSQL_TYPE_SHORT;
                    results[24].buffer = (char *) &labelfontset;
                    results[24].is_null = &is_null[24];
                    results[24].length = &length[24];
                    results[24].error = &error[24];
                    results[25].buffer_type = MYSQL_TYPE_SHORT;
                    results[25].buffer = (char *) &labelfontptc;
                    results[25].is_null = &is_null[25];
                    results[25].length = &length[25];
                    results[25].error = &error[25];
                    results[26].buffer_type = MYSQL_TYPE_STRING;
                    results[26].buffer = (char *)labelfontname;
                    results[26].buffer_length = 18;
                    results[26].is_null = &is_null[26];
                    results[26].length = &length[26];
                    results[26].error = &error[26];
                    results[27].buffer_type = MYSQL_TYPE_STRING;
                    results[27].buffer = (char *)comments;
                    results[27].buffer_length = 254;
                    results[27].is_null = &is_null[27];
                    results[27].length = &length[27];
                    results[27].error = &error[27];
                    if( mysql_stmt_bind_result( stmt, results ) )
                    {
                        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( stmt ) );
                        errorMsg.push_back( err );
                        result = 1;
                    }
                    else
                    {
                        if( mysql_stmt_store_result( stmt ) )
                        {
                            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( stmt ) );
                            errorMsg.push_back( err );
                            result = 1;
                        }
                        else
                        {
                            while( !mysql_stmt_fetch( stmt ) )
                            {
                                table->SetDataFontSize( datafontheight );
                                table->SetDataFontWeight( datafontweight );
                                table->SetDataFontItalic( datafontitalic );
                                table->SetDataFontUnderline( datafontunderline );
                                table->SetDataFontCharacterSet( datafontset );
                                table->SetDataFontPixelSize( datafontptc );
                                table->SetDataFontName( m_pimpl->m_myconv.from_bytes( datafontname ) );
                                table->SetHeadingFontSize( headingfontheight );
                                table->SetHeadingFontWeight( headingfontweight );
                                table->SetHeadingFontItalic( headingfontitalic );
                                table->SetHeadingFontUnderline( headingfontunderline );
                                table->SetHeadingFontCharacterSet( headingfontset );
                                table->SetHeadingFontPixelSize( headingfontptc );
                                table->SetHeadingFontName( m_pimpl->m_myconv.from_bytes( headingfontname ) );
                                table->SetLabelFontSize( labelfontheight );
                                table->SetLabelFontWeight( labelfontweight );
                                table->SetLabelFontItalic( labelfontitalic );
                                table->SetLabelFontUnderline( labelfontunderline );
                                table->SetLabelFontCharacterSet( labelfontset );
                                table->SetLabelFontPixelSize( labelfontptc );
                                table->SetLabelFontName( m_pimpl->m_myconv.from_bytes( labelfontname ) );
                                table->SetComment( m_pimpl->m_myconv.from_bytes( comments ) );
                            }
                        }
                    }
                }
            }
        }
    }
    if( stmt )
    {
        if( mysql_stmt_close( stmt ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( stmt ) );
            errorMsg.push_back( err );
            result = 1;
        }
    }
    delete str_data1;
    delete str_data2;
    str_data1 = NULL;
    str_data2 = NULL;
    return result;
}

int MySQLDatabase::SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    bool exist;
    std::wstring query = L"BEGIN TRANSACTION";
    std::wostringstream istr;
    int res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() );
    if( res )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    else
    {
        std::wstring tableName = const_cast<DatabaseTable *>( table )->GetTableName();
        std::wstring schemaName = const_cast<DatabaseTable *>( table )->GetSchemaName();
        std::wstring comment = const_cast<DatabaseTable *>( table )->GetComment();
        unsigned long tableId = const_cast<DatabaseTable *>( table )->GetTableId();
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
                command += pimpl->m_connectedUser;
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
                istr << properties.m_dataFontPixelSize;
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
                istr << properties.m_labelFontPixelSize;
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
                command += pimpl->m_connectedUser;
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
                istr << properties.m_labelFontPixelSize;
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
                res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() );
                if( res )
                {
                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                    errorMsg.push_back( err );
                    result = 1;
                }
            }
        }
        if( result == 1 )
            query = L"ROLLBACK";
        else
            query = L"COMMIT";
        res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() );
        if( res )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            result = 1;
        }
    }
    return result;
}

bool MySQLDatabase::IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    bool result = false;
    char *str_data1 = NULL, *str_data2 = NULL;
    MYSQL_STMT *stmt = NULL;
    std::wstring tname = const_cast<DatabaseTable *>( table )->GetSchemaName() + L".";
    tname += const_cast<DatabaseTable *>( table )->GetTableName();
    std::wstring query = L"SELECT 1 FROM abcattbl WHERE abt_tnam = ? AND abt_ownr = ?;";
    stmt = mysql_stmt_init( m_db );
    if( !stmt )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
    }
    else
    {
        if( mysql_stmt_prepare( stmt, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), query.length() ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
        }
        else
        {
            MYSQL_BIND params[2];
            unsigned long str_length1, str_length2;
            str_data1 = new char[tname.length()], str_data2 = new char[pimpl->m_connectedUser.length()];
            memset( params, 0, sizeof( params ) );
            params[0].buffer_type = MYSQL_TYPE_STRING;
            params[0].buffer = (char *) str_data1;
            params[0].buffer_length = tname.length();
            params[0].is_null = 0;
            params[0].length = &str_length1;
            params[1].buffer_type = MYSQL_TYPE_STRING;
            params[1].buffer = (char *) str_data2;
            params[1].buffer_length = pimpl->m_connectedUser.length();
            params[1].is_null = 0;
            params[1].length = &str_length2;
            if( mysql_stmt_bind_param( stmt, params ) )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( err );
            }
            else
            {
                if( mysql_stmt_execute( stmt ) )
                {
                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                    errorMsg.push_back( err );
                }
                else
                {
                    if( !( mysql_store_result( m_db ) ) )
                    {
                        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                        errorMsg.push_back( err );
                    }
                    else
                    {
                        if( mysql_stmt_num_rows( stmt ) == 1 )
                            result = true;
                    }
                }
            }
        }
    }
    if( stmt )
    {
        if( mysql_stmt_close( stmt ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
        }
    }
    delete str_data1;
    str_data1 = NULL;
    delete str_data2;
    str_data2 = NULL;
    return result;
}

int MySQLDatabase::GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &ownerName, const std::wstring &fieldName, Field *field, std::vector<std::wstring> &errorMsg)
{
    char /**str_data1 = NULL, *str_data2 = NULL, */*str_data3 = NULL;
    int result = 0;
    const char *table = m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str();
    const char *schema = m_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str();
    const char *owner = m_pimpl->m_myconv.to_bytes( ownerName.c_str() ).c_str();
    const char *fieldNameReq = m_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str();
    size_t len = strlen( table ) + strlen( schema ) + 2;
    char *tname = new char[len];
    memset( tname, '\0', len );
    strcpy( tname, schema );
    strcat( tname, "." );
    strcat( tname, table );
    std::wstring query = L"SELECT * FROM abcatcol WHERE abc_tnam = ? AND abc_ownr = ? AND abc_cnam = ?";
    MYSQL_STMT *stmt = mysql_stmt_init( m_db );
    if( !stmt )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    else 
    {
        if( mysql_stmt_prepare( stmt, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), query.length() ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            result = 1;
        }
        else
        {
            MYSQL_BIND params[3];
            unsigned long str_length1, str_length2, str_length3;
            params[0].buffer_type = MYSQL_TYPE_STRING;
            params[0].buffer = (char *) tname;
            params[0].buffer_length = strlen( tname );
            params[0].is_null = 0;
            params[0].length = &str_length1;
            params[1].buffer_type = MYSQL_TYPE_STRING;
            params[1].buffer = (char *) owner;
            params[1].buffer_length = strlen( owner );
            params[1].is_null = 0;
            params[1].length = &str_length2;
            params[2].buffer_type = MYSQL_TYPE_STRING;
            params[2].buffer = (char *) fieldNameReq;
            params[2].buffer_length = strlen( fieldNameReq );
            params[2].is_null = 0;
            params[2].length = &str_length3;
            if( mysql_stmt_bind_param( stmt, params ) )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( err );
                result = 1;
            }
            else
            {
                if( mysql_stmt_execute( stmt ) )
                {
                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                    errorMsg.push_back( err );
                    result = 1;
                }
                else
                {
                    if( !( mysql_store_result( m_db ) ) )
                    {
                        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                        errorMsg.push_back( err );
                        result = 1;
                    }
                    else
                    {
                        MYSQL_BIND results[17];
                        int tableId, fieldId;
                        my_bool is_null[17], error[17];
                        unsigned long length[17];
                        char *label, *comment;
                        results[0].buffer_type = MYSQL_TYPE_STRING;
                        results[0].buffer = &table;
                        results[0].buffer_length = 129;
                        results[0].is_null = &is_null[0];
                        results[0].length = &length[0];
                        results[0].error = &error[0];
                        results[1].buffer_type = MYSQL_TYPE_LONG;
                        results[1].buffer = (char *)&tableId;
                        results[1].is_null = &is_null[1];
                        results[1].length = &length[1];
                        results[1].error = &error[1];
                        results[2].buffer_type = MYSQL_TYPE_STRING;
                        results[2].buffer = &owner;
                        results[2].buffer_length = 129;
                        results[2].is_null = &is_null[2];
                        results[2].length = &length[2];
                        results[2].error = &error[2];
                        results[3].buffer_type = MYSQL_TYPE_STRING;
                        results[3].buffer = &fieldNameReq;
                        results[3].buffer_length = 129;
                        results[3].is_null = &is_null[3];
                        results[3].length = &length[3];
                        results[3].error = &error[3];
                        results[4].buffer_type = MYSQL_TYPE_LONG;
                        results[4].buffer = (char *)&fieldId;
                        results[4].is_null = &is_null[4];
                        results[4].length = &length[4];
                        results[4].error = &error[4];
                        results[5].buffer_type = MYSQL_TYPE_STRING;
                        results[5].buffer = &label;
                        results[5].buffer_length = 254;
                        results[5].is_null = &is_null[5];
                        results[5].length = &length[5];
                        results[5].error = &error[5];
                        results[17].buffer_type = MYSQL_TYPE_STRING;
                        results[17].buffer = &comment;
                        results[17].buffer_length = 256;
                        results[17].is_null = &is_null[17];
                        results[17].length = &length[17];
                        results[17].error = &error[17];
                        if( mysql_stmt_bind_result (stmt, results) )
                        {
                            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                            errorMsg.push_back( err );
                            result = 1;
                        }
                        else
                        {
                            if( mysql_stmt_fetch (stmt) )
                            {
                                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                                errorMsg.push_back( err );
                                result = 1;
                            }
                            else
                            {
                                field->SetComment( m_pimpl->m_myconv.from_bytes( comment ) );
                            }
                        }
                    }
                }
            }
        }
        if( stmt )
        {
            if( mysql_stmt_close( stmt ) )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( err );
                result = 1;
            }
        }
    }
    delete str_data3;
    str_data3 = NULL;
    return result;
}

int MySQLDatabase::GetFieldProperties(const std::wstring &table, Field *field, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    bool found = false;
    std::wstring schemaName, ownerName;
    for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it = pimpl->m_tables.begin(); it != pimpl->m_tables.end(); ++it )
    {
        if( ( *it ).first == pimpl->m_dbName )
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

int MySQLDatabase::ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &foreignKeyFields, const std::wstring &refTableName, const std::vector<std::wstring> &refKeyFields, int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int match, std::vector<std::wstring> &errorMsg)
{
    match = match;
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
    if( !isNew )
        result = DropForeignKey( command, tableName, keyName, logOnly, errorMsg );
    if( !logOnly && !result )
    {
        if( mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( L"Applying foreign key failed: " + err );
            result = 1;
        }
        else
        {
            std::map<int, std::vector<FKField *> > &fKeys = tableName.GetForeignKeyVector();
            size_t size = fKeys.size();
            size++;
            for( unsigned int i = 0; i < foreignKeyFields.size(); i++ )
                fKeys[size].push_back( new FKField( i, keyName, L"", tableName.GetTableName(), foreignKeyFields.at( i ), L"", refTableName, refKeyFields.at( i ), origFields, refFields, updProp, delProp ) );
            newFK = fKeys[size];
        }
    }
    else
        command = query;
    return result;
}

int MySQLDatabase::DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    std::wstring query = L"DROP TABLE ";
    query += tableName;
    int res = 0;
    if( mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        res = 1;
    }
    return res;
}

int MySQLDatabase::SetFieldProperties(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, const Field *field, bool isLogOnly, std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    bool exist = IsFieldPropertiesExist( tableName, ownerName, fieldName, errorMsg );
    if( exist )
    {
        command = L"INSERT INTO abcatcol(abc_tnam, abc_ownr, abc_cnam, abc_labl, abc_hdr, abc_cmnt) VALUES(";
        command += tableName;
        command += L", " + ownerName;
        command += L", " + fieldName;
        command += L", " + const_cast<Field *>( field )->GetLabel();
        command += L", " + const_cast<Field *>( field )->GetHeading();
        command += L", " + const_cast<Field *>( field )->GetComment() + L");";
    }
    else
    {
        command = L"UPDATE abcatcol SET abc_labl = ";
        command += const_cast<Field *>( field )->GetLabel() + L", abc_hdr = ";
        command += const_cast<Field *>( field )->GetHeading() + L", abc_cmnt = ";
        command += const_cast<Field *>( field )->GetComment() + L"WHERE abc_tnam = ";
        command += tableName + L" AND abc_ownr = ";
        command += ownerName + L" AND abc_cnam = ";
        command += fieldName + L";";
    }
    if( !isLogOnly )
    {
        if( mysql_query( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            res = 1;
        }
    }
    return res;
}

int MySQLDatabase::TokenizeConnectionString(const std::wstring &connectStr, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring tmp = connectStr;
    while( !tmp.empty() )
    {
        std::wstring temp = tmp.substr( 0, tmp.find( ' ' ) );
        std::wstring temp1 = temp.substr( 0, temp.find( '=' ) );
        std::wstring temp2 = temp.substr( temp.find( '=' ) + 1 );
        if( temp1 == L"host" )
            m_pimpl->m_host = temp2;
        else if( temp1 == L"user" )
            m_pimpl->m_user = temp2;
        else if( temp1 == L"password" )
            m_pimpl->m_password = temp2;
        else if( temp1 == L"port" )
            m_port = std::stoi( temp2 );
        else if( temp1 == L"dbname" )
        {
            m_pimpl->m_dbName = temp2;
            connectToDatabase = true;
        }
        else if( temp1 == L"socket" )
            m_pimpl->m_socket = temp2;
        else if( temp1 == L"flags" )
        {
            int flags = std::stoi( temp2 );
            if( flags & 1 )
                m_flags |= CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS;
            if( flags & 2 )
                m_flags |= CLIENT_COMPRESS;
            if( flags & 4 )
                m_flags |= CLIENT_FOUND_ROWS;
            if( flags & 8 )
                m_flags |= CLIENT_IGNORE_SIGPIPE;
            if( flags & 16 )
                m_flags |= CLIENT_IGNORE_SPACE;
            if( flags & 32 )
                m_flags |= CLIENT_INTERACTIVE;
            if( flags & 64 )
                m_flags |= CLIENT_LOCAL_FILES;
            if( flags & 128 )
                m_flags |= CLIENT_MULTI_RESULTS;
            if( flags & 256 )
                m_flags |= CLIENT_MULTI_STATEMENTS;
            if( flags & 512 )
                m_flags |= CLIENT_NO_SCHEMA;
            if( flags & 1024 )
                m_flags |= CLIENT_ODBC;
            if( flags & 2048 )
                m_flags |= CLIENT_SSL;
            if( flags & 4096 )
                m_flags |= CLIENT_REMEMBER_OPTIONS;
        }
        else
        {
            mysql_option option = MYSQL_DEFAULT_AUTH;
            if( temp1 == L"MYSQL_DEFAULT_AUTH" )
                option = MYSQL_DEFAULT_AUTH;
            if( temp1 == L"MYSQL_ENABLE_CLEARTEXT_PLUGIN" )
                option = MYSQL_ENABLE_CLEARTEXT_PLUGIN;
            if( temp1 == L"MYSQL_INIT_COMMAND" )
                option = MYSQL_INIT_COMMAND;
            if( temp1 == L"MYSQL_OPT_BIND" )
                option = MYSQL_OPT_BIND;
            if( temp1 == L"MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS" )
                option = MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS;
            if( temp1 == L"MYSQL_OPT_COMPRESS" )
                option = MYSQL_OPT_COMPRESS;
            if( temp1 == L"MYSQL_OPT_CONNECT_ATTR_DELETE" )
                option = MYSQL_OPT_CONNECT_ATTR_DELETE;
            if( option >= MYSQL_OPT_CONNECT_TIMEOUT && option <= MYSQL_OPT_SSL_MODE )
            {
                int res = mysql_options( m_db, option, m_pimpl->m_myconv.to_bytes( temp2.c_str() ).c_str() );
                if( res )
                {
                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                    errorMsg.push_back( err );
                    result = 1;
                    break;
                }
            }
        }
        tmp = tmp.substr( tmp.find( ' ' ) + 1 );
    }
    return result;
}

int MySQLDatabase::GetServerVersion(std::vector<std::wstring> &UNUSED(errorMsg))
{
    unsigned long version;
    int result = 0;
    version = mysql_get_server_version( m_db );
    pimpl->m_versionMajor = version / 10000;
    pimpl->m_versionRevision = ( version - pimpl->m_versionMajor ) / 100;
    return result;
}

int MySQLDatabase::DropForeignKey(std::wstring &command, const DatabaseTable &tableName, const std::wstring &keyName, bool logOnly, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query, err;
    query = L"ALTER TABLE ";
    query += tableName.GetSchemaName() + L"." + tableName.GetTableName() +L" ";
    query += L"DROP FOREIGN KEY " + keyName;
    if( !logOnly )
    {
        if( mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str() ) )
        {
            err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            result = 1;
        }
        else
        {
            bool found = false;
            std::map<int, std::vector<FKField *> > &fKeys = const_cast<DatabaseTable &>( tableName ).GetForeignKeyVector();
            for( std::map<int, std::vector<FKField *> >::iterator it = fKeys.begin(); it != fKeys.end() && !found; ++it )
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
                }
        }
    }
    else
        command = query;
    return result;
}

int MySQLDatabase::NewTableCreation(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

int MySQLDatabase::AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, const std::wstring &tableOwner, int tableId, bool tableAdded, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

bool MySQLDatabase::IsFieldPropertiesExist (const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, std::vector<std::wstring> &errorMsg)
{
    bool exist = false;
    std::wstring query = L"SELECT 1 FROM abcatcol WHERE abc_tnam = ? AND abc_ownr = ? AND abc_cnam = ?;";
    MYSQL_BIND bind[3];
    unsigned long str_len1, str_len2, str_len3;
    MYSQL_STMT *res = mysql_stmt_init( m_db );
    if( res )
    {
        if( !mysql_stmt_prepare( res, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str (), query.length () ) )
        {
            bind[0].buffer_type = MYSQL_TYPE_STRING;
            bind[0].buffer = (char *) m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str();
            bind[0].buffer_length = strlen( m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str() );
            bind[0].is_null = 0;
            bind[0].length = &str_len1;
            bind[1].buffer_type = MYSQL_TYPE_STRING;
            bind[1].buffer = (char *) m_pimpl->m_myconv.to_bytes( ownerName.c_str() ).c_str();
            bind[1].buffer_length = strlen( m_pimpl->m_myconv.to_bytes( ownerName.c_str() ).c_str() );
            bind[1].is_null = 0;
            bind[1].length = &str_len2;
            bind[2].buffer_type = MYSQL_TYPE_STRING;
            bind[2].buffer = (char *) m_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str();
            bind[2].buffer_length = strlen( m_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str() );
            bind[2].is_null = 0;
            bind[2].length = &str_len3;
            if( !mysql_stmt_bind_param( res, bind ) )
            {
                str_len1 = bind[0].buffer_length;
                str_len2 = bind[1].buffer_length;
                str_len3 = bind[2].buffer_length;
                if( !mysql_stmt_execute( res ) )
                {
                    if( mysql_stmt_affected_rows( res ) )
                        exist = true;
                }
                else
                {
                    errorMsg.push_back( m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) ) );
                }
            }
            else
            {
                errorMsg.push_back( m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) ) );
            }
        }
        else
        {
            errorMsg.push_back( m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) ) );
        }
    }
    else
    {
        errorMsg.push_back( m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) ) );
    }
    return exist;
}
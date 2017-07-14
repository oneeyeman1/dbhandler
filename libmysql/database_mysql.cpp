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
}

MySQLDatabase::~MySQLDatabase()
{
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

int MySQLDatabase::Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg)
{
    int result = 0, res;
    std::wstring err;
    std::wstring query1 = L"CREATE TABLE IF NOT EXISTS abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));";
    std::wstring query2 = L"CREATE TABLE IF NOT EXISTS abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));";
    std::wstring query3 = L"CREATE TABLE IF NOT EXISTS abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));";
    std::wstring query4 = L"CREATE TABLE IF NOT EXISTS abcattbl(abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ));";
    std::wstring query5 = L"CREATE TABLE IF NOT EXISTS abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));";
    std::wstring errorMessage;
    m_db = mysql_init( m_db );
    if( !m_db )
    {
        err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( L"Connection to database failed: " + err );
        result = 1;
    }
    else
    {
        if( TokenizeConnectionString( selectedDSN, errorMsg ) )
        {
            err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( L"Connection to database failed: " + err );
            result = 1;
        }
        m_db = mysql_real_connect( m_db, m_pimpl->m_myconv.to_bytes( m_pimpl->m_host.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_pimpl->m_user.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_pimpl->m_password.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_pimpl->m_dbName.c_str() ).c_str(), m_port, m_pimpl->m_socket == L"" ? NULL : m_pimpl->m_myconv.to_bytes( m_pimpl->m_socket.c_str() ).c_str(), m_flags );
        if( !m_db )
        {
            err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( L"Connection to database failed: " + err );
            result = 1;
        }
        else
        {
            res = mysql_query( m_db, "START TRANSACTION" );
            if( res )
            {
                err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( L"Starting transaction failed during connection: " + err );
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
                                res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query5.c_str() ).c_str() );
                                if( !res )
                                {
                                    res = mysql_commit( m_db );
                                }
                            }
                        }
                    }
                }
            }
            if( res )
            {
                err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( L"Error during database connection: " + err );
                res = mysql_rollback( m_db );
                result = 1;
            }
            else
            {
                result = GetTableListFromDb( errorMsg );
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
    delete m_pimpl;
    m_pimpl = NULL;
    return result;
}

int MySQLDatabase::GetTableListFromDb(std::vector<std::wstring> &errorMsg)
{
    int res;
    MYSQL_STMT *res1, *res2;
    char *str_data1 = NULL, *str_data2 = NULL, *str_data3 = NULL;
    char fkField[64], refTableSchema[64], refTableName[64], refTableField[64], updateCon[64], deleteCon[64];
    char colName[64], colType[64], defValue[64], nullable[3], autoInc[30];
    MYSQL_RES *prepare_meta_result;
    std::vector<Field *> fields;
    std::vector<std::wstring> fk_names;
    std::map<int,std::vector<FKField *> > foreign_keys;
    std::wstring errorMessage;
    std::wstring fieldName, fieldType, fieldDefaultValue, fkSchema, fkTable, fkFld, fkTableField, fkUpdateConstraint, fkDeleteConstraint;
    int fkId, charLen, charOctet, numLen, numPrec, pk;
    bool is_nullable, autoincrement, is_pk;
    FK_ONUPDATE update_constraint = NO_ACTION_UPDATE;
    FK_ONDELETE delete_constraint = NO_ACTION_DELETE;
    std::wstring query1 = L"SELECT table_catalog, table_schema, table_name FROM information_schema.tables WHERE table_type = 'BASE TABLE' OR table_type = 'VIEW';";
    std::wstring query2 = L"SELECT cols.column_name, cols.data_type, cols.character_maximum_length, cols.character_octet_length, cols.numeric_precision, cols.numeric_scale, cols.column_default, cols.is_nullable, cols.extra, (CASE WHEN kcu.column_name = cols.column_name THEN 1 ELSE 0 END) as pk_flag FROM information_schema.columns cols, information_schema.key_column_usage kcu WHERE kcu.constraint_name = 'PRIMARY' AND kcu.table_schema = cols.table_schema AND kcu.table_name = cols.table_name AND cols.table_catalog = ? AND cols.table_schema = ? AND cols.table_name = ?;";
    std::wstring query3 = L"SELECT kcu.column_name, kcu.ordinal_position, kcu.referenced_table_schema, kcu.referenced_table_name, kcu.referenced_column_name, rc.update_rule, rc.delete_rule FROM information_schema.key_column_usage kcu, information_schema.referential_constraints rc WHERE kcu.constraint_name = rc.constraint_name AND kcu.table_catalog = ? AND kcu.table_schema = ? AND kcu.table_name = ?;";
    res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str() );
    if( res )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
    MYSQL_RES *results = mysql_store_result( m_db );
    if( !results )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
    MYSQL_ROW row;
    while( ( row = mysql_fetch_row( results ) ) != NULL )
    {
        char *catalog_name = row[0] ? row[0] : NULL;
        char *schema_name = row[1] ? row[1] : NULL;
        char *table_name = row[2] ? row[2] : NULL;
        res1 = mysql_stmt_init( m_db );
        if( !res1 )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
            errorMsg.push_back( err );
            return 1;
        }
        if( mysql_stmt_prepare( res1, m_pimpl->m_myconv.to_bytes( query3.c_str() ).c_str(), query3.length() ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
            errorMsg.push_back( err );
            return 1;
        }
        MYSQL_BIND params[3];
        unsigned long str_length1, str_length2, str_length3;
        str_data1 = new char[strlen( catalog_name )], str_data2 = new char[strlen( schema_name )], str_data3 = new char[strlen( table_name )];
        memset( params, 0, sizeof( params ) );
        str_length1 = strlen( catalog_name );
        str_length2 = strlen( schema_name );
        str_length3 = strlen( table_name );
        strncpy( str_data1, catalog_name, strlen( catalog_name ) );
        strncpy( str_data2, schema_name, strlen( schema_name ) );
        strncpy( str_data3, table_name, strlen( table_name ) );
        params[0].buffer_type = MYSQL_TYPE_STRING;
        params[0].buffer = (char *) str_data1;
        params[0].buffer_length = strlen( catalog_name );
        params[0].is_null = 0;
        params[0].length = &str_length1;
        params[1].buffer_type = MYSQL_TYPE_STRING;
        params[1].buffer = (char *) str_data2;
        params[1].buffer_length = strlen( schema_name );
        params[1].is_null = 0;
        params[1].length = &str_length2;
        params[2].buffer_type = MYSQL_TYPE_STRING;
        params[2].buffer = (char *) str_data3;
        params[2].buffer_length = strlen( table_name );
        params[2].is_null = 0;
        params[2].length = &str_length3;
        if( mysql_stmt_bind_param( res1, params ) )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
            errorMsg.push_back( err );
            return 1;
        }
        prepare_meta_result = mysql_stmt_result_metadata( res1 );
        if( !prepare_meta_result )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
            errorMsg.push_back( err );
            return 1;
        }
        if( mysql_stmt_execute( res1 ) )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
            errorMsg.push_back( err );
            return 1;
        }
        MYSQL_BIND results[7];
        my_bool is_null[7], error[7];
        unsigned long length[7];
        memset( results, 0, sizeof( results ) );
        results[0].buffer_type = results[2].buffer_type = results[3].buffer_type = results[4].buffer_type = results[5].buffer_type = results[6].buffer_type = MYSQL_TYPE_STRING;
        results[1].buffer_type = MYSQL_TYPE_LONG;

        results[1].buffer = (char *) &fkId;
        results[1].is_null = &is_null[1];
        results[1].error = &error[1];
        results[1].length = &length[1];

        length[0] = 64;
        results[0].buffer = fkField;
        results[0].buffer_length = length[0];
        results[0].is_null = &is_null[0];
        results[0].length = &length[0];
        results[0].error = &error[1];

        length[2] = 64;
        results[2].buffer = refTableSchema;
        results[2].buffer_length = length[2];
        results[2].is_null = &is_null[2];
        results[2].length = &length[2];
        results[2].error = &error[2];

        length[3] = 64;
        results[3].buffer = refTableName;
        results[3].buffer_length = length[3];
        results[3].is_null = &is_null[3];
        results[3].length = &length[3];
        results[3].error = &error[3];

        length[4] = 64;
        results[4].buffer = refTableField;
        results[4].buffer_length = length[4];
        results[4].is_null = &is_null[4];
        results[4].length = &length[4];
        results[4].error = &error[4];

        length[5] = 64;
        results[5].buffer = updateCon;
        results[5].buffer_length = length[5];
        results[5].is_null = &is_null[5];
        results[5].length = &length[5];
        results[5].error = &error[5];

        length[6] = 64;
        results[6].buffer = deleteCon;
        results[6].buffer_length = length[6];
        results[6].is_null = &is_null[6];
        results[6].length = &length[6];
        results[6].error = &error[6];

        if( mysql_stmt_bind_result( res1, results ) )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
            errorMsg.push_back( err );
            return 1;
        }
        while( !mysql_stmt_fetch( res1 ) )
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
//            foreign_keys[].push_back( new FKField( , fkTable, fkFld, fkTableField, fkSchema, update_constraint, delete_constraint ) );
            fk_names.push_back( fkFld );
        }
        mysql_free_result( prepare_meta_result );
        if( mysql_stmt_close( res1 ) )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            return 1;
        }
        res2 = mysql_stmt_init( m_db );
        if( !res2 )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
            errorMsg.push_back( err );
            return 1;
        }
        if( mysql_stmt_prepare( res2, m_pimpl->m_myconv.to_bytes( query2.c_str() ).c_str(), query2.length() ) )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
            errorMsg.push_back( err );
            return 1;
        }
        if( mysql_stmt_bind_param( res2, params ) )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
            errorMsg.push_back( err );
            return 1;
        }
        prepare_meta_result = mysql_stmt_result_metadata( res2 );
        if( !prepare_meta_result )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
            errorMsg.push_back( err );
            return 1;
        }
        if( mysql_stmt_execute( res2 ) )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
            errorMsg.push_back( err );
            return 1;
        }
        MYSQL_BIND results1[10];
        my_bool is_null1[10], error1[10];
        long unsigned int length1[10];
        
        length1[0] = 64;
        results1[0].buffer = colName;
        results1[0].buffer_type = MYSQL_TYPE_STRING;
        results1[0].buffer_length = length1[0];
        results1[0].is_null = &is_null1[0];
        results1[0].length = &length1[0];
        results1[0].error = &error1[0];

        length1[1] = 64;
        results1[1].buffer = colType;
        results1[1].buffer_type = MYSQL_TYPE_STRING;
        results1[1].buffer_length = length1[1];
        results1[1].is_null = &is_null1[1];
        results1[1].length = &length1[1];
        results1[1].error = &error1[1];

        results1[2].buffer = (char *) &charLen;
        results1[2].buffer_type = MYSQL_TYPE_LONG;
        results1[2].is_null = &is_null1[2];
        results1[2].length = &length1[2];
        results1[2].error = &error1[2];

        results1[3].buffer = (char *) &charOctet;
        results1[3].buffer_type = MYSQL_TYPE_LONG;
        results1[3].is_null = &is_null1[3];
        results1[3].length = &length1[3];
        results1[3].error = &error1[3];

        results1[4].buffer = (char *) &numLen;
        results1[4].buffer_type = MYSQL_TYPE_LONG;
        results1[4].is_null = &is_null1[4];
        results1[4].length = &length1[4];
        results1[4].error = &error1[4];

        results1[5].buffer = (char *) &numPrec;
        results1[5].buffer_type = MYSQL_TYPE_LONG;
        results1[5].is_null = &is_null1[5];
        results1[5].length = &length1[5];
        results1[5].error = &error1[5];

        length1[6] = 64;
        results1[6].buffer = defValue;
        results1[6].buffer_type = MYSQL_TYPE_STRING;
        results1[6].buffer_length = length1[6];
        results1[6].is_null = &is_null1[6];
        results1[6].length = &length1[6];
        results1[6].error = &error[6];

        length1[7] = 3;
        results1[7].buffer = nullable;
        results1[7].buffer_type = MYSQL_TYPE_STRING;
        results1[7].buffer_length = length1[7];
        results1[7].is_null = &is_null1[7];
        results1[7].length = &length1[7];
        results1[7].error = &error[7];

        length1[8] = 30;
        results1[8].buffer = autoInc;
        results1[8].buffer_type = MYSQL_TYPE_STRING;
        results1[8].buffer_length = length1[8];
        results1[8].is_null = &is_null1[8];
        results1[8].length = &length1[8];
        results1[8].error = &error1[8];

        results1[9].buffer = (char *) &pk;
        results1[9].buffer_type = MYSQL_TYPE_LONG;
        results1[9].is_null = &is_null1[9];
        results1[9].length = &length1[9];
        results1[9].error = &error1[9];

        if( mysql_stmt_bind_result( res2, results1 ) )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
            errorMsg.push_back( err );
            return 1;
        }
        if( mysql_stmt_store_result( res2 ) )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
            errorMsg.push_back( err );
            return 1;
        }
        int fieldSize, fieldPrec;
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
            if( !strcmp( nullable, "YES" ) )
                is_nullable = true;
            else
                is_nullable = false;
            if( !strcmp( autoInc, "autoincrement" ) )
                autoincrement = true;
            else
                autoincrement = false;
            is_pk = pk == 1 ? true : false;
//            Field *field = new Field( fieldName, fieldType, fieldSize, fieldPrec, fieldDefaultValue, );
        }
        mysql_free_result( prepare_meta_result );
        if( mysql_stmt_close( res2 ) )
        {
            delete str_data1;
            str_data1 = NULL;
            delete str_data2;
            str_data2 = NULL;
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            return 1;
        }
        delete str_data1;
        str_data1 = NULL;
        delete str_data2;
        str_data2 = NULL;
    }
    if( !row )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
/*    DatabaseTable *table = new DatabaseTable( m_pimpl->m_myconv.from_bytes( table_name ), m_pimpl->m_myconv.from_bytes( schema_name ), fields, foreign_keys );
                if( GetTableProperties( table, errorMsg ) )
                {
                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                    errorMsg.push_back( err );
                    return 1;
                }
                pimpl->m_tables[m_pimpl->m_myconv.from_bytes( catalog_name )].push_back( table );
                fields.erase( fields.begin(), fields.end() );
                foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                fk_names.clear();*/
    delete str_data1;
    str_data1 = NULL;
    delete str_data2;
    str_data2 = NULL;
    return 0;
}

int MySQLDatabase::CreateIndex(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    int res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() );
    if( res )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    return result;
}

int MySQLDatabase::SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg)
{
    char *str_data1, *str_data2, *str_data3, *str_data4;
    MYSQL_RES *prepare_meta_result;
    int result = 0;
    std::wstring query1 = L"SELECT 1 FROM abcatcol WHERE abc_tnam = ? AND abc_cnam = ?;", query2;
    MYSQL_STMT *res1 = mysql_stmt_init( m_db );
    if( !res1 )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
        errorMsg.push_back( err );
        return 1;
    }
    if( mysql_stmt_prepare( res1, m_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str(), query1.length() ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
        errorMsg.push_back( err );
        return 1;
    }
    MYSQL_BIND params[2];
    unsigned long str_length1, str_length2, str_length3, str_length4;
    str_data1 = new char[tableName.length()], str_data2 = new char[fieldName.length()];
    memset( params, 0, sizeof( params ) );
    str_length1 = tableName.length();
    str_length2 = fieldName.length();
    strncpy( str_data1, m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), tableName.length() );
    strncpy( str_data2, m_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str(), fieldName.length() );
    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char *) str_data1;
    params[0].buffer_length = tableName.length();
    params[0].is_null = 0;
    params[0].length = &str_length1;
    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (char *) str_data2;
    params[1].buffer_length = fieldName.length();
    params[1].is_null = 0;
    params[1].length = &str_length2;
    if( mysql_stmt_bind_param( res1, params ) )
    {
        delete str_data1;
        str_data1 = NULL;
        delete str_data2;
        str_data2 = NULL;
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
        errorMsg.push_back( err );
        return 1;
    }
    prepare_meta_result = mysql_stmt_result_metadata( res1 );
    if( !prepare_meta_result )
    {
        delete str_data1;
        str_data1 = NULL;
        delete str_data2;
        str_data2 = NULL;
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
        errorMsg.push_back( err );
        return 1;
    }
    if( mysql_stmt_execute( res1 ) )
    {
        delete str_data1;
        str_data1 = NULL;
        delete str_data2;
        str_data2 = NULL;
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
        errorMsg.push_back( err );
        return 1;
    }
    if( mysql_stmt_store_result( res1 ) )
    {
        delete str_data1;
        str_data1 = NULL;
        delete str_data2;
        str_data2 = NULL;
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res1 ) );
        errorMsg.push_back( err );
        return 1;
    }
    if( mysql_stmt_num_rows( res1 ) == 1 )
        query2 = L"UPDATE abcatcol SET abc_cmnt = ? WHERE abc_tnam = ? AND abc_ownr == ? AND abc_cnam = ?;";
    else
        query2 = L"INSERT INTO abcattbl(abc_cmnt, abc_tnam, abc_ownr, abc_cnam ) VALUES( ?, ?, ?, ? );";
    mysql_free_result( prepare_meta_result );
    if( mysql_stmt_close( res1 ) )
    {
        delete str_data1;
        str_data1 = NULL;
        delete str_data2;
        str_data2 = NULL;
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
    delete str_data1;
    str_data1 = NULL;
    delete str_data2;
    str_data2 = NULL;
    MYSQL_STMT *res2 = mysql_stmt_init( m_db );
    if( !res2 )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
        errorMsg.push_back( err );
        return 1;
    }
    if( mysql_stmt_prepare( res2, m_pimpl->m_myconv.to_bytes( query2.c_str() ).c_str(), query2.length() ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
        errorMsg.push_back( err );
        return 1;
    }
    MYSQL_BIND params1[4];
    str_data1 = new char[comment.length()], str_data2 = new char[tableName.length()], str_data3 = new char[pimpl->m_connectedUser.length()], str_data4 = new char[fieldName.length()];
    memset( params, 0, sizeof( params ) );
    str_length1 = comment.length();
    str_length2 = tableName.length();
    str_length3 = pimpl->m_connectedUser.length();
    str_length4 = fieldName.length();
    strncpy( str_data1, m_pimpl->m_myconv.to_bytes( comment.c_str() ).c_str(), comment.length() );
    strncpy( str_data2, m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), tableName.length() );
    strncpy( str_data3, m_pimpl->m_myconv.to_bytes( pimpl->m_connectedUser.c_str() ).c_str(), pimpl->m_connectedUser.length() );
    strncpy( str_data4, m_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str(), fieldName.length() );
    params1[0].buffer_type = MYSQL_TYPE_STRING;
    params1[0].buffer = (char *) str_data1;
    params1[0].buffer_length = comment.length();
    params1[0].is_null = 0;
    params1[0].length = &str_length1;
    params1[1].buffer_type = MYSQL_TYPE_STRING;
    params1[1].buffer = (char *) str_data2;
    params1[1].buffer_length = tableName.length();
    params1[1].is_null = 0;
    params1[1].length = &str_length2;
    params1[3].buffer_type = MYSQL_TYPE_STRING;
    params1[3].buffer = (char *) str_data3;
    params1[3].buffer_length = pimpl->m_connectedUser.length();
    params1[3].is_null = 0;
    params1[3].length = &str_length3;
    params1[4].buffer_type = MYSQL_TYPE_STRING;
    params1[4].buffer = (char *) str_data4;
    params1[4].buffer_length = fieldName.length();
    params1[4].is_null = 0;
    params1[4].length = &str_length4;
    if( mysql_stmt_bind_param( res2, params1 ) )
    {
        delete str_data1;
        str_data1 = NULL;
        delete str_data2;
        str_data2 = NULL;
        delete str_data3;
        str_data3 = NULL;
        delete str_data4;
        str_data4 = NULL;
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
        errorMsg.push_back( err );
        return 1;
    }
    if( mysql_stmt_execute( res2 ) )
    {
        delete str_data1;
        str_data1 = NULL;
        delete str_data2;
        str_data2 = NULL;
        delete str_data3;
        str_data3 = NULL;
        delete str_data4;
        str_data4 = NULL;
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_stmt_error( res2 ) );
        errorMsg.push_back( err );
        return 1;
    }
    delete str_data1;
    str_data1 = NULL;
    delete str_data2;
    str_data2 = NULL;
    delete str_data3;
    str_data3 = NULL;
    delete str_data4;
    str_data4 = NULL;
    return result;
}

bool MySQLDatabase::IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    MYSQL_STMT *res;
    bool exists = false;
    char *str_data[3];
    unsigned long *str_length[3];
    std::wstring query = L"SELECT count(*) FROM information_schema.statistics WHERE table_schema = $1 AND table_name = $2 AND index_name = $3;";
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
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
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
            if( mysql_stmt_bind_param( res, values ) )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( err );
            }
            else
            {
                strncpy( str_data[0], m_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str(), schemaName.length() );
                strncpy( str_data[1], m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), tableName.length() );
                strncpy( str_data[2], m_pimpl->m_myconv.to_bytes( indexName.c_str() ).c_str(), indexName.length() );
                *str_length[0] = schemaName.length();
                *str_length[1] = tableName.length();
                *str_length[2] = indexName.length();
                if( mysql_stmt_execute( res ) )
                {
                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                    errorMsg.push_back( err );
                }
                else
                {
                    MYSQL_RES *result = mysql_store_result( m_db );
                    if( !result )
                    {
                        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                        errorMsg.push_back( err );
                    }
                    else
                    {
                        if( mysql_fetch_row( result ) )
                            exists = 1;
                    }
                }
            }
        }
    }
    delete str_data[0];
    str_data[0] = NULL;
    delete str_data[1];
    str_data[1] = NULL;
    delete str_data[2];
    str_data[2] = NULL;
    delete str_data[3];
    str_data[3] = NULL;
    delete str_data[4];
    str_data[4] = NULL;
    return exists;
}

int MySQLDatabase::GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    MYSQL_STMT *stmt;
    int result = 0;
    char *str_data1, *str_data2;
    std::wstring query = L"SELECT * FROM abcattbl WHERE abt_snam = ? AND abt_tnam = ?;";
    std::wstring schemaName = table->GetSchemaName(), tableName = table->GetTableName();
    stmt = mysql_stmt_init( m_db );
    if( !stmt )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
    int res = mysql_stmt_prepare( stmt, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), query.length() );
    if( res )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
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
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
    strncpy( str_data1, m_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str(), schemaName.length() );
    strncpy( str_data2, m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), tableName.length() );
    str_length1 = schemaName.length();
    str_length2 = tableName.length();
    res = mysql_stmt_execute( stmt );
    if( res )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
    MYSQL_ROW tableProp;
    MYSQL_RES *queryResult = mysql_store_result( m_db );
    if( !queryResult )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
    if( queryResult )
    {
        tableProp = mysql_fetch_row( queryResult );
        while( tableProp )
        {
            tableProp = mysql_fetch_row( queryResult );
        }
    }
    else
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
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
        int tableId = const_cast<DatabaseTable *>( table )->GetTableId();
        if( IsTablePropertiesExist( const_cast<DatabaseTable *>( table )->GetTableName(), const_cast<DatabaseTable *>( table )->GetSchemaName(), errorMsg ) && errorMsg.size() == 0 )
            exist = true;
        else
            exist = false;
        if( exist )
        {
            command = L"UPDATE \"sys.abcattbl\" SET \"abt_tnam\" = ";
            command += tableName;
            command += L", \"abt_tid\" = ";
            istr << tableId;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abt_ownr\" = ";
            command += pimpl->m_connectedUser;
            command += L",  \"abd_fhgt\" = ";
            istr << properties.m_dataFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abd_fwgt\" = ";
            istr << properties.m_isDataFontBold;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abd_fitl\" = ";
            command += properties.m_isDataFontItalic ? L"Y" : L"N";
            command += L", \"abd_funl\" = ";
            command += properties.m_isDataFontUnderlined ? L"Y" : L"N";
            command += L", \"abd_fchr\" = ";
            istr << properties.m_dataFontEncoding;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abd_fptc\" = ";
            istr << properties.m_dataFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abd_ffce\" = ";
            command += properties.m_dataFontName;
            command += L",  \"abh_fhgt\" = ";
            istr << properties.m_headingFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abd_fwgt\" = ";
            istr << properties.m_isHeadingFontBold;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abh_fitl\" = ";
            command += properties.m_isHeadingFontItalic ? L"Y" : L"N";
            command += L", \"abh_funl\" = ";
            command += properties.m_isHeadingFontUnderlined ? L"Y" : L"N";
            command += L", \"abh_fchr\" = ";
            istr << properties.m_headingFontEncoding;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abh_fptc\" = ";
            istr << properties.m_headingFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abh_ffce\" = ";
            command += properties.m_headingFontName;
            command += L",  \"abl_fhgt\" = ";
            istr << properties.m_labelFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abl_fwgt\" = ";
            istr << properties.m_isLabelFontBold;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abl_fitl\" = ";
            command += properties.m_isLabelFontItalic ? L"Y" : L"N";
            command += L", \"abl_funl\" = ";
            command += properties.m_isLabelFontUnderlined ? L"Y" : L"N";
            command += L", \"abl_fchr\" = ";
            istr << properties.m_labelFontEncoding;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abl_fptc\" = ";
            istr << properties.m_labelFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", \"abl_ffce\" = ";
            command += properties.m_labelFontName;
            command += L", \"abt_cmnt\" = ";
            command += comment;
            command += L" WHERE \"abt_tnam\" = ";
            command += tableName;
            command += L" AND \"abt_tid\" = ";
            istr << tableId;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L" AND \"abt_ownr\" = ";
            command += pimpl->m_connectedUser;
        }
        else
        {
            command = L"INSERT INTO \"sys.abcattbl\" VALUES( ";
            command += tableName;
            command += L", ";
            istr << tableId;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            command += pimpl->m_connectedUser;
            command += L", ";
            istr << properties.m_dataFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            istr << properties.m_isDataFontBold;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            command += properties.m_isDataFontItalic ? L"Y" : L"N";
            command += L", ";
            command += properties.m_isDataFontUnderlined ? L"Y" : L"N";
            command += L", ";
            istr << properties.m_dataFontEncoding;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            istr << properties.m_dataFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            command += properties.m_dataFontName;
            command += L", ";
            istr << properties.m_headingFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            istr << properties.m_isHeadingFontBold;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            command += properties.m_isHeadingFontItalic ? L"Y" : L"N";
            command += L", ";
            command += properties.m_isHeadingFontUnderlined ? L"Y" : L"N";
            command += L", ";
            istr << properties.m_headingFontEncoding;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            istr << properties.m_headingFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            command += properties.m_headingFontName;
            command += L", ";
            istr << properties.m_labelFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            istr << properties.m_isLabelFontBold;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            command += properties.m_isLabelFontItalic ? L"Y" : L"N";
            command += L", ";
            command += properties.m_isLabelFontUnderlined ? L"Y" : L"N";
            command += L", ";
            istr << properties.m_labelFontEncoding;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            istr << properties.m_labelFontSize;
            command += istr.str();
            istr.clear();
            istr.str( L"" );
            command += L", ";
            command += properties.m_labelFontName;
            command += L", ";
            command += comment;
            command += L" )";
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

bool MySQLDatabase::IsTablePropertiesExist(const std::wstring &tableName, const std::wstring &schemaName, std::vector<std::wstring> &errorMsg)
{
    bool result = false;
    char *str_data1, *str_data2;
    MYSQL_STMT *stmt;
    std::wstring tname = schemaName + L".";
    tname += tableName;
    std::wstring query = L"SELECT 1 FROM abcattbl WHERE abt_tnam = ? AND abt_ownr = ?;";
    stmt = mysql_stmt_init( m_db );
    if( !stmt )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
    if( mysql_stmt_prepare( stmt, m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), query.length() ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
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
        return 1;
    }
    if( mysql_stmt_execute( stmt ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
    if( !( mysql_store_result( m_db ) ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
    if( mysql_stmt_num_rows( stmt ) == 1 )
        result = true;
    if( mysql_stmt_close( stmt ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        return 1;
    }
    return result;
}

int MySQLDatabase::GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &fieldName, Field *table, std::vector<std::wstring> &errorMsg)
{
    char *str_data1, *str_data2, *str_data3;
    int result = 0;
    std::wstring tname = schemaName + L".";
    tname += tableName;
    std::wstring query = L"SELECT * FROM abcatcol WHERE abc_tnam = ? AND abc_ownr = ? AND abc_cnam = ?;";
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
            str_data1 = new char[tname.length()], str_data2 = new char[pimpl->m_connectedUser.length()], str_data3 = new char[fieldName.length()];
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
            params[2].buffer_type = MYSQL_TYPE_STRING;
            params[2].buffer = (char *) str_data3;
            params[2].buffer_length = fieldName.length();
            params[2].is_null = 0;
            params[2].length = &str_length3;
            if( mysql_stmt_bind_param( stmt, params ) )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( err );
                return 1;
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
                    }
                }
            }
        }
        if( mysql_stmt_close( stmt ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            result = 1;
        }
    }
    return result;
}

int MySQLDatabase::ApplyForeignKey(const std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    char *str_data1, *str_data2, *str_data3;
    std::wstring query = L"SELECT 1 FROM information_schema.key_column_usage kcu WHERE constraint_name = ? AND table_schema = ? AND table_name = ?";
    if( mysql_query( m_db, "START TRANSACTION" ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( L"Starting transaction failed during connection: " + err );
        result = 1;
    }
    else
    {
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
                str_data1 = new char[keyName.length()], str_data2 = new char[tableName.GetSchemaName().length()], str_data3 = new char[tableName.GetTableName().length()];
                memset( params, 0, sizeof( params ) );
                params[0].buffer_type = MYSQL_TYPE_STRING;
                params[0].buffer = (char *) str_data1;
                params[0].buffer_length = keyName.length();
                params[0].is_null = 0;
                params[0].length = &str_length1;
                params[1].buffer_type = MYSQL_TYPE_STRING;
                params[1].buffer = (char *) str_data2;
                params[1].buffer_length = tableName.GetSchemaName().length();
                params[1].is_null = 0;
                params[1].length = &str_length2;
                params[3].buffer_type = MYSQL_TYPE_STRING;
                params[3].buffer = (char *) str_data3;
                params[3].buffer_length = tableName.GetTableName().length();
                params[3].is_null = 0;
                params[3].length = &str_length1;
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
                            if( mysql_stmt_num_rows( stmt ) == 0 )
                            {
                                if( mysql_query( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() ) )
                                {
                                    std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                                    errorMsg.push_back( err );
                                    result = 1;
                                }
                            }
                        }
                    }
                }
            }
            if( mysql_stmt_close( stmt ) )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( err );
                result = 1;
            }
        }
        if( result == 0 )
        {
            if( mysql_query( m_db, "COMMIT" ) )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( L"Starting transaction failed during connection: " + err );
                return 1;
            }
        }
        else
        {
            if( mysql_query( m_db, "ROLLBACK" ) )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( L"Starting transaction failed during connection: " + err );
                return 1;
            }
        }
    }
    return result;
}

int MySQLDatabase::DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    return res;
}

int MySQLDatabase::SetFieldProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    if( mysql_query( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() ) )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( err );
        res = 1;
    }
    return res;
}

int MySQLDatabase::TokenizeConnectionString(std::wstring &connectStr, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    while( !connectStr.empty() )
    {
        std::wstring temp = connectStr.substr( 0, connectStr.find( ' ' ) );
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
            m_pimpl->m_dbName = temp2;
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
            mysql_option option;
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
        connectStr = connectStr.substr( connectStr.find( ' ' ) + 1 );
    }
    return result;
}

int MySQLDatabase::GetTableId(const DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

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
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
        }
        else
        {
            result = mysql_select_db( m_db, "" );
            if( result )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( err );
            }
        }
    }
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
        m_db = mysql_real_connect( m_db, m_pimpl->m_myconv.to_bytes( m_pimpl->m_host.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_pimpl->m_user.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_pimpl->m_password.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_pimpl->m_dbName.c_str() ).c_str(), m_port, m_pimpl->m_myconv.to_bytes( m_pimpl->m_socket.c_str() ).c_str(), m_flags );
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
                if( res )
                {
                    res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query2.c_str() ).c_str() );
                    if( res )
                    {
                        res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query3.c_str() ).c_str() );
                        if( res )
                        {
                            res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query4.c_str() ).c_str()  );
                            if( res )
                            {
                                res = mysql_query( m_db, m_pimpl->m_myconv.to_bytes( query5.c_str() ).c_str() );
                                if( res )
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

int MySQLDatabase::Disconnect(std::vector<std::wstring> &WXUNUSED(errorMsg))
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

void MySQLDatabase::GetErrorMessage(int code, std::wstring &errorMsg)
{
    switch( code )
    {
    case 1:
        errorMsg = L"SQL error";
        break;
    case 2:
        errorMsg = L"Internal logic error";
        break;
    case 3:
        errorMsg = L"Access permission denied";
        break;
    case 4:
        errorMsg = L"Callback routine requested an abort";
        break;
    case 5:
        errorMsg = L"The database is locked";
        break;
    case 6:
        errorMsg = L"The table is locked";
        break;
    case 7:
        errorMsg = L"No more memory";
        break;
    case 8:
        errorMsg = L"Database is read-only. No write permited";
        break;
    case 9:
        errorMsg = L"Operation terminated.";
        break;
    case 10:
        errorMsg = L"Disk I/O error occured";
        break;
    case 11:
        errorMsg = L"The database is malformed";
        break;
    case 12:
        errorMsg = L"Unknown opcode in sqlite3_file_control";
        break;
    case 13:
        errorMsg = L"Insertion failed because the database is full";
        break;
    case 14:
        errorMsg = L"Unable to open the database";
        break;
    case 15:
        errorMsg = L"Database lock protocol error";
        break;
    case 16:
        errorMsg = L"Database is empty";
        break;
    case 17:
        errorMsg = L"The database schema changed";
        break;
    case 18:
        errorMsg = L"String or BLOB exceeds size limit";
        break;
    case 19:
        errorMsg = L"Abort due to constraint violation";
        break;
    case 20:
        errorMsg = L"Data type mismatch";
        break;
    case 21:
        errorMsg = L"Library used incorrectly";
        break;
    case 22:
        errorMsg = L"Feature is not supported by OS host";
        break;
    case 23:
        errorMsg = L"Authorization denied";
        break;
    case 24:
        errorMsg = L"Auxiliary database format error";
        break;
    case 25:
        errorMsg = L"Bind parameter out of range";
        break;
    case 26:
        errorMsg = L"Not SQLite database";
        break;
    case 27:
        errorMsg = L"Notification from the log function";
        break;
    case 28:
        errorMsg = L"Warning from the log function";
        break;
    }
}

int MySQLDatabase::GetTableListFromDb(std::vector<std::wstring> &errorMsg)
{
    int res;
    MYSQL_STMT *res1, *res2;
    std::vector<Field *> fields;
    std::vector<std::wstring> fk_names;
    std::map<int,std::vector<FKField *> > foreign_keys;
    std::wstring errorMessage;
    std::string fieldName, fieldType, fieldDefaultValue, fkTable, fkField, fkTableField, fkUpdateConstraint, fkDeleteConstraint;
    char *str_data1, *str_data2;
    int result = 0, fieldIsNull, fieldPK, fkReference, fkId;
    FK_ONUPDATE update_constraint = NO_ACTION_UPDATE;
    FK_ONDELETE delete_constraint = NO_ACTION_DELETE;
    std::wstring query1 = L"SELECT table_catalog, table_schema, table_name FROM information_schema.tables WHERE table_type = 'BASE TABLE' OR table_type = 'VIEW';";
    std::wstring query2 = L"SELECT cols.column_name, cols.data_type, cols.character_maximum_length, cols.character_octet_length, cols.numeric_precision, cols.numeric_scale, cols.column_default, cols.is_nullable, cols.ordinal_position, (CASE WHEN kcu.column_name = cols.column_name THEN 1 ELSE 0 END) as pk_flag FROM information_schema.columns cols, information_schema.key_column_usage kcu WHERE kcu.constraint_name = 'PRIMARY' AND kcu.table_schema = cols.table_schema AND kcu.table_name = cols.table_name AND cols.table_schema = ? AND cols.table_name = ?;";
    std::wstring query3 = L"SELECT kcu.table_catalog, kcu.table_schema, kcu.table_name, kcu.column_name, kcu.ordinal_position, kcu.referenced_table_schema, kcu.referenced_table_name, kcu.referenced_column_name, rc.update_rule, rc.delete_rule FROM information_schema.key_column_usage kcu, information_schema.referential_constraints rc WHERE kcu.constraint_name = rc.constraint_name AND kcu.table_schema = ? AND kcu.table_name = ?;";
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
    while( row = mysql_fetch_row( results ) )
    {
        char *catalog_name = row[0] ? row[0] : NULL;
        char *schema_name = row[1] ? row[1] : NULL;
        char *table_name = row[2] ? row[2] : NULL;
        res1 = mysql_stmt_init( m_db );
        if( !res1 )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            return 1;
        }
        if( mysql_stmt_prepare( res1, m_pimpl->m_myconv.to_bytes( query3.c_str() ).c_str(), query3.length() ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            return 1;
        }
        MYSQL_BIND params[2];
        unsigned long str_length1, str_length2;
        str_data1 = new char[strlen( schema_name )], str_data2 = new char[strlen( table_name )];
        memset( params, 0, sizeof( params ) );
        params[0].buffer_type = MYSQL_TYPE_STRING;
        params[0].buffer = (char *) str_data1;
        params[0].buffer_length = strlen( schema_name );
        params[0].is_null = 0;
        params[0].length = &str_length1;
        params[1].buffer_type = MYSQL_TYPE_STRING;
        params[1].buffer = (char *) str_data2;
        params[1].buffer_length = strlen( table_name );
        params[1].is_null = 0;
        params[1].length = &str_length2;
        if( mysql_stmt_bind_param( res1, params ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            return 1;
        }
        str_length1 = strlen( schema_name );
        str_length2 = strlen( table_name );
        strncpy( str_data1, schema_name, strlen( schema_name ) );
        strncpy( str_data2, table_name, strlen( table_name ) );
        if( mysql_stmt_execute( res1 ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            return 1;
        }
        MYSQL_ROW fk;
        MYSQL_RES *queryResult = mysql_store_result( m_db );
        if( queryResult )
        {
            while( ( fk = mysql_fetch_row( queryResult ) ) )
            {
            }
        }
        if( mysql_stmt_close( res1 ) )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
            errorMsg.push_back( err );
            return 1;
        }
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

void MySQLDatabase::SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg)
{
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
    delete str_length[0];
    str_length[0] = NULL;
    delete str_length[1];
    str_length[1] = NULL;
    delete str_length[3];
    str_length[3] = NULL;
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
        while( ( tableProp = mysql_fetch_row( queryResult ) ) )
        {
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

int MySQLDatabase::SetTableProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

bool MySQLDatabase::IsTablePropertiesExist(const std::wstring &tableName, const std::wstring &schemaName, std::vector<std::wstring> &errorMsg)
{
    bool result = false;
    return result;
}

int MySQLDatabase::GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &fieldName, Field *table, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

int MySQLDatabase::ApplyForeignKey(const std::wstring &command, DatabaseTable &tableName, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
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
            int res = mysql_options( m_db, option, m_pimpl->m_myconv.to_bytes( temp2.c_str() ).c_str() );
            if( res )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
                errorMsg.push_back( err );
                result = 1;
                break;
            }
        }
        connectStr = connectStr.substr( connectStr.find( ' ' ) + 1 );
    }
    return result;
}

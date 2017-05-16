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
        result = Connect(name, errorMsg);
    return result;
}

int MySQLDatabase::DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    PGresult *res;
    std::wstring query = L"DROP TABLE " + name;
    if( pimpl->m_dbName == name )
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

int MySQLDatabase::Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg)
{
    int result = 0, res;
    std::wstring err;
    std::string query1 = "CREATE TABLE IF NOT EXISTS abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));";
    std::string query2 = "CREATE TABLE IF NOT EXISTS abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));";
    std::string query3 = "CREATE TABLE IF NOT EXISTS abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));";
    std::string query4 = "CREATE TABLE IF NOT EXISTS abcattbl(abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ));";
    std::string query5 = "CREATE TABLE IF NOT EXISTS abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));";
    std::wstring errorMessage;
    m_db = mysql_init();
    if( !m_db )
    {
        err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( L"Connection to database failed: " + err );
        result = 1;
    }
    TokenizeConnectionString( selectedDSN );
    m_db = mysql_real_connect( m_db, m_pimpl->m_myconv.to_bytes( m_host.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_user.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_password.c_str() ).c_str(), m_pimpl->m_myconv.to_bytes( m_db.c_str() ).c_str(), m_port );
    if( !m_db )
    {
        err = m_pimpl->m_myconv.from_bytes( mysql_error( m_db ) );
        errorMsg.push_back( L"Connection to database failed: " + err );
        result = 1;
    }
    else
    {
        res = mysql_query( m_db, "START TRANSACTION" );
        if( !res )
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
    PGresult *res, *res1, *res2;
    std::vector<Field *> fields;
    std::vector<std::wstring> fk_names;
    std::map<int,std::vector<FKField *> > foreign_keys;
    std::wstring errorMessage;
    std::string fieldName, fieldType, fieldDefaultValue, fkTable, fkField, fkTableField, fkUpdateConstraint, fkDeleteConstraint;
    int result = 0, fieldIsNull, fieldPK, fkReference, fkId;
    FK_ONUPDATE update_constraint = NO_ACTION_UPDATE;
    FK_ONDELETE delete_constraint = NO_ACTION_DELETE;
    std::string query1 = "SELECT table_catalog, table_schema, table_name FROM information_schema.tables WHERE table_type = 'BASE TABLE' OR table_type = 'VIEW';";
    std::string query2 = "SELECT column_name, data_type, character_maximum_length, character_octet_length, numeric_precision, numeric_scale, column_default, is_nullable, FROM information_schema.columns WHERE table_catalog = ? AND table_schema = ? AND table_name = ?;";
    std::string query3 = "SELECT * FROM information_schema.key_column_usage WHERE referenced_table_schema = ? AND referenced_table_name = ?;";
    res = mysql_query( m_db, query1.c_str() );
    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        PQclear( res );
        return 1;
    }
    for( int i = 0; i < PQntuples( res ); i++ )
    {
        char *catalog_name = PQgetvalue( res, i, 0 );
        char *schema_name = PQgetvalue( res, i, 1 );
        char *table_name = PQgetvalue( res, i, 2 );
        char *values1[2];
        values1[0] = new char[strlen( schema_name ) + 1];
        values1[1] = new char[strlen( table_name ) + 1];
        strcpy( values1[0], schema_name );
        strcpy( values1[1], table_name );
        int len1 = strlen( schema_name );
        int len2 = strlen( table_name );
        int length1[2] = { len1, len2 };
        int formats1[2] = { 1, 1 };
        res1 = PQprepare( m_db, "get_fkeys", query3.c_str(), 3, NULL );
        if( PQresultStatus( res1 ) != PGRES_COMMAND_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error executing query: " + err );
            PQclear( res1 );
            return 1;
        }
        else
        {
            PQclear( res1 );
            res1 = PQexecPrepared( m_db, "get_fkeys", 2, values1, length1, formats1, 1 );
            status = PQresultStatus( res1 ); 
            if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                errorMsg.push_back( L"Error executing query: " + err );
                PQclear( res1 );
                return 1;
            }
            int count = 0;
            for( int j = 0; j < PQntuples( res1 ); j++ )
            {
                fkField = PQgetvalue( res1, j, 2 );
                fkTable = PQgetvalue( res1, j, 3 );
                fkTableField = PQgetvalue( res1, j, 4 );
                fkUpdateConstraint = PQgetvalue( res1, j, 5 );
                fkDeleteConstraint = PQgetvalue( res1, j, 6 );
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
                foreign_keys[count].push_back( new FKField( fkReference, m_pimpl->m_myconv.from_bytes( fkTable ), m_pimpl->m_myconv.from_bytes( fkField ), m_pimpl->m_myconv.from_bytes( fkTableField ), L"", update_constraint, delete_constraint ) );
                fk_names.push_back( m_pimpl->m_myconv.from_bytes( fkField ) );
            }
            PQclear( res1 );
            res2 = PQprepare( m_db, "get_columns", query2.c_str(), 3, NULL );
            if( PQresultStatus( res2 ) != PGRES_COMMAND_OK )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                errorMsg.push_back( L"Error executing query: " + err );
                PQclear( res2 );
                return 1;
            }
            else
            {
                PQclear( res2 );
                res2 = PQexecPrepared( m_db, "get_columns", 2, values1, length1, formats1, 1 );
                if( PQresultStatus( res2 ) != PGRES_COMMAND_OK )
                {
                    std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                    errorMsg.push_back( L"Error executing query: " + err );
                    PQclear( res2 );
                    return 1;
                }
                for( int j = 0; j < PQntuples( res2 ); j++ )
                {
                    fieldName = PQgetvalue( res2, j, 0 );
                    fieldType = PQgetvalue( res2, j, 1 );
                    char *char_length = PQgetvalue( res2, j, 2 );
                    char *char_radix = PQgetvalue( res2, j, 3 );
                    char *numeric_length = PQgetvalue( res2, j, 4 );
                    char *numeric_radix = PQgetvalue( res2, j, 5 );
                    char *numeric_scale = PQgetvalue( res2, j, 6 );
                    fieldDefaultValue = PQgetvalue( res2, j, 7 );
                    fieldIsNull = !strcmp( PQgetvalue( res2, j, 8 ), "YES" ) ? 1 : 0;
                    Field *field = new Field();
                    if( GetFieldProperties( m_pimpl->m_myconv.from_bytes( table_name ), m_pimpl->m_myconv.from_bytes( schema_name ), m_pimpl->m_myconv.from_bytes( fieldName ), field, errorMsg ) )
                    {
                        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                        errorMsg.push_back( err );
                        PQclear( res2 );
                        return 1;
                    }
                    fields.push_back( field );
                }
                PQclear( res2 );
                DatabaseTable *table = new DatabaseTable( m_pimpl->m_myconv.from_bytes( table_name ), m_pimpl->m_myconv.from_bytes( schema_name ), fields, foreign_keys );
                if( GetTableProperties( table, errorMsg ) )
                {
                    char *err = PQerrorMessage( m_db );
                    errorMsg.push_back( m_pimpl->m_myconv.from_bytes( err ) );
                    return 1;
                }
                pimpl->m_tables[m_pimpl->m_myconv.from_bytes( catalog_name )].push_back( table );
                fields.erase( fields.begin(), fields.end() );
                foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                fk_names.clear();
            }
        }
        PQclear( res1 );
    }
    return 0;
}

int MySQLDatabase::CreateIndex(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    PGresult *res;
    int result = 0;
    res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() );
    ExecStatusType status = PQresultStatus( res ); 
    if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        PQclear( res );
        result = 1;
    }
    return result;
}

void MySQLDatabase::SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg)
{
}

bool MySQLDatabase::IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    PGresult *res;
    bool exists = false;
    std::wstring query = L"SELECT count(*) FROM pg_indexes WHERE schemaname = $1 AND tablename = $2 AND indexname = $3;";
    char *values[3];
    values[0] = new char[schemaName.length() + 1];
    values[1] = new char[tableName.length() + 1];
    values[2] = new char[indexName.length() + 1];
    memset( values[0], '\0', schemaName.length() + 1 );
    memset( values[1], '\0', tableName.length() + 1 );
    memset( values[2], '\0', indexName.length() + 1 );
    strcpy( values[0], m_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str() );
    strcpy( values[1], m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str() );
    strcpy( values[2], m_pimpl->m_myconv.to_bytes( indexName.c_str() ).c_str() );
    int len1 = schemaName.length();
    int len2 = tableName.length();
    int len3 = indexName.length();
    int length[3] = { len1, len2, len3 };
    int formats[3] = { 1, 1, 1 };
    res = PQprepare( m_db, "index_exist", m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), 3, NULL );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Error executing query: " + err );
        PQclear( res );
    }
	else
    {
        res = PQexecPrepared( m_db, "index_exist", 3, values, length, formats, 1 );
        ExecStatusType status = PQresultStatus( res ); 
        if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error executing query: " + err );
            PQclear( res );
        }
		else
        {
            if( PQnfields( res ) == 1 )
                exists = 1;
        }
	}
    delete values[0];
    values[0] = NULL;
    delete values[1];
    values[1] = NULL;
    delete values[2];
    values[2] = NULL;
    return exists;
}

int MySQLDatabase::GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring query = L"SELECT * FROM abcattbl WHERE abt_snam = $1 AND abt_tnam = $2;";
    std::wstring schemaName = table->GetSchemaName(), tableName = table->GetTableName();
    char *values[2];
    values[0] = new char[schemaName.length() + 1];
    values[1] = new char[tableName.length() + 1];
    memset( values[0], '\0', schemaName.length() + 1 );
    memset( values[1], '\0', tableName.length() + 1 );
    strcpy( values[0], m_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str() );
    strcpy( values[1], m_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str() );
    int len1 = schemaName.length();
    int len2 = tableName.length();
    int length[2] = { len1, len2 };
    int formats[2] = { 1, 1 };
    PGresult *res = PQprepare( m_db, "index_exist", m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), 3, NULL );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Error executing query: " + err );
        PQclear( res );
        result = 1;
    }
    else
    {
        res = PQexecPrepared( m_db, "index_exist", 2, values, length, formats, 1 );
        ExecStatusType status = PQresultStatus( res );
        if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error executing query: " + err );
            PQclear( res );
            result = 1;
        }
        else
        {
            for( int i = 0; i < PQntuples( res ); i++ )
            {
                table->SetComment( m_pimpl->m_myconv.from_bytes( (const char *) PQgetvalue( res, i, 25 ) ) );
            }
        }
	}
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

void MySQLDatabase::TokenizeConnectionString(const std::wstring &connectStr)
{
    std::wstring temp = connectStr.substr( 0, connectStr.find( ' ' ) );
    std::wstring temp1 = temp.substr( 0, temp.find( '=' ) );
    std::wstring temp2 = temp.substr( temp.find( '=' ) );
    if( temp1 == "host" )
        m_host = temp2;
    if( temp1 == "user" )
        m_user = temp2;
    if( temp1 == "password" )
        m_password = temp2;
    if( temp1 == "port" )
        m_port = std::stoi( temp2 );
    if( temp1 == "dbname" )
        m_db = temp2;
    connectStr = connectStr.substr( connectStr.find( ' ' ) );
}

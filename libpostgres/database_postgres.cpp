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
#include "libpq-fe.h"
#include "database.h"
#include "database_postgres.h"

PostgresDatabase::PostgresDatabase() : Database()
{
    m_db = NULL;
    pimpl = new Impl;
    pimpl->m_type = L"PostgreSQL";
    pimpl->m_subtype = L"";
    m_pimpl = new PostgresImpl;
}

PostgresDatabase::~PostgresDatabase()
{
}

int PostgresDatabase::CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    result = Disconnect( errorMsg );
    if( !result )
        result = Connect(name, errorMsg);
    return result;
}

int PostgresDatabase::DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
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
			char *err = PQerrorMessage( m_db );
			errorMsg.push_back( _( "Error dropping database: " ) + err );
        }
		PQclear( res );
    }
    return result;
}

int PostgresDatabase::Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    PGresult *res;
    char *err;
    std::string query1 = "CREATE TABLE IF NOT EXISTS abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));";
    std::string query2 = "CREATE TABLE IF NOT EXISTS abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));";
    std::string query3 = "CREATE TABLE IF NOT EXISTS abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));";
    std::string query4 = "CREATE TABLE IF NOT EXISTS abcattbl(abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ));";
    std::string query5 = "CREATE TABLE IF NOT EXISTS abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));";
    std::wstring errorMessage;
    m_db = PQconnectdb( m_pimpl->m_myconv.to_bytes( selectedDSN.c_str() ).c_str() );
    if( PQstatus( m_db ) != CONNECTION_OK )
    {
        err = PQerrorMessage( m_db );
        errorMsg.push_back( _( "Connection to database failed: " ) + err );
        result = 1;
    }
    else
    {
        res = PQexec( m_db, "START TRANSACTION" );
        if( PQresultStatus( res ) != PGRES_COMMAND_OK )
        {
            err = PQerrorMessage( m_db );
            errorMsg.push_back( _( "Starting transaction failed during connection: " ) + err );
            result = 1;
            PQclear( res );
        }
        else
        {
            PQclear( res );
            res = PQexec( m_db, query1.c_str() );
            if( PQresultStatus( res ) == PGRES_COMMAND_OK )
            {
                PQclear( res );
                res = PQexec( m_db, query2.c_str() );
                if( PQresultStatus( res ) == PGRES_COMMAND_OK )
                {
                    PQclear( res );
                    res = PQexec( m_db, query3.c_str() );
                    if( PQresultStatus( res ) == PGRES_COMMAND_OK )
					{
                        PQclear( res );
                        res = PQexec( m_db, query4.c_str() );
                        if( PQresultStatus( res ) == PGRES_COMMAND_OK )
                        {
                            PQclear( res );
                            res = PQexec( m_db, query5.c_str() );
                            if( PQresultStatus(res) == PGRES_COMMAND_OK )
                            {
                                PQclear( res );
                                res = PQexec(m_db, "COMMIT");
                                if( PQresultStatus(res) == PGRES_COMMAND_OK )
                                    PQclear( res );
                            }
                        }
                    }
                }
            }
        }
        if( PQresultStatus( res ) != PGRES_COMMAND_OK )
        {
            PQclear( res );
            err = PQerrorMessage( m_db );
            errorMsg.push_back( _( "Error during database connection: " ) + err );
            res = PQexec( m_db, "ROLLBACK" );
            result = 1;
        }
        else
        {
            GetTableListFromDb( errorMsg );
        }
    }
    return result;
}

int PostgresDatabase::Disconnect(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    PQfinish( m_db );
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

void PostgresDatabase::GetErrorMessage(int code, std::wstring &errorMsg)
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

int PostgresDatabase::GetTableListFromDb(std::vector<std::wstring> &errorMsg)
{
    PGresult *res, *res1, *res2, *res3;
    std::vector<Field *> fields;
    std::vector<std::wstring> fk_names;
    std::map<int,std::vector<FKField *> > foreign_keys;
    std::wstring errorMessage;
    std::string fieldName, fieldType, fieldDefaultValue, fkTable, fkField, fkTableField, fkUpdateConstraint, fkDeleteConstraint;
    int result = 0, fieldIsNull, fieldPK, fkReference, autoinc, fkId;
    FK_ONUPDATE update_constraint = NO_ACTION_UPDATE;
    FK_ONDELETE delete_constraint = NO_ACTION_DELETE;
	std::string query1 = "DECLARE alltables CURSOR SELECT table_schema, table_name FROM information_schema.tables WHERE table_type = 'BASE TABLE' OR table_type = 'VIEW' OR table_type = 'LOCAL TEMPORARY';";
    std::string query2 = "DECLARE allcolumns CURSOR SELECT cols.column_name, cols.data_type, cols.character_maximum_length, cols.character_octet_length, cols.numeric_precision, cols.numeric_precision_radix, cols.numeric_scale, cols.column_default, cols.is_nullable, table_cons.constraint_type FROM information_schema.columns AS cols, information_schema.table_constraints AS table_cons WHERE cols.schema_name = table_cons.table_schema AND cols.table_name = table_cons.table_name AND table_schema = $1 AND table_name = $2 ORDER BY ordinal_position ASC;";
    std::string query3 = "DECLARE allforeignkeys CURSOR SELECT information_schema.table_constraints.constraint_name, information_schema.table_constraints.schema_name, information_schema.table_constraints.table_name, information_schema.key_column_usage.column_name, information_schema.constraint_column_usage.table_name, information_schema.constraint_column_usage.column_name, information_schema.referential_constraints.update_rule, information_schema.referential_constraints.delete_rule FROM information_schema.table_constraints, information_schema.key_column_usage, information_schema.constraint_column_usage, information_schema.referential_constraints WHERE information_schema.table_constraints.constraint_name = information_schema.key_column_usage.constraint_name AND information_schema.constraint_column_usage.constraint_name = information_schema.table_constraints.constraint_name AND information_schema.referential_constraints.constraint_name = information_schema.table_constraints.constraint_name AND constraint_type = 'FOREIGN KEY' AND information_schema.table_constraints.schema_name = $1 AND information_schema.table_constraints.table_name = $2;";
    res = PQexec( m_db, query1.c_str() );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        char *err = PQerrorMessage( m_db );
        errorMsg.push_back( _( "Error executing query: " ) + err );
        PQclear( res1 );
        return 1;
    }
    PQclear( res );
    res = PQexec( m_db, "FETCH ALL in alltables" );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        char *err = PQerrorMessage( m_db );
        errorMsg.push_back( _( "Error executing query: " ) + err );
        PQclear( res1 );
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
        int length1[2] = { strlen( schema_name ), strlen( table_name ) };
        int formats1[2] = { 1, 1 };
        res1 = PQprepare( m_db, "get_fkeys", query3.c_str(), 3, NULL );
        if( PQresultStatus( res1 ) != PGRES_COMMAND_OK )
        {
            char *err = PQerrorMessage( m_db );
            errorMsg.push_back( _( "Error executing query: " ) + err );
            PQclear( res1 );
            return 1;
        }
        else
        {
            PQclear( res1 );
            res1 = PQexecPrepared( m_db, "get_fkeys", 2, values1, length1, formats1, 1 );
            if( PQresultStatus( res1 ) != PGRES_COMMAND_OK )
            {
                char *err = PQerrorMessage( m_db );
                errorMsg.push_back( _( "Error executing query: " ) + err );
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
                char *err = PQerrorMessage( m_db );
                errorMsg.push_back( _( "Error executing query: " ) + err );
                PQclear( res2 );
                return 1;
            }
            else
            {
                PQclear( res2 );
                res2 = PQexecPrepared( m_db, "get_columns", 2, values1, length1, formats1, 1 );
                if( PQresultStatus( res2 ) != PGRES_COMMAND_OK )
                {
                    char *err = PQerrorMessage( m_db );
                    errorMsg.push_back( _( "Error executing query: " ) + err );
                    PQclear( res2 );
                    return 1;
                }
                for( j = 0; j < PQntuples( res2 ); j++ )
                {
                    fieldName = PQgetvalue( res2, j, 0 );
                    fieldType = PQgetvalue( res2, j, 1 );
                    char *char_length = PQgetvalue( res2, j, 2 );
                    char *char_radix = PQegtvalue( res2, j, 3 );
                    char *numeric_length = PQgetvalue( res2, j, 4 );
                    char *numeric_radix = PQgetvalue( res2, j, 5 );
                    char *numeric_scale = PQgetvalue( res2, j, 6 );
                    fieldDefaultValue = PQgetvalue( res2, j, 7 );
                    fieldIsNull = PQgetvalue( res2, j, 8 );
                    Field *field = new Field();
                    if( GetFieldProperties() )
                    {
                        char *err = PQerrorMessage( m_db );
                        errorMsg.push_back( _() );
                        PQclear( res2 );
                        return 1;
                    }
                    field.push_back( field );
                }
                PQclear( res2 );
                DatabaseTable *table = new DatabaseTable();
                if( GetTableProperties( table, errorMsg ) )
                {
                    char *err = PQerrorMessage();
                    errorMsg.push_back();
                    return 1;
                }
                pimpl->m_tables[].push_back( table );
                fields.erase( fields.begin(), fields.end() );
                foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                fk_names.clear();
            }
        }
        PQclear( res1 );
    }
    return 0;
}

int PostgresDatabase::CreateIndex(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

void PostgresDatabase::SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg)
{
}

bool PostgresDatabase::IsIndexExists(const std::wstring &indexName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    bool exists = false;
    return exists;
}

int PostgresDatabase::GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

int PostgresDatabase::SetTableProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

bool PostgresDatabase::IsTablePropertiesExist(const std::wstring &tableName, const std::wstring &schemaName, std::vector<std::wstring> &errorMsg)
{
    bool result = false;
    return result;
}

int PostgresDatabase::GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &fieldName, Field *table, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

int PostgresDatabase::ApplyForeignKey(const std::wstring &command, DatabaseTable &tableName, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

int PostgresDatabase::DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    return res;
}

int PostgresDatabase::SetFieldProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    return res;
}

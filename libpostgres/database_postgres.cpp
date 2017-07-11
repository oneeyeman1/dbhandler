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
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error dropping database: " + err );
        }
        PQclear( res );
    }
    return result;
}

int PostgresDatabase::Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    PGresult *res;
    std::wstring err;
    std::string query1 = "CREATE TABLE IF NOT EXISTS abcatcol(abc_tnam char(129) NOT NULL, abc_tid integer, abc_ownr char(129) NOT NULL, abc_cnam char(129) NOT NULL, abc_cid smallint, abc_labl char(254), abc_lpos smallint, abc_hdr char(254), abc_hpos smallint, abc_itfy smallint, abc_mask char(31), abc_case smallint, abc_hght smallint, abc_wdth smallint, abc_ptrn char(31), abc_bmap char(1), abc_init char(254), abc_cmnt char(254), abc_edit char(31), abc_tag char(254), PRIMARY KEY( abc_tnam, abc_ownr, abc_cnam ));";
    std::string query2 = "CREATE TABLE IF NOT EXISTS abcatedt(abe_name char(30) NOT NULL, abe_edit char(254), abe_type smallint, abe_cntr integer, abe_seqn smallint NOT NULL, abe_flag integer, abe_work char(32), PRIMARY KEY( abe_name, abe_seqn ));";
    std::string query3 = "CREATE TABLE IF NOT EXISTS abcatfmt(abf_name char(30) NOT NULL, abf_frmt char(254), abf_type smallint, abf_cntr integer, PRIMARY KEY( abf_name ));";
    std::string query4 = "CREATE TABLE IF NOT EXISTS abcattbl(abt_snam char(129), abt_tnam char(129) NOT NULL, abt_tid integer, abt_ownr char(129) NOT NULL, abd_fhgt smallint, abd_fwgt smallint, abd_fitl char(1), abd_funl char(1), abd_fchr smallint, abd_fptc smallint, abd_ffce char(18), abh_fhgt smallint, abh_fwgt smallint, abh_fitl char(1), abh_funl char(1), abh_fchr smallint, abh_fptc smallint, abh_ffce char(18), abl_fhgt smallint, abl_fwgt smallint, abl_fitl char(1), abl_funl char(1), abl_fchr smallint, abl_fptc smallint, abl_ffce char(18), abt_cmnt char(254), PRIMARY KEY( abt_tnam, abt_ownr ));";
    std::string query5 = "CREATE TABLE IF NOT EXISTS abcatvld(abv_name char(30) NOT NULL, abv_vald char(254), abv_type smallint, abv_cntr integer, abv_msg char(254), PRIMARY KEY( abv_name ));";
    std::wstring errorMessage;
    m_db = PQconnectdb( m_pimpl->m_myconv.to_bytes( selectedDSN.c_str() ).c_str() );
    if( PQstatus( m_db ) != CONNECTION_OK )
    {
        err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Connection to database failed: " + err );
        result = 1;
    }
    else
    {
        res = PQexec( m_db, "START TRANSACTION" );
        if( PQresultStatus( res ) != PGRES_COMMAND_OK )
        {
            err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Starting transaction failed during connection: " + err );
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
            err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error during database connection: " + err );
            res = PQexec( m_db, "ROLLBACK" );
            result = 1;
        }
        else
        {
            result = GetTableListFromDb( errorMsg );
        }
    }
    return result;
}

int PostgresDatabase::Disconnect(std::vector<std::wstring> &UNUSED(errorMsg))
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

/*SELECT CAST(current_database() AS sql_identifier) AS table_catalog,
CAST(nc.nspname AS sql_identifier) AS table_schema,
CAST(c.relname AS sql_identifier) AS table_name,
CAST(a.attname AS sql_identifier) AS column_name,
CAST(
CASE WHEN t.typtype = 'd' THEN
CASE WHEN bt.typelem <> 0 AND bt.typlen = -1 THEN 'ARRAY'
WHEN nbt.nspname = 'pg_catalog' THEN format_type(t.typbasetype, null)
ELSE 'USER-DEFINED' END
ELSE
CASE WHEN t.typelem <> 0 AND t.typlen = -1 THEN 'ARRAY'
WHEN nt.nspname = 'pg_catalog' THEN format_type(a.atttypid, null)
ELSE 'USER-DEFINED' END
END
AS character_data) AS data_type,
CAST(_pg_char_max_length(_pg_truetypid(a, t), _pg_truetypmod(a, t)) AS cardinal_number) AS character_maximum_length,
CAST(_pg_char_octet_length(_pg_truetypid(a, t), _pg_truetypmod(a, t)) AS cardinal_numer) AS character_octet_length,
CAST(_pg_numeric_precision(_pg_truetypid(a, t), _pg_truetypmod(a, t)) AS cardinal_number) AS numeric_precision,
CAST(_pg_numeric_precision_radix(_pg_truetypid(a, t), _pg_truetypmod(a, t)) AS cardinal_numer) AS numeric_precision_radix,
CAST(_pg_numeric_scale(_pg_truetypid(a, t), _pg_truetypmod(a, t)) AS cardinal_number) AS numeric_scale,
CAST(CASE WHEN a.attnotnull OR (t.typtype = 'd' AND t.typnotnull) THEN 'NO' ELSE 'YES' END AS yes_or_no) AS isnullable,
CAST(pg_get_expr(ad.adbin, ad.adrelid) AS character_data) AS column_default

FROM pg_class c, pg_namespace nc, pg_attribute a, pg_type t, pg_type bt, pg_namespace nbt, pg_namespace nt, pg_attrdef ad

WHERE a.attrelid = ad.adrelid AND a.attnum = ad.adnum AND c.relnamespace = nc.old AND a.attrelid = c.old AND t.typnamespace = nt.old AND a.atttypid = t.old AND bt.typnamespace = nbt.old
AND t.typtype = 'd' AND t.typbasetype = bt.old

ORDER BY CAST(a.attnum AS cardinal_number) AS ordinal_position ASC;*/
int PostgresDatabase::GetTableListFromDb(std::vector<std::wstring> &errorMsg)
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
    std::string query1 = "SELECT table_catalog, table_schema, table_name FROM information_schema.tables WHERE table_type = 'BASE TABLE' OR table_type = 'VIEW' OR table_type = 'LOCAL TEMPORARY';";
    std::string query2 = "SELECT DISTINCT cols.column_name, cols.data_type, cols.character_maximum_length, cols.character_octet_length, cols.numeric_precision, cols.numeric_precision_radix, cols.numeric_scale, cols.column_default, cols.is_nullable, table_cons.constraint_type FROM information_schema.columns AS cols, information_schema.table_constraints AS table_cons WHERE cols.table_schema = table_cons.table_schema AND cols.table_name = table_cons.table_name AND cols.table_schema = $1 AND cols.table_name = $2 ORDER BY ordinal_position ASC;";
    std::string query3 = "SELECT information_schema.table_constraints.constraint_name, information_schema.table_constraints.constraint_schema, information_schema.table_constraints.table_name, information_schema.key_column_usage.column_name, information_schema.constraint_column_usage.table_name, information_schema.constraint_column_usage.column_name, information_schema.referential_constraints.update_rule, information_schema.referential_constraints.delete_rule FROM information_schema.table_constraints, information_schema.key_column_usage, information_schema.constraint_column_usage, information_schema.referential_constraints WHERE information_schema.table_constraints.constraint_name = information_schema.key_column_usage.constraint_name AND information_schema.constraint_column_usage.constraint_name = information_schema.table_constraints.constraint_name AND information_schema.referential_constraints.constraint_name = information_schema.table_constraints.constraint_name AND constraint_type = 'FOREIGN KEY' AND information_schema.table_constraints.constraint_schema = $1 AND information_schema.table_constraints.table_name = $2;";
    res = PQexec( m_db, query1.c_str() );
    ExecStatusType status = PQresultStatus( res ); 
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

int PostgresDatabase::CreateIndex(const std::wstring &command, std::vector<std::wstring> &errorMsg)
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

int PostgresDatabase::SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

bool PostgresDatabase::IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
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

int PostgresDatabase::GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg)
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

int PostgresDatabase::SetTableProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring err;
    PGresult *res;
    if( IsTablePropertiesExist( table->GetTableName(), table->GetSchemaName(), errors ) && errors.size() == 0 )
        exist = true;
    else
         exist = false;
    if( exist )
        query = L"UPDATE abcattbl SET abt_tnam = $1, abt_ownr = $2,  abd_fhgt = $3, abd_fwgt = $4";
    else
        query = L"INSERT INTO abcattbl VALUES( $1, $2, $3, $4 );";
    res = PQexec( m_db, m_pimpl->m_myconv.to_bytes( command.c_str() ).c_str() );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        PQclear( res );
        err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( err );
        result = 1;
    }
    return result;
}

bool PostgresDatabase::IsTablePropertiesExist(const std::wstring &tableName, const std::wstring &schemaName, std::vector<std::wstring> &errorMsg)
{
    bool result = false;
    std::wstring query = L"SELECT 1 FROM abcattbl WHERE abt_tnam = $1 AND abt_ownr = $2;";
    std::wstring tname = schemaName + L".";
    tname += tableName;
    char *values[2];
    values[1] = new char[tname.length() + 1];
    values[2] = new char[pimpl->m_connectedUser.length() + 1];
    memset( values[0], '\0', tname.length() + 1 );
    memset( values[1], '\0', pimpl->m_connectedUser.length() + 1 );
    strcpy( values[0], m_pimpl->m_myconv.to_bytes( tname.c_str() ).c_str() );
    strcpy( values[1], m_pimpl->m_myconv.to_bytes( pimpl->m_connectedUser.c_str() ).c_str() );
    int len1 = tname.length();
    int len2 = pimpl->m_connectedUser.length();
    int length[2] = { len1, len2 };
    int formats[2] = { 1, 1 };
    PGresult *res = PQprepare( m_db, "table_properties_exist", m_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), 2, NULL );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Error executing query: " + err );
        PQclear( res );
    }
	else
    {
        res = PQexecPrepared( m_db, "table_properties_exist", 2, values, length, formats, 1 );
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
                result = true;
        }
	}
    return result;
}

int PostgresDatabase::GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &fieldName, Field *table, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    return result;
}

int PostgresDatabase::ApplyForeignKey(const std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, std::vector<std::wstring> &errorMsg)
{
    bool exist = false;
    int result = 0;
    std::wstring err;
    std::wstring query1 = L"SELECT 1 FROM information_schema.table_constraints WHERE constraint_name = $1 AND table_schema = $2 AND table_name = $2";
    char *values[3];
    values[0] = new char[keyName.length() + 1];
    values[1] = new char[tableName.GetSchemaName().length() + 1];
    values[2] = new char[tableName.GetTableName().length() + 1];
    memset( values[0], '\0', keyName.length() + 1 );
    memset( values[1], '\0', tableName.GetSchemaName().length() + 1 );
    memset( values[2], '\0', tableName.GetTableName().length() + 1 );
    strcpy( values[0], m_pimpl->m_myconv.to_bytes( keyName.c_str() ).c_str() );
    strcpy( values[1], m_pimpl->m_myconv.to_bytes( tableName.GetSchemaName().c_str() ).c_str() );
    strcpy( values[2], m_pimpl->m_myconv.to_bytes( tableName.GetTableName().c_str() ).c_str() );
    int len0 = keyName.length();
    int len1 = tableName.GetSchemaName().length();
    int len2 = tableName.GetTableName().length();
    int length[3] = { len0, len1, len2 };
    int formats[3] = { 1, 1, 1 };
    PGresult *res = PQexec( m_db, "START TRANSACTION" );
    if( PQresultStatus( res ) != PGRES_COMMAND_OK )
    {
        PQclear( res );
        err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
        errorMsg.push_back( L"Starting transaction failed during connection: " + err );
        result = 1;
    }
    else
    {
        res = PQprepare( m_db, "foreign_key_exist", m_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str(), 2, NULL );
        if( PQresultStatus( res ) != PGRES_COMMAND_OK )
        {
            std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
            errorMsg.push_back( L"Error executing query: " + err );
            PQclear( res );
        }
        else
        {
            res = PQexecPrepared( m_db, "foreign_key_exist", 2, values, length, formats, 1 );
            ExecStatusType status = PQresultStatus( res );
            if( status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK )
            {
                std::wstring err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                errorMsg.push_back( L"Error executing query: " + err );
                PQclear( res );
                result = 1;
            }
            else if( status == PGRES_TUPLES_OK )
            {
                exist = true;
                PQclear( res );
            }
        }
        if( !exist )
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
        if( !result )
        {
            res = PQexec( m_db, "COMMIT" );
            if( PQresultStatus( res ) != PGRES_COMMAND_OK )
            {
                PQclear( res );
                err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                errorMsg.push_back( L"Starting transaction failed during connection: " + err );
                result = 1;
            }
        }
        else
        {
            res = PQexec( m_db, "ROLLBACK" );
            if( PQresultStatus( res ) != PGRES_COMMAND_OK )
            {
                PQclear( res );
                err = m_pimpl->m_myconv.from_bytes( PQerrorMessage( m_db ) );
                errorMsg.push_back( L"Starting transaction failed during connection: " + err );
                result = 1;
            }
        }
    }
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

int PostgresDatabase::SetFieldProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    return res;
}

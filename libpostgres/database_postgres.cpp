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
#include "sqlite3.h"
#include "database.h"
#include "database_sqlite.h"

SQLiteDatabase::SQLiteDatabase() : Database()
{
    m_db = NULL;
    pimpl = new Impl;
    pimpl->m_type = L"SQLite";
    pimpl->m_subtype = L"";
    sqlite_pimpl = new SQLiteImpl;
    sqlite_pimpl->m_catalog = L"";
}

SQLiteDatabase::~SQLiteDatabase()
{
    if( pimpl )
    {
        std::vector<DatabaseTable *> tableVec = pimpl->m_tables[sqlite_pimpl->m_catalog];
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
        delete pimpl;
        pimpl = NULL;
    }
}

int SQLiteDatabase::CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    char *err;
    result = Disconnect( errorMsg );
    if( result == SQLITE_OK )
        result = Connect(name, errorMsg);
    return result;
}

int SQLiteDatabase::DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    if( pimpl->m_dbName == name )
        result = Disconnect( errorMsg );
    if( !result )
        result = remove( sqlite_pimpl->m_myconv.to_bytes( name.c_str() ).c_str() );
    return result;
}

int SQLiteDatabase::Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    char *err;
    std::string query1 = "CREATE TABLE IF NOT EXISTS \"sys.abcatcol\"(\"abc_tnam\" char(129) NOT NULL, \"abc_tid\" integer, \"abc_ownr\" char(129) NOT NULL, \"abc_cnam\" char(129) NOT NULL, \"abc_cid\" smallint, \"abc_labl\" char(254), \"abc_lpos\" smallint, \"abc_hdr\" char(254), \"abc_hpos\" smallint, \"abc_itfy\" smallint, \"abc_mask\" char(31), \"abc_case\" smallint, \"abc_hght\" smallint, \"abc_wdth\" smallint, \"abc_ptrn\" char(31), \"abc_bmap\" char(1), \"abc_init\" char(254), \"abc_cmnt\" char(254), \"abc_edit\" char(31), \"abc_tag\" char(254), PRIMARY KEY( \"abc_tnam\", \"abc_ownr\", \"abc_cnam\" ));";
    std::string query2 = "CREATE TABLE IF NOT EXISTS \"sys.abcatedt\"(\"abe_name\" char(30) NOT NULL, \"abe_edit\" char(254), \"abe_type\" smallint, \"abe_cntr\" integer, \"abe_seqn\" smallint NOT NULL, \"abe_flag\" integer, \"abe_work\" char(32), PRIMARY KEY( \"abe_name\", \"abe_seqn\" ));";
    std::string query3 = "CREATE TABLE IF NOT EXISTS \"sys.abcatfmt\"(\"abf_name\" char(30) NOT NULL, \"abf_frmt\" char(254), \"abf_type\" smallint, \"abf_cntr\" integer, PRIMARY KEY( \"abf_name\" ));";
    std::string query4 = "CREATE TABLE IF NOT EXISTS \"sys.abcattbl\"(\"abt_tnam\" char(129) NOT NULL, \"abt_tid\" integer, \"abt_ownr\" char(129) NOT NULL, \"abd_fhgt\" smallint, \"abd_fwgt\" smallint, \"abd_fitl\" char(1), \"abd_funl\" char(1), \"abd_fchr\" smallint, \"abd_fptc\" smallint, \"abd_ffce\" char(18), \"abh_fhgt\" smallint, \"abh_fwgt\" smallint, \"abh_fitl\" char(1), \"abh_funl\" char(1), \"abh_fchr\" smallint, \"abh_fptc\" smallint, \"abh_ffce\" char(18), \"abl_fhgt\" smallint, \"abl_fwgt\" smallint, \"abl_fitl\" char(1), \"abl_funl\" char(1), \"abl_fchr\" smallint, \"abl_fptc\" smallint, \"abl_ffce\" char(18), \"abt_cmnt\" char(254), PRIMARY KEY( \"abt_tnam\", \"abt_ownr\" ));";
    std::string query5 = "CREATE TABLE IF NOT EXISTS \"sys.abcatvld\"(\"abv_name\" char(30) NOT NULL, \"abv_vald\" char(254), \"abv_type\" smallint, \"abv_cntr\" integer, \"abv_msg\" char(254), PRIMARY KEY( \"abv_name\" ));";
    std::wstring errorMessage;
    if( !pimpl )
        pimpl = new Impl;
    if( !sqlite_pimpl )
        sqlite_pimpl = new SQLiteImpl;
    int res = sqlite3_open( sqlite_pimpl->m_myconv.to_bytes( selectedDSN.c_str() ).c_str(), &m_db );
    if( res != SQLITE_OK )
    {
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
        result = 1;
    }
    else
    {
        res = sqlite3_exec( m_db, "BEGIN TRANSACTION", NULL, NULL, &err );
        if( res != SQLITE_OK )
        {
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
            result = 1;
            sqlite3_free( err );
        }
        else
        {
            res = sqlite3_exec( m_db, query1.c_str(), NULL, NULL, &err );
            if( res == SQLITE_OK )
            {
                res = sqlite3_exec( m_db, query2.c_str(), NULL, NULL, &err );
                if( res == SQLITE_OK )
                {
                    res = sqlite3_exec( m_db, query3.c_str(), NULL, NULL, &err );
                    if( res == SQLITE_OK )
					{
                        res = sqlite3_exec( m_db, query4.c_str(), NULL, NULL, &err );
                        if( res == SQLITE_OK )
                        {
                            res = sqlite3_exec( m_db, query5.c_str(), NULL, NULL, &err );
                            if( res == SQLITE_OK )
                                sqlite3_exec( m_db, "COMMIT", NULL, NULL, &err );
                        }
                    }
                }
            }
        }
        if( res != SQLITE_OK )
        {
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
            res = sqlite3_exec( m_db, "ROLLBACK", NULL, NULL, NULL );
            result = 1;
            sqlite3_free( err );
        }
        else
        {
            sqlite_pimpl->m_catalog = selectedDSN;
            GetTableListFromDb( errorMsg );
            res = sqlite3_exec( m_db, "PRAGMA foreign_keys = ON", NULL, NULL, NULL );
            if( res != SQLITE_OK )
            {
                result = 1;
                GetErrorMessage( res, errorMessage );
                errorMsg.push_back( errorMessage );
            }
            else
                pimpl->m_dbName = sqlite_pimpl->m_catalog;
        }
    }
    return result;
}

int SQLiteDatabase::Disconnect(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    const char *query = NULL;
    std::wstring errorMessage;
    int res = sqlite3_close( m_db );
    if( res != SQLITE_OK )
    {
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
        result = 1;
//  For debugging purposes - helps find non-closed statements
        sqlite3_stmt *statement = sqlite3_next_stmt( m_db, NULL );
        if( statement )
            query = sqlite3_sql( statement );
//  For debugging purposes - helps find non-closed statements
    }
    std::vector<DatabaseTable *> tableVec = pimpl->m_tables[sqlite_pimpl->m_catalog];
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
    delete pimpl;
    pimpl = NULL;
    delete sqlite_pimpl;
    sqlite_pimpl = NULL;
    return result;
}

void SQLiteDatabase::GetErrorMessage(int code, std::wstring &errorMsg)
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

int SQLiteDatabase::GetTableListFromDb(std::vector<std::wstring> &errorMsg)
{
    std::vector<Field *> fields;
    std::vector<std::wstring> fk_names;
    std::map<int,std::vector<FKField *> > foreign_keys;
    std::wstring errorMessage;
    sqlite3_stmt *stmt = NULL, *stmt2 = NULL, *stmt3 = NULL;
    std::string fieldName, fieldType, fieldDefaultValue, fkTable, fkField, fkTableField, fkUpdateConstraint, fkDeleteConstraint;
    int result = 0, res = SQLITE_OK, res1 = SQLITE_OK, res2 = SQLITE_OK, res3 = SQLITE_OK, fieldIsNull, fieldPK, fkReference, autoinc, fkId;
    FK_ONUPDATE update_constraint = NO_ACTION_UPDATE;
    FK_ONDELETE delete_constraint = NO_ACTION_DELETE;
    std::string query1 = "SELECT name FROM sqlite_master WHERE type = ?";
    std::string query2 = "PRAGMA table_info(\"%w\");";
    std::string query3 = "PRAGMA foreign_key_list(\"%w\")";
    if( ( res = sqlite3_prepare_v2( m_db, query1.c_str(), (int) query1.length(), &stmt, 0 ) ) == SQLITE_OK )
    {
        res = sqlite3_bind_text( stmt, 1, "table", -1, SQLITE_STATIC );
        if( res == SQLITE_OK )
        {
            for( ; ; )
            {
                res = sqlite3_step( stmt );
                if( res == SQLITE_ROW  )
                {
                    const unsigned char *tableName = sqlite3_column_text( stmt, 0 );
                    char *y = sqlite3_mprintf( query3.c_str(), tableName );
                    res2 = sqlite3_prepare( m_db, y, -1, &stmt3, 0 );
                    if( res2 == SQLITE_OK )
                    {
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
                                foreign_keys[fkId].push_back( new FKField( fkReference, sqlite_pimpl->m_myconv.from_bytes( fkTable ), sqlite_pimpl->m_myconv.from_bytes( fkField ), sqlite_pimpl->m_myconv.from_bytes( fkTableField ), L"", update_constraint, delete_constraint ) );
	                            fk_names.push_back( sqlite_pimpl->m_myconv.from_bytes( fkField ) );
                            }
                            else if( res3 == SQLITE_DONE )
                                break;
                            else
                            {
                                result = 1;
                                GetErrorMessage( res, errorMessage );
                                errorMsg.push_back( errorMessage );
                                break;
                            }
                        }
                        if( res3 != SQLITE_DONE )
                            break;
                    }
                    else
                    {
                        result = 1;
                        GetErrorMessage( res, errorMessage );
                        errorMsg.push_back( errorMessage );
                    }
                    sqlite3_finalize( stmt3 );
                    sqlite3_free( y );
                    if( res3 == SQLITE_DONE )
                    {
                        char *z = sqlite3_mprintf( query2.c_str(), tableName );
                        if( ( res1 = sqlite3_prepare_v2( m_db, z, -1, &stmt2, 0 ) ) == SQLITE_OK )
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
                                    int res = sqlite3_table_column_metadata( m_db, NULL, (const char *) tableName, fieldName.c_str(), NULL, NULL, NULL, NULL, &autoinc );
                                    if( res != SQLITE_OK )
                                    {
                                        result = 1;
                                        GetErrorMessage( res, errorMessage );
                                        errorMsg.push_back( errorMessage );
                                        break;
                                    }
                                    std::wstring fieldComment = L"";
//                                    GetColumnComment( sqlite_pimpl->m_myconv.from_bytes( (const char *) tableName ), sqlite_pimpl->m_myconv.from_bytes( fieldName ), fieldComment, errorMsg );
                                    if( errorMsg.empty() )
                                    {
                                        Field *field = new Field( sqlite_pimpl->m_myconv.from_bytes( fieldName ), sqlite_pimpl->m_myconv.from_bytes( fieldType ), 0, 0, sqlite_pimpl->m_myconv.from_bytes( fieldDefaultValue ), fieldIsNull == 0 ? false: true, autoinc == 1 ? true : false, fieldPK >= 1 ? true : false, std::find( fk_names.begin(), fk_names.end(), sqlite_pimpl->m_myconv.from_bytes( fieldName ) ) != fk_names.end() );
                                        if( GetFieldProperties( sqlite_pimpl->m_myconv.from_bytes( (const char *) tableName ), L"", sqlite_pimpl->m_myconv.from_bytes( fieldName ), field, errorMsg ) )
                                        {
                                            result = 1;
                                            GetErrorMessage( res, errorMessage );
                                            errorMsg.push_back( errorMessage );
                                            sqlite3_finalize( stmt2 );
                                            break;
                                        }
                                        fields.push_back( field );
                                    }
                                }
                                else if( res1 == SQLITE_DONE )
                                    break;
                                else
                                {
                                    result = 1;
                                    GetErrorMessage( res, errorMessage );
                                    errorMsg.push_back( errorMessage );
                                    break;
                                }
                            }
                            if( res1 != SQLITE_DONE )
                                break;
                        }
                        else
                        {
                            result = 1;
                            GetErrorMessage( res, errorMessage );
                            errorMsg.push_back( errorMessage );
                        }
                        sqlite3_finalize( stmt2 );
                        sqlite3_free( z );
                    }
                    if( res1 == SQLITE_DONE && res3 == SQLITE_DONE )
                    {
                        std::wstring comment = L"";
                        std::wstring name = sqlite_pimpl->m_myconv.from_bytes( (const char *) tableName );
                        DatabaseTable *table = new DatabaseTable( name, L"", fields, foreign_keys );
                        if( GetTableProperties( table, errorMsg ) )
                        {
                            fields.erase( fields.begin(), fields.end() );
                            foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                            fk_names.clear();
                            sqlite3_finalize( stmt );
                            return 1;
                        }
                        pimpl->m_tables[sqlite_pimpl->m_catalog].push_back( table );
                        fields.erase( fields.begin(), fields.end() );
                        foreign_keys.erase( foreign_keys.begin(), foreign_keys.end() );
                        fk_names.clear();
                    }
                }
                else if( res == SQLITE_DONE )
                    break;
                else
                {
                    result = 1;
                    GetErrorMessage( res, errorMessage );
                    errorMsg.push_back( errorMessage );
                    break;
                }
            }
        }
        else
        {
            result = 1;
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
        }
    }
    else
    {
        result = 1;
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
    }
    sqlite3_finalize( stmt );
    return result;
}

int SQLiteDatabase::CreateIndex(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    std::wstring errorMessage;
    int res = SQLITE_OK, result = 0;
    sqlite3_stmt *stmt = NULL;
    if( result == SQLITE_OK )
    {
        if( ( res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( command.c_str() ).c_str(), -1, &stmt, 0 ) ) == SQLITE_OK )
        {
            res = sqlite3_step( stmt );
            if( res != SQLITE_DONE )
            {
                result = 1;
                GetErrorMessage( res, errorMessage );
                errorMsg.push_back( errorMessage );
            }
        }
        else
        {
            result = 1;
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
        }
        sqlite3_finalize( stmt );
    }
    else
    {
        result = 1;
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
    }
    return result;
}

void SQLiteDatabase::SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg)
{
    bool found = false;
    std::wstring errorMessage;
    sqlite3_stmt *stmt = NULL;
    std::wstring query1 = L"SELECT count(*) FROM \"sys.abcatcol\" WHERE \"abc_tnam\" = ? AND \"abc_cnam\" = ?;", query2;
    int res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str(), (int) query1.length(), &stmt, 0 );
    if( res == SQLITE_OK )
    {
        res = sqlite3_bind_text( stmt, 1, sqlite_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
        if( res == SQLITE_OK )
        {
            res = sqlite3_bind_text( stmt, 2, sqlite_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
            if( res == SQLITE_OK )
            {
                res = sqlite3_step( stmt );
                if( res == SQLITE_ROW )
                {
                    found = true;
                }
                else if( res != SQLITE_DONE )
                {
                    GetErrorMessage( res, errorMessage );
                    errorMsg.push_back( errorMessage );
                }
            }
            else
            {
                GetErrorMessage( res, errorMessage );
                errorMsg.push_back( errorMessage );
            }
        }
        else
        {
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
        }
    }
    else
    {
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
    }
    sqlite3_finalize( stmt );
    if( found )
        query2 = L"UPDATE \"sys.abcatcol\" SET \"abc_cmnt\" = ? WHERE \"abc_tnam\" = ? AND \"abc_ownr\" == \"\" AND \"abc_cnam\" = ?;";
    else
        query2 = L"INSERT INTO \"sys.abcattbl\"(\"abc_cmnt\", \"abc_tnam\", \"abc_ownr\", \"abc_cnam\" ) VALUES( ?, ?, \"\", ? );";
    res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query2.c_str() ).c_str(), (int) query2.length(), &stmt, 0 );
    if( res == SQLITE_OK )
    {
        res = sqlite3_bind_text( stmt, 1, sqlite_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
        if( res == SQLITE_OK )
        {
            res = sqlite3_bind_text( stmt, 2, sqlite_pimpl->m_myconv.to_bytes( comment.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
            if( res == SQLITE_OK )
            {
                res = sqlite3_step( stmt );
                if( res != SQLITE_DONE )
                {
                    GetErrorMessage( res, errorMessage );
                    errorMsg.push_back( errorMessage );
                }
            }
            else
            {
                GetErrorMessage( res, errorMessage );
                errorMsg.push_back( errorMessage );
            }
        }
        else
        {
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
        }
    }
    else
    {
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
    }
}

bool SQLiteDatabase::IsIndexExists(const std::wstring &indexName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    bool exists = false;
    int res = SQLITE_OK, result = 0;
    sqlite3_stmt *stmt = NULL;
    std::wstring errorMessage, dbIndexName;
    std::string query = "PRAGMA index_list( \"%w\" );";
    char *z = sqlite3_mprintf( query.c_str(), tableName.c_str() );
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
                GetErrorMessage( res, errorMessage );
                errorMsg.push_back( errorMessage );
                break;
            }
        }
    }
    else
    {
        result = 1;
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
    }
    sqlite3_finalize( stmt );
    return exists;
}

int SQLiteDatabase::GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg)
{
    sqlite3_stmt *stmt = NULL;
    std::wstring errorMessage;
    int result = 0;
    std::wstring query = L"SELECT * FROM \"sys.abcattbl\" WHERE \"abt_tnam\" = ? AND \"abt_ownr\" = ?;";
    const unsigned char *dataFontName, *headingFontName, *labelFontName;
    int res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), (int) query.length(), &stmt, 0 );
    if( res == SQLITE_OK )
    {
        res = sqlite3_bind_text( stmt, 1, sqlite_pimpl->m_myconv.to_bytes( table->GetTableName().c_str() ).c_str(), -1, SQLITE_TRANSIENT );
        if( res == SQLITE_OK )
        {
            res = sqlite3_bind_text( stmt, 2, sqlite_pimpl->m_myconv.to_bytes( table->GetSchemaName().c_str() ).c_str(), -1, SQLITE_TRANSIENT );
            if( res == SQLITE_OK )
            {
                while( true )
                {
                    res = sqlite3_step( stmt );
                    if( res == SQLITE_ROW )
                    {
                        table->SetDataFontSize( sqlite3_column_int( stmt, 3 ) );
                        table->SetDataFontWeight( sqlite3_column_int( stmt, 4 ) );
                        char *italic = (char *) sqlite3_column_text( stmt, 5 );
                        if( italic )
                            table->SetDataFontItalic( italic[0] == 'Y' );
                        char *underline = (char *) sqlite3_column_text( stmt, 6 );
                        if( underline )
                            table->SetDataFontUnderline( underline[0] == 'Y' );
                        dataFontName = (const unsigned char *) sqlite3_column_text( stmt, 9 );
                        if( dataFontName )
                            table->SetDataFontName( sqlite_pimpl->m_myconv.from_bytes( (const char *) dataFontName ) );
                        table->SetHeadingFontSize( sqlite3_column_int( stmt, 10 ) );
                        table->SetHeadingFontWeight( sqlite3_column_int( stmt, 11 ) );
                        italic = (char *) sqlite3_column_text( stmt, 12 );
                        if( italic )
                            table->SetHeadingFontItalic( italic[0] == 'Y' );
                        underline = (char *) sqlite3_column_text( stmt, 13 );
                        if( underline )
                            table->SetHeadingFontUnderline( underline[0] == 'Y' );
                        headingFontName = (const unsigned char *) sqlite3_column_text( stmt, 16 );
                        if( headingFontName )
                            table->SetHeadingFontName( sqlite_pimpl->m_myconv.from_bytes( (const char *) dataFontName ) );
                        table->SetLabelFontSize( sqlite3_column_int( stmt, 17 ) );
                        table->SetLabelFontWeight( sqlite3_column_int( stmt, 18 ) );
                        italic = (char *) sqlite3_column_text( stmt, 19 );
                        if( italic )
                            table->SetLabelFontItalic( italic[0] == 'Y' );
                        underline = (char *) sqlite3_column_text( stmt, 20 );
                        if( underline )
                            table->SetLabelFontUnderline( underline[0] == 'Y' );
                        labelFontName = (const unsigned char *) sqlite3_column_text( stmt, 23 );
                        if( labelFontName )
                            table->SetLabelFontName( sqlite_pimpl->m_myconv.from_bytes( (const char *) dataFontName ) );
                        table->SetComment( sqlite_pimpl->m_myconv.from_bytes( (const char *) sqlite3_column_text( stmt, 24 ) ) );
                    }
                    else if( res == SQLITE_DONE )
                        break;
                    else
                    {
                        result = 1;
                        GetErrorMessage( res, errorMessage );
                        errorMsg.push_back( errorMessage );
                    }
                }
            }
            else
            {
                result = 1;
                GetErrorMessage( res, errorMessage );
                errorMsg.push_back( errorMessage );
            }
        }
        else
        {
            result = 1;
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
        }
    }
    else
    {
        result = 1;
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
    }
    sqlite3_finalize( stmt );
    return result;
}

int SQLiteDatabase::SetTableProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    std::wstring errorMessage;
    sqlite3_stmt *stmt = NULL;
    int result = 0;
    int res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( command.c_str() ).c_str(), (int) command.length(), &stmt, 0 );
    if( res == SQLITE_OK )
    {
        res = sqlite3_step( stmt );
        if( res != SQLITE_DONE )
        {
            result = 1;
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
        }
    }
    else
    {
        result = 1;
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
    }
    sqlite3_finalize( stmt );
    return result;
}

bool SQLiteDatabase::IsTablePropertiesExist(const std::wstring &tableName, const std::wstring &schemaName, std::vector<std::wstring> &errorMsg)
{
    bool result = false;
    sqlite3_stmt *stmt = NULL;
    std::wstring errorMessage;
    std::wstring query = L"SELECT 1 FROM \"sys.abcattbl\" WHERE \"abt_tnam\" = ? AND \"abt_ownr\" = '';";
    int res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), (int) query.length(), &stmt, 0 );
    if( res == SQLITE_OK )
    {
        res = sqlite3_bind_text( stmt, 1, sqlite_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
        if( res == SQLITE_OK )
        {
            res = sqlite3_step( stmt );
            if( res == SQLITE_ROW )
                result = true;
            else if( res != SQLITE_DONE )
            {
                GetErrorMessage( res, errorMessage );
                errorMsg.push_back( errorMessage );
            }
        }
        else
        {
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
        }
    }
    else
    {
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
    }
    sqlite3_finalize( stmt );
    return result;
}

int SQLiteDatabase::GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &fieldName, Field *table, std::vector<std::wstring> &errorMsg)
{
    sqlite3_stmt *stmt;
    std::wstring errorMessage;
    int result = 0;
    std::wstring query = L"SELECT * FROM \"sys.abcatcol\" WHERE \"abc_tnam\" = ? AND \"abc_ownr\" = ? AND \"abc_cnam\" = ?;";
    int res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), (int) query.length(), &stmt, 0 );
    if( res == SQLITE_OK )
    {
        res = sqlite3_bind_text( stmt, 1, sqlite_pimpl->m_myconv.to_bytes( tableName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
        if( res == SQLITE_OK )
        {
            res = sqlite3_bind_text( stmt, 2, sqlite_pimpl->m_myconv.to_bytes( schemaName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
            if( res == SQLITE_OK )
            {
                res = sqlite3_bind_text( stmt, 3, sqlite_pimpl->m_myconv.to_bytes( fieldName.c_str() ).c_str(), -1, SQLITE_TRANSIENT );
                if( res == SQLITE_OK )
                {
                    res = sqlite3_step( stmt );
                    if( res == SQLITE_ROW )
                    {
                        table->SetComment( sqlite_pimpl->m_myconv.from_bytes( (const char *) sqlite3_column_text( stmt, 17 ) ) );
                    }
                    else if( res != SQLITE_DONE )
                    {
                        result = 1;
                        GetErrorMessage( res, errorMessage );
                        errorMsg.push_back( errorMessage );
                    }
                }
                else
                {
                    result = 1;
                    GetErrorMessage( res, errorMessage );
                    errorMsg.push_back( errorMessage );
                }
            }
            else
            {
                result = 1;
                GetErrorMessage( res, errorMessage );
                errorMsg.push_back( errorMessage );
            }
        }
        else
        {
            result = 1;
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
        }
    }
    else
    {
        result = 1;
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
    }
    sqlite3_finalize( stmt );
    return result;
}

int SQLiteDatabase::ApplyForeignKey(const std::wstring &command, DatabaseTable &tableName, std::vector<std::wstring> &errorMsg)
{
    sqlite3_stmt *stmt = NULL;
    std::wstring errorMessage;
    char **error;
    std::vector<std::wstring> references;
    std::wstring query0 = L"PRAGMA foreign_keys=OFF", query1 = L"PRAGMA foreign_keys=ON";
    std::wstring query = L"SELECT type, sql FROM sqlite_master WHERE tbl_name = ";
    query += tableName.GetTableName();
    query += L" AND type <> 'table'";
    int result = 0;
    int res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( query.c_str() ).c_str(), query.length(), &stmt, NULL );
    if( res != SQLITE_OK )
    {
        result = 1;
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
        return result;
    }
    while( true )
    {
        res = sqlite3_step( stmt );
        if( res == SQLITE_ROW )
            references.push_back( sqlite_pimpl->m_myconv.from_bytes( (const char *) sqlite3_column_text( stmt, 1 ) ) );
        else if( res == SQLITE_DONE )
            break;
		else
        {
            result = 1;
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
            break;
        }
    }
    sqlite3_finalize( stmt );
    if( res != SQLITE_DONE )
        return 1;
    res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( query0.c_str() ).c_str(), NULL, NULL, error );
    if( res != SQLITE_OK )
    {
        result = 1;
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
        return result;
    }
    std::wstring alterQuery = command.substr( 0, command.find( ';' ) );
    while( alterQuery != L"" )
    {
        res = sqlite3_prepare_v2( m_db, sqlite_pimpl->m_myconv.to_bytes( alterQuery.c_str() ).c_str(), alterQuery.length(), &stmt, NULL );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
            return result;
        }
        while( true )
        {
            res = sqlite3_step( stmt );
            if( res == SQLITE_ROW )
                references.push_back( sqlite_pimpl->m_myconv.from_bytes( (const char *) sqlite3_column_text( stmt, 1 ) ) );
            else if( res == SQLITE_DONE )
                break;
		    else
            {
                result = 1;
                GetErrorMessage( res, errorMessage );
                errorMsg.push_back( errorMessage );
                break;
            }
        }
        sqlite3_finalize( stmt );
        if( res != SQLITE_DONE )
            break;
        alterQuery = alterQuery.substr( 2 );
        alterQuery = alterQuery.substr( 0, alterQuery.find( ';' ) );
    }
    if( res != SQLITE_DONE )
    {
        sqlite3_exec( m_db, "ROLLBACK", NULL, NULL, error );
        return 1;
    }
    for( std::vector<std::wstring>::iterator it = references.begin(); it < references.end(); it++ )
    {
        res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( (*it).c_str() ).c_str(), NULL, NULL, error );
        if( res != SQLITE_OK )
            break;
    }
    if( res != SQLITE_OK )
    {
        sqlite3_exec( m_db, "ROLLBACK", NULL, NULL, error );
        return 1;
    }
    res = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( query1.c_str() ).c_str(), NULL, NULL, error );
    if( res != SQLITE_OK )
    {
        result = 1;
        sqlite3_exec( m_db, "ROLLBACK", NULL, NULL, error );
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
    }
	else
        sqlite3_exec( m_db, "COMMIT", NULL, NULL, error );
    return result;
}

int SQLiteDatabase::DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    char *error;
    std::wstring err;
    std::wstring query = L"DROP TABLE ";
    query += tableName;
    int result = sqlite3_exec( m_db, sqlite_pimpl->m_myconv.to_bytes( query ).c_str(), 0, 0, &error );
    if( result != SQLITE_OK )
    {
        res = 1;
        GetErrorMessage( res, err );
        errorMsg.push_back( err );
    }
    return res;
}

int SQLiteDatabase::SetFieldProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg)
{
    int res = 0;
    return res;
}

#ifdef WIN32
#include <windows.h>
#pragma execution_character_set("utf-8")
#endif

#include <map>
#include <vector>
#include <string.h>
#include <string>
#include <locale>
#include <codecvt>
#include "sqlite3.h"
#include "database.h"
#include "database_sqlite.h"

SQLiteDatabase::SQLiteDatabase() : Database()
{
    m_db = NULL;
    pimpl = new Impl;
}

SQLiteDatabase::~SQLiteDatabase()
{
}

int SQLiteDatabase::Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring errorMessage;
    std::wstring_convert<std::codecvt_utf8<wchar_t> > myconv;
    int res = sqlite3_open( myconv.to_bytes( selectedDSN.c_str() ).c_str(), &m_db );
    if( res != SQLITE_OK )
    {
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
        result = 1;
    }
    else
    {
        m_catalog = selectedDSN;
        GetTableListFromDb( errorMsg );
        res = sqlite3_exec( m_db, "PRAGMA foreign_keys = ON", NULL, NULL, NULL );
        if( res != SQLITE_OK )
        {
            result = 1;
            GetErrorMessage( res, errorMessage );
            errorMsg.push_back( errorMessage );
        }
    }
    return result;
}

int SQLiteDatabase::Disconnect(std::vector<std::wstring> &errorMsg)
{
    int result = 0;
    std::wstring errorMessage;
    int res = sqlite3_close( m_db );
    if( res != SQLITE_OK )
    {
        GetErrorMessage( res, errorMessage );
        errorMsg.push_back( errorMessage );
        result = 1;
    }
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
    std::vector<Field> fields;
    std::map<int,std::vector<FKField> > foreign_keys;
    std::wstring errorMessage;
    sqlite3_stmt *stmt = NULL, *stmt2 = NULL, *stmt3 = NULL;
    std::string fieldName, fieldType, fieldDefaultValue, fkTable, fkField, fkTableField, fkUpdateConstraint, fkDeleteConstraint;
    int result = 0, res = SQLITE_OK, res1, res2 = SQLITE_OK, fieldIsNull, fieldPK, fkReference, autoinc;
    FK_ONUPDATE update_constraint = NO_ACTION_UPDATE;
    FK_ONDELETE delete_constraint = NO_ACTION_DELETE;
    std::string query1 = "SELECT name FROM sqlite_master WHERE type = ?";
    std::string query2 = "PRAGMA table_info(\"%w\");";
    std::string query3 = "PRAGMA foreign_key_list(\"%w\")";
    std::wstring_convert<std::codecvt_utf8<wchar_t> > myconv;
    if( ( res = sqlite3_prepare_v2( m_db, query1.c_str(), query1.length(), &stmt, 0 ) ) == SQLITE_OK )
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
                                fields.push_back( Field( myconv.from_bytes( fieldName ), myconv.from_bytes( fieldType ), 0, 0, myconv.from_bytes( fieldDefaultValue ), fieldIsNull == 0 ? false: true, autoinc == 1 ? true : false, fieldPK == 1 ? true : false ) );
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
                        if( res1 != SQLITE_DONE )
                            break;
                    }
                    else
                    {
                        result = 1;
                        GetErrorMessage( res, errorMessage );
                        errorMsg.push_back( errorMessage );
                    }
                    sqlite3_reset( stmt2 );
                    if( res1 == SQLITE_DONE )
                    {
                        char *y = sqlite3_mprintf( query3.c_str(), tableName );
                        res2 = sqlite3_prepare( m_db, y, -1, &stmt3, 0 );
                        if( res2 == SQLITE_OK )
                        {
                            for( ; ; )
                            {
                                res2 = sqlite3_step( stmt3 );
                                if( res2 == SQLITE_ROW )
                                {
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
                                    foreign_keys[fkReference].push_back( FKField( myconv.from_bytes( fkTable ), myconv.from_bytes( fkField ), myconv.from_bytes( fkTableField ), L"", update_constraint, delete_constraint ) );
                                }
                                else if( res2 == SQLITE_DONE )
                                    break;
                                else
                                {
                                    result = 1;
                                    GetErrorMessage( res, errorMessage );
                                    errorMsg.push_back( errorMessage );
                                    break;
                                }
                            }
                            if( res2 != SQLITE_DONE )
                                break;
                        }
                        else
                        {
                            result = 1;
                            GetErrorMessage( res, errorMessage );
                            errorMsg.push_back( errorMessage );
                        }
                        sqlite3_reset( stmt3 );
                    }
                    if( res1 == SQLITE_DONE && res2 == SQLITE_DONE )
                        pimpl->m_tables[m_catalog].push_back( Table( myconv.from_bytes( (const char *) tableName ), fields, foreign_keys ) );
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
    sqlite3_finalize( stmt2 );
    sqlite3_finalize( stmt3 );
    return result;
}

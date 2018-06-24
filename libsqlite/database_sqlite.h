#ifndef DBMANAGER_SQLITE
#define DBMANAGER_SQLITE

#ifdef WIN32
class __declspec(dllexport) SQLiteDatabase : public Database
#else
class SQLiteDatabase : public Database
#endif
{
public:
    SQLiteDatabase();
    virtual ~SQLiteDatabase();
    virtual int Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg);
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg);
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg);
    virtual int Disconnect(std::vector<std::wstring> &errorMsg);
    virtual int CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg);
    virtual int GetFieldProperties(const char *tableName, const char *schemaName, const char *ownerName, const char *fieldName, Field *field, std::vector<std::wstring> &errorMsg);
    virtual int SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual int SetFieldProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual int ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &foreignKeyFields, const std::wstring &refTableName, const std::vector<std::wstring> &refKeyFields, int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int match, std::vector<std::wstring> &errorMsg);
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    virtual int NewTableCreation(std::vector<std::wstring> &errorMsg);
protected:
    struct SQLiteImpl;
    SQLiteImpl *sqlite_pimpl;
    void GetErrorMessage(int code, std::wstring &errorMsg);
    void SetFullType(Field *field, const std::wstring &type);
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg);
    virtual bool IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg);
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    virtual int GetServerVersion(std::vector<std::wstring> &errorMsg);
    virtual int ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg);
    int DropForeignKey(DatabaseTable &tableName, std::vector<FKField *> &newFK, const std::wstring &sql, std::wstring &newSQL, const std::wstring &refTableName);
private:
    sqlite3 *m_db;
    sqlite3_stmt *m_stmt1, *m_stmt2;
    int m_schema;
};

struct SQLiteDatabase::SQLiteImpl
{
    std::wstring m_catalog;
    std::wstring_convert<std::codecvt_utf8<wchar_t> > m_myconv;
};

#endif
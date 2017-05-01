#ifndef DBMANAGER_SQLITE
#define DBMANAGER_SQLITE

#ifdef WIN32
class __declspec(dllexport) PostgresDatabase : public Database
#else
class PostgresDatabase : public Database
#endif
{
public:
    PostgresDatabase();
    virtual ~PostgresDatabase();
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg);
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg);
    virtual int Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg);
    virtual int Disconnect(std::vector<std::wstring> &errorMsg);
    virtual int CreateIndex(const std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg);
    virtual int GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &fieldName, Field *table, std::vector<std::wstring> &errorMsg);
    virtual int SetTableProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual int SetFieldProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual bool IsTablePropertiesExist(const std::wstring &tableName, const std::wstring &schemaName, std::vector<std::wstring> &errorMsg);
    virtual int ApplyForeignKey(const std::wstring &command, DatabaseTable &tableName, std::vector<std::wstring> &errorMsg);
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
protected:
//    struct SQLiteImpl;
//    SQLiteImpl *sqlite_pimpl;
    void GetErrorMessage(int code, std::wstring &errorMsg);
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg);
    virtual void SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg);
private:
    PGConn *m_db;
};
/*
struct SQLiteDatabase::SQLiteImpl
{
    std::wstring m_catalog;
    std::wstring_convert<std::codecvt_utf8<wchar_t> > m_myconv;
};
*/
#endif
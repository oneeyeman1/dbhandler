#ifndef DBMANAGER_POSTGRES
#define DBMANAGER_POSTGRES

#ifdef WIN32
class __declspec(dllexport) PostgresDatabase : public Database
#else
class PostgresDatabase : public Database
#endif
{
public:
    PostgresDatabase();
    virtual ~PostgresDatabase();
    virtual int Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg);
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg);
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg);
    virtual int Disconnect(std::vector<std::wstring> &UNUSED(errorMsg));
    virtual int CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg);
    virtual int GetFieldProperties(const char *tableName, const char *schemaName, const char *ownerName, const char *fieldName, Field *field, std::vector<std::wstring> &errorMsg);
    virtual int SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual int SetFieldProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual int ApplyForeignKey(const std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, std::vector<std::wstring> &errorMsg);
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    int CreateSystemObjectsAndGetDatabaseInfo(std::vector<std::wstring> &errorMsg);
protected:
    struct PostgresImpl;
    PostgresImpl *m_pimpl;
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg);
    virtual bool IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg);
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    virtual int GetServerVersion(std::vector<std::wstring> &errorMsg);
    int CreateIndexesOnPostgreConnection(std::vector<std::wstring> &errorMsg);
    virtual int ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg);
private:
    PGconn *m_db;
};

struct PostgresDatabase::PostgresImpl
{
    std::wstring_convert<std::codecvt_utf8<wchar_t> > m_myconv;
};
#endif
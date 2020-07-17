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
    virtual int GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &ownerName, const std::wstring &fieldName, Field *field, std::vector<std::wstring> &errorMsg);
    virtual int GetFieldProperties(const std::wstring &table, Field *field, std::vector<std::wstring> &errorMsg);
    virtual int SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual int SetFieldProperties(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, const Field *field, bool isLogOnly, std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual int ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &foreignKeyFields, const std::wstring &refTableName, const std::vector<std::wstring> &refKeyFields, int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int match, std::vector<std::wstring> &errorMsg);
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    int CreateSystemObjectsAndGetDatabaseInfo(std::vector<std::wstring> &errorMsg);
    virtual int NewTableCreation(std::vector<std::wstring> &errorMsg);
    int GetTableOwner(const std::wstring &schemaName, const std::wstring &tableName, std::wstring &owner, std::vector<std::wstring> &errorMsg);
    virtual int GetFieldHeader(const std::wstring &tabeName, const std::wstring &fieldName, std::wstring &headerStr);
protected:
    struct PostgresImpl;
    PostgresImpl *m_pimpl;
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg);
    virtual bool IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg);
    virtual bool IsFieldPropertiesExist(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, std::vector<std::wstring> &errorMsg);
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    virtual int GetServerVersion(std::vector<std::wstring> &errorMsg);
    int CreateIndexesOnPostgreConnection(std::vector<std::wstring> &errorMsg);
    virtual int ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg);
    virtual int DropForeignKey(std::wstring &command, const DatabaseTable &tableName, const std::wstring &keyName, bool logOnly, std::vector<std::wstring> &errorMsg);
    virtual int AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, const std::wstring &ownerName, int tableId, bool tableAdded, std::vector<std::wstring> &errorMsg);
private:
    PGconn *m_db;
    bool m_isConnected;
};

struct PostgresDatabase::PostgresImpl
{
    std::wstring_convert<std::codecvt_utf8<wchar_t> > m_myconv;
};
#endif
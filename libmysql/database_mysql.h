#ifndef DBMANAGER_MYSQL
#define DBMANAGER_MYSQL

#ifdef WIN32
class __declspec(dllexport) MySQLDatabase : public Database
#else
class MySQLDatabase : public Database
#endif
{
public:
    MySQLDatabase();
    virtual ~MySQLDatabase();
    virtual int Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) override;
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) override;
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) override;
    int mySQLConnect();
    virtual int Disconnect(std::vector<std::wstring> &errorMsg) override;
    virtual int CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg) override;
    virtual int GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &ownerName, const std::wstring &fieldName, TableField *field, std::vector<std::wstring> &errorMsg) override;
    virtual int GetFieldProperties(const std::wstring &table, TableField *field, std::vector<std::wstring> &errorMsg) override;
    virtual int SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg) override;
    virtual int SetFieldProperties(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, const FieldProperties &prop, bool isLogOnly, std::wstring &command, std::vector<std::wstring> &errorMsg) override;
    virtual int ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &foreignKeyFields, const std::wstring &refTableName, const std::vector<std::wstring> &refKeyFields, int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int match, std::vector<std::wstring> &errorMsg) override;
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    int CreateSystemObjectsAndGetDatabaseInfo(std::vector<std::wstring> &errorMsg);
    virtual int NewTableCreation(std::vector<std::wstring> &errorMsg) override;
    virtual int GetFieldHeader(const std::wstring &tabeName, const std::wstring &fieldName, std::wstring &headerStr) override;
    virtual int PrepareStatement(const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    virtual int EditTableData(std::vector<DataEditFiield> &row, std::vector<std::wstring> &errorMsg) override;
    virtual int FinalizeStatement(std::vector<std::wstring> &errorMsg) override;
    virtual int GetTableCreationSyntax(const std::wstring tableName, std::wstring &syntax, std::vector<std::wstring> &errorMsg) override;
    virtual int AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errors) override;
    virtual int AttachDatabase(const std::wstring &catalog, const std::wstring &schema, std::vector<std::wstring> &errorMsg) override;
protected:
    struct MySQLImpl;
    MySQLImpl *m_pimpl;
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg) override;
    virtual bool IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg) override;
    virtual bool IsFieldPropertiesExist(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, std::vector<std::wstring> &errorMsg) override;
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    bool IsSystemIndexExists(MYSQL_STMT *res, const std::wstring &indexName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    int TokenizeConnectionString(const std::wstring &connectStr, std::vector<std::wstring> &errorMsg);
    virtual int GetServerVersion(std::vector<std::wstring> &errorMsg) override;
    virtual int ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) override;
    virtual int DropForeignKey(std::wstring &command, const DatabaseTable &tableName, const std::wstring &keyName, bool logOnly, std::vector<std::wstring> &errorMsg) override;
    virtual int AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, const std::wstring &tableOwner, long tableId, bool tableAdded, std::vector<std::wstring> &errorMsg) override;
private:
    MYSQL *m_db;
    int m_port, m_flags;
};

struct MySQLDatabase::MySQLImpl
{
    std::wstring_convert<std::codecvt_utf8<wchar_t> > m_myconv;
    std::wstring m_host, m_user, m_password, m_dbName, m_socket, m_catalog;
};
#endif

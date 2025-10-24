#ifndef DBMANAGER_POSTGRES
#define DBMANAGER_POSTGRES

#ifdef WIN32
class __declspec(dllexport) PostgresDatabase : public Database
#else
class PostgresDatabase : public Database
#endif
{
public:
    PostgresDatabase(const int osId, const std::wstring &desktop);
    virtual ~PostgresDatabase();
    virtual int Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) override;
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) override;
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) override;
    virtual int Disconnect(std::vector<std::wstring> &UNUSED(errorMsg)) override;
    virtual int CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &catalogName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    virtual int DropIndex(const std::wstring &fullTableName, const std::wstring &indexName, const DropIndexOption &options, std::vector<std::wstring> &errorMsg) override;
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg) override;
    virtual int GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &ownerName, const std::wstring &fieldName, TableField *field, std::vector<std::wstring> &errorMsg) override;
    virtual int GetFieldProperties(const std::wstring &table, TableField *field, std::vector<std::wstring> &errorMsg) override;
    virtual int SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg) override;
    virtual int SetFieldProperties(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, const FieldProperties &prop, bool isLogOnly, std::wstring &command, std::vector<std::wstring> &errorMsg) override;
    virtual int ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &foreignKeyFields, const std::wstring &refTableName, const std::vector<std::wstring> &refKeyFields, int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int match, std::vector<std::wstring> &errorMsg) override;
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    int CreateSystemObjectsAndGetDatabaseInfo(std::vector<std::wstring> &errorMsg);
    virtual int NewTableCreation(std::vector<std::wstring> &errorMsg) override;
    int GetTableOwner(const std::wstring &schemaName, const std::wstring &tableName, std::wstring &owner, std::vector<std::wstring> &errorMsg);
    virtual int GetFieldHeader(const std::wstring &tabeName, const std::wstring &fieldName, std::wstring &headerStr, std::vector<std::wstring> &errorMsg) override;
    virtual int PrepareStatement(const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    virtual int EditTableData(std::vector<DataEditFiield> &row, std::vector<std::wstring> &errorMsg) override;
    virtual int FinalizeStatement(std::vector<std::wstring> &errorMsg) override;
    virtual int GetTableCreationSyntax(const std::wstring tableName, std::wstring &syntax, std::vector<std::wstring> &errorMsg) override;
    virtual int AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, bool tableAdded, std::vector<std::wstring> &errors) override;
    virtual int AttachDatabase(const std::wstring &catalog, const std::wstring &schema, std::vector<std::wstring> &errorMsg) override;
    virtual int GetDatabaseNameList(std::vector<std::wstring> &names, std::vector<std::wstring> &errorMsg) override;
    virtual int GetQueryRow(const std::wstring &query, std::vector<std::wstring> &values) override;
    virtual int AddUpdateFormat() override;
    virtual int CreateUpdateValidationRule(bool isNew, const std::wstring &name, const std::wstring &rule, const int type, const std::wstring &message, std::vector<std::wstring> &errorMsg) override;
    virtual int GetTablespacesList(std::vector<std::wstring> &list, std::vector<std::wstring> &errorMsg)  override;
    virtual int GetTableFields(const std::wstring &catalog, const std::wstring &schema, const std::wstring &table, std::vector<std::wstring> &fields, std::vector<std::wstring> &errors) override;
    virtual int EditPrimaryKey(const std::wstring &tableName, const std::vector<std::wstring> &newKey, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg);
protected:
    struct PostgresImpl;
    PostgresImpl *m_pimpl;
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg) override;
    virtual bool IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg) override;
    virtual bool IsFieldPropertiesExist(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, std::vector<std::wstring> &errorMsg) override;
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &catalogName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    virtual int GetServerVersion(std::vector<std::wstring> &errorMsg) override;
    virtual int ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) override;
    virtual int DropForeignKey(std::wstring &command, DatabaseTable *tableName, const std::wstring &keyName, bool logOnly, const std::vector<FKField *> &fkFields, std::vector<std::wstring> &errorMsg) override;
    virtual int PopulateValdators(std::vector<std::wstring> &errorMsg) override;
private:
    PGconn *m_db;
    PGresult *queryRes;
};

struct PostgresDatabase::PostgresImpl
{
    std::wstring_convert<std::codecvt_utf8<wchar_t> > m_myconv;
    std::vector<std::wstring> m_tablespaces;
};
#endif

#ifndef DBMANAGER_SQLITE
#define DBMANAGER_SQLITE

#ifdef WIN32
class __declspec(dllexport) SQLiteDatabase : public Database
#else
class SQLiteDatabase : public Database
#endif
{
public:
    SQLiteDatabase(const int osId, const std::wstring &desktop);
    virtual ~SQLiteDatabase();
    virtual int Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) override;
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) override;
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) override;
    virtual int Disconnect(std::vector<std::wstring> &errorMsg) override;
    virtual int CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &catalogName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg) override;
    virtual int GetFieldProperties(const std::wstring &tableName, const std::wstring &UNUSED(schemaName), const std::wstring &ownerName, const std::wstring &fieldName, TableField *field, std::vector<std::wstring> &errorMsg) override;
    virtual int GetFieldProperties(const std::wstring &table, TableField *field, std::vector<std::wstring> &errorMsg) override;
    virtual int SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg) override;
    virtual int SetFieldProperties(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, const FieldProperties &field, bool isLogOnly, std::wstring &command, std::vector<std::wstring> &errorMsg) override;
    virtual int ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &foreignKeyFields, const std::wstring &refTableName, const std::vector<std::wstring> &refKeyFields, int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int UNUSED(match), std::vector<std::wstring> &errorMsg) override;
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    virtual int NewTableCreation(std::vector<std::wstring> &errorMsg) override;
    virtual int DropForeignKey(std::wstring &command, const DatabaseTable &tableName, const std::wstring &keyName, bool logOnly, std::vector<std::wstring> &errorMsg) override;
    virtual int GetFieldHeader(const std::wstring &tabeName, const std::wstring &fieldName, std::wstring &headerStr, std::vector<std::wstring> &errorMsg) override;
    virtual int PrepareStatement(const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    virtual int EditTableData(std::vector<DataEditFiield> &row, std::vector<std::wstring> &errorMsg) override;
    virtual int FinalizeStatement(std::vector<std::wstring> &errorMsg) override;
    int GetAttachedDBList(std::vector<std::wstring> &dbNames, std::vector<std::wstring> &errorMsg);
    virtual int GetTableCreationSyntax(const std::wstring tableName, std::wstring &syntax, std::vector<std::wstring> &errorMsg) override;
    virtual int AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errors) override;
    virtual int AttachDatabase(const std::wstring &catalog, const std::wstring &schema, std::vector<std::wstring> &errorMsg) override;
    virtual int GetDatabaseNameList(std::vector<std::wstring> &names, std::vector<std::wstring> &errorMsg) override;
    virtual int GetQueryRow(const std::wstring &query, std::vector<std::wstring> &values) override;
    virtual int AddUpdateFormat() override;
protected:
    struct SQLiteImpl;
    SQLiteImpl *sqlite_pimpl;
    void GetErrorMessage(int code, std::wstring &errorMsg);
    void SetFullType(TableField *field, const std::wstring &type);
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg) override;
    virtual bool IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg) override;
    virtual bool IsFieldPropertiesExist(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, std::vector<std::wstring> &errorMsg) override;
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &catalogName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    virtual int GetServerVersion(std::vector<std::wstring> &errorMsg) override;
    virtual int ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) override;
    int DropForeignKey(DatabaseTable &tableName, std::vector<FKField *> &newFK, const std::wstring &sql, std::wstring &newSQL, const std::wstring &refTableName);
    virtual int AddDropTable(const std::wstring &UNUSED(catalog), const std::wstring &schemaName, const std::wstring &tableName, const std::wstring &UNUSED(ownerName), long UNUSED(tableId), bool tableAdded, std::vector<std::wstring> &errorMsg) override;
private:
    sqlite3 *m_db;
    sqlite3_stmt *m_stmt1, *m_stmt2, *m_stmt3, *m_stmt;
    int m_schema;
};

struct SQLiteDatabase::SQLiteImpl
{
    std::wstring m_catalog;
    std::wstring_convert<std::codecvt_utf8<wchar_t> > m_myconv;
};

#endif

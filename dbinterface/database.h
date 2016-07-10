#include <map>
#include <vector>

#ifndef DBMANAGER_DATABASE
#define DBMANAGER_DATABASE

enum FK_ONUPDATE
{
    NO_ACTION_UPDATE,
    RESTRICT_UPDATE,
    SET_NULL_UPDATE,
    SET_DEFAULT_UPDATE,
    CASCADE_UPDATE
};

enum FK_ONDELETE
{
    NO_ACTION_DELETE,
    RESTRICT_DELETE,
    SET_NULL_DELETE,
    SET_DEFAULT_DELETE,
    CASCADE_DELETE
};

class Field
{
public:
    Field(const std::wstring &columnName, const std::wstring &columnType, int size, int decimalsize, const std::wstring &columnDefaultValue = L"", const bool columnIsNull = false, bool autoincrement = false, const bool columnPK = false)
    {
        column_name = columnName;
        column_type = columnType;
        field_size = size;
        decimal_size = decimalsize;
        column_defaultValue = columnDefaultValue;
        column_isNull = columnIsNull;
        autoIncrement = autoincrement;
        column_pk = columnPK;
    }
private:
    std::wstring column_name, column_type, column_defaultValue;
    bool autoIncrement;
    bool column_isNull, column_pk;
    int field_size, decimal_size;
};

struct FKField
{
    FKField(const std::wstring &table_name, const std::wstring &original_field, const std::wstring &referenced_field, const std::wstring &schema, FK_ONUPDATE update_constraint, FK_ONDELETE delete_constraint)
    {
        schemaName = schema;
        tableName = table_name;
        originalField = original_field;
        referencedField = referenced_field;
        updateConstraint = update_constraint;
        deleteConstraint = delete_constraint;
    }
    std::wstring schemaName, tableName, originalField, referencedField;
    FK_ONUPDATE updateConstraint;
    FK_ONDELETE deleteConstraint;
};

class Table
{
public:
    Table(const std::wstring &tableName, const std::vector<Field> &tableFields, const std::map<int,std::vector<FKField> > &foreignKeys)
    {
        table_name = tableName;
        table_fields = tableFields;
        foreign_keys = foreignKeys;
    }
	const std::wstring &GetTableName() { return table_name; }
	std::map<int,std::vector<FKField> > &GetForeignKeyVector() { return foreign_keys; }
private:
    std::wstring table_name;
    std::vector<Field> table_fields;
    std::map<int,std::vector<FKField> > foreign_keys;
};

#ifdef WIN32
class __declspec(dllexport) Database
#else
class Database
#endif
{
protected:
    struct Impl;
    Impl *pimpl;
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg) = 0;
public:
    virtual ~Database() = 0;
    Impl &GetTableVector() { return *pimpl; };
    virtual int Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg) = 0;
    virtual int Disconnect(std::vector<std::wstring> &errorMsg) = 0;
};

struct Database::Impl
{
    std::map<std::wstring, std::vector<Table> > m_tables;
    std::wstring m_dbName, m_type, m_subtype;
};

inline Database::~Database()
{
    delete pimpl;
}

#endif

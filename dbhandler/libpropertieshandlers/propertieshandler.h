#pragma once
class WXEXPORT DatabasePropertiesHandler : public PropertiesHandler
{
public:
    DatabasePropertiesHandler(const Database *db, DatabaseTable *table, wxTextCtrl *log);
    virtual ~DatabasePropertiesHandler() { }
    virtual int GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
    TableProperties &GetProperty() { return m_prop; }
    virtual const std::wstring &GetCommand() const wxOVERRIDE { return m_command; }
    DatabaseTable *GetTable() { return m_table; }
private:
    wxTextCtrl *m_log;
    DatabaseTable *m_table;
    TableProperties m_prop;
    const Database *m_db;
    std::wstring m_command;
};


#pragma once
class WXEXPORT DatabasePropertiesHandler : public PropertiesHandler
{
public:
    DatabasePropertiesHandler(const Database *db, wxTextCtrl *log);
    virtual ~DatabasePropertiesHandler() { }
    virtual wxAny GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
    virtual int ApplyProperties() wxOVERRIDE;
    virtual const std::wstring &GetCommand() const wxOVERRIDE { return m_command; }
private:
    wxTextCtrl *m_log;
    TableProperties m_prop;
    const Database *m_db;
    std::wstring m_command;
};


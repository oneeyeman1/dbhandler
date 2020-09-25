#pragma once
class WXEXPORT DatabasePropertiesHandler : public PropertiesHandler
{
public:
    DatabasePropertiesHandler(const Database *db, DatabaseTable *table, wxTextCtrl *log);
    virtual ~DatabasePropertiesHandler() { }
    virtual void EditProperies(wxNotebook *parent) wxOVERRIDE;
    virtual int GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
    TableProperties &GetProperty() { return m_prop; }
    virtual const std::wstring &GetCommand() const wxOVERRIDE { return m_command; }
    virtual bool IsLogOnly() const wxOVERRIDE { return m_page1->IsLogOnly(); }

private:
    wxTextCtrl *m_log;
    DatabaseTable *m_table;
    TableProperties m_prop;
    TableGeneralProperty *m_page1;
    CFontPropertyPage *m_page2, *m_page3, *m_page4;
    TablePrimaryKey *m_page5;
    const Database *m_db;
    std::wstring m_command;
};


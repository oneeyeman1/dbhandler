#pragma once
class WXEXPORT PropertiesHandler
{
public:
    PropertiesHandler() {}
    virtual void EditProperies(wxNotebook *parent) = 0;
    virtual void GetProperties() = 0;
    virtual bool IsLogOnly() const { return false; };
    virtual const std::wstring &GetCommand() const { return L""; };
};

class WXEXPORT DatabasePropertiesHandler : public PropertiesHandler
{
public:
    DatabasePropertiesHandler(const Database *db, DatabaseTable *table);
    virtual void EditProperies(wxNotebook *parent) wxOVERRIDE;
    virtual void GetProperties() wxOVERRIDE;
    TableProperties &GetProperty() { return m_prop; }
    virtual const std::wstring &GetCommand() const wxOVERRIDE { return m_command; }
    virtual bool IsLogOnly() const wxOVERRIDE { return m_page1->IsLogOnly(); }

private:
    DatabaseTable *m_table;
    TableProperties m_prop;
    TableGeneralProperty *m_page1;
    CFontPropertyPage *m_page2, *m_page3, *m_page4;
    const Database *m_db;
    std::wstring m_command;
};


#ifndef DBMANAGER_DATABASETYPE
#define DBMANAGER_DATABASETYPE

//typedef template<typename T, typename T2> Database<T, T2> *(*CONNECTTODB)(const wxString &, const wxString &, Database<T, T2> *db, wxString &, WXWidget);

class DBType : public wxWizardPage
{
public:
    DBType(wxWizard *parent);
    virtual wxWizardPage *GetPrev() const;
    virtual wxWizardPage *GetNext() const;
    wxComboBox *GetComboBoxTypes() const;
private:
    wxComboBox *m_types;
};

class SQLiteConnect : public wxWizardPage
{
public:
    SQLiteConnect(wxWizard *parent);
    virtual wxWizardPage *GetPrev() const;
    virtual wxWizardPage *GetNext() const;
    wxFilePickerCtrl *GetFileCtrl() const { return dbName; }
private:
    wxFilePickerCtrl *dbName;
};

typedef void (*CONVERTFROMSQLWCHAR)(SQLWCHAR *, wxString &);

class ODBCConnect : public wxWizardPage
{
public:
    ODBCConnect(wxWizard *parent, const std::vector<std::wstring> &dsn);
    virtual wxWizardPage *GetPrev() const;
    virtual wxWizardPage *GetNext() const;
    void AppendDSNsToList(const std::vector<std::string> &dsns);
    wxListBox *GetDSNTypesCtrl() const;
    wxCheckBox *GetAskForParameters() const;
protected:
     void GetDSNList();
private:
    wxListBox *m_types;
    wxCheckBox *m_ask;
    HENV m_henv;
};

class DatabaseType : public wxWizard
{
public:
    DatabaseType(wxWindow *parent, const wxString &title, const wxString &name, const wxString &engine, const std::vector<std::wstring> &dsn);
    wxWizardPage *GetFirstPage() const { return page1; }
    SQLiteConnect *GetSQLitePage() { return page2; }
    ODBCConnect *GetODBCPage() {return page3; }
    void GetDatabaseEngine(wxString &databaseEngine);
    bool GetODBCConnectionParam();
    wxString GetDatabaseName();
protected:
    void OnPageChanged(wxWizardEvent &event);
    void OnConnect(wxWizardEvent &event);
    void OnButtonUpdateUI(wxUpdateUIEvent &event);
private:
    DBType *page1;
    SQLiteConnect *page2;
    ODBCConnect *page3;
    wxWindow *button;
    wxString m_dbName, m_dbEngine;
    bool m_askForConnectParameter;
};

#endif
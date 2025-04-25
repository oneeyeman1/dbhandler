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
    wxTextCtrl *GetProfilesCtrl() const { return profile; }
    wxStaticText *GetErrorCtrl() { return label1; }
private:
    void OnComboSelecton(wxCommandEvent &event);
    wxStaticText *label1;
    wxTextCtrl *profile;
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

class ODBCConnect : public wxWizardPage
{
public:
    ODBCConnect(wxWizard *parent, const std::vector<std::wstring> &dsn);
    virtual wxWizardPage *GetPrev() const;
    virtual wxWizardPage *GetNext() const;
    void AppendDSNsToList(const std::vector<std::string> &dsns);
    wxListBox *GetDSNTypesCtrl() const;
    wxCheckBox *GetAskForParameters() const;
    void OnSelectionDClick(wxCommandEvent &event);
protected:
     void GetDSNList();
private:
    wxListBox *m_types;
    wxCheckBox *m_ask;
};

class PostgresConnect : public wxWizardPage
{
public:
    PostgresConnect(wxWizard *parent);
    virtual wxWizardPage *GetPrev() const;
    virtual wxWizardPage *GetNext() const;
    void OnAdvanced(wxCommandEvent &event);
    wxTextCtrl *GetDatabaseName();
    wxTextCtrl *GetHost() const;
    wxTextCtrl *GetHostAddr() const;
    wxTextCtrl *GetPort() const;
    wxTextCtrl *GetUserID() const;
    wxTextCtrl *GetPassword() const;
    wxTextCtrl *GetDBName() const;
    wxSpinCtrl *GetTimeout() const;
private:
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5, *m_label6, *m_label7;
    wxTextCtrl *m_host, *m_hostAddr, *m_port, *m_userID, *m_password, *m_dbName;
//    wxMaskedEdit *m_hostAddr;
    wxSpinCtrl *m_timeout;
    wxButton *m_advanced;
    unsigned long m_value;
    wxString m_engine;
};

class PostgresAdvanced : public wxDialog
{
public:
    PostgresAdvanced(wxWindow *parent);
    wxComboBox *m_encoding;
private:
    wxStaticText *m_label1;
    wxPanel *m_panel;
};

class mySQLConnect : public wxWizardPage
{
public:
    mySQLConnect(wxWizard *parent);
    virtual wxWizardPage *GetPrev() const;
    virtual wxWizardPage *GetNext() const;
    void OnAdvanced(wxCommandEvent &event);
    wxTextCtrl *GetDatabaseName();
    wxTextCtrl *GetHost() const;
    wxTextCtrl *GetPort() const;
    wxTextCtrl *GetUserID() const;
    wxTextCtrl *GetPassword() const;
    wxTextCtrl *GetDBName() const;
    wxTextCtrl *GetSocket() const;
    int GetFlags();
    wxString GetOptions() const;
private:
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5, *m_label6, *m_label7;
    wxTextCtrl *m_host, *m_port, *m_userID, *m_password, *m_dbName, *m_socket;
    wxButton *m_advanced;
    int m_flags;
    unsigned long m_value;
    wxString m_engine, m_options;
};

class mySQLAdvanced : public wxDialog
{
public:
    mySQLAdvanced(wxWindow *parent, int flags);
    wxPanel *m_panel;
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5;
    wxCheckBox *m_expPass, *m_clientCompress, *m_foundRows, *m_ignoreSigPipe, *m_ignoreSpace, *m_interactive, *m_localFiles, *m_multiResults, *m_multiStat, *m_noSchema, *m_odbc, *m_ssl, *m_remember, *m_clearText, *m_handleExpiredPass, *m_optCompress, *m_guessConnect;
    wxTextCtrl *m_defaultAuth, *m_initCommand, *m_optBind, *m_connectAttrDelete, *m_connectTimeout;
};

class DatabaseType : public wxWizard
{
public:
    DatabaseType(wxWindow *parent, const wxString &title, const wxString &name, const wxString &engine, const std::vector<std::wstring> &dsn, const std::vector<Profile> &profiles);
    wxWizardPage *GetFirstPage() const { return page1; }
    SQLiteConnect *GetSQLitePage() { return page2; }
    ODBCConnect *GetODBCPage() { return page3; }
    PostgresConnect *GetPostgresPage() { return page4; };
    mySQLConnect *GetmySQLPage() { return page5; };
    void GetDatabaseEngine(wxString &databaseEngine);
    bool GetODBCConnectionParam();
    wxString GetDatabaseName();
    void SetDbEngine(const wxString &engine);
    wxString &GetConnectString() const;
    wxTextCtrl *GetUserControl() const;
    const wxString &GetSelectedProfile() const { return m_newCurrent; }
protected:
    void OnPageChanged(wxWizardEvent &event);
    void OnPageChanging(wxWizardEvent &event);
    void OnConnect(wxWizardEvent &event);
    void OnButtonUpdateUI(wxUpdateUIEvent &event);
private:
    DBType *page1;
    SQLiteConnect *page2;
    ODBCConnect *page3;
    PostgresConnect *page4;
    mySQLConnect *page5;
    wxWindow *button;
    wxString m_dbName, m_dbEngine, m_connStr, m_newCurrent;
    bool m_askForConnectParameter;
    std::vector<Profile> m_profiles;
};

#endif

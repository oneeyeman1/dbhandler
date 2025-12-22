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
    wxMaskedEditText *GetHostAddr() const;
    wxTextCtrl *GetPort() const;
    wxTextCtrl *GetUserID() const;
    wxTextCtrl *GetPassword() const;
    wxTextCtrl *GetDBName() const;
    wxSpinCtrl *GetTimeout() const;
private:
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5, *m_label6, *m_label7;
    wxTextCtrl *m_host/*, *m_hostAddr*/, *m_port, *m_userID, *m_password, *m_dbName;
    wxMaskedEditText *m_hostAddr;
    wxSpinCtrl *m_timeout;
    wxButton *m_advanced;
    unsigned long m_value;
    wxString m_engine;
};

class PostgresAdvanced : public wxDialog
{
public:
    PostgresAdvanced(wxWindow *parent);
private:
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5, *m_label6, *m_label7, *m_label8, *m_label9, *m_label10, *m_label11, *m_label12;
    wxPanel* panel_1;
    wxComboBox* m_clientEncoding;
    wxFilePickerCtrl* m_passFile;
    wxTextCtrl* m_authenticate;
    wxComboBox* m_binding;
    wxTextCtrl* m_appName;
    wxTextCtrl* m_fallbackName;
    wxRadioBox* m_keepAlive;
    wxTextCtrl* m_idle;
    wxTextCtrl* m_count;
    wxTextCtrl* text_ctrl_1;
    wxTextCtrl* m_timeout;
    wxRadioBox* m_replication;
    wxRadioBox* m_gss;
    wxComboBox* m_ssl;
    wxRadioBox* m_negotiate;
    wxRadioBox* m_compression;
    wxButton *m_ok, *m_cancel, *m_help;
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

class SybaseSQLServer : public wxWizardPage
{
public:
    SybaseSQLServer(wxWizard* parent, bool isSQLServer = true);
    int GeDBAPI() { return m_api->GetSelection(); }
    int GetLibrary() { return m_library->GetSelection(); }
    virtual wxWizardPage *GetPrev() const;
    virtual wxWizardPage *GetNext() const;

protected:
    // begin wxGlade: MyDialog::attributes
    wxRadioBox* m_api;
    wxRadioBox* m_library;
    wxStaticText *m_label1, *m_label2, *m_label3;
    wxTextCtrl *m_user, *m_password, *m_serverName; 
    wxStaticLine *m_divider;
    // end wxGlade
};

class DatabaseType : public wxWizard
{
public:
    DatabaseType(wxWindow *parent, const wxString &title, const wxString &name, const wxString &engine, const std::vector<std::wstring> &dsn, const std::vector<Profile> &profiles);
    wxWizardPage *GetFirstPage() const { return page1; }
    SQLiteConnect *GetSQLitePage() const { return page2; }
    ODBCConnect *GetODBCPage() const { return page3; }
    PostgresConnect *GetPostgresPage() const { return page4; };
    mySQLConnect *GetmySQLPage() const { return page5; };
    SybaseSQLServer *GetSybasePage() const { return page7; }
    SybaseSQLServer *GetSQLServerPage() const { return page6; }
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
    SybaseSQLServer *page6;
    SybaseSQLServer *page7;
    wxWindow *button;
    wxString m_dbName, m_dbEngine, m_connStr, m_newCurrent;
    bool m_askForConnectParameter;
    std::vector<Profile> m_profiles;
};

#endif

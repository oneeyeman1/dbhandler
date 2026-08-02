#pragma once

class PostgresODBCSetupDialog : public wxDialog
{
public:
    PostgresODBCSetupDialog(wxWindow *parent);
private:
    wxPanel *m_panel;
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5, *m_label6, *m_label7, *m_label8;
    wxTextCtrl *m_dataSource, *m_desc, *m_dbName, *m_server, *m_port, *m_name, *m_password;
    wxComboBox *m_ssl;
    wxButton *m_datasrc, *m_global, *m_test, *m_save, *m_cancel;
};


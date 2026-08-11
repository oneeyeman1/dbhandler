#pragma once

class mySQLODBCSetupDialog : public wxDialog
{
public:
    mySQLODBCSetupDialog(wxWindow *parent, std::map<std::wstring, std::wstring> &values);
    const std::map<std::wstring, std::wstring> &GetDSNData() const { return m_data; }
    const std::wstring GetDSNName() const { return m_name->GetValue().ToStdWstring(); }
protected:
    void OnTestUpdateUI(wxUpdateUIEvent &event);
    void OnTest(wxCommandEvent &event);
    void OnDetails(wxCommandEvent &event);
    void OnPipe(wxCommandEvent &event);
    void OnServer(wxCommandEvent &event);
    void OnOK(wxCommandEvent &event);
private:
    wxPanel *m_panel;
    wxStaticBitmap *m_logo;
    wxStaticLine *m_line;
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5, *m_label6, *m_label7;
    wxTextCtrl *m_name, *m_desc, *m_serverName, *m_port, *m_pipeName, *m_user, *m_password, *m_dbName;
    wxRadioButton *m_network, *m_pipe;
    wxButton *m_test;
    wxButton *m_details;
    wxNotebook *m_detailsOptions;
    bool m_detailsShown = false;
    unsigned long m_value = 3306;
    std::map<std::wstring, std::wstring> m_data;
};


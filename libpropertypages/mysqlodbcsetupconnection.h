#pragma once

class WXEXPORT MySQLODBCSetupConnection : public wxPanel
{
public:
    MySQLODBCSetupConnection(wxWindow *parent, const std::map<std::wstring, std::wstring> &values);
    bool IsChanged() { return m_changed; }
    wxCheckBox *Get1Check() const { return m_allowBgResultSet; }
    wxCheckBox *Get2Check() const { return checkbox_2; }
    wxCheckBox *Get3Check() const { return checkbox_1; }
    wxCheckBox *Get4Check() const { return checkbox_3; }
    wxCheckBox *Get5Check() const { return checkbox_4; }
    wxCheckBox *Get6Check() const { return checkbox_5; }
    wxCheckBox *Get7Check() const { return checkbox_6; }
    wxCheckBox *Get8Check() const { return checkbox_7; }
    wxCheckBox *Get9Check() const { return checkbox_8; }
    wxCheckBox *Get10Check() const { return checkbox_9; }
    wxCheckBox *Get11Check() const { return checkbox_10; }
    wxComboBox *GetCharSet() const { return combo_box_1; }
    wxTextCtrl *GetInitStmt() const { return text_ctrl_1; }
    wxDirPickerCtrl *GetPluginDir() const { return m_plugin; }
    wxTextCtrl *GetAuth() const { return text_ctrl_3; }
    wxFilePickerCtrl *GetOCI() const { return m_ociFile; }
protected:
    void DataChanged(wxCommandEvent &event);
private:
    wxCheckBox* m_allowBgResultSet;
    wxCheckBox* checkbox_1;
    wxCheckBox* checkbox_2;
    wxCheckBox* checkbox_3;
    wxCheckBox* checkbox_4;
    wxCheckBox* checkbox_5;
    wxCheckBox* checkbox_6;
    wxCheckBox* checkbox_7;
    wxCheckBox* checkbox_8;
    wxCheckBox* checkbox_9;
    wxCheckBox* checkbox_10;
    wxComboBox* combo_box_1;
    wxTextCtrl* text_ctrl_1;
    wxTextCtrl* text_ctrl_3;
    bool m_changed = false;
    wxDirPickerCtrl *m_plugin;
    wxFilePickerCtrl *m_ociFile;
};

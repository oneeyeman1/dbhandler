#pragma once

class WXEXPORT MySQLODBCSetupConnection : public wxPanel
{
public:
    MySQLODBCSetupConnection(wxWindow *parent);
    bool IsChanged() { return m_changed; }
    wxCheckBox *Get1Check() { return m_allowBgResultSet; }
    wxCheckBox *Get2Check() { return checkbox_2; }
    wxCheckBox *Get3Check() { return checkbox_1; }
    wxCheckBox *Get4Check() { return checkbox_3; }
    wxCheckBox *Get5Check() { return checkbox_4; }
    wxCheckBox *Get6Check() { return checkbox_5; }
	wxCheckBox *Get7Check() { return checkbox_6; }
    wxCheckBox *Get8Check() { return checkbox_7; }
    wxCheckBox *Get9Check() { return checkbox_8; }
    wxCheckBox *Get10Check() { return checkbox_9; }
    wxCheckBox *Get11Check() { return checkbox_10; }
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
    wxTextCtrl* text_ctrl_2;
    wxTextCtrl* text_ctrl_3;
    wxTextCtrl* text_ctrl_4;
    bool m_changed = false;
};

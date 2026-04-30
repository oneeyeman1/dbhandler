//
//  createdatabase.h
//  libdialogs
//
//  Created by Igor Korot on 6/28/17.
//  Copyright (c) 2017 Igor Korot. All rights reserved.
//

#ifndef __libdialogs__createdatabase__
#define __libdialogs__createdatabase__

class WXEXPORT CreateDatabase : public wxDialog
{
public:
    CreateDatabase(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, int serverVersion, std::shared_ptr<CreateDBOptions> options);
    std::shared_ptr<CreateDBOptions> GetOptions() { return m_opts; }
protected:
    void OnCharacterSetChanged(wxCommandEvent &event);
    void OnSQLServerFileSecAdd(wxCommandEvent &event);
    void OnSQLServerFileSecDelete(wxCommandEvent &event);
    void OnOKUpdateUI(wxUpdateUIEvent &event);
    void OnOK(wxCommandEvent &event);
private:
    std::shared_ptr<CreateDBOptions> m_opts;
    wxStaticText *m_label1 = nullptr, *m_label2 = nullptr, *m_label3 = nullptr, *m_label4 = nullptr;
    wxFilePickerCtrl *m_SQLiteName = nullptr;
    wxTextCtrl *m_name = nullptr;
    wxCheckBox *m_exist = nullptr;
    wxCollapsiblePane *m_options = nullptr;
    wxComboBox *m_characterSet = nullptr;
    wxComboBox *m_collations = nullptr;
    wxCheckBox *m_encrypted = nullptr;
    wxComboBox *m_owner = nullptr;
    wxComboBox *m_template = nullptr;
    wxComboBox *m_containment = nullptr;
    wxSpinCtrl *m_connlimit = nullptr;
    wxComboBox *m_tablespace = nullptr;
    wxButton *m_add = nullptr;
    wxButton *m_delete = nullptr;
    std::wstring m_type, m_subtype;
};

#endif /* defined(__libdialogs__createdatabase__) */

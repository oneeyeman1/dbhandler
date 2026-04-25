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
    CreateDatabase(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, CreateDBOptions *options);
protected:
    void OnCharacterSetChanged(wxCommandEvent &event);
    void OnOKUpdateUI(wxUpdateUIEvent &event);
private:
    CreateDBOptions *m_opts;
    wxStaticText *m_label1 = nullptr, *m_label2 = nullptr, *m_label3 = nullptr;
    wxFilePickerCtrl *m_SQLiteName = nullptr;
    wxTextCtrl *m_name = nullptr;
    wxCollapsiblePane *m_options = nullptr;
    wxComboBox *m_characterSet = nullptr;
    wxComboBox *m_collations = nullptr;
    wxCheckBox *m_encrypted = nullptr;
    wxComboBox *m_owner = nullptr;
    wxComboBox *m_template = nullptr;
};

#endif /* defined(__libdialogs__createdatabase__) */

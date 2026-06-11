//
//  createdatabase.h
//  libdialogs
//
//  Created by Igor Korot on 6/28/17.
//  Copyright (c) 2017 Igor Korot. All rights reserved.
//

#ifndef __libdialogs__createdatabase__
#define __libdialogs__createdatabase__

class ScrollPanel : public wxPanel
{
public:
    ScrollPanel(wxWindow *parent, wxWindow *cols);
private:
    wxWindow *m_columns;
};

class WXEXPORT CreateDatabase : public wxDialog
{
public:
    CreateDatabase(wxWindow *parent, const std::wstring &type, const std::wstring &subtype, int serverVersionMaor, int serverVersionMinor, std::shared_ptr<CreateDBOptions> options);
    const std::shared_ptr<CreateDBOptions> GetOptions() const { return m_opts; }
    wxCollapsiblePane *GetSQLOptons() { return m_options; }
protected:
    void OnCharacterSetChanged(wxCommandEvent &event);
    void OnSQLServerFileSecAdd(wxCommandEvent &event);
    void OnSQLServerFileSecDelete(wxCommandEvent &event);
    void OnOKUpdateUI(wxUpdateUIEvent &event);
    void OnOK(wxCommandEvent &event);
    void OnPersistentLog(wxCommandEvent &event);
private:
    std::shared_ptr<CreateDBOptions> m_opts;
    wxStaticText *m_label1 = nullptr, *m_label2 = nullptr, *m_label3 = nullptr, *m_label4 = nullptr, *m_label5 = nullptr, *m_label6 = nullptr, *m_label7 = nullptr, *m_label8 = nullptr, *m_label9 = nullptr, *m_label10 = nullptr, *m_label11 = nullptr, *m_label12 = nullptr;
    wxFilePickerCtrl *m_SQLiteName = nullptr;
    wxTextCtrl *m_name = nullptr;
    wxCheckBox *m_exist = nullptr;
    wxCollapsiblePane *m_options = nullptr;
    wxCollapsiblePane *m_with = nullptr;
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
    wxButton *m_add1 = nullptr;
    wxButton *m_delete1 = nullptr;
    wxButton *m_add2 = nullptr;
    wxButton *m_delete2 = nullptr;
    std::wstring m_type, m_subtype;
    int m_versionMajor, m_versionMinor;
    wxFlexGridSizer *paneSizer1 = nullptr;
    wxComboBox *m_ctype = nullptr;
    wxCheckBox *m_allowConn = nullptr;
    wxChoice *m_access = nullptr;
    wxCheckBox *m_istemplate = nullptr;
    wxDirPickerCtrl *m_dirName1 = nullptr;
    wxChoice *m_fulltext = nullptr;
    wxChoice *m_lang = nullptr;
    wxCheckBox *m_triggers = nullptr;
    wxCheckBox *m_noise = nullptr;
    wxSpinCtrl *m_yearCutoff = nullptr;
    wxCheckBox *m_dbChain = nullptr;
    wxCheckBox *m_trust = nullptr;
    wxDirPickerCtrl *m_dirName2 = nullptr;
    wxCheckBox *m_persistantLog = nullptr;
    wxCheckBox *m_ledger = nullptr;
    wxScrolledWindow *m_scrolled = nullptr;
    wxScrolled<wxWindow> *m_scrolled1 = nullptr;
    wxScrolledWindow *m_scrolled2 = nullptr;
    int m_position = 12;
    wxCheckBox *m_filegroup = nullptr;
    wxTextCtrl *m_filegroupName = nullptr;
    wxChoice *m_filegroupContains = nullptr;
    wxCheckBox *m_memoryData = nullptr;
    wxCheckBox *m_log = nullptr;
//    wxScrolledWindow *m_canvas1 = nullptr, *m_canvas2 = nullptr, *m_canvas3 = nullptr;
};

#endif /* defined(__libdialogs__createdatabase__) */

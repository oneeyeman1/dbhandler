//
//  attachdb.hpp
//  libdialogs
//
//  Created by Igor Korot on 10/8/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//

#ifndef attachdb_hpp
#define attachdb_hpp

class AttachDB : public wxDialog
{
public:
    AttachDB(wxWindow *parent, Database *db, const std::vector<std::wstring> &names);
    auto &GetCatalog() const { return m_catalog; }
    auto &GetSchea() const { return m_schema;; }
protected:
    void OnFileSelected(wxCommandEvent &event);
    void OnOUpdate(wxUpdateUIEvent &event);
    void OnOk(wxCommandEvent &event);
private:
    Database *m_db;
    wxPanel *m_panel;
    wxStaticText *m_label1, *m_label2;
    wxFilePickerCtrl *m_picker;
    wxTextCtrl *m_schemaName;
    wxListBox *m_dbList;
    short m_dbtype;
    wxButton *m_ok, *m_cancel, *m_help;
    wxString m_catalog, m_schema;
};

#endif /* attachdb_hpp */

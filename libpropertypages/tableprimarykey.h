#pragma once
class WXEXPORT TablePrimaryKey : public PropertyPageBase
{
public:
    TablePrimaryKey(wxWindow *parent, Database *db, const DatabaseTable *table);
    const std::vector<std::wstring> &GetNewKey() const { return newKey; }
protected:
    void do_layout();
    void OnLeftDown(wxMouseEvent &event);
private:
    Database *m_db;
    FieldWindow* m_foreignKeyColumnsFields;
    wxStaticText *m_label;
    wxListCtrl *m_fields;
    const DatabaseTable *m_table;
    std::vector<std::wstring> newKey;
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5, *m_label6;
    wxTextCtrl *m_pkName;
    wxComboBox *m_conflict = nullptr;
    wxCheckBox *m_autoincrement = nullptr;
    wxCheckBox *m_clustered = nullptr;
    wxTextCtrl *m_included = nullptr;
    wxCheckBox *m_padIndex = nullptr;
    wxCheckBox *m_xml;
    wxCheckBox *m_ignoreDup = nullptr;
    wxCheckBox *m_norecompute = nullptr;
    wxCheckBox *m_incremental = nullptr;
    wxCheckBox *m_rowLocks = nullptr;
    wxCheckBox *m_pageLocks = nullptr;
    wxCheckBox *m_sequential = nullptr;
    wxSpinCtrl *m_fillFactor = nullptr;
    wxTextCtrl *m_delay = nullptr;
    wxComboBox *m_compression = nullptr;
    wxTextCtrl *m_tableSpace = nullptr;
    std::shared_ptr<PKOptions> m_options;
};

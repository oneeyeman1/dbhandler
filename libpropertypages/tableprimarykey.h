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
    wxStaticText *m_label1;
    wxComboBox *m_conflict = nullptr;
    wxCheckBox *m_autoincrement = nullptr;
    std::shared_ptr<PKOptions> m_options;
};

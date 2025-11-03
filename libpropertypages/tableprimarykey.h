#pragma once
class WXEXPORT TablePrimaryKey : public PropertyPageBase
{
public:
    TablePrimaryKey(wxWindow *parent, const DatabaseTable *table);
    const std::vector<std::wstring> &GetNewKey() const { return newKey; }
protected:
    void do_layout();
    void OnLeftDown(wxMouseEvent &event);
private:
    FieldWindow* m_foreignKeyColumnsFields;
    wxStaticText *m_label;
    wxListCtrl *m_fields;
    const DatabaseTable *m_table;
    std::vector<std::wstring> newKey;
};

#pragma once
class WXEXPORT TablePrimaryKey : public PropertyPageBase
{
public:
    TablePrimaryKey(wxWindow *parent, const DatabaseTable *table);
protected:
    void do_layout();
    void OnLeftDown(wxMouseEvent &event);
    const std::vector<wxString> &GetNewKey() const { return newKey; }
private:
    FieldWindow* m_foreignKeyColumnsFields;
    wxStaticText *m_label;
    wxListCtrl *m_fields;
    const DatabaseTable *m_table;
    std::vector<wxString> newKey;
};

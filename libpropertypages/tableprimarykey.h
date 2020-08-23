#pragma once
class WXEXPORT TablePrimaryKey : public PropertyPageBase
{
public:
    TablePrimaryKey(wxWindow *parent);
protected:
    void do_layout();
private:
    wxTextCtrl *temp;
    FieldWindow* m_foreignKeyColumnsFields;
    wxStaticText *m_label;
};
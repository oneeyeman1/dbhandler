#ifndef __RETRIEVALARGUMENTS__H
#define __RETRIEVALARGUMENTS__H

class QueryArguments
{
public:
    int m_pos;
    wxString m_name, m_type;
    QueryArguments(int pos, const wxString &name, const wxString type) : m_pos(pos), m_name(name), m_type(type) {}
};

class QueryLines
{
public:
    wxStaticBitmap *m_pointer;
    wxStaticText *m_number;
    wxTextCtrl *m_name;
    wxComboBox *m_type;

    QueryLines (wxStaticBitmap *pointer, wxStaticText *number, wxTextCtrl *name, wxComboBox *type) : m_pointer (pointer), m_number (number), m_name (name), m_type (type)
    {
    };
};

class RetrievalArguments : public wxDialog
{
public:
    RetrievalArguments(wxWindow *parent, std::vector<QueryArguments> &arguments, const wxString &dbType, const wxString &subType);
    ~RetrievalArguments(void);
protected:
    void OnAddArgument(wxCommandEvent &event);
    void OnRemoveArgument(wxCommandEvent &event);
    void do_layout();
    void set_properties();
private:
    wxPanel *m_panel, *argPanel;
///    MySubScrolledWindow *m_arguments;
    wxStaticText *m_labe11, *m_label2, *m_label3;
    wxButton *m_ok, *m_cancel, *m_help, *m_add, *m_insert, *m_remove;
    wxStaticBoxSizer *box;
};

#endif
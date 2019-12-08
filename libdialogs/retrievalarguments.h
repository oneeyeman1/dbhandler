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
    TypeComboBox *m_type;

    QueryLines (wxStaticBitmap *pointer, wxStaticText *number, wxTextCtrl *name, TypeComboBox *type) : m_pointer (pointer), m_number (number), m_name (name), m_type (type)
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
    void OnKeyDown(wxKeyEvent &event);
    void OnMouse(wxMouseEvent &event);
    void OnSize(wxSizeEvent &event);
    void OnRemoveUpdateUI(wxUpdateUIEvent &event);
    void UpdateHeader();
    void set_properties();
private:
    wxPanel *m_panel, *argPanel, *m_mainPanel, *dummy_1, *dummy_2, *dummy_3, *dummy_4;
    wxBoxSizer *sizer;
    wxBitmap bmp;
    wxScrolledWindow *args;
    wxFlexGridSizer *fgs;
///    MySubScrolledWindow *m_arguments;
    wxStaticText *m_labe11, *m_label2, *m_label3;
    wxButton *m_ok, *m_cancel, *m_help, *m_add, *m_insert, *m_remove;
    wxStaticBoxSizer *box;
    wxString m_type, m_subType;
    int numArgs, m_currentLine;
    std::list<QueryLines> m_lines;
};

#endif
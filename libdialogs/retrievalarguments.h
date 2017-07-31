#ifndef __RETRIEVALARGUMENTS__H
#define __RETRIEVALARGUMENTS_H

class RetrievalArguments : public wxDialog
{
public:
    RetrievalArguments(wxWindow *parent);
    ~RetrievalArguments(void);
protected:
    void do_layout();
    void set_properties();
private:
    wxPanel *m_panel;
    wxScrolled<wxWindow> *m_arguments;
    wxButton *m_ok, *m_cancel, *m_help, *m_add, *m_remove;
};

class ColumnLabels : public wxWindow
{
public:
    ColumnLabels(wxScrolled<wxWindow> *parent );
private:
    void OnPaint(wxPaintEvent &event);
    wxScrolled<wxWindow> *m_parent;
};

#endif
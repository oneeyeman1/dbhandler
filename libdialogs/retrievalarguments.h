#ifndef __RETRIEVALARGUMENTS__H
#define __RETRIEVALARGUMENTS_H

class RetrievalArguments : public wxDialog
{
public:
    RetrievalArguments(void);
    ~RetrievalArguments(void);
protected:
    void do_layout();
    void set_properties();
private:
    wxPanel *m_panel;
    wxButton *m_ok, *m_cancel, *m_help, *m_add, *m_remove;
};

#endif
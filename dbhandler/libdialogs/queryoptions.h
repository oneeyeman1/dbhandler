#ifndef __QUERYOPTIONS__H
#define __QUERYOPTIONS__H

class QueryOptionsDialog : public wxDialog
{
public:
    QueryOptionsDialog(wxWindow *parent, int optionsType);
private:
    wxListBox *m_options;
    wxPanel *m_panel;
    wxButton *m_ok, *m_cancel, *m_help;
};

#endif

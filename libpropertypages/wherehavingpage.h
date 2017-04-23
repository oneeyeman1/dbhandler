#pragma once
class WXEXPORT WhereHavingPage :	public wxPanel
{
public:
    WhereHavingPage(wxWindow *parent);
    ~WhereHavingPage(void);
protected:
    void do_layout();
    void set_properties();
private:
    wxGrid *m_grid;
};


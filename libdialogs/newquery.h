#pragma once
class NewQuery : public wxDialog
{
public:
    NewQuery(wxWindow *parent, const wxString &title);
    ~NewQuery();
private:
    BitmapPanel *m_panels[14];
};


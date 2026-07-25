#pragma once

class mySQLODBCSetupDialog : public wxDialog
{
public:
    mySQLODBCSetupDialog(wxWindow *parent, wxWindowID id, const wxString &title);
private:
    wxPanel *m_panel;
    wxStaticBitmap *m_logo;
    wxStaticLine *m_line;
};


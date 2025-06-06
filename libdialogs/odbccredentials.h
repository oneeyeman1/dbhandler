// -*- C++ -*-
//
// generated by wxGlade 0.7.2 (standalone edition) on Sun Jul 08 00:47:10 2018
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#ifndef ODBCCREDENTIALS_H
#define ODBCCREDENTIALS_H

#ifndef APP_CATALOG
#define APP_CATALOG "app"  // replace with the appropriate catalog name
#endif

class ODBCCredentials: public wxDialog
{
public:
    // begin wxGlade: ODBCCredentials::ids
    // end wxGlade

    ODBCCredentials(wxWindow *parent, wxWindowID id, const wxString& title, const wxString &dsn, const wxString &userID, const wxString &password);
    const wxTextCtrl &GetUserIDControl();
    const wxTextCtrl &GetPasswordControl();

private:
    wxString m_dsn;
    // begin wxGlade: ODBCCredentials::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    // begin wxGlade: ODBCCredentials::attributes
    wxPanel *m_panel;
    wxStaticText* label_1;
    wxTextCtrl* m_userID;
    wxStaticText* label_2;
    wxTextCtrl* m_password;
    // end wxGlade
}; // wxGlade: end class


#endif // ODBCCREDENTIALS_H

// -*- C++ -*-
//
// generated by wxGlade 0.7.2 (standalone edition) on Tue Dec 31 21:39:49 2019
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#ifndef GOTOLINE_H
#define GOTOLINE_H

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade


class GoToDialog: public wxDialog
{
public:
    // begin wxGlade: GoToDialog::ids
    // end wxGlade

    GoToDialog(wxWindow* parent);
    wxTextCtrl *GetLineNumberCtrl() { return m_lineNo; };
private:
    unsigned int m_lineNumber;
    // begin wxGlade: GoToDialog::methods
    void set_properties();
    void do_layout();
    // end wxGlade

protected:
    void OnOKUpdateUI(wxUpdateUIEvent &event);
    // begin wxGlade: GoToDialog::attributes
    wxStaticText* m_label1;
    wxTextCtrl* m_lineNo;
    wxPanel* m_panel;
    wxButton *m_ok;
    // end wxGlade
}; // wxGlade: end class


#endif // GOTOLINE_H
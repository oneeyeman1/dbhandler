// -*- C++ -*-
//
// generated by wxGlade 1.0.4 on Sun May 21 10:43:09 2023
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#ifndef TOOLBARSETUP_H
#define TOOLBARSETUP_H

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade


class ToolbarSetup: public wxDialog {
public:
    // begin wxGlade: ToolbarSetup::ids
    // end wxGlade

    ToolbarSetup(wxWindow* parent, wxWindowID id, const wxString& title);

private:

protected:
    // begin wxGlade: ToolbarSetup::attributes
    wxPanel* m_panel;
    wxListBox* m_toolbars;
    wxRadioBox* m_orientation;
    wxCheckBox* m_showText;
    wxCheckBox* m_showToolTips;
    wxComboBox* m_fontNames;
    wxComboBox* m_fontSize;
    wxButton* m_close;
    wxButton* m_help;
    wxButton* m_hideShow;
    wxButton* m_customize;
    wxButton* m_new;
    // end wxGlade
}; // wxGlade: end class


#endif // TOOLBARSETUP_H

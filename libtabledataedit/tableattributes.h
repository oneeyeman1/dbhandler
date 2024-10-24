// -*- C++ -*-
//
// generated by wxGlade 1.0.4 on Mon Nov 27 15:45:05 2023
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#ifndef TABLEATTRIBUTES_H
#define TABLEATTRIBUTES_H

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/intl.h>

#ifndef APP_CATALOG
#define APP_CATALOG "app"  // replace with the appropriate catalog name
#endif

class TableSettngs: public wxPanel
{
public:
    TableSettngs(wxWindow* parent, wxWindowID id, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0);

private:

protected:
    // begin wxGlade: TableSettngs::attributes
    wxStaticText* m_label1;
    wxComboBox* m_format;
    wxStaticText* m_label2;
    wxComboBox* m_edit;
    wxStaticText* m_label3;
    wxComboBox* m_validation;
    wxStaticText* m_lael4;
    wxTextCtrl* m_header;
    wxStaticText* m_lael5;
    wxComboBox* m_justify;
    wxStaticText* m_label6;
    wxTextCtrl* text_ctrl_1;
    wxStaticText* m_label7;
    wxStaticText* m_label8;
    wxTextCtrl* m_width;
    wxStaticText* m_label9;
    wxStaticText* m_label10;
    wxComboBox* combo_box_1;
    wxStaticText* label_1;
    wxTextCtrl* text_ctrl_2;
    wxStaticText* m_label12;
    wxTextCtrl* m_comment;
    // end wxGlade
}; // wxGlade: end class


#endif // TABLEATTRIBUTES_H

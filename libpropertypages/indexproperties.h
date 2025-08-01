// -*- C++ -*-
//
// generated by wxGlade 1.1.0 on Wed Jul  2 23:40:30 2025
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#ifndef INDEXPROPERTIES_H
#define INDEXPROPERTIES_H

// begin wxGlade: ::dependencies
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade


class WXEXPORT TableIndex: public PropertyPageBase
{
public:
    // begin wxGlade: TableIndex::ids
    // end wxGlade

    TableIndex(wxWindow *parent, wxWindowID id, const std::map<unsigned long, std::vector<FKField *> > &fKeys, bool isIndex);
    TableIndex(wxWindow *parent, wxWindowID id, const std::vector<std::wstring> &indexes, bool isIndex);

private:
    bool m_isIndex;
protected:
    void OnIndexSelected(wxCommandEvent &event);

    void InitGui();
    // begin wxGlade: TableIndex::attributes
    wxListBox* list_box_1;
    wxButton* m_edit;
    wxButton* m_new;
    wxButton* m_delete;
    // end wxGlade
}; // wxGlade: end class


#endif // INDEXPROPERTIES_H

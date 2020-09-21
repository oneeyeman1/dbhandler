//
//  colorcombobox.h
//  libpropertypages
//
//  Created by Igor Korot on 9/17/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//

#ifndef colorcombobox_h
#define colorcombobox_h

struct ColorStruct
{
    ColorStruct()
    {
#ifdef __WXMSW__
        ::ZeroMemory( this, sizeof( ColorStruct ) );
#endif
    }
    ColorStruct(wxColour color, wxString name)
    {
        m_color = color;
        m_name = name;
    }
    wxColour m_color;
    wxString m_name;
};

class WXEXPORT CColorComboBox : public wxBitmapComboBox
{
public:
    CColorComboBox( wxWindow *parent, wxWindowID = wxID_ANY, wxString selection = wxEmptyString, const wxPoint &pos =  wxDefaultPosition, const wxSize &size = wxDefaultSize, int n = 0, const wxString choices[] = NULL, long style = wxCB_READONLY );
    void SetColourValue(wxColour colour);
    wxColour &GetColourValue();
private:
    std::vector<ColorStruct> m_colors;
};

#endif /* colorcombobox_h */

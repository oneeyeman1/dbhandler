//
//  dbview.h
//  libdbwindow
//
//  Created by Igor Korot on 3/12/23.
//  Copyright © 2023 Igor Korot. All rights reserved.
//

#ifndef dbview_h
#define dbview_h

struct Properties
{
    wxString m_name, m_tag, m_text, m_cursor;
    bool m_supressPrint;
    int m_border, m_alignment;
    wxFont m_font;
    wxPoint m_position;
    wxSize m_size;
};
 
struct DesignLabelProperties : public Properties
{
};

struct DesignFieldProperties : public Properties
{
};

#endif /* dbview_h */

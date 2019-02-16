#pragma once
#include "wx/wxprec.h"

class WXEXPORT PropertyPageBase : public wxPanel
{
public:
    PropertyPageBase(wxWindow *parent, wxWindowID id = wxID_ANY) : wxPanel(parent, id) { m_isModified = false; }
    bool IsModified() { return m_isModified; };
protected:
    bool m_isModified;
};


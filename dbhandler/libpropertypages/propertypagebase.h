#pragma once
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class WXEXPORT PropertyPageBase : public wxPanel
{
public:
    PropertyPageBase(wxWindow *parent, wxWindowID id = wxID_ANY) : wxPanel(parent, id) { m_isModified = false; }
    bool IsModified() { return m_isModified; };
    void SetModified(bool modified) { m_isModified = modified; }
protected:
    bool m_isModified;
};


#pragma once

class TabOrder : public wxSFRectShape
{
public:
    TabOrder();
    virtual ~TabOrder();
private:
    wxSFGridShape *m_grid;
    wxSFTextShape *m_text;
};
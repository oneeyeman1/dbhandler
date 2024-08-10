#pragma once

class LibraryViewPainter :  public wxView
{
public:
    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    void SetViewType(ViewType type) { m_type = type; }
    void SetParentWindow(wxWindow *window) { m_parent = wxStaticCast( window, wxDocMDIParentFrame ); }
    void SetConfiguration(Configuration *conf) { m_conf = conf; }
private:
    ViewType m_type;
    wxDocMDIParentFrame *m_parent;
    Configuration *m_conf;
};

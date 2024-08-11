#pragma once

class LibraryViewPainter :  public wxView
{
public:
    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    virtual void OnDraw(wxDC *) wxOVERRIDE;
    void SetViewType(ViewType type) { m_type = type; }
    void SetParentWindow(wxWindow *window) { m_parent = wxStaticCast( window, wxDocMDIParentFrame ); }
    void SetConfiguration(Configuration *conf) { m_conf = conf; }
private:
    ViewType m_type;
    wxDocMDIParentFrame *m_parent;
    Configuration *m_conf;
    wxToolBar *m_tb;
    wxString m_libPath;
    wxDECLARE_DYNAMIC_CLASS(LibraryViewPainter);
};

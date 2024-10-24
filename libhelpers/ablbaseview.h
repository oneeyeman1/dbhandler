#pragma once
class WXEXPORT ABLBaseView : public wxView
{
public:
    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    virtual void ApplyProperties() {};
protected:
    virtual void LoadToolbarIcons() {};
    wxDocMDIParentFrame *m_parent;
    wxToolBar *m_tb, *m_styleBar;
    wxString m_libPath;
};


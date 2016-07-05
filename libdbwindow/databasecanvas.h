#ifndef _DATABASECANVAS_H
#define _DATABASECANVAS_H

class WXEXPORT DatabaseCanvas : public wxScrolledWindow
{
public:
    DatabaseCanvas(wxView *view, wxWindow *parent = NULL);
    virtual ~DatabaseCanvas();
    virtual void OnDraw(wxDC& dc) wxOVERRIDE;
private:
    wxView *m_view;
};

#endif
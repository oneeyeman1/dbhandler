#pragma once
#include "C:\wxWidgets\include\wx\docview.h"

class LibraryView :  public wxView
{
public:
    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    void SetViewType(ViewType type) { m_type = type; }
private:
    ViewType m_type;
};
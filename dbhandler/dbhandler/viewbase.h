/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/mainframe.h
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _DOCVIEW_DBHANDLER_VIEWBASE
#define _DOCVIEW_DBHANDLER_VIEWBASE

class CViewBase : public wxView
{
public:
    CViewBase();
protected:
    bool m_isInitialized;
};

#endif
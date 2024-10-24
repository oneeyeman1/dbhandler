// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "ablbaseview.h"

bool ABLBaseView::OnCreate(wxDocument *doc, long flags)
{
    if( !wxView::OnCreate( doc, flags ) )
        return false;
    wxWindowList children;
#ifndef __WXMSW__
    children = m_parent->GetChildren();
#else
    children = m_parent->GetClientWindow()->GetChildren();
#endif
    auto found = false;
    wxToolBar *tb = nullptr;
    for( wxWindowList::iterator it = children.begin(); it != children.end() && !found; it++ )
    {
        tb = wxDynamicCast( *it, wxToolBar );
        if( tb && tb->GetName() == "ViewBar" )
        {
            found = true;
            m_tb = tb;
        }
        if( tb && tb->GetName() == "StyleBar" )
            m_styleBar = tb;
    }
    auto stdPath = wxStandardPaths::Get();
#ifdef __WXOSX__
    wxFileName fn( stdPath.GetExecutablePath() );
    fn.RemoveLastDir();
    m_libPath = fn.GetPathWithSep() + "Frameworks/";
#elif __WXGTK__ || __WXQT__
    m_libPath = stdPath.GetInstallPrefix() + "/lib/";
#elif __WXMSW__
    wxFileName fn( stdPath.GetExecutablePath() );
    m_libPath = fn.GetPathWithSep();
#endif
    return true;
}

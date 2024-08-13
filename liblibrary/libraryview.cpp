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

#include <map>
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "wx/bmpcbox.h"
#include "wx/volume.h"
#include "wx/treectrl.h"
#include "configuration.h"
#include "librarydocument.h"
#include "libraryview.h"

wxIMPLEMENT_DYNAMIC_CLASS(LibraryViewPainter, wxView);

bool LibraryViewPainter::OnCreate(wxDocument *doc, long flags)
{
    wxToolBar *tb = nullptr;
    wxWindowList children;
    wxRect clientRect = m_parent->GetClientRect();
    if( !wxView::OnCreate( doc, flags ) )
        return false;
#ifndef __WXMSW__
    children = m_parent->GetChildren();
#else
    children = m_parent->GetClientWindow()->GetChildren();
#endif
    auto found = false;
    for( wxWindowList::iterator it = children.begin(); it != children.end() && !found; it++ )
    {
        tb = wxDynamicCast( *it, wxToolBar );
        if( tb && tb->GetName() == "ViewBar" )
        {
            found = true;
            m_tb = tb;
        }
    }
    auto stdPath = wxStandardPaths::Get();
#ifdef __WXOSX__
    wxFileName fn( stdPath.GetExecutablePath() );
    fn.RemoveLastDir();
    m_libPath = fn.GetPathWithSep() + "Frameworks/";
#elif __WXGTK__ || __WXGTK__
    m_libPath = stdPath.GetInstallPrefix() + "/lib/";
#elif __WXMSW__
    wxFileName fn( stdPath.GetExecutablePath() );
    m_libPath = fn.GetPathWithSep();
#endif
    wxPoint pos;
#ifdef __WXOSX__
    pos.y = ( m_parent->GetClientWindow()->GetClientRect().GetHeight() - m_parent->GetClientRect().GetHeight() ) - m_parent->GetToolBar()->GetRect().GetHeight();
#else
    pos = wxDefaultPosition;
#endif
    wxArrayString vs;
    auto volumes = wxFSVolume::GetVolumes();
    auto path = GetDocument()->GetFilename();
    wxFileName fileName( path );
//    auto volume = fileName.GetVolume();
    m_frame = new wxDocMDIChildFrame( doc, this, m_parent, wxID_ANY, _( "Library" ), pos, wxSize( clientRect.GetWidth(), clientRect.GetHeight() ) );
    auto sizer = new wxBoxSizer( wxVERTICAL );
    m_drive = new wxBitmapComboBox( m_frame, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, vs, wxCB_READONLY  );
    for( auto volume : volumes )
    {
        wxFSVolume vol( volume );
        m_drive->Append( volume, vol.GetIcon( wxFS_VOL_ICO_SMALL ) );
    }
    wxString str = "";
#ifdef __WXMSW__
    str = fileName.GetVolume() + ":\\";
#else
    str = fileName.GetVolume();
#endif
    m_drive->SetStringSelection( str );
    sizer->Add( m_drive, 0 , wxEXPAND, 0 );
    m_tree = new wxTreeCtrl( m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_NO_BUTTONS | wxTR_LINES_AT_ROOT );
    sizer->Add( m_tree, 1, wxEXPAND, 0 );
    m_frame->SetSizer( sizer );
    m_frame->Layout();
    m_frame->Show();
    return true;
}

void LibraryViewPainter::OnDraw( wxDC * )
{
}

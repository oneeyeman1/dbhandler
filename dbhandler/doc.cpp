/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/doc.cpp
// Purpose:     Implements document functionality
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_STD_IOSTREAM
    #include "wx/ioswrap.h"
#else
    #include "wx/txtstrm.h"
#endif
#include "wx/wfstream.h"

#include "doc.h"
#include "view.h"


// ----------------------------------------------------------------------------
// wxTextDocument: wxDocument and wxTextCtrl married
// ----------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxTextDocument, wxDocument);

bool wxTextDocument::OnCreate(const wxString& path, long flags)
{
    if ( !wxDocument::OnCreate(path, flags) )
        return false;

    // subscribe to changes in the text control to update the document state
    // when it's modified
    GetTextCtrl()->Connect
    (
        wxEVT_TEXT,
        wxCommandEventHandler(wxTextDocument::OnTextChange),
        NULL,
        this
    );

    return true;
}

// Since text windows have their own method for saving to/loading from files,
// we override DoSave/OpenDocument instead of Save/LoadObject
bool wxTextDocument::DoSaveDocument(const wxString& filename)
{
    return GetTextCtrl()->SaveFile(filename);
}

bool wxTextDocument::DoOpenDocument(const wxString& filename)
{
    if ( !GetTextCtrl()->LoadFile(filename) )
        return false;

    // we're not modified by the user yet
    Modify(false);

    return true;
}

bool wxTextDocument::IsModified() const
{
    wxTextCtrl* wnd = GetTextCtrl();
    return wxDocument::IsModified() || (wnd && wnd->IsModified());
}

void wxTextDocument::Modify(bool modified)
{
    wxDocument::Modify(modified);

    wxTextCtrl* wnd = GetTextCtrl();
    if (wnd && !modified)
    {
        wnd->DiscardEdits();
    }
}

void wxTextDocument::OnTextChange(wxCommandEvent& event)
{
    Modify(true);

    event.Skip();
}

// ----------------------------------------------------------------------------
// TextEditDocument implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(TextEditDocument, wxDocument);

wxTextCtrl* TextEditDocument::GetTextCtrl() const
{
    wxView* view = GetFirstView();
    return view ? wxStaticCast(view, TextEditView)->GetText() : NULL;
}

// ----------------------------------------------------------------------------
// ImageDocument and ImageDetailsDocument implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(ImageDocument, wxDocument);

bool ImageDocument::DoOpenDocument(const wxString& file)
{
    return m_image.LoadFile(file);
}

bool ImageDocument::OnOpenDocument(const wxString& filename)
{
    if ( !wxDocument::OnOpenDocument(filename) )
        return false;

    // we don't have a wxDocTemplate for the image details document as it's
    // never created by wxWidgets automatically, instead just do it manually
    ImageDetailsDocument * const docDetails = new ImageDetailsDocument(this);
    docDetails->SetFilename(filename);

    new ImageDetailsView(docDetails);

    return true;
}

ImageDetailsDocument::ImageDetailsDocument(ImageDocument *parent)
    : wxDocument(parent)
{
    const wxImage image = parent->GetImage();

    m_size.x = image.GetWidth();
    m_size.y = image.GetHeight();
    m_numColours = image.CountColours();
    m_type = image.GetType();
    m_hasAlpha = image.HasAlpha();
}

/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/doc.h
// Purpose:     Document classes
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLES_DOCVIEW_DOC_H_
#define _WX_SAMPLES_DOCVIEW_DOC_H_

#include "wx/docview.h"
#include "wx/cmdproc.h"
#include "wx/vector.h"
#include "wx/image.h"

// This sample is written to build both with wxUSE_STD_IOSTREAM==0 and 1, which
// somewhat complicates its code but is necessary in order to support building
// it under all platforms and in all build configurations
//
// In your own code you would normally use std::stream classes only and so
// wouldn't need these typedefs
#if wxUSE_STD_IOSTREAM
    typedef wxSTD istream DocumentIstream;
    typedef wxSTD ostream DocumentOstream;
#else // !wxUSE_STD_IOSTREAM
    typedef wxInputStream DocumentIstream;
    typedef wxOutputStream DocumentOstream;
#endif // wxUSE_STD_IOSTREAM/!wxUSE_STD_IOSTREAM

// ----------------------------------------------------------------------------
// The document class and its helpers
// ----------------------------------------------------------------------------




// ----------------------------------------------------------------------------
// Some operations (which can be done and undone by the view) on the document:
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxTextDocument: wxDocument and wxTextCtrl married
// ----------------------------------------------------------------------------

class wxTextDocument : public wxDocument
{
public:
    wxTextDocument() : wxDocument() { }

    virtual bool OnCreate(const wxString& path, long flags) wxOVERRIDE;

    virtual wxTextCtrl* GetTextCtrl() const = 0;

    virtual bool IsModified() const wxOVERRIDE;
    virtual void Modify(bool mod) wxOVERRIDE;

protected:
    virtual bool DoSaveDocument(const wxString& filename) wxOVERRIDE;
    virtual bool DoOpenDocument(const wxString& filename) wxOVERRIDE;

    void OnTextChange(wxCommandEvent& event);

    wxDECLARE_NO_COPY_CLASS(wxTextDocument);
    wxDECLARE_ABSTRACT_CLASS(wxTextDocument);
};

// ----------------------------------------------------------------------------
// A very simple text document class
// ----------------------------------------------------------------------------

class TextEditDocument : public wxTextDocument
{
public:
    TextEditDocument() : wxTextDocument() { }
    virtual wxTextCtrl* GetTextCtrl() const wxOVERRIDE;

    wxDECLARE_NO_COPY_CLASS(TextEditDocument);
    wxDECLARE_DYNAMIC_CLASS(TextEditDocument);
};

// ----------------------------------------------------------------------------
// Image and image details document classes (both are read-only for simplicity)
// ----------------------------------------------------------------------------

// This is a normal document containing an image, just like TextEditDocument
// above contains some text. It can be created from an image file on disk as
// usual.
class ImageDocument : public wxDocument
{
public:
    ImageDocument() : wxDocument() { }

    virtual bool OnOpenDocument(const wxString& file) wxOVERRIDE;

    wxImage GetImage() const { return m_image; }

protected:
    virtual bool DoOpenDocument(const wxString& file) wxOVERRIDE;

private:
    wxImage m_image;

    wxDECLARE_NO_COPY_CLASS(ImageDocument);
    wxDECLARE_DYNAMIC_CLASS(ImageDocument);
};

// This is a child document of ImageDocument: this document doesn't
// correspond to any file on disk, it's part of ImageDocument and can't be
// instantiated independently of it.
class ImageDetailsDocument : public wxDocument
{
public:
    ImageDetailsDocument(ImageDocument *parent);

    // accessors for ImageDetailsView
    wxSize GetSize() const { return m_size; }
    unsigned long GetNumColours() const { return m_numColours; }
    wxBitmapType GetType() const { return m_type; }
    bool HasAlpha() const { return m_hasAlpha; }

private:
    // some information about the image we choose to show to the user
    wxSize m_size;
    unsigned long m_numColours;
    wxBitmapType m_type;
    bool m_hasAlpha;

    wxDECLARE_NO_COPY_CLASS(ImageDetailsDocument);
};

#endif // _WX_SAMPLES_DOCVIEW_DOC_H_

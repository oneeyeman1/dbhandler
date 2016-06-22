/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/view.cpp
// Purpose:     View classes implementation
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

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

#include "docview.h"
#include "doc.h"
#include "view.h"

// ----------------------------------------------------------------------------
// TextEditView implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(TextEditView, wxView);

wxBEGIN_EVENT_TABLE(TextEditView, wxView)
    EVT_MENU(wxID_COPY, TextEditView::OnCopy)
    EVT_MENU(wxID_PASTE, TextEditView::OnPaste)
    EVT_MENU(wxID_SELECTALL, TextEditView::OnSelectAll)
wxEND_EVENT_TABLE()

bool TextEditView::OnCreate(wxDocument *doc, long flags)
{
    if ( !wxView::OnCreate(doc, flags) )
        return false;

    wxFrame* frame = wxGetApp().CreateChildFrame(this, false);
    wxASSERT(frame == GetFrame());
    m_text = new wxTextCtrl(frame, wxID_ANY, "",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE);
    frame->Show();

    return true;
}

void TextEditView::OnDraw(wxDC *WXUNUSED(dc))
{
    // nothing to do here, wxTextCtrl draws itself
}

bool TextEditView::OnClose(bool deleteWindow)
{
    if ( !wxView::OnClose(deleteWindow) )
        return false;

    Activate(false);

    if ( deleteWindow )
    {
        GetFrame()->Destroy();
        SetFrame(NULL);
    }
    return true;
}

// ----------------------------------------------------------------------------
// ImageCanvas implementation
// ----------------------------------------------------------------------------

// Define a constructor for my canvas
ImageCanvas::ImageCanvas(wxView* view)
    : wxScrolledWindow(view->GetFrame())
{
    m_view = view;
    SetScrollRate( 10, 10 );
}

// Define the repainting behaviour
void ImageCanvas::OnDraw(wxDC& dc)
{
    if ( m_view )
        m_view->OnDraw(& dc);
}

// ----------------------------------------------------------------------------
// ImageView implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(ImageView, wxView);

ImageDocument* ImageView::GetDocument()
{
    return wxStaticCast(wxView::GetDocument(), ImageDocument);
}

bool ImageView::OnCreate(wxDocument* doc, long flags)
{
    if ( !wxView::OnCreate(doc, flags) )
        return false;

    wxFrame* frame = wxGetApp().CreateChildFrame(this, false);
    wxASSERT(frame == GetFrame());
    m_canvas = new ImageCanvas(this);
    frame->Show();

    return true;
}

void ImageView::OnUpdate(wxView* sender, wxObject* hint)
{
    wxView::OnUpdate(sender, hint);
    wxImage image = GetDocument()->GetImage();
    if ( image.IsOk() )
    {
        m_canvas->SetVirtualSize(image.GetWidth(), image.GetHeight());
    }
}

void ImageView::OnDraw(wxDC* dc)
{
    wxImage image = GetDocument()->GetImage();
    if ( image.IsOk() )
    {
        dc->DrawBitmap(wxBitmap(image), 0, 0, true /* use mask */);
    }
}

bool ImageView::OnClose(bool deleteWindow)
{
    if ( !wxView::OnClose(deleteWindow) )
        return false;

    Activate(false);

    if ( deleteWindow )
    {
        GetFrame()->Destroy();
        SetFrame(NULL);
    }
    return true;
}

// ----------------------------------------------------------------------------
// ImageDetailsView
// ----------------------------------------------------------------------------

ImageDetailsView::ImageDetailsView(ImageDetailsDocument *doc)
                : wxView()
{
    SetDocument(doc);

    m_frame = wxGetApp().CreateChildFrame(this, false);
    m_frame->SetTitle("Image Details");

    wxPanel * const panel = new wxPanel(m_frame);
    wxFlexGridSizer * const sizer = new wxFlexGridSizer(2, wxSize(5, 5));
    const wxSizerFlags
        flags = wxSizerFlags().Align(wxALIGN_CENTRE_VERTICAL).Border();

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Image &file:"), flags);
    sizer->Add(new wxStaticText(panel, wxID_ANY, doc->GetFilename()), flags);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Image &type:"), flags);
    wxString typeStr;
    switch ( doc->GetType() )
    {
        case wxBITMAP_TYPE_PNG:
            typeStr = "PNG";
            break;

        case wxBITMAP_TYPE_JPEG:
            typeStr = "JPEG";
            break;

        default:
            typeStr = "Unknown";
    }
    sizer->Add(new wxStaticText(panel, wxID_ANY, typeStr), flags);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Image &size:"), flags);
    wxSize size = doc->GetSize();
    sizer->Add(new wxStaticText(panel, wxID_ANY,
                                wxString::Format("%d*%d", size.x, size.y)),
               flags);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Number of unique &colours:"),
               flags);
    sizer->Add(new wxStaticText(panel, wxID_ANY,
                                wxString::Format("%lu", doc->GetNumColours())),
               flags);

    sizer->Add(new wxStaticText(panel, wxID_ANY, "Uses &alpha:"), flags);
    sizer->Add(new wxStaticText(panel, wxID_ANY,
                                doc->HasAlpha() ? "Yes" : "No"), flags);

    panel->SetSizer(sizer);
    m_frame->SetClientSize(panel->GetBestSize());
    m_frame->Show(true);
}

void ImageDetailsView::OnDraw(wxDC * WXUNUSED(dc))
{
    // nothing to do here, we use controls to show our information
}

bool ImageDetailsView::OnClose(bool deleteWindow)
{
    if ( deleteWindow )
    {
        delete m_frame;
        m_frame = NULL;
    }

    return true;
}

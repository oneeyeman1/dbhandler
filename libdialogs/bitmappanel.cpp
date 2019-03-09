/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/mainframe.cpp
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
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

#include "bitmappanel.h"

BitmapPanel::BitmapPanel (wxWindow *parent, const wxBitmap &bitmap, const wxString &label) : wxPanel(parent)
{
    m_label = new wxStaticText( this, wxID_ANY, label );
}
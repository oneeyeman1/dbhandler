// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif
#include <string>
#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/docview.h"
#include "painterobjects.h"
#include "librarydocument.h"

// ----------------------------------------------------------------------------
// DrawingDocument implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(LibraryDocument, wxDocument);

LibraryDocument::~LibraryDocument()
{
//    m_successfulLoad = false;
}


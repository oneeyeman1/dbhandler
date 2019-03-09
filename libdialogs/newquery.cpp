// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "res/Quick_Select.xpm"
#include "res/SQL_Select.xpm"
#include "bitmappanel.h"
#include "newquery.h"

NewQuery::NewQuery(wxWindow *parent, const wxString &title) : wxDialog(parent, wxID_ANY, title)
{
    m_panels[0] = new BitmapPanel( this, wxBitmap( Quick_Select ), _( "Quick Select" ) );
    m_panels[1] = new BitmapPanel( this, wxBitmap( SQL_Select ), _( "SQL Select" ) );
    SetTitle( title );
}

NewQuery::~NewQuery()
{
}

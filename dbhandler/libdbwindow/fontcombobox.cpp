// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/fontenum.h"
#include "wx/bmpcbox.h"
#include "res/gui/opentype.h"
#include "fontcombobox.h"

FontComboBox::FontComboBox(wxWindow *parent) : wxBitmapComboBox(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_SORT)
{
#ifdef __WXMSW__
    PopuateFontNames();
#else
    m_enumerator.EnumerateFacenames();
    if( m_enumerator.GotAny () )
    {
        unsigned long count = m_enumerator.GetFacenames().GetCount();
        for( int i = 0; i < count; ++i )
            Append( m_enumerator.GetFacenames().Item( i ) );
    }
#endif
    SetSelection( 0 );
}

void FontComboBox::PopuateFontNames()
{
#ifdef __WXMSW__
    HDC dc = ::GetDC( NULL );
    EnumFontFamilies( dc, NULL, (FONTENUMPROC) FontComboBox::EnumFontFamiliesCallback, (LPARAM) this );
    ::ReleaseDC( NULL, dc );
#endif
}
#ifdef __WXMSW__
int CALLBACK FontComboBox::EnumFontFamiliesCallback(ENUMLOGFONT *lpelf, NEWTEXTMETRIC *, int FontType, LPARAM lParam)
{
    wxBitmap bmp = wxBitmap::NewFromPNGData( opentype_png, WXSIZEOF( opentype_png ) );
    FontComboBox *view = (FontComboBox *) lParam;
    wxASSERT( view );
    view->Append( lpelf->elfLogFont.lfFaceName, FontType == TRUETYPE_FONTTYPE ? wxBitmap::NewFromPNGData( opentype_png, WXSIZEOF( opentype_png ) ) : wxNullBitmap, &FontType );
    return 1;
}
#endif

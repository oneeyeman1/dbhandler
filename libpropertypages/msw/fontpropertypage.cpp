// -*- C++ -*- generated by wxGlade 0.5 on Wed Feb 06 22:39:49 2008
/***************************************************************************
 *   Copyright (C) 2005 by Igor Korot                                      *
 *   igor@IgorsGentoo                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "res/truetype.xpm"
//#include "res/opentype.xpm"
#include "wx/msw/private.h"
#include "wx/msw/dcclient.h"
#include "wx/fontenum.h"
#include "wx/gbsizer.h"
#include "wx/imaglist.h"
#include "wx/font.h"
#include "wx/vector.h"
#include "wx/bmpcbox.h"
#include "colorcombobox.h"
#include "propertypagebase.h"
#include "fontpropertypagebase.h"

CFontNamesComboBox::CFontNamesComboBox(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, int n, const wxString choices[], long style )
	: wxBitmapComboBox( parent, id, value, pos, size, n, choices, style )
{
    m_bmp1 = new wxBitmap( truetype );
    m_bmp2 = NULL;
}

CFontNamesComboBox::~CFontNamesComboBox()
{
    if( m_bmp1 )
    {
        delete m_bmp1;
        m_bmp1 = NULL;
    }
    if( m_bmp2 )
    {
        delete m_bmp2;
        m_bmp2 = NULL;
    }
}

int CFontNamesComboBox::AddFont( LOGFONT *plf, int type )
{
    if( FindString( plf->lfFaceName ) != wxNOT_FOUND )
        return wxNOT_FOUND;
    if( type == TRUETYPE_FONTTYPE )
    {
        Append( plf->lfFaceName, *m_bmp1, &type );
    }
    else
    {
        Append( plf->lfFaceName, wxNullBitmap, &type );
    }
    SetSize( GetBestSize() );
    return 1;
}

wxSize CFontNamesComboBox::DoGetBestSize() const
{
    int hBitmap = 0;
    int wChoice = 0;
    int hChoice;
    const unsigned int nItems = GetCount();
    for( unsigned int i = 0; i < nItems; i++ )
    {
        int wLine;
        GetTextExtent( GetString( i ), &wLine, NULL );
        if( wLine > wChoice )
            wChoice = wLine;
    }
    if( wChoice == 0 )
        wChoice = 100;
    wChoice += 5 * GetCharWidth();
    if( m_bmp1 )
    {
        wChoice += m_bmp1->GetWidth();
        hBitmap = m_bmp1->GetHeight();
    }
    int cx, cy;
    wxGetCharSize( GetHWND(), &cx, &cy, GetFont() );
    if( hBitmap > cy )
        cy = hBitmap;
    int hItem = SendMessage( GetHwnd(), CB_GETITEMHEIGHT, (WPARAM) -1, 0 );
    if( hItem > cy )
        hItem = cy;
    SendMessage( GetHwnd(), CB_SETITEMHEIGHT, (WPARAM) -1, hItem );
    hChoice = ( EDIT_HEIGHT_FROM_CHAR_HEIGHT( cy ) * 6 ) + hItem - 6;
    wxSize best( wChoice, hChoice );
    CacheBestSize( best );
    return best;
}

void CFontNamesComboBox::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    int heightOrig = height;
    wxSize bitmapSize( 0, 0 );
    if( height != wxDefaultCoord )
    {
        int cx, cy;
        wxGetCharSize( GetHWND(), &cx, &cy, GetFont() );
        int hItem = SendMessage( GetHwnd(), CB_GETITEMHEIGHT, (WPARAM) -1, 0 );
        if( m_bmp1 )
            bitmapSize = m_bmp1->GetSize();
        height = ( EDIT_HEIGHT_FROM_CHAR_HEIGHT( cy ) * 6 ) + hItem - 6;
    }
    wxBitmapComboBox::DoSetSize( x, y, width, height, sizeFlags );
    if( m_pendingSize != wxDefaultSize )
        m_pendingSize = wxSize( width, heightOrig );
}

CStyleComboBox::CStyleComboBox(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, int n, const wxString choices[], long style)
	: wxComboBox( parent, id, value, pos, size, n, choices, style )
{
}

wxSize CStyleComboBox::DoGetBestSize() const
{
    int wChoice = 0;
    int hChoice;
    const unsigned int nItems = GetCount();
    for( unsigned int i = 0; i < nItems; i++ )
    {
        int wLine;
        GetTextExtent( GetString( i ), &wLine, NULL );
        if( wLine > wChoice )
            wChoice = wLine;
    }
    if( wChoice == 0 )
        wChoice = 100;
    wChoice += 5 * GetCharWidth();
    int cx, cy;
    wxGetCharSize( GetHWND(), &cx, &cy, GetFont() );
    int hItem = SendMessage( GetHwnd(), CB_GETITEMHEIGHT, (WPARAM) -1, 0 );
    hChoice = ( EDIT_HEIGHT_FROM_CHAR_HEIGHT( cy ) * 6 ) + hItem  - 6;
    wxSize best( wChoice, hChoice );
    CacheBestSize( best );
    return best;
}

void CStyleComboBox::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    int heightOrig = height;
    if( height != wxDefaultCoord )
    {
        int cx, cy;
        wxGetCharSize( GetHWND(), &cx, &cy, GetFont() );
        int hItem = SendMessage( GetHwnd(), CB_GETITEMHEIGHT, (WPARAM) -1, 0 );
        height = ( EDIT_HEIGHT_FROM_CHAR_HEIGHT( cy ) * 6 ) + hItem;
    }
    wxControl::DoSetSize( x, y, width, height, sizeFlags );
    if( m_pendingSize != wxDefaultSize )
        m_pendingSize = wxSize( width, heightOrig );
}

CSizeComboBox::CSizeComboBox(wxWindow *parent, wxWindowID id, const wxString &value, const wxPoint &pos, const wxSize &size, int n, const wxString choices[], long style)
	: wxComboBox( parent, id, value, pos, size, n, choices, style )
{
    HDC dc = ::GetDC( NULL );
    m_cyppi = ::GetDeviceCaps( dc, LOGPIXELSY );
}

int CSizeComboBox::AddSize(int size, int lfHeight)
{
    int pointSize;
    if( lfHeight != 0 )
        pointSize = ::MulDiv( size, 72, m_cyppi );
    else
        pointSize = size;
    if( lfHeight == 0 )
        lfHeight = ::MulDiv( -m_cyppi, size, 72 );
    int max = GetCount(), i;
    wxString str;
    wxStringBuffer( str, 16 );
    str.Printf( "%d", pointSize );
    if( lfHeight > 0 )
    {
        for( i = 0; i < max; i++ )
        {
            int iComp = (int)( lfHeight - GetFontHeight() );
            if( !iComp )
                return CB_ERR;
            if( iComp < 0 )
                break;
        }
    }
    else
    {
        for( i = 0; i < max; i++ )
        {
            int iComp = (int)( lfHeight - GetFontHeight() );
            if( !iComp )
                return CB_ERR;
            if( iComp > 0 )
                break;
        }
    }
    if( i == max )
        i = Append( str, &lfHeight );
    else
        i = Insert( str, i, &lfHeight );
    return i;
}

LONG CSizeComboBox::GetFontHeight()
{
    return GetSelection();
}

wxSize CSizeComboBox::DoGetBestSize() const
{
    int wChoice = 0;
    int hChoice;
    const unsigned int nItems = GetCount();
    for( unsigned int i = 0; i < nItems; i++ )
    {
        int wLine;
        GetTextExtent( GetString( i ), &wLine, NULL );
        if( wLine > wChoice )
            wChoice = wLine;
    }
    if( wChoice == 0 )
        wChoice = 100;
    wChoice += 5 * GetCharWidth();
    int cx, cy;
    wxGetCharSize( GetHWND(), &cx, &cy, GetFont() );
    int hItem = SendMessage( GetHwnd(), CB_GETITEMHEIGHT, (WPARAM) -1, 0 );
    hChoice = ( EDIT_HEIGHT_FROM_CHAR_HEIGHT( cy ) * 6 ) + hItem  - 6;
    wxSize best( wChoice, hChoice );
    CacheBestSize( best );
    return best;
}

void CSizeComboBox::DoSetSize( int x, int y, int width, int height, int sizeFlags )
{
    int heightOrig = height;
    if( height != wxDefaultCoord )
    {
        int cx, cy;
        wxGetCharSize( GetHWND(), &cx, &cy, GetFont() );
        int hItem = SendMessage( GetHwnd(), CB_GETITEMHEIGHT, (WPARAM) -1, 0 );
        height = ( EDIT_HEIGHT_FROM_CHAR_HEIGHT( cy ) * 6 ) + hItem;
    }
    wxControl::DoSetSize( x, y, width, height, sizeFlags );
    if( m_pendingSize != wxDefaultSize )
        m_pendingSize = wxSize( width, heightOrig );
}

CFontPropertyPage::CFontPropertyPage(wxWindow* parent, FontPropertyPage &font, bool colorEnabled)
 : CFontPropertyPageBase(parent, font)
{
    m_bUnderline = false;
    m_bStrikethrough = false;
    wxString text;
    text = "AaBbYyZz";
    m_font = font;
    if( m_font.font.IsOk() )
    {
        m_fontSize = wxString::Format( "%d", m_font.font.GetPointSize() );
        if( m_font.font.GetStyle() == wxFONTSTYLE_ITALIC && m_font.font.GetWeight() == wxFONTWEIGHT_BOLD )
            m_nCurrentStyle = NTM_ITALIC | NTM_BOLD;
        else if( m_font.font.GetStyle() == wxFONTSTYLE_ITALIC )
            m_nCurrentStyle = NTM_ITALIC;
        else if( m_font.font.GetWeight() == wxFONTWEIGHT_BOLD )
            m_nCurrentStyle = NTM_BOLD;
        else
            m_nCurrentStyle = NTM_REGULAR;
        if( m_font.font.GetUnderlined() )
            m_bUnderline = true;
        else
            m_bUnderline = false;
        if( m_font.font.GetStrikethrough() )
            m_bStrikethrough = true;
		else
            m_bStrikethrough = false;
    }
    else
    {
        m_fontSize = wxEmptyString;
        m_bUnderline = false;
        m_bStrikethrough = false;
    }
    m_nActualStyle = m_nCurrentStyle;
    m_textStr = m_font.text;
    m_backgroundStr = m_font.back;
    itemStaticText6 = new wxStaticText( this, wxID_STATIC, _T( "&Font:" ), wxDefaultPosition, wxDefaultSize, 0 );
    itemChoice7 = new CFontNamesComboBox( this, wxID_ANY );
    itemStaticText9 = new wxStaticText( this, wxID_ANY, _T("Font St&yles:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemChoice10 = new CStyleComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_SIMPLE );
    itemStaticText18 = new wxStaticText( this, wxID_ANY, _T("&Size:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemChoice19 = new CSizeComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_SIMPLE );
    itemCheckBox1 = new wxCheckBox( this, wxID_ANY, _T( "&Underline" ) );
    itemCheckBox2 = new wxCheckBox( this, wxID_ANY, _T( "Stri&keout" ) );
    itemStaticBox1 = new wxStaticBox( this, wxID_ANY, _T( "Effects" ) );
    itemStaticBox2 = new wxStaticBox( this, wxID_ANY, _T( "Sample" ) );
    itemStaticText15 = new wxStaticText( this, wxID_STATIC, _T( "&Text Color:" ), wxDefaultPosition, wxDefaultSize, 0 );
    itemChoice16 = new CColorComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize );
    itemStaticText23 = new wxStaticText( this, wxID_STATIC, _T("&Background:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemChoice17 = new CColorComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize );
    itemWindow24 = new wxFontPreviewer( this, text );
//    itemWindow24->SetMinSize( wxSize( 130, 32 ) );
    itemStaticText30 = new wxStaticText( this, wxID_STATIC, _T( "" ), wxDefaultPosition, wxDefaultSize, 0 );
    do_layout();
    set_properties();
    if( colorEnabled )
    {
        itemChoice16->Enable( true );
        itemChoice17->Enable( true );
    }
    else
    {
        itemChoice16->Enable( false );
        itemChoice17->Enable( false );
    }
    m_dirty = false;
    itemWindow24->SetFont( m_font );
    itemWindow24->Refresh();
    itemChoice7->Bind( wxEVT_COMBOBOX, &CFontPropertyPage::OnChangeFont, this );
    itemChoice10->Bind( wxEVT_COMBOBOX, &CFontPropertyPage::OnChangeFont, this );
    itemChoice19->Bind( wxEVT_COMBOBOX, &CFontPropertyPage::OnChangeFont, this );
    itemCheckBox2->Bind( wxEVT_CHECKBOX, &CFontPropertyPage::OnChangeFont, this );
    itemCheckBox1->Bind( wxEVT_CHECKBOX, &CFontPropertyPage::OnChangeFont, this );
}

CFontPropertyPage::~CFontPropertyPage()
{
}

void CFontPropertyPage::do_layout()
{
    wxBoxSizer* sizer1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer2 = new wxBoxSizer( wxVERTICAL );
    sizer1->Add( 5, 5, 0, wxEXPAND|wxGROW|wxALL, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND|wxGROW|wxALL, 0 );
//    wxGridBagSizer *sizer_3 = new wxGridBagSizer();
    wxFlexGridSizer *sizer_3 = new wxFlexGridSizer( 2, 3, 5, 5 );
    sizer_3->SetFlexibleDirection( wxBOTH );
    sizer_3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    wxStaticBoxSizer *sizer_4 = new wxStaticBoxSizer( itemStaticBox1, wxVERTICAL );
    wxStaticBoxSizer *sizer_5 = new wxStaticBoxSizer( itemStaticBox2, wxVERTICAL );
    wxBoxSizer *sizer_6 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer_7 = new wxBoxSizer( wxVERTICAL );
//    wxFlexGridSizer *sizer_8 = new wxFlexGridSizer( 2, 2, 5, 5 );
    wxBoxSizer *sizer_8 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_9 = new wxBoxSizer( wxHORIZONTAL );
    sizer_3->Add( itemStaticText6, 0, wxEXPAND, 0 );
    sizer_3->Add( itemStaticText9, 0, wxEXPAND, 0 );
    sizer_3->Add( itemStaticText18, 0, wxEXPAND, 0 );
    sizer_3->Add( itemChoice7, 0, wxEXPAND, 0 );
    sizer_3->Add( itemChoice10, 0, wxEXPAND, 0 );
    sizer_3->Add( itemChoice19, 0, wxEXPAND, 0 );
    sizer2->Add( sizer_3, 1, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND|wxGROW|wxALL, 0 );
    sizer_4->Add( itemCheckBox1, 0, wxEXPAND, 0 );
    sizer_4->Add( 20, 5, 0, wxEXPAND, 0 );
    sizer_4->Add( itemCheckBox2, 0, wxEXPAND, 0 );
    sizer_8->Add( sizer_4, 0, wxEXPAND, 0 );
    sizer_5->Add( itemWindow24, 1, wxGROW | wxALL, 5 );
    sizer_8->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_8->Add( sizer_5, 1, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( sizer_8, 0, wxEXPAND, 0 );
    sizer_6->Add( itemStaticText15, 0, wxEXPAND, 0 );
    sizer_6->Add( 20, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( itemChoice16, 0, wxEXPAND, 0 );
    sizer_9->Add( sizer_6, 0, wxEXPAND, 0 );
    sizer_9->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_7->Add( itemStaticText23, 0, wxEXPAND, 0 );
    sizer_7->Add( 20, 5, 0, wxEXPAND, 0 );
    sizer_7->Add( itemChoice17, 0, wxEXPAND, 0 );
    sizer_9->Add( sizer_7, 0, wxEXPAND, 0 );
    sizer2->Add( sizer_9, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND|wxGROW|wxALL, 0 );
    sizer1->Add( sizer2, 0, wxEXPAND|wxGROW|wxALL, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND|wxGROW|wxALL, 0 );
    SetSizerAndFit( sizer1 );
/*    wxGridBagSizer *sizer3 = new wxGridBagSizer( 0, 0 );
    sizer3->SetFlexibleDirection( wxBOTH );*/
//    sizer1->Fit( this );
//    Layout();
}

void CFontPropertyPage::set_properties()
{
    FillFacenameList();
    itemChoice10->AppendString( "Regular" );
    itemChoice10->AppendString( "Italic" );
    itemChoice10->AppendString( "Bold" );
    itemChoice10->AppendString( "Bold Italic" );
    if( m_font.font.IsOk() )
    {
        int sel = itemChoice7->FindString( m_font.font.GetFaceName() );
        if( sel != wxNOT_FOUND )
        {
            itemChoice7->SetSelection( sel );
            FillSizeList();
        }
    }
    itemChoice16->SetColourValue( m_textStr );
    itemChoice17->SetColourValue( m_backgroundStr );
    if( m_bUnderline )
        itemCheckBox1->Enable( true );
    if( m_bStrikethrough )
        itemCheckBox2->Enable( true );
    if( m_font.font.IsOk() )
    {
        if( m_font.font.GetStyle() == wxFONTSTYLE_ITALIC && m_font.font.GetWeight() == wxFONTWEIGHT_BOLD )
            itemChoice10->SetSelection( itemChoice10->FindString( "Bold Italic" ) );
        else if( m_font.font.GetStyle() == wxFONTSTYLE_ITALIC )
            itemChoice10->SetSelection( itemChoice10->FindString( "Italic" ) );
        else if( m_font.font.GetWeight() == wxFONTWEIGHT_BOLD )
            itemChoice10->SetSelection( itemChoice10->FindString( "Bold" ) );
        else
            itemChoice10->SetSelection( itemChoice10->FindString( "Regular" ) );
    }
    else
    {
        itemChoice10->SetSelection( itemChoice10->FindString( "Regular" ) );
        itemChoice19->SetSelection( itemChoice19->FindString( "0" ) );
    }
}

void CFontPropertyPage::OnChangeFont(wxCommandEvent &event)
{
    if( !m_dirty )
        m_dirty = true;
    if( event.GetEventObject() == itemChoice7 )
    {
        m_faceName = itemChoice7->GetValue();
        FillSizeList();
    }
    if( event.GetEventObject() == itemChoice10 )
    {
        wxString style = itemChoice10->GetValue();
        if( style == "Bold" || style == "Bold Italic" )
        {
            m_weight = wxFONTWEIGHT_BOLD;
            m_font.font.MakeBold();
        }
        if( style == "Italic" || style == "Bold Italic" )
        {
            m_style = wxFONTSTYLE_ITALIC;
            m_font.font.MakeItalic();
        }
        if( style == "Regular" )
        {
            m_weight = wxFONTWEIGHT_NORMAL;
            m_style = wxFONTSTYLE_NORMAL;
            m_font.font.SetStyle( wxFONTSTYLE_NORMAL );
            m_font.font.SetWeight( wxFONTWEIGHT_NORMAL );
        }
    }
    if( event.GetEventObject() == itemChoice19 )
    {
        m_ptSize = wxAtoi( itemChoice19->GetValue() );
        m_font.font.SetPointSize( wxAtoi( itemChoice19->GetValue() ) );
    }
    if( event.GetEventObject() == itemCheckBox1 )
    {
        m_striken = itemCheckBox1->GetValue();
        m_font.font.SetUnderlined( itemCheckBox1->GetValue() );
    }
    if( event.GetEventObject() == itemCheckBox2 )
    {
        m_underline = itemCheckBox2->GetValue();
        m_font.font.SetStrikethrough( itemCheckBox2->GetValue() );
    }
    itemWindow24->SetFont( m_font );
    itemWindow24->Refresh();
    GetParent()->FindWindowById( wxID_APPLY )->Enable();
}

void CFontPropertyPage::FillSizeList()
{
    itemChoice10->Clear();
    itemChoice19->Clear();
    wxString strFaceName = itemChoice7->GetStringSelection();
    int entry;
    wxArrayInt ttSizes;
    ttSizes.Add( 8 );
    ttSizes.Add( 9 );
    ttSizes.Add( 10 );
    ttSizes.Add( 11 );
    ttSizes.Add( 12 );
    ttSizes.Add( 14 );
    ttSizes.Add( 16 );
    ttSizes.Add( 18 );
    ttSizes.Add( 20 );
    ttSizes.Add( 22 );
    ttSizes.Add( 24 );
    ttSizes.Add( 26 );
    ttSizes.Add( 28 );
    ttSizes.Add( 36 );
    ttSizes.Add( 48 );
    ttSizes.Add( 72 );
    HDC dc = ::GetDC( NULL );
    EnumFontFamilies( dc, strFaceName, (FONTENUMPROC) CFontPropertyPage::EnumFontFamiliesCallback2, (LPARAM) this );
    ::ReleaseDC( NULL, dc );
    if( (int) itemChoice7->GetClientData( itemChoice7->GetSelection() ) != RASTER_FONTTYPE )
    {
        for( unsigned int i = 0; i < ttSizes.GetCount(); i++ )
            itemChoice19->AddSize( ttSizes[i], 0 );
    }
    BOOL bRegular = (BOOL)( m_nStyles & NTM_REGULAR );
    BOOL bBold = (BOOL)( m_nStyles & NTM_BOLD );
    BOOL bItalic = (BOOL)( m_nStyles & NTM_ITALIC );
    BOOL bBoldItalic = (BOOL)( ( m_nStyles & NTM_BOLD ) && ( m_nStyles & NTM_ITALIC ) );
    if (bRegular)
        bBold = bItalic = TRUE;
    if (bBold || bItalic)
        bBoldItalic = TRUE;
    if( bRegular )
    {
        entry = itemChoice10->Append( "Regular" );
        itemChoice10->SetClientData( entry, (void *) NTM_REGULAR );
    }
    if( bBold )
    {
        entry = itemChoice10->Append( "Bold" );
        itemChoice10->SetClientData( entry, (void *) NTM_BOLD );
    }
    if( bItalic )
    {
        entry = itemChoice10->Append( "Italic" );
        itemChoice10->SetClientData( entry, (void *) NTM_ITALIC );
    }
    if( bBoldItalic )
    {
        entry = itemChoice10->Append( "Bold Italic" );
        itemChoice10->SetClientData( entry, (void *) ( NTM_BOLD | NTM_ITALIC ) );
    }
    int sel = itemChoice19->FindString( m_fontSize );
    if( sel != wxNOT_FOUND )
        itemChoice19->SetSelection( sel );
    else
        itemChoice19->SetValue( m_fontSize );
    bool found = false;
    for( unsigned int i = 0; i < itemChoice10->GetCount(); i++ )
    {
        if( (DWORD) itemChoice10->GetClientData( i ) == m_nActualStyle )
        {
            itemChoice10->SetSelection( i );
            found = true;
        }
    }
    if( !found )
    {
        itemChoice10->SetSelection( 0 );
        m_nCurrentStyle = NTM_REGULAR;
    }
    else
        m_nCurrentStyle = m_nActualStyle;
    if( m_bUnderline )
        itemCheckBox1->SetValue( true );
    if( m_bStrikethrough )
        itemCheckBox2->SetValue( true );
    UpdateSampleFont();
}

int CALLBACK CFontPropertyPage::EnumFontFamiliesCallback(ENUMLOGFONT *lpelf, NEWTEXTMETRIC *, int FontType, LPARAM lParam)
{
    CFontPropertyPage *pPage = (CFontPropertyPage *) lParam;
    wxASSERT( pPage );
    pPage->itemChoice7->AddFont( &lpelf->elfLogFont, FontType );
    return 1;
}

int CALLBACK CFontPropertyPage::EnumFontFamiliesCallback2(ENUMLOGFONT *lpelf, NEWTEXTMETRIC *lpntm, int FontType, LPARAM lParam)
{
    CFontPropertyPage *pPage = (CFontPropertyPage *) lParam;
    wxASSERT( pPage );
    if( FontType & TRUETYPE_FONTTYPE )
    {
        if( !( lpntm->ntmFlags & ( NTM_BOLD | NTM_ITALIC ) ) )
            pPage->m_nStyles |= NTM_REGULAR;
        if( lpntm->ntmFlags & NTM_ITALIC )
            pPage->m_nStyles |= NTM_ITALIC;
        if( lpntm->ntmFlags & NTM_BOLD )
            pPage->m_nStyles |= NTM_BOLD;
    }
    else
    {
        if( FontType & RASTER_FONTTYPE )
        {
            int height = lpntm->tmHeight - lpntm->tmInternalLeading;
            pPage->itemChoice19->AddSize( height, height );
        }
        if( lpelf->elfLogFont.lfWeight >= FW_BOLD && lpelf->elfLogFont.lfItalic )
            pPage->m_nStyles |= NTM_BOLD | NTM_ITALIC;
        else if( lpelf->elfLogFont.lfWeight >= FW_BOLD )
            pPage->m_nStyles |= NTM_BOLD;
        else if( lpelf->elfLogFont.lfItalic )
            pPage->m_nStyles |= NTM_ITALIC;
        else
            pPage->m_nStyles |= NTM_REGULAR;
    }
    return 1;
}

void CFontPropertyPage::FillFacenameList()
{
    HDC dc = ::GetDC( NULL );
    EnumFontFamilies( dc, NULL, (FONTENUMPROC) CFontPropertyPage::EnumFontFamiliesCallback, (LPARAM) this );
    ::ReleaseDC( NULL, dc );
}

void CFontPropertyPage::UpdateSampleFont()
{
    m_dirty = true;
    bool isBold = false, isItalic = false, isUnderline = false;
//    wxString name = itemChoice7->GetSelection();
    int height = itemChoice19->GetFontHeight();
    if( m_nCurrentStyle & NTM_BOLD )
        isBold = true;
    if( m_nCurrentStyle & NTM_ITALIC )
        isItalic = true;
}

void CFontPropertyPage::GetData(void *WXUNUSED(data))
{
//    SetDirty( false );
}

void CFontPropertyPage::SetFont(const std::wstring &name, int size, bool italic, bool bold, bool underline, bool strikethrough)
{
/*    m_fontName = name;
    if( name == L"" )
    {
        m_font = wxFont::New( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "MS Sans Serif" );
    }
    else
    {
        m_font = wxFont::New( size, wxFONTFAMILY_DEFAULT, italic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL, bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL, underline, name );
        m_font->SetStrikethrough( strikethrough );
    }
    if( m_fontName != wxEmptyString )
    {
        itemChoice7->SetSelection( itemChoice7->FindString( m_fontName ) );
        if( bold && italic )
            itemChoice10->SetStringSelection( "Bold Italic" );
        else if( bold )
            itemChoice10->SetStringSelection( "Bold" );
        else if( italic )
            itemChoice10->SetStringSelection( "Italic" );
        else
            itemChoice10->SetStringSelection( "Regular" );
        if( underline )
            itemCheckBox1->SetValue( true );
        if( strikethrough )
            itemCheckBox2->SetValue( true );
    }*/
}

FontPropertyPage &CFontPropertyPage::GetFont()
{
    return m_font;
}

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/stockitem.h"
#endif

#if defined __WXMSW__ && defined __MEMORYLEAKS__
#include <vld.h>
#endif


#include <vector>
#include <map>
#include <mutex>
#include "wx/listctrl.h"
#include"database.h"
#if defined __WXGTK__
#include "wx/nativewin.h"
#elif defined __WXOSX__
#include "wx/fontpicker.h"
#endif
#include "wx/font.h"
#include "wx/fontutil.h"
#include "wx/notebook.h"
#include "wx/bmpcbox.h"
#include "wx/fontenum.h"
#include "wxsf/ShapeCanvas.h"
#include "colorcombobox.h"
#include "fieldwindow.h"
#include "propertypagebase.h"
#include "tablegeneral.h"
#include "fontpropertypagebase.h"
#include "tableprimarykey.h"
#include "propertieshandlerbase.h"
#include "propertieshandler.h"

#if _MSC_VER >= 1900 || !(defined __WXMSW__)
std::mutex Database::Impl::my_mutex;
#endif

DatabasePropertiesHandler::DatabasePropertiesHandler(const Database *db, DatabaseTable *table, wxTextCtrl *log) : PropertiesHandler()
{
    m_db = db;
    m_table = table;
    m_log = log;
    m_page1 = nullptr;
    m_page2 = m_page3 = m_page4 = nullptr;
    m_prop = table->GetTableProperties();
}

void DatabasePropertiesHandler::EditProperies(wxNotebook *parent)
{
    wxFont data_font( m_prop.m_dataFontSize, wxFONTFAMILY_DEFAULT, m_prop.m_dataFontItalic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL, m_prop.m_dataFontWeight ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL, m_prop.m_dataFontUnderline, m_prop.m_dataFontName );
    if( m_prop.m_dataFontStrikethrough )
        data_font.SetStrikethrough( true );
    wxFont heading_font( m_prop.m_headingFontSize, wxFONTFAMILY_DEFAULT, m_prop.m_headingFontItalic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL, m_prop.m_headingFontWeight ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL, m_prop.m_headingFontUnderline, m_prop.m_headingFontName );
    if( m_prop.m_headingFontStrikethrough )
        heading_font.SetStrikethrough( true );
    wxFont label_font( m_prop.m_labelFontSize, wxFONTFAMILY_DEFAULT, m_prop.m_labelFontItalic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL, m_prop.m_labelFontWeight ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL, m_prop.m_labelFontUnderline, m_prop.m_labelFontName );
    if( m_prop.m_labelFontStrikethrough )
        label_font.SetStrikethrough( true );
    FontPropertyPage dataFont;
    dataFont.font = data_font;
    dataFont.text = wxColour( *wxBLACK );
    dataFont.back = wxColour( *wxWHITE );
    FontPropertyPage headingFont;
    headingFont.font = heading_font;
    headingFont.text = wxColour( *wxBLACK );
    headingFont.back = wxColour( *wxWHITE );
    FontPropertyPage labelFont;
    labelFont.font = label_font;
    labelFont.text = wxColour( *wxBLACK );
    labelFont.back = wxColour( *wxWHITE );
    m_page1 = new TableGeneralProperty( parent, m_prop.table_name, m_prop.m_owner, m_prop.m_comment, DatabaseTableProperties );
    parent->AddPage( m_page1, _( "General" ) );
    m_page2 = new CFontPropertyPage( parent, dataFont, false );
    m_page3 = new CFontPropertyPage( parent, headingFont, false );
    m_page4 = new CFontPropertyPage( parent, labelFont, false );
    m_page5 = new TablePrimaryKey( parent, m_table );
    parent->AddPage( m_page2, _( "Data Font" ) );
    parent->AddPage( m_page3, _( "Heading Font" ) );
    parent->AddPage( m_page4, _( "Label Font" ) );
    parent->AddPage( m_page5, _( "Primary Key" ) );
}

int DatabasePropertiesHandler::GetProperties(std::vector<std::wstring> &errors)
{
    int result = 0;
    m_prop.m_comment = m_page1->GetCommentCtrl()->GetValue();
    m_prop.m_dataFontItalic = m_page2->GetFont().font.GetNativeFontInfo()->GetStyle() == wxFONTSTYLE_ITALIC;
    m_prop.m_dataFontCharacterSet = m_page2->GetFont().font.GetNativeFontInfo()->GetEncoding();
    m_prop.m_dataFontSize = m_page2->GetFont().font.GetNativeFontInfo()->GetPointSize();
    m_prop.m_dataFontName = m_page2->GetFont().font.GetNativeFontInfo()->GetFaceName();
    m_prop.m_dataFontWeight = m_page2->GetFont().font.GetNativeFontInfo()->GetNumericWeight();
    m_prop.m_dataFontEncoding = m_page2->GetFont().font.GetNativeFontInfo()->GetEncoding();
//    m_prop.m_dataFontPixelSize = m_page2->GetFont().GetNativeFontInfo()->GetPixelSize().GetWidth();
    m_prop.m_dataFontUnderline = m_page2->GetFont().font.GetNativeFontInfo()->GetUnderlined();
    m_prop.m_dataFontStrikethrough = m_page2->GetFont().font.GetNativeFontInfo()->GetStrikethrough();
    m_prop.m_headingFontItalic = m_page3->GetFont().font.GetNativeFontInfo()->GetStyle() == wxFONTSTYLE_ITALIC;
    m_prop.m_headingFontCharacterSet = m_page3->GetFont().font.GetNativeFontInfo()->GetEncoding();
    m_prop.m_headingFontSize = m_page3->GetFont().font.GetNativeFontInfo()->GetPointSize();
    m_prop.m_headingFontName = m_page3->GetFont().font.GetNativeFontInfo()->GetFaceName();
    m_prop.m_headingFontWeight = m_page3->GetFont().font.GetNativeFontInfo()->GetNumericWeight();
    m_prop.m_headingFontEncoding = m_page3->GetFont().font.GetNativeFontInfo()->GetEncoding();
//    m_prop.m_headingFontPixelSize = m_page3->GetFont().GetNativeFontInfo()->GetPixelSize().GetWidth();
    m_prop.m_headingFontUnderline = m_page3->GetFont().font.GetNativeFontInfo()->GetUnderlined();
    m_prop.m_headingFontStrikethrough = m_page3->GetFont().font.GetNativeFontInfo()->GetStrikethrough();
    m_prop.m_labelFontItalic = m_page4->GetFont().font.GetNativeFontInfo()->GetStyle() == wxFONTSTYLE_ITALIC;
    m_prop.m_labelFontCharacterSer = m_page4->GetFont().font.GetNativeFontInfo()->GetEncoding();
    m_prop.m_labelFontSize = m_page4->GetFont().font.GetNativeFontInfo()->GetPointSize();
    m_prop.m_labelFontName = m_page4->GetFont().font.GetNativeFontInfo()->GetFaceName();
    m_prop.m_labelFontWeight = m_page4->GetFont().font.GetNativeFontInfo()->GetNumericWeight();
    m_prop.m_labelFontEncoding = m_page4->GetFont().font.GetNativeFontInfo()->GetEncoding();
//    m_prop.m_labelFontPixelSize = m_page4->GetFont().GetNativeFontInfo()->GetPixelSize().GetWidth();
    m_prop.m_labelFontUnderline = m_page4->GetFont().font.GetNativeFontInfo()->GetUnderlined();
    m_prop.m_labelFontStrikethrough = m_page4->GetFont().font.GetNativeFontInfo()->GetStrikethrough();
    bool isLogOnly = m_page1->IsLogOnly();
    {
#if defined __WXMSW__ && _MSC_VER < 1900
        wxCriticalSectionLocker( *pcs );
#else
        //#if _MSC_VER >= 1900 || !(defined __WXMSW__)
        std::lock_guard<std::mutex> lock( const_cast<Database *>( m_db )->GetTableVector().my_mutex );
#endif
        result = const_cast<Database *>( m_db )->SetTableProperties( m_table, m_prop, isLogOnly, m_command, errors );
        if( !result && !isLogOnly )
            m_table->SetTableProperties( m_prop );
        if( isLogOnly )
            m_log->AppendText( m_command );
    }
    return result;
}

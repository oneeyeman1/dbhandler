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
    m_prop = table->GetTableProperties();
}

int DatabasePropertiesHandler::GetProperties(std::vector<std::wstring> &errors)
{
   int result = 0;
/*    m_prop.m_comment = m_page1->GetCommentCtrl()->GetValue();
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
    }*/
    return result;
}

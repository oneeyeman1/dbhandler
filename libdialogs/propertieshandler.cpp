// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/stockitem.h"
#endif

#include <vector>
#include <map>
#include"database.h"
#include "wx/font.h"
#include "wx/notebook.h"
#include "wx/bmpcbox.h"
#include "wx/fontenum.h"
#include "wx/nativewin.h"
#include "propertypagebase.h"
#include "tablegeneral.h"
#include "fontpropertypagebase.h"
#include "propertieshandler.h"

DatabasePropertiesHandler::DatabasePropertiesHandler(const Database *db, DatabaseTable *table) : PropertiesHandler()
{
    m_db = db;
    m_page1 = nullptr;
    m_page2 = m_page3 = m_page4 = nullptr;
    bool found = false;
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
    m_page1 = new TableGeneralProperty( parent, m_prop.table_name, m_prop.m_owner, m_prop.m_comment, DatabaseTableProperties );
    parent->AddPage( m_page1, _( "General" ) );
    m_page2 = new CFontPropertyPage( parent, data_font );
    m_page3 = new CFontPropertyPage( parent, heading_font );
    m_page4 = new CFontPropertyPage( parent, label_font );
    parent->AddPage( m_page2, _( "Data Font" ) );
    parent->AddPage( m_page3, _( "Heading Font" ) );
    parent->AddPage( m_page4, _( "Label Font" ) );
}

void DatabasePropertiesHandler::GetProperties ()
{

}
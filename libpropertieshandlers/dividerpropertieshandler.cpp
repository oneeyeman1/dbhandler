// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/stockitem.h"
#endif

#include "wx/notebook.h"
#include "wx/bmpcbox.h"
#include "objectproperties.h"
#include "propertypagebase.h"
#include "bandgeneral.h"
#include "propertieshandlerbase.h"
#include "dividerpropertieshandler.h"

DividerPropertiesHander::DividerPropertiesHander(BandProperties props) : PropertiesHandler()
{
    m_props = props;
}

void DividerPropertiesHander::EditProperies(wxNotebook *parent)
{
    m_page1 = new BandGeneralProperties( parent, m_props );
    parent->AddPage( m_page1, _( "General" ) );
}

int DividerPropertiesHander::GetProperties(std::vector<std::wstring> &errors)
{
    return 0;
}


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
#include "wx/any.h"
#include "propertieshandlerbase.h"
#include "dividerpropertieshandler.h"

DividerPropertiesHandler::DividerPropertiesHandler(BandProperties props) : PropertiesHandler()
{
    m_props = props;
}

wxAny DividerPropertiesHandler::GetProperties(std::vector<std::wstring> &WXUNUSED(errors))
{
    return wxAny( 0 );
}

int DividerPropertiesHandler::ApplyProperties()
{
    return 0;
}

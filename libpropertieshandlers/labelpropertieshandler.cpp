// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/any.h"
#include "propertieshandlerbase.h"
#include "labelpropertieshandler.h"

LabelPropertiesHandler::LabelPropertiesHandler(DesignLabelProperties *prop)
{
    m_prop = prop;
}

wxAny LabelPropertiesHandler::GetProperties(std::vector<std::wstring> &UNUSED(errors))
{
    return wxAny( 0 );
}

int LabelPropertiesHandler::ApplyProperties()
{
    return 0;
}

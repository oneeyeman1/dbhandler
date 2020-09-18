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
#include "objectproperties.h"
#include "propertieshandlerbase.h"
#include "designpropertieshandler.h"

DesignPropertiesHander::DesignPropertiesHander(DesignOptions *options)
{
    m_options = options;
}

void DesignPropertiesHander::EditProperies(wxNotebook *parent)
{

}

int DesignPropertiesHander::GetProperties(std::vector<std::wstring> &errors)
{
    return 0;
}

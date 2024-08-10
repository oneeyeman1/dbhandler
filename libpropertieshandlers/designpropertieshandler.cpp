// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/stockitem.h"
#endif
#include "database.h"
#include "wx/any.h"
#include "wx/notebook.h"
#include "wx/bmpcbox.h"
#include "wx/filepicker.h"
#include "wx/docview.h"
#include "propertieshandlerbase.h"
#include "designpropertieshandler.h"

DesignPropertiesHander::DesignPropertiesHander(DesignOptions canvas)
{
    m_options = canvas;
}

wxAny DesignPropertiesHander::GetProperties(std::vector<std::wstring> &WXUNUSED(errors))
{
//    m_options->GetOptions().units = m_page1->GetUnitsCtrl()->GetSelection();
/*    wxColour color = m_page1->GetColorCtrl()->GetColourValue();
    if( m_options.colorBackground != color )
    {
        m_options.colorBackground = color;
    }*/
    return wxAny( 0 );
}

int DesignPropertiesHander::ApplyProperties()
{
    return 0;
}
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
#include "wx/filepicker.h"
#include "propertypagebase.h"
#include "pointerproperty.h"
#include "objectproperties.h"
#include "colorcombobox.h"
#include "designgeneral.h"
#include "printspec.h"
#include "propertieshandlerbase.h"
#include "designpropertieshandler.h"

DesignPropertiesHander::DesignPropertiesHander(DesignOptions *options)
{
    m_options = options;
}

void DesignPropertiesHander::EditProperies(wxNotebook *parent)
{
    m_page1 = new DesignGeneral( parent, m_options );
    parent->AddPage( m_page1, _( "General" ) );
    m_page2 = new PointerPropertiesPanel( parent, m_options->cursorName, m_options->cursor );
    parent->AddPage( m_page2, _( "Pointer" ) );
    m_page3 = new PrintSpec( parent );
    parent->AddPage( m_page3, _( "Print Specification" ) );
}

int DesignPropertiesHander::GetProperties(std::vector<std::wstring> &errors)
{
    m_options->units = m_page1->GetUnitsCtrl()->GetSelection();
    m_options->colorBackground = m_page1->GetColorCtrl()->GetColourValue();
    return 0;
}

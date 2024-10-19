// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <map>
#include "wx/any.h"
#include "configuration.h"
#include "propertieshandlerbase.h"
#include "librarypainterpropertieshandler.h"

LibraryPainterPropertiesHandler::LibraryPainterPropertiesHandler( LibraryPainterOptions *prop )
{
    m_prop = prop;
}

wxAny LibraryPainterPropertiesHandler::GetProperties(std::vector<std::wstring> &errors)
{
    return m_prop;
}

int LibraryPainterPropertiesHandler::ApplyProperties()
{
    return 0;
}

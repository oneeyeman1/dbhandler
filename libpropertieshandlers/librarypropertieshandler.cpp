// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <vector>
#include "painterobjects.h"
#include "propertieshandlerbase.h"
#include "librarypropertieshandler.h"

LibraryPropertiesHandler::LibraryPropertiesHandler(LibraryProperty prop)
{
    m_properties = prop;
}

wxAny LibraryPropertiesHandler::GetProperties(std::vector<std::wstring> &errors)
{
    return m_obj;
}

int LibraryPropertiesHandler::ApplyProperties()
{
    return 0;
}

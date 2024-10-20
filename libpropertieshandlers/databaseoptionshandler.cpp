// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/stockitem.h"
#endif
#include <map>
#include "wx/any.h"
#include "configuration.h"
#include "propertieshandlerbase.h"
#include "databaseoptionshandler.h"

DatabaseOptionsHandler::DatabaseOptionsHandler(DatabaseOptions options)
{
    m_options = options;
}

wxAny DatabaseOptionsHandler::GetProperties(std::vector<std::wstring> &WXUNUSED(errors))
{
    return wxAny( 0 );
}

int DatabaseOptionsHandler::ApplyProperties()
{
    return 0; 
}

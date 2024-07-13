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
#include "configuration.h"
#include "propertieshandlerbase.h"
#include "databaseoptionshandler.h"

DatabaseOptionsHandler::DatabaseOptionsHandler(DatabaseOptions options)
{
    m_options = options;
}

int DatabaseOptionsHandler::GetProperties(std::vector<std::wstring> &WXUNUSED(errors))
{
    return 0;
}

/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tableeditdocument.cpp
 * Copyright (C) 2020 Igor Korot <igor@IgorReinCloud>
 *
 * dbhandler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * dbhandler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#include <string>
#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <map>
#include <memory>
#include "wx/docview.h"
#include "database.h"
#include "tableeditdocument.h"

wxIMPLEMENT_DYNAMIC_CLASS(TableEditDocument, wxDocument);

std::map<long,std::vector<int> > &TableEditDocument::GetData()
{
    return m_tableData;
}

void TableEditDocument::SetDatabaseAndTableName(Database *db)
{
    m_db = db;
    auto found = false;
    for( std::map<std::wstring, std::vector<DatabaseTable *> >::const_iterator it = db->GetTableVector().m_tables.begin(); it != db->GetTableVector().m_tables.begin() && !found; ++it )
    {
        for( std::vector<DatabaseTable *>::const_iterator it1 = ( *it ).second.begin(); it1 < ( *it ).second.end() && !found; ++it1 )
        {

        }
    }
}

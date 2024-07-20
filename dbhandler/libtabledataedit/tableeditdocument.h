/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tableeditdocument.h
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

#ifndef _TABLEEDITDOCUMENT_H_
#define _TABLEEDITDOCUMENT_H_

class TableEditDocument: public wxDocument 
{
public:
    std::map<long,std::vector<int> > &GetData();
    void SetTable(DatabaseTable *table) { m_table = table; }
    void SetDatabaseAndTableName(Database *db);
    const wxString &GetTableName() const { return m_name; }
    Database *GetDatabase() const { return m_db; }
    void SetDatabase(Database *db) { m_db = db; }
protected:

private:
    std::map<long,std::vector<int> > m_tableData;
    DatabaseTable *m_table;
    Database *m_db;
    wxString m_name;
    wxDECLARE_DYNAMIC_CLASS(TableEditDocument);
};

#endif // _TABLEEDITDOCUMENT_H_


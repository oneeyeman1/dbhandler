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

#include <vector>
#include <map>
#include <memory>
#if wxUSE_STD_IOSTREAM
    #include "wx/ioswrap.h"
#else
    #include "wx/txtstrm.h"
#endif
#include "wx/wfstream.h"
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/cmdproc.h"
#include "database.h"
#include "tabledoc.h"
#include "tableview.h"

// ----------------------------------------------------------------------------
// DrawingDocument implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(TableDocument, wxDocument);

TableDocument::~TableDocument()
{
}

DocumentOstream& TableDocument::SaveObject(DocumentOstream& ostream)
{
#if wxUSE_STD_IOSTREAM
    DocumentOstream& stream = ostream;
#else
    wxTextOutputStream stream( ostream );
#endif

    wxDocument::SaveObject( ostream );

    return ostream;
}

DocumentIstream& TableDocument::LoadObject(DocumentIstream& istream)
{
#if wxUSE_STD_IOSTREAM
    DocumentIstream& stream = istream;
#else
    wxTextInputStream stream( istream );
#endif

    wxDocument::LoadObject( istream );

    return istream;
}

void TableDocument::DoUpdate()
{
    Modify( true );
    UpdateAllViews();
}

void TableDocument::SetDatabase(Database *db, DatabaseTable *table)
{
    m_db = db;
    if( !table )
        dynamic_cast<TableView *>( GetFirstView() )->GetTablesForView( db );
}

Database *TableDocument::GetDatabase()
{
    return m_db;
}

void TableDocument::AddTables(const std::vector<wxString> &selections)
{
    bool found = false;
    std::map<std::wstring, std::vector<DatabaseTable *> > tables = m_db->GetTableVector().m_tables;
    std::vector<DatabaseTable *> tableVec = tables.at( m_db->GetTableVector().m_dbName );
    for( std::vector<wxString>::const_iterator it = selections.begin(); it < selections.end(); it++ )
    {
        for( std::vector<DatabaseTable *>::iterator it1 = tableVec.begin(); it1 < tableVec.end() && !found; it1++ )
        {
            if( (*it).ToStdWstring() == (*it1)->GetTableName() )
            {
                DatabaseTable *dbTable = (*it1);
                found = true;
            }
        }
        found = false;
    }
}

std::vector<std::wstring> &TableDocument::GetTableNames()
{
    return m_tableNames;
}


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
#if wxUSE_STD_IOSTREAM
    #include "wx/ioswrap.h"
#else
    #include "wx/txtstrm.h"
#endif
#include "wx/wfstream.h"
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/cmdproc.h"
#include "ErdForeignKey.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "column.h"
#include "constraint.h"
#include "GUIColumn.h"
#include "MyErdTable.h"
#include "databasecanvas.h"
#include "databasedoc.h"
#include "databaseview.h"

// ----------------------------------------------------------------------------
// DrawingDocument implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(DrawingDocument, wxDocument);

DrawingDocument::~DrawingDocument()
{
}

DocumentOstream& DrawingDocument::SaveObject(DocumentOstream& ostream)
{
#if wxUSE_STD_IOSTREAM
    DocumentOstream& stream = ostream;
#else
    wxTextOutputStream stream( ostream );
#endif

    wxDocument::SaveObject( ostream );

    const wxInt32 count = m_doodleSegments.size();
    stream << count << '\n';

    for( int n = 0; n < count; n++ )
    {
        m_doodleSegments[n].SaveObject( ostream );
        stream << '\n';
    }

    return ostream;
}

DocumentIstream& DrawingDocument::LoadObject(DocumentIstream& istream)
{
#if wxUSE_STD_IOSTREAM
    DocumentIstream& stream = istream;
#else
    wxTextInputStream stream( istream );
#endif

    wxDocument::LoadObject( istream );

    wxInt32 count = 0;
    stream >> count;
    if( count < 0 )
    {
        wxLogWarning( "Drawing document corrupted: invalid segments count." );
#if wxUSE_STD_IOSTREAM
        istream.clear( std::ios::badbit );
#else
        istream.Reset( wxSTREAM_READ_ERROR );
#endif
        return istream;
    }

    for( int n = 0; n < count; n++ )
    {
        DoodleSegment segment;
        segment.LoadObject( istream );
        m_doodleSegments.push_back( segment );
    }

    return istream;
}

void DrawingDocument::DoUpdate()
{
    Modify( true );
    UpdateAllViews();
}

void DrawingDocument::AddDoodleSegment(const DoodleSegment& segment)
{
    m_doodleSegments.push_back( segment );

    DoUpdate();
}

bool DrawingDocument::PopLastSegment(DoodleSegment *segment)
{
    if( m_doodleSegments.empty() )
        return false;

    if( segment )
        *segment = m_doodleSegments.back();

    m_doodleSegments.pop_back();

    DoUpdate();

    return true;
}

void DrawingDocument::SetDatabase(Database *db)
{
    m_db = db;
    dynamic_cast<DrawingView *>( GetFirstView() )->GetTablesForView( db );
}

Database *DrawingDocument::GetDatabase()
{
    return m_db;
}

void DrawingDocument::AddTables(const std::vector<wxString> &selections)
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
                MyErdTable *table = new MyErdTable( dbTable );
                m_tables.push_back( table );
				m_tableNames.push_back( table->GetTableName() );
                found = true;
            }
        }
        found = false;
    }
/*    for( std::vector<MyErdTable *>::iterator it2 = m_tables.begin(); it2 < m_tables.end(); it2++ )
    {
        std::map<int, std::vector<FKField> > foreignKeys = const_cast<DatabaseTable &>( (*it2)->GetTable() ).GetForeignKeyVector();
        for( std::map<int, std::vector<FKField> >::iterator it3 = foreignKeys.begin(); it3 != foreignKeys.end(); it3++ )
        {
            for( std::vector<FKField>::iterator it4 = (*it3).second.begin(); it4 < (*it3).second.end(); it4++ )
            {
                wxString referencedTableName = (*it4).GetReferencedTableName();
                Constraint* pConstr = new Constraint();
				pConstr->SetLocalColumn( (*it4).GetOriginalFieldName() );
				pConstr->SetRefCol( (*it4).GetReferencedFieldName() );
				pConstr->SetRefTable( referencedTableName );
                pConstr->SetType( Constraint::foreignKey );
//                pConstr->SetOnDelete( (Constraint::constraintAction) m_radioOnDelete->GetSelection() );
//                pConstr->SetOnUpdate( (Constraint::constraintAction) m_radioOnUpdate->GetSelection() );
                if( std::find( selections.begin(), selections.end(), referencedTableName ) != selections.end() )
                {
                    (*it2)->GetShapeManager()->CreateConnection( (*it2)->GetColumnId( (*it4).GetOriginalFieldName() ),
                                                                  GetReferencedTable( referencedTableName )->GetColumnId( (*it4).GetReferencedFieldName() ),
                                                                  new ErdForeignKey( pConstr ), sfDONT_SAVE_STATE );
                }
            }
        }
    }*/
}

MyErdTable *DrawingDocument::GetReferencedTable(const wxString &tableName)
{
    bool found = false;
    MyErdTable *table = NULL;
    for( std::vector<MyErdTable *>::iterator it2 = m_tables.begin(); it2 < m_tables.end() && !found; it2++ )
    {
        if( const_cast<DatabaseTable &>( (*it2)->GetTable() ).GetTableName() == tableName )
        {
            table = (*it2);
            found = true;
        }
    }
    return table;
}

std::vector<MyErdTable *> &DrawingDocument::GetTables()
{
    return m_tables;
}

std::vector<std::wstring> &DrawingDocument::GetTableNames()
{
    return m_tableNames;
}

// ----------------------------------------------------------------------------
// DoodleSegment implementation
// ----------------------------------------------------------------------------

DocumentOstream& DoodleSegment::SaveObject(DocumentOstream& ostream)
{
#if wxUSE_STD_IOSTREAM
    DocumentOstream& stream = ostream;
#else
    wxTextOutputStream stream(ostream);
#endif

    const wxInt32 count = m_lines.size();
    stream << count << '\n';

    for ( int n = 0; n < count; n++ )
    {
        const DoodleLine& line = m_lines[n];
        stream
            << line.x1 << ' '
            << line.y1 << ' '
            << line.x2 << ' '
            << line.y2 << '\n';
    }

    return ostream;
}

DocumentIstream& DoodleSegment::LoadObject(DocumentIstream& istream)
{
#if wxUSE_STD_IOSTREAM
    DocumentIstream& stream = istream;
#else
    wxTextInputStream stream(istream);
#endif

    wxInt32 count = 0;
    stream >> count;

    for ( int n = 0; n < count; n++ )
    {
        DoodleLine line;
        stream
            >> line.x1
            >> line.y1
            >> line.x2
            >> line.y2;
        m_lines.push_back(line);
    }

    return istream;
}
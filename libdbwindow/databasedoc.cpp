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
#include "wx/notebook.h"
#include "wx/grid.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "column.h"
#include "constraint.h"
#include "constraintsign.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "commenttableshape.h"
#include "MyErdTable.h"
#include "FieldShape.h"
#include "fieldwindow.h"
#include "wherehavingpage.h"
#include "syntaxproppage.h"
#include "databasecanvas.h"
#include "databasedoc.h"
#include "databaseview.h"
#include "ErdForeignKey.h"

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

    return istream;
}

void DrawingDocument::DoUpdate()
{
    Modify( true );
    UpdateAllViews();
}

void DrawingDocument::SetDatabase(Database *db, bool isInit)
{
    m_db = db;
    if( !isInit )
        dynamic_cast<DrawingView *>( GetFirstView() )->GetTablesForView( db, true );
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
                MyErdTable *table = new MyErdTable( dbTable, dynamic_cast<DrawingView *>( GetFirstView() )->GetViewType() );
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

std::vector<std::wstring> &DrawingDocument::GetTableNameVector()
{
    return m_tableNames;
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

void DrawingDocument::AppendFieldToQueryFields(const std::wstring &field)
{
    m_queryFields.push_back( field );
}

const std::vector<std::wstring> &DrawingDocument::GetQueryFields()
{
    return m_queryFields;
}

void DrawingDocument::AddRemoveField(const std::wstring &fieldName, bool isAdded)
{
    if( isAdded )
        m_queryFields.push_back( fieldName );
    else
        m_queryFields.erase( std::remove( m_queryFields.begin(), m_queryFields.end(), fieldName ), m_queryFields.end() );
}
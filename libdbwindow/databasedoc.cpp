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
#include "wx/dynlib.h"
#include "wx/fontenum.h"
#include "wx/docmdi.h"
#include "wx/cmdproc.h"
#include "wx/notebook.h"
#include "wx/fdrepdlg.h"
#include "wx/grid.h"
#include "wx/stc/stc.h"
#include "wx/bmpcbox.h"
#include "wx/listctrl.h"
#include "wx/dataview.h"
#include "wx/renderer.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "objectproperties.h"
#include "column.h"
#include "constraint.h"
#include "constraintsign.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "commenttableshape.h"
#include "fontcombobox.h"
#include "MyErdTable.h"
#include "FieldShape.h"
#include "field.h"
#include "fieldwindow.h"
#include "sortgroupbypage.h"
#include "wherehavingpage.h"
#include "syntaxproppage.h"
#include "databasecanvas.h"
#include "databasedoc.h"
#include "designcanvas.h"
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
    std::wstring dbType = m_db->GetTableVector().m_type;
    std::map<std::wstring, std::vector<DatabaseTable *> > tables = m_db->GetTableVector().m_tables;
    std::vector<DatabaseTable *> tableVec = tables.at( m_db->GetTableVector().m_dbName );
    for( std::vector<wxString>::const_iterator it = selections.begin(); it < selections.end(); it++ )
    {
        for( std::vector<DatabaseTable *>::iterator it1 = tableVec.begin(); it1 < tableVec.end() && !found; it1++ )
        {
            if( (*it).ToStdWstring() == (*it1)->GetSchemaName() + L"." + (*it1)->GetTableName() )
            {
                m_dbTables.push_back( (*it1 ) );
                DatabaseTable *dbTable = (*it1);
                MyErdTable *table = new MyErdTable( dbTable, dynamic_cast<DrawingView *>( GetFirstView() )->GetViewType() );
                m_tables.push_back( table );
                m_tableNames.push_back( table->GetTableName() );
                found = true;
            }
        }
        found = false;
    }
}

std::vector<std::wstring> &DrawingDocument::GetTableNameVector()
{
    return m_tableNames;
}

std::vector<DatabaseTable *> &DrawingDocument::GetDBTables()
{
    return m_dbTables;
}

MyErdTable *DrawingDocument::GetReferencedTable(const wxString &tableName)
{
    bool found = false;
    MyErdTable *table = NULL;
    for( std::vector<MyErdTable *>::iterator it2 = m_tables.begin(); it2 < m_tables.end() && !found; it2++ )
    {
        if( const_cast<DatabaseTable *>( (*it2)->GetTable() )->GetTableName() == tableName )
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

void DrawingDocument::SetQueryFields(const std::vector<TableField *> &fields)
{
    for( std::vector<TableField *>::const_iterator it = fields.begin(); it < fields.end(); ++it )
        m_queryFields.push_back( (*it)->GetFieldName() );
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
    Modify( true );
}

void DrawingDocument::AddGroupByAvailableField(const wxString &name, long position)
{
    m_groupByFieldsAll.push_back( GroupFields( name, position, position ) );
}

void DrawingDocument::AddGroupByFieldToQuery(const wxString &name, long position, long original, wxString &replace)
{
    m_groupByFieldsAll.erase( std::remove_if( m_groupByFieldsAll.begin(), m_groupByFieldsAll.end(), 
    [name](GroupFields field)
    {
        return field.fieldName == name;
    }), m_groupByFieldsAll.end() );
    if( position == m_groupByFields.size() )
        m_groupByFields.push_back( GroupFields( name, position, original ) );
    else
        m_groupByFields.insert( m_groupByFields.begin() + position, GroupFields( name, position, original ) );
    replace = "GROUP BY ";
    for( std::vector<GroupFields>::iterator it = m_groupByFields.begin(); it < m_groupByFields.end(); ++it )
    {
        if( it == m_groupByFields.begin() )
            replace += (*it).fieldName;
        else
        {
            replace += ",\n";
            replace += "         ";
            replace += (*it).fieldName;
        }
    }
    Modify( true );
}

void DrawingDocument::DeleteGroupByTable(const wxString &tableName)
{
    for( std::vector<GroupFields>::iterator it = m_groupByFieldsAll.begin(); it < m_groupByFieldsAll.end(); ++it )
        m_groupByFieldsAll.erase( std::remove_if( m_groupByFieldsAll.begin(), m_groupByFieldsAll.end(), 
        [tableName](GroupFields field)
        {
            return field.fieldName.StartsWith( "\"" + tableName );
        }), m_groupByFieldsAll.end() );
    for( std::vector<GroupFields>::iterator it = m_groupByFields.begin(); it < m_groupByFields.end(); ++it )
        m_groupByFieldsAll.erase( std::remove_if( m_groupByFields.begin(), m_groupByFields.end(), 
        [tableName](GroupFields field)
        {
            return field.fieldName.StartsWith( "\"" + tableName );
        } ), m_groupByFields.end() );
    Modify( true );
}

void DrawingDocument::DeleteGroupByField(const wxString &name, long original, wxString &replace)
{
    m_groupByFields.erase( std::remove_if( m_groupByFields.begin(), m_groupByFields.end(), 
                              [name](GroupFields field)
                              {
                                  return field.fieldName == name;
                              }), m_groupByFields.end() );
    m_groupByFieldsAll.insert( m_groupByFieldsAll.begin() + original, GroupFields( name, original, original ) );
    if( m_groupByFields.size() == 0 )
        replace = "";
    else
    {
        replace = "GROUP BY ";
        for( std::vector<GroupFields>::iterator it = m_groupByFields.begin(); it < m_groupByFields.end(); ++it )
        {
            if( it == m_groupByFields.begin() )
                replace += (*it).fieldName;
            else
            {
                replace += ",\n";
                replace += "         ";
                replace += (*it).fieldName;
            }
        }
    }
}

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
#include "databasedoc.h"
#include "databasecanvas.h"
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
    ((DrawingView *) GetFirstView() )->GetDatabaseCanvas()->GetDiagramManager().SerializeToXml( GetFilename(), xsWITH_ROOT );
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
    auto result = ((DrawingView *) GetFirstView() )->GetDatabaseCanvas()->GetDiagramManager().DeserializeFromXml( GetFilename() );
    if( result )
    {
        result = ((DrawingView *) GetFirstView() )->GetDatabaseCanvas()->UpdateCanvasWithQuery();
    }
    wxDocument::LoadObject( istream );

    return istream;
}

void DrawingDocument::DoUpdate()
{
    Modify( true );
    UpdateAllViews();
}

void DrawingDocument::SetDatabase(Database *db, bool isInit, const std::vector<QueryInfo> &queries, std::vector<LibrariesInfo> &libPath)
{
    m_db = db;
    if( !isInit )
        dynamic_cast<DrawingView *>( GetFirstView() )->GetTablesForView( db, true, queries, libPath );
}

Database *DrawingDocument::GetDatabase()
{
    return m_db;
}

void DrawingDocument::AddTables(const std::map<wxString,std::vector<TableDefinition> > &selections)
{
    bool found = false;
    std::wstring dbType = m_db->GetTableVector().m_type;
    std::map<std::wstring, std::vector<DatabaseTable *> > tables = m_db->GetTableVector().m_tables;
    for( auto sel : selections )
    {
        for( auto def : sel.second )
        {
            for( std::map<std::wstring, std::vector<DatabaseTable *> >::iterator it1 = tables.begin(); it1 != tables.end() && !found; it1++ )
            {
                for( auto it2 = (*it1).second.begin(); it2 < (*it1).second.end() && !found; ++it2 )
                {
                    wxString name, name1;
                    if( dbType == L"SQLite" )
                    {
                        name = def.schemaName + L"." + def.tableName;
                        name1 = (*it2)->GetSchemaName() + L"." + (*it2)->GetTableName();
                    }
                    else
                    {
                        name = def.catalogName + L"." + def.schemaName + L"." + def.tableName;
                        name1 = (*it2)->GetCatalog() + L"." + (*it2)->GetSchemaName() + L"." + (*it2)->GetTableName();
                    }
                    if( name == name1 )
                    {
                        m_dbTables.push_back( (*it2 ) );
                        DatabaseTable *dbTable = (*it2);
                        MyErdTable *table = new MyErdTable( dbTable, dynamic_cast<DrawingView *>( GetFirstView() )->GetViewType() );
                        m_tables.push_back( table );
                        m_tableNames.push_back( table->GetTableName() );
                        found = true;
                    }
                }
            }
            found = false;
        }
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

void DrawingDocument::AddRemoveField(const TableField *tableField, QueryFieldChange isAdded)
{
    auto position = m_sortedFieldsAll.size();
    auto original_position = m_queryFields.size();
    if( isAdded == ADD )
    {
        m_queryFields.push_back( const_cast<TableField *>( tableField ) );
        m_sortedFieldsAll.push_back( FieldSorter( tableField->GetFieldName(), position, original_position ) );
    }
    else if( isAdded == REMOVE )
    {
        m_queryFields.erase( std::remove( m_queryFields.begin(), m_queryFields.end(), tableField ), m_queryFields.end() );
        m_sortedFieldsAll.erase( std::remove_if( m_sortedFieldsAll.begin(), m_sortedFieldsAll.end(), 
                                [tableField](FieldSorter field)
        {
            return field.m_name == tableField->GetFieldName();
        }), m_sortedFieldsAll.end() );
    }
    else
    {

    }
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

void DrawingDocument::DeleteGroupByTable(const wxString &tableName, wxString &replace)
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

void DrawingDocument::AddAllSortedFeld(const wxString &name, long original_position)
{
    m_sortedFieldsAll.push_back( FieldSorter( name, true, original_position ) );
}

void DrawingDocument::DeleteSortedTable(const wxString &tableName, wxString &replace)
{
    for( std::vector<FieldSorter>::iterator it = m_sortedFieldsAll.begin(); it < m_sortedFieldsAll.end(); ++it )
        m_sortedFieldsAll.erase( std::remove_if( m_sortedFieldsAll.begin(), m_sortedFieldsAll.end(), 
                                 [tableName](FieldSorter field)
    {
        return field.m_name.StartsWith( "\"" + tableName );
    }), m_sortedFieldsAll.end() );
    for( std::vector<FieldSorter>::iterator it = m_sortedFields.begin(); it < m_sortedFields.end(); ++it )
        m_sortedFields.erase( std::remove_if( m_sortedFields.begin(), m_sortedFields.end(), 
                                 [tableName](FieldSorter field)
    {
        return field.m_name.StartsWith( "\"" + tableName );
    } ), m_sortedFields.end() );
    if( m_sortedFields.size() == 0 )
        replace = "";
    else
    {
        replace = "GROUP BY ";
        for( std::vector<FieldSorter>::iterator it = m_sortedFields.begin(); it < m_sortedFields.end(); ++it )
        {
            if( it == m_sortedFields.begin() )
                replace += (*it).m_name;
            else
            {
                replace += ",\n";
                replace += "         ";
                replace += (*it).m_name;
            }
        }
    }
    Modify( true );
}

void DrawingDocument::AddSortedField(const wxString &name, long original_position, long position, wxString &replace)
{
    m_sortedFieldsAll.erase( std::remove_if( m_sortedFieldsAll.begin(), m_sortedFieldsAll.end(), 
                             [name](FieldSorter field)
    {
        return field.m_name == name;
    }), m_sortedFieldsAll.end() );
    if( position == m_sortedFields.size() )
        m_sortedFields.push_back( FieldSorter( name, position, original_position ) );
    else
        m_sortedFields.insert( m_sortedFields.begin() + position, FieldSorter( name, position, original_position ) );
    replace = "ORDER BY ";
    for( std::vector<FieldSorter>::iterator it = m_sortedFields.begin(); it < m_sortedFields.end(); ++it )
    {
        if( it == m_sortedFields.begin() )
            replace += (*it).m_name;
        else
        {
            replace += ",\n";
            replace += "         ";
            replace += (*it).m_name;
        }
    }
    Modify( true );
}

void DrawingDocument::DeleteSortedField(const wxString &name, long original, wxString &replace)
{
    m_sortedFields.erase( std::remove_if( m_sortedFields.begin(), m_sortedFields.end(), 
                          [name](FieldSorter field)
    {
        return field.m_name == name;
    }), m_sortedFields.end() );
    m_sortedFieldsAll.insert( m_sortedFieldsAll.begin() + original, FieldSorter( name, original, original ) );
    if( m_sortedFields.size() == 0 )
        replace = "";
    else
    {
        replace = "GROUP BY ";
        for( std::vector<FieldSorter>::iterator it = m_sortedFields.begin(); it < m_sortedFields.end(); ++it )
        {
            if( it == m_sortedFields.begin() )
                replace += (*it).m_name;
            else
            {
                replace += ",\n";
                replace += "         ";
                replace += (*it).m_name;
            }
        }
    }
}

void DrawingDocument::ChangeSortOrder(const wxString &fieldName, long newPosition)
{
    for_each( m_sortedFields.begin(), m_sortedFields.end(), [fieldName](FieldSorter field) { if( field.m_name == fieldName ) field.m_isAscending = !field.m_isAscending; } );
}

void DrawingDocument::ShuffleGroupByFields(const wxString &fieldName, long newPosition, wxString &replace)
{
    GroupFields fieldToRemove;
    m_groupByFields.erase( std::remove_if( m_groupByFields.begin(), m_groupByFields.end(), 
                          [fieldName, &fieldToRemove](GroupFields field) mutable
    {
        if( field.fieldName == fieldName )
            fieldToRemove = field;
        return field.fieldName == fieldName;
    }), m_groupByFields.end() );
    fieldToRemove.position = newPosition;
    if( newPosition == m_groupByFields.size() )
        m_groupByFields.push_back( fieldToRemove );
    else
        m_groupByFields.insert( m_groupByFields.begin() + newPosition, fieldToRemove );
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

void DrawingDocument::DeleteQueryFieldForTable(const wxString &tableName, wxString &replace)
{
    m_queryFields.erase( std::remove_if( m_queryFields.begin(), m_queryFields.end(), 
                        [tableName](TableField *field)
                        {
                            return field->GetFullName().find( tableName ) == 0;
                        } ) );
    if( m_queryFields.size() == 0 )
        replace = "";
    else
    {
        replace = "SELECT ";
        for( std::vector<TableField *>::iterator it = m_queryFields.begin(); it < m_queryFields.end(); ++it )
        {
            replace += (*it)->GetFullName();
        }
    }
}

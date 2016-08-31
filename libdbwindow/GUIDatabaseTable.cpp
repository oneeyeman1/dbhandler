#include <string>
#include "database.h"
#include "XmlSerializer.h"
#include "GUIColumn.h"
#include "GUIDatabaseTable.h"

GUIDatabaseTable::GUIDatabaseTable(DatabaseTable *table) :Table()
{
    m_table = table;
    m_name = table->GetTableName();
    m_parentName = wxEmptyString;
}


GUIDatabaseTable::~GUIDatabaseTable(void)
{
}

void GUIDatabaseTable::AddColumn(GUIColumn *column)
{
    AddChild( column );
}

GUIColumn *GUIDatabaseTable::GetFirstColumn()
{
    return (GUIColumn *) GetFirstChild( CLASSINFO( GUIColumn ) );
}

#include <string>
#include "wxsf/TextShape.h"
#include "database.h"
#include "XmlSerializer.h"
#include "constraintsign.h"
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

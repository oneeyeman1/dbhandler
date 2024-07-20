#include <string>
#include "wxsf/BitmapShape.h"
#include "wxsf/FlexGridShape.h"
#include "wxsf/TextShape.h"
#include "database.h"
#include "XmlSerializer.h"
#include "constraint.h"
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

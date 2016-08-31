#pragma once
#include "table.h"
class GUIDatabaseTable : public Table
{
public:
    GUIDatabaseTable(DatabaseTable *table);
    ~GUIDatabaseTable(void);
    void AddColumn(GUIColumn *column);
private:
    DatabaseTable *m_table;
};


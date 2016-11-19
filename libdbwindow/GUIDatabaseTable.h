#pragma once
#include "table.h"
class GUIDatabaseTable : public Table
{
public:
    GUIDatabaseTable(DatabaseTable *table);
    ~GUIDatabaseTable(void);
private:
    DatabaseTable *m_table;
};


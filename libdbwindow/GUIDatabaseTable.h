#pragma once
#include "table.h"
class GUIDatabaseTable : public Table
{
public:
    GUIDatabaseTable(DatabaseTable *table);
    ~GUIDatabaseTable(void);
    void AddColumn(GUIColumn *column);
    inline GUIColumn *GetFirstColumn();
private:
    DatabaseTable *m_table;
};


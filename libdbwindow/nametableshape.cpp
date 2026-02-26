#include <memory>
#include "wxsf/TextShape.h"
#include "database.h"
#include "nametableshape.h"

XS_IMPLEMENT_CLONABLE_CLASS(NameTableShape, wxSFTextShape);

NameTableShape::NameTableShape() : wxSFTextShape()
{
    m_table = wxEmptyString;
    XS_SERIALIZE( m_table, "table_name" );
}

NameTableShape::NameTableShape(DatabaseTable *table) : wxSFTextShape()
{
    m_table = table->GetTableName();
    XS_SERIALIZE( m_table, "table_name" );
}

const wxString &NameTableShape::GetTableName() const
{
    return m_table;
}

void NameTableShape::SetTableName(const wxString &table)
{
    m_table = table;
    SetText( m_table );
}


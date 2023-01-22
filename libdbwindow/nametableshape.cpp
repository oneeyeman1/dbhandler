#include "wxsf/TextShape.h"
#include "database.h"
#include "nametableshape.h"

XS_IMPLEMENT_CLONABLE_CLASS(NameTableShape, wxSFTextShape);

NameTableShape::NameTableShape() : wxSFTextShape()
{
    m_table = nullptr;
    XS_SERIALIZE( GetText(), "table_name" );
}

NameTableShape::NameTableShape(DatabaseTable *table) : wxSFTextShape()
{
    m_table = table;
    XS_SERIALIZE( GetText(), "table_name" );
}


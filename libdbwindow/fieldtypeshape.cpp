#include <string>
#include <algorithm>
#include "database.h"
#include "wxsf/TextShape.h"
#include "fieldtypeshape.h"

XS_IMPLEMENT_CLONABLE_CLASS(FieldTypeShape, wxSFTextShape);

FieldTypeShape::FieldTypeShape() : wxSFTextShape()
{
    m_field = nullptr;
}

FieldTypeShape::FieldTypeShape(Field *field) : wxSFTextShape()
{
    m_field = field;
}

FieldTypeShape::FieldTypeShape(FieldTypeShape &shape)
{
    m_field = shape.m_field;
}

FieldTypeShape::~FieldTypeShape()
{
}

const Field *FieldTypeShape::GetFieldForComment()
{
    return m_field;
}

void FieldTypeShape::SetField(Field *field)
{
    m_field = field;
}

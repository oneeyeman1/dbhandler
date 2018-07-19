#include <string>
#include <algorithm>
#include "database.h"
#include "wxsf/TextShape.h"
#include "commentfieldshape.h"

XS_IMPLEMENT_CLONABLE_CLASS(CommentFieldShape, wxSFTextShape);

CommentFieldShape::CommentFieldShape() : wxSFTextShape()
{
    m_field = NULL;
}

CommentFieldShape::CommentFieldShape(Field *field) : wxSFTextShape()
{
    m_field = field;
}

CommentFieldShape::CommentFieldShape(CommentFieldShape &shape)
{
    m_field = shape.m_field;
}

CommentFieldShape::~CommentFieldShape(void)
{
}

const Field *CommentFieldShape::GetFieldForComment()
{
    return m_field;
}

void CommentFieldShape::SetField(Field *field)
{
    m_field = field;
}

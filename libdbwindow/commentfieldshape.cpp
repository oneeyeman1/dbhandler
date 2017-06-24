#include "database.h"
#include "wxsf/textshape.h"
#include "commentfieldshape.h"

CommentFieldShape::CommentFieldShape(Field *field)
{
    m_field = field;
}

CommentFieldShape::~CommentFieldShape(void)
{
}

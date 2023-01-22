#include "wxsf/TextShape.h"
#include "database.h"
#include "commenttableshape.h"

XS_IMPLEMENT_CLONABLE_CLASS(CommentTableShape, wxSFTextShape);

CommentTableShape::CommentTableShape(void) : wxSFTextShape()
{
    m_table = NULL;
    XS_SERIALIZE( GetText(), "table_comment" );
}

CommentTableShape::CommentTableShape(DatabaseTable *table) : wxSFTextShape()
{
    m_table = table;
    XS_SERIALIZE( GetText(), "table_comment" );
}

CommentTableShape::~CommentTableShape(void)
{
}

const DatabaseTable *CommentTableShape::GetDatabaseTable()
{
    return m_table;
}

void CommentTableShape::SetDatabaseTable(const DatabaseTable *table)
{
    m_table = const_cast<DatabaseTable *>( table );
}

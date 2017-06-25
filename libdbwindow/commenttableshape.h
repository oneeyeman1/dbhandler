#pragma once
class CommentTableShape : public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(CommentTableShape);
    CommentTableShape(void);
    CommentTableShape(DatabaseTable *table);
    virtual ~CommentTableShape(void);
    const DatabaseTable *GetDatabaseTable();
    void SetDatabaseTable(const DatabaseTable *table);
private:
    DatabaseTable *m_table;
};


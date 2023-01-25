#pragma once
class CommentTableShape : public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(CommentTableShape);
    CommentTableShape(void);
    CommentTableShape(DatabaseTable *table);
    virtual ~CommentTableShape(void);
    const wxString &GetDatabaseComment();
    void SetDatabaseComment(const wxString &table);
private:
    wxString m_comment;
};


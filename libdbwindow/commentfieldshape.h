#pragma once
class CommentFieldShape : public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(CommentFieldShape);
    CommentFieldShape();
    CommentFieldShape(Field *field);
    CommentFieldShape(CommentFieldShape &shape);
    virtual ~CommentFieldShape(void);
    const Field *GetFieldForComment();
    void SetField(Field *field);
private:
    Field *m_field;
};


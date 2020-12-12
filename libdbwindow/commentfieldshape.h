#pragma once
class CommentFieldShape : public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(CommentFieldShape);
    CommentFieldShape();
    CommentFieldShape(TableField *field);
    CommentFieldShape(CommentFieldShape &shape);
    virtual ~CommentFieldShape(void);
    const TableField *GetFieldForComment();
    void SetField(TableField *field);
private:
    TableField *m_field;
};


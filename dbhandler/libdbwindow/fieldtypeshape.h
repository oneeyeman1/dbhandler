#pragma once

class CommentFieldShape;
class FieldShape;

class FieldTypeShape : public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(FieldTypeShape);
    FieldTypeShape();
    FieldTypeShape(TableField *field);
    FieldTypeShape(FieldTypeShape &shape);
    virtual ~FieldTypeShape();
    const TableField *GetFieldForComment();
    void SetField(TableField *field);
    CommentFieldShape *GetCommentShape() const { return m_comment; }
    FieldShape *GetFieldShape() const { return m_fieldShape; }
    void SetCommentShape(CommentFieldShape *shape) { m_comment = shape; }
    void SetFieldShape(FieldShape *shape) { m_fieldShape = shape; }
protected:    
    virtual void DrawNormal(wxDC &dc);
    virtual void DrawSelected(wxDC& dc);
private:
    TableField *m_field;
    wxRect m_parentRect;
    wxColour m_backColour;
    CommentFieldShape *m_comment;
    FieldShape *m_fieldShape;
};


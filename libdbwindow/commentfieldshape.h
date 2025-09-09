#pragma once

class FieldShape;
class FieldTypeShape;

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
    FieldShape *GetFieldShape() const { return m_fieldShape; }
    FieldTypeShape *GetTypeShape() const { return m_typeShape; }
    void SetFieldShape(FieldShape *shape) { m_fieldShape = shape; }
    void SetTypeShape(FieldTypeShape *shape) { m_typeShape = shape; }
protected:
    virtual void DrawNormal(wxDC &dc) override;
    virtual void DrawSelected(wxDC &dc) override;
private:
    wxColour m_backColor;
    TableField *m_tableField;
    FieldShape *m_fieldShape = nullptr;
    FieldTypeShape *m_typeShape = nullptr;
};


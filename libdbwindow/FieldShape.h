#pragma once

class FieldTypeShape;
class CommentFieldShape;

class FieldShape :	public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(FieldShape);
    FieldShape(void);
    FieldShape(const TableField *field);
    FieldShape(const FieldShape &shape);
    virtual ~FieldShape(void);
//    virtual wxRect GetBoundingBox() wxOVERRIDE;
//    virtual bool Contains(const wxPoint& pos) wxOVERRIDE;
    void SetParentRect(const wxRect &rect);
    void SetField(TableField *field);
    void Select(bool state);
    TableField *GetField();
    FieldTypeShape *GetTypeShape() const { return m_typeShape; }
    CommentFieldShape *GetCommentShape() const { return m_comment; }
    void SetTypeShape(FieldTypeShape *shape) { m_typeShape = shape; }
    void SetCommentShape(CommentFieldShape *shape) { m_comment = shape; }
protected:    
    virtual void DrawNormal(wxDC &dc) wxOVERRIDE;
    virtual void DrawSelected(wxDC& dc) wxOVERRIDE;
private:
    bool m_isNotNull, m_isPK, m_isAutoInc;
    TableField *m_field;
    wxRect m_parentRect;
    int m_size, m_decimal;
    wxString m_name, m_type;
    wxColour m_backColour;
    FieldTypeShape *m_typeShape;
    CommentFieldShape *m_comment;
};


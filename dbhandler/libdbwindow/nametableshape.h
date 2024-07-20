#pragma once
class NameTableShape :  public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(NameTableShape);
    NameTableShape();
    NameTableShape(DatabaseTable *table);
    virtual ~NameTableShape() {};
    void SetTableName(const wxString &table);
    const wxString &GetTableName() const;
private:
    wxString m_table;
};


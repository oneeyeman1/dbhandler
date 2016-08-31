#pragma once
class MyErdTable : public ErdTable
{
public:
    MyErdTable(GUIDatabaseTable *table);
    void UpdateColumn();
private:
    void AddColumnShape(const wxString& colName, int id, Constraint::constraintType type);
    wxSFTextShape *m_pLabel;
    wxSFFlexGridShape* m_pGrid;
};

#ifndef _DATABASECANVAS_H
#define _DATABASECANVAS_H

#define sfDONT_SAVE_STATE false

class WXEXPORT DatabaseCanvas : public wxSFShapeCanvas
{
public:
    enum MODE { modeDESIGN, modeTABLE, modeVIEW, modeLine };
    DatabaseCanvas(wxView *view, const wxPoint &pt, wxWindow *parent = NULL);
    void DisplayTables(std::vector<wxString> &selections, wxString &query);
    virtual ~DatabaseCanvas();
    void CreateFKConstraint(const DatabaseTable *fkTable, const std::vector<FKField *> &foreignKeyField);
    virtual void OnLeftDown(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnRightDown(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnLeftDoubleClick(wxMouseEvent& event) wxOVERRIDE;
    void OnDropTable(wxCommandEvent &event);
    void OnShowSQLBox(wxCommandEvent &event);
    void OnShowComments(wxCommandEvent &event);
    void OnShowDataTypes(wxCommandEvent &event);
    inline wxSFDiagramManager &GetDiagramManager() { return m_pManager; }
    virtual void OnDraw(wxDC& dc) wxOVERRIDE;
    void AddQuickQueryFields(const wxString &tbl, std::vector<Field *> &quickSelectFields, bool quickSelect);
protected:
    bool IsTableDisplayed(const std::wstring &name);
private:
    std::vector<MyErdTable *> m_displayedTables;
    wxView *m_view;
    wxSFDiagramManager m_pManager;
    bool m_showComments, m_showIndexKeys, m_showIntegrity;
    bool m_showDataTypes, m_showLabels, m_showToolBox;
    wxSFShapeBase *m_selectedShape, *m_realSelectedShape;
    MODE m_mode;
    wxPoint startPoint;
    ConstraintSign *m_oldSelectedSign;
};

#define wxID_TABLECLOSE            20
#define wxID_TABLEALTERTABLE       21
#define wxID_TABLEPROPERTIES       22
#define wxID_TABLENEW              23
//#define wxID_TABLEDROPTABLE        24
#define wxID_TABLEEDITDATA         25
#define wxID_TABLEDATATRANSFER     26
#define wxID_TABLEPRINTDEFINITION  27
#define wxID_VIEWARRANGETABLES     31
#define wxID_VIEWSHOWCOMMENTS      32
#define wxID_VIEWSHOWINDEXKEYS     33
#define wxID_VIEWSHOWINTEGRITY     34
#define wxID_PROPERTIES            50
#define wxID_CREATEDATABASE       100
#define wxID_DELETEDATABASE       101
#define wxID_SELECTTABLE          102
#define wxID_OBJECTNEWTABLE       103
#define wxID_OBJECTNEWINDEX       104
#define wxID_OBJECTNEWVIEW        105
#define wxID_OBJECTNEWFF          106
#define wxID_FIELDDEFINITION      107
#define wxID_FIELDPROPERTIES      108
#define wxID_OBJECTNEWPK          109
#define wxID_STARTLOG             110
#define wxID_STOPLOG              111
#define wxID_SAVELOG              112
#define wxID_CLEARLOG             113
#define wxID_SELECTALLFIELDS      114
#define wxID_DESELECTALLFIELDS    115
#define wxID_ARRANGETABLES        116
#define wxID_SHOWDATATYPES        117
#define wxID_SHOWLABELS           118
#define wxID_SHOWCOMMENTS         119
#define wxID_SHOWSQLTOOLBOX       120
#define wxID_FKDEFINITION         121
#define wxID_FKOPENREFTABLE       122
#define wxID_DROPOBJECT           123
#define WHEREPAGECOLUMNS          194
#define WHEREPAGEFUNCTIONS        195
#define WHEREPAGEARGUMENTS        196
#define WHEREPAGEVALUE            197
#define WHEREPAGESELECT           198
#define WHEREPAGECLEAR            199
#define wxID_DATASOURCE           301
#define wxID_RETRIEVEARGS         304
#define wxID_DISTINCT             306
#define BOLDFONT                  325
#define ITALICFONT                326
#define UNDERLINEFONGT            327
#define wxID_DESIGNTABORDER       328
#define wxID_COLUMNSPEC           329
#define wxID_PREVIEDWQUERY        330
#define wxID_CONVERTTOSYNTAX      331
#define wxID_SHOWJOINS            332
#define myID_FINDNEXT             333
#define wxID_GOTOLINE             334
#define wxID_CONVERTTOGRAPHICS    335
#define wxID_UNDOALL              336
#endif

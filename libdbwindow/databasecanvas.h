#ifndef _DATABASECANVAS_H
#define _DATABASECANVAS_H

#define sfDONT_SAVE_STATE false

class QueryRoot : public xsSerializable
{
public:
    XS_DECLARE_CLONABLE_CLASS(QueryRoot);
    QueryRoot();
    QueryRoot(const QueryRoot &root);
    ~QueryRoot() {}
    void SetDbName(const wxString &name) { m_dbName = name; }
    void SetDbType(const wxString &type) { m_dbType = type; }
    const wxString &GetDbName() const { return m_dbName; }
    const wxString &GetDbType() const { return m_dbType; }
private:
    wxString m_dbName, m_dbType;
};

class WXEXPORT DatabaseCanvas : public wxSFShapeCanvas
{
public:
    enum MODE { modeDESIGN, modeTABLE, modeVIEW, modeLine };
    DatabaseCanvas(wxView *view, const wxPoint &pt, const wxString &dbName, const wxString &dbType, wxWindow *parent = NULL);
    void DisplayTables(std::map<wxString,std::vector<TableDefinition> > &selections, const std::vector<TableField *> &queryFields, wxString &query, std::vector<wxString> &relations);
    virtual ~DatabaseCanvas();
    void CreateFKConstraint(const DatabaseTable *fkTable, const std::vector<FKField *> &foreignKeyField);
    virtual void OnLeftDown(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnRightDown(wxMouseEvent &event) wxOVERRIDE;
    virtual void OnLeftDoubleClick(wxMouseEvent& event) wxOVERRIDE;
    virtual void OnMouseMove(wxMouseEvent &event) wxOVERRIDE;
    void OnCloseTable(wxCommandEvent &event);
    void OnDropTable(wxCommandEvent &event);
    inline wxSFDiagramManager &GetDiagramManager() { return m_pManager; }
    virtual void OnDraw(wxDC& dc) wxOVERRIDE;
    void AddQuickQueryFields(const wxString &tbl, std::vector<TableField *> &quickSelectFields, bool quickSelect);
    void ShowHideTablePart(int part, bool show);
    void CheckSQLToolbox();
    void GetAllSelectedShapes(ShapeList &shapes);
    void SetQueryInfo(const std::vector<QueryInfo> &queries) { m_queries = queries; }
    void SetObjectPath(const std::vector<LibrariesInfo> path) { m_path = path; }
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
    std::vector<QueryInfo> m_queries;
    std::vector<LibrariesInfo> m_path;
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
#define wxID_EXPORTSYNTAX         108
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
#define wxID_ATTACHDATABASE       124
#define wxID_DETACHDATABASE       125
#define WHEREPAGECOLUMNS          194
#define WHEREPAGEFUNCTIONS        195
#define WHEREPAGEARGUMENTS        196
#define WHEREPAGEVALUE            197
#define WHEREPAGESELECT           198
#define WHEREPAGECLEAR            199
#define wxID_DATASOURCE           301
#define wxID_UNIONS               303
#define wxID_RETRIEVEARGS         304
#define wxID_CHECKOPTION          305
#define wxID_DISTINCT             306
#define wxID_SELECTABOVE          311
#define wxID_SELECTBELOW          312
#define wxID_SELECTLEFT           313
#define wxID_SELECTRIGHT          314
#define wxID_SELECTCOLUMNS        315
#define wxID_SELECTTEXT           316
#define wxID_BRINGTOFRONT         317
#define wxID_SENDTOBACK           318
#define wxID_FORMATCURRENCY       319
#define wxID_FORMATFORMAT         320
#define wxID_SELECTOBJECT         321
#define wxID_TEXTOBJECT           322
#define wxID_PICTUREOBJECT        323
#define wxID_LINEOBJECT           324
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
#define wxID_SAVEQUERY            404
#define wxID_SAVEQUERYAS          405
#endif

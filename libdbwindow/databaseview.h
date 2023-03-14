#ifndef __DATABASEVIEW__H
#define __DATABASEVIEW__H

#if !defined CONSTRAINT_H && !defined __GRIDTABLESHAPE_H__
enum ViewType
{
    DatabaseView,
    QueryView
};
#endif

enum DrawinViewMenu
{
    QuickQueryMenu,
    SQLSelectMenu,
    QueryMenu,
    ExternalMenu,
    QuerySyntaxMenu
};

struct Profile
{
    wxString m_name;
    bool m_isActive;
    
    Profile(wxString name, bool isActive) : m_name(name), m_isActive(isActive) {}
};

// The view using MyCanvas to show its contents
class DrawingView : public wxView
{
public:
    DrawingView() : wxView(), m_canvas(NULL) {}
    ~DrawingView();
    void UpdateQueryFromSignChange(const QueryConstraint *type, const long oldSign);
    void DropTableFromQeury(const wxString &name = "" );
    wxFrame *GetLogWindow() const;
    wxTextCtrl *GetFieldTextCtrl();
    FontComboBox *GetFontName() const;
    wxTextCtrl *GetTextLogger() const;
    void GetTablesForView(Database *db, bool init, const std::vector<QueryInfo> &queries, std::vector<LibrariesInfo> &path);
    int SelectTable(bool isTableView, std::map<wxString, std::vector<TableDefinition> > &tables, wxString &query, bool quickSelect);
    std::map<wxString, std::vector<TableDefinition> > &GetTablesMap();
    void SetQueryArguments(const QueryArguments &args) { m_arguments.push_back( args ); }
    void SetViewType(ViewType type);
    ViewType GetViewType();
    SortGroupByPage *GetSortPage();
    SortGroupByPage *GetGroupByPage();
    WhereHavingPage *GetWherePage();
    WhereHavingPage *GetHavingPage();
    SyntaxPropPage *GetSyntaxPage();
    void AddFieldToQuery(const FieldShape &field, QueryFieldChange isAdding, const std::wstring &tableName, bool quickSelect);
    void HideShowSQLBox(bool show);
    void SetPaintersMap(std::map<wxString, wxDynamicLibrary *> &painters);
    void ChangeFontEement();
    void ChangeTableTypeMMenu();
    void ChangeTableCommentsMenu();
    void SetProperties(const wxSFRectShape *shape);
    void SetProfiles(const std::vector<Profile> &profiles) { m_profiles = profiles; }
    DatabaseCanvas *GetDatabaseCanvas() const { return m_canvas; }
    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    virtual void OnDraw(wxDC *dc) wxOVERRIDE;
    virtual void OnUpdate(wxView *sender, wxObject *hint = NULL) wxOVERRIDE;
    virtual bool OnClose(bool deleteWindow = true) wxOVERRIDE;
#if defined __WXMSW__ || defined __WXGTK__
    virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView);
#endif
    void OnViewSelectedTables(wxCommandEvent &event);
    void OnNewIndex(wxCommandEvent &event);
    void OnFieldDefinition(wxCommandEvent &event);
    void OnSetProperties(wxCommandEvent &event);
    void OnCloseLogWindow(wxCloseEvent &event);
    void OnForeignKey(wxCommandEvent &event);
    void OnLogUpdateUI(wxUpdateUIEvent &event);
    void OnStartLog(wxCommandEvent &event);
    void OnStopLog(wxCommandEvent &event);
    void OnSaveLog(wxCommandEvent &event);
    void OnClearLog(wxCommandEvent &event);
    void OnAlterTable(wxCommandEvent &event);
    void OnCreateDatabase(wxCommandEvent &event);
    void OnSelectAllFields(wxCommandEvent &event);
    void OnSQLNotebookPageChanged(wxBookCtrlEvent &event);
    void OnDistinct(wxCommandEvent &event);
    void OnQueryChange(wxCommandEvent &event);
    void OnRetrievalArguments(wxCommandEvent &event);
    void OnTabOrder(wxCommandEvent &event);
    void FieldTextUpdateUI(wxUpdateUIEvent &event);
    void OnDataSource(wxCommandEvent &event);
    void OnFontSeectionChange(wxCommandEvent &event);
    void OnQueryPreviewUpdateUI(wxUpdateUIEvent &event);
    void OnShowDataTypes(wxCommandEvent &event);
    void OnShowComments(wxCommandEvent &event);
    void OnShowSQLBox(wxCommandEvent &event);
    void OnConvertToSyntaxUpdateUI(wxUpdateUIEvent &event);
    void OnConvertToSyntax(wxCommandEvent &event);
    void OnUndo(wxCommandEvent &event);
    void OnCut(wxCommandEvent &event);
    void OnCopy(wxCommandEvent &event);
    void OnPaste(wxCommandEvent &event);
    void OnClear(wxCommandEvent &event);
    void OnSelectAll(wxCommandEvent &event);
    void OnFind(wxCommandEvent &event);
    void OnFindNext(wxCommandEvent &event);
    void OnFindReplaceText(wxFindDialogEvent &event);
    void OnGotoLine(wxCommandEvent &event);
    void OnConvertToGraphics(wxCommandEvent &event);
    void OnIconise(wxIconizeEvent &event);
    void OnTableDataEdit(wxCommandEvent &event);
    void OnFieldShuffle(wxCommandEvent &event);
    void OnExportSyntax(wxCommandEvent &event);
    void OnQuerySave(wxCommandEvent &event);
    void OnQuerySaveUpdateUI(wxUpdateUIEvent &event);
    void OnQuerySaveAsUpdateUI(wxUpdateUIEvent &event);
    void OnLabelTextChanged(wxCommandEvent &event);
/*#if defined __WXMSW__ || defined __WXGTK__
    virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView);
#endif*/
    DrawingDocument* GetDocument();
protected:
    void CreateDBMenu();
    void CreateQueryMenu(const int queryType);
    void SortGroupByHandling(const int type, const wxString &fieldName, const int queryType, wxString &query, const Positions *sortType);
    void AddDeleteFields(MyErdTable *table, bool isAdd, const std::wstring &tableName);
    void CreateViewToolBar();
    int AddSize(int size, int lfHeight);
    void FindTextInEditor();
    void PopuateQueryCanvas( );
#ifdef __WXMSW__
    static int CALLBACK EnumFontFamiliesCallback2(ENUMLOGFONT *lpelf, NEWTEXTMETRIC *lpntm, int FontType, LPARAM lParam);
#endif
private:
    bool m_isActive;
    wxDocMDIParentFrame *m_parent;
    wxToolBar *m_tb, *m_styleBar;
    wxTextCtrl *m_fieldText;
    FontComboBox *m_fontName;
    wxComboBox *m_fontSize;
    wxFrame *m_log;
    wxTextCtrl *m_text;
    DatabaseCanvas *m_canvas;
    bool m_isCreated;
    ViewType m_type;
    FieldWindow *m_fields;
    wxDocMDIChildFrame *m_frame;
    wxNotebook *m_queryBook;
    wxBoxSizer *sizer, *mainSizer;
    WhereHavingPage *m_page2, *m_page4;
    SortGroupByPage *m_page1, *m_page3;
    SyntaxPropPage *m_page6;
    wxCriticalSection *pcs;
    int m_source, m_presentation, m_searchPos, m_start, m_end, m_searchFlags, m_searchDirection, m_queryType;
    wxString m_stringToFind;
    std::vector<DatabaseTable *> m_selectTableName;
    std::map<int, wxString> m_whereCondition, m_havingCondition;
    std::vector<wxString> m_whereRelatons;
    std::vector<QueryArguments> m_arguments;
    std::map<wxString, wxDynamicLibrary *> m_painters;
    DesignCanvas *m_designCanvas;
    wxStyledTextCtrl *m_edit;
    wxFindReplaceDialog *m_findDlg;
    wxFindReplaceData m_data;
    std::vector<Profile> m_profiles;
    std::vector<QueryInfo> m_queries;
    std::vector<LibrariesInfo> m_path;
    std::map<wxString, std::vector<TableDefinition> > m_tables;
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(DrawingView);
};

wxDECLARE_EVENT(wxEVT_SET_TABLE_PROPERTY, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_SET_FIELD_PROPERTY, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_CHANGE_QUERY, wxCommandEvent);

#define wxID_DATABASEWINDOW 2

#if __cplusplus < 201300
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif

#endif


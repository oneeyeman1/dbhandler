#pragma once

#define COL_NAME       0
#define COL_CHECKEDOUT 1
#define COL_MODIFIED   2
#define COL_COMPILED   3
#define COL_SIZE       4
#define COL_COMMENT    5
 
class LibraryViewPainter :  public ABLBaseView, public PropertiesHandler
{
public:
    virtual bool OnCreate(wxDocument *doc, long flags) wxOVERRIDE;
    virtual void OnDraw(wxDC *) wxOVERRIDE;
    virtual int ApplyProperties(const wxAny &any, bool logOnly, std::wstring &command) wxOVERRIDE;
    virtual wxAny &GetProperties() wxOVERRIDE;
    void SetViewType(ViewType type) { m_type = type; }
    void SetParentWindow(wxWindow *window) { m_parent = wxStaticCast( window, wxDocMDIParentFrame ); }
    void SetConfiguration(Configuration *conf) { m_conf = conf; }
protected:
    void CreateViewToolBar();
    void ApplyOptions();
    void CreateLibraryMenu();
    void GetIconFromArtProvider( int id);
    void ExpandRoot(const wxString &path);
    void ExpandDir(wxTreeListItem parent);
    bool ExpandPath(const wxString &path);
    void PopulateNode(wxTreeListItem parent);
    const wxTreeListItem AddSection(const wxString& path, const wxString& name);
    wxTreeListItem FindChild(wxTreeListItem parentId, const wxString& path, bool& done);
    void LayoutChildren(const wxSize &size);

    void SelectAllLibraryObjects(wxTreeListItem item);

    void OnItemContextMenu(wxTreeListEvent &event);
    void OnSelectionChanged(wxTreeListEvent &event);
    void OnLibraryCreate(wxCommandEvent &event);
    void OnDeleteLibraryUpdateUI(wxUpdateUIEvent &event);
    void OnLibraryDelete(wxCommandEvent &event);
    void OnSelectAllUpdateUI(wxUpdateUIEvent &event);
    void OnExpandCollapsUpdateUI(wxUpdateUIEvent &event);
    void OnSelectAll(wxCommandEvent &event);
    void OnExpandCollapse(wxCommandEvent &event);
    void OnSelectDevice(wxCommandEvent &event);
    void OnPainterProperties(wxCommandEvent &event);
private:
    bool LoadApplicationOject(const wxString &fileName, std::unique_ptr<LibraryObject> &library);
    size_t GetAvailableDrives(wxArrayString &paths, wxArrayString &names, wxArrayInt &icons);
    bool IsDriveAvailable(const wxString& dirName);
    ViewType m_type;
    Configuration *m_conf;
    wxDocMDIChildFrame *m_frame;
    wxBitmapComboBox *m_drive;
    wxTreeListCtrl *m_tree;
    wxTreeListItem m_rootId;
    wxBitmapBundle libraryOpen, libraryClosed;
    wxDECLARE_DYNAMIC_CLASS(LibraryViewPainter);
    wxDECLARE_EVENT_TABLE();
};

#define wxID_CONFIGUREODBC          1
#define wxID_DATABASEWINDOWPROFILE  2
#define wxID_TABLE                  3
#define wxID_DATABASE               4
#define wxID_QUERY                  5
#define wxID_LIBRARY                6
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
#define wxID_CUSTOMCOLORS         126
#define wxID_DATABASEOPTIONS      127
#define wxID_CREATETABLESPACE     128
#define wxID_EDITTABLEOBJECT      129
#define WHEREPAGECOLUMNS          194
#define WHEREPAGEFUNCTIONS        195
#define WHEREPAGEARGUMENTS        196
#define WHEREPAGEVALUE            197
#define WHEREPAGESELECT           198
#define WHEREPAGECLEAR            199
#define wxID_EDITCUTCOLUMN        200
#define wxID_EDITCOPYCOLUMN       201
#define wxID_EDITPASTECOLUMN      202
#define wxID_EDITINSERTCOLUMN     203
#define wxID_EDITDELETECOLUMN     204
#define wxID_EDITTABLEPROPERTY    205
#define wxID_DESIGNSYNTAX         206
#define wxID_DATASOURCE           301
#define wxID_PREVIEW              302
#define wxID_UNIONS               303
#define wxID_RETRIEVEARGS         304
#define wxID_CHECKOPTION          305
#define wxID_DISTINCT             306
#define wxID_OPTIONS              309
#define wxID_SELECTALL            310
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
#define wxID_PREVIEWQUERY         330
#define wxID_CONVERTTOSYNTAX      331
#define wxID_SHOWJOINS            332
#define myID_FINDNEXT             333
#define wxID_GOTOLINE             334
#define wxID_CONVERTTOGRAPHICS    335
#define wxID_UNDOALL              336
#define wxID_QUERYCANCEL          400
#define wxID_TOP                  402
#define wxID_BACK                 403
#define wxID_SAVEQUERY            404
#define wx_SAVEQUERYAS            405
#define wxID_CUTCOLUMN            501
#define wxID_COPYCOLUMN           502
#define wxID_PASTECOLUMN          503
#define wxID_INSERTCOLUMN         504
#define wxID_DELETECOLUMN         505
#define wxID_LIBRARYNEW           601
#define wxID_LIBRARYDELETE        602
#define wxID_IMPORTLIBRARY        603
#define wxID_LIBRARYSELECTALL     604
#define wxID_EXPANDCOLLAPS        605
#define wxID_SELECTDEVICE         606
#define wxID_LEFTALIGN            1000
#define wxID_CENTERALIGN          1001 
#define wxID_RIGHTALIGN           1002
#define wxID_COMMENTFIELD         1003
#define wxID_FONTNAME             1004
#define wxID_FONTSIZE             1005
#define wxID_JOINS                1006

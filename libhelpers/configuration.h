#pragma once

enum ViewType
{
    DatabaseView,
    NewViewView,
    QueryView,
    TableView,
    LibraryView
};

struct QueryInfo
{
    wxString name, comment;
    void operator=(const QueryInfo &info)
    {
        name = info.name;
        comment = info.comment;
    }
};

struct LibrariesInfo
{
    wxString m_path;
    bool m_isActive;
    LibrariesInfo(const wxString &path, bool active) : m_path(path), m_isActive(active) {}
};

struct ToolbarSetup
{
    bool m_hideShow, m_showTooltips, m_showText;
    int m_orientation;
};

struct DBOptionGeneral
{
    wxString m_sharedProfile, m_sqlTerminator, m_tableRefresh, m_tableColumns;
    bool m_tableLst, m_useRepo, m_readOnly, m_keepAlive;
    DBOptionGeneral &operator=( const DBOptionGeneral &right )
    {
        if( this == &right )
            return *this;
        else
        {
            m_sharedProfile = right.m_sharedProfile;
            m_sqlTerminator = right.m_sqlTerminator;
            m_tableRefresh = right.m_tableRefresh;
            m_tableColumns = right.m_tableColumns;
            m_tableLst = right.m_tableLst;
            m_useRepo = right.m_useRepo;
            m_readOnly = right.m_readOnly;
            m_keepAlive = right.m_keepAlive;
        }
        return *this;
    }
};

struct DatabaseOptionColors
{
    wxColor m_background, m_tableCol, m_indexLine, m_tableHeader, m_tableColText, m_primaryKeyLine, m_tableHeaderText, m_tableCommentText, m_foreignKeyLine;
    DatabaseOptionColors &operator=(const DatabaseOptionColors &right)
    {
        if( this == &right )
            return *this;
        else
        {
            m_background = right.m_background;
            m_tableCol = right.m_tableCol;
            m_indexLine = right.m_indexLine;
            m_tableHeader = right.m_tableHeader;
            m_tableColText = right.m_tableColText;
            m_primaryKeyLine = right.m_primaryKeyLine;
            m_tableHeaderText = right.m_tableHeaderText;
            m_tableCommentText = right.m_tableCommentText;
            m_foreignKeyLine = right.m_foreignKeyLine;
            return *this;
        }
    }
};

struct DatabaseOptions
{
    DBOptionGeneral m_general;
    DatabaseOptionColors m_colors;
};

struct LibraryPainterOptionsGeneral
{
    bool m_showCheckedOut, m_showModification, m_showCompilation, m_showSizes, m_showComments;
    bool m_compInformation, m_compObsolete;
    bool m_saveBackups, m_confirmDelete;
};

struct LibraryPanterOptionsInclude
{
    wxString m_name;
    bool m_app, m_pipeline, m_datawin, m_function, m_menu, m_project, m_proxy, m_query, m_structure, m_userobject, m_wndow;
};

struct LibraryPainterOptionsManagement
{
    bool m_logEverything, m_requireComment;
    wxString m_logFile;
    int m_options;
};

struct LibraryPainterOptions
{
    LibraryPainterOptionsGeneral m_general;
    LibraryPanterOptionsInclude m_include;
    LibraryPainterOptionsManagement m_management;
};

struct Configuration
{
    std::map<wxString, ToolbarSetup> m_tbSettings;
    int m_querySource, m_queryPresentation;
    DatabaseOptions m_dbOptions;
    LibraryPainterOptions m_libPainterOptions;
    wxString m_currentLibrary;
    wxLayoutDirection m_textDirection;
    DesignProperties m_desginProp;
};



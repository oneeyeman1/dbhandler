#pragma once

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
    }
};

struct DatabaseOptionColors
{
    wxColor m_background, m_tableCol, m_indexLine, m_tableHeader, m_tableColText, m_primaryKeyLine, m_tableHeaderText, m_tableCommentText, m_foreignKeyLine;
};

struct DatabaseOptions
{
    DBOptionGeneral m_general;
    DatabaseOptionColors m_colors;
};

struct Configuration
{
    std::map<wxString, ToolbarSetup> m_tbSettings;
    int m_querySource, m_queryPresentation;
    DatabaseOptions m_dbOptions;
};



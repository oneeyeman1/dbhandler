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
};

struct DatabaseptionColors
{

};

struct DatabaseOptions
{
    DBOptionGeneral m_general;
    DatabaseptionColors m_colors;
};

struct Configuration
{
    std::map<wxString, ToolbarSetup> m_tbSettings;
    int m_querySource, m_queryPresentation;
    DatabaseOptions m_dbOptions;
};



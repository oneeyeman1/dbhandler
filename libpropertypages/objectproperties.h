#pragma once
struct DesignOptions
{
    int units, interval, display;
    wxColour colorBackground;
    bool customMove, mouseSelect, rowResize;
    int cursor;
    wxString cursorName;
};

struct BandProperties
{
    wxString m_color = "Transparent";
    wxString m_cursorFile, m_stockCursor;
    wxString m_type;
    int m_height = 200, m_cursor = -1;
    bool m_autosize = false;
};


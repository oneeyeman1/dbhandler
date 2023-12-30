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
    wxString m_color;
    wxString m_cursorFile, m_stockCursor;
    wxString m_type;
    int m_height, m_cursor;
};


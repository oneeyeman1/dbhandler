#pragma once

struct BandGeneralProperties
{
    wxString m_color = "Transparent";
    int m_height = 200;
    bool m_autosize = false;
};

struct BandConditionsProperties
{
    wxString m_colorCondition, m_cursorCondition;
};

struct BandProperties
{
    BandGeneralProperties m_general;
    BandConditionsProperties m_expressions;
    wxString m_cursorFile;
    int m_stockCursor;
    wxString m_type;
};

struct FieldTableHeadingProperties
{
    wxString m_label, m_heading;
    int m_labelAlignment, m_headingAlignment;
};

struct FieldTableDisplayProperties
{
    std::vector<std::pair<wxString,wxString> > m_format;
    int m_justify, m_height, m_width;
};

struct FieldTableValidationProperties
{
    wxArrayString m_rules;
    int m_inirial;
};

struct FieldTableProperties
{
    wxString m_comment;
    FieldTableHeadingProperties m_heading;
    FieldTableDisplayProperties m_display;
    FieldTableValidationProperties m_valdation;
    wxArrayString m_styles;
};

struct DesignCanvasGeneralProperties
{
    int units, interval;
    wxColour colorBackground;
};

struct DesignCanvasPrintProperties
{
    wxString m_documentName;
    int m_left, m_right, m_top, m_bottom;
    int m_orientation, m_size, m_source;
    bool m_prompt;
    int m_newPaperColumn, m_newPaperAcross;
    bool m_displayButtonsPrint, m_displayButtonsPreview;
};

struct DesignProperties
{
    DesignCanvasGeneralProperties m_general;
    int cursor;
    wxString cursorName;
    DesignCanvasPrintProperties m_printSpec;
};

struct DesignLabelGeneralProperties
{
    wxString m_name, m_tag, m_text;
    bool m_supressPrint;
    int m_border, m_alignment;
};

struct DesgnLabelPositionProperties
{
    wxPoint m_position;
    wxSize m_size;
    bool m_reszable, m_moveale, m_autosize;
    int m_layer, m_slideUp;
    bool m_slideLeft;
};

struct DesignLabelProperties
{
    DesignLabelGeneralProperties m_general;
    wxFont m_font;
    DesgnLabelPositionProperties m_position;;
    wxString m_cursor;
};

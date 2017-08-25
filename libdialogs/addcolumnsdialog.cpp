// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <vector>
#include <string>
#include "addcolumnsdialog.h"

AddColumnsDialog::AddColumnsDialog(wxWindow *parent, int type, const std::vector<std::wstring> &fields, const wxString &dbType, const wxString &dbSubtype) : wxDialog( parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0 )
{
    m_dbType = dbType;
    m_dbSubtype = dbSubtype;
    m_type = type;
    m_allFields = fields;
    // begin wxGlade: MyDialog::MyDialog
    m_panel = new wxPanel( this, wxID_ANY );
    m_fields = new wxListBox( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
    m_paste = new wxButton( m_panel, wxID_OK, _( "Paste" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    set_properties();
    do_layout();
    // end wxGlade
    m_paste->Bind( wxEVT_UPDATE_UI, &AddColumnsDialog::OnPasteUpdateUI, this );
}

AddColumnsDialog::~AddColumnsDialog(void)
{
}

void AddColumnsDialog::set_properties()
{
    m_paste->Enable( false );
    m_paste->SetDefault();
    if( m_type == 2 )
    {
        if( ( m_dbType == "ODBC" && m_dbSubtype == "Sybase SQL Anywhere ") || m_dbType == "Sybase SQL Anywhere" )
        {
            m_fields->Append( "abs()" );
            m_fields->Append( "avg()" );
            m_fields->Append( "count()" );
            m_fields->Append( "date()" );
            m_fields->Append( "dateformat()" );
            m_fields->Append( "datetime()" );
            m_fields->Append( "day()" );
            m_fields->Append( "days()" );
            m_fields->Append( "dow()" );
            m_fields->Append( "hour()" );
            m_fields->Append( "hours()" );
            m_fields->Append( "ifnull()" );
            m_fields->Append( "isnull()" );
            m_fields->Append( "length()" );
            m_fields->Append( "list()" );
            m_fields->Append( "max()" );
            m_fields->Append( "min()" );
            m_fields->Append( "minute()" );
            m_fields->Append( "minutes()" );
            m_fields->Append( "mod()" );
            m_fields->Append( "month()" );
            m_fields->Append( "months()" );
            m_fields->Append( "now(*)" );
            m_fields->Append( "number(*)" );
            m_fields->Append( "remainder()" );
            m_fields->Append( "second()" );
            m_fields->Append( "seconds()" );
            m_fields->Append( "similar()" );
            m_fields->Append( "soundex()" );
            m_fields->Append( "string()" );
            m_fields->Append( "substr()" );
            m_fields->Append( "sum()" );
            m_fields->Append( "today(*)" );
            m_fields->Append( "weeks()" );
            m_fields->Append( "year()" );
            m_fields->Append( "years()" );
            m_fields->Append( "ymd()" );
        }
        if( ( m_dbType == "ODBC" && m_dbSubtype == "PostgreSQL" ) || m_dbType == "PostgreSQL" )
        {
            m_fields->Append( "abbrev" );
            m_fields->Append( "abs" );
            m_fields->Append( "acos" );
            m_fields->Append( "age" );
            m_fields->Append( "area" );
            m_fields->Append( "array_agg" );
            m_fields->Append( "array_append" );
            m_fields->Append( "array_cat" );
            m_fields->Append( "array_ndims" );
            m_fields->Append( "array_dims" );
            m_fields->Append( "array_fill" );
            m_fields->Append( "array_length" );
            m_fields->Append( "array_lower" );
            m_fields->Append( "array_prepend" );
            m_fields->Append( "array_to_string" );
            m_fields->Append( "array_upper" );
            m_fields->Append( "ascii" );
            m_fields->Append( "asin" );
            m_fields->Append( "atan" );
            m_fields->Append( "atan2" );
            m_fields->Append( "avg" );
            m_fields->Append( "bit_and" );
            m_fields->Append( "bit_length" );
            m_fields->Append( "bit_or" );
            m_fields->Append( "box" );
            m_fields->Append( "broadcast" );
            m_fields->Append( "btrim" );
            m_fields->Append( "cbrt" );
            m_fields->Append( "ceil" );
            m_fields->Append( "ceiling" );
            m_fields->Append( "center" );
            m_fields->Append( "char_length" );
            m_fields->Append( "chr" );
            m_fields->Append( "circle" );
            m_fields->Append( "clock_timestamp" );
            m_fields->Append( "COALESCE" );
            m_fields->Append( "concat" );
            m_fields->Append( "convert" );
            m_fields->Append( "convert_from" );
            m_fields->Append( "convert_to" );
            m_fields->Append( "corr" );
            m_fields->Append( "cos" );
            m_fields->Append( "cot" );
            m_fields->Append( "count" );
            m_fields->Append( "covar_pop" );
            m_fields->Append( "covar_samp" );
            m_fields->Append( "cume_dist" );
            m_fields->Append( "current_date" );
            m_fields->Append( "current_time" );
            m_fields->Append( "current_timestamp" );
            m_fields->Append( "currval" );
            m_fields->Append( "cursor_to_xml" );
            m_fields->Append( "date_part" );
            m_fields->Append( "date_trunc" );
            m_fields->Append( "decode" );
            m_fields->Append( "degrees" );
            m_fields->Append( "dence_rank" );
            m_fields->Append( "diameter" );
            m_fields->Append( "div" );
            m_fields->Append( "encode" );
            m_fields->Append( "enum_first" );
            m_fields->Append( "enum_last" );
            m_fields->Append( "enum_range" );
            m_fields->Append( "every" );
            m_fields->Append( "exp" );
            m_fields->Append( "extract" );
            m_fields->Append( "family" );
            m_fields->Append( "first_value" );
            m_fields->Append( "floor" );
            m_fields->Append( "format" );
            m_fields->Append( "generate_series" );
            m_fields->Append( "generate_subscripts" );
            m_fields->Append( "get_bit" );
            m_fields->Append( "get_byte" );
            m_fields->Append( "get_current_ts_config" );
            m_fields->Append( "GREATEST" );
            m_fields->Append( "height" );
            m_fields->Append( "host" );
            m_fields->Append( "hostmask" );
            m_fields->Append( "initcap" );
            m_fields->Append( "isclosed" );
            m_fields->Append( "isfinite" );
            m_fields->Append( "isopen" );
            m_fields->Append( "justify_days" );
            m_fields->Append( "justify_hours" );
            m_fields->Append( "justify_interval" );
            m_fields->Append( "lag" );
            m_fields->Append( "last_value" );
            m_fields->Append( "lastval" );
            m_fields->Append( "lead" );
            m_fields->Append( "LEAST" );
            m_fields->Append( "left" );
            m_fields->Append( "length" );
            m_fields->Append( "ln" );
            m_fields->Append( "localtime" );
            m_fields->Append( "localtimestamp" );
            m_fields->Append( "log" );
            m_fields->Append( "lower" );
            m_fields->Append( "lpad" );
            m_fields->Append( "lseg" );
            m_fields->Append( "ltrim" );
            m_fields->Append( "masklen" );
            m_fields->Append( "max" );
            m_fields->Append( "md5" );
            m_fields->Append( "min" );
            m_fields->Append( "mod" );
            m_fields->Append( "netmask" );
            m_fields->Append( "network" );
            m_fields->Append( "nextval" );
            m_fields->Append( "now" );
            m_fields->Append( "npoints" );
            m_fields->Append( "nth_value" );
            m_fields->Append( "ntile" );
            m_fields->Append( "NULLIF" );
            m_fields->Append( "numnode" );
            m_fields->Append( "octet" );
            m_fields->Append( "octet_length" );
            m_fields->Append( "overlay" );
            m_fields->Append( "path" );
            m_fields->Append( "pclose" );
            m_fields->Append( "percent_rank" );
            m_fields->Append( "pg_client_encoding" );
            m_fields->Append( "pi" );
            m_fields->Append( "plainto_tsquery" );
            m_fields->Append( "point" );
            m_fields->Append( "polygon" );
            m_fields->Append( "popen" );
            m_fields->Append( "position" );
            m_fields->Append( "power" );
            m_fields->Append( "query_to_xml" );
            m_fields->Append( "querytree" );
            m_fields->Append( "quote_ident" );
            m_fields->Append( "quote_literal" );
            m_fields->Append( "quote_nullable" );
            m_fields->Append( "radians" );
            m_fields->Append( "radius" );
            m_fields->Append( "random" );
            m_fields->Append( "rank" );
            m_fields->Append( "regexp_matches" );
            m_fields->Append( "regexp_replace" );
            m_fields->Append( "regexp_split_to_array" );
            m_fields->Append( "regexp_split_to_table" );
            m_fields->Append( "regr_avgx" );
            m_fields->Append( "regr_avgy" );
            m_fields->Append( "regr_count" );
            m_fields->Append( "regr_intercept" );
            m_fields->Append( "regr_r2" );
            m_fields->Append( "regr_slcpe" );
            m_fields->Append( "regr_sxx" );
            m_fields->Append( "regr_sxy" );
            m_fields->Append( "regr_syy" );
            m_fields->Append( "repeat" );
            m_fields->Append( "replace" );
            m_fields->Append( "reverse" );
            m_fields->Append( "right" );
            m_fields->Append( "round" );
            m_fields->Append( "row_number" );
            m_fields->Append( "rpad" );
            m_fields->Append( "rtrim" );
            m_fields->Append( "set_bit" );
            m_fields->Append( "set_byte" );
            m_fields->Append( "set_masklen" );
            m_fields->Append( "setseed" );
            m_fields->Append( "setval" );
            m_fields->Append( "setweight" );
            m_fields->Append( "sign" );
            m_fields->Append( "sin" );
            m_fields->Append( "split_part" );
            m_fields->Append( "sqrt" );
            m_fields->Append( "statement_timestamp" );
            m_fields->Append( "stddev" );
            m_fields->Append( "stddev_pop" );
            m_fields->Append( "stddev_samp" );
            m_fields->Append( "string_agg" );
            m_fields->Append( "string_to_array" );
            m_fields->Append( "strip" );
            m_fields->Append( "strpos" );
            m_fields->Append( "substring" );
            m_fields->Append( "sum" );
            m_fields->Append( "table_to_xml" );
            m_fields->Append( "tan" );
            m_fields->Append( "text" );
            m_fields->Append( "timeofday" );
            m_fields->Append( "to_ascii" );
            m_fields->Append( "to_char" );
            m_fields->Append( "to_date" );
            m_fields->Append( "to_hex" );
            m_fields->Append( "to_number" );
            m_fields->Append( "to_timestamp" );
            m_fields->Append( "to_tsquery" );
            m_fields->Append( "to_tsvector" );
            m_fields->Append( "transaction_timestamp" );
            m_fields->Append( "translate" );
            m_fields->Append( "trim" );
            m_fields->Append( "trunc" );
            m_fields->Append( "ts_headline" );
            m_fields->Append( "ts_rank" );
            m_fields->Append( "ts_rank_cd" );
            m_fields->Append( "ts_rewrite" );
            m_fields->Append( "tsvector_update_trigger" );
            m_fields->Append( "tsvector_update_trigger_column" );
            m_fields->Append( "unnest" );
            m_fields->Append( "upper" );
            m_fields->Append( "var_pop" );
            m_fields->Append( "var_samp" );
            m_fields->Append( "variance" );
            m_fields->Append( "width" );
            m_fields->Append( "xml_is_well_formed" );
            m_fields->Append( "xml_is_well_formed_content" );
            m_fields->Append( "xml_is_well_formed_document" );
            m_fields->Append( "xmlagg" );
            m_fields->Append( "xmlcomment" );
            m_fields->Append( "xmlconcat" );
            m_fields->Append( "xmlelement" );
            m_fields->Append( "xmlforest" );
            m_fields->Append( "xpath" );
            m_fields->Append( "xpath_exists" );
            m_fields->Append( "xmlpi" );
            m_fields->Append( "xmlroot" );
        }
    }
    else
    {
        for( std::vector<std::wstring>::iterator it = m_allFields.begin(); it < m_allFields.end(); it++ )
        {
            m_fields->Append( (*it) );
        }
    }
}

void AddColumnsDialog::do_layout()
{
    // begin wxGlade: MyDialog::do_layout
    wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* sizer2 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer3 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer4 = new wxBoxSizer( wxVERTICAL );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer3->Add( m_fields, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer4->Add( m_paste, 0, 0, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer4->Add( m_cancel, 0, 0, 0 );
    sizer3->Add( sizer4, 0, 0, 0 );
    sizer2->Add( sizer3, 0, 0, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, 0, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer1 );
    sizer->Add( m_panel, 1, 0, 0 );
    SetSizer( sizer );
    sizer->Fit( this );
    Layout();
    // end wxGlade
}

void AddColumnsDialog::OnPasteUpdateUI(wxUpdateUIEvent &event)
{
    if( m_fields->GetSelection() != wxNOT_FOUND )
        event.Enable( true );
    else
        event.Enable( false );
}

wxListBox *AddColumnsDialog::GetFieldsControl() const
{
    return m_fields;
}

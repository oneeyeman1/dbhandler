// This sample is written to build both with wxUSE_STD_IOSTREAM==0 and 1, which
// somewhat complicates its code but is necessary in order to support building
// it under all platforms and in all build configurations
//
// In your own code you would normally use std::stream classes only and so
// wouldn't need these typedefs
#if wxUSE_STD_IOSTREAM
    typedef wxSTD istream DocumentIstream;
    typedef wxSTD ostream DocumentOstream;
#else // !wxUSE_STD_IOSTREAM
    typedef wxInputStream DocumentIstream;
    typedef wxOutputStream DocumentOstream;
#endif // wxUSE_STD_IOSTREAM/!wxUSE_STD_IOSTREAM

// Represents a line from one point to the other
struct DoodleLine
{
    DoodleLine() { /* leave fields uninitialized */ }

    DoodleLine(const wxPoint& pt1, const wxPoint& pt2)
        : x1(pt1.x), y1(pt1.y), x2(pt2.x), y2(pt2.y)
    {
    }

    wxInt32 x1;
    wxInt32 y1;
    wxInt32 x2;
    wxInt32 y2;
};

typedef wxVector<DoodleLine> DoodleLines;

// Contains a list of lines: represents a mouse-down doodle
class DoodleSegment
{
public:
    DocumentOstream& SaveObject(DocumentOstream& stream);
    DocumentIstream& LoadObject(DocumentIstream& stream);

    bool IsEmpty() const { return m_lines.empty(); }
    void AddLine(const wxPoint& pt1, const wxPoint& pt2)
    {
        m_lines.push_back(DoodleLine(pt1, pt2));
    }
    const DoodleLines& GetLines() const { return m_lines; }

private:
    DoodleLines m_lines;
};

typedef wxVector<DoodleSegment> DoodleSegments;
// The drawing document (model) class itself
class DrawingDocument : public wxDocument
{
public:
    DrawingDocument() : wxDocument() { }

    DocumentOstream& SaveObject(DocumentOstream& stream) wxOVERRIDE;
    DocumentIstream& LoadObject(DocumentIstream& stream) wxOVERRIDE;

    // add a new segment to the document
    void AddDoodleSegment(const DoodleSegment& segment);
    void SetDatabase(Database *db);
    // remove the last segment, if any, and copy it in the provided pointer if
    // not NULL and return true or return false and do nothing if there are no
    // segments
    bool PopLastSegment(DoodleSegment *segment);

    // get direct access to our segments (for DrawingView)
    const DoodleSegments& GetSegments() const { return m_doodleSegments; }

    void AddTables(const std::vector<wxString> &selections);
    std::vector<Table> &GetTables();

private:
    void DoUpdate();

    DoodleSegments m_doodleSegments;
    Database *m_db;
    std::vector<Table> m_tables;
    wxDECLARE_DYNAMIC_CLASS(DrawingDocument);
};

// Base class for all operations on DrawingDocument
class DrawingCommand : public wxCommand
{
public:
    DrawingCommand(DrawingDocument *doc, const wxString& name, const DoodleSegment& segment = DoodleSegment())
        : wxCommand(true, name),
          m_doc(doc),
          m_segment(segment)
    {
    }

protected:
    bool DoAdd() { m_doc->AddDoodleSegment(m_segment); return true; }
    bool DoRemove() { return m_doc->PopLastSegment(&m_segment); }

private:
    DrawingDocument * const m_doc;
    DoodleSegment m_segment;
};

// The command for adding a new segment
class DrawingAddSegmentCommand : public DrawingCommand
{
public:
    DrawingAddSegmentCommand(DrawingDocument *doc, const DoodleSegment& segment)
        : DrawingCommand(doc, "Add new segment", segment)
    {
    }

    virtual bool Do() wxOVERRIDE { return DoAdd(); }
    virtual bool Undo() wxOVERRIDE { return DoRemove(); }
};


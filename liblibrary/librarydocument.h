#pragma once
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

class LibraryDocument :  public wxDocument
{
public:
    LibraryDocument() : wxDocument() { }
    ~LibraryDocument();
    void SetLiraryObject(const LibraryObject *library) { m_libraryObject = const_cast<LibraryObject *>( library ); }
    LibraryObject *GetLibraryObject() const { return m_libraryObject; }
    DocumentOstream& SaveObject(DocumentOstream& stream) wxOVERRIDE;
    DocumentIstream& LoadObject(DocumentIstream& stream) wxOVERRIDE;
private:
    LibraryObject *m_libraryObject;
    wxDECLARE_DYNAMIC_CLASS(LibraryDocument);
};

#ifndef PACK_INCLUDE
#define PACK_INCLUDE

#include "stdafx.h"

#include "CStr.hpp"
#include "CWStr.hpp"
#include "CException.hpp"
#include "CMain.hpp"
#include "CHeap.hpp"
#include "CBuf.hpp"

namespace Base {

//#define RAISE_ALL_EXCEPTIONS
//#define HANDLE_OUT_OF_PACK_FILES
//#define SUPPORT_IN_MEMORY_STRUCTURES

enum EFileType
{
    FILEEC_TEXT         = 0,
    FILEEC_BINARY       = 1,
    FILEEC_COMPRESSED   = 2,
    FILEEC_FOLDER       = 3,

    FILEEC_FORCE_DWORD   = 0x7fffffff
};

#define MAX_VIRTUAL_HANDLE_COUNT_BITS   4
#define MAX_FILENAME_LENGTH         63 // ������������ ����� ����� �����



#define MAX_VIRTUAL_HANDLE_COUNT    (1 << MAX_VIRTUAL_HANDLE_COUNT_BITS) // do not modify this


struct SFileRec
{
    DWORD   m_Size;                 // ������ ����� (+4) - ��� ������ �����
    DWORD   m_RealSize;             // ��������� ������ ����� (�� �������)
    char    m_Name[MAX_FILENAME_LENGTH];      // ��� �����
    char    m_RealName[MAX_FILENAME_LENGTH];  // ��� ����� � �������� ������� � ������ ���������
    EFileType   m_Type;                 // ��� �����, �� ������� ��������� ������
    EFileType   m_NType;                // ��� ������� ������ ���� �������� �����
    DWORD   m_Free;                 // �������� ������ ��������� ��� ������
    DWORD   m_Date;                 // ���� � ����� �����
    DWORD   m_Offset;               // �������� ������ ������������ ������ �����
    DWORD   m_Extra;                // ������ �� ����� ������ �������
};
typedef SFileRec *PSFileRec;

struct SFolderRec
{
    DWORD   m_Size;             // ������ ���� ����������
    DWORD   m_Recnum;           // ���������� ������� � ���
    DWORD   m_RecSize;          // ������ ����� ������ �����
};
typedef SFolderRec *PSFolderRec;

struct SFileHandleRec
{

    DWORD   m_Handle;       // ���������� ����� �����
    DWORD   m_StartOffset;  // �������� ������ ����� ������������ ������ ��������� �����
    DWORD   m_Offset;       // ��������� ��������
    DWORD   m_Size;         // ������ �����
    BYTE   *m_SouBuf;       // ����� ��� ������ ������ ������
    BYTE   *m_DesBuf;       // ����� ��� �������� ������
    int     m_Blocknumber;  // ����� ����� � ������ �����
    bool    m_Free;         // ������ ��������
    bool    m_Compressed;   // �������� �� ���� ������
    WORD    dummy00;    // align
};


typedef bool (*FILENAME_CALLBACK_FUNC)(bool Dir, bool Compr, const CStr &name);
class CHsFolder;

struct SSearchRec
{

    CStr    Name;
    CStr    Path;
    int     Ind;
    DWORD   T;
    CHsFolder *Folder;

    SSearchRec(CHeap *heap):Name(heap), Path(heap) {}

};

class CHsFolder : public CMain
{
    CHeap       *m_Heap;

    CStr        m_Name;             // ��� ����� ��� ����� ���������
    CStr        m_RealName;         // ��� ����� � ������ ���������
    SFolderRec  m_FolderRec;
    CHsFolder  *m_Parent;
    SFileRec   *m_Files;            // ������, ��������� � �������
    //bool        m_ToUpdate;         // ���������� �������� ���������� � �������� �����
    //bool        m_ToSave;           // ���������� �������� �� ����� ����� � �������� ����

    SFileRec *  GetFileRec(int ind) const // ���������� ������ �� ������
    {
        if (DWORD(ind)<m_FolderRec.m_Recnum)
        {
            return (SFileRec *)(((BYTE *)m_Files) + m_FolderRec.m_RecSize *ind);
        }
        return NULL;
    }


    SFileRec *  GetFreeRec(void) const;
    bool        AddFileEx(const CStr &name,DWORD date, DWORD size, EFileType ftype); // ��������� ���� � �������� ����
public:
    CHsFolder(const CStr &name, CHeap *heap);                    // ������� ������ �����, �� ��������� � ������
    CHsFolder(const CStr &name,CHsFolder *Parent, CHeap *heap);  // ������� ������ �����, �� ��������� � ������
    ~CHsFolder() {Clear();};                                     // ���������� ��� ������, ��������� � ��������

    bool    ReadFolder(DWORD Handle, DWORD Offset); // ������ ������ �� ��������� �����
    //void    AllocEmptyFolder(void);

    void    Clear(void);                            // ������� ������ � ���������� � ������
    bool    FileExists(const CStr &name) const;     // ���������� �� ��������� ����
    bool    PathExists(const CStr &name) const;     // ���������� �� ��������� ����
    SFileRec*   GetFileRec(const CStr &name) const;       // ���������� ������ �� ����� �����
    SFileRec*   GetFileRecEx(const CStr &name) const;     // ���������� ������ �� ���� �����
    CHsFolder*  GetFolderEx(const CStr &path);
    int         FileRecsNumber(void) const         // ���������� ���������� ������� � ����������
    {
        return m_FolderRec.m_Recnum;
    }

    //bool        ReAllocFileRecs(int number);        // �������� ���������� ������� � ���������� (������ � ������� ����������)
    void        UpdateFileRec(void);                // ��������� ���������� �� ���������� �������� �������

    SFolderRec*  GetFolderRec(void)
    {
        return &m_FolderRec;
    }

        //*** �������, ������������ ��� �������� � �������� ����

    //bool    CreateFolder(const CStr &name);
    //bool    AddPath(const CStr &name);
    //bool    AddPath(const CStr &name,EFileType ftype);
    //bool    AddFile(const CStr &name);
    //bool    AddFile(const CStr &name,EFileType ftype);
    //bool    DeleteFile(const CStr &name);
    CStr    GetFullPath(const CStr &name);

    DWORD   CompressedFileSize(DWORD Handle, const CStr &filename);
    DWORD   DecompressedFileSize(DWORD Handle, const CStr &filename);
    int     GetSize(void)
    {
        return GetLocalSize();
    }

    int     GetLocalSize(void);
    int     GetFolderSize(void)
    {
        return sizeof(SFolderRec) + m_FolderRec.m_Recnum*m_FolderRec.m_RecSize;
    }

    //DWORD   Pack(DWORD Handle,DWORD Offset,int *PInt,const CStr &PStr);
    //DWORD   PackFile(DWORD Handle,const CStr &name,DWORD Offset);
    //DWORD   PackAndCompressFile(DWORD Handle,const CStr &name,DWORD Offset);

    //DWORD   DecodeFile(DWORD Handle,SFileRec *PFile,DWORD Offset);
    //DWORD   EncodeFile(DWORD Handle,SFileRec *PFile,DWORD Offset);

    //DWORD   CompressFile(DWORD SouHandle,DWORD Handle,const CStr &name,DWORD Offset);
    //DWORD   Compress(DWORD SouHandle,DWORD Handle,DWORD Offset,int *PInt,const CStr &PStr);
    //void    CompressFolder(void);

    //    //*** �������, ������������ ��� ���������� ��������� �����
    //bool    UnpackFile(DWORD SouHandle,const CStr &name);
    //bool    UnpackCompressedFile(DWORD SouHandle,const CStr &name);
    //bool    UnpackFolder(const CStr &name);
    //bool    Unpack(DWORD SouHandle,int *PInt,const CStr &PStr);

    void    ListFileNames(FILENAME_CALLBACK_FUNC Func);

        //*** �������, ������������ ��� ��������� ���� ����� *****
    //void    SetFileType(const CStr &name, EFileType NType);
    //void    SetFolderType(const CStr &name, EFileType NType);

    int     FindNext(SSearchRec &S);
};

#ifdef SUPPORT_IN_MEMORY_STRUCTURES
#define PFFLAG_EMPTY    SETBIT(0)       // ��������� ��������� �� ������� �� � ����� ������
#endif

class CPackFile : public CMain
{
public:
    CHeap          *m_Heap;
    CWStr           m_FileName;                 // ��� ��������� �����

#ifdef SUPPORT_IN_MEMORY_STRUCTURES
    DWORD           m_Flags;
#endif

        // ��������� ��������� ����������� ������ � ������ - �� ������� � ������
    DWORD           m_Handle;                   // �������� �����
    CHsFolder      *m_RootFolder;               // �������� �������
    SFileHandleRec  m_Handles[MAX_VIRTUAL_HANDLE_COUNT];
    DWORD           m_RootOffset;               // �������� ��������� �������� ������������ ������ �����
    //DWORD           m_WorkFileSize;             // ������ �������� ����� (�� ���������)
    //DWORD           m_WorkFileOffset;           // �������� �������� �����
    //DWORD           m_WorkFileStartOffset;      // ��������� �������� �������� �����
    //DWORD           m_ID;                       // ����������������� ����� ������ � ������.
    
    int             GetFreeHandle(void);
    
    // ������������� ��������� � ���� - ���������� ������ ������� �����
    DWORD           SetCompressedBlockPointer(DWORD StartOffset,int nBlock);
public:
    CPackFile(CHeap *heap, const wchar *name);
    ~CPackFile(void);

    const CWStr GetName(void) const {return m_FileName;}

        // ******* ��������� ������ � �������� ������ ******** //
    bool    OpenPacketFile(void);               // ��������� �������� ���� � ��������� ������ ������
    bool    ClosePacketFile(void);              // ��������� �������� ���� � ���������� ����������� ������� ������
    //bool    CreatePacketFileEx(void);               // ������� ����� �������� ���� ������/������
    bool    OpenPacketFileEx(void);             // ��������� �������� ���� � ��������� ������ ������/������
    bool    ClosePacketFileEx(void)            // ��������� �������� ���� � ���������� ����������� ������� ������/������
    {
        return ClosePacketFile();
    }
    DWORD   GetHandle(void) const {return m_Handle;}
    void    Clear(void);
        //***** ��������� ������ � ������� -- ���������������� ��������� � ���� ������� �� ������ PackFile
    DWORD   Open(const CStr &filename,DWORD modeopen=GENERIC_READ|GENERIC_WRITE);
    bool    Close(DWORD Handle);
    bool    Read(DWORD Handle, void *buf,int Size);
#ifdef HANDLE_OUT_OF_PACK_FILES
    bool    Write(DWORD Handle, const void *buf,int Size);
#endif
    bool    SetPos(DWORD Handle, DWORD Pos, int ftype=FILE_BEGIN);
    DWORD   GetPos(DWORD Handle);
    DWORD   GetSize(DWORD Handle);
    DWORD   GetHandle(DWORD Handle);

        //***** ������������� ��������� ������ � ������� ****

#ifdef HANDLE_OUT_OF_PACK_FILES
    bool    PathExists(const CStr &path);
    bool    FileExists(const CStr &path);
#else
    bool    PathExists(const CStr &path)
    {
        return m_RootFolder->PathExists(path);
    }
    bool    FileExists(const CStr &path)
    {
        return m_RootFolder->FileExists(path);
    }
#endif
    DWORD   CompressedFileSize(const CStr &filename)
    {
        if (m_Handle == 0xFFFFFFFF) return 0xFFFFFFFF;
        if (m_RootFolder) return m_RootFolder->CompressedFileSize(m_Handle,filename);
        return 0xFFFFFFFF;
    }

    DWORD   DecompressedFileSize(const CStr &filename)
    {
        if (m_Handle == 0xFFFFFFFF) return 0xFFFFFFFF;
        if (m_RootFolder) return m_RootFolder->DecompressedFileSize(m_Handle,filename);
        return 0xFFFFFFFF;
    }

        //***** ��������� ������ � �������� ������ **********
    //bool    AddFile(const CStr &name);
    //bool    AddFile(const CStr &name,EFileType ftype);
    //bool    AddFiles(CBlockPar &block, CStr &log);
    //bool    AddFiles(CBlockPar &block);
    //bool    AddPath(const CStr &name);
    //bool    AddPath(const CStr &name,EFileType ftype);
    //bool    DeleteFile(const CStr &name);
    //bool    CreateFolder(const CStr &name);

    //bool    Compress(int *PInt,CStr &PStr);
    //bool    Compress(const CStr &name,int *PInt,CStr &PStr);
    //bool    Pack(int *PInt,CStr &PStr);
    //bool    Unpack(int *PInt,CStr &PStr);
    //bool    Compress(void);
    //bool    Compress(const CStr &name);
    //bool    Pack(void);
    //bool    Unpack(void);
    //bool    UnpackFile(const CStr &souname);
    //bool    UnpackFile(const CStr &souname,const CStr &desname);

    void    ListFileNames(FILENAME_CALLBACK_FUNC Func)
    {
        if (m_Handle == 0xFFFFFFFF) return;
        if (m_RootFolder)
        {
            m_RootFolder->ListFileNames(Func);
        }
    }

        //*** �������, ������������ ��� ��������� ���� ����� *****
    //void    SetFileType(const CStr &name, EFileType NType)
    //{
    //    if (m_RootFolder == NULL) return;
    //    if (m_Handle == 0xFFFFFFFF) return;
    //    m_RootFolder->SetFileType(name, NType);
    //}
    //void    SetFolderType(const CStr &name, EFileType NType)
    //{
    //    if (m_RootFolder == NULL) return;
    //    if (m_Handle == 0xFFFFFFFF) return;
    //    m_RootFolder->SetFolderType(name, NType);
    //}



    int     FindFirst(const CStr &path,DWORD Attr, SSearchRec &S);
    int     FindNext(SSearchRec &S)
    {
        if (m_RootFolder == NULL) return 1;
        if (S.Folder == NULL) return 1;
        return S.Folder->FindNext(S);
    }
    void    FindClose(SSearchRec &S)
    {
        S.Folder = NULL;
        S.Path = "";
        S.Name = "";
    }
};

typedef CPackFile        *PCPackFile;

class CPackCollection : public CMain
{
public:
    CHeap           *m_Heap;
    CBuf             m_PackFiles;

public:
    CPackCollection(CHeap *heap):m_Heap(heap), m_PackFiles(heap) {}
    ~CPackCollection() { Clear(); };

    //******** ��������� ������ �� ������� �������� ������ ********//
    void    Clear(void);
    void    AddPacketFile(const wchar *filename);
    void    DelPacketFile(const wchar *filename);
    //******** ��������� ��� ������ � ��������� ������� ***********//
    bool        OpenPacketFiles(void);
    bool        ClosePacketFiles(void);
    bool        OpenPacketFilesEx(void);
    bool        ClosePacketFilesEx(void);
    CPackFile*  GetPacketFile(int i) {return m_PackFiles.Buff<PCPackFile>()[i];};
    //******** ��������� ��� ������ ������� ***********//
    bool        FileExists(const CStr &name);
    bool        PathExists(const CStr &path);
    //******* ������ � ������������ �������� �������� CPackFile
    DWORD       Open(const CStr&name, DWORD modeopen=GENERIC_READ);
    bool        Close(DWORD Handle);
    bool        Read(DWORD Handle, void *Buf, int Size);
    //bool        Write(DWORD Handle, const void *Buf, int Size);
    bool        SetPos(DWORD Handle, DWORD Pos, int ftype=FILE_BEGIN);
    DWORD       GetPos(DWORD Handle) ;
    DWORD       GetSize(DWORD Handle) ;
    DWORD       GetHandle(DWORD Handle);
    //******* ������ � ������������ ������� *********************
    //bool        UnpackFile(const CStr&souname) {UnpackFile(souname,souname);};
    //bool        UnpackFile(const CStr&souname,const CStr&desname);

};


} // namespace Base
#endif
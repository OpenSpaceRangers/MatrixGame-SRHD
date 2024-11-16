#include "stdafx.h"
#include "Pack.hpp"

//#ifdef BLABLA


#undef ZEXPORT
#define ZEXPORT _cdecl

#include "../../ZLIB/ZLIB/zlib.h"



//#define _MAKESTR1(x) #x
//#define MAKESTR(n) _MAKESTR1(n)


//char *sss2 = MAKESTR(5);
//
//char *sss3 = MAKESTR(ZEXPORT)
//char *sss4 = MAKESTR(ZEXTERN)
//char *sss5 = MAKESTR(OF)

// @rdesc - ������ ������ ������  0 - ������
//static int OKGF_ZLib_Compress(BYTE * desbuf,BYTE * soubuf,int len_sou_and_des_buf,int fSpeed)
//{
//
//    DWORD deslen=len_sou_and_des_buf-8;
//    if(len_sou_and_des_buf<16) return 0;
//    if(compress2(desbuf+8,&deslen,soubuf,len_sou_and_des_buf,fSpeed?Z_BEST_SPEED:Z_BEST_COMPRESSION)!=Z_OK) return 0;
//    *(desbuf+0)='Z';
//    *(desbuf+1)='L';
//    *(desbuf+2)='0';
//    *(desbuf+3)='1';
//    *((DWORD *)(desbuf+4))=len_sou_and_des_buf;
//    return deslen+8;
//}

// if desbuf==NULL then return ������ ��������� ������
// return ������ ��������� ������
// return 0 ���� ������
static int OKGF_ZLib_UnCompress(BYTE * desbuf,int lendesbuf,BYTE * soubuf,int lensoubuf)
{
    DWORD deslen=lendesbuf;

    if(lensoubuf<8) return 0;
    if(*(soubuf+0)!='Z' || *(soubuf+1)!='L' || *(soubuf+2)!='0' || *(soubuf+3)!='2') return 0;
    if(desbuf==NULL) return *((DWORD *)(soubuf+4));
    if(*((DWORD *)(soubuf+4))>DWORD(lendesbuf)) return 0;

    if(uncompress(desbuf,&deslen,soubuf+8,lensoubuf-8)!=Z_OK) return 0;
    if(deslen!=*((DWORD *)(soubuf+4))) return 0;
    return deslen;
}


//int OKGF_ZLib_Compress2(BYTE * desbuf,int len_des_buf, BYTE * soubuf,int len_sou_buf,int fSpeed)
//{
//    DWORD deslen=len_des_buf-8;
////  if(len_sou_buf<16) return 0;
//    if(compress2(desbuf+8,&deslen,soubuf,len_sou_buf,fSpeed?Z_BEST_SPEED:Z_BEST_COMPRESSION)!=Z_OK) return 0;
//    *(desbuf+0)='Z';
//    *(desbuf+1)='L';
//    *(desbuf+2)='0';
//    *(desbuf+3)='2';
//    *((DWORD *)(desbuf+4))=len_sou_buf;
//    return deslen+8;
//}

// if desbuf==NULL then return ������ ��������� ������
// return ������ ��������� ������
// return 0 ���� ������
int OKGF_ZLib_UnCompress2(BYTE * desbuf,int len_des_buf,BYTE * soubuf,int len_sou_buf)
{
    DWORD deslen=len_des_buf;

    if(len_sou_buf<8) return 0;
    if(*(soubuf+0)!='Z' || *(soubuf+1)!='L' || *(soubuf+2)!='0' || *(soubuf+3)!='2') return 0;
//  if(desbuf==NULL) return *((DWORD *)(soubuf+4));
    if(*((DWORD *)(soubuf+4))>DWORD(len_des_buf)) return 0;

    if(uncompress(desbuf,&deslen,soubuf+8,len_sou_buf-8)!=Z_OK) return 0;
    if(deslen!=*((DWORD *)(soubuf+4))) return 0;
    return deslen;
}








namespace Base {








//**********************************************************************
//***************** ����� - ������� ************************************
//**********************************************************************

CHsFolder::CHsFolder(const CStr &Name, CHeap *heap):m_Heap(heap), m_Name(Name, heap), m_RealName(Name, heap)
{
    m_Files = NULL;
    m_FolderRec.m_Size = sizeof(SFolderRec);
    m_FolderRec.m_Recnum = 0;
    m_FolderRec.m_RecSize = sizeof(SFileRec);
    m_Parent = NULL;
    m_Name.UpperCase();
}

CHsFolder::CHsFolder(const CStr &Name, CHsFolder *parent, CHeap *heap):m_Heap(heap), m_Name(Name, heap), m_RealName(Name, heap)
{
    m_Files = NULL;
    m_FolderRec.m_Size = sizeof(SFolderRec);
    m_FolderRec.m_Recnum = 0;
    m_FolderRec.m_RecSize = sizeof(SFileRec);
    m_Parent = parent;
    m_Name.UpperCase();
}


SFileRec *  CHsFolder::GetFileRec(const CStr &name) const
{
    SFileRec *PFile;

    CStr n(name, m_Heap);
    n.UpperCase();
    for (DWORD i=0; i<m_FolderRec.m_Recnum; ++i)
    {
        PFile = GetFileRec(i);
        if (PFile->m_Free == 0)
        {
            if (n == PFile->m_Name) return PFile;
        }
    }
    return NULL;
}

SFileRec  *  CHsFolder::GetFreeRec(void) const
{
    SFileRec *PFile;

    for (DWORD i=0; i<m_FolderRec.m_Recnum; ++i)
    {
        PFile = GetFileRec(i);
        if (PFile->m_Free != 0)
        {
            return PFile;
        }
    }
    return NULL;
}

//Procedure   CHsFolder.AllocEmptyFolder;
//begin
//    m_FolderRec.m_Recnum:=0;
//    m_FolderRec.m_RecSize:=sizeof(TFileRecEC);
//    m_FolderRec.m_Size:=sizeof(TFolderRecEC)+m_FolderRec.m_RecSize*m_FolderRec.m_Recnum;
//    // ������ ���������� ��������� ������, ��������� � ��������� ������� � ������ �����
//    m_Files:=nil;
//    m_ToSave:=true;
//    // � ������ ���������� �������� ���������� � ������������ ����� � ������� � ���������
//    UpdateFileRec;
//end;

bool   CHsFolder::ReadFolder(DWORD Handle, DWORD Offset)
{
    DWORD temp;

    if (m_Files) return false;

    // ������ ��������� �����
    SetFilePointer((HANDLE)Handle,Offset,NULL,FILE_BEGIN);
    BOOL res = ::ReadFile((HANDLE)Handle,&m_FolderRec,sizeof(SFolderRec),&temp,NULL);
    if (res == FALSE || temp != sizeof(SFolderRec)) return false;
    if (sizeof(SFileRec) != m_FolderRec.m_RecSize) return false;

    // ������ ���������� ��������� ������, ��������� � ��������� ������� � ������ �����
    int sz = m_FolderRec.m_Recnum*m_FolderRec.m_RecSize;
    m_Files = (SFileRec *)HAllocClear(sz, m_Heap);

    res = ::ReadFile((HANDLE)Handle, m_Files, sz, &temp, NULL);
    if (res == FALSE || temp!=sz)
    {
        for (DWORD i=0; i<m_FolderRec.m_Recnum; ++i)
        {
            m_Files[i].m_Extra = 0;
        }
        Clear();
        return false;
    }
    for (DWORD i=0; i<m_FolderRec.m_Recnum; ++i)
    {
        m_Files[i].m_Extra = 0;
        m_Files[i].m_NType = m_Files[i].m_Type;
    }

    // ��� ���������� � ������ � ������� ���������� ���������
    // ������ ������ ���������� � ��������� ������
    for (DWORD i=0; i<m_FolderRec.m_Recnum; ++i)
    {
        if (m_Files[i].m_Type == FILEEC_FOLDER && m_Files[i].m_Free == 0)
        {
            CHsFolder *PFolder = HNew(m_Heap) CHsFolder(CStr(m_Files[i].m_RealName, m_Heap),this, m_Heap);
            m_Files[i].m_Extra = DWORD(PFolder);
            if (!PFolder->ReadFolder(Handle,m_Files[i].m_Offset))
            {
                Clear();
                return false;
            }
        }
    }
    return true;
}

void   CHsFolder::Clear(void)
{
    if (m_Files == NULL) return;
    for (DWORD i=0; i<m_FolderRec.m_Recnum; ++i)
    {
        if (m_Files[i].m_Type == FILEEC_FOLDER && m_Files[i].m_Free == 0)
        {
            CHsFolder *PFolder = (CHsFolder *)m_Files[i].m_Extra;
            if (PFolder) HDelete(CHsFolder, PFolder, m_Heap);
            m_Files[i].m_Extra = 0;
        }
    }
    HFree(m_Files, m_Heap);
    m_Files = NULL;
    m_FolderRec.m_Recnum = 0;
    m_FolderRec.m_Size = sizeof(SFolderRec);
    UpdateFileRec();
}

bool    CHsFolder::FileExists(const CStr &name) const
{
    CStr beg(m_Heap);
    CStr rem(m_Heap);

    name.Split(beg,rem, "/\\");
    SFileRec *PFile = GetFileRec(beg);
    if (PFile == NULL) return false;
    if (PFile->m_Type == FILEEC_FOLDER)
    {
        if (rem.IsEmpty()) return false;
        CHsFolder *PFolder = (CHsFolder *)PFile->m_Extra;
        return PFolder->FileExists(rem);
    }
    return rem.IsEmpty();
}

//Procedure   CHsFolder.SetFileType(name:string; NType:DWORD);
//Var
//    beginning,remainder:string;
//    PFile:PFileRecEC;
//    PFolder:CHsFolder;
//begin
//    beginning:=GetBeginningStr(name,'/\');
//    remainder:=GetRemainderStr(name,'/\');
//    PFile:=GetFileRec(beginning);
//    if PFile=nil then exit;
//    if PFile.m_Type=FILEEC_FOLDER then
//    begin
//        if remainder='' then
//        begin
//            exit;
//        end;
//        PFolder:=CHsFolder(PFile.m_Extra);
//        PFolder.SetFileType(remainder,NType);
//        exit;
//    end;
//    if remainder<>'' then exit;
//    { ��������� ������ ���� ����� }
//    if NType=FILEEC_COMPRESSED then
//    begin
//        PFile.m_NType:=PFile.m_Type;
//        if PFile.m_Type=FILEEC_COMPRESSED then exit;
//        if PFile.m_Offset=0 then
//        begin
//            PFile.m_Type:=NType;
//            PFile.m_NType:=NType;
//        end else
//        begin
//            PFile.m_NType:=NType;
//        end;
//    end else if NType<>FILEEC_FOLDER then
//    begin
//        PFile.m_NType:=PFile.m_Type;
//        if PFile.m_Type=NType then exit;
//        if PFile.m_Offset=0 then
//        begin
//            PFile.m_Type:=NType;
//            PFile.m_NType:=NType;
//        end else
//        begin
//            PFile.m_NType:=NType;
//        end;
//    end;
//    m_ToUpdate:=true;
//end;

//Procedure   CHsFolder.SetFolderType(name:string; NType:DWORD);
//Var
//    beginning,remainder:string;
//    PFile:PFileRecEC;
//    PFolder:CHsFolder;
//    i:integer;
//begin
//    if name='' then
//    begin
//        for i:=0 to m_FolderRec.m_Recnum-1 do
//        begin
//            PFile:=GetFileRec(i);
//            if PFile.m_Free=0 then
//            begin
//                if PFile.m_Type=FILEEC_FOLDER then
//                begin
//                    PFolder:=CHsFolder(PFile.m_Extra);
//                    PFolder.SetFolderType('',NType);
//                end else
//                begin
//                    SetFileType(PFile.m_Name,NType);
//                end;
//            end;
//        end;
//        exit;
//    end;
//    beginning:=GetBeginningStr(name,'/\');
//    remainder:=GetRemainderStr(name,'/\');
//    PFile:=GetFileRec(beginning);
//    if PFile=nil then exit;
//    if PFile.m_Type=FILEEC_FOLDER then
//    begin
//        PFolder:=CHsFolder(PFile.m_Extra);
//        PFolder.SetFolderType(remainder,NType);
//        exit;
//    end;
//end;

bool    CHsFolder::PathExists(const CStr &name) const
{
    CStr beg(m_Heap);
    CStr rem(m_Heap);

    name.Split(beg,rem, "/\\");
    SFileRec *PFile = GetFileRec(beg);
    if (PFile == NULL) return false;

    if (PFile->m_Type == FILEEC_FOLDER)
    {
        if (rem.IsEmpty()) return true;
        CHsFolder *PFolder = (CHsFolder *)PFile->m_Extra;
        return PFolder->PathExists(rem);
    }
    return rem.IsEmpty();
}

SFileRec *  CHsFolder::GetFileRecEx(const CStr &name) const
{
    CStr beg(m_Heap);
    CStr rem(m_Heap);

    name.Split(beg,rem, "/\\");
    SFileRec *PFile = GetFileRec(beg);
    if (PFile == NULL) return false;
    if (PFile->m_Type == FILEEC_FOLDER)
    {
        if (rem.IsEmpty()) return PFile;
        CHsFolder *PFolder = (CHsFolder *)PFile->m_Extra;
        return PFolder->GetFileRecEx(rem);
    }
    return rem.IsEmpty()?PFile:NULL;
}

//Function    CHsFolder.ReAllocFileRecs(number:integer):boolean;
//Var
//    i:DWORD;
//    last:DWORD;
//    PFile:PFileRecEC;
//begin
//    Result:=false;
//    if m_FolderRec.m_Recnum>=DWORD(number) then exit;
//    last:=m_FolderRec.m_Recnum;
//    m_FolderRec.m_Recnum:=DWORD(number);
//    ReAllocMem(m_Files,m_FolderRec.m_Recnum*m_FolderRec.m_RecSize);
//    for i:=last to number-1 do
//    begin
//        PFile:=GetFileRec(i);
//        PFile.m_Free:=1;
//    end;
//    m_FolderRec.m_Size:=sizeof(TFolderRecEC)+m_FolderRec.m_RecSize*m_FolderRec.m_Recnum;
//    // ������ ��������� ���������� � ����� ���������� � ������������ �����
//    m_ToSave:=true;
//    m_ToUpdate:=false;
//    UpdateFileRec;
//    Result:=true;
//end;

//////////////////////////////////////////////////////////////////////////
/////////////////  ������� ���������� ����� ����������  //////////////////
//////////////////////////////////////////////////////////////////////////

//Function    CHsFolder.CreateFolder(name:string):boolean;
//Var
//    remainder:string;
//    beginning:string;
//    PFile:PFileRecEC;
//    PFolder:CHsFolder;
//begin
//    Result:=false;
//    remainder:=GetRemainderStr(name,'/\');
//    beginning:=GetBeginningStr(name,'/\');
//    PFile:=GetFileRec(beginning);
//    // ���� ���������� � ����� ������ �� ����������, ������� �����
//    if PFile=nil then
//    begin
//        if length(beginning)>=MAX_FILENAME_LENGTH then
//            raise Exception.Create('Too long file name: '+beginning+' > '+
//                                    IntToStr(MAX_FILENAME_LENGTH)+' chars');
//        PFile:=GetFreeRec;
//        if PFile=nil then
//        begin
//            ReAllocFileRecs(m_FolderRec.m_Recnum+10);
//            PFile:=GetFreeRec;
//        end;
//        strcopy(PFile.m_Name,PChar(UpperCase(beginning)));
//        strcopy(PFile.m_RealName,PChar(beginning));
//        PFile.m_Type:=FILEEC_FOLDER;
//        PFile.m_NType:=FILEEC_FOLDER;
//        PFile.m_Free:=0;
//        PFile.m_Date:=0;
//        PFile.m_Offset:=0;
//        PFile.m_Extra:=0;
//        PFolder:=CHsFolder.Create(beginning,self);
//        PFile.m_Extra:=DWORD(PFolder);
//        PFolder.AllocEmptyFolder;
//        PFile.m_Size:=PFolder.GetFolderRec.m_Size;
//        m_ToUpdate:=true;
//        Result:=true;
//        if remainder='' then exit;
//        Result:=PFolder.CreateFolder(remainder);
//        exit;
//    end;
//    // ���� ��� ���������� � ����� ������ ��� ����������
//    if PFile.m_Type<>FILEEC_FOLDER then exit;
//    Result:=true;
//    if remainder='' then exit;
//    PFolder:=CHsFolder(PFile.m_Extra);
//    if PFolder=nil then raise Exception.Create('���������� ���������� ��������� �� ���������� :'+beginning);
//    Result:=PFolder.CreateFolder(remainder);
//end;

//Function    CHsFolder.AddPath(name:string):boolean;
//begin
//    Result:=AddPath(name,FILEEC_BINARY);
//end;
//
//Function    CHsFolder.AddFileEx(name:string;date,size:DWORD;ftype:DWORD):boolean;
//Var
//    remainder:string;
//    beginning:string;
//    PFile:PFileRecEC;
//    PFolder:CHsFolder;
//begin
//    Result:=false;
//    remainder:=GetRemainderStr(name,'/\');
//    beginning:=GetBeginningStr(name,'/\');
//    PFile:=GetFileRec(beginning);
//    // ���� ����� ���� � ����� �� ���������� - ������� �����
//    if PFile=nil then
//    begin
//        if length(beginning)>=MAX_FILENAME_LENGTH then
//             raise Exception.Create('Too long file name: '+beginning+' > '+
//                                     IntToStr(MAX_FILENAME_LENGTH)+' chars');
//        PFile:=GetFreeRec;
//        if PFile=nil then
//        begin
//            ReAllocFileRecs(m_FolderRec.m_Recnum+10);
//            PFile:=GetFreeRec;
//        end;
//        // ��������� �� ������������� �������� ������ ��� �����
//        if remainder='' then
//        begin
//            strcopy(PFile.m_Name,PChar(UpperCase(beginning)));
//            strcopy(PFile.m_RealName,PChar(beginning));
//            PFile.m_Type:=ftype;
//            PFile.m_NType:=ftype;
//            PFile.m_Free:=0;
//            PFile.m_Date:=date;
//            PFile.m_Offset:=0;
//            PFile.m_Extra:=0;
//            PFile.m_Size:=size+4;
//            PFile.m_RealSize:=size;
//            m_ToUpdate:=true;
//            // ���������� ����������
//            UpdateFileRec;
//            Result:=true;
//            exit;
//        end;
//        strcopy(PFile.m_Name,PChar(UpperCase(beginning)));
//        strcopy(PFile.m_RealName,PChar(beginning));
//        PFile.m_Type:=FILEEC_FOLDER;
//        PFile.m_NType:=FILEEC_FOLDER;
//        PFile.m_Free:=0;
//        PFile.m_Date:=0;
//        PFile.m_Offset:=0;
//        PFile.m_Extra:=0;
//        PFolder:=CHsFolder.Create(beginning,self);
//        PFile.m_Extra:=DWORD(PFolder);
//        PFolder.AllocEmptyFolder;
//        PFile.m_Size:=PFolder.GetFolderRec.m_Size;
//        m_ToUpdate:=true;
//        UpdateFileRec;
//        Result:=PFolder.AddFileEx(remainder,date,size,ftype);
//        exit;
//    end;
//    // ���� ��� ���������� � ����� ������ ��� ����������
//    if remainder='' then
//    begin
//        PFile.m_Offset:=0;
//        PFile.m_Type:=ftype;
//        PFile.m_NType:=ftype;
//        Result:=true;
//        exit;
//    end;
//    if PFile.m_Type<>FILEEC_FOLDER then exit;
//    PFolder:=CHsFolder(PFile.m_Extra);
//    if PFolder=nil then raise Exception.Create('���������� ���������� ��������� �� ���������� :'+beginning);
//    Result:=PFolder.AddFileEx(remainder,date,size,ftype);
//end;
//
//Function    CHsFolder.DeleteFile(name:string):boolean;
//Var
//    beginning,remainder:string;
//    PFile:PFileRecEC;
//    PFolder:CHsFolder;
//begin
//    Result:=false;
//    beginning:=GetBeginningStr(name,'/\');
//    remainder:=GetRemainderStr(name,'/\');
//    PFile:=GetFileRec(beginning);
//    if PFile=nil then exit;
//    if PFile.m_Type=FILEEC_FOLDER then
//    begin
//        if remainder='' then
//        begin
//            PFolder:=CHsFolder(PFile.m_Extra);
//            PFolder.Free;
//            PFile.m_Free:=1;
//            m_ToUpdate:=true;
//            Result:=true;
//            exit;
//        end;
//        PFolder:=CHsFolder(PFile.m_Extra);
//        Result:=PFolder.DeleteFile(remainder);
//        exit;
//    end;
//    if remainder<>'' then exit;
//    PFile.m_Free:=1;
//    m_ToUpdate:=true;
//    Result:=true;
//end;
//
//Function    CHsFolder.AddFile(name:string):boolean;
//begin
//    Result:=AddFile(name,FILEEC_BINARY);
//end;
//
//Function    CHsFolder.AddPath(name:string;ftype:DWORD):boolean;
//Var
//    S:TSearchRec;
//    Err:Integer;
//begin
//    if name<>'' then
//    begin
//        if (name[length(name)]<>'\') and (name[length(name)]<>'/') then
//            name:=name+'\';
//    end;
//    Err:=FindFirst(name+'*.*',faAnyFile,S);
//    while Err=0 do
//    begin
//        //**************** ���������� ******************
//        if (S.Name<>'.') and (S.Name<>'..') then
//        begin
//            if ((S.Attr and faDirectory)=faDirectory) then
//            begin
//                if not AddPath(name+S.Name,ftype) then raise Exception.Create('������ ���������� ���� � �������� ����: '+name+S.Name);
//            end else
//            begin
//                if not AddFile(name+S.Name,ftype) then raise Exception.Create('������ ���������� ����� � �������� ����: '+name+S.Name);
//            end;
//        end;
//        Err:=FindNext(S);
//    end;
//    FindClose(S);
//    Result:=true;
//end;
//
//Function    CHsFolder.AddFile(name:string;ftype:DWORD):boolean;
//Var
//    Handle:DWORD;
//    date:DWORD;
//    size:DWORD;
//begin
//    // ��������� ������ ����� � �.�.
//    Result:=false;
//    date:=0;
//    Handle:=CreateFile(PChar(name),
//                       GENERIC_READ,
//                       FILE_SHARE_READ,nil,
//                       OPEN_EXISTING,
//                       FILE_ATTRIBUTE_NORMAL,
//                       0);
//    if Handle=INVALID_HANDLE_VALUE then exit;
//    size:=SetFilePointer(Handle,0,nil,FILE_END);
//    CloseHandle(Handle);
//    if size=$FFFFFFFF then exit;
//    result:=AddFileEx(GetLocalPath(name),date,size,ftype);
//end;

CStr    CHsFolder::GetFullPath(const CStr &name)
{
    if (m_Parent)
    {
        return m_Parent->GetFullPath(m_RealName+"\\"+name);
    } else
    {
        return name;
    }
}

DWORD   CHsFolder::CompressedFileSize(DWORD Handle, const CStr &filename)
{
    CStr beg(m_Heap);
    CStr rem(m_Heap);

    filename.Split(beg,rem, "/\\");
    SFileRec *PFile = GetFileRec(beg);
    if (PFile == NULL) return 0xFFFFFFFF;

    //DWORD size,temp;
    //BYTE *soubuf,compbuf; // ������� ����� � ����� ��� ������ ������
    //DWORD compsize;  // ������ ������� ����� ������ <65536
    //DWORD totalsize; // ����� ������ ������ ������ + ����� (4 �����)

    if (PFile->m_Type == FILEEC_FOLDER)
    {
        if (rem.IsEmpty()) return 0xFFFFFFFF;
        CHsFolder *PFolder = (CHsFolder *)PFile->m_Extra;
        return PFolder->CompressedFileSize(Handle,rem);
    }
    if (!rem.IsEmpty()) return 0xFFFFFFFF;

    // ����� ���� - ������ ���������� ��������� ������ ����� �����
    // ������ 1 - ���� ��� �������� � ����, � ��� �������� �� ���� �������� ������ ������
    if (PFile->m_Offset != 0 && PFile->m_Type == FILEEC_COMPRESSED)
    {
        return PFile->m_Size-4;
    }

    ERROR_S(L"Error getting compressed file size");

    //// � ����� �� ���� ��������� ������� ���� ������� �������� � ������
    //if (PFile->m_Offset == 0)
    //{
    //    // ������ 2 - ���� ��� �� ��� �������� � �����, � ������ ����� �� �����
    //    Handle:=CreateFile(PChar(GetFullPath(filename)),
    //                      GENERIC_READ,
    //                      FILE_SHARE_READ,nil,
    //                      OPEN_EXISTING,
    //                      FILE_ATTRIBUTE_NORMAL,
    //                      0);
    //    if Handle=INVALID_HANDLE_VALUE then exit;
    //    size:=SetFilePointer(Handle,0,nil,FILE_END);
    //    if size=$FFFFFFFF then raise Exception.Create('Calc size error :'+GetFullPath(filename));
    //    temp:=SetFilePointer(Handle,0,nil,FILE_BEGIN);
    //    if temp=$FFFFFFFF then raise Exception.Create('Calc size error :'+GetFullPath(filename));
    //end else
    //begin
    //    // ������ 3 - ���� ��� ��������, �� ��� �������� ���� ��������
    //    temp:=SetFilePointer(Handle,PFile.m_Offset,nil,FILE_BEGIN);
    //    if temp=$FFFFFFFF then raise Exception.Create('Calc size error :'+GetFullPath(filename));
    //    if not Windows.ReadFile(Handle,size,4,temp,nil) then
    //        raise Exception.Create('Calc size error :'+GetFullPath(filename));
    //end;
    //// ������� ����
    //soubuf:=AllocMem(65536);
    //compbuf:=AllocMem(72112);

    //totalsize:=0;
    //while size>65536 do
    //begin
    //    if not ReadFile(Handle,soubuf^,65536,temp,nil) then raise Exception.Create('Calc error:'+GetFullPath(filename));
    //    {$ifndef SUPPORT_COMPRESSION_OFF}
    //    compsize:=OKGF_ZLib_Compress2(compbuf,72112,soubuf,65536);
    //    {$else}
    //    {$endif}
    //    if (compsize=0) then raise Exception.Create('Calc error:'+GetFullPath(filename));
    //    totalsize:=totalsize+compsize+4;
    //    size:=size-65536;
    //end;
    //if not ReadFile(Handle,soubuf^,size,temp,nil) then raise Exception.Create('Calc error:'+GetFullPath(filename));
    //{$ifndef SUPPORT_COMPRESSION_OFF}
    //compsize:=OKGF_ZLib_Compress2(compbuf,72112,soubuf,size);
    //{$else}
    //{$endif}
    //if (compsize=0) then raise Exception.Create('Calc error:'+GetFullPath(filename));
    //totalsize:=totalsize+compsize+4;
    //FreeMem(soubuf);
    //FreeMem(compbuf);
    //if PFile.m_Offset=0 then
    //begin
    //    CloseHandle(Handle);
    //end;
    //Result:=totalsize;
}

DWORD   CHsFolder::DecompressedFileSize(DWORD Handle, const CStr &filename)
{
    CStr beg(m_Heap);
    CStr rem(m_Heap);

    filename.Split(beg,rem, "/\\");
    SFileRec *PFile = GetFileRec(beg);
    if (PFile == NULL) return 0xFFFFFFFF;

    //size:DWORD;

    if (PFile->m_Type == FILEEC_FOLDER)
    {
        if (rem.IsEmpty()) return 0xFFFFFFFF;
        CHsFolder *PFolder = (CHsFolder *)PFile->m_Extra;
        return PFolder->DecompressedFileSize(Handle,rem);
    }
    if (!rem.IsEmpty()) return 0xFFFFFFFF;

    // ����� ���� - ������ ���������� ��������� ������ ����� �����
    // ������ 1 - ���� ��� ��������

    if (PFile->m_Offset!=0)
    {
        return PFile->m_RealSize;
    }

    ERROR_S(L"Error getting decompressed file size");

//    // ������ 2 - ���� ��� �� �������� - ���� ��� �� �����
//    Handle:=CreateFile(PChar(GetFullPath(filename)),
//                      GENERIC_READ,
//                      FILE_SHARE_READ,nil,
//                      OPEN_EXISTING,
//                      FILE_ATTRIBUTE_NORMAL,
//                      0);
//    if Handle=INVALID_HANDLE_VALUE then exit;
//    size:=SetFilePointer(Handle,0,nil,FILE_END);
//    if size=$FFFFFFFF then raise Exception.Create('Calc size error :'+GetFullPath(filename));
//    CloseHandle(Handle);
//    Result:=size;
}

int   CHsFolder::GetLocalSize(void)
{
    int size = 0;

    for (DWORD i=0; i<m_FolderRec.m_Recnum; ++i)
    {
        if (m_Files[i].m_Free == 0)
        {
            size += m_Files[i].m_Size;
        }
    }
    return size;
}

void  CHsFolder::UpdateFileRec(void)
{
    if (m_Parent)
    {
        SFileRec *PFile = m_Parent->GetFileRec(m_RealName);
        if (PFile == NULL) ERROR_S(L"Packet file system error!");
        if (PFile->m_Type != FILEEC_FOLDER) ERROR_S(L"Name file/folder conflict");
        PFile->m_Size = m_FolderRec.m_Size;
        PFile->m_Offset = 0;
    }
}

//******** ������� ���������� ����� �������� � ����� **************//
//Function    CHsFolder.PackFile(Handle:DWORD;name:string;Offset:DWORD):DWORD;
//Var
//    H:DWORD;
//    size:DWORD;
//    temp:DWORD;
//    buf:Pointer;
//begin
//    Result:=Offset;
//    H:=CreateFile(PChar(name),
//                  GENERIC_READ,
//                  FILE_SHARE_READ,nil,
//                  OPEN_EXISTING,
//                  FILE_ATTRIBUTE_NORMAL,
//                  0);
//    if H=INVALID_HANDLE_VALUE then exit;
//    size:=SetFilePointer(H,0,nil,FILE_END);
//    if size=$FFFFFFFF then raise Exception.Create('������ �������� ����� :'+name);
//    SetFilePointer(H,0,nil,FILE_BEGIN);
//    temp:=SetFilePointer(Handle,Offset,nil,FILE_BEGIN);
//    if temp=$FFFFFFFF then raise Exception.Create('������ �������� ����� :'+name);
//    temp:=0;
//    WriteFile(Handle,size,4,temp,nil);
//    buf:=AllocMem(65536);
//    Result:=Offset+size+4;
//    while size>65536 do
//    begin
//        ReadFile(H,buf^,65536,temp,nil);
//        WriteFile(Handle,buf^,65536,temp,nil);
//        size:=size-65536;
//    end;
//    ReadFile(H,buf^,size,temp,nil);
//    WriteFile(Handle,buf^,size,temp,nil);
//    FreeMem(buf);
//    size:=0;
//    CloseHandle(H);
//end;
//
//Function    CHsFolder.PackAndCompressFile(Handle:DWORD;name:string;Offset:DWORD):DWORD;
//Var
//    H:DWORD;
//    size:DWORD;      // ������ ���������� ����� ������
//    temp:DWORD;
//    soubuf,compbuf:Pointer; // ������� ����� � ����� ��� ������ ������
//    compsize:DWORD;  // ������ ������� ����� ������ <65536
//    totalsize:DWORD; // ����� ������ ������ ������ + ����� (4 �����)
//begin
//    {$ifdef SUPPORT_COMPRESSION_OFF}
//    raise Exception.Create('Compression is not supported');
//    {$endif}
//    Result:=Offset;
//    H:=CreateFile(PChar(name),
//                  GENERIC_READ,
//                  FILE_SHARE_READ,nil,
//                  OPEN_EXISTING,
//                  FILE_ATTRIBUTE_NORMAL,
//                  0);
//    if H=INVALID_HANDLE_VALUE then exit;
//    size:=SetFilePointer(H,0,nil,FILE_END);
//    if size=$FFFFFFFF then raise Exception.Create('������ �������� ����� :'+name);
//    SetFilePointer(H,0,nil,FILE_BEGIN);
//    temp:=SetFilePointer(Handle,Offset,nil,FILE_BEGIN);
//    if temp=$FFFFFFFF then raise Exception.Create('������ �������� ����� :'+name);
//
//    //************* ����� �������� ������ ����� ***************//
//    soubuf:=AllocMem(65536);
//    compbuf:=AllocMem(72112);
//
//    totalsize:=4;
//    if not WriteFile(Handle, totalsize, 4, temp, nil) then raise Exception.Create('������ ������ � �������� ���� �����:'+name);
//
//    while size>65536 do
//    begin
//        if not ReadFile(H,soubuf^,65536,temp,nil) then raise Exception.Create('������ ������ ������ ��� ������ �����:'+name);
//        {$ifndef SUPPORT_COMPRESSION_OFF}
//        compsize:=OKGF_ZLib_Compress2(compbuf,72112,soubuf,65536);
//        {$else}
//        {$endif}
//        if (compsize=0) then raise Exception.Create('������ ������ �����:'+name);
//        temp:=0;
//        totalsize:=totalsize+compsize+4;
//        if not WriteFile(Handle,compsize,4,temp,nil) then raise Exception.Create('������ ������ � �������� ���� �����:'+name);
//        if not WriteFile(Handle,compbuf^,compsize,temp,nil) then raise Exception.Create('������ ������ � �������� ���� �����:'+name);
//        size:=size-65536;
//    end;
//    if not ReadFile(H,soubuf^,size,temp,nil) then raise Exception.Create('������ ������ ������ ��� ������ �����:'+name);
//    {$ifndef SUPPORT_COMPRESSION_OFF}
//    compsize:=OKGF_ZLib_Compress2(compbuf,72112,soubuf,size);
//    {$else}
//    {$endif}
//    if (compsize=0) then raise Exception.Create('������ ������ �����:'+name);
//    temp:=0;
//    totalsize:=totalsize+compsize+4;
//    if not WriteFile(Handle,compsize,4,temp,nil) then raise Exception.Create('������ ������ � �������� ���� �����:'+name);
//    if not WriteFile(Handle,compbuf^,compsize,temp,nil) then raise Exception.Create('������ ������ � �������� ���� �����:'+name);
////    size:=0;
//
//    if SetFilePointer(Handle,Offset,nil,FILE_BEGIN)<>Offset then raise Exception.Create('������ ���������������� � �������� �����');
//    WriteFile(Handle,totalsize,4,temp,nil);
//    Result:=Offset+totalsize;
//    FreeMem(soubuf);
//    FreeMem(compbuf);
//    CloseHandle(H);
//
//    // ������ �����
//end;
//
//Function    CHsFolder.DecodeFile(Handle:DWORD;PFile:PFileRecEC;Offset:DWORD):DWORD;
//Var
//    soubuf, ucompbuf:Pointer;
//    soubufsize, ucompbufsize:DWORD;
//    temp:DWORD;
//    soupos, despos:DWORD;
//    size:DWORD;
//begin
//    {$ifdef SUPPORT_COMPRESSION_OFF}
//    raise Exception.Create('Compression is not supported');
//    {$endif}
//    soubuf:=AllocMem(72112);
//    ucompbuf:=AllocMem(65536);
//    soupos:=PFile.m_Offset;
//    despos:=Offset;
//    { ������ ������� ������� ����� }
//    temp:=SetFilePointer(Handle,soupos,nil,FILE_BEGIN);
//    if temp=$FFFFFFFF then raise Exception.Create('Decode file error :'+PFile.m_RealName);
//    if not ReadFile(Handle,size,4,temp,nil) then raise Exception.Create('Decode file error :'+PFile.m_RealName);
//    soupos:=soupos+4;
//    { ������ ������� ����� }
//    temp:=SetFilePointer(Handle,despos,nil,FILE_BEGIN);
//    if temp=$FFFFFFFF then raise Exception.Create('Decode file error :'+PFile.m_RealName);
//    if not WriteFile(Handle,PFile.m_RealSize,4,temp,nil) then raise Exception.Create('Decode file error :'+PFile.m_RealName);
//    despos:=despos+4;
//    size:=size-4;
//    { �������� ����� }
//    while size>0 do
//    begin
//        temp:=SetFilePointer(Handle,soupos,nil,FILE_BEGIN);
//        if temp=$FFFFFFFF then raise Exception.Create('Decode file error :'+PFile.m_RealName);
//        if not ReadFile(Handle, soubufsize, 4, temp, nil) then raise Exception.Create('Decode file error :'+PFile.m_RealName);
//        if not ReadFile(Handle,soubuf^,soubufsize, temp, nil) then raise Exception.Create('Decode file error :'+PFile.m_RealName);
//        size:=size-4-soubufsize;
//        soupos:=soupos+4+soubufsize;
//        {$ifndef SUPPORT_COMPRESSION_OFF}
//        ucompbufsize:=OKGF_ZLib_uncompress2(ucompbuf,65536,soubuf,72112);
//        {$else}
//        ucompbufsize:=0;
//        {$endif}
//        if (soubufsize<>0) and (ucompbufsize=0) then raise Exception.Create('Decode file error :'+PFile.m_RealName);
//        temp:=SetFilePointer(Handle,despos,nil,FILE_BEGIN);
//        if not WriteFile(Handle,ucompbuf^,ucompbufsize,temp,nil) then raise Exception.Create('Decode file error :'+PFile.m_RealName);
//        despos:=despos+ucompbufsize;
//    end;
//    FreeMem(soubuf);
//    FreeMem(ucompbuf);
//    Result:=despos;
//end;
//
//Function    CHsFolder.EncodeFile(Handle:DWORD;PFile:PFileRecEC;Offset:DWORD):DWORD;
//Var
//    soubuf, desbuf:Pointer;
//    dessize: DWORD;
//    temp:DWORD;
//    soupos, despos:DWORD;
//    size, total:DWORD;
//begin
//    {$ifdef SUPPORT_COMPRESSION_OFF}
//    raise Exception.Create('Compression is not supported');
//    {$endif}
//    soubuf:=AllocMem(65536);
//    desbuf:=AllocMem(72112);
//    soupos:=PFile.m_Offset;
//    despos:=Offset;
//    size:=PFile.m_RealSize;
//    total:=4;
//    if not WriteFile(Handle,total,4,temp,nil) then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//    despos:=despos+4;
//{    temp:=SetFilePointer(Handle,soupos,nil,FILE_BEGIN);
//    if temp=$FFFFFFFF then raise Exception.Create('Encode file error :'+PFile.m_RealName);}
//    soupos:=soupos+4;
//    while size>65536 do
//    begin
//        temp:=SetFilePointer(Handle,soupos,nil,FILE_BEGIN);
//        if temp=$FFFFFFFF then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//        if not ReadFile(Handle,soubuf^,65536,temp,nil) then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//        {$ifndef SUPPORT_COMPRESSION_OFF}
//        dessize:=OKGF_ZLib_Compress2(desbuf,72112,soubuf,65536);
//        {$else}
//        dessize:=0;
//        {$endif}
//        if dessize=0 then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//        SetFilePointer(Handle,despos,nil,FILE_BEGIN);
//        if not WriteFile(Handle,dessize,4,temp,nil) then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//        if not WriteFile(Handle,desbuf^,dessize,temp,nil) then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//        soupos:=soupos+65536;
//        despos:=despos+4+dessize;
//        size:=size-65536;
//        total:=total+4+dessize;
//    end;
//    temp:=SetFilePointer(Handle,soupos,nil,FILE_BEGIN);
//    if temp=$FFFFFFFF then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//    if not ReadFile(Handle,soubuf^,size,temp,nil) then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//    {$ifndef SUPPORT_COMPRESSION_OFF}
//    dessize:=OKGF_ZLib_Compress2(desbuf,72112,soubuf,size);
//    {$else}
//    dessize:=0;
//    {$endif}
//    if dessize=0 then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//    SetFilePointer(Handle,despos,nil,FILE_BEGIN);
//    if not WriteFile(Handle,dessize,4,temp,nil) then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//    if not WriteFile(Handle,desbuf^,dessize,temp,nil) then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//    despos:=despos+4+dessize;
//    total:=total+4+dessize;
//    temp:=SetFilePointer(Handle,Offset,nil,FILE_BEGIN);
//    if temp=$FFFFFFFF then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//    if not WriteFile(Handle,total,4,temp,nil) then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//
//    temp:=SetFilePointer(Handle,despos,nil,FILE_BEGIN);
//    if temp=$FFFFFFFF then raise Exception.Create('Encode file error :'+PFile.m_RealName);
//
//    FreeMem(soubuf);
//    FreeMem(desbuf);
//
//    Result:=despos;
//end;
////******** ������� ���������� ����� �������� � ����� **************//
//Function    CHsFolder.Pack(Handle:DWORD;Offset:DWORD;PInt:PInteger;PStr:PString):DWORD;
//Var
//    Temp:DWORD;
//    i:Integer;
//    PFile:PFileRecEC;
//    PFolder:CHsFolder;
//    nOffset:DWORD;
//    FolderOffset:DWORD;
//begin
//    // ���������� ����� ��������� ���������� � �������� �����
//    FolderOffset:=0;
//    if (not m_ToSave) then
//    begin
//        if m_Parent<>nil then
//        begin
//            PFile:=m_Parent.GetFileRec(m_RealName);
//            if PFile=nil then raise Exception.Create('������ � �������� ������� ��������� �����');
//            FolderOffset:=PFile.m_Offset;
//        end else if not m_ToUpdate then FolderOffset:=$FFFFFFFF;
//    end;
//    if FolderOffset=0 then
//    begin
//        FolderOffset:=Offset;
//        Offset:=Offset+m_FolderRec.m_RecSize*m_FolderRec.m_RecNum+sizeof(TFolderRecEC);
//    end;
//
//    if (m_ToSave) or (m_ToUpdate) then
//    begin
//        Temp:=SetFilePointer(Handle,FolderOffset,nil,FILE_BEGIN);
//        if Temp=$FFFFFFFF then raise Exception.Create('������ �������� ���������� :'+GetFullPath(''));
//        WriteFile(Handle,m_FolderRec,sizeof(TFolderRecEC),temp,nil);
//        WriteFile(Handle,m_Files^,m_FolderRec.m_RecSize*m_FolderRec.m_RecNum,
//                  temp,nil);
//    end;
//
//    { ����������� ������ ������ �������� ��������� ����� }
//    for i:=0 to m_FolderRec.m_Recnum-1 do
//    begin
//        PFile:=GetFileRec(i);
//        if (PFile.m_Free=0) and (PFile.m_Offset=0) then
//        begin
//            if PFile.m_Type=FILEEC_FOLDER then
//            begin
//                PFolder:=CHsFolder(PFile.m_Extra);
//                if PFolder=nil then
//                    raise Exception.Create('������������ ������');
//                nOffset:=PFolder.Pack(Handle,Offset,PInt,PStr);
//                PFile.m_Offset:=Offset;
//                Offset:=nOffset;
//            end else
//            begin
//                if PInt<>nil then Inc(PInt^);
//                if PStr<>nil then PStr^:=GetFullPath(PFile.m_RealName+'');
//                if PFile.m_Type=FILEEC_COMPRESSED then
//                begin
//                    nOffset:=PackAndCompressFile(Handle,GetFullPath(PFile.m_RealName+''),Offset);
//                    PFile.m_Size:=nOffset-Offset;
//                    PFile.m_Offset:=Offset;
//                    Offset:=nOffset;
//                end else
//                begin
//                    nOffset:=PackFile(Handle,GetFullPath(PFile.m_RealName+''),Offset);
//                    PFile.m_Size:=nOffset-Offset;
//                    PFile.m_Offset:=Offset;
//                    Offset:=nOffset;
//                end;
//            end;
//        end else if (PFile.m_Type<>PFile.m_NType) and (PFile.m_Free=0) then
//        begin
//            { �������� / ������ ���������� ������ ������ }
//            if PInt<>nil then Inc(PInt^);
//            if PStr<>nil then PStr^:=GetFullPath(PFile.m_RealName+'');
//            if PFile.m_NType=FILEEC_COMPRESSED then
//            begin
////                PFile.m_RealSize:=PFile.m_Size;
//                nOffset:=EncodeFile(Handle,PFile,Offset);
////                nOffset:=PackAndCompressFile(Handle,GetFullPath(PFile.m_RealName+''),Offset);
//                PFile.m_Size:=nOffset-Offset;
//                PFile.m_Offset:=Offset;
//                PFile.m_Type:=PFile.m_NType;
//                Offset:=nOffset;
//            end else
//            begin
//                nOffset:=DecodeFile(Handle,PFile,Offset);
//                PFile.m_Size:=nOffset-Offset;
//                PFile.m_Offset:=Offset;
//                PFile.m_Type:=PFile.m_NType;
//                Offset:=nOffset;
//            end;
//        end else if (PFile.m_Free=0) then
//        begin
//            if PFile.m_Type=FILEEC_FOLDER then
//            begin
//                PFolder:=CHsFolder(PFile.m_Extra);
//                if PFolder=nil then
//                    raise Exception.Create('������������ ������');
//                nOffset:=PFolder.Pack(Handle,Offset,PInt,PStr);
//                Offset:=nOffset;
//            end else
//            begin
//                if PInt<>nil then Inc(PInt^);
//                if PStr<>nil then PStr^:=GetFullPath(PFile.m_RealName+'');
//            end;
//        end;
//    end;
//
//    if (m_ToSave) or (m_ToUpdate) then
//    begin
//        Temp:=SetFilePointer(Handle,FolderOffset,nil,FILE_BEGIN);
//        if Temp=$FFFFFFFF then raise Exception.Create('������ �������� ���������� :'+GetFullPath(''));
//        WriteFile(Handle,m_FolderRec,sizeof(TFolderRecEC),temp,nil);
//        WriteFile(Handle,m_Files^,m_FolderRec.m_RecSize*m_FolderRec.m_RecNum,
//                  temp,nil);
//    end;
//    m_ToUpdate:=false;
//    m_ToSave:=false;
//    Result:=Offset;
//end;
//
//Procedure   CHsFolder.CompressFolder;
//Var
//    i:Integer;
//    count:integer;
//    PFile:PFileRecEC;
//    PDestFile:PFileRecEC;
//    Files:Pointer;
//begin
//    // ******** ���� ��� ������������ ������ � ���������� ********
//    count:=0;
//    for i:=0 to m_FolderRec.m_Recnum-1 do
//    begin
//        PFile:=GetFileRec(i);
//        if PFile.m_Free=0 then inc(count);
//    end;
//    // ******** ������������ ����� � ����� ������ ****************
//    Files:=AllocMem(DWORD(count)*m_FolderRec.m_RecSize);
//    count:=0;
//    for i:=0 to m_FolderRec.m_Recnum-1 do
//    begin
//        PFile:=GetFileRec(i);
//        if PFile.m_Free=0 then
//        begin
//            PDestFile:=PAdd(Files,count*sizeof(TFileRecEC));
//            PDestFile^:=PFile^;
//            inc(count);
//        end;
//    end;
//    // ******** ������ �������������� ��������� ���������� *******
//    FreeMem(m_Files);
//    m_Files:=Files;
//    m_FolderRec.m_Recnum:=count;
//    m_FolderRec.m_Size:=GetFolderSize;
//    // ������ ��������� ���������� � ����� ���������� � ������������ �����
//    UpdateFileRec;
//end;
//
//Function    CHsFolder.CompressFile(SouHandle,Handle:DWORD;name:string;Offset:DWORD):DWORD;
//Var
//    size:DWORD;
//    temp:DWORD;
//    desptr, souptr:DWORD;
//    buf:Pointer;
//    PFile:PFileRecEC;
//begin
//    PFile:=GetFileRec(name);
//    if SouHandle=$FFFFFFFF then raise Exception.Create('�������� ���� ������ ���� ������ ��� �������� ������');
//    if PFile.m_Offset=0 then raise Exception.Create('������ ������� ����������� �������� ����');
//    souptr:=PFile.m_Offset;
//    desptr:=Offset;
//    temp:=0;
//    size:=PFile.m_Size;
//    buf:=AllocMem(65536);
//    while size>65536 do
//    begin
//        SetFilePointer(SouHandle,souptr,nil,FILE_BEGIN);
//        ReadFile(SouHandle,buf^,65536,temp,nil);
//        SetFilePointer(Handle,desptr,nil,FILE_BEGIN);
//        WriteFile(Handle,buf^,65536,temp,nil);
//        souptr:=souptr+65536;
//        desptr:=desptr+65536;
//        size:=size-65536;
//    end;
//    SetFilePointer(SouHandle,souptr,nil,FILE_BEGIN);
//    ReadFile(SouHandle,buf^,size,temp,nil);
//    SetFilePointer(Handle,desptr,nil,FILE_BEGIN);
//    WriteFile(Handle,buf^,size,temp,nil);
//    desptr:=desptr+size;
//    FreeMem(buf);
//    Result:=desptr;
//end;
//
//Function    CHsFolder.Compress(SouHandle,Handle:DWORD;Offset:DWORD;PInt:PInteger;PStr:PString):DWORD;
//Var
//    Temp:DWORD;
//    i:Integer;
//    PFile:PFileRecEC;
//    PFolder:CHsFolder;
//    nOffset:DWORD;
//    StartOffset:DWORD;
//begin
//    CompressFolder;
//    StartOffset:=Offset;
//    Temp:=SetFilePointer(Handle,StartOffset,nil,FILE_BEGIN);
//    if Temp=$FFFFFFFF then raise Exception.Create('������ �������� ���������� :'+GetFullPath(''));
//    WriteFile(Handle,m_FolderRec,sizeof(TFolderRecEC),temp,nil);
//    WriteFile(Handle,m_Files^,m_FolderRec.m_RecSize*m_FolderRec.m_RecNum,
//              temp,nil);
//    Offset:=Offset+m_FolderRec.m_RecSize*m_FolderRec.m_RecNum+sizeof(TFolderRecEC);
//
//    for i:=0 to m_FolderRec.m_Recnum-1 do
//    begin
//        PFile:=GetFileRec(i);
//        if (PFile.m_Free=0) then
//        begin
//            if PFile.m_Type=FILEEC_FOLDER then
//            begin
//                PFolder:=CHsFolder(PFile.m_Extra);
//                if PFolder=nil then
//                    raise Exception.Create('������������ ������ :'+PFile.m_Name);
//                nOffset:=PFolder.Compress(SouHandle,Handle,Offset,PInt,PStr);
//                PFile.m_Offset:=Offset;
//                Offset:=nOffset;
//            end else
//            begin
//                if PInt<>nil then Inc(PInt^);
//                if PStr<>nil then PStr^:=GetFullPath(PFile.m_RealName+'');
//                nOffset:=CompressFile(SouHandle,Handle,PFile.m_Name+'',Offset);
//                PFile.m_Size:=nOffset-Offset;
//                PFile.m_Offset:=Offset;
//                Offset:=nOffset;
//            end;
//        end;
//    end;
//
//    Temp:=SetFilePointer(Handle,StartOffset,nil,FILE_BEGIN);
//    if Temp=$FFFFFFFF then raise Exception.Create('������ �������� ���������� :'+GetFullPath(''));
//    WriteFile(Handle,m_FolderRec,sizeof(TFolderRecEC),temp,nil);
//    WriteFile(Handle,m_Files^,m_FolderRec.m_RecSize*m_FolderRec.m_RecNum,
//              temp,nil);
//    Result:=Offset;
//end;
//
//Function    CHsFolder.UnpackFile(SouHandle:DWORD;name:string):boolean;
//Var
//    size:DWORD;
//    temp:DWORD;
//    desptr, souptr:DWORD;
//    buf:Pointer;
//    PFile:PFileRecEC;
//    Handle:DWORD;
//    path:string;
//begin
//    Result:=false;
//    PFile:=GetFileRec(name);
//    if SouHandle=$FFFFFFFF then raise Exception.Create('�������� ���� ������ ���� ������ ��� �������� ����������');
//    if PFile.m_Offset=0 then raise Exception.Create('������ ������������� ����������� �������� ����');
//    path:=GetFullPath(name);
//    Handle:=CreateFile(PChar(path),
//                  GENERIC_WRITE,
//                  0,nil,
//                  OPEN_ALWAYS,
//                  FILE_ATTRIBUTE_NORMAL,
//                  0);
//    if Handle=INVALID_HANDLE_VALUE then exit;
//    souptr:=PFile.m_Offset+4;
//    desptr:=0;
//    temp:=0;
//    size:=PFile.m_Size-4;
//    buf:=AllocMem(65536);
//    while size>65536 do
//    begin
//        SetFilePointer(SouHandle,souptr,nil,FILE_BEGIN);
//        ReadFile(SouHandle,buf^,65536,temp,nil);
//        SetFilePointer(Handle,desptr,nil,FILE_BEGIN);
//        WriteFile(Handle,buf^,65536,temp,nil);
//        souptr:=souptr+65536;
//        desptr:=desptr+65536;
//        size:=size-65536;
//    end;
//    SetFilePointer(SouHandle,souptr,nil,FILE_BEGIN);
//    ReadFile(SouHandle,buf^,size,temp,nil);
//    SetFilePointer(Handle,desptr,nil,FILE_BEGIN);
//    WriteFile(Handle,buf^,size,temp,nil);
//    FreeMem(buf);
//    CloseHandle(Handle);
//    Result:=true;
//end;
//
//Function    CHsFolder.UnpackCompressedFile(SouHandle:DWORD;name:string):boolean;
//Var
//    temp:DWORD;
//
//    soubuf,ucompbuf:pointer;
//    size,blocksize,ucompblocksize,ucompsize:DWORD;
//
//    PFile:PFileRecEC;
//    Handle:DWORD;
//    path:string;
//begin
////    exit;
//    {$ifdef SUPPORT_COMPRESSION_OFF}
//    raise Exception.Create('Compression is not supported');
//    {$endif}
//    Result:=false;
//    PFile:=GetFileRec(name);
//    if PFile=nil then raise Exception.Create('������ � �������� ������� ��������� �����');
//    if SouHandle=$FFFFFFFF then raise Exception.Create('�������� ���� ������ ���� ������ ��� �������� ����������');
//    if PFile.m_Offset=0 then raise Exception.Create('������ ������������� ����������� �������� ����');
//    path:=GetFullPath(name);
//    Handle:=CreateFile(PChar(path),
//                  GENERIC_WRITE,
//                  0,nil,
//                  OPEN_ALWAYS,
//                  FILE_ATTRIBUTE_NORMAL,
//                  0);
//    if Handle=INVALID_HANDLE_VALUE then exit;
//
//    //********* ����� �������� ��������� ************/
//    SetFilePointer(SouHandle,PFile.m_Offset+4,nil,FILE_BEGIN);
//    size:=PFile.m_Size-4;
//    ucompsize:=PFile.m_RealSize;
//    soubuf:=AllocMem(72112);
//    ucompbuf:=AllocMem(65536);
//    while size<>0 do
//    begin
//        ReadFile(SouHandle,blocksize,4,temp,nil);
//        ReadFile(SouHandle,soubuf^,blocksize,temp,nil);
//        {$ifndef SUPPORT_COMPRESSION_OFF}
//        ucompblocksize:=OKGF_ZLib_UnCompress2(ucompbuf,65536,soubuf,blocksize);
//        {$else}
//        {$endif}
//        if (ucompsize<>0) and (ucompblocksize=0) then raise Exception.Create('������ ���������� ����� :'+path);
//        WriteFile(Handle,ucompbuf^,ucompblocksize,temp,nil);
//        size:=size-4-blocksize;
//        ucompsize:=ucompsize-ucompblocksize;
//    end;
//    FreeMem(soubuf);
//    FreeMem(ucompbuf);
//    CloseHandle(Handle);
//    if ucompsize<>0 then raise Exception.Create('������ : �������� ������ ��������� ����� :'+path);
//    Result:=true;
//
//    //********* ������ �������� ��������� *************/
//
//{    SetFilePointer(SouHandle,PFile.m_Offset+4,nil,FILE_BEGIN);
//    size:=PFile.m_Size-4;
//    ucompsize:=PFile.m_RealSize;
//    soubuf:=AllocMem(size);
//    ucompbuf:=AllocMem(ucompsize);
//
//    ReadFile(SouHandle,soubuf^,size,temp,nil);
//    ucompsize:=OKGF_ZLib_UnCompress2(ucompbuf,ucompsize,soubuf,size);
//    if ucompsize=0 then raise Exception.Create('������ ���������� ����� :'+path);
//    WriteFile(Handle,ucompbuf^,ucompsize,temp,nil);
//    ///////////////////////// ??????????????????????????????
//    FreeMem(soubuf);
//    FreeMem(ucompbuf);
//    CloseHandle(Handle);
//    Result:=true;}
//end;
//
//Function    CHsFolder.UnpackFolder(name:string):boolean;
//Var
//    path:string;
//begin
//    path:=GetFullPath(name);
//    CreateDirectory(PChar(path),nil);
//    Result:=true;
//end;
//
//Function    CHsFolder.Unpack(SouHandle:DWORD;PInt:PInteger;PStr:PString):boolean;
//Var
//    i:Integer;
//    PFile:PFileRecEC;
//    PFolder:CHsFolder;
//begin
//    Result:=false;
//    for i:=0 to m_FolderRec.m_Recnum-1 do
//    begin
//        PFile:=GetFileRec(i);
//        if (PFile.m_Free=0) then
//        begin
//            if PFile.m_Type=FILEEC_FOLDER then
//            begin
//                if not UnpackFolder(PFile.m_RealName+'') then exit;
//                PFolder:=CHsFolder(PFile.m_Extra);
//                if PFolder=nil then raise Exception.Create('������ � �������� ������� ��������� �����');
//                if not PFolder.Unpack(SouHandle,PInt,PStr) then exit;
//            end else
//            begin
//                if PInt<>nil then Inc(PInt^);
//                if PStr<>nil then PStr^:=GetFullPath(PFile.m_RealName+'');
//                if PFile.m_Type=FILEEC_COMPRESSED then
//                begin
//                    if not UnpackCompressedFile(SouHandle,PFile.m_RealName+'') then exit;
//                end else
//                begin
//                    if not UnpackFile(SouHandle,PFile.m_RealName+'') then exit;
//                end;
//            end;
//        end;
//    end;
//    Result:=true;
//end;

void   CHsFolder::ListFileNames(FILENAME_CALLBACK_FUNC Func)
{

    if (!m_RealName.IsEmpty())
    {
        Func(true, false, m_RealName);
    }
    for (DWORD i=0; i<m_FolderRec.m_Recnum; ++i)
    {
        SFileRec *PFile = m_Files + i;
        if (PFile->m_Free == 0)
        {
            if (PFile->m_Type == FILEEC_FOLDER)
            {
                CHsFolder *PFolder = (CHsFolder *)PFile->m_Extra;
                if (PFolder==NULL) ERROR_S(L"Pack file system error!");
                PFolder->ListFileNames(Func);
            } else
            {
                if (PFile->m_NType == FILEEC_COMPRESSED)
                {
                    if (!Func(false,true,CStr(PFile->m_RealName, m_Heap))) return;
                } else
                {
                    if (!Func(false,false,CStr(PFile->m_RealName, m_Heap))) return;
                }
            }
        }
    }
    if (!m_RealName.IsEmpty())
    {
        Func(true, false, CStr("..", m_Heap));
    }
}


CHsFolder *  CHsFolder::GetFolderEx(const CStr &path)
{
    if (path.IsEmpty()) return this;

    CStr beg(m_Heap);
    CStr rem(m_Heap);

    path.Split(beg,rem, "/\\");
    SFileRec *PFile = GetFileRec(beg);
    if (PFile == NULL) return NULL;
    if (PFile->m_Type == FILEEC_FOLDER)
    {
        CHsFolder *PFolder = (CHsFolder *)PFile->m_Extra;
        if (rem.IsEmpty()) return PFolder;
        return PFolder->GetFolderEx(rem);
    }
    return NULL;
}

int   CHsFolder::FindNext(SSearchRec &S)
{
    SFileRec *PFile;

    do
    {
        PFile = GetFileRec(S.Ind);
        if (PFile == NULL)
        {
            return 1;
        }
        S.Name  = PFile->m_RealName;
        S.T     = PFile->m_Type;
        ++S.Ind;
    } while (PFile->m_Free != 0);
    return 0;
}

















//**********************************************************************
//***************** ����� -�������� ���� *******************************
//**********************************************************************

CPackFile::CPackFile(CHeap *heap, const wchar *name):m_Heap(heap),m_FileName(name,heap)
{
    m_Handle = 0xFFFFFFFF;
#ifdef SUPPORT_IN_MEMORY_STRUCTURES
    RESETFLAG(m_Flags, PFFLAG_EMPTY);
#endif

    m_RootFolder = NULL;
    m_RootOffset = 0;
    //m_ID = 0xFFFFFFFF;
    for (int i=0;i<MAX_VIRTUAL_HANDLE_COUNT;++i)
    {
        m_Handles[i].m_Free = true;
    }
}

CPackFile::~CPackFile()
{
    Clear();
    ClosePacketFileEx();
}

void CPackFile::Clear(void)
{
    for (int i=0;i<MAX_VIRTUAL_HANDLE_COUNT;++i)
    {
        if (!m_Handles[i].m_Free)
        {
            Close(i);
            m_Handles[i].m_Free = true;
        }
    }
}


bool  CPackFile::OpenPacketFile(void)
{
    if (m_Handle!=0xFFFFFFFF || m_RootFolder!=NULL) ClosePacketFile();

#ifdef SUPPORT_IN_MEMORY_STRUCTURES
    if (FLAG(m_Flags, PFFLAG_EMPTY))
    {
        m_RootOffset = 0;
        m_RootFolder = HNew(m_Heap) CHsFolder(CStr(m_Heap));
        m_RootFolder->AllocEmptyFolder();
        return true;
    }
#endif

    // ��������� ���� ��� ������/������
    if (IS_UNICODE())
    {
        m_Handle = (DWORD)CreateFileW(m_FileName.Get(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    } else
    {
        m_Handle = (DWORD)CreateFileA(CStr(m_FileName, m_Heap).Get(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    }
    
    if ((HANDLE)m_Handle == INVALID_HANDLE_VALUE)
    {
#ifdef RAISE_ALL_EXCEPTIONS
        ERROR_S((L"Error openning package file [READ]:"+m_FileName).Get());
#endif
        m_Handle = 0xFFFFFFFF;
        return false;
    }
    // ������ ������ ������ � �������������� ��������� ��������
    DWORD temp = 0;
    if (!::ReadFile((HANDLE)m_Handle,&m_RootOffset,4,&temp,NULL))
    {
#ifdef RAISE_ALL_EXCEPTIONS
        ERROR_S((L"Error openning package file [READ]:"+m_FileName).Get());
#endif
        m_Handle = 0xFFFFFFFF;
        return false;
    }
    // ������ ������ ������ � �������� �����
    m_RootFolder = HNew(m_Heap) CHsFolder(CStr(m_Heap), m_Heap);
    if (!m_RootFolder->ReadFolder(m_Handle,m_RootOffset))
    {
        HDelete(CHsFolder, m_RootFolder, m_Heap);
        m_RootFolder = NULL;
        ClosePacketFile();
#ifdef RAISE_ALL_EXCEPTIONS
        ERROR_S((L"Error reading file system of the package file: "+ m_FileName).Get());
#endif
        return false;
    }
    return true;
}


bool CPackFile::ClosePacketFile(void)
{
    if (m_Handle == 0xFFFFFFFF && m_RootFolder==NULL) return false;
    Clear();
    if (m_RootFolder)
    {
        HDelete (CHsFolder, m_RootFolder, m_Heap);
        m_RootFolder = NULL;
    }
    bool res;
    if (m_Handle != 0xFFFFFFFF)
    {
        res = CloseHandle((HANDLE)m_Handle) != FALSE;
    } else
    {
        res = true;
    }
    m_Handle = 0xFFFFFFFF;
    return res;
}


//bool    CPackFile::CreatePacketFileEx(void)
//{
//    if (m_Handle != 0xFFFFFFFF || m_RootFolder) ClosePacketFileEx();
//#ifdef SUPPORT_IN_MEMORY_STRUCTURES
//    if (FLAG(m_Flags, PFFLAG_EMPTY))
//    {
//        m_RootOffset = 0;
//        m_RootFolder = HNew(m_Heap) CHsFolder(CStr(m_Heap));
//        m_RootFolder->AllocEmptyFolder();
//        return true;
//    }
//#endif
//
//    // ��������� ���� ��� ������/������
//    if (IS_UNICODE())
//    {
//        m_Handle = (DWORD)CreateFileW(m_FileName.Get(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
//    } else
//    {
//        m_Handle = (DWORD)CreateFileA(CStr(m_FileName, m_Heap).Get(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
//    }
//
//    if ((HANDLE)m_Handle == INVALID_HANDLE_VALUE)
//    {
//#ifdef RAISE_ALL_EXCEPTIONS
//        ERROR_S((L"Error creating package file [READ|WRITE]:"+m_FileName).Get());
//#endif
//        m_Handle = 0xFFFFFFFF;
//        return false;
//    }
//    // ������ ������ ������ � �������� �����
//    m_RootOffset = 4;
//    m_RootFolder = HNew (m_Heap) CHsFolder(CStr(m_Heap));
//    m_RootFolder->AllocEmptyFolder();
//    return true;
//}

bool    CPackFile::OpenPacketFileEx()
{

    if (m_Handle!=0xFFFFFFFF || m_RootFolder!=NULL) ClosePacketFileEx();

#ifdef SUPPORT_IN_MEMORY_STRUCTURES
    if (FLAG(m_Flags, PFFLAG_EMPTY))
    {
        m_RootOffset = 0;
        m_RootFolder = HNew(m_Heap) CHsFolder(CStr(m_Heap));
        m_RootFolder->AllocEmptyFolder();
        return true;
    }
#endif

    // ��������� ���� ��� ������/������
    if (IS_UNICODE())
    {
        m_Handle = (DWORD)CreateFileW(m_FileName.Get(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    } else
    {
        m_Handle = (DWORD)CreateFileA(CStr(m_FileName, m_Heap).Get(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    }
    
    if ((HANDLE)m_Handle == INVALID_HANDLE_VALUE)
    {
#ifdef RAISE_ALL_EXCEPTIONS
        ERROR_S((L"Error openning package file [READ|WRITE]:"+m_FileName).Get());
#endif
        m_Handle = 0xFFFFFFFF;
        return false;
    }
    // ������ ������ ������ � �������������� ��������� ��������
    DWORD temp = 0;
    if (!::ReadFile((HANDLE)m_Handle,&m_RootOffset,4,&temp,NULL))
    {
#ifdef RAISE_ALL_EXCEPTIONS
        ERROR_S((L"Error openning package file [READ]:"+m_FileName).Get());
#endif
        m_Handle = 0xFFFFFFFF;
        return false;
    }
    // ������ ������ ������ � �������� �����
    m_RootFolder = HNew(m_Heap) CHsFolder(CStr(m_Heap), m_Heap);
    if (!m_RootFolder->ReadFolder(m_Handle,m_RootOffset))
    {
        HDelete(CHsFolder, m_RootFolder, m_Heap);
        m_RootFolder = NULL;
        ClosePacketFile();
#ifdef RAISE_ALL_EXCEPTIONS
        ERROR_S((L"Error reading file system of the package file: "+ m_FileName).Get());
#endif
        return false;
    }
    return true;
}




int    CPackFile::GetFreeHandle(void)
{
    for (int i=0;i<MAX_VIRTUAL_HANDLE_COUNT; ++i)
    {
        if (m_Handles[i].m_Free)
        {
            return i;
        }
    }
    return -1;
}

DWORD   CPackFile::Open(const CStr &filename,DWORD modeopen)
{
    
    SFileRec *PFile;

    int H = GetFreeHandle();
    if (H<0) return 0xFFFFFFFF;
    if (m_RootFolder == NULL)
    {
        ERROR_S((L"Package not opened: " + CWStr(filename)).Get());
    }
    PFile = m_RootFolder->GetFileRecEx(filename);

    if (PFile == NULL)
    {

#ifdef HANDLE_OUT_OF_PACK_FILES
        // *** ���� ���� �� ������, �� �������� ����� ��� �� ����� *** //
        WIN32_FIND_DATAA fd;
        HANDLE h = FindFirstFileA(filename.Get(), &fd);
        if (h = INVALID_HANDLE_VALUE) return 0xFFFFFFFF;
        FindClose(h);

        m_Handles[H].m_Handle = (DWORD)CreateFileA(filename.Get(), modeopen, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if ((HANDLE)m_Handles[H].m_Handle == INVALID_HANDLE_VALUE) return 0xFFFFFFFF;
        m_Handles[H].m_StartOffset = 0;
        m_Handles[H].m_Offset = 0;
        m_Handles[H].m_Size = ::GetFileSize((HANDLE)m_Handles[H].m_Handle,NULL);
        m_Handles[H].m_SouBuf = NULL;
        m_Handles[H].m_DesBuf = NULL;
        m_Handles[H].m_Compressed = false;
        m_Handles[H].m_Blocknumber = -1;
        if (m_Handles[H].m_Size == INVALID_FILE_SIZE)
        {
            ERROR_S((L"File system error:" + CWStr(filename)).Get());
        }

        DWORD Error = SetFilePointer((HANDLE)m_Handles[H].m_Handle, m_Handles[H].m_Offset, NULL, FILE_BEGIN);
        if (Error == 0xFFFFFFFF)
        {
            ERROR_S((L"File system error:" + CWStr(filename)).Get());
        }

        m_Handles[H].m_Free = false;
        return H;
#else
        return 0xFFFFFFFF;
#endif
    }

    // *** ���� ��� ������ ������ ��������� ����� - ��������� ��� ��� ������ ***
    if (m_Handle == 0xFFFFFFFF) return 0xFFFFFFFF;
    
    m_Handles[H].m_Handle = m_Handle;
    m_Handles[H].m_StartOffset = PFile->m_Offset+4;
    m_Handles[H].m_Offset = PFile->m_Offset+4;
    m_Handles[H].m_Size = PFile->m_RealSize;
    m_Handles[H].m_Free = false;
    m_Handles[H].m_Compressed = PFile->m_Type==FILEEC_COMPRESSED;
    m_Handles[H].m_Blocknumber = -1;
    if (m_Handles[H].m_Compressed)
    {
        m_Handles[H].m_SouBuf = (BYTE *)HAlloc(72112, m_Heap);
        m_Handles[H].m_DesBuf = (BYTE *)HAlloc(65536, m_Heap);
    } else
    {
        m_Handles[H].m_SouBuf = NULL;
        m_Handles[H].m_DesBuf = NULL;
    };
    DWORD Error = SetFilePointer((HANDLE)m_Handles[H].m_Handle, m_Handles[H].m_Offset, NULL, FILE_BEGIN);
    if (Error == 0xFFFFFFFF)
    {
        ERROR_S((L"Packet file system error:" + CWStr(filename)).Get());
    }
    return H;
}

bool  CPackFile::Close(DWORD Handle)
{
    if (Handle == 0xFFFFFFFF) return false;
    if (Handle>=MAX_VIRTUAL_HANDLE_COUNT)
    {
        ERROR_S(L"Too big file handle number");
    }
    if (m_Handles[Handle].m_Free) return false;

    if (m_Handles[Handle].m_Handle == m_Handle)
    {
        // in pack file
        if (m_Handles[Handle].m_Compressed)
        {
            HFree(m_Handles[Handle].m_SouBuf, m_Heap); m_Handles[Handle].m_SouBuf = NULL;
            HFree(m_Handles[Handle].m_DesBuf, m_Heap); m_Handles[Handle].m_DesBuf = NULL;
        }
        m_Handles[Handle].m_Free = true;
        return true;
    }
#ifdef HANDLE_OUT_OF_PACK_FILES

    if (0 == ::CloseHandle((HANDLE)m_Handles[Handle].m_Handle))
    {
        ERROR_S(L"Error closing file");
    }
    if (m_Handles[Handle].m_Compressed)
    {
        HFree(m_Handles[Handle].m_SouBuf, m_Heap); m_Handles[Handle].m_SouBuf = NULL;
        HFree(m_Handles[Handle].m_DesBuf, m_Heap); m_Handles[Handle].m_DesBuf = NULL;
    }
    m_Handles[Handle].m_Free = true;
#else
    return false;
#endif
}

DWORD   CPackFile::SetCompressedBlockPointer(DWORD StartOffset,int nBlock)
{
    DWORD Temp,Size;

    DWORD Offset = StartOffset;
    for(;;)
    {
        ::SetFilePointer((HANDLE)m_Handle,Offset,NULL,FILE_BEGIN);
        ::ReadFile((HANDLE)m_Handle,&Size,4,&Temp,NULL);
        if (nBlock)
        {
            --nBlock;
            Offset += Size+4;
        } else
            break;
    }
    return Size;
}

bool CPackFile::Read(DWORD Handle, void *buf,int Size)
{
    DWORD Temp;
    int Block;
    DWORD IntBlockOffset;
    DWORD IntBlockSize;
    //#ifdef SUPPORT_COMPRESSION_OFF
    //#else
    DWORD CompSize;
    //#endif

    DWORD IntOffset;
    
    if (Handle == 0xFFFFFFFF) return false;
    if (Handle>=MAX_VIRTUAL_HANDLE_COUNT)
    {
        ERROR_S(L"Too big file handle number");
    }
   
    if (m_Handles[Handle].m_Free) return false;

    // *** ��������� ��������� �� ������ �������
    if (m_Handles[Handle].m_Compressed)
    {
        //  ����������� ������ �����, � �������� ���������� �������� ������
        BYTE *SouPtr = m_Handles[Handle].m_DesBuf;
        BYTE *DesPtr = (BYTE *)buf;
        while (Size)
        {
            // ���������� ����� �����, ������ �������� ���� ���������� � �����

            IntOffset = m_Handles[Handle].m_Offset-m_Handles[Handle].m_StartOffset;
            Block = IntOffset >> 16; // 65536
            IntBlockOffset = IntOffset - (DWORD(Block) << 16);
            IntBlockSize = Size;
            if (IntBlockSize>(65536-IntBlockOffset))
            {
                IntBlockSize = 65536-IntBlockOffset;
            }

            // ���� ���� �� ��� ������ �������, �� ��������� ���
            if (m_Handles[Handle].m_Blocknumber != Block)
            {
                CompSize = SetCompressedBlockPointer(m_Handles[Handle].m_StartOffset,Block);
                BOOL res = ::ReadFile((HANDLE)m_Handle, m_Handles[Handle].m_SouBuf, CompSize, &Temp, NULL);
                if (res == FALSE) return false;
                
                OKGF_ZLib_UnCompress2(m_Handles[Handle].m_DesBuf,65536,
                                                 m_Handles[Handle].m_SouBuf,CompSize);

                m_Handles[Handle].m_Blocknumber = Block;
            }
            // ������ ��� ���������� ��������� ����� ������ � �����, ��������� �������������
            memcpy(DesPtr, SouPtr + IntBlockOffset, IntBlockSize);
            DesPtr += IntBlockSize;
            Size -= IntBlockSize;
            m_Handles[Handle].m_Offset = m_Handles[Handle].m_Offset + IntBlockSize;
        }
        return true;
    } else
    {
        ::SetFilePointer((HANDLE)m_Handles[Handle].m_Handle,
                       m_Handles[Handle].m_Offset,NULL,FILE_BEGIN);
        BOOL res = ::ReadFile((HANDLE)m_Handles[Handle].m_Handle, buf, Size, &Temp, NULL);
        bool ret = (res != FALSE) && (Size == Temp);
        m_Handles[Handle].m_Offset = m_Handles[Handle].m_Offset + Temp;
        return ret;
    }
}

#ifdef HANDLE_OUT_OF_PACK_FILES
bool  CPackFile::Write(DWORD Handle, const void *buf,int Size)
{
    DWORD Temp;
    DWORD LocalOffset;

    if (Handle == 0xFFFFFFFF) return false;
    if (Handle>=MAX_VIRTUAL_HANDLE_COUNT)
    {
        ERROR_S(L"Too big file handle number");
    }
    if (m_Handles[Handle].m_Free) return false;

    // *** ��������� ��������� �� ������ �������
    if (m_Handles[Handle].m_Compressed)
    {
        ERROR_S(L"Error writing compressed file");
    }
    ::SetFilePointer((HANDLE)m_Handles[Handle].m_Handle, m_Handles[Handle].m_Offset,NULL,FILE_BEGIN);
    
    BOOL res = ::WriteFile((HANDLE)m_Handles[Handle].m_Handle, buf, Size, &Temp, NULL);
    bool ret = (res != FALSE) && (Size == Temp);
    m_Handles[Handle].m_Offset = m_Handles[Handle].m_Offset + Temp;
    LocalOffset = m_Handles[Handle].m_Offset - m_Handles[Handle].m_StartOffset;
    if (m_Handles[Handle].m_Size < LocalOffset) 
    {
        m_Handles[Handle].m_Size = LocalOffset;
    }
}
#endif

bool  CPackFile::SetPos(DWORD Handle, DWORD Pos, int ftype)
{
    DWORD Error;
    int   Block;

    if (Handle == 0xFFFFFFFF) return false;
    if (Handle>=MAX_VIRTUAL_HANDLE_COUNT)
    {
        ERROR_S(L"Too big file handle number");
    }
    if (m_Handles[Handle].m_Free) return false;

    // �������� �������
    if (ftype==FILE_CURRENT)
    {
        Pos += m_Handles[Handle].m_Offset - m_Handles[Handle].m_StartOffset;
    }
    else if (ftype==FILE_END)
    {
        Pos = m_Handles[Handle].m_Size-Pos;
    }
    //{$ifdef SUPPORT_COMPRESSION_OFF}
    //if m_Handles[Handle].m_Compressed then
    //    raise Exception.Create('Compression is not supported');
    //{$endif}
    if (m_Handles[Handle].m_Compressed)
    {
        if (Pos>m_Handles[Handle].m_Size) return false;
        Block = Pos >> 16;
        if (Block!=m_Handles[Handle].m_Blocknumber)
        {
            m_Handles[Handle].m_Blocknumber = -1;
        }
        m_Handles[Handle].m_Offset = Pos+m_Handles[Handle].m_StartOffset;
    } else
    {
        Error = SetFilePointer((HANDLE)m_Handles[Handle].m_Handle, m_Handles[Handle].m_StartOffset+Pos,NULL,FILE_BEGIN);
        if (Error==0xFFFFFFFF)
        {
            ERROR_S(L"Unable to set file pointer.");
            //then raise Exception.Create('������ ��������� ��������� � �������� ����� :'+m_filename);
        }
        m_Handles[Handle].m_Offset = Error;
    }
    return true;
}

DWORD   CPackFile::GetPos(DWORD Handle)
{
    if (Handle == 0xFFFFFFFF) return 0xFFFFFFFF;
    if (Handle>=MAX_VIRTUAL_HANDLE_COUNT)
    {
        ERROR_S(L"Too big file handle number");
    }
    if (m_Handles[Handle].m_Free) return 0xFFFFFFFF;

    return m_Handles[Handle].m_Offset - m_Handles[Handle].m_StartOffset;
}

DWORD    CPackFile::GetSize(DWORD Handle)
{
    if (Handle == 0xFFFFFFFF) return 0xFFFFFFFF;
    if (Handle>=MAX_VIRTUAL_HANDLE_COUNT)
    {
        ERROR_S(L"Too big file handle number");
    }
    if (m_Handles[Handle].m_Free) return 0xFFFFFFFF;
    return m_Handles[Handle].m_Size;
}

DWORD   CPackFile::GetHandle(DWORD Handle)
{
    if (Handle == 0xFFFFFFFF) return 0xFFFFFFFF;
    if (Handle>=MAX_VIRTUAL_HANDLE_COUNT)
    {
        ERROR_S(L"Too big file handle number");
    }
    if (m_Handles[Handle].m_Free) return 0xFFFFFFFF;
    return m_Handles[Handle].m_Handle;
}

#ifdef HANDLE_OUT_OF_PACK_FILES
bool    CPackFile::PathExists(const CStr &path)
{
    if (m_RootFolder->PathExists(path)) return true;

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(path.Get(), &fd);
    if (h = INVALID_HANDLE_VALUE) return false;
    FindClose(h);
    return (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool    CPackFile::FileExists(const CStr &path)
{
    if (m_RootFolder->FileExists(path)) return true;

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(path.Get(), &fd);
    if (h = INVALID_HANDLE_VALUE) return false;
    FindClose(h);
    return true;
}
#endif


//Function    CPackFile.AddFiles(block:TBlockParEC;Var log:Text):boolean;
//Var
//    i:Integer;
//    value,spec:WideString;
//    svalue:string;
//    inblock:TBlockParEC;
//begin
//    Result:=true;
//    // ���������� ���� ������, ������ �� ������� ���� � ������� �����
//    for i:=0 to block.Par_Count-1 do
//    begin
//        value:=block.Par_Get(i);
//        if GetCountParEC(value,',')>1 then
//        begin
//            spec:=GetStrParEC(value,0,',');
//            value:=GetStrParEC(value,1,',');
//            if (spec='z') or (spec='Z') then
//                Result:=AddFile(value,FILEEC_COMPRESSED)
//            else
//                Result:=AddFile(value,FILEEC_BINARY);
//        end else Result:=AddFile(value);
//        if not Result then
//        begin
//            svalue:=value;
//            Writeln(log,'Add Error: ',svalue);
//            Result:=true;
//        end;
//    end;
//    // �������� ���� ��������� ������
//    for i:=0 to block.Block_Count-1 do
//    begin
//        inblock:=block.Block_Get(i);
//        Result:=AddFiles(inblock,log);
//        if not Result then exit;
//    end;
//end;
//
//Function    CPackFile.AddFiles(block:TBlockParEC):boolean;
//Var
//    i:Integer;
//    value,spec:WideString;
//    svalue:string;
//    inblock:TBlockParEC;
//begin
//    Result:=true;
//    // ���������� ���� ������, ������ �� ������� ���� � ������� �����
//    for i:=0 to block.Par_Count-1 do
//    begin
//        value:=block.Par_Get(i);
//        if GetCountParEC(value,',')>1 then
//        begin
//            spec:=GetStrParEC(value,0,',');
//            value:=GetStrParEC(value,1,',');
//            if (spec='z') or (spec='Z') then
//                Result:=AddFile(value,FILEEC_COMPRESSED)
//            else
//                Result:=AddFile(value,FILEEC_BINARY);
//        end else Result:=AddFile(value);
//        if not Result then
//        begin
//            svalue:=value;
//            Result:=true;
//        end;
//    end;
//    // �������� ���� ��������� ������
//    for i:=0 to block.Block_Count-1 do
//    begin
//        inblock:=block.Block_Get(i);
//        Result:=AddFiles(inblock);
//        if not Result then exit;
//    end;
//end;
//
//Function    CPackFile.AddFile(name:string):boolean;
//begin
//    Result:=AddFile(name,FILEEC_BINARY);
//end;
//
//Function    CPackFile.AddPath(name:string):boolean;
//begin
//    Result:=AddPath(name,FILEEC_BINARY);
//end;
//
//Function    CPackFile.AddFile(name:string;ftype:DWORD):boolean;
//begin
//    Result:=false;
//    if m_RootFolder=nil then exit;
//    Result:=m_RootFolder.AddFile(name,ftype);
//end;
//
//Function    CPackFile.DeleteFile(name:string):boolean;
//begin
//    Result:=false;
//    if m_RootFolder=nil then exit;
//    Result:=m_RootFolder.DeleteFile(name);
//end;
//
//Function    CPackFile.AddPath(name:string;ftype:DWORD):boolean;
//begin
//    Result:=false;
//    if m_RootFolder=nil then exit;
//    Result:=m_RootFolder.AddPath(name,ftype);
//end;
//
//Function    CPackFile.CreateFolder(name:string):boolean;
//begin
//    Result:=false;
//    if m_RootFolder=nil then exit;
//    Result:=m_RootFolder.CreateFolder(GetLocalPath(name));
//end;
//
//Function    CPackFile.Compress(PInt:PInteger;PStr:PString):boolean;
//Var
//    Handle:DWORD;
//    RootOffset:DWORD;
//    Temp:DWORD;
//begin
//    SetCurrentDir(ExtractFileDir(ParamStr(0)));
//    Result:=false;
//    if m_RootFolder=nil then exit;
//    Handle:=CreateFile('00000000.tmp',
//                        GENERIC_WRITE,
//                        0,nil,
//                        CREATE_ALWAYS,
//                        FILE_ATTRIBUTE_NORMAL,
//                        0);
//    if Handle=INVALID_HANDLE_VALUE then exit;
//    RootOffset:=4;
//    SetFilePointer(Handle,0,nil,FILE_BEGIN);
//    Windows.WriteFile(Handle,RootOffset,4,Temp,nil);
//    m_RootFolder.Compress(m_Handle,Handle,4,PInt,PStr);
//    CloseHandle(Handle);
//    ClosePacketFileEx;
//    SysUtils.DeleteFile(m_Filename);
//    RenameFile('00000000.tmp',m_Filename);
//    OpenPacketFileEx;
//    Result:=true;
//end;
//
//Function    CPackFile.Compress(name:string;PInt:PInteger;PStr:PString):boolean;
//Var
//    Handle:DWORD;
//    RootOffset:DWORD;
//    Temp:DWORD;
//begin
//    SetCurrentDir(ExtractFileDir(ParamStr(0)));
//    Result:=false;
//    if m_RootFolder=nil then exit;
//    Handle:=CreateFile(PChar(name),
//                        GENERIC_WRITE,
//                        0,nil,
//                        CREATE_ALWAYS,
//                        FILE_ATTRIBUTE_NORMAL,
//                        0);
//    if Handle=INVALID_HANDLE_VALUE then exit;
//    RootOffset:=4;
//    SetFilePointer(Handle,0,nil,FILE_BEGIN);
//    Windows.WriteFile(Handle,RootOffset,4,Temp,nil);
//    m_RootFolder.Compress(m_Handle,Handle,4,PInt,PStr);
//    CloseHandle(Handle);
//    ClosePacketFileEx; // ��� ���������� �����������
//    OpenPacketFileEx;
//    Result:=true;
//end;
//
//Function    CPackFile.Pack(PInt:PInteger;PStr:PString):boolean;
//Var
//    Pos:DWORD;
//    Temp:DWORD;
//begin
//    SetCurrentDir(ExtractFileDir(ParamStr(0)));
//    Result:=false;
//    if m_RootFolder=nil then exit;
//    if m_Handle=$FFFFFFFF then exit;
//    Pos:=SetFilePointer(m_Handle,0,nil,FILE_END);
//    if (m_RootFolder.m_ToUpdate) or (m_RootFolder.m_ToSave) then
//    begin
//        if Pos=0 then Pos:=4;
//        m_RootOffset:=Pos;
//        SetFilePointer(m_Handle,0,nil,FILE_BEGIN);
//        Windows.WriteFile(m_Handle,m_RootOffset,4,Temp,nil);
//    end;
//    m_RootFolder.Pack(m_Handle,Pos,PInt,PStr);
//    ClosePacketFileEx; // ��� ���������� �����������
//    OpenPacketFileEx;
//    Result:=true;
//end;
//
//Function    CPackFile.Unpack(PInt:PInteger;PStr:PString):boolean;
//begin
//    SetCurrentDir(ExtractFileDir(ParamStr(0)));
//    Result:=false;
//    if m_RootFolder=nil then exit;
//    if m_Handle=$FFFFFFFF then exit;
//    Result:=m_RootFolder.Unpack(m_Handle,PInt,PStr);
//end;
//
//Function    CPackFile.Compress:boolean;
//begin
//    Result:=Compress(nil,nil);
//end;
//
//Function    CPackFile.Compress(name:string):boolean;
//begin
//    Result:=Compress(name,nil,nil);
//end;
//
//Function    CPackFile.Pack:boolean;
//begin
//    Result:=Pack(nil,nil);
//end;
//
//Function    CPackFile.Unpack:boolean;
//begin
//    Result:=Unpack(nil,nil);
//end;
//
//Function    CPackFile.UnpackFile(souname:string):boolean;
//begin
//    Result:=UnpackFile(souname,souname);
//end;
//
//Function    CPackFile.UnpackFile(souname,desname:string):boolean;
//Var
//    SouHandle,DesHandle:DWORD;
//    Size,Temp:DWORD;
//    Buf:Pointer;
//    beginning:string;
//    remainder:string;
//    path:string;
//begin
//    SetCurrentDir(ExtractFileDir(ParamStr(0)));
//    Result:=false;
//    // �������� ��������� ���������, ���� ��� ����������
//    remainder:=desname;
//    path:='';
//    Repeat
//        beginning:=GetBeginningStr(remainder,'/\');
//        remainder:=GetRemainderStr(remainder,'/\');
//        if path='' then path:=beginning
//                   else path:=path+'\'+beginning;
//        if remainder<>'' then
//        begin
//            if not DirectoryExists(path) then
//                CreateDirectory(PChar(path),nil);
//        end;
//    until remainder='';
//    // ��������� ���� ��� ������
//    DesHandle:=CreateFile(PChar(desname),
//                          GENERIC_WRITE,
//                          0,nil,
//                          CREATE_ALWAYS,
//                          FILE_ATTRIBUTE_NORMAL,
//                          0);
//    if DesHandle=INVALID_HANDLE_VALUE then exit;
//    SouHandle:=Open(souname);
//    if SouHandle=$FFFFFFFF then
//    begin
//        CloseHandle(DesHandle);
//        exit;
//    end;
//    Size:=GetSize(SouHandle);
//    // �������� ������
//    Buf:=AllocMem(65536);
//    while Size>65536 do
//    begin
//        Read(SouHandle,Buf^,65536);
//        Windows.WriteFile(DesHandle,Buf^,65536,Temp,nil);
//        Size:=Size-65536;
//    end;
//    Read(SouHandle,Buf^,Size);
//    Windows.WriteFile(DesHandle,Buf^,Size,Temp,nil);
//    Close(SouHandle);
//    CloseHandle(DesHandle);
//    FreeMem(Buf);
//    Result:=true;
//end;


int     CPackFile::FindFirst(const CStr &path,DWORD Attr, SSearchRec &S)
{
    S.Path      = path;
    S.Ind       = 0;
    S.Name      = "";
    S.Folder    = NULL;
    if (m_RootFolder == NULL) return NULL;
    if (path.IsEmpty())
        S.Folder = m_RootFolder;
    else
        S.Folder = m_RootFolder->GetFolderEx(S.Path);
    
    return S.Folder->FindNext(S);
}












//**********************************************************************
//***************** ����� - ��������� �������� ������ ******************
//**********************************************************************

void    CPackCollection::Clear(void)
{
    PCPackFile *ff = m_PackFiles.Buff<PCPackFile>();
    PCPackFile *fe = m_PackFiles.BuffEnd<PCPackFile>();
    for(;ff < fe; ++ff)
    {
        HDelete(CPackFile, *ff, m_Heap);
    }
    m_PackFiles.Clear();
}

void    CPackCollection::AddPacketFile(const wchar *name)
{
    m_PackFiles.Expand(sizeof(PCPackFile));
    PCPackFile f = HNew(m_Heap) CPackFile(m_Heap, name);
    *(m_PackFiles.BuffEnd<PCPackFile>()-1) = f;
}


void    CPackCollection::DelPacketFile(const wchar *name)
{
    PCPackFile *ff = m_PackFiles.Buff<PCPackFile>();
    PCPackFile *fe = m_PackFiles.BuffEnd<PCPackFile>();
    for(;ff < fe; ++ff)
    {
        if ((*ff)->GetName() == name)
        {
            *ff = *(fe-1);
            m_PackFiles.SetLenNoShrink(m_PackFiles.Len()- sizeof(PCPackFile));
            return;
        }
    }
}

bool CPackCollection::OpenPacketFiles(void)
{
    PCPackFile *ff = m_PackFiles.Buff<PCPackFile>();
    PCPackFile *fe = m_PackFiles.BuffEnd<PCPackFile>();
    for(;ff < fe; ++ff)
    {
        if (!(*ff)->OpenPacketFile()) break;
    }

    if (ff < fe)
    {
        //*********** ������ �������� ������ �� �������� ������ **********
        PCPackFile *fff = m_PackFiles.Buff<PCPackFile>();
        for(;fff < ff; ++fff)
        {
            (*fff)->ClosePacketFile();
        }
        return false;
    }
    return true;
}

bool  CPackCollection::ClosePacketFiles(void)
{
    PCPackFile *ff = m_PackFiles.Buff<PCPackFile>();
    PCPackFile *fe = m_PackFiles.BuffEnd<PCPackFile>();
    for(;ff < fe; ++ff)
    {
        (*ff)->ClosePacketFile();
    }
    return true;
}

bool CPackCollection::OpenPacketFilesEx(void)
{
    PCPackFile *ff = m_PackFiles.Buff<PCPackFile>();
    PCPackFile *fe = m_PackFiles.BuffEnd<PCPackFile>();
    for(;ff < fe; ++ff)
    {
        if (!(*ff)->OpenPacketFileEx()) break;
    }

    if (ff < fe)
    {
        //*********** ������ �������� ������ �� �������� ������ **********
        PCPackFile *fff = m_PackFiles.Buff<PCPackFile>();
        for(;fff < ff; ++fff)
        {
            (*fff)->ClosePacketFileEx();
        }
        return false;
    }
    return true;
}

bool  CPackCollection::ClosePacketFilesEx(void)
{
    PCPackFile *ff = m_PackFiles.Buff<PCPackFile>();
    PCPackFile *fe = m_PackFiles.BuffEnd<PCPackFile>();
    for(;ff < fe; ++ff)
    {
        (*ff)->ClosePacketFileEx();
    }
    return true;
}


//******** ��������� ��� ������ ������� ***********//

bool  CPackCollection::FileExists(const CStr &name)
{
    PCPackFile *ff = m_PackFiles.Buff<PCPackFile>();
    PCPackFile *fe = m_PackFiles.BuffEnd<PCPackFile>();
    for(;ff < fe; ++ff)
    {
        if ((*ff)->FileExists(name)) return true;
    }
    return false;
}

bool  CPackCollection::PathExists(const CStr &path)
{
    PCPackFile *ff = m_PackFiles.Buff<PCPackFile>();
    PCPackFile *fe = m_PackFiles.BuffEnd<PCPackFile>();
    for(;ff < fe; ++ff)
    {
        if ((*ff)->PathExists(path)) return true;
    }
    return false;
}

//******* ������ � ������������ �������� �������� CPackFile
DWORD CPackCollection::Open(const CStr&name, DWORD modeopen)
{
    DWORD Handle;
    DWORD Counter = 0;

    PCPackFile *ff = m_PackFiles.Buff<PCPackFile>();
    PCPackFile *fe = m_PackFiles.BuffEnd<PCPackFile>();
    for(;ff < fe; ++ff)
    {
        Handle = (*ff)->Open(name,modeopen);
        if (Handle!=0xFFFFFFFF) break;
        ++Counter;
    }
    if (Handle==0xFFFFFFFF) return 0xFFFFFFFF;
    return Handle + (Counter << MAX_VIRTUAL_HANDLE_COUNT_BITS);
}

bool  CPackCollection::Close(DWORD Handle)
{
    ASSERT(MAX_VIRTUAL_HANDLE_COUNT == (1 << MAX_VIRTUAL_HANDLE_COUNT_BITS));
    int number = Handle >> MAX_VIRTUAL_HANDLE_COUNT_BITS;
    PCPackFile P = GetPacketFile(number);
    if (P==NULL) return false;
    return P->Close(Handle & (MAX_VIRTUAL_HANDLE_COUNT-1));
}

bool  CPackCollection::Read(DWORD Handle, void *Buf, int Size)
{
    ASSERT(MAX_VIRTUAL_HANDLE_COUNT == (1 << MAX_VIRTUAL_HANDLE_COUNT_BITS));

    int number = Handle >> MAX_VIRTUAL_HANDLE_COUNT_BITS;
    PCPackFile P = GetPacketFile(number);
    if (P==NULL) return false;
    return P->Read(Handle & (MAX_VIRTUAL_HANDLE_COUNT-1),Buf,Size);
}

//bool  CPackCollection::Write(DWORD Handle, const void *Buf, int Size)
//{
//    ASSERT(MAX_VIRTUAL_HANDLE_COUNT == (1 << MAX_VIRTUAL_HANDLE_COUNT_BITS));
//
//    int number = Handle >> MAX_VIRTUAL_HANDLE_COUNT_BITS;
//    PCPackFile P = GetPacketFile(number);
//    if (P==NULL) return false;
//    return P->Write(Handle & (MAX_VIRTUAL_HANDLE_COUNT-1),Buf,Size);
//}

bool  CPackCollection::SetPos(DWORD Handle, DWORD Pos, int ftype)
{
    ASSERT(MAX_VIRTUAL_HANDLE_COUNT == (1 << MAX_VIRTUAL_HANDLE_COUNT_BITS));

    int number = Handle >> MAX_VIRTUAL_HANDLE_COUNT_BITS;
    PCPackFile P = GetPacketFile(number);
    if (P==NULL) return false;
    return P->SetPos(Handle & (MAX_VIRTUAL_HANDLE_COUNT-1),Pos,ftype);
}

DWORD  CPackCollection::GetPos(DWORD Handle)
{
    ASSERT(MAX_VIRTUAL_HANDLE_COUNT == (1 << MAX_VIRTUAL_HANDLE_COUNT_BITS));

    int number = Handle >> MAX_VIRTUAL_HANDLE_COUNT_BITS;
    PCPackFile P = GetPacketFile(number);
    if (P==NULL) return 0xFFFFFFFF;
    return P->GetPos(Handle & (MAX_VIRTUAL_HANDLE_COUNT-1));
}

DWORD  CPackCollection::GetSize(DWORD Handle)
{
    ASSERT(MAX_VIRTUAL_HANDLE_COUNT == (1 << MAX_VIRTUAL_HANDLE_COUNT_BITS));

    int number = Handle >> MAX_VIRTUAL_HANDLE_COUNT_BITS;
    PCPackFile P = GetPacketFile(number);
    if (P==NULL) return 0xFFFFFFFF;
    return P->GetSize(Handle & (MAX_VIRTUAL_HANDLE_COUNT-1));
}

DWORD  CPackCollection::GetHandle(DWORD Handle)
{
    ASSERT(MAX_VIRTUAL_HANDLE_COUNT == (1 << MAX_VIRTUAL_HANDLE_COUNT_BITS));

    int number = Handle >> MAX_VIRTUAL_HANDLE_COUNT_BITS;
    PCPackFile P = GetPacketFile(number);
    if (P==NULL) return 0xFFFFFFFF;
    return P->GetHandle(Handle & (MAX_VIRTUAL_HANDLE_COUNT-1));
}


//bool  CPackCollection::UnpackFile(const CStr&souname,const CStr&desname)
//{
//    PCPackFile *ff = m_PackFiles.Buff<PCPackFile>();
//    PCPackFile *fe = m_PackFiles.BuffEnd<PCPackFile>();
//    for(;ff < fe; ++ff)
//    {
//        if ((*ff)->FileExists(souname))
//        {
//            return (*ff)->UnpackFile(souname,desname);
//        }
//    }
//    return false;
//}



} // namespace

//#endif
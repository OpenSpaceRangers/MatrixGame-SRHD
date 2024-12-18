#pragma once

#include "CException.hpp"
#include "CMain.hpp"
#include "CHeap.hpp"



namespace Base {

__forceinline int WStrLen(const wchar * str)
{
    if(str==NULL) {return 0;}
	int len=0;
    while(str[len]!=0) {len++;}
	return len;
}

__forceinline bool WStrCmp(const wchar_t * s1,const wchar_t * s2) 
{
	for(;;)
    {
        if(*s1!=*s2) {return false;}
        if(*s1==0) {return true;}
		s1++;
		s2++;
	}
}

class CStr;

struct CWStrData
{
	CHeap * m_Heap;
	int m_Refs;
	int m_Len;
	int m_Max;

	wchar * Data() { return (wchar *)(this+1); }

    void RefDecAndDelete(void) {if ((--m_Refs)<=0) HFree(this, m_Heap);}
    void RefDec(void) {--m_Refs;};
};

class BASE_API CWStr : public CMain
{

        CWStrData * m_Data;

        void Tream(int len);
        void ModifyLen(CHeap *heap,int len);
        void ModifyLenNoCopy(CHeap *heap,int len);
        void NewDataLen(CHeap *heap, int len);

        CWStr(const wchar * s1,int len1,const wchar * s2,int len2, CHeap * heap) {NewDataLen(heap, len1+len2); memcpy(m_Data->Data(), s1, len1*sizeof(wchar)); memcpy(m_Data->Data()+len1, s2, len2*sizeof(wchar)); m_Data->Data()[m_Data->m_Len] = 0; }
    public:

        static int CWStr::call_num;

        explicit CWStr(CHeap * heap=NULL):CMain()                           { NewDataLen(heap, 0); m_Data->Data()[0] = 0; }
        explicit CWStr(const CStr & s);
        CWStr(const CWStr & s):CMain(), m_Data(s.m_Data)                    { ++s.m_Data->m_Refs; }
        explicit CWStr(const wchar * s, CHeap * heap=NULL):CMain()          { int len = WStrLen(s); NewDataLen(heap,len); memcpy(m_Data->Data(), s, (len+1) * sizeof(wchar)); }
		CWStr(const wchar * s,int len, CHeap * heap=NULL):CMain()           { NewDataLen(heap,len); memcpy(m_Data->Data(), s, len * sizeof(wchar)); m_Data->Data()[len] = 0; }
		explicit CWStr(wchar sim, CHeap * heap=NULL):CMain()                { NewDataLen(heap,1); *m_Data->Data() = sim, *(m_Data->Data()+1) = 0; }
        CWStr(wchar sim,int count, CHeap * heap=NULL)                       { NewDataLen(heap,count);	for(int i=0;i<count;i++) {m_Data->Data()[i]=sim;}	m_Data->Data()[count]=0; }
        explicit CWStr(int zn, CHeap * heap=NULL):CMain()                   { NewDataLen(heap,32);Set(zn); }
        explicit CWStr(DWORD zn, CHeap * heap=NULL):CMain()                 { NewDataLen(heap,32);Set(zn); }
		explicit CWStr(double zn,int zpz=8, CHeap * heap=NULL):CMain()      { NewDataLen(heap,32);Set(zn,zpz); }
//		CWStr(void * zn, CHeap * heap=NULL);
//		CWStr(BYTE zn, CHeap * heap=NULL);

        
        
        //lint -save -e1740
		~CWStr()				{  m_Data->RefDecAndDelete();  }
        //lint -restore
        
        CHeap * GetHeap(void) const	{ return m_Data->m_Heap; }

		// Clear - ������� ������
		void Clear(void)					{ ModifyLenNoCopy(GetHeap(), 0); }

        void SetLen(int len) {ModifyLen(m_Data->m_Heap,len); m_Data->Data()[len] = 0;};

        void CWStr::Set(const CWStr & s)    { if (m_Data != s.m_Data) { m_Data->RefDecAndDelete(); m_Data = s.m_Data; ++s.m_Data->m_Refs;} }
		void Set(const CStr & cstr);
        void Set(const wchar * s)           { if (m_Data->Data() != s) { int len = WStrLen(s); ModifyLenNoCopy(m_Data->m_Heap, len); memcpy(m_Data->Data(), s, (len+1) * sizeof(wchar));}}
        void Set(const wchar * s,int len)   { if (m_Data->Data() == s) {ModifyLen(GetHeap(),len);} else { ModifyLenNoCopy(m_Data->m_Heap, len); memcpy(m_Data->Data(), s, len * sizeof(wchar)); } m_Data->Data()[len] = 0;}
        void Set(wchar sim)                 { ModifyLenNoCopy(m_Data->m_Heap, 1); m_Data->Data()[0] = sim; m_Data->Data()[1] = 0;}
        void Set(wchar sim,int count)       { ModifyLenNoCopy(m_Data->m_Heap, count); for(int i=0;i<count;i++) {m_Data->Data()[i]=sim;}	m_Data->Data()[count]=0;}
		void Set(int zn);
		void Set(dword zn);
		void Set(double zn,int zpz=8);
		void SetHex(void * zn);
		void SetHex(BYTE zn);

		CWStr & Add(const CWStr & cstr);
		CWStr & Add(const wchar * str);
        CWStr & Add(const wchar * str,int lstr);
		CWStr & Add(wchar sim);
		CWStr & Add(wchar sim,int count);
		CWStr & Add(int zn)						{ Add(CWStr(zn)); return *this; }
		CWStr & Add(double zn,int zpz=8)		{ Add(CWStr(zn,zpz)); return *this; }
		CWStr & AddHex(void * zn)				{ CWStr s(GetHeap()); s.SetHex(zn); Add(s); return *this; }
		CWStr & AddHex(BYTE zn)					{ CWStr s(GetHeap()); s.SetHex(zn); Add(s); return *this; }

        const wchar * Get(void)	const				{ return m_Data->Data(); }
        const wchar * GetEx(void)	const		    { if(IsEmpty()) return NULL; else return m_Data->Data(); }
        wchar * GetBuf(void)                        { return m_Data->Data(); }
        wchar * GetBufEx(void)                      { if(IsEmpty()) return NULL; else return m_Data->Data(); }
        int GetLen(void)	const					{ return m_Data->m_Len; }

		int GetInt(void) const ;
        DWORD GetDword(void) const ;
		double GetDouble(void) const ;
		int GetHex(void) const ;
        DWORD GetHexUnsigned(void) const ;
        

		bool IsOnlyInt(void) const;
		bool IsEmpty(void)	const				{ return GetLen() == 0; }

		CWStr & Trim(void);						// ������� � ������ � � ����� ������� 0x20,0x9,0x0d,0x0a
		CWStr & TrimFull(void);					// Trim() � � �������� ������ ������� ������������� 0x20,0x9
		void TabToSpace(void);					// ��������� 0x9 � 0x20

		CWStr & Del(int sme,int len); // ������� �������
		CWStr & Insert(int sme,const CWStr & istr) // �������� �������
        {
            return Insert(sme,istr.Get(), istr.GetLen());
        }
        CWStr & Insert(int sme,const wchar *str, int len); // �������� �������
        CWStr & Insert(int sme,const wchar *str) // �������� �������
        {
            return Insert(sme,str, WStrLen(str));
        }
		CWStr & Replace(CWStr & substr,const CWStr & strreplace); // �������� ����� ������ �� ������

        int Find(const wchar * substr, int slen,int sme=0) const;// ����� ���������. return = �������� �� ������  -1 = ��������� �� �������
        int Find(const CWStr & substr,int sme=0) const	{return Find(substr.Get(), substr.GetLen(), sme);}
        int Find(const wchar * substr,int sme=0) const	{return Find(substr, WStrLen(substr), sme);}
		int FindR(wchar ch,int sme=-1) const;		// ����� ������� � �������� �������. return = �������� �� ������  -1 = ��������� �� �������

		void LowerCase(int sme=0,int len=-1);
		void UpperCase(int sme=0,int len=-1);

		CWStr & Format(const wchar * format,...);

			// ������� ��� ������ � �����������
		// ������� :
		//      Str="count=5,7"    GetCountPar("=,")      return 3
		//      Str="count=5,7"    GetStrPar(str,1,"=")   str="5,7"
		//      Str="count=5,7"    GetIntPar(2,"=,")      return 7
		int GetCountPar(const wchar * ogsim) const;
		int GetSmePar(int np,const wchar * ogsim) const;
		int GetLenPar(int smepar,const wchar * ogsim) const;
		void GetStrPar(CWStr & str,int np,const wchar * ogsim) const;
		CWStr GetStrPar(int np,const wchar * ogsim) const
        {
	        int sme=GetSmePar(np,ogsim);
	        return CWStr(Get()+sme,GetLenPar(sme,ogsim), GetHeap());
        }


		void GetStrPar(CWStr & str,int nps,int npe,const wchar * ogsim) const;
		CWStr GetStrPar(int nps,int npe,const wchar * ogsim) const;
		int GetIntPar(int np,const wchar * ogsim) const							{ return GetStrPar(np,ogsim).GetInt(); }
		double GetDoublePar(int np,const wchar * ogsim) const					{ return GetStrPar(np,ogsim).GetDouble(); }
		bool GetTrueFalsePar(int np,const wchar * ogsim) const;

		static int Compare(const wchar * zn1,int zn1len,const wchar * zn2,int zn2len); // "A","B"=-1  "A","A"=0  "B","A"=1
        static int Compare(const CWStr & zn1,const CWStr & zn2) { return Compare(zn1.Get(), zn1.GetLen(), zn2.Get(), zn2.GetLen());}

        bool Equal(const wchar * zn,int len) const;
        bool Equal(const wchar * zn) const { return Equal(zn, WStrLen(zn)); }

		bool CompareFirst(const CWStr & str) const;
		bool CompareFirst(const wchar * str) const										{ return CompareFirst(CWStr(str)); }
		int CompareSubstring(const CWStr & str) const;
		int CompareSubstring(const wchar * str) const									{ return CompareSubstring(CWStr(str)); }

		CWStr & operator = (const CWStr & s)								{ Set(s); return *this; }
		CWStr & operator = (const wchar * s)							    { Set(s); return *this; }
		CWStr & operator = (wchar zn)										{ Set(zn); return *this; }
		CWStr & operator = (int zn)											{ Set(zn); return *this; }
		CWStr & operator = (double zn)										{ Set(zn); return *this; }
//		CWStr & operator = (void * zn)										{ Set(zn); return *this; }

        wchar & operator [] (int n)											{ if(m_Data==NULL) {ERROR_E;} else return (m_Data->Data())[n]; }

		friend bool operator == (const CWStr & zn1,const CWStr & zn2)
        {
	        int lens1=zn1.GetLen();
            if(lens1!=zn2.GetLen()) return false;

	        const wchar * str1=zn1.Get();
	        const wchar * str2=zn2.Get();

	        for(int i=0;i<lens1;i++) if(str1[i]!=str2[i]) return false;
            return true;
        }
		friend bool operator == (const CWStr & zn1,const wchar * zn2)
        {
	        int lens1=zn1.GetLen();
            if(lens1!=WStrLen(zn2)) return false;
        	
	        const wchar * str1=zn1.Get();

	        for(int i=0;i<lens1;i++) if(str1[i]!=zn2[i]) return false;
            return true;
        }
		friend bool operator == (const wchar * zn1,const CWStr & zn2) 					{ return zn2==zn1; }

		friend bool operator != (const CWStr &zn1,const CWStr &zn2)						{ return !(zn1==zn2); }
		friend bool operator != (const CWStr &zn1,const wchar * zn2)					{ return !(zn1==zn2); }
		friend bool operator != (const wchar * zn1,const CWStr &zn2)					{ return !(zn1==zn2); }

		friend bool operator < (const CWStr & zn1,const CWStr & zn2)					{ if(Compare(zn1,zn2)<0) return 1; return 0; }
		friend bool operator < (const CWStr & zn1,const wchar * zn2)					{ if(Compare(zn1,CWStr(zn2))<0) return 1; return 0; }
		friend bool operator < (const wchar * zn1,const CWStr & zn2)					{ if(Compare(CWStr(zn1),zn2)<0) return 1; return 0; }

		friend bool operator > (const CWStr & zn1,const CWStr & zn2) 					{ if(Compare(zn1,zn2)>0) return 1; return 0; }
		friend bool operator > (const CWStr & zn1,const wchar * zn2)					{ if(Compare(zn1,CWStr(zn2))>0) return 1; return 0; }
		friend bool operator > (const wchar * zn1,const CWStr & zn2)					{ if(Compare(CWStr(zn1),zn2)>0) return 1; return 0; }

		friend bool operator <= (const CWStr & zn1,const CWStr & zn2)				{ if(Compare(zn1,zn2)<=0) return 1; return 0; }
		friend bool operator <= (const CWStr & zn1,const wchar * zn2)				{ if(Compare(zn1,CWStr(zn2))<=0) return 1; return 0; }
		friend bool operator <= (const wchar * zn1,const CWStr & zn2)				{ if(Compare(CWStr(zn1),zn2)<=0) return 1; return 0; }

		friend bool operator >= (const CWStr &zn1,const CWStr & zn2)					{ if(Compare(zn1,zn2)>=0) return 1; return 0; }
		friend bool operator >= (const CWStr &zn1,const wchar * zn2)					{ if(Compare(zn1,CWStr(zn2))>=0) return 1; return 0; }
		friend bool operator >= (const wchar * zn1,const CWStr & zn2)					{ if(Compare(CWStr(zn1),zn2)>=0) return 1; return 0; }

		CWStr & operator +=(const CWStr & str)	                        { Add(str); return *this; }
		CWStr & operator +=(const wchar * str)	    					{ Add(str); return *this; }
		CWStr & operator +=(const wchar sim)							{ Add(sim); return *this; }
		CWStr & operator +=(int zn)										{ Add(zn); return *this; }
		CWStr & operator +=(double zn)									{ Add(zn); return *this; }
//		CWStr & operator +=(void * zn)									{ Add(zn); return *this; }

		friend CWStr operator + (const CWStr & s1,const CWStr & s2)		{ return CWStr(s1.Get(),s1.GetLen(),s2.Get(),s2.GetLen(), s1.GetHeap());}
		friend CWStr operator + (const CWStr & s1,const wchar * s2)		{ return CWStr(s1.Get(),s1.GetLen(),s2, WStrLen(s2), s1.GetHeap()); }
		friend CWStr operator + (const wchar * s1,const CWStr & s2)		{ return CWStr(s1, WStrLen(s1), s2.Get(), s2.GetLen(), s2.GetHeap()); }
		friend CWStr operator + (const CWStr & s1,wchar sim)			{ CWStr str(s1.Get(),s1.GetLen(),s1.GetHeap()); str+=sim; return str; }
		friend CWStr operator + (wchar sim,const CWStr & s2)			{ CWStr str(sim,s2.GetHeap()); str+=s2; return str; }
		friend CWStr operator + (const CWStr & s,int zn)				{ CWStr str(s,s.GetHeap()); str+=zn; return str; }
		friend CWStr operator + (int zn,const CWStr & s)				{ CWStr str(zn,s.GetHeap()); str+=s; return str; }
		friend CWStr operator + (const CWStr & s,double zn)				{ CWStr str(s,s.GetHeap()); str+=zn; return str; }
		friend CWStr operator + (double zn,const CWStr & s)				{ CWStr str(zn,8,s.GetHeap()); str+=s; return str; }
//		BASE_API friend CWStr operator + (void * zn,CWStr & s)						{ CWStr str(zn,s.m_Heap); str+=s; return str; }
//		BASE_API friend CWStr operator + (CWStr & s,void * zn)						{ s.Add(zn); return s; }

		//operator int (void) const											{ return GetInt(); }
		//operator double (void) const										{ return GetDouble(); }
        //lint -e1930
		operator const wchar * (void) const	    							{ return Get(); }
        //lint +e1930
};

__forceinline void CWStr::Tream(int len)
{
    if(m_Data->m_Len<len && len >= m_Data->m_Max)
    {
        m_Data->m_Len=len;
        m_Data->m_Max=len+16;
        m_Data=(CWStrData *)HAllocEx(m_Data,sizeof(CWStrData)+(len+16+1)*2,m_Data->m_Heap);
    } else
    {
        m_Data->m_Len=len;
    }
}

__forceinline void CWStr::NewDataLen(CHeap *heap, int len)
{
    m_Data=(CWStrData *)HAlloc(sizeof(CWStrData)+(len+16+1)*2,heap);
    m_Data->m_Heap=heap;
    m_Data->m_Refs=1;
    m_Data->m_Len=len;
    m_Data->m_Max=len+16;
}

__forceinline void CWStr::ModifyLen(CHeap *heap,int len)
{
	if(m_Data->m_Refs>1)
    {
		CWStrData * olddata=m_Data;
		olddata->RefDec();
		NewDataLen(heap,len);
		memcpy(m_Data->Data(),olddata->Data(),(min(len,olddata->m_Len)+1)*sizeof(wchar));
	} else
    {
		Tream(len);
	}
}

__forceinline void CWStr::ModifyLenNoCopy(CHeap *heap,int len)
{
	if(m_Data->m_Refs>1)
    {
		CWStrData * olddata=m_Data;
		olddata->RefDec();
		NewDataLen(heap,len);
	} else
    {
		Tream(len);
	}
}

}
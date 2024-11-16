#pragma once

#ifdef FILEPNG_DLL
	#ifdef FILEPNG_EXPORTS
		#define FILEPNG_API __declspec(dllexport)
	#else
		#define FILEPNG_API __declspec(dllimport)
	#endif
#else
	#define FILEPNG_API
#endif

// format: 1-gray 2-rgb 3-rgba 4-palate
FILEPNG_API DWORD _cdecl FilePNG_ReadStart_Buf(void * soubuf,DWORD soubuflen,DWORD * lenx,DWORD * leny,DWORD * countcolor,DWORD * format);
FILEPNG_API DWORD _cdecl FilePNG_Read(DWORD id,void * buf,DWORD lenline,DWORD * arraycolor);

// ���������� ������ ������ �����. ���� ������ bufoutlen �� ����� ������� ��������. ��� ������ 0
FILEPNG_API int _cdecl FilePNG_Write(void * bufout,int bufoutlen,void * buf,DWORD ll,DWORD lx,DWORD ly,DWORD bytepp,int rgb_to_bgr);

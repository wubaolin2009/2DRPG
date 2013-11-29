#ifndef _MY_CDRAW_H
#define _MY_CDRAW_H
#include <ddraw.h>
#include <dinput.h>
#include "cApp.h"
class CDraw
{
public:
	LPDIRECTDRAWSURFACE7 lpDDSPrimary;
	LPDIRECTDRAWSURFACE7 lpDDSBack;
	HWND hwnd;
	LPDIRECTDRAW7 lpDD;
	 DWORD DDColorMatch(LPDIRECTDRAWSURFACE7 pdds);

public:
	bool Transparent(LPDIRECTDRAWSURFACE7 lpsuftTo,int x,int y,LPDIRECTDRAWSURFACE7 lpsurfFrom,const RECT& rectTo,bool bNeedChange = true);
	LPDIRECTDRAWSURFACE7 GetBackBuffer(){return lpDDSBack;}
	CDraw()
	{
		lpDDSBack	    = NULL;
		lpDDSPrimary	= NULL;
		
	};
#ifdef WINDOW_DEBUG
	HRESULT UseClipper();
#endif
	bool Initialize(CApp& caApplication);
	bool CreateNULLSurface(LPDIRECTDRAWSURFACE7* lpSurf, DWORD dwWidth,DWORD dwHeight);  //创建空表面
	bool CreateBMPSurface(LPDIRECTDRAWSURFACE7* lpSurf, DWORD dwWidth,DWORD dwHeight,LPCWSTR filename,const RECT& rectToCopy,bool bNeedColorkey);  //创建位图表面
	bool BltToBack(DWORD dX,DWORD dY,LPDIRECTDRAWSURFACE7 lpSurf,RECT& rectToBlt,bool bUseSrcColorKey);
	void Render();
	~CDraw();
};

class CInput
{
private:
	LPDIRECTINPUT8           lpDirectInput;  // DirectInput object
	LPDIRECTINPUTDEVICE8    lpKeyboard;  // DirectInput device
	HINSTANCE hInstance;
	HWND hwnd;
	char diks[256];
public:
	CInput()
	{
		lpDirectInput = NULL;
		lpKeyboard    = NULL;
		hInstance    =  NULL;
		hwnd        = NULL;
	}
	~CInput()
	{
		if(lpKeyboard)	  
		{	
			lpKeyboard->Unacquire();
			lpKeyboard->Release(); 
		}
		if(lpDirectInput)  lpDirectInput->Release();
	}
	
	bool Initialize(HINSTANCE chInstance,HWND chwnd);
	bool Restore();
	bool IsReady(){return hwnd!=NULL;}
	bool IsPressed(DWORD dwKey);
	LPDIRECTINPUTDEVICE8 GetInput(){return lpKeyboard;}
	
};





#endif
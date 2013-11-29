#include "cDraw.h"


#ifdef WINDOW_DEBUG
HRESULT CDraw::UseClipper()
{
	HRESULT ddrval;
	LPDIRECTDRAWCLIPPER lpClipper = NULL;

	//����һ������������
	ddrval = lpDD->CreateClipper(0, &lpClipper, NULL);
	if ( ddrval != DD_OK){
		//����CreateClipper������������������������
		//��������Լ�¼������Ϣ
		return ddrval;
	}

	//���ü�������ΪhWndָ��Ĵ��ڵĿͻ����ķ�Χ
	ddrval = lpClipper->SetHWnd(0,hwnd);
	if ( ddrval != DD_OK){
		//����SetHWnd���ô��ھ�������������
		//��������Լ�¼������Ϣ
		return ddrval;
	}

	//���ü�������������
	ddrval = lpDDSPrimary->SetClipper(lpClipper);


	//�������Ѿ��Զ������˲ü���������ֵ,����Ӧ�����ͷ�
	if( lpClipper != NULL ) { 
		lpClipper -> Release();
		lpClipper = NULL; 
	} 

	return DD_OK;
}
#endif

bool CDraw::Initialize(CApp& caApplication)
{
	hwnd = caApplication.GetHWND();
	if(FAILED(DirectDrawCreateEx(NULL,(void **) &lpDD,IID_IDirectDraw7,NULL) ))
	{
		return  false;
	}
#ifndef WINDOW_DEBUG
	if( FAILED(lpDD->SetCooperativeLevel(hwnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT )) )
	{
		return false;
	}
	if(FAILED(lpDD->SetDisplayMode(640,480,32,NULL,NULL)) )
	{
		return false;
	}

	DDSURFACEDESC2 ddsd;

	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof( ddsd );
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
		DDSCAPS_FLIP |
		DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;


	if(FAILED(lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL )) )
		return false;

	DDSCAPS2 ddscaps;
	ZeroMemory(&ddscaps,sizeof(ddscaps));
	ddscaps.dwCaps=DDSCAPS_BACKBUFFER;

	if( FAILED(lpDDSPrimary->GetAttachedSurface(&ddscaps,&lpDDSBack))) return  false;
#else
	//����Э���ȼ�
	lpDD->SetCooperativeLevel(hwnd,DDSCL_NORMAL );

	//����������
	DDSURFACEDESC2 	ddsd;
	ZeroMemory( &ddsd, sizeof(ddsd) );
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	//���������棬��ȡ��ָ���������ָ��lpDDSPrimary
	lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
	
	//ʹ�òü���
	UseClipper();

	//������̨�������
	//:::::::ddsdǰ���Ѿ�����
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth  = 640;//��ķֱ���
	ddsd.dwHeight = 480;//�ߵķֱ���
	//����һ��̨�������
	lpDD->CreateSurface( &ddsd, &lpDDSBack, NULL );
#endif
	
	return true;
}

bool CDraw::BltToBack(DWORD dX,DWORD dY,LPDIRECTDRAWSURFACE7 lpSurf,RECT& rectToBlt,bool bUseSrcColorKey)
{
	HRESULT hr = 0;

	if(bUseSrcColorKey)
		hr=lpDDSBack->BltFast(dX,dY,lpSurf,&rectToBlt ,DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
		//hr = lpDDSBack->Blt(&rr,lpSurf,&rectToBlt,DDBLT_WAIT|DDBLT_KEYSRC,NULL);
	else 
		hr=lpDDSBack->BltFast(dX,dY,lpSurf,&rectToBlt ,DDBLTFAST_WAIT);
		//hr = lpDDSBack->Blt(&rr,lpSurf,&rectToBlt,DDBLT_WAIT,NULL);
	if(FAILED(hr)) return false;
	return true;
}

bool CDraw::CreateNULLSurface(LPDIRECTDRAWSURFACE7* lpSurf, DWORD dwWidth,DWORD dwHeight)
{
	DDSURFACEDESC2 ddsd;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize				= sizeof(ddsd);
	ddsd.dwFlags		    = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps     = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth		    = dwWidth;
	ddsd.dwHeight		    = dwHeight;
	if(FAILED(lpDD->CreateSurface( &ddsd, lpSurf, NULL)))
		return false;
	return true;
}


bool CDraw::CreateBMPSurface(LPDIRECTDRAWSURFACE7* lpSurf, DWORD dwWidth,DWORD dwHeight,LPCWSTR filename ,const RECT& rectToCopy,bool bNeedColorkey)
{

	CreateNULLSurface(lpSurf,rectToCopy.right - rectToCopy.left,rectToCopy.bottom-rectToCopy.top);
	HBITMAP hBitmap;
	hBitmap = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(hBitmap == NULL) return false;

	HDC hdcImage;
	HDC hdc;
	hdcImage = CreateCompatibleDC(NULL);
	if(hdcImage == NULL) return false;
	SelectObject(hdcImage, hBitmap);
	(*lpSurf)->GetDC(&hdc);

	if(BitBlt(hdc,rectToCopy.left,rectToCopy.top,rectToCopy.right,rectToCopy.bottom, hdcImage, 0, 0, SRCCOPY) == 0 )
	{
		(*lpSurf)->ReleaseDC(hdc);
		return false;
	}
	(*lpSurf)->ReleaseDC(hdc);
	DeleteDC(hdcImage);
	DeleteObject(hBitmap);

	if(bNeedColorkey)
	{
		DDCOLORKEY          ddck;
		ddck.dwColorSpaceLowValue  = DDColorMatch(*lpSurf);
		ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
		(*lpSurf)->SetColorKey(DDCKEY_SRCBLT, &ddck);
	}
	return true;
}

DWORD CDraw::DDColorMatch(LPDIRECTDRAWSURFACE7 pdds)
{
	if( pdds == NULL )
		return 0x00000000;

	DWORD dw = CLR_INVALID;
	DDSURFACEDESC2 ddsd;
	HRESULT hres;


	//---------��������ֱ�ӶԱ�����ж�����
	ddsd.dwSize = sizeof(ddsd);
	hres = pdds->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
	if (hres == DD_OK)
	{

		dw  = *(DWORD *)ddsd.lpSurface;


		if( ddsd.ddpfPixelFormat.dwRGBBitCount < 32 )
			dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)-1; 
		pdds->Unlock(NULL);
	}
	return dw;
}


void CDraw::Render()
{
	HRESULT hr;
	
#ifndef WINDOW_DEBUG
	
		hr= lpDDSPrimary->Flip(NULL,DDFLIP_WAIT);
#else
		RECT rectWin;
		GetClientRect( hwnd, &rectWin );
		if(lpDDSPrimary)
			hr = lpDDSPrimary->Blt(&rectWin,lpDDSBack,NULL,DDBLT_WAIT|DDBLTFAST_SRCCOLORKEY,NULL);
			
#endif
}
CDraw::~CDraw()
{
	if(lpDDSBack) lpDDSBack->Release();
	if(lpDDSPrimary) lpDDSPrimary->Release();
	if(lpDD) lpDD->Release();
}


bool CInput::Initialize(HINSTANCE chInstance,HWND chwnd)
{
	hInstance = chInstance;
	hwnd = chwnd;
	if FAILED( DirectInput8Create(hInstance, DIRECTINPUT_VERSION,IID_IDirectInput8, (void**)&lpDirectInput, NULL) )
	{
		return false;
	}

	if(FAILED (lpDirectInput->CreateDevice(GUID_SysKeyboard, &lpKeyboard, NULL) ))
		return false;
	if(FAILED(lpKeyboard->SetDataFormat(&c_dfDIKeyboard) )) 
		return false;
	if(FAILED(lpKeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
		return false;
	DIPROPDWORD     property;

	property.diph.dwSize = sizeof(DIPROPDWORD);
	property.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	property.diph.dwObj = 0;
	property.diph.dwHow = DIPH_DEVICE;
	property.dwData = 256;

	lpKeyboard->SetProperty(DIPROP_BUFFERSIZE, &property.diph);
	lpKeyboard->Acquire();
	return true;
}

bool CInput::Restore()
{
	if( FAILED(lpKeyboard->Acquire() ) ) return false;
	return true;
}

bool CInput::IsPressed(DWORD dwKey)
{
	lpKeyboard->Acquire();
	lpKeyboard->GetDeviceState(sizeof(diks), (void*)diks);
	if(diks[dwKey]&0x80) 
	{
		diks[dwKey] = 0x00;
		return true;
	}
	return false;
}


bool CDraw::Transparent(LPDIRECTDRAWSURFACE7 lpsuftTo,int x,int y,LPDIRECTDRAWSURFACE7 lpsurfFrom,const RECT& rectTo,bool bNeedChange)
 {
	DDSURFACEDESC2 ddsd, ddsd2; //DirectDrawҳ������
	ZeroMemory(&ddsd, sizeof(ddsd)); //ddsd��ǰҪ���
	ddsd.dwSize = sizeof(ddsd); //DirectDraw�еĶ���Ҫ����
	ZeroMemory(&ddsd2, sizeof(ddsd2));
	ddsd2.dwSize = sizeof(ddsd2);
	lpsuftTo->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL); //Lock!
	lpsurfFrom->Lock(NULL, &ddsd2, DDLOCK_WAIT, NULL);
	int pointCol = (rectTo.right - rectTo.left);

	BYTE *Bitmap = (BYTE*)ddsd.lpSurface; //Lock��ҳ�����Ϣ�����������ע��
	//���ָ�����ÿ��Lock( )�󶼲�ͬ��
	BYTE *Bitmap2 = (BYTE*)ddsd2.lpSurface;

	int pos;
	int pos2;
	int iLine = 0;
	for (int yy=y;yy<y+rectTo.bottom-rectTo.top; yy++,iLine++)
	{
		pos=yy*ddsd.lPitch+x*4;
		pos2 = iLine*ddsd2.lPitch;
		for (int xx=x; xx<x+rectTo.right - rectTo.left; xx++)
		{
			pos++;
			pos2++;	
			if(bNeedChange)
				Bitmap[pos] =(Bitmap[pos]*(xx-x)+Bitmap2[pos2]*(pointCol-xx+x))/pointCol; //��R
			else
				Bitmap[pos] =(Bitmap[pos]*10+Bitmap2[pos2]*10)/20; //��R

			pos++;
			pos2++;	
			if(bNeedChange)
				Bitmap[pos] =(Bitmap[pos]*(xx-x)+Bitmap2[pos2]*(pointCol-xx+x))/pointCol; //��R
			else
				Bitmap[pos] =(Bitmap[pos]*10+Bitmap2[pos2]*10)/20; //��R

			pos++;
			pos2++;
			if(bNeedChange)
				Bitmap[pos] =(Bitmap[pos]*(xx-x)+Bitmap2[pos2]*(pointCol-xx+x))/pointCol; //��R
			else
				Bitmap[pos] =(Bitmap[pos]*10+Bitmap2[pos2]*10)/20; //��R

			pos++;//����һ��R��
			pos2++;
		}
	}
	lpsurfFrom->Unlock(NULL); //Unlock!
	lpsuftTo->Unlock(NULL);
	return true;
}


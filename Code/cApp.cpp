/*    CApp.cpp */
/* CAppwindows 窗口框架类的实现    */
#include"CApp.h"

CApp::CApp()
{
	hwnd = NULL;
	bIsOk = false;
};

bool CApp::Initilize(HINSTANCE phInstance,LPCWSTR wcWndClassName,WNDPROC wndproc)
{
	hInstance			  = phInstance;
	wndclass.style        = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc  =  wndproc;
	wndclass.cbClsExtra   = 0;
	wndclass.cbWndExtra   = 0;
	wndclass.hInstance    = hInstance;
	wndclass.hIcon        = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor      = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName= wcWndClassName;
	if ( !RegisterClass(&wndclass) )  //注册窗口类
	{
		bIsOk = false;
		return 0;
	}
	bIsOk = true;
#ifndef WINDOW_DEBUG
	hwnd = CreateWindow( wndclass.lpszClassName, // window class name
		TEXT ("The Hello Program"),   // window caption
		WS_POPUP,  // window style
		0,// initial x position
		0,// initial y position
		640,480,
		NULL,  // parent window handle
		NULL,  // window menu handle
		hInstance,  // program instance handle
		NULL); // creation parameters
#else
	hwnd = CreateWindowExW(
		0,
		wndclass.lpszClassName,
		TEXT("HELLO"),
		WS_OVERLAPPEDWINDOW,
		0,0,640,480,
		NULL,
		NULL,
		hInstance,
		NULL );
#endif

	if(hwnd == 0) return 0;
	ShowWindow (hwnd, SW_NORMAL);
	UpdateWindow (hwnd);
	bIsActive = true;
	return 1;
}

HWND CApp::GetHWND()
{
	return hwnd;
};

CApp::~CApp()
{
	if(bIsOk)
	UnregisterClass(wndclass.lpszClassName,hInstance);
}







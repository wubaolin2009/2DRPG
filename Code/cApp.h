/*   cApp.h    */
/* 窗口框架类 */

#ifndef _MY_CAPP_H
#define _MY_CAPP_H
#include <Windows.h>

class CApp
{
private:
	HWND hwnd;           //窗口句柄
	WNDCLASS wndclass;   //窗口类型
	bool bIsActive;
	HINSTANCE hInstance; //实例句柄
	bool bIsOk;	//判断是否被初始化
	
public:
	
	CApp();
	~CApp();
	bool Initilize(HINSTANCE phInstance,LPCWSTR wcWndClassName,WNDPROC wndproc);  //初始化窗口类
	HWND GetHWND();//得到当前窗口句柄
	bool IsActive()
	{
		return bIsActive;
	}
	HINSTANCE GetHInstace()
	{
		return hInstance;
	}
};

#endif
/*   cApp.h    */
/* ���ڿ���� */

#ifndef _MY_CAPP_H
#define _MY_CAPP_H
#include <Windows.h>

class CApp
{
private:
	HWND hwnd;           //���ھ��
	WNDCLASS wndclass;   //��������
	bool bIsActive;
	HINSTANCE hInstance; //ʵ�����
	bool bIsOk;	//�ж��Ƿ񱻳�ʼ��
	
public:
	
	CApp();
	~CApp();
	bool Initilize(HINSTANCE phInstance,LPCWSTR wcWndClassName,WNDPROC wndproc);  //��ʼ��������
	HWND GetHWND();//�õ���ǰ���ھ��
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
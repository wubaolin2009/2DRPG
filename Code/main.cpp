#include<Windows.h>
#include<Mmsystem.h>

#include "midi.h"
#include "cApp.h"
#include "cDraw.h"
#include "cobject.h"
#include "CDb.h"





LPDIRECTDRAWSURFACE7 lpppp;
CApp  Mycapp;
CDraw w;
CMidi midi;
CSceneManager psmanger(NULL);
CInput *pcinput;
HWND hwnd;
HINSTANCE ghInstance;


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int  timer = 0; //计时器 
	static int   timer2 = 0;
	switch (message)
	{
	case WM_CREATE:
		SetTimer(hwnd,0,150,NULL);
		return 0;
	case WM_TIMER:
		
		if(psmanger.GetRegisteredNum()!=0)
		{
			
			if(psmanger.GetStatus() == WALK)
			{
				timer++;
				if(timer%2==0)
				{
					psmanger.RecieveMoveAllNpc();
					timer=0;
				}
			}
			if(psmanger.GetStatus() == TALK)
			{
				timer2++;
				if(timer2 %10 ==0)
				{
					psmanger.RecieveUpdataDialog();
					timer2 =0;
				}
			}
			if(psmanger.GetStatus() == FIGHT)
			{
				psmanger.UpdateScene(hwnd);
				
			}					
		}
						
		return 0;
	case  WM_DESTROY:
		KillTimer(hwnd,0);
		PostQuitMessage (0) ;
		return 0 ;

	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}


int WINAPI WinMain  ( HINSTANCE hInstance,  HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
{
	CoInitialize(NULL);

	Mycapp.Initilize(hInstance,TEXT("MY"),WndProc);
	hwnd = Mycapp.GetHWND();
	ghInstance = hInstance;

	w.Initialize(Mycapp);

	CInput cinput;
	cinput.Initialize(hInstance,Mycapp.GetHWND());
	pcinput = &cinput;
	RECT _rect ={0,0,640,480};

	PlaySound(TEXT("melody of life.wav"),hInstance,SND_LOOP|SND_FILENAME|SND_ASYNC );

	LeadReader r;
	Attribute attr = r.GetAttr();
	CLead  lead(&w,attr);
	lead.Initialize();

	CWeaponReader cwR;
	CSceneReader cr;
	vector<WeaponInfo> vecWeapon = cwR.GetWeapons();
	CWeapons Weapon(&w);
	Weapon.SetVec(vecWeapon);
	psmanger.SetWeapon(&Weapon);
	psmanger.SetDraw(&w);
	psmanger.SetOverImage(TEXT("gameover.bmp"));

	

	

	CSceneInit csi;
	CScene* pscene = csi.InitScene("4096",&lead,&w,pcinput);
	CScene* pscene2 = csi.InitScene("4097",&lead,&w,pcinput);
	CScene* pscene3 = csi.InitScene("4098",&lead,&w,pcinput);
	CScene* pscene4 = csi.InitScene("4099",&lead,&w,pcinput);
	CScene* pscene5 = csi.InitScene("4100",&lead,&w,pcinput);
	CPathReader cpr;
	psmanger.SetPaths(cpr.GetPaths());

	psmanger.RegisterScene(pscene);
	psmanger.RegisterScene(pscene2);
	psmanger.RegisterScene(pscene3);
	psmanger.RegisterScene(pscene4);
	psmanger.RegisterScene(pscene5);
	CSceneReader sr;
	SceneDesc scenedesc = sr.GetDesc("4097");
	scenedesc.lpImage = NULL;
	scenedesc.pCDraw = &w;
	scenedesc.pcInput = &cinput;	
	scenedesc.lpwFileName = TEXT("4.bmp");
	scenedesc.bIsFightable = true;
	CMyMenu menuRectVictory(TEXT("6.bmp"),pcinput,&w,250,400);
	CMyMenu menuSelect(TEXT("6.bmp"),pcinput,&w,250,200);
	menuSelect.Initialize();
	menuSelect.AddItem(TEXT("Attack!"));
	menuSelect.AddItem(TEXT("使用药水:10个"));
	CMyMenu menuStatus(TEXT("6.bmp"),pcinput,&w,250,200);
	menuStatus.Initialize();
	menuStatus.AddItem(TEXT("主  角 H P:"));
	menuStatus.AddItem(TEXT("主角攻击力:"));
	menuStatus.AddItem(TEXT("主角防御力:"));
	menuRectVictory.Initialize();
	CFightScene csceneFight(scenedesc,&menuRectVictory,&menuSelect,TEXT("bomb.bmp"),&menuStatus);

	CMyMenu menuSelectBuy(TEXT("6.bmp"),pcinput,&w,250,400);
	menuSelectBuy.Initialize();
	cwR.FillItems( vecWeapon,menuSelectBuy);

	CSellScene css(scenedesc,&menuSelectBuy);
	css.Initialize();
	css.SetWeapons(&Weapon);
	css.RegisterLead(&lead);
	
	psmanger.SetSellScene(&css);
	csceneFight.Initialize();
	psmanger.SetFightScene(&csceneFight);
	

#ifndef WINDOW_DEBUG
	psmanger.Draw();

#else
		psmanger.Draw(Mycapp.GetHWND());

#endif
	MSG  msg;	
	msg.message = WM_NULL;
	for(;;)
	{	if(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);		
	}
	if(msg.message == WM_QUIT) break;
	else
	{


#ifndef WINDOW_DEBUG
			psmanger.Draw();
#else
			psmanger.Draw(Mycapp.GetHWND());
#endif
	
	}
	}
	
	CoUninitialize();
	return msg.message;

}













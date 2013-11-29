/* 关于游戏中各种信息的类 */

#ifndef _MY_COBJECT_BASE_CLASS
#define _MY_COBJECT_BASE_CLASS
#include <ddraw.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include "cDraw.h"
#include "midi.h"

using std::vector;
using std::string;
using std::wstring;
using std::map;

class CPathReader;
class CSceneManager;
class CMenuRect;
class CMyMenu;
class CObject
{
protected:
	CDraw* pCDraw; 
	LPDIRECTDRAWSURFACE7 lpImage;  //位图表面
	wstring lpwFileName;   //位图的文件名
	int iNumRect;        //位图的矩形个数
	DWORD dwID;			//编号
	DWORD dwWidth;	    //位图宽度
	DWORD dwHeight;    //位图高度
public:
#ifndef WINDOW_DEBUG
	virtual void draw() =0;
	
#else
	virtual void draw(HWND hwnd)=0;
	
#endif
	//virtual void Release() = 0;
	DWORD GetID(){return dwID;};   //获得ID
	CObject(const CDraw* ccdraw,DWORD dwIID,wstring filename,int ciNumRect, DWORD cdwWidth,DWORD cdwHeight)
	{
		lpImage = NULL;
		lpwFileName = filename;
		iNumRect = ciNumRect;
		dwID = dwIID;
		dwWidth = cdwWidth;
		dwHeight = cdwHeight;
		pCDraw = const_cast<CDraw*>(ccdraw);
	};
	virtual ~CObject(){};
};

struct WeaponInfo          //武器描述信息
{
	int iID;         //武器编号
	int iAttack;     //武器所增加的Attack iRateCritical
	int iRateCritical;
	int iMoney;      //价钱
	RECT rect;
	wstring wstrDesc;
	wstring wstrName;
	bool operator==(WeaponInfo w2)
	{
		return iID == w2.iID;
	}
};

class CWeapons:public CObject   //武器类 
{
public:
	CWeapons(const CDraw* ccdraw):CObject(ccdraw,0x0000,TEXT("goods.bmp"),1,384,192)
	{
	};
	void SetVec(vector<WeaponInfo> vec)
	{
		vecWeapon = vec;
	}
	WeaponInfo operator[](int iIndex)
	{
		return vecWeapon[iIndex];
	}
	
private:
#ifndef WINDOW_DEBUG
	virtual void draw() {};

#else
	virtual void draw(HWND hwnd){};

#endif
	vector<WeaponInfo> vecWeapon;   //武器的种类 和 每种武器的信息
};


struct _sCharacte           //人物的战斗部分相关信息
{
	int iAttack;
	int iDefence;
	int iRateCritical;
	int  iHP;
	int  iLevel;
	int  iExp;
	int  iNeedExp;
	int  iMaxHP;
};

typedef struct  _sCharacte Information;



struct _sInfoCharater   //人物描述信息
{
	wstring ImageFileName;  //人物图像文件
	int INumRect;			//每种走路几个图
	vector<RECT> vecRect;	//每种方向走路所对应的
	DWORD dwID;
	wstring strName;
	DWORD dwScene;
	RECT loadRect;
	Information Info;
	int dwWidth;
	int dwHeight;
	vector<wstring> vecTalk;
	int      iExpKill;
	bool bIsSell;
	bool bIsMove;
};

typedef struct _sInfoCharater Attribute;



class CFigure:public CObject
{
protected:
	wstring strName;
	vector<RECT> vecRect;
	DWORD dwSceneID;
	RECT rectPosition;
	int bWalkState;       //切换人物走路动作
	Information information;
	int iDirection;      //上下左右  0123
	bool bIsSell;
	//	CScene* pScene;
public:
	DWORD GetSceneID() {return dwSceneID;};
	void SetRect(const RECT& rect){rectPosition = rect;}
	Information& GetInformation(){return information;}
	void MoveTo(RECT rectPosition);
	RECT GetRect();
	
	int GetDirection(){return iDirection;}
	void Move(DWORD dwmove,int iStep = 3);
	bool Initialize();
	void SetDirection(int dirction){iDirection = dirction;}
	bool WasBeatenBy(CFigure* fighter,HWND hwnd = 0);  //0 没事 1结束 

#ifndef WINDOW_DEBUG
	virtual void draw();

#else
	virtual void draw(HWND hwnd);
#endif
	//virtual void Release() = 0;
	virtual ~CFigure(){};

	CFigure(const CDraw* pcdraw,wstring sName,DWORD cdwsceneID,RECT rrectPosition,const Information info,const Attribute& attr)
		:CObject(pcdraw,attr.dwID,attr.ImageFileName,attr.INumRect,attr.dwWidth,attr.dwHeight)
	{
		strName = sName;
		dwSceneID = cdwsceneID;
		rectPosition = rrectPosition;
		bWalkState = 0;
		vecRect = attr.vecRect;
		information = info;
		iDirection = 1;
		bIsSell = attr.bIsSell;
	}
};

enum EState{NORMAL,DIALOG,ATTACK};

class CWeaponComp
{
public:
	bool operator()(WeaponInfo w1,WeaponInfo w2)
	{
		return w1.iID < w2.iID;
	}
};

class CLead:public CFigure
{
private:
	EState Estatt;
	map<WeaponInfo,int,CWeaponComp> mapWeapon;
	int iMoney;
	int iDrugNum;
public:
	const map<WeaponInfo,int,CWeaponComp>& GetWeapon()
	{
		return mapWeapon;
	}
	void SetSceneID(DWORD dwID){dwSceneID = dwID;}
	friend class CScene;
	CLead(const CDraw* pcdraw,const Attribute& attr)
		:CFigure(pcdraw,attr.strName,attr.dwScene,attr.loadRect,attr.Info,attr)
	{
		Estatt = NORMAL;
		iMoney = 3000;
		iDrugNum = 10;
	}
	int GetMoney(){return iMoney;}
	void AddMoney(int iMoney1) {iMoney += iMoney1;}
	void Buy(WeaponInfo);
	void Equip(WeaponInfo wpInfo);
	int GetDrugNum(){return iDrugNum;};
	void SetDrugNum(int i){iDrugNum = i;};

	~CLead();			
};

class CNpc:public CFigure
{
private:
	vector<wstring>  vecTalkMessage;
	int iExpKill;
	bool bIsMove;
public:
	friend class CScene;
	int GetExpKill()
	{
		return iExpKill;
	}
	bool GetIsMove(){return bIsMove;};
	CNpc(const CDraw* pcdraw,const Attribute& attr)
		:CFigure(pcdraw,attr.strName,attr.dwScene,attr.loadRect ,attr.Info,attr)
	{
		vecTalkMessage = attr.vecTalk;
		iExpKill = attr.iExpKill;
		bIsMove = attr.bIsMove;
	}
	
	//virtual void ShowDialog(){};
	~CNpc();
	
};

typedef struct tagRoleList
{
	CLead* leadrole;
	vector<CNpc*> Npcs;
}RoleList;

struct _SceneDesc
{
	CDraw* pCDraw;
	LPDIRECTDRAWSURFACE7 lpImage;
	wstring lpwFileName;
	int iNumRect;
	vector<RECT> vecRect;
	DWORD dwID;
	DWORD dwWidth;
	DWORD dwHeight;
	wstring wstrMidiName;
	wstring lpwDialogBmp;
	CInput *			pcInput;
	RECT LeadLoadPosition;
	bool bIsFightable;
	vector<vector<int> > vecmap;
	vector<RECT> vecmapimage;
	vector<vector<int> > veccollision;
	bool bIswholeimage;
	int iNumLine;
	int iNumCol;
	//vector<LPCWSTR> vecRain;
};
typedef _SceneDesc SceneDesc;


enum Status {WALK,TALK,CHANGE,FIGHT,SELL,GAMEOVER};

class CScene:public CObject
{
	friend class CSceneManager;

public:
	DWORD GetSceneID(){return rolelist.leadrole->dwSceneID;}
	void SetStatus(Status status){gamestate=status;}
	
	CScene(SceneDesc desc)
		:CObject(desc.pCDraw,desc.dwID,desc.lpwFileName,desc.iNumRect,desc.dwWidth,desc.dwHeight)
	{
		rolelist.leadrole = NULL;
		//pCsceneManger = NULL:
		
		iNumLine = desc.iNumLine;
		iNumColumn = desc.iNumCol;
		if(!desc.bIswholeimage)
			vecMap = desc.vecmap;
		bIsWholeMap = desc.bIswholeimage;
		
		vecMapImage = desc.vecmapimage;
		lpImageBuffer = NULL;
		vecCollision = desc.veccollision;
		wstrMidiName = desc.wstrMidiName;
		gamestate = WALK;
		FocusID = 0xffff;
		iDialogNow = 0;
		lpwDialogBmp = desc.lpwDialogBmp;
		pcInput=desc.pcInput;
		rectLeadLoadPosition = desc.LeadLoadPosition;
		bIsFightable = desc.bIsFightable;
		//vecRainFile = desc.vecRain;
		

	}
	RoleList& ReturnRoleList(){return rolelist;}
	Status GetStatus(){return gamestate;}
	DWORD NearNpcID(const RECT& rectWant,DWORD dwID,int iDirection);           //返回离某一个figure最近的figure的ID 0xffff为什么没找到
	const RECT&  ReturnLeadRect(){return rectLeadLoadPosition;}
	virtual bool Initialize();
	void ShowDialog();
	bool CheckNpcMove(DWORD dwID,RECT rectPosition);
	bool CheckLeadMove(const RECT& rectmove);
	bool CheckMoveCollision(const RECT& rect,int iDirection);
	bool RegisterLead(CLead* pleadRole);
	bool RegisterNpc(CNpc* pNpcRole);
	void MoveAllNpc();
	void UpdateDialog(){iDialogNow++;};
	void SetManger(CSceneManager* pcSmanger)
	{
		pCsceneManger = pcSmanger;
	}

	
	~CScene();
	//void Release();
	void MoveRole(DWORD dwID,DWORD dwMove,int iStep = 2);
#ifndef WINDOW_DEBUG
	void draw();
#else
	void draw(HWND hwnd);
#endif
public:
		vector<LPCWSTR> vecRainFile;
protected:

	//vector<LPDIRECTDRAWSURFACE7> vecRain;
	bool bIsWholeMap;    //是不是整张地图
	CSceneManager* pCsceneManger;
	RECT rectLeadLoadPosition;
	CInput *			pcInput;
	wstring             lpwDialogBmp;
	LPDIRECTDRAWSURFACE7 lpDialog;
	Status gamestate;
	DWORD FocusID;   //当前焦点对象
	int   iDialogNow; //当前对话框的编号
	static bool IsRectCross(const RECT& rect1,const RECT& rect2);
	RoleList rolelist;
	LPDIRECTDRAWSURFACE7 lpImageBuffer;
	int iNumLine;
	int iNumColumn;
	vector<vector<int> > vecMap;             //地图矩阵
	vector<vector<int> > vecCollision;			//碰撞检测矩阵
	vector<RECT> vecMapImage;          //载入位图的矩阵
	wstring wstrMidiName;
	bool bIsFightable;
	

};

class CFightScene:public CScene
{
public:
	CFightScene(SceneDesc desc,CMyMenu* pMenuVictory,CMyMenu* pmenu,wstring strFileExplode,CMyMenu* pstatus = NULL):
	  CScene(desc){bIsFighting = false;turn = NONE;bIsAttacking = true;pMenuRect = pMenuVictory;pMenu = pmenu;pStatus = pstatus;
	  RECT rect = {0,0,540,50};
	  pCDraw->CreateBMPSurface(&lpsurtExplode,540,50,strFileExplode.c_str(),rect,true);};
	  enum TURN{MY,ENEMY,NONE};
#ifdef WINDOW_DEBUG  
	  virtual void draw(HWND hwnd);
#else
	  virtual void draw();
#endif
	void UpdateScene(HWND hwnd = 0);
	void FightEnd();
	void HpUp();
	void ShowEffect(RECT rectToDraw);
	void LevelUp();
	~CFightScene()
	{
	
	}
private:
	bool bIsFighting;
	bool bIsAttacking;
	LPDIRECTDRAWSURFACE7 lpsurtExplode;
	TURN turn;
	CMyMenu    * pMenuRect;
	CMyMenu    * pMenu;
	CMyMenu    * pStatus;

};




class CSellScene:public CScene
{
public:
	CSellScene(SceneDesc desc,CMyMenu* pMenu):CScene(desc)
	{
	  pMyMenu = pMenu;
	};
	
#ifdef WINDOW_DEBUG  
	  virtual void draw(HWND hwnd);
#else
	  virtual void draw();
#endif
	  virtual bool Initialize();
	  void DrawPic();
	  void Buy();
	  void SetPSceneManger(CSceneManager* csm){pCsceneManger = csm;};
	  void SetWeapons(CWeapons* cwp) {pCWeapon = cwp;}
	  void DrawWeaponInfo();
private:
	CMyMenu* pMyMenu;
	CSceneManager* pCsceneManger;
	CWeapons*  pCWeapon;
};


struct CScenePath
{
	int iSceneFrom;
	RECT rectFrom;
	RECT rectTo;
	int  iSceneToID;
};


class CSceneManager
{
friend class CScene;
friend class CSellScene;
public:
	CSceneManager(CFightScene* FightScene)
	{iSceneNum = 0;iRegisteredNum = 0;
	sceneFight = FightScene;
	mstatus = WALK;
	};
	void SetPaths(vector<CScenePath> vcs)
	{
		vecScenepaths = vcs;
	}
	void RecieveMoveAllNpc();
	void RecieveUpdataDialog();
	Status GetStatus();
	void ReturnToNormal(CLead* lead,CNpc* npc);
	void SetDraw(CDraw* pCdraw)
	{
		pCDraw = pCdraw;
	}
	void SetOverImage(wstring wstrFileName)
	{
		RECT rect = {0,0,640,480};
		pCDraw->CreateBMPSurface(&lpGameOver,640,480,wstrFileName.c_str(),rect,false);
		
	}
	void GameOver();


#ifndef WINDOW_DEBUG
	void Draw();
#else
	void Draw(HWND hwnd);
#endif
	bool RegisterScene(CScene* pCscene);
	int GetSceneNum(){return iSceneNum;}
	int GetRegisteredNum(){return iRegisteredNum;};
	void SetFightScene(CFightScene* Fight){sceneFight = Fight;sceneFight->SetManger(this);}
	void SetSellScene(CSellScene* Sell){sceneSell = Sell;sceneSell->SetPSceneManger(this);}
    void SetStatus(Status status)
	{
		vecScene[iSceneNum]->SetStatus(status);
	}

	void SetStatusM(Status status)
	{
		mstatus = status;

	}

	void ClearFightScene();
	void UpdateScene(HWND hwnd = 0);
	void SetWeapon(CWeapons* cWeap){pCweapon = cWeap;}
	~CSceneManager()
	{

		for(int i = 0;i<(int)vecScene.size();i++)
		{
			if(vecScene[i])
			{
				delete vecScene[i];
				vecScene[i] = NULL;
			}

		}
	}
	vector<CScenePath> GetRectToChangeScene(int iSceneID);

	//void Fight();

private:
	Status mstatus;
	LPDIRECTDRAWSURFACE7 lpGameOver;
	CDraw* pCDraw;
	vector<CScenePath> vecScenepaths;
	CWeapons* pCweapon;
	int iSceneNum;
	int iRegisteredNum;
	vector<CScene*> vecScene;
	CFightScene* sceneFight;
	CSellScene*  sceneSell;
	RECT rectLeadRoleToSave;	
};


class CMenuRect
{
protected:
	CDraw* pCDraw;
	LPDIRECTDRAWSURFACE7 lpImage;
	LPCWSTR lpwFileName;
	int     iBoundsX;
	int     iBoundsY;
	RECT    rectToDraw;
	CInput *	pcInput;
public:
	CMenuRect(LPCWSTR filename,CInput* pcinput = NULL,CDraw* draw = NULL,int boundx = 0,int boundy = 0)
	{
		pCDraw = draw;
		lpImage = NULL;
		lpwFileName  = filename;
		pcInput  = pcinput;
		iBoundsX = boundx;
		iBoundsY = boundy;
	}
	
	void Initialize();
	void SetInput(CInput* input)
	{
		pcInput = input;
	}
	void SetDraw(CDraw* draw)
	{
		pCDraw = draw;
	}
	virtual void Draw(LPCWSTR TextToDraw,const RECT& rectDraw);
	virtual ~CMenuRect()
	{
		if(lpImage)
		{
			lpImage->Release();
			lpImage = NULL;
		}
	}
};

class CMyMenu : public CMenuRect
{
private:
	int iItemCount;
	int iCurrentItem;
	vector<wstring> wstrItems;
public:
	CMyMenu(LPCWSTR filename,CInput* pcinput = NULL,CDraw* draw = NULL,int boundx = 0,int boundy = 0):CMenuRect(filename,pcinput,draw,boundx,boundy)
	{
		iItemCount = 0;
		iCurrentItem = -1;
	}
	void SetItemValue(int iIndex,wstring wstrValue)
	{
		wstrItems[iIndex] = wstrValue;

	}
	void AddItem(wstring strItemText);
	void ClearItem()
	{
		if(wstrItems.size()>0)
		{
			wstrItems.clear();
			iItemCount = 0;
			iCurrentItem = -1;
		}
	}
	
	virtual void Draw(const RECT& rectDraw,DWORD dwKey,bool bNeedChange = true);
	int GetStatus(){return iCurrentItem;}
};

#endif


#include <Cstdlib>
#include <Ctime>
#include "cobject.h"

#include "cDraw.h"
extern CMidi midi;
extern HWND hwnd;
extern HINSTANCE ghInstance;


bool CFigure::Initialize()
{
	RECT reRect = {0,0,dwWidth,dwHeight};
	if(pCDraw->CreateBMPSurface(&lpImage,dwWidth,dwHeight, lpwFileName.c_str(),reRect,true) == false)
		return false;
	return true;
}

void CFigure::Move(DWORD dwmove,int iStep)
{
	static int timer = 0;

	if(dwmove == DIK_DOWN)
	{
		rectPosition.top+=iStep;
		rectPosition.bottom+=iStep;
	}
	if(dwmove == DIK_UP)
	{
		rectPosition.top-=iStep;
		rectPosition.bottom-=iStep;
	}
	if(dwmove == DIK_LEFT)
	{
		rectPosition.left-=iStep;
		rectPosition.right-=iStep;
	}
	if(dwmove == DIK_RIGHT)
	{
		rectPosition.left+=iStep;
		rectPosition.right+=iStep;
	}
	timer++;
	if(timer%10 == 9)
	{bWalkState++;
		if(bWalkState>=iNumRect)
			bWalkState = 0;
		timer = 0;
	}
}


RECT CFigure::GetRect()
{
	return rectPosition;
}

#ifndef WINDOW_DEBUG
void CFigure::draw()
{
	if(!lpImage) return;
	pCDraw->BltToBack(rectPosition.left,rectPosition.top-16,lpImage,vecRect[iDirection*iNumRect + bWalkState],true);
}



#else
void CFigure::draw(HWND hwnd)
{
	RECT rectNow;
	GetClientRect(hwnd,&rectNow);
	POINT point = {rectNow.left,rectNow.top};
	ClientToScreen(hwnd,&point);
	if(!lpImage) return;
	pCDraw->BltToBack(point.x+rectPosition.left,point.y+rectPosition.top-16,lpImage,vecRect[iDirection*iNumRect + bWalkState],true);
}
#endif
bool CScene::Initialize()
{
	RECT rect = {0,0,32*iNumColumn,32*iNumLine};
	if(!pCDraw->CreateBMPSurface(&lpImageBuffer,dwWidth,dwHeight,lpwFileName.c_str(),rect,false))
		return false;

	pCDraw->CreateNULLSurface(&lpImage,rect.right,rect.bottom);
	if(!bIsWholeMap)
	{
		for(int i = 0;i!=vecMap.size();i++)
		{
			for(int j = 0;j!= vecMap[0].size();j++)
			{
				int iNow = vecMap[i][j];
				lpImage->BltFast(j*32,i*32,lpImageBuffer,&vecMapImage[iNow],DDBLTFAST_WAIT);
			}
		}
	}
	else  
		lpImage->BltFast(0,0,lpImageBuffer,&rect,DDBLTFAST_WAIT);
	
	/* DiaLog    */
	RECT rectBmpDialog = {0,0,500,100};
	RECT rectBmpRain = {0,0,640,480};
	pCDraw->CreateBMPSurface(&lpDialog,500,100,lpwDialogBmp.c_str(),rectBmpDialog,false);
	
	return true;

}


#ifndef WINDOW_DEBUG
void CScene::draw()
{
	if(gamestate == WALK)
	{	if(pcInput->IsPressed(DIK_DOWN))
	MoveRole(0,DIK_DOWN);
	if(pcInput->IsPressed(DIK_UP))
		MoveRole(0,DIK_UP);
	if(pcInput->IsPressed(DIK_LEFT))
		MoveRole(0,DIK_LEFT);
	if(pcInput->IsPressed(DIK_RIGHT))
		MoveRole(0,DIK_RIGHT);
	}
	if(pcInput->IsPressed(DIK_SPACE))   //检测是不是有事件发生
		ShowDialog();			
	if(lpImage==NULL) return ;
	RECT rectN;
	rectN.bottom=iNumLine * 32;
	rectN.top=0;
	rectN.left=0;
	rectN.right=iNumColumn*32;
	pCDraw->BltToBack(0,0,lpImage,rectN,false);

	
	for(int i=0;i!=rolelist.Npcs.size(); i++)
		rolelist.Npcs[i]->draw();
	rolelist.leadrole->draw();
	if(gamestate == TALK )    //画对话框
	{
		if(iDialogNow == rolelist.Npcs[FocusID&0x000f]->vecTalkMessage.size())   //到最后了
		{
			if(bIsFightable)   //开打
			{
				gamestate = FIGHT;
				pCsceneManger->rectLeadRoleToSave = rolelist.leadrole->rectPosition;
				RECT rectToload = {100,200,132,232};
				rolelist.Npcs[FocusID&0x000f]->SetRect(rectToload);
				rolelist.Npcs[FocusID&0x000f]->SetDirection(3);

				rectToload.left +=400;
				rectToload.right+=400;
				rolelist.leadrole->SetRect(rectToload);		
				rolelist.leadrole->SetDirection(2);
				pCsceneManger->sceneFight->RegisterLead(rolelist.leadrole);
				pCsceneManger->sceneFight->RegisterNpc(rolelist.Npcs[FocusID&0x000f]);
			}
			else if(rolelist.Npcs[FocusID&0x000f]->bIsSell == true)
			{
				gamestate = SELL;
			}
			else
				gamestate = WALK;
			iDialogNow = 0;
			if(pCDraw)
				pCDraw->Render();
			return;
		}
		RECT rectdraw = {0,0,500,100};
		pCDraw->Transparent(pCDraw->GetBackBuffer(),60,340,lpDialog,rectdraw);


		HDC hdc;
		pCDraw->GetBackBuffer()->GetDC(&hdc);
		SetBkMode(hdc,TRANSPARENT);   
		SetTextColor(hdc,   RGB(0,0,0));   //设置文字颜色  
		HFONT hfont = CreateFont(15,0,0,0,400,0,0,0,DEFAULT_CHARSET,OUT_TT_PRECIS,CLIP_TT_ALWAYS,     
			PROOF_QUALITY   ,DEFAULT_PITCH,TEXT("隶书"));
		SelectObject(hdc,hfont);

		LPCWSTR lpwWordNow = rolelist.Npcs[FocusID&0x000f]->vecTalkMessage[iDialogNow].c_str();   //当前人物的对话 
		int iStrLength = wcslen(lpwWordNow);
		TextOut(hdc,120,380,lpwWordNow,iStrLength);   //句柄,   左上角X,   左上角Y,
		//文字(char   *),   文字长度   
		pCDraw->GetBackBuffer()->ReleaseDC(hdc);   //释放句柄 
	}

		if(pCDraw)
			pCDraw->Render();
}
#else
void CScene::draw(HWND hwnd)
{

	if(gamestate == WALK)
	{
		if(pcInput->IsPressed(DIK_DOWN))
			MoveRole(0,DIK_DOWN);
		if(pcInput->IsPressed(DIK_UP))
			MoveRole(0,DIK_UP);
		if(pcInput->IsPressed(DIK_LEFT))
			MoveRole(0,DIK_LEFT);
		if(pcInput->IsPressed(DIK_RIGHT))
			MoveRole(0,DIK_RIGHT);
	}


		if(pcInput->IsPressed(DIK_SPACE))   //检测是不是有事件发生
			ShowDialog();




	RECT rectNow;
	GetClientRect(hwnd,&rectNow);
	POINT poToC;
	poToC.x=rectNow.left;
	poToC.y=rectNow.top;
	ClientToScreen(hwnd,&poToC);
	if(lpImage==NULL) return ;
	
	
	RECT rect = {0,0,32*iNumColumn,32*iNumLine};
	pCDraw->BltToBack(0,0,lpImage,rect,false);  //!!!!!!!!!!!!!!!!!!!!!!!!1以后改
	
	for(int i=0;i!=rolelist.Npcs.size(); i++)
		rolelist.Npcs[i]->draw(hwnd);
	rolelist.leadrole->draw(hwnd);

	
	if(gamestate == TALK )    //画对话框
	{

		if(iDialogNow == rolelist.Npcs[FocusID&0x000f]->vecTalkMessage.size())   //到最后了
		{
			if(bIsFightable)   //开打
			{
				gamestate = FIGHT;
				pCsceneManger->rectLeadRoleToSave = rolelist.leadrole->rectPosition;
				RECT rectToload = {100,200,132,232};
				rolelist.Npcs[FocusID&0x000f]->SetRect(rectToload);
				rolelist.Npcs[FocusID&0x000f]->SetDirection(3);
				
				rectToload.left +=400;
				rectToload.right+=400;
				rolelist.leadrole->SetRect(rectToload);		
				rolelist.leadrole->SetDirection(2);
				pCsceneManger->sceneFight->RegisterLead(rolelist.leadrole);
				pCsceneManger->sceneFight->RegisterNpc(rolelist.Npcs[FocusID&0x000f]);
			}
			else if(rolelist.Npcs[FocusID&0x000f]->bIsSell == true)
			{
				gamestate = SELL;
			}
			else 
				gamestate = WALK;
			iDialogNow = 0;
			if(pCDraw)
				pCDraw->Render();
			return;
		}
		RECT rectdraw = {0,0,500,100};

		/*  半透明色*/
		pCDraw->Transparent(pCDraw->GetBackBuffer(),60+poToC.x,340+poToC.y,lpDialog,rectdraw);
	

		HDC hdc;
		pCDraw->GetBackBuffer()->GetDC(&hdc);
		SetBkMode(hdc,TRANSPARENT);   
		SetTextColor(hdc,   RGB(0,0,0));   //设置文字颜色   
		
		LPCWSTR lpwWordNow = rolelist.Npcs[FocusID&0x000f]->vecTalkMessage[iDialogNow].c_str();   //当前人物的对话 
		int iStrLength = (int)wcslen(lpwWordNow);
		TextOut(hdc,100,400,lpwWordNow,iStrLength);   //句柄,   左上角X,   左上角Y,
		//文字(char   *),   文字长度   
		pCDraw->GetBackBuffer()->ReleaseDC(hdc);   //释放句柄 
		
	}
	
	if(pCDraw)
		pCDraw->Render();

}
#endif

#ifndef WINDOW_DEBUG
void CFightScene::draw()
{

	RECT rect = {0,0,32*iNumColumn,32*iNumLine};
	pCDraw->BltToBack(0,0,lpImage,rect,false);  //!!!!!!!!!!!!!!!!!!!!!!!!1以后改
	rolelist.leadrole->draw();
	for(int i=0;i!=rolelist.Npcs.size(); i++)
		rolelist.Npcs[i]->draw();
RECT rect1 = {250,320,450,470};
	if(pcInput->IsPressed(DIK_RETURN)&&bIsFighting == false)
	{
		if(pMenu->GetStatus() == 0)
		{
			bIsFighting = true;
			bIsAttacking = true;
			turn  = MY;
		}
		if(pMenu->GetStatus() == 1)
		{
			if(rolelist.leadrole->GetDrugNum() != 0)
			{
				bIsFighting = true;
				bIsAttacking = true;
				turn  = ENEMY;
				HpUp();
			}
		}
	}
	if(!bIsFighting)
	{
		if(pcInput->IsPressed(DIK_UP))
			pMenu->Draw(rect1,DIK_UP,false);
		if(pcInput->IsPressed(DIK_DOWN))
			pMenu->Draw(rect1,DIK_DOWN,false);
		else 
			pMenu->Draw(rect1,NULL,false);
	}
	if(pStatus)
	{
		wchar_t lStatus[20] = {TEXT("主角HP:")};
		_itow(rolelist.leadrole->GetInformation().iHP,lStatus+5,10);
		wstring wstrHp(lStatus);
		pStatus->SetItemValue(0,wstrHp);

		wchar_t lStatus2[20] = {TEXT("主角Attack:")};
		_itow(rolelist.leadrole->GetInformation().iAttack,lStatus2+9,10);
		wstring wstrHp2(lStatus2);
		pStatus->SetItemValue(1,wstrHp2);

		wchar_t lStatus3[20] = {TEXT("主角Defense:")};
		_itow(rolelist.leadrole->GetInformation().iDefence,lStatus3+10,10);
		wstring wstrHp3(lStatus3);
		pStatus->SetItemValue(2,wstrHp3);
		RECT rect = {420,320,620,400};
		pStatus->Draw(rect,NULL,false);


	}



	if(pCDraw)
		pCDraw->Render();
}
#else
void CFightScene::draw(HWND hwnd)
{
	
	
		//	turn= MY;
	
	RECT rect = {0,0,32*iNumColumn,32*iNumLine};
	pCDraw->BltToBack(0,0,lpImage,rect,false);  //!!!!!!!!!!!!!!!!!!!!!!!!1以后改
	rolelist.leadrole->draw(hwnd);
	for(int i=0;i!=rolelist.Npcs.size(); i++)
		rolelist.Npcs[i]->draw(hwnd);
	RECT rect1 = {250,320,450,470};
	if(pcInput->IsPressed(DIK_RETURN)&&bIsFighting == false)
	{
		if(pMenu->GetStatus() == 0)
		{
			bIsFighting = true;
			bIsAttacking = true;
			turn  = MY;
		}
		if(pMenu->GetStatus() == 1)
		{
			if(rolelist.leadrole->GetDrugNum() != 0)
			{
				bIsFighting = true;
				bIsAttacking = true;
				turn  = ENEMY;
				HpUp();
			}
		}
	}
	if(!bIsFighting)
	{
		if(pcInput->IsPressed(DIK_UP))
			pMenu->Draw(rect1,DIK_UP,false);
		if(pcInput->IsPressed(DIK_DOWN))
			pMenu->Draw(rect1,DIK_DOWN,false);
		else 
			pMenu->Draw(rect1,NULL,false);
	}
	if(pStatus)
	{
		wchar_t lStatus[20] = {TEXT("主角HP:")};
		_itow(rolelist.leadrole->GetInformation().iHP,lStatus+5,10);
		wstring wstrHp(lStatus);
		pStatus->SetItemValue(0,wstrHp);

		wchar_t lStatus2[20] = {TEXT("主角Attack:")};
		_itow(rolelist.leadrole->GetInformation().iAttack,lStatus2+9,10);
		wstring wstrHp2(lStatus2);
		pStatus->SetItemValue(1,wstrHp2);

		wchar_t lStatus3[20] = {TEXT("主角Defense:")};
		_itow(rolelist.leadrole->GetInformation().iDefence,lStatus3+10,10);
		wstring wstrHp3(lStatus3);
		pStatus->SetItemValue(2,wstrHp3);
		RECT rect = {420,320,620,400};
		pStatus->Draw(rect,NULL,false);
	}



	if(pCDraw)
		pCDraw->Render();

}

#endif

void CFightScene::HpUp()
{
	srand((unsigned int)clock());
	int iHpToAdd = 50 + rand()%150;
	Information& infor = rolelist.leadrole->GetInformation();
	if(infor.iHP+iHpToAdd > infor.iMaxHP)
		infor.iHP = infor.iMaxHP;
	else
		infor.iHP += iHpToAdd;
	rolelist.leadrole->SetDrugNum( rolelist.leadrole->GetDrugNum() -1);
	wstring wstrNum = TEXT("使用药水:");
	int iNumDrug = rolelist.leadrole	->GetDrugNum();
	WCHAR wBuffer[20];
	_itow(iNumDrug,wBuffer,10);
	wstrNum += wBuffer;
	wstrNum += TEXT("个");
	pMenu->SetItemValue(1,wstrNum);
}

void CSceneManager::UpdateScene(HWND hwnd)
{
	if(sceneFight)
	{
		sceneFight->UpdateScene(hwnd);
	}
}


void CFightScene::UpdateScene(HWND hwnd)
{
	if(turn == MY)
	{
		if(bIsAttacking)
		{
			MoveRole(0x0000,DIK_LEFT,50);
			if(rolelist.leadrole->GetRect().left<=150)
			{
				bIsAttacking = false;
				ShowEffect(rolelist.Npcs[0]->GetRect());

				if(rolelist.Npcs[0]->WasBeatenBy(rolelist.leadrole,hwnd))
				{
					bIsAttacking = true;
					bIsFighting = false;
					turn = NONE;
					
					FightEnd();
					pCDraw->Render();
					Sleep(2500);
					if(rolelist.Npcs[0]->GetID() == 64)
					{
						pCsceneManger->SetStatusM(GAMEOVER);
						pCsceneManger->ReturnToNormal(rolelist.leadrole,rolelist.Npcs[0]);
					}

					else
						pCsceneManger->ReturnToNormal(rolelist.leadrole,rolelist.Npcs[0]);
				
				}
			}				
			return;
		}
		else
		{
			MoveRole(0x0000,DIK_RIGHT,50);			
			if(rolelist.leadrole->GetRect().left>=500)
			{
				bIsAttacking = true;
				rolelist.leadrole->SetDirection(2);

				turn = ENEMY;
			}
			return;

		}
	}
	else if(turn == ENEMY)
	{
		if(bIsAttacking)
		{
			MoveRole(0x0010,DIK_RIGHT,50);
			if(rolelist.Npcs[0]->GetRect().right>=450)
			{
				bIsAttacking = false;
				ShowEffect(rolelist.leadrole->GetRect());
				if(	rolelist.leadrole->WasBeatenBy(rolelist.Npcs[0],hwnd))
				{
					//主角死亡
					pCsceneManger->SetStatusM(GAMEOVER);
					pCsceneManger->ReturnToNormal(rolelist.leadrole,rolelist.Npcs[0]);
				}
			}
			return;
		}
		else
		{
			MoveRole(0x0010,DIK_LEFT,50);			
			if(rolelist.Npcs[0]->GetRect().left<=100)
			{
				bIsAttacking = true;
				rolelist.Npcs[0]->SetDirection(3);
				bIsFighting = false;
				turn = NONE;
			}
			return;
		
		}
	}
}


CScene::~CScene()
{
	if(lpImage)
	{
		lpImage->Release();
		lpImage = NULL;
	}
	if(lpImageBuffer)
	{
		lpImageBuffer->Release();
		lpImageBuffer = NULL;
	}
	for(int i = 0;i<(int)rolelist.Npcs.size();i++)
	{
		if(rolelist.Npcs[i])
		{
			delete rolelist.Npcs[i];
			rolelist.Npcs[i] = NULL;
		}
	}
}

CLead::~CLead()
{
	if(lpImage) 
	{
		lpImage->Release();
		lpImage = NULL;
	}
}


bool CScene::CheckLeadMove(const RECT& rectmove)
{
	/* 先检测是不是和 某个 NPC 相交 */
	if(rolelist.leadrole == NULL) return false;
	
	for(int i = 0; i!= rolelist.Npcs.size(); i++)
	{
		RECT rectTotest = rolelist.Npcs[i]->GetRect();
		if(CScene::IsRectCross(rectmove,rectTotest))
			return false;
	}
	
	
	vector<CScenePath> vecPath = pCsceneManger->GetRectToChangeScene(dwID);
	for(int i = 0;i<(int)vecPath.size();i++)
	{
		if(CScene::IsRectCross(vecPath[i].rectFrom,rectmove))
		{
			gamestate = CHANGE;
			wstring wstrSoundNow = pCsceneManger->vecScene[rolelist.leadrole->GetSceneID()&0x000f]->wstrMidiName;
			wstring wstrSoundTo  = pCsceneManger->vecScene[vecPath[i].iSceneToID&0x000f]->wstrMidiName;
			if(wstrSoundNow != wstrSoundTo)
				PlaySound(wstrSoundTo.c_str(),ghInstance,SND_LOOP|SND_FILENAME|SND_ASYNC );
			Sleep(500);
			rolelist.leadrole->SetSceneID(vecPath[i].iSceneToID);
			rolelist.leadrole->SetRect(vecPath[i].rectTo);
			return true;
		}
	}
	return true;
}

void CScene::MoveRole(DWORD dwID,DWORD dwMove,int iStep)
{
	RECT rectNow;
	int fiDirection;
	if(dwID&&0x00f0)  //NPC
	{
		 RECT rectT = rolelist.Npcs[dwID&0x000f]->GetRect();
		 memcpy(&rectNow,&rectT,sizeof(RECT));
	}
	else                         //主角
	{
		RECT rectT = rolelist.leadrole->GetRect();
		memcpy(&rectNow,&rectT,sizeof(RECT));
	}
	switch(dwMove)
	{
	case DIK_UP:
		rectNow.top-=iStep;
		rectNow.bottom-=iStep;
		fiDirection = 0;
		break;
	case DIK_DOWN:
		rectNow.top+=iStep;
		rectNow.bottom+=iStep;
		fiDirection = 1;
		break;
	case DIK_LEFT:
		rectNow.left-=iStep;
		rectNow.right-=iStep;
		fiDirection = 2;
		break;
	case DIK_RIGHT: 
		rectNow.left+=iStep;
		rectNow.right+=iStep;
		fiDirection = 3;
		break;
	default:
		return;
	}
	if(dwID == 0x0000)
	{
		if(CheckLeadMove(rectNow) && CheckMoveCollision(rectNow,fiDirection))	
			rolelist.leadrole->Move(dwMove,iStep);
		rolelist.leadrole->iDirection = fiDirection;
	}
	else if( dwID&0x00f0)
	{
		 if(CheckNpcMove(dwID,rectNow)&& CheckMoveCollision(rectNow,fiDirection))
			 rolelist.Npcs[dwID & 0x000f]->Move(dwMove,iStep);
		 rolelist.Npcs[dwID & 0x000f]->iDirection = fiDirection;
	}

		
}

bool CScene::CheckNpcMove(DWORD dwID,RECT rectPosition)
{
	int iNumNpc = dwID & 0x000f ;  //npc 在向量中的编号
	RECT rectLead = rolelist.leadrole->GetRect();  //主角矩形
	for(int i = 0;i!= rolelist.Npcs.size(); i++ )
	{
		if(i == iNumNpc)         //判断是不是自己		
		{
			if(CScene::IsRectCross(rectPosition,rectLead)) return false;
			continue;
		}
		RECT rectNPC = rolelist.Npcs[i]->GetRect();  //NPC矩形
		if(CScene::IsRectCross(rectPosition,rectLead)  || CScene::IsRectCross(rectPosition,rectNPC))
			return false;
	}
	return true; 
}


bool CScene::RegisterLead(CLead* pleadRole)
{
	if(rolelist.leadrole)
		return false;
	else if(!pleadRole)
		return false;
	else
	{
		rolelist.leadrole = pleadRole;
	}
	return true;

}
bool CScene::RegisterNpc(CNpc* pNpcRole)
{
	if(!pNpcRole)
		return false;
	for(int i = 0;i!=rolelist.Npcs.size();i++)
	{
		if(rolelist.Npcs[i] == pNpcRole)
			return false;
	}
	rolelist.Npcs.push_back(pNpcRole);
	return true;
}

CNpc::~CNpc()
{
	if(lpImage) 
		lpImage->Release();
}

bool CScene::IsRectCross(const RECT& rect1,const RECT& rect2)
{
	int iWidth1 = rect1.right - rect1.left;  //记录第一个矩形的高和宽
	int iHeight1 = rect1.bottom - rect1.top;
	//写出 第二个 矩形周围扩展 半个边长的 新矩形 
	RECT rectTest;
	rectTest.top = rect2.top - iHeight1/2 +5;   //+3是为了 不要太严格的碰撞
	rectTest.bottom = rect2.bottom + iHeight1/2 -5;
	rectTest.left  = rect2.left - iWidth1/2 +5;
	rectTest.right = rect2.right + iWidth1/2 -5;
	//判断第一个矩形中心是不是在这个矩形里 
	int xPoint = rect1.left + iWidth1/2;
	int yPoint = rect1.top + iHeight1/2;
	if(  xPoint > rectTest.left &&
		xPoint < rectTest.right &&
		yPoint < rectTest.bottom &&
		yPoint > rectTest.top )     //相交
		return true;
	return false;
}

void CScene::MoveAllNpc()
{
	srand( (unsigned int)time(NULL));
	int iDirection = rand()%4;
	int iDirection2 = rand()%4;
	for(int i = 0; i!=rolelist.Npcs.size(); i++)
	{
		if(rolelist.Npcs[i]->GetIsMove() == false)
			continue;
		
		switch((iDirection+i*3)%4)   //srand时间太短 这个是 骗人用的  。。
		{
		case 0:
			MoveRole(rolelist.Npcs[i]->GetID(),DIK_UP);
			break;
		case 1:
			MoveRole(rolelist.Npcs[i]->GetID(),DIK_DOWN);
			break;
		case 2:
			MoveRole(rolelist.Npcs[i]->GetID(),DIK_LEFT);
			break;
		case 3:
			MoveRole(rolelist.Npcs[i]->GetID(),DIK_RIGHT);
			break;
		default:
			continue;
		}
	}
}

void CScene::ShowDialog()
{
	static int siDialog;  //当前 对话框编号
	if(rolelist.Npcs.size() == 0) return;
		/*检查现在是不是有人在 主角前面  */
	RECT rectLead = rolelist.leadrole->GetRect();
	int iDirection = rolelist.leadrole->GetDirection();  //主角的方向
	/*在主角的面前建立一个矩形 通过判断 有没有npc在此中 返回第一个找到的NPC的dwID */
	
	
	DWORD dwID = NearNpcID(rectLead,0,iDirection);
	if(dwID != 0xffff)   // 找到了 
	{
		gamestate = TALK;
		FocusID = dwID;
	}
}

DWORD CScene::NearNpcID(const RECT& rectWant,DWORD dwID,int iDirection)
{
	RECT rectNPC = rolelist.Npcs[0]->GetRect();
	int iWidth = rectNPC.right - rectNPC.left;
	int iHeight = rectNPC.bottom = rectNPC.top;
	RECT rectAttach;
	switch(iDirection)
	{
	case 0:   //UP
		rectAttach.top = rectWant.top - iHeight/2;
		rectAttach.bottom = rectWant.top;
		rectAttach.left = rectWant.left - iWidth/2;
		rectAttach.right = rectWant.right + iWidth/2;
		break;
	case 1:   //DOWN
		rectAttach.top = rectWant.bottom;
		rectAttach.bottom = rectWant.bottom + iHeight/2;
		rectAttach.left = rectWant.left - iWidth/2;
		rectAttach.right = rectWant.right + iWidth/2;
		break;
	case 2:   //LEFT
		rectAttach.top = rectWant.top - iHeight/2;
		rectAttach.bottom = rectWant.bottom + iHeight/2;
		rectAttach.left = rectWant.left - iWidth/2;
		rectAttach.right = rectWant.left;
		break;
	case 3:   //RIGHT
		rectAttach.top = rectWant.top - iHeight/2;
		rectAttach.bottom = rectWant.bottom + iHeight/2;
		rectAttach.left = rectWant.right;
		rectAttach.right = rectWant.right + iWidth/2;
		break;
	default:
		break;
	}

	/* 如果rectWant 是 主角  */
	for(int i = 0; i!= rolelist.Npcs.size(); i++)
	{
		if(CScene::IsRectCross(rectAttach,rolelist.Npcs[i]->GetRect()))
			return rolelist.Npcs[i]->GetID();
	}
	return 0xffff;  //没找到
	
}

bool CScene::CheckMoveCollision(const RECT& rectPosition,int iDirection)
{
	if(rectPosition.left<0 || rectPosition.right>640 || rectPosition.top< 16 || rectPosition.bottom >480)
		return false;
	for(int i = 0;i<(int)vecCollision.size();i++)
	{
		for(int j= 0;j<(int)vecCollision[0].size();j++)
		{
			RECT rect = {j*32,i*32,j*32+32,i*32+32};
			if(vecCollision[i][j] == 1 && IsRectCross(rectPosition,rect))
			{
				return false;
			}
		}
	}
	return true;
	
}


bool CFigure::WasBeatenBy(CFigure* fighter,HWND hwnd)
{
	RECT rectNow = this->GetRect();
	
	//看伤了多少血
	bool bCritical = (rand()%100)<fighter->information.iRateCritical;
	int iDamage = fighter->information.iAttack - this->information.iDefence;
	if(iDamage <10) iDamage = 10;
	if(bCritical)
		iDamage = iDamage*3;
	information.iHP-=iDamage;
	wchar_t textDamage[20];
	_itow(iDamage,textDamage,10);
	
	//画文字
	HDC hdc;
	pCDraw->GetBackBuffer()->GetDC(&hdc);
	SetBkMode(hdc,TRANSPARENT);   
	SetTextColor(hdc,   RGB(255,255,255));   //设置文字颜色   
	int iStrLength = (int)wcslen(textDamage);
	RECT rectToDrawText = rectNow;
	rectToDrawText.top += 5;
	rectToDrawText.bottom+=5;
	TextOut(hdc,rectToDrawText.left,rectToDrawText.top,textDamage,iStrLength);   //句柄,   左上角X,   左上角Y,
	pCDraw->GetBackBuffer()->ReleaseDC(hdc);   //释放句柄 
	pCDraw->Render();

	Sleep(1000);
	
	//文字(char   *),   文字长度   
	if(this->information.iHP<0)
		return true;
	return false;
	
	
}

void CSceneManager::ReturnToNormal(CLead* lead,CNpc* npc)
{
	lead->SetRect(rectLeadRoleToSave);
	RECT rectInvalid = {1000,1000,1032,1032};
	npc->SetRect(rectInvalid);
	vecScene[iSceneNum]->SetStatus(WALK);
	ClearFightScene();	
}


void CMenuRect::Initialize()
{
	if(pCDraw==NULL || pcInput==NULL || iBoundsX==0 || iBoundsY ==0)
		throw "CMenuRect没有初始化!";

	RECT rect = {0,0,iBoundsX,iBoundsY};
	if(!pCDraw->CreateBMPSurface(&lpImage,iBoundsX,iBoundsY,lpwFileName,rect,false))
		throw "DirectDraw 创建Surface时失败！";
}


void CMenuRect::Draw(LPCWSTR TextToDraw,const RECT& rectDraw)
{
	rectToDraw = rectDraw;
	RECT RectDrawt;
	RectDrawt.left = 0;
	RectDrawt.top = 0;
	RectDrawt.right = rectDraw.right - rectDraw.left;
	RectDrawt.bottom = rectDraw.bottom - rectDraw.top;
	pCDraw->BltToBack(rectDraw.left,rectDraw.top,lpImage,RectDrawt,false);
	HDC hdc;
	pCDraw->GetBackBuffer()->GetDC(&hdc);
	SetBkMode(hdc,TRANSPARENT);   
	SetTextColor(hdc,   RGB(0,0,0));   //设置文字颜色   
	int iStrLength = (int)wcslen(TextToDraw);
	TextOut(hdc,100,400,TextToDraw,iStrLength);   //句柄,   左上角X,   左上角Y,
	//文字(char   *),   文字长度   
	pCDraw->GetBackBuffer()->ReleaseDC(hdc);   //释放句柄 

}

void CMyMenu::AddItem(wstring strItemText)
{
	wstrItems.push_back(strItemText);
	iItemCount++;
	iCurrentItem = 0;
}

void CMyMenu::Draw(const RECT& rectDraw,DWORD dwKey,bool bNeedChange)
{
	if(DIK_UP == dwKey)
	{
		if(iCurrentItem == 0)
			return;
		iCurrentItem--;

	}
	else if(DIK_DOWN ==dwKey )
	{
		if(iCurrentItem == iItemCount -1)
			return;
		iCurrentItem++;
	}
	rectToDraw = rectDraw;
	RECT RectDrawt;
	RectDrawt.left = 0;
	RectDrawt.top = 0;
	RectDrawt.right = 120;
	RectDrawt.bottom = iItemCount*40;
	//pCDraw->BltToBack(rectDraw.left,rectDraw.top,lpImage,RectDrawt,false);
	pCDraw->Transparent(pCDraw->GetBackBuffer(),rectDraw.left,rectDraw.top,lpImage,RectDrawt,bNeedChange);
	HDC hdc;
	pCDraw->GetBackBuffer()->GetDC(&hdc);
	SetBkMode(hdc,TRANSPARENT);   
	SetTextColor(hdc,   RGB(0,0,0));   //设置文字颜色 
	
	for(int i = 0;i<iItemCount;i++)
	{
		int iStrLength = (int)wstrItems[i].size();
		if(i == iCurrentItem)
			SetTextColor(hdc,RGB(255,255,255));
		else 
			SetTextColor(hdc,RGB(0,0,0));
		TextOut(hdc,rectDraw.left + 5,rectDraw.top + i*20,wstrItems[i].c_str(),iStrLength);   //句柄,   左上角X,   左上角Y,
	}

	//文字(char   *),   文字长度   
	pCDraw->GetBackBuffer()->ReleaseDC(hdc);   //释放句柄 

}

void CFightScene::FightEnd()
{
	static int debug = 0;
	if(debug == 1)
		int iii = 0;
	RECT rect = {200,100,450,300};
	wchar_t Infor[40] = {TEXT("获得经验值:")};  //buffer
	wchar_t Infor2[40];  //buffer
	Information& infoRole = rolelist.leadrole->GetInformation();
	infoRole.iExp += rolelist.Npcs[0]->GetExpKill();
	srand((unsigned int)clock());
	int iMoney = rand() % 100 + 50;
	rolelist.leadrole->AddMoney(iMoney);
	wstring wstrMoney = TEXT("获得Money: ");
	bool bIslevelup = false;
	rolelist.leadrole->SetDrugNum( rolelist.leadrole->GetDrugNum() + rand()%2 +1);
	if(infoRole.iExp>=infoRole.iNeedExp)
	{
		LevelUp();
		bIslevelup = true;
	}

	pMenuRect->ClearItem();
	_itow(rolelist.Npcs[0]->GetExpKill(),Infor+6,10);
	wstring strInfo(Infor);
	pMenuRect->AddItem(strInfo.c_str());

	_itow(iMoney,Infor2,10);
	wstrMoney += Infor2;
	pMenuRect->AddItem(wstrMoney.c_str());
	_itow(infoRole.iExp,Infor2,10);
	for(int i = 0;i<10;i++)
	{
		if(!iswdigit(Infor2[i]))
			Infor2[i] = TEXT(' ');
	}
	
	Infor[10] = TEXT('/');
	_itow(infoRole.iNeedExp,Infor2 + 11,10);
	wstring strInfo2(Infor2);
	pMenuRect->AddItem(strInfo2.c_str());
	
	debug ++;

	if(bIslevelup)   //升级了
	{
		pMenuRect->AddItem(TEXT("升级了！"));
		_itow(infoRole.iLevel-1,Infor,10);
		_itow(infoRole.iLevel,Infor2,10);
		wstring strLevel(TEXT("等级提升"));
		strLevel = (strLevel + Infor + TEXT("-----") + Infor2);
		pMenuRect->AddItem(strLevel.c_str());
	
		_itow(infoRole.iAttack-20,Infor,10);
		_itow(infoRole.iAttack,Infor2,10);
		wstring strAttack(TEXT("攻击提升"));
		strAttack = (strAttack + Infor + TEXT("-----") + Infor2);
		pMenuRect->AddItem(strAttack.c_str());

		_itow(infoRole.iDefence-4,Infor,10);
		_itow(infoRole.iDefence,Infor,10);
		wstring strDef(TEXT("防御提升"));
		strDef = (strDef + Infor + TEXT("-----") + Infor2);
		pMenuRect->AddItem(strDef.c_str());

		_itow(infoRole.iMaxHP -50,Infor,10);
		_itow(infoRole.iMaxHP,Infor2,10);
		wstring strHp(TEXT("生命最大值提升"));
		strHp = (strHp + Infor + TEXT("-----") + Infor2);
		pMenuRect->AddItem(strHp.c_str());
	}
	pMenuRect->Draw(rect,NULL);
	

}

void CFightScene::LevelUp()
{
	Information& infoRole = rolelist.leadrole->GetInformation();
	infoRole.iAttack+=20;
	infoRole.iDefence +=4;
	infoRole.iRateCritical+=10;
	infoRole.iMaxHP +=50;
	infoRole.iLevel++;
	infoRole.iNeedExp = infoRole.iLevel*infoRole.iLevel*25;
	infoRole.iHP+=50;
	if(infoRole.iHP>infoRole.iMaxHP)
		infoRole.iHP = infoRole.iMaxHP;
}

void CFightScene::ShowEffect(RECT rectToDraw)
{
	RECT rect = {0,0,54,50};
	for(int i = 0;i<10;i++)
	{
		pCDraw->BltToBack(rectToDraw.left,rectToDraw.top-20,lpsurtExplode,rect,true);
		rect.left+=54;
		rect.right+=54;
		pCDraw->Render();
		Sleep(50);
	}
}

void CSellScene::DrawPic()
{
	RECT rectBlt = {0,0,48,48};
	pCDraw->BltToBack(0,0,lpImageBuffer,rectBlt,false);
	pCDraw->BltToBack(0,0,lpImageBuffer,(*pCWeapon)[pMyMenu->GetStatus()].rect,true);
	DrawWeaponInfo();



}

void CSellScene::Buy()
{
	int iMoney = (*pCWeapon)[pMyMenu->GetStatus()].iMoney;
	if(rolelist.leadrole->GetMoney() < iMoney)
		return;
	rolelist.leadrole->Buy((*pCWeapon)[pMyMenu->GetStatus()]);
}

void CSellScene::DrawWeaponInfo()
{
	HDC hdc;
	pCDraw->GetBackBuffer()->GetDC(&hdc);
	SetBkMode(hdc,TRANSPARENT);   


	//名称 
	wstring  wstrName = (*pCWeapon)[pMyMenu->GetStatus()].wstrName;
	wstring  wstrDesc = (*pCWeapon)[pMyMenu->GetStatus()].wstrDesc;
	int      iPrice   = (*pCWeapon)[pMyMenu->GetStatus()].iMoney;
	int      iMoney   = rolelist.leadrole->GetMoney();
	wstring  wstrTemp1 = TEXT("价钱");
	wstring  wstrTemp2 = TEXT("剩余价钱");
	wchar_t  wcBuffer[20];
	_itow(iPrice,wcBuffer,10);
	wstring  wstrPrice = wstrTemp1 + wcBuffer;
	_itow(iMoney,wcBuffer,10);
	wstring  wstrPrice2 = wstrTemp2 + wcBuffer;


	int iAttack = 0;
	int iCritical = 0;
	int iAttackBase;
	int iCriticalBase;

	map<WeaponInfo,int,CWeaponComp> mapWeapon = rolelist.leadrole->GetWeapon();
	for(map<WeaponInfo,int,CWeaponComp>::iterator i = mapWeapon.begin();i!=mapWeapon.end();i++)
	{
		if((*i).first.iAttack > iAttack)
			iAttack = (*i).first.iAttack;
		if((*i).first.iRateCritical >iCritical)
			iCritical = (*i).first.iRateCritical;

	}
	
	iAttackBase = rolelist.leadrole->GetInformation().iAttack - iAttack;
	iCriticalBase = rolelist.leadrole->GetInformation().iRateCritical - iCritical;
	

	_itow(iAttack + iAttackBase,wcBuffer,10);
	wstring wstrAttackNow = TEXT("攻击力:");
	wstrAttackNow += wcBuffer;
	wstrAttackNow  += TEXT("----->");

	_itow(iCritical+ iCriticalBase,wcBuffer,10);
	wstring wstrCNow = TEXT("暴击率:");
	wstrCNow += wcBuffer;
	wstrCNow  += TEXT("----->");

	WeaponInfo wpInfo = (*pCWeapon)[pMyMenu->GetStatus()];
	int  iAttackTo;
	int  iCriticalTo;
	
	 if(wpInfo.iAttack > iAttack)
		iAttackTo = iAttackBase + wpInfo.iAttack;
	 else
		 iAttackTo = iAttackBase  + iAttack;


	 if(wpInfo.iRateCritical > iCritical)
		iCriticalTo = iCriticalBase + wpInfo.iRateCritical;
	 else
		 iCriticalTo = iCriticalBase + iCritical;


	_itow(iAttackTo,wcBuffer,10);
	wstrAttackNow += wcBuffer;

	_itow(iCriticalTo,wcBuffer,10);
	wstrCNow += wcBuffer;
	

	TextOut(hdc,300,200,wstrName.c_str(),(int)wstrName.size());   //句柄,   左上角X,   左上角Y,
	TextOut(hdc,300,230,wstrDesc.c_str(),(int)wstrDesc.size());
	TextOut(hdc,300,260,wstrPrice.c_str(),(int)wstrPrice.size());
	TextOut(hdc,300,290,wstrPrice2.c_str(),(int)wstrPrice2.size());
	TextOut(hdc,300,320,wstrAttackNow.c_str(),(int)wstrAttackNow.size());
	TextOut(hdc,300,350,wstrCNow.c_str(),(int)wstrCNow.size());
	//文字(char   *),   文字长度   
	pCDraw->GetBackBuffer()->ReleaseDC(hdc);   //释放句柄 
}


#ifdef WINDOW_DEBUG
void CSellScene::draw(HWND hwnd)
{

	RECT rect = {0,0,32*iNumColumn,32*iNumLine};
	pCDraw->BltToBack(0,0,lpImage,rect,false);

	RECT rect1 = {100,250,200,350};
	if(pcInput->IsPressed(DIK_UP))
	{
		pMyMenu->Draw(rect1,DIK_UP,false);
		Sleep(100);
	}
	if(pcInput->IsPressed(DIK_DOWN))
	{
		pMyMenu->Draw(rect1,DIK_DOWN,false);
		Sleep(100);
	}
	if(pcInput->IsPressed(DIK_RETURN)) 
		Buy();
	if(pcInput->IsPressed(DIK_ESCAPE))
		pCsceneManger->SetStatus(WALK);
	else
		pMyMenu->Draw(rect1,NULL,false);
	DrawPic();
	pCDraw->Render();
	
	
}
#else
void CSellScene::draw()
{
	RECT rect = {0,0,32*iNumColumn,32*iNumLine};
	pCDraw->BltToBack(0,0,lpImage,rect,false);

	RECT rect1 = {100,250,200,350};
	if(pcInput->IsPressed(DIK_UP))
	{
		pMyMenu->Draw(rect1,DIK_UP,false);
		Sleep(100);
	}
	if(pcInput->IsPressed(DIK_DOWN))
	{
		pMyMenu->Draw(rect1,DIK_DOWN,false);
		Sleep(100);
	}
	if(pcInput->IsPressed(DIK_RETURN)) 
		Buy();
	if(pcInput->IsPressed(DIK_ESCAPE))
		pCsceneManger->SetStatus(WALK);
	else
		pMyMenu->Draw(rect1,NULL,false);
	DrawPic();
	pCDraw->Render();
	
}
#endif

void CLead::Buy(WeaponInfo wpInfo)
{
	Equip(wpInfo);
	mapWeapon[wpInfo]++;
	iMoney = iMoney - wpInfo.iMoney;
	Sleep(200);

}

void CLead::Equip(WeaponInfo wpInfo)
{
	int iAttack = 0;
	int iCritical = 0;
	for(map<WeaponInfo,int,CWeaponComp>::iterator i = mapWeapon.begin();i!=mapWeapon.end();i++)
	{
		if((*i).first.iAttack > iAttack)
			iAttack = (*i).first.iAttack;
		if((*i).first.iRateCritical >iCritical)
			iCritical = (*i).first.iRateCritical;

	}
	if(mapWeapon.size() == 0)
	{
		information.iAttack += wpInfo.iAttack;
		information.iRateCritical += wpInfo.iRateCritical;
		return;
	}
	
	information.iAttack += (  wpInfo.iAttack > iAttack )?(wpInfo.iAttack - iAttack):0;
	information.iRateCritical += (  wpInfo.iRateCritical > iCritical )?(wpInfo.iRateCritical - iCritical ):0;
}


bool CSellScene::Initialize()
{
	RECT rect = {0,0,32*iNumColumn,32*iNumLine};
	if(!pCDraw->CreateBMPSurface(&lpImage,dwWidth,dwHeight,lpwFileName.c_str(),rect,false))
		return false;
	RECT rect2 = {0,0,384,192};
	pCDraw->CreateBMPSurface(&lpImageBuffer,384,192,TEXT("goods.bmp"),rect2,true);
	/* DiaLog    */
	
	return true;
}
	







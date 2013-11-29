#include "cobject.h"


Status CSceneManager::GetStatus() 
{
	return vecScene[iSceneNum]->GetStatus();
}
void CSceneManager::RecieveMoveAllNpc()
{
	if(iRegisteredNum == 0) return;
	//if(vecScene[iSceneNum]->GetStatus == WALK)
		vecScene[iSceneNum]->MoveAllNpc();

}

vector<CScenePath> CSceneManager::GetRectToChangeScene(int iSceneID)
{
	vector<CScenePath>  vecPaths;
	for(int i =0;i<(int)vecScenepaths.size();i++)
	{
		if(vecScenepaths[i].iSceneFrom == iSceneID)
			vecPaths.push_back(vecScenepaths[i]);

	}
	return vecPaths;
}

void CSceneManager::RecieveUpdataDialog()
{
	if(iRegisteredNum == 0) return;
	//if(vecScene[iSceneNum]->GetStatus == TALK)
		vecScene[iSceneNum]->UpdateDialog();
}

#ifndef WINDOW_DEBUG
void CSceneManager::Draw()
#else
void CSceneManager::Draw(HWND hwnd)
#endif
{
    if(mstatus == GAMEOVER)
	{
		GameOver();
		return;
		
	}
	if(vecScene[iSceneNum]->GetStatus() == FIGHT)
	{
#ifndef WINDOW_DEBUG
		sceneFight->draw();
#else
		sceneFight->draw(hwnd);
#endif
		return;
	}
	if(vecScene[iSceneNum]->GetStatus() == SELL)
	{
#ifndef WINDOW_DEBUG
		sceneSell->draw();
#else
		sceneSell->draw(hwnd);
#endif
		return;
	}
	if(iRegisteredNum == 0) return;
#ifndef WINDOW_DEBUG
	vecScene[iSceneNum]->draw();
#else
	vecScene[iSceneNum]->draw(hwnd);
#endif	
	if(vecScene[iSceneNum]->GetStatus() == CHANGE)
	{
		iSceneNum = (vecScene[iSceneNum]->GetSceneID() )&0x000f;
		vecScene[iSceneNum]->SetStatus(WALK);
		RoleList rolelist = vecScene[iSceneNum]->ReturnRoleList();
		
	
	}
	
	return;
}


bool CSceneManager::RegisterScene(CScene* pCscene)
{
	for(int i = 0 ;i!= vecScene.size();i++)
	{
		if(vecScene[i] == pCscene)
			return false;
	}
	vecScene.push_back(pCscene);
	iRegisteredNum++;
	pCscene->SetManger(this);
	return true;

}

void CSceneManager::ClearFightScene()
{
	if(sceneFight == NULL)
		throw "没有初始化打斗场景！";
	while(sceneFight->ReturnRoleList().Npcs.size()!=0)
	{
		sceneFight->ReturnRoleList().Npcs.clear();
	}
	sceneFight->ReturnRoleList().leadrole = NULL;

}

void CSceneManager::GameOver()
{
	RECT rect = {0,0,640,480};
	pCDraw->BltToBack(0,0,lpGameOver,rect,false);
	
	pCDraw->Render();

}
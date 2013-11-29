/*  CDb.cpp */ 
/*  Database 读取的操作类以及相关实现 */

#include <string>
#include <sstream>
#include <vector>
using namespace std;
#include "CDb.h"
void CDb::CnOpen()
{
	HRESULT hr = m_pConnection->Open( m_strConnString.c_str(), "", "", adModeUnknown);
	if( FAILED ( hr ) )
	{
		throw "连接失败!";
	}
}

void CDb::SetCommandText(string strCmdText)
{
	m_strCmd = strCmdText;
}

void CDb::ExcuteCmd() //执行命令并将结果返回RecordSet中
{
	if(m_strCmd.length() == 0)
		throw "命令没有初始化!";
	m_pCommand->ActiveConnection = m_pConnection;
	HRESULT hr = m_pRecordset->Open(m_strCmd.c_str(),// 查询DemoTable表中所有字段
		m_pConnection.GetInterfacePtr(),  // 获取库接库的IDispatch指针
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	if(FAILED(hr))
		throw "返回结果的时候出错!";
}

void CDb::EndCmd()
{
	HRESULT hr = m_pRecordset->Close();			
	if(FAILED(hr))
		throw "RecordSet关闭出错 !";

}

bool CDb::MoveNext() //如果为假则读到最后一行
{
	if(!m_pRecordset)
		throw "RecordSet没有初始化!";
	HRESULT hr = m_pRecordset->MoveNext();
	if(FAILED(hr))
		throw "RecordSet在移动到下一行出错!";
	if(m_pRecordset->EndOfFile)
		return false;
	return true;
}

vector<WeaponInfo> CWeaponReader::GetWeapons()  //从数据库中读取武器信息
{
	vector<WeaponInfo> vecWeapon;
	WeaponInfo         weaInfo;
	SetConnTimeOut(0);
	Initialize();
	SetCommandText("select * from Weapons");
	CnOpen();
	ExcuteCmd();
	do 
	{
		weaInfo.iAttack = GetInt("iAttack");
		weaInfo.iID     = GetInt("iID");
		weaInfo.iMoney  = GetInt("iMoney");
		weaInfo.iRateCritical = GetInt("iRateCritical");
		weaInfo.rect    = GetSingleRect("rect");
		weaInfo.wstrDesc = GetWChar("wstrDesc");
		weaInfo.wstrName = GetWChar("wstrName");		
		vecWeapon.push_back(weaInfo);
	} while (MoveNext());
	EndCmd();
	CnClose();
	return vecWeapon;

}


void CDb::Initialize()
{

	HRESULT
		hr = m_pCommand.CreateInstance(__uuidof(Command));
	if(FAILED(hr))
		throw "初始化cmd错误！";

	hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
	if(FAILED(hr))
		throw "初始化数据集错误！";

}




Attribute CDataReader::FillRole(string cmd)  //填充人物信息
{
	Attribute attr;
	SetConnTimeOut(0);
	Initialize();
	SetCommandText(cmd);
	
	CnOpen();
	ExcuteCmd();
	do 
	{
		ZeroMemory(&attr,sizeof(attr));
		attr.dwHeight = GetInt("dwHeight");
		attr.dwID     = GetInt("dwID");
		attr.dwScene  = GetInt("dwScene");
		attr.dwWidth  = GetInt("dwWidth");
		attr.ImageFileName = GetWChar("ImageFileName");
		attr.INumRect = GetInt("INumRect");
		attr.loadRect = GetSingleRect("LoadRect");
		attr.strName  = GetWChar("strName");
		attr.vecRect  = GetVecRect("VecRect");
		attr.Info.iAttack = GetInt("iAttack");
		attr.Info.iDefence = GetInt("iDefence");
		attr.Info.iRateCritical = GetInt("iRateCritical");
		attr.Info.iHP = GetInt("iHP");
		attr.Info.iMaxHP = GetInt("iMaxHP");
		attr.iExpKill = GetInt("iExpKill");
		attr.vecTalk = GetTalkNpc("vecTalkNpc");
		attr.bIsSell = GetInt("bIsSell")==1?true:false;
		attr.bIsMove = GetInt("bIsMove")==1?true:false;
		
	} while (MoveNext());
	EndCmd();
	CnClose();
	return attr;

}

vector<CScenePath> CPathReader::GetPaths() //获得场景切换路线的信息
{
	vector<CScenePath> path;
	SetConnTimeOut(0);
	Initialize();
	SetCommandText("select * from Path");
	CnOpen();
	ExcuteCmd();
	do 
	{
		CScenePath csP;
		csP.iSceneFrom = GetInt("dwSceneIDFrom");
		csP.iSceneToID = GetInt("dwSceneIDTo");
		csP.rectFrom = GetSingleRect("SceneRectFrom");
		csP.rectTo   = GetSingleRect("SceneRectTo");
		path.push_back(csP);
	} while (MoveNext());
	
	EndCmd();
	CnClose();
	return path;

}

RECT CDb::GetSingleRect(string strColName) //将读取的数据转换为RECT
{
	wstring wstrRect((WCHAR*)_bstr_t(m_pRecordset->Fields->GetItem(_variant_t(strColName.c_str()))->Value));
	RECT rect;
	wistringstream wstream(wstrRect);
	
	wstream>>rect.left>>rect.top>>rect.right>>rect.bottom;
	return rect;

}

vector<RECT> CDb::GetVecRect(string strColName)  //将读取的数据转换为RECT的向量
{
	wstring wstrRect((WCHAR*)_bstr_t(m_pRecordset->Fields->GetItem(_variant_t(strColName.c_str()))->Value));
	vector<RECT> vecRect;
	wistringstream wstream(wstrRect);
	while(wstream)
	{
		RECT rect;
		wstream>>rect.left>>rect.top>>rect.right>>rect.bottom;
		vecRect.push_back(rect);
	}
	return vecRect;

}

SceneDesc CSceneReader::GetDesc(string strSceneID)   //获得场景的描述信息
{

		SceneDesc sc;
		SetConnTimeOut(0);
		Initialize();
		string cmd = "select * from Scene where dwID = ";
		cmd += strSceneID;
		SetCommandText(cmd);


		CnOpen();
		ExcuteCmd();

		do 
		{
			ZeroMemory(&sc,sizeof(sc));
			sc.dwHeight = GetInt("dwHeight");
			sc.dwID    = GetInt("dwID");
			sc.dwWidth  = GetInt("dwWidth");
			sc.lpwFileName = GetWChar("lpwFileName");
			sc.bIsFightable = (GetInt("bIsFightable")==1)?true:false;
			sc.wstrMidiName = GetWChar("wstrMidName");
			sc.iNumLine = GetInt("iNumLine");
			sc.iNumCol  = GetInt("iNumCol");
			sc.lpwDialogBmp = GetWChar("lpwDialogBmp");
			sc.bIswholeimage = (GetInt("bIswholeimage")==1)?true:false;
			if(!sc.bIswholeimage)
			{
				
				sc.vecmap       = GetVec("vecmap",sc.iNumLine,sc.iNumCol);
				sc.vecmapimage  = GetVecImage("vecmapimage");
				
			}
			sc.veccollision = GetVec("vecmapcollision",sc.iNumLine,sc.iNumCol);



		} while (MoveNext());


		EndCmd();
		CnClose();
		return sc;
	
}

vector<vector<int> > CSceneReader::GetVec(string strColName,int iLine,int iCol)   //获得场景的矩阵信息
{
	wstring wstrRect((WCHAR*)_bstr_t(m_pRecordset->Fields->GetItem(_variant_t(strColName.c_str()))->Value));
	vector<vector<int> > vecRect;
	wistringstream wstream(wstrRect);

	for(int i = 0;i<iLine;i++)
	{
		vector<int> vec;
		for(int j= 0;j<iCol;j++)
		{
			int temp;
			wstream>>temp;
			vec.push_back(temp);
			
		}
		vecRect.push_back(vec);
	}
	
	return vecRect;


}

vector<RECT> CSceneReader::GetVecImage(string strColName)     //得到一些列的矩阵信息
{
	wstring wstrRect((WCHAR*)_bstr_t(m_pRecordset->Fields->GetItem(_variant_t(strColName.c_str()))->Value));
	vector<RECT> vecRect;
	wistringstream wstream(wstrRect);

	while(wstream)
	{
		RECT rec;
		wstream>>rec.left>>rec.top>>rec.right>>rec.bottom;
		vecRect.push_back(rec);
		
	}

	return vecRect;


}

vector<Attribute> CNpcReader::GetNpc(string strSceneID)        //获取某一场景的所有NPC属性
{
	vector<int> vecNpcID = GetNpcVecID(strSceneID);
	vector<Attribute> vecAttr;
	char cBuffer[20];
	for(vector<int>::iterator it = vecNpcID.begin();it!=vecNpcID.end();it++)
	{
		_itoa(*it,cBuffer,10);
		string strNpcID(cBuffer);
		Attribute attr = FillRole("select * from Roles where dwID = "+strNpcID);
		vecAttr.push_back(attr);
	}
	return vecAttr;
	
}

vector<int> CNpcReader::GetNpcVecID(string strSceneID)   //获取场景中NPC的ID号
{
	vector<int> vecIntNpcID;
	SetConnTimeOut(0);
	Initialize();
	SetCommandText("select * from SceneRole where dwSceneID = "+strSceneID);

	CnOpen();
	ExcuteCmd();
	do 
	{
		int dwID = GetInt("dwRoleID");
		vecIntNpcID.push_back(dwID);
	} while (MoveNext());
	EndCmd();
	CnClose();
	return vecIntNpcID;
}

vector<wstring> CDataReader::GetTalkNpc(string strColName)      //获得NPC的对话
{
	wstring wstrTalk((WCHAR*)_bstr_t(m_pRecordset->Fields->GetItem(_variant_t(strColName.c_str()))->Value));
	wistringstream wiStream(wstrTalk);
	vector<wstring> wsvec;
	wstring wsTalk;
	while(wiStream>>wsTalk)
	{
		wsvec.push_back(wsTalk);
	}
	return wsvec;


}

CScene* CSceneInit::InitScene(string strSceneID,CLead* pLead,CDraw* pcDraw,CInput* pcInput)   //从数据库初始化某一场景
{
	SceneDesc scendesc = sr.GetDesc(strSceneID);
	scendesc.pCDraw = pcDraw;
	scendesc.pcInput = pcInput;
	CScene* pcScene = new CScene(scendesc);
	pcScene->Initialize();


	vector<Attribute> vecSceneNpc = crNpc.GetNpc(strSceneID);
	vector<CNpc*>      vecNpc;
	for(int i=0;i<(int)vecSceneNpc.size();i++)
	{
		CNpc* npc = new CNpc(pcDraw,vecSceneNpc[i]);
		npc->Initialize();
		pcScene->RegisterNpc(npc);
	}
	pcScene->RegisterLead(pLead);
	return pcScene;	
}

void CWeaponReader::FillItems(vector<WeaponInfo> vecWpi,CMyMenu& menuToFill)    //将读入的武器信息写入菜单中
{
	wstring str;
	for(int i = 0;i<(int)vecWpi.size();i++)
	{
		str = vecWpi[i].wstrName;
		menuToFill.AddItem(str);
	}
}

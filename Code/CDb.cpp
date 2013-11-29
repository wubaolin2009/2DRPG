/*  CDb.cpp */ 
/*  Database ��ȡ�Ĳ������Լ����ʵ�� */

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
		throw "����ʧ��!";
	}
}

void CDb::SetCommandText(string strCmdText)
{
	m_strCmd = strCmdText;
}

void CDb::ExcuteCmd() //ִ��������������RecordSet��
{
	if(m_strCmd.length() == 0)
		throw "����û�г�ʼ��!";
	m_pCommand->ActiveConnection = m_pConnection;
	HRESULT hr = m_pRecordset->Open(m_strCmd.c_str(),// ��ѯDemoTable���������ֶ�
		m_pConnection.GetInterfacePtr(),  // ��ȡ��ӿ��IDispatchָ��
		adOpenDynamic,
		adLockOptimistic,
		adCmdText);
	if(FAILED(hr))
		throw "���ؽ����ʱ�����!";
}

void CDb::EndCmd()
{
	HRESULT hr = m_pRecordset->Close();			
	if(FAILED(hr))
		throw "RecordSet�رճ��� !";

}

bool CDb::MoveNext() //���Ϊ����������һ��
{
	if(!m_pRecordset)
		throw "RecordSetû�г�ʼ��!";
	HRESULT hr = m_pRecordset->MoveNext();
	if(FAILED(hr))
		throw "RecordSet���ƶ�����һ�г���!";
	if(m_pRecordset->EndOfFile)
		return false;
	return true;
}

vector<WeaponInfo> CWeaponReader::GetWeapons()  //�����ݿ��ж�ȡ������Ϣ
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
		throw "��ʼ��cmd����";

	hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
	if(FAILED(hr))
		throw "��ʼ�����ݼ�����";

}




Attribute CDataReader::FillRole(string cmd)  //���������Ϣ
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

vector<CScenePath> CPathReader::GetPaths() //��ó����л�·�ߵ���Ϣ
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

RECT CDb::GetSingleRect(string strColName) //����ȡ������ת��ΪRECT
{
	wstring wstrRect((WCHAR*)_bstr_t(m_pRecordset->Fields->GetItem(_variant_t(strColName.c_str()))->Value));
	RECT rect;
	wistringstream wstream(wstrRect);
	
	wstream>>rect.left>>rect.top>>rect.right>>rect.bottom;
	return rect;

}

vector<RECT> CDb::GetVecRect(string strColName)  //����ȡ������ת��ΪRECT������
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

SceneDesc CSceneReader::GetDesc(string strSceneID)   //��ó�����������Ϣ
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

vector<vector<int> > CSceneReader::GetVec(string strColName,int iLine,int iCol)   //��ó����ľ�����Ϣ
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

vector<RECT> CSceneReader::GetVecImage(string strColName)     //�õ�һЩ�еľ�����Ϣ
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

vector<Attribute> CNpcReader::GetNpc(string strSceneID)        //��ȡĳһ����������NPC����
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

vector<int> CNpcReader::GetNpcVecID(string strSceneID)   //��ȡ������NPC��ID��
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

vector<wstring> CDataReader::GetTalkNpc(string strColName)      //���NPC�ĶԻ�
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

CScene* CSceneInit::InitScene(string strSceneID,CLead* pLead,CDraw* pcDraw,CInput* pcInput)   //�����ݿ��ʼ��ĳһ����
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

void CWeaponReader::FillItems(vector<WeaponInfo> vecWpi,CMyMenu& menuToFill)    //�������������Ϣд��˵���
{
	wstring str;
	for(int i = 0;i<(int)vecWpi.size();i++)
	{
		str = vecWpi[i].wstrName;
		menuToFill.AddItem(str);
	}
}

/*  CDb.h */
/*  Database操作类 */ 

#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","EndOfFile")
#ifndef _CDb_H_
#define _CDb_H_
#define INITGUID
#include <icrsint.h>  //简单数据库操作类的封装
#include <string>
#include <sstream>
#include <vector>
#include "cobject.h"
using namespace std;
class CDb
{
protected:
	_ConnectionPtr m_pConnection;    //SqlConnection
	_CommandPtr    m_pCommand;      //SqlCommand
	string         m_strConnString; //连接字符串
	string         m_strCmd;       //SQL命令
	_RecordsetPtr m_pRecordset;    //结果集

public:
	CDb(string strCn)
	{
		m_pConnection.CreateInstance("ADODB.Connection");
		m_strConnString = strCn;
	}
	void SetConnTimeOut(long lTimeOut)
	{
		m_pConnection->ConnectionTimeout = lTimeOut;
	}
	virtual void Initialize();    //初始化数据库连接
	void CnOpen();
	void CnClose()
	{
		m_pConnection->Close();
	}

	void SetCommandText(string strCmdText);

	void ExcuteCmd();
	void EndCmd();
	bool MoveNext();   //如果为假  则读到最后一行
	int GetInt(string strColName)
	{
		return (int)(m_pRecordset->Fields->GetItem(_variant_t(strColName.c_str()))->Value);
	}

	WCHAR* GetWChar(string strColName)  //将某一列解释为宽字符
	{
		return (WCHAR*)_bstr_t(m_pRecordset->Fields->GetItem(_variant_t(strColName.c_str()))->Value);
	}

	RECT GetSingleRect(string strColName);  //解释为单个的矩形
	vector<RECT> GetVecRect(string strColName);  //得到一系列的矩形

};

class CDataReader:protected CDb//人物信息读取
{
protected:
	CDataReader():CDb("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=db.mdb")
	{
		
	};
	Attribute FillRole(string cmd);   //填充人物信息
protected:
	Attribute Attr;
private:
	vector<wstring> GetTalkNpc(string strColName);

};

class CPathReader:private CDb//路径读取
{
public:
	CPathReader():CDb("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=db.mdb"){};
	vector<CScenePath> GetPaths();
};

class CWeaponReader:private CDb//武器装备读取
{
public:
	CWeaponReader():CDb("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=db.mdb"){};
	vector<WeaponInfo> GetWeapons();
	void FillItems(vector<WeaponInfo> vecWpi,CMyMenu& menuToFill);
};


class LeadReader:private CDataReader
{
public:
	Attribute GetAttr()
	{
		return FillRole("select * from Roles where dwID = 0");		
	}

};

class CNpcReader:private CDataReader//NPC信息读取
{
public:
    vector<Attribute> GetNpc(string strSceneID);
private:
	vector<int> GetNpcVecID(string strSceneID);
};

class CSceneReader:private CDataReader//场景读取
{
public:
	SceneDesc GetDesc(string strSceneID);
protected:
	vector<vector<int> > GetVec(string strColName,int iLine,int iCol);
	vector<RECT>         GetVecImage(string strColName);
};

class CSceneInit//场景的开关
{
private:
	vector<vector<CNpc*> > vecPtrNpc;
	CNpcReader crNpc;
	vector<LPDIRECTDRAWSURFACE7> vecSurf;
	CSceneReader sr;
		
public:
	CScene* InitScene(string strSceneID,CLead* pLead,CDraw* pcDraw,CInput* pcInput);
};

#endif


/*  CDb.h */
/*  Database������ */ 

#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","EndOfFile")
#ifndef _CDb_H_
#define _CDb_H_
#define INITGUID
#include <icrsint.h>  //�����ݿ������ķ�װ
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
	string         m_strConnString; //�����ַ���
	string         m_strCmd;       //SQL����
	_RecordsetPtr m_pRecordset;    //�����

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
	virtual void Initialize();    //��ʼ�����ݿ�����
	void CnOpen();
	void CnClose()
	{
		m_pConnection->Close();
	}

	void SetCommandText(string strCmdText);

	void ExcuteCmd();
	void EndCmd();
	bool MoveNext();   //���Ϊ��  ��������һ��
	int GetInt(string strColName)
	{
		return (int)(m_pRecordset->Fields->GetItem(_variant_t(strColName.c_str()))->Value);
	}

	WCHAR* GetWChar(string strColName)  //��ĳһ�н���Ϊ���ַ�
	{
		return (WCHAR*)_bstr_t(m_pRecordset->Fields->GetItem(_variant_t(strColName.c_str()))->Value);
	}

	RECT GetSingleRect(string strColName);  //����Ϊ�����ľ���
	vector<RECT> GetVecRect(string strColName);  //�õ�һϵ�еľ���

};

class CDataReader:protected CDb//������Ϣ��ȡ
{
protected:
	CDataReader():CDb("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=db.mdb")
	{
		
	};
	Attribute FillRole(string cmd);   //���������Ϣ
protected:
	Attribute Attr;
private:
	vector<wstring> GetTalkNpc(string strColName);

};

class CPathReader:private CDb//·����ȡ
{
public:
	CPathReader():CDb("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=db.mdb"){};
	vector<CScenePath> GetPaths();
};

class CWeaponReader:private CDb//����װ����ȡ
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

class CNpcReader:private CDataReader//NPC��Ϣ��ȡ
{
public:
    vector<Attribute> GetNpc(string strSceneID);
private:
	vector<int> GetNpcVecID(string strSceneID);
};

class CSceneReader:private CDataReader//������ȡ
{
public:
	SceneDesc GetDesc(string strSceneID);
protected:
	vector<vector<int> > GetVec(string strColName,int iLine,int iCol);
	vector<RECT>         GetVecImage(string strColName);
};

class CSceneInit//�����Ŀ���
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


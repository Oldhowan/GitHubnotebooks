#pragma once
#import "D:\downloads\msado15.dll" no_namespace rename("EOF", "adoEOF") 
#include <string>
#include <vector>
#include <iostream>
#include <atlbase.h>
#include <atlstr.h>
using namespace std;


struct _TESTRES
{
	vector<string> Name;
	vector<vector<string>> Data;
};
class ADOClass
{
public:
	ADOClass() {
		CoInitialize(NULL);
		m_pConnection.CreateInstance(_T("ADODB.Connection"));
		m_pRecordSet.CreateInstance(_T("ADODB.Recordset"));
	}
	~ADOClass() {
		if (m_pConnection)
		{
			try
			{
				//�ر����ݿ�����
				HRESULT hr = m_pConnection->Close();
				//�رռ�¼��
				m_pRecordSet->Close();
				//�ͷ�com���
				::CoUninitialize();
			}
			catch (_com_error e)
			{
				e.ErrorMessage();
			}
		}
	}

public:

	//1.�������ݿ�
	bool OpenDatabase(CString server, CString DBName, CString UseName, CString PassWord) {

		try
		{
			CString strConnectionName;
			strConnectionName.Format(_T("DATABASE=%s;DSN=myodbc;OPTION=0;PWD=%s;PORT=0;SERVER=localhost;UID=%s;"),
				DBName, PassWord, UseName);
			m_pConnection->Open(_bstr_t(strConnectionName.GetBuffer(0)), _T(""), _T(""), -1);
		}
		catch (_com_error e) {
			MessageBox(0, e.ErrorMessage(), 0, 0);
			return false;
		}return true;
	}


	//2.����SQL���
	bool ExecuteSQl(string sql) {
		try {
			_variant_t Record; m_pConnection->Execute(_bstr_t(sql.c_str()), &Record, adCmdText);
		}
		catch (_com_error e) {
			MessageBox(0, e.ErrorMessage(), 0, 0);
			return false;
		}
		return true;
	}


	//3.���ؽ����
	bool ReturnSQL(_TESTRES& tr, string sql) {
		try {//�򿪼�¼��
			m_pRecordSet->Open(_bstr_t(sql.c_str()), m_pConnection.GetInterfacePtr(), adOpenStatic, adLockOptimistic, adCmdText);
			//ȡ����
			Fields* pFields = m_pRecordSet->GetFields();
			//��ȡ�ֶ�����
			long lCount = pFields->GetCount();
			if (lCount == 0) {
				return false;
			}
			for (long i = 0; i < lCount; i++) {
				//ȡ��ÿ���ֶε�����
				FieldPtr pFieldPtr = pFields->GetItem(i);
				string colName = _com_util::ConvertBSTRToString(pFieldPtr->GetName());
				tr.Name.push_back(colName);
			}//��ȡ����
			while
				(!m_pRecordSet->GetadoEOF()) {
				vector <string> itemelem;
				for (long i = 0; i < lCount; i++) {
					//ͨ����Ż��ֶλ�ȡ��ǰ���е�����
					_variant_t value = m_pRecordSet->GetCollect(i);
					if (value.vt == VT_NULL) {
						itemelem.push_back("");
						continue;
					}
					string str = _com_util::ConvertBSTRToString(_bstr_t(value));
					itemelem.push_back(str);
				}
				tr.Data.push_back(itemelem);
				m_pRecordSet->MoveNext();
			}
		}
		catch (_com_error e) {
			MessageBox(0, e.ErrorMessage(), 0, 0);
			return false;
		}
		return true;
	}

private:
	// _ConnectionPtr�ӿ� ����һ����¼����һ����ָ�룬ͨ�������������ݿ�
	   //_RecordsetPtr�ӿ� ��һ����¼�����󣬰������ƹ��ܣ��������α��
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordSet;
};


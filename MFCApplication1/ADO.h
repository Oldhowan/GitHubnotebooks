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
				//关闭数据库连接
				HRESULT hr = m_pConnection->Close();
				//关闭记录集
				m_pRecordSet->Close();
				//释放com组件
				::CoUninitialize();
			}
			catch (_com_error e)
			{
				e.ErrorMessage();
			}
		}
	}

public:

	//1.连接数据库
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


	//2.发送SQL语句
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


	//3.返回结果集
	bool ReturnSQL(_TESTRES& tr, string sql) {
		try {//打开记录集
			m_pRecordSet->Open(_bstr_t(sql.c_str()), m_pConnection.GetInterfacePtr(), adOpenStatic, adLockOptimistic, adCmdText);
			//取列名
			Fields* pFields = m_pRecordSet->GetFields();
			//获取字段数量
			long lCount = pFields->GetCount();
			if (lCount == 0) {
				return false;
			}
			for (long i = 0; i < lCount; i++) {
				//取出每个字段的名字
				FieldPtr pFieldPtr = pFields->GetItem(i);
				string colName = _com_util::ConvertBSTRToString(pFieldPtr->GetName());
				tr.Name.push_back(colName);
			}//读取数据
			while
				(!m_pRecordSet->GetadoEOF()) {
				vector <string> itemelem;
				for (long i = 0; i < lCount; i++) {
					//通过序号或字段获取当前行中的数据
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
	// _ConnectionPtr接口 返回一个记录集或一个空指针，通常用来连接数据库
	   //_RecordsetPtr接口 是一个记录集对象，包含控制功能，锁定，游标等
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordSet;
};


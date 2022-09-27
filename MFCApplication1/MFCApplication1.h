
// MFCApplication1.h: MFCApplication1 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"       // 主符号


// CMFCApplication1App:
// 有关此类的实现，请参阅 MFCApplication1.cpp
//

class CMFCApplication1App : public CWinAppEx
{
public:
	CMFCApplication1App() noexcept;

	bool recordmodify(int pzuid, BitSet<2, 2> value1, BitSet<2, 2> value2);


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	bool recordmodify(int pzuid, BitSet<2, 2> value1, BitSet<2, 2> value2);//修改记录
	bool recordupdata(int pzuid, BitSet<2, 2> b);//更新记录
	bool modify(int uid, BitSet<2, 2> newb);//修改
	bool compare(Vehicle v1, Vehicle v2, int pzuid);//比较
	static CString Identity();//验证
	BitSet<2, 2> EnCode(CString c);//Cstring转换成位图
	CString toEnCode(BitSet<2, 2> b);
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCApplication1App theApp;

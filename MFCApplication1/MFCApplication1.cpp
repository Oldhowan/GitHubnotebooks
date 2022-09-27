
// MFCApplication1.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MFCApplication1.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "MFCApplication1Doc.h"
#include "MFCApplication1View.h"

#include <iostream>
#include <atlstr.h>
#include <string>
#include<cassert>
#include<vector>
#include<map>
#include <afxwin.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "ADO.h"

template<size_t Size, int Storage_Bits>//位图大小和位数
class BitSet
{
public:
	BitSet() {
		_bits.resize((Size / Storage_Bits), 0);
	}
	void Setzero(size_t x) {//全置位0
		size_t index_int;
		for (index_int = 0; index_int < x; index_int++) {
			_bits[index_int] &= (~(1 << index_int));
		}
	}
	void Set(size_t x)//把x映射的位标记为1
	{
		assert(x < Size);
		size_t index_int = x / Storage_Bits;
		size_t index_bit = x % Storage_Bits;
		_bits[index_int] |= (1 << index_bit);
	}
	void ReSet(size_t x)//把x映射的位标记为0.
	{
		assert(x < Size);
		size_t index_int = x / Storage_Bits;
		size_t index_bit = x % Storage_Bits;
		_bits[index_int] &= (~(1 << index_bit));
	}

	bool Find(size_t x)//查找x是否在位图上，true为存在
	{
		assert(x < Size);
		size_t index_int = x / Storage_Bits;
		size_t index_bit = x % Storage_Bits;
		return ((_bits[index_int] >> index_bit) & 1) == 1;
	}
	int getfirst() {//得到第一位字符
		return _bits[0];
	}
	int getnext() {
		return _bits[1];//得到第二位字符
	}
	bool operator == (const BitSet& _Right) const {//重载
		int nThisArrSize = Storage_Bits;
		int nRightArrSize = _Right.Storage_Bits;
		if (nThisArrSize >= nRightArrSize) {
			int i = 0;
			for (; i < nRightArrSize; ++i) {
				if (_bits[i] != _Right.getArrElem(i)) {
					return false;
				}
			}
			for (; i < nThisArrSize; ++i) {
				if (_bits[i] != static_cast<int>(0)) { //如果位数不相等，使用 0<默认> 参与运算
					return false;
				}
			}
		}
		else {
			int i = 0;
			for (; i < nThisArrSize; ++i) {
				if (_bits[i] != _Right.getArrElem(i)) {
					return false;
				}
			}
			for (; i < nRightArrSize; ++i) {
				if (_Right.getArrElem(i) != static_cast<int>(0)) { //如果位数不相等，使用 0<默认> 参与运算
					return false;
				}
			}
		}
		return true;
	}
	bool operator != (const BitSet& _Right) const {//重载
		return (!(*this == _Right));
	}

	//异或操作
	BitSet& operator ^= (const BitSet& _Right) {
		int nThisArrSize = Storage_Bits;
		int nRightArrSize = _Right.Storage_Bits;
		int i = 0;
		for (; i < nRightArrSize; ++i) {
			_bits[i] ^= _Right.getArrElem(i);
		}
		if (i < nThisArrSize) {
			for (; i < nThisArrSize; ++i) {
				_bits[i] ^= static_cast<int>(0);
			}
		}
		return *this;
	}

	int getArrElem(int indx) const {
		return _bits[indx];
	}
	void setArrElem(int index, int value) {
		vector<int>::iterator it = _bits.begin();
		for (int i = 0; i < index; i++) {
			it++;
		}
		*it = value;
	}
private:
	vector<int> _bits;
	int Storage_Bits;//位图位数
};

class Vehicle
{
public:
	CString UID;
	map<int, BitSet<2, 2>> pz;
};

// CMFCApplication1App

BEGIN_MESSAGE_MAP(CMFCApplication1App, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CMFCApplication1App::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CMFCApplication1App 构造

CMFCApplication1App::CMFCApplication1App() noexcept
{
	m_bHiColorIcons = TRUE;


	m_nAppLook = 0;
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("MFCApplication1.AppID.NoVersion"));

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CMFCApplication1App 对象

CMFCApplication1App theApp;

bool CMFCApplication1App::recordmodify(int pzuid, BitSet<2, 2> value1, BitSet<2, 2> value2) {//记录修改
	//数据库建表table record,三列对象，配置编号int 一个是value 表示修改前的值，另一个value2 表示修改后的值。
	//将传入的数据 pzuid、value1、value2保存在表中
	ADOClass adoObj;
	adoObj.OpenDatabase("localhost", "mysql", "root", "123456");
	//执行sql语句
	if (adoObj.ExecuteSQl("create table test(id int);"))
	{
		return 1;
	}
	else {
		return 0;
	}

}

bool CMFCApplication1App::recordupdata(int pzuid, BitSet<2, 2> b) {//记录新增
	//数据库建表table record,三列对象，配置编号int 一个是value 表示修改前的值，另一个value2 表示修改后的值。
	//将传入的数据 pzuid、value1、b保存在表中  value1传入空值，表示新增的配置
	//连接数据库
	ADOClass adoObj;
	adoObj.OpenDatabase("localhost", "mysql", "root", "123456");
	//执行sql语句
	if (adoObj.ExecuteSQl("create table test(id int);"))
	{
		return 1;
	}
	else {
		return 0;
	}
}

bool CMFCApplication1App::modify(int uid, BitSet<2, 2> newb) {//输入配置uid 和 配置位图
	//连接数据库 通过输入的uid找到车型的数据
	Vehicle v;
	map<int, BitSet<2, 2>>::iterator iter = v.pz.find(uid);
	if (iter != v.pz.end()) {
		//该配置存在
		//得到数据库配置值
		BitSet<2, 2> value = v.pz[uid];
		//连接数据库 将配置位图换成输入的位图newb
		v.pz[uid] = newb;
		//调用记录函数 记录结果
		recordmodify(uid, value, newb);
		return 1;
	}
	else {
		//该配置不存在
		return 0;
	}

}

bool CMFCApplication1App ::compare(Vehicle v1, Vehicle v2, int pzuid) {//比较两个车型的配置(需输入配置号）
	//连接数据库 通过传入的车型的UID得到车型的配置map<int,BitSet<2,2>> pz
	map<int, BitSet<2, 2>>::iterator iter1 = v1.pz.begin();
	map<int, BitSet<2, 2>>::iterator iter2 = v2.pz.begin();
	CString c;
	for (; iter1 != v1.pz.end(); iter1++) {//遍历v1的配置
		for (; iter2 != v2.pz.end(); iter2++) {
			if (iter1->first == iter2->first == pzuid && iter1->second == iter2->second) {
				return 1;
			}
			else if (iter1->first == iter2->first == pzuid) {
				BitSet<2, 2> b = iter1->second ^= iter2->second;
				if (b.getArrElem(0) == 0 && b.getArrElem(1) == 0) {
					return 1;
				}
				else {
					return 0;
				}
			}
		}
	}
}

CString CMFCApplication1App::Identity()          //返回电脑用户名
{
	WCHAR PCName[255];//直接使用wchar类型定义
	WCHAR UserName[255];
	unsigned long size = 255;
	GetComputerName(PCName, &size);//电脑名称
	GetUserName(UserName, &size);//电脑用户名称
	return UserName;
}

BitSet<2, 2> EnCode(CString c) {//cstring转换成位图
	std::vector<CString> vecStrItem;
	BitSet<2, 2> b;
	b.setArrElem(0, _ttoi(vecStrItem[0]));
	b.setArrElem(1, _ttoi(vecStrItem[1]));
	return b;
}

CString CMFCApplication1App::toEnCode(BitSet<2, 2> b) {//位图转换成cstring
	int a = b.getArrElem(0);
	int c = b.getArrElem(1);
	CString encode;
	encode.Format(L" % d", a+";"+c);
	return encode;
}

class User {
public:
	void setpwd(string n) {
		pwd = n;
	}
	CString getname() {
		return CMFCApplication1App::Identity();
	}
	string getpwd() {
		return pwd;
	}
	int getuid() {
		return uid;
	}
private:
	int uid;//身份认证 五种用户类型
	string pwd;
};


// CMFCApplication1App 初始化

BOOL CMFCApplication1App::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_MFCApplication1TYPE,
		RUNTIME_CLASS(CMFCApplication1Doc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CMFCApplication1View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CMFCApplication1App::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CMFCApplication1App 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CMFCApplication1App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMFCApplication1App 自定义加载/保存方法

void CMFCApplication1App::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CMFCApplication1App::LoadCustomState()
{
}

void CMFCApplication1App::SaveCustomState()
{
}

// CMFCApplication1App 消息处理程序




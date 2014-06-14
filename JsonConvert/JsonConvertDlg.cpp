// JsonConvertDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "JsonConvert.h"
#include "JsonConvertDlg.h"
#include "LoadFile.h"
#include "JsonLoader.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



char	g_CurrExeDir[_MAX_PATH];
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CJsonConvertDlg 对话框




CJsonConvertDlg::CJsonConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJsonConvertDlg::IDD, pParent)
	, m_strTemplateJSON(_T(""))
	, m_strSourceJSON(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJsonConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTemplateJSON);
	DDX_Text(pDX, IDC_EDIT2, m_strSourceJSON);
	DDX_Text(pDX, IDC_EDIT3, m_strExportJSON);

}

BEGIN_MESSAGE_MAP(CJsonConvertDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CJsonConvertDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CJsonConvertDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CJsonConvertDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDOK, &CJsonConvertDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CJsonConvertDlg 消息处理程序

BOOL CJsonConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。
	::GetCurrentDirectory(_MAX_PATH,g_CurrExeDir);
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CJsonConvertDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CJsonConvertDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CJsonConvertDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CJsonConvertDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString		szFilterFDlg = "JSON文件 (*.json)|*.json"; 
	CFileDialog hFileDlg( TRUE ,  NULL, NULL, NULL,szFilterFDlg, NULL);
	hFileDlg.m_ofn.lpstrInitialDir = g_CurrExeDir;

	if(hFileDlg.DoModal() == IDOK)
	{
		m_strTemplateJSON = hFileDlg.GetPathName();
		UpdateData(FALSE);

	}
}

void CJsonConvertDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString		szFilterFDlg = "JSON文件 (*.json)|*.json"; 
	CFileDialog hFileDlg( TRUE ,  NULL, NULL, NULL,szFilterFDlg, NULL);
	hFileDlg.m_ofn.lpstrInitialDir = g_CurrExeDir;

	if(hFileDlg.DoModal() == IDOK)
	{
		m_strSourceJSON = hFileDlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CJsonConvertDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CString		szFilterFDlg = "JSON文件 (*.json)||"; 
	CFileDialog hFileDlg( FALSE ,  NULL, NULL, NULL,szFilterFDlg, NULL);
	hFileDlg.m_ofn.lpstrInitialDir = g_CurrExeDir;

	if(hFileDlg.DoModal() == IDOK)
	{
		m_strExportJSON = hFileDlg.GetPathName();
		CString strTemp = m_strExportJSON;
		strTemp.MakeLower();
		if(strTemp.Find(".json") == -1)
		{
			m_strExportJSON += ".json";
		}
		UpdateData(FALSE);
	
	}
}

void CJsonConvertDlg::OnBnClickedOk()
{

	if(m_strTemplateJSON == "")
	{
		MessageBox("The Template Json File can't be empty !","Error",MB_OK);
		return ;
	}
	if(m_strSourceJSON == "")
	{
		MessageBox("The Source Json File can't be empty !","Error",MB_OK);
		return ;
	}
	if(m_strExportJSON == "")
	{
		MessageBox("The Target Json File can't be empty !","Error",MB_OK);
		return ;
	}



	// TODO: 在此添加控件通知处理程序代码
	JsonLoader	tCocoLoader_Old;
	size_t	 ulSize = 0;
	if( true == GetFileSize(m_strTemplateJSON ,&ulSize))
	{

		char*	pBinBuff = new char[ulSize];

		if( pBinBuff )
		{
			size_t	tRealLoadSize = 0;

			if(false == LoadFile( m_strTemplateJSON,pBinBuff,ulSize,&tRealLoadSize))
			{
				if(pBinBuff)
				{
					delete[] pBinBuff ;
					pBinBuff = NULL   ;
				}
				ulSize = 0;
				MessageBox("Load Template File Failed!","Error",MB_OK);
				return ;
			}

			DWORD dwStartTime = ::GetTickCount();
			//初始化，按旧的读取规格

			tCocoLoader_Old.ReadCocoJsonBuff(pBinBuff);


			//delete[] pBinBuff ;
			//pBinBuff = NULL   ;

		}
		ulSize = 0;
	}

	JsonLoader	tCocoLoader_New;
	ulSize = 0;
	if( true == GetFileSize(m_strSourceJSON ,&ulSize))
	{

		char*	pBinBuff = new char[ulSize];

		if( pBinBuff )
		{
			size_t	tRealLoadSize = 0;

			if(false == LoadFile(m_strSourceJSON,pBinBuff,ulSize,&tRealLoadSize))
			{
				if(pBinBuff)
				{
					delete[] pBinBuff ;
					pBinBuff = NULL   ;
				}
				ulSize = 0;
				MessageBox("Load Source Json File Failed!","Error",MB_OK);
				return ;
			}

			DWORD dwStartTime = ::GetTickCount();
			//初始化，按旧的读取规格
			tCocoLoader_New.ReadCocoJsonBuff(pBinBuff);

			delete[] pBinBuff ;
			pBinBuff = NULL   ;

		}
		ulSize = 0;
	}

	//取得JSON结点
	stJsonNode	tJsonNode_Old = tCocoLoader_Old.GetRootJsonNode();
	//取得JSON结点
	stJsonNode	tJsonNode_New = tCocoLoader_New.GetRootJsonNode();

	//测试
	stJsonNode	tGoodJsonNode = tJsonNode_New.GetGoodJsonNode(&tCocoLoader_Old);

	//打印
	const char* szPathName = m_strExportJSON;
	tGoodJsonNode.Print((void*)szPathName);

	MessageBox("Convert Finished!","OK",MB_OK);
}

BOOL CJsonConvertDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			return FALSE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

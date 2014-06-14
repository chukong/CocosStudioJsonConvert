// JsonConvertDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CJsonConvertDlg 对话框
class CJsonConvertDlg : public CDialog
{
// 构造
public:
	CJsonConvertDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_JSONCONVERT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strTemplateJSON;
	CString m_strSourceJSON;
	CString m_strExportJSON;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedOk();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

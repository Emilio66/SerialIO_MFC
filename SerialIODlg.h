
// SerialIODlg.h : 头文件
//

#pragma once
#include "SerialPort.h"
#define FIRST_CMD_LEN 7
#define SECOND_CMD_LEN 11
// CSerialIODlg 对话框
class CSerialIODlg : public CDialogEx
{
// 构造
public:
	CSerialIODlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERIALIO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnReceiveMsg(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSendCmd();
	afx_msg void OnCleanInput();
	afx_msg void OnComPortChange();
	afx_msg void OnClearMsg();
	afx_msg void OnEnd();
	
//	BOOL isChecked;	
	bool start;
	CComboBox m_combo;
	CString selected_com;
//	CString m_response;
//	CString m_send;
	CString m_needed;
	CSerialPort* myPort;
	unsigned char content[40];
};

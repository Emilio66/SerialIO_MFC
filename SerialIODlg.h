
// SerialIODlg.h : ͷ�ļ�
//

#pragma once
#include "SerialPort.h"
#define FIRST_CMD_LEN 7
#define SECOND_CMD_LEN 11
// CSerialIODlg �Ի���
class CSerialIODlg : public CDialogEx
{
// ����
public:
	CSerialIODlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SERIALIO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

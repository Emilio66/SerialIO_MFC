
// SerialIODlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SerialIO.h"
#include "SerialIODlg.h"
#include "afxdialogex.h"
#include "SerialPort.h"
#include <string>
#include "CommonUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//��ȡ����com�� (֧�ֵ�256��)
void GetComList(CComboBox * CCombox)
{
	CString  strCom,strComOpen;  
	int  nCom  =  0;  
	int  count  =  0;  
	HANDLE    hCom;

	CCombox->ResetContent();
	do  
	{  
		nCom++;  
		strCom.Format(_T("COM%d"),  nCom);
		strComOpen.Format(_T("\\\\.\\COM%d"),  nCom);
		hCom  =  CreateFile(strComOpen,  GENERIC_READ | GENERIC_WRITE,  0,  NULL,    
										OPEN_EXISTING,  FILE_FLAG_OVERLAPPED,  0);  
		if(INVALID_HANDLE_VALUE  ==  hCom  )  
		{ 
			DWORD error=::GetLastError();//ȡ�ô�����Ϣ
			continue;
		} 
		else
		{
			CCombox->AddString( strCom );
		}  
		CloseHandle(hCom);
	}  while(nCom<21);
	
	CCombox->SetCurSel(0);
}
// CSerialIODlg �Ի���
CSerialIODlg::CSerialIODlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSerialIODlg::IDD, pParent)
//	, m_send(_T(""))
//	, isChecked(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//	m_response = _T("");
	myPort = NULL;
	selected_com = _T("");
//	m_needed = _T("");
}

void CSerialIODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
//	DDX_Text(pDX, IDC_EDIT1, m_response);
//	DDX_Text(pDX, IDC_EDIT2, m_send);
	DDX_Control(pDX, IDC_COMBO1, m_combo);
	DDX_CBString(pDX, IDC_COMBO1, selected_com);
	DDX_Text(pDX, IDC_EDIT5, m_needed);
}

BEGIN_MESSAGE_MAP(CSerialIODlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CSerialIODlg::OnSendCmd)
	ON_MESSAGE(WM_RECEIVE_MSG, &CSerialIODlg::OnReceiveMsg)
	ON_BN_CLICKED(IDC_BUTTON2, &CSerialIODlg::OnClearMsg)
//	ON_BN_CLICKED(IDC_BUTTON4, &CSerialIODlg::OnCleanInput)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CSerialIODlg::OnComPortChange)
//	ON_BN_CLICKED(IDC_BUTTON6, &CSerialIODlg::OnEnd)
END_MESSAGE_MAP()


// CSerialIODlg ��Ϣ�������
BOOL CSerialIODlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	
	GetComList(&m_combo);	//�õ����е�COM��
	int index = m_combo.GetCurSel();
	m_combo.GetLBText(index,selected_com);
	this->start = true;

	//set initial position
	SetWindowPos(&wndTop,400,400,0,0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_SHOWWINDOW );
	ShowWindow(SW_SHOW);
	UpdateWindow();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSerialIODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSerialIODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/************************************************/
//////////// serial port read& write /////////////////////
/** ����ָ��˳��
* 7E 00 00 01 C1 00 0D	//������
* 7E 20 00 01 A0 00 0D
* 7E 20 00 05 A6 60 02 00 20 00 0D 
* 7E 00 00 01 C0 00 0D	//�ر�����
*/

void CSerialIODlg::OnSendCmd()
{
	CString init("�򿪴���");
	init+=selected_com;
	init+=_T("ʧ�ܣ�");
	CString write("����д��ʧ�ܣ�");
	CString read("�����޷���ȡ���ݣ�");
	this->start = true;
	unsigned char open[FIRST_CMD_LEN]={0x7E, 0, 0 ,1 ,0xC1 ,0 ,0x0D};

	//single instance
	if(this->myPort == NULL){
		myPort = new CSerialPort();
		myPort->hwnd = GetSafeHwnd();
		
		if(!myPort->InitPort(selected_com)	)//init com 1
			MessageBox(init);
		
		if(!myPort->WriteData(open, FIRST_CMD_LEN))
			MessageBox(write);

		if(!myPort->OpenListenThread())
			MessageBox(read);
	}else if(!myPort->WriteData(open, FIRST_CMD_LEN))
		MessageBox(write);
	
/*	UpdateData(true);
	char * send = (char *) malloc(FIRST_CMD_LEN*2+1);//save hex string
	HexDisplay(open,send,FIRST_CMD_LEN);
	CString tmp(send);
	m_send.Append(tmp);
	m_send+="\r\n";
	free(send);
	UpdateData(false);*/
}

/*
Received message:
7E00000100000D
7E00000100000D
7E200001FF000D
7E00000100000D
*/
	//message processor
	//The 3rd-4th byte indicate return length
	/*	0x0001FF wrong
		0x000100 wireless antenna on/off
		0x000900 return ID
		0x002100 return file content
	*/
afx_msg LRESULT CSerialIODlg::OnReceiveMsg(WPARAM wParam, LPARAM lParam)
{
	//get message from worker thread, used to update UI component
	unsigned char * p_msg = (unsigned char *)lParam;
	int size = wParam;

	UpdateData(TRUE);	
	switch(p_msg[3]){
		case 1:
			if(p_msg[4] == 0 && this->start){				
				//send command 1
				unsigned char cmd1[FIRST_CMD_LEN]={0x7E,0x20,0,1,0xA0,0,0x0D};
			/*	char * hex = (char *)malloc(FIRST_CMD_LEN*2+1);
				HexDisplay(cmd1,hex,FIRST_CMD_LEN);
				CString tmp(hex);
				this->m_send.Append(tmp);
				this->m_send+="\r\n";*/
				if(!myPort->WriteData(cmd1, FIRST_CMD_LEN))
					MessageBox(_T("����д��ʧ�ܣ�"));
				//free(hex);
			}else{
				//end of reading
				if(!this->start)
					break;
				MessageBox(_T("δ��⵽���ܿ�"));
			}
			break;
		case 9:		
			if(p_msg[4]==0){
				//save this info, send command 2
				for(int i=0;i<8;i++)
					this->content[i] = p_msg[5+i];
				unsigned char cmd2[SECOND_CMD_LEN]= {0x7E,0x20,0,5,0xA6,0x60,2,0,0x20,0,0x0D};
			/*	char * hex = (char *)malloc(SECOND_CMD_LEN*2+1);
				HexDisplay(cmd2,hex,SECOND_CMD_LEN);
				CString tmp(hex);
				this->m_send.Append(tmp);
				this->m_send+="\r\n";*/
				if(!myPort->WriteData(cmd2, SECOND_CMD_LEN))
					MessageBox(_T("����д��ʧ�ܣ�"));
				//free(hex);
			}else{
				MessageBox(_T("δ��⵽���ܿ�"));
			}
			break;
		case 0x21:	
			//save this info, combine with the former, display them in a critical place and close the wireless module
			if(p_msg[4] == 0){
				for(int i=0;i<32;i++)
					this->content[i+8] = p_msg[5+i];	
				char * msg = (char *)malloc(81);
				HexDisplay(this->content,msg,40);
				CString tmp(msg);
				this->m_needed = tmp;

				this->start =false;		//end reading
				unsigned char close[FIRST_CMD_LEN]={0x7E, 0, 0 ,1 ,0xC0 ,0 ,0x0D};
				/*char * hex = (char *)malloc(FIRST_CMD_LEN*2+1);
				HexDisplay(close,hex,FIRST_CMD_LEN);
				CString tmp2(hex);
				this->m_send.Append(tmp2);
				this->m_send+="\r\n";*/
				if(!myPort->WriteData(close,FIRST_CMD_LEN))
					MessageBox(_T("д�봮��ʧ�ܣ�"));
				
				//free(hex);
				free(msg);
			}else{
				MessageBox(_T("δ��⵽���ܿ�"));
			}
			break;
		default: 
			MessageBox(_T("����ֵ�쳣"));
			break;//unknown message
	}
	
/*	char * buffer =(char *)malloc(size*2+1);
	HexDisplay(p_msg,buffer,size);
	CString buf(buffer);
	m_response.Append(buf);
	m_response += "\r\n";*/
	UpdateData(FALSE);

	free(p_msg);
	//free(buffer);
	return 0;
}

//CLEAR SCREEN
void CSerialIODlg::OnClearMsg()
{
	UpdateData(TRUE);
//	m_response.Empty();
	m_needed.Empty();
	UpdateData(FALSE);
}


//	unsigned char tmp2[SECOND_CMD_LEN]= {0x7E,0x20,0,5,0xA6,0x60,2,0,0x20,0,0x0D};

void CSerialIODlg::OnCleanInput()
{
	UpdateData(TRUE);
//	m_send.Empty();
	UpdateData(FALSE);
}


void CSerialIODlg::OnComPortChange()
{
	int portNo = m_combo.GetCurSel();
	m_combo.GetLBText(portNo,selected_com);
	//close last open port and clean all the stuff
	if(myPort != NULL){
		myPort->CloseListenTread();
		myPort->ClosePort();
		myPort = NULL;	//clean this port, it will be triggered by other sending message behavior
	}
}

//�ر�����
void CSerialIODlg::OnEnd()
{
	if(this->myPort == NULL){
		MessageBox(_T("���ȵ����ʼ��ȡ"));
		return ;
	}

	this->start =false;		//end reading
	unsigned char close[FIRST_CMD_LEN]={0x7E, 0, 0 ,1 ,0xC0 ,0 ,0x0D};

	if(!myPort->WriteData(close,FIRST_CMD_LEN))
		MessageBox(_T("д�봮��ʧ�ܣ�"));
		
	UpdateData(true);
	char * send = (char *) malloc(FIRST_CMD_LEN*2+1);//save hex string
	HexDisplay(close,send,FIRST_CMD_LEN);
	CString tmp(send);
	//m_send.Append(tmp);
	//m_send+="\r\n";
	free(send);
	UpdateData(false);
}


// SerialIODlg.cpp : 实现文件
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

//获取可用com口 (支持到256个)
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
			DWORD error=::GetLastError();//取得错误信息
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
// CSerialIODlg 对话框
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


// CSerialIODlg 消息处理程序
BOOL CSerialIODlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	
	GetComList(&m_combo);	//得到所有的COM口
	int index = m_combo.GetCurSel();
	m_combo.GetLBText(index,selected_com);
	this->start = true;

	//set initial position
	SetWindowPos(&wndTop,400,400,0,0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_SHOWWINDOW );
	ShowWindow(SW_SHOW);
	UpdateWindow();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSerialIODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSerialIODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/************************************************/
//////////// serial port read& write /////////////////////
/** 发送指令顺序
* 7E 00 00 01 C1 00 0D	//打开天线
* 7E 20 00 01 A0 00 0D
* 7E 20 00 05 A6 60 02 00 20 00 0D 
* 7E 00 00 01 C0 00 0D	//关闭天线
*/

void CSerialIODlg::OnSendCmd()
{
	CString init("打开串口");
	init+=selected_com;
	init+=_T("失败！");
	CString write("串口写入失败！");
	CString read("串口无法读取数据！");
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
					MessageBox(_T("串口写入失败！"));
				//free(hex);
			}else{
				//end of reading
				if(!this->start)
					break;
				MessageBox(_T("未检测到智能卡"));
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
					MessageBox(_T("串口写入失败！"));
				//free(hex);
			}else{
				MessageBox(_T("未检测到智能卡"));
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
					MessageBox(_T("写入串口失败！"));
				
				//free(hex);
				free(msg);
			}else{
				MessageBox(_T("未检测到智能卡"));
			}
			break;
		default: 
			MessageBox(_T("返回值异常"));
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

//关闭天线
void CSerialIODlg::OnEnd()
{
	if(this->myPort == NULL){
		MessageBox(_T("请先点击开始读取"));
		return ;
	}

	this->start =false;		//end reading
	unsigned char close[FIRST_CMD_LEN]={0x7E, 0, 0 ,1 ,0xC0 ,0 ,0x0D};

	if(!myPort->WriteData(close,FIRST_CMD_LEN))
		MessageBox(_T("写入串口失败！"));
		
	UpdateData(true);
	char * send = (char *) malloc(FIRST_CMD_LEN*2+1);//save hex string
	HexDisplay(close,send,FIRST_CMD_LEN);
	CString tmp(send);
	//m_send.Append(tmp);
	//m_send+="\r\n";
	free(send);
	UpdateData(false);
}

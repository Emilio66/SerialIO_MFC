#ifndef SERIALPORT_H_
#define SERIALPORT_H_
#include "stdafx.h"
#include <Windows.h>


#define WM_RECEIVE_MSG WM_USER+1

/** ����ͨ����
 *   
 *  ����ʵ���˶Դ��ڵĻ�������
 *  �����������ָ�����ڵ����ݡ�����ָ�����ݵ�����
 */
class CSerialPort
{
public:
	CSerialPort(void);
	~CSerialPort(void);

public:
	
	/** ��ʼ�����ں���
	 *
	 *  @param:  CString portNo ���ڱ��,Ĭ��ֵΪCOM1
	 *  @param:  UINT baud   ������,Ĭ��Ϊ9600
	 *  @param:  char parity �Ƿ������żУ��,'Y'��ʾ��Ҫ��żУ��,'N'��ʾ����Ҫ��żУ��
	 *  @param:  UINT databits ����λ�ĸ���,Ĭ��ֵΪ8������λ
	 *  @param:  UINT stopsbits ֹͣλʹ�ø�ʽ,Ĭ��ֵΪ1
	 *  @param:  DWORD dwCommEvents Ĭ��ΪEV_RXCHAR,��ֻҪ�շ�����һ���ַ�,�����һ���¼�
	 *  @return: bool  ��ʼ���Ƿ�ɹ�
	 *  @note:   ��ʹ�����������ṩ�ĺ���ǰ,���ȵ��ñ��������д��ڵĳ�ʼ��
	 *���������� \n�������ṩ��һЩ���õĴ��ڲ�������,����Ҫ����������ϸ��DCB����,��ʹ�����غ���
	 *           \n������������ʱ���Զ��رմ���,�������ִ�йرմ���
	 *  @see:    
	 */
	bool InitPort( CString  portN = _T("COM1"),UINT  baud = CBR_9600,char  parity = 'N',UINT  databits = 8, 
		           UINT  stopsbits = 1,DWORD dwCommEvents = EV_RXCHAR);

	/** ���ڳ�ʼ������
	 *
	 *  �������ṩֱ�Ӹ���DCB�������ô��ڲ���
	 *  @param:  UINT portNo
	 *  @param:  const LPDCB & plDCB
	 *  @return: bool  ��ʼ���Ƿ�ɹ�
	 *  @note:   �������ṩ�û��Զ���ش��ڳ�ʼ������
	 *  @see:    
	 */
	bool InitPort( CString  portN ,const LPDCB& plDCB );

	/** ���������߳�
	 *
	 *  �������߳���ɶԴ������ݵļ���,�������յ������ݴ�ӡ����Ļ���
	 *  @return: bool  �����Ƿ�ɹ�
	 *  @note:   ���߳��Ѿ����ڿ���״̬ʱ,����flase
	 *  @see:    
	 */
	bool OpenListenThread();

	/** �رռ����߳�
	 *
	 *  
	 *  @return: bool  �����Ƿ�ɹ�
	 *  @note:   ���ñ�������,�������ڵ��߳̽��ᱻ�ر�
	 *  @see:    
	 */
	bool CloseListenTread();

    /** �򴮿�д����
	 *
	 *  ���������е�����д�뵽����
	 *  @param:  unsigned char * pData ָ����Ҫд�봮�ڵ����ݻ�����
	 *  @param:  unsigned int length ��Ҫд������ݳ���
	 *  @return: bool  �����Ƿ�ɹ�
	 *  @note:   length��Ҫ����pData��ָ�򻺳����Ĵ�С
	 *  @see:    
	 */
	bool WriteData(unsigned char* pData, unsigned int length);
/** �߳��˳���־���� */ 
	static bool s_bExit;
	/** ��ȡ���ڻ������е��ֽ���
	 *
	 *  
	 *  @return: UINT  �����Ƿ�ɹ�
	 *  @note:   �����ڻ�������������ʱ,����0
	 *  @see:    
	 */
	UINT GetBytesInCOM();

	/** ��ȡ���ڽ��ջ������ж���ֽڵ�����
	 *
	 *  
	 *  @param:  char & cRecved ��Ŷ�ȡ���ݵ��ַ�����
	 *  @return: int  ��ȡ�ĳ���
	 *  @note:   
	 *  @see:    
	 */
	int ReadChar(unsigned char * data, int length);

	//show response message, got main thread's handle
	HWND hwnd;

	/** �򿪴���
	 *
	 *  
	 *  @param:  CString portNo �����豸��
	 *  @return: bool  ���Ƿ�ɹ�
	 *  @note:   
	 *  @see:    
	 */
	bool openPort( CString  portN );

	/** �رմ���
	 *
	 *  
	 *  @return: void  �����Ƿ�ɹ�
	 *  @note:   
	 *  @see:    
	 */
	void ClosePort();
	
	/** ���ڼ����߳�
	 *
	 *  �������Դ��ڵ����ݺ���Ϣ
	 *  @param:  void * pParam �̲߳���
	 *  @return: UINT WINAPI �̷߳���ֵ
	 *  @note:   
	 *  @see:    
	 */
	static UINT WINAPI ListenThread(void* pParam);

	/** ���ھ�� */ 
	HANDLE  m_hComm;

	

	/** �߳̾�� */ 
	volatile HANDLE    m_hListenThread;

	/** ͬ������,�ٽ������� */ 
	CRITICAL_SECTION   m_csCommunicationSync;       //!< �����������

};

#endif //SERIALPORT_H_
/*-----------------------------------------
* Copyright (c) 2008 Eric Wong
* ����������߲ο������������κ���ҵ��Ϊ
*
* �ļ����ƣ� TCPClient_CE.h
* �ļ���ʶ�� 
* ժҪ�����ڷ�װTCP �ͻ���ͨѶ
*
* ��ǰ�汾�� 1.0
* ���ߣ� ���� Eric Wong
* ������ڣ� 2008��1��19��
*
* ȡ���汾��
* ԭ���ߣ� 
* ������ڣ� 
----------------------------------------*/
#pragma once

#include <winsock.h>

//�������ӶϿ��¼�
typedef void (CALLBACK* ONDISCONNECT)(void* pOwner);
//���嵱�����ݽ����¼�
typedef void (CALLBACK* ONREAD)(void* pOwner,const char * buf,DWORD dwBufLen );
//����Socket�����¼�
typedef void (CALLBACK* ONERROR)(void* pOwner,int nErrorCode);

class CTCPClient_CE
{
public:
	CTCPClient_CE(void);
	~CTCPClient_CE(void);
public:
	//Զ������IP��ַ
	CString	 m_remoteHost;  
	//Զ�������˿�
    int      m_port;
	/*--���¿ͻ���ͨѶ�¼�--*/
	//���ӶϿ��¼����ص�����
	ONDISCONNECT OnDisConnect;
	//���������¼����ص�����
	ONREAD       OnRead;
	//���������¼����ص�����
	ONERROR      OnError;
private:
	//ͨѶSocket���
	SOCKET m_socket;      
	//ͨѶ�߳��˳��¼����
	HANDLE m_exitThreadEvent; 
	//ͨѶ�߳̾��
	HANDLE m_tcpThreadHandle;
	//��������
	void * m_pOwner;
	//���ջ�����
	char m_recvBuf[4096];
private:
    //ͨѶ�̺߳���
	static DWORD SocketThreadFunc(LPVOID lparam);
public:
	//���ڴ򿪿ͻ���socket
	BOOL Open(void * pOwner);
public:
	//���ڹرտͻ���socket
	BOOL Close();
public:
	//���ڽ�����TCP����������
	BOOL Connect();
public:
	//��������˷�������
	BOOL SendData(const char * buf , DWORD dwBufLen);
};

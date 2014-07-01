/*-----------------------------------------
* Copyright (c) 2008 Eric Wong
* ����������߲ο������������κ���ҵ��Ϊ
*
* �ļ����ƣ� TCPCustom_CE.h
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

#include "winsock.h"
#include "TCPServer_CE.h"

class CTCPCustom_CE
{
public:
	CTCPCustom_CE(void);
	~CTCPCustom_CE(void);
private:
    //ͨѶ�̺߳���
	static DWORD SocketThreadFunc(PVOID lparam);
public:
	//��socket������ͨѶ�߳�
	BOOL Open(CTCPServer_CE *pTCPServer);
	//�ر�socket���ر��̣߳��ͷ�Socket��Դ
	BOOL Close();
public:
	//��������
	BOOL SendData(const char * buf , DWORD dwBufLen);
public:
	CTCPServer_CE * m_pTCPServer_CE; //����TCP����˼���Socket
	CString m_RemoteHost; //Զ������IP��ַ
	DWORD m_RemotePort; //Զ�������˿ں�
	SOCKET m_socket;      //ͨѶSocket���
private:
	HANDLE m_exitThreadEvent;  //ͨѶ�߳��˳��¼����
	HANDLE m_tcpThreadHandle;  //ͨѶ�߳̾��
};

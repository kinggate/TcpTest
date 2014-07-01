/*-----------------------------------------
* Copyright (c) 2008 Eric Wong
* ����������߲ο������������κ���ҵ��Ϊ
*
* �ļ����ƣ� TCPClient_CE.cpp
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
#include "StdAfx.h"
#include "TCPClient_CE.h"

//���캯��
CTCPClient_CE::CTCPClient_CE()
{
	//��ʼ��socket����
	WSADATA wsd;
	WSAStartup(MAKEWORD(2,2),&wsd);
	//�ÿջ�����
	ZeroMemory(m_recvBuf,4096);
	OnDisConnect = NULL;	//���ӶϿ��¼����ص�����
	OnRead = NULL;	//���������¼����ص�����
	OnError = NULL;	//���������¼����ص�����
	//�����߳��˳��¼����
	m_exitThreadEvent = CreateEvent(NULL,FALSE,FALSE,L"EVENT_TCP_CLIENT_THREAD");
}

//��������
CTCPClient_CE::~CTCPClient_CE()
{
	//�ر��߳��˳��¼����
	CloseHandle(m_exitThreadEvent);
	//�ͷ�socket��Դ
	WSACleanup();
}


/*------------------------------------------------------------------
���������ܡ�:  ���߳����ڼ���TCP�ͻ���ͨѶ���¼������統���յ����ݡ�
���ӶϿ���ͨѶ���̷���������¼�
����ڲ�����:  lparam:������ָ�룬����ͨ���˲��������߳��д�����Ҫ�õ�����Դ��
���������ǽ�CTCPClient_CE��ʵ��ָ�봫����
�����ڲ�����:  (��)
������  ֵ��:  ����ֵû���ر�����壬�ڴ����ǽ�����ֵ��Ϊ0��
------------------------------------------------------------------*/
DWORD CTCPClient_CE::SocketThreadFunc(LPVOID lparam)
{
	CTCPClient_CE *pSocket;
	//�õ�CTCPClient_CEʵ��ָ��
	pSocket = (CTCPClient_CE*)lparam;
	//������¼�����
	fd_set fdRead;
	int ret;
	//�����¼��ȴ�ʱ��
	TIMEVAL	aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 0;
	while (TRUE)
	{
		//�յ��˳��¼��������߳�
		if (WaitForSingleObject(pSocket->m_exitThreadEvent,0) == WAIT_OBJECT_0)
		{
			break;
		}
		//�ÿ�fdRead�¼�Ϊ��
		FD_ZERO(&fdRead);
		//���ͻ���socket���ö��¼�
		FD_SET(pSocket->m_socket,&fdRead);
		//����select�������ж��Ƿ��ж��¼�����
		ret = select(0,&fdRead,NULL,NULL,&aTime);

		if (ret == SOCKET_ERROR)
		{
			if (pSocket->OnError)
			{
				//���������¼�
				pSocket->OnError(pSocket->m_pOwner,1);
			}
			if (pSocket->OnDisConnect)
			{
				//�������ӶϿ��¼�
				pSocket->OnDisConnect(pSocket->m_pOwner);
			}
			//�رտͻ���socket
			closesocket(pSocket->m_socket);
			break;
		}

		if (ret > 0)
		{
			//�������¼�
			if (FD_ISSET(pSocket->m_socket,&fdRead))
			{
				ZeroMemory(pSocket->m_recvBuf,4096);
				//��������
				int recvLen = recv(pSocket->m_socket,pSocket->m_recvBuf, 4096,0); 
				if (recvLen == SOCKET_ERROR)
				{
					int iError = WSAGetLastError();
					if (pSocket->OnError)
					{
						//����socket�����¼�
						pSocket->OnError(pSocket->m_pOwner,iError);
					}
					if (pSocket->OnDisConnect)
					{
						//������������Ͽ��¼�
						pSocket->OnDisConnect(pSocket->m_pOwner);
					}
					//�رտͻ���socket
					closesocket(pSocket->m_socket);
					break;
				}
				else if (recvLen == 0)
				{
					if (pSocket->OnDisConnect)
					{
						//������������˶Ͽ��¼�
						pSocket->OnDisConnect(pSocket->m_pOwner);
					}
					//�رտͻ���socket
					closesocket(pSocket->m_socket);
					break;
				}
				else
				{
					if (pSocket->OnRead)
					{
						//�������ݽ����¼�
						pSocket->OnRead(pSocket->m_pOwner,pSocket->m_recvBuf,recvLen);
					}
				}
			}
		}
	}
	TRACE(L"�ͻ����߳��˳�\n");
	return 0;
}

/*-----------------------------------------------------------------
���������ܡ�: ���ڴ򿪿ͻ���socket
����ڲ�����: pOwner ����ָ��������
�����ڲ�����: (��)
������  ֵ��: TRUE:�򿪳ɹ�;FALSE:��ʧ��
-----------------------------------------------------------------*/
BOOL CTCPClient_CE::Open(void * pOwner)
{
	//��λ�߳��˳��¼�
	ResetEvent(m_exitThreadEvent);
	//�洢������ָ��
	m_pOwner = pOwner;
	//����TCP�׽��� 
	m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_socket == SOCKET_ERROR)
	{
		return FALSE;
	}
	//����ͨѶ�߳�
	m_tcpThreadHandle = CreateThread(NULL,0,SocketThreadFunc,this,0,NULL);
	if (m_tcpThreadHandle == NULL)
	{
		closesocket(m_socket);
		return FALSE;
	}
	return TRUE;
}

/*----------------------------------------------------------------
���������ܡ�: ���ڹرտͻ���socket
����ڲ�����:  (��)
�����ڲ�����:  (��)
������  ֵ��: TRUE:�رճɹ�;FALSE:�ر�ʧ��
-----------------------------------------------------------------*/
BOOL CTCPClient_CE::Close()
{
	//����ͨѶ�߳̽����¼�
	SetEvent(m_exitThreadEvent);

	//�ȴ�1�룬������߳�û���˳�����ǿ���˳�
	if (WaitForSingleObject(m_tcpThreadHandle,1000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_tcpThreadHandle,0);
		TRACE(L"ǿ����ֹ�ͻ����߳�\n");
	}
	m_tcpThreadHandle = NULL;
	//�ر�Socket���ͷ���Դ
	int err = closesocket(m_socket);
	if (err == SOCKET_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}

/*-----------------------------------------------------------------
���������ܡ�: ���ڽ�����TCP����������
����ڲ�����: (��)
�����ڲ�����: (��)
������  ֵ��: TRUE:�������ӳɹ�;FALSE:��������ʧ��
------------------------------------------------------------------*/
BOOL CTCPClient_CE::Connect()
{
	struct sockaddr_in addr;
	int err;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_port);
	//�˴�Ҫ��˫�ֽ�ת���ɵ��ֽ�
	char ansiRemoteHost[255];
	ZeroMemory(ansiRemoteHost,255);
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,m_remoteHost,wcslen(m_remoteHost)
		,ansiRemoteHost,wcslen(m_remoteHost),NULL,NULL);

	addr.sin_addr.s_addr=inet_addr(ansiRemoteHost);
	//��ʱ����ͬ�����ӷ�ʽ,connectֱ�ӷ��سɹ�����ʧ��
	err = connect(m_socket,(struct sockaddr *)&addr,sizeof(addr));
	if (err == SOCKET_ERROR) 
	{
		return FALSE;
	}
	//����ͨѶģʽΪ�첽ģʽ
	DWORD ul= 1;
	ioctlsocket(m_socket,FIONBIO,&ul);
	return TRUE;
}

/*-----------------------------------------------------------------
���������ܡ�: ��������˷�������
����ڲ�����: buf:�����͵�����
dwBufLen:�����͵����ݳ���
�����ڲ�����: (��)
������  ֵ��: TRUE:�������ݳɹ�;FALSE:��������ʧ��
------------------------------------------------------------------*/
BOOL CTCPClient_CE::SendData(const char * buf , DWORD dwBufLen)
{
	int nBytes = 0;
	int nSendBytes=0;

	while (nSendBytes < dwBufLen)
	{
		nBytes = send(m_socket,buf+nSendBytes,dwBufLen-nSendBytes,0);
		if (nBytes==SOCKET_ERROR )
		{
			int iErrorCode = WSAGetLastError();
			//����socket��Error�¼�
			if (OnError)
			{
				OnError(m_pOwner,iErrorCode);
			}
			//������������˶Ͽ������¼�
			if (OnDisConnect)
			{
				OnDisConnect(m_pOwner);
			}
			//�ر�socket
			Close();
			return FALSE;
		}
		
		//�ۼƷ��͵��ֽ���
		nSendBytes = nSendBytes + nBytes;

		if (nSendBytes < dwBufLen)
		{
			Sleep(1000);
		}
	} 
	return TRUE; 
}


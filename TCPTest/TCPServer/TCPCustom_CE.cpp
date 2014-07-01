/*-----------------------------------------
* Copyright (c) 2008 Eric Wong
* ����������߲ο������������κ���ҵ��Ϊ
*
* �ļ����ƣ� TCPCustom_CE.cpp
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
#include "TCPCustom_CE.h"

CTCPCustom_CE::CTCPCustom_CE(void)
{
}

CTCPCustom_CE::~CTCPCustom_CE(void)
{
}

/*----------------------------------------------------------------
���������ܡ�:  ���߳����ڼ�����ͻ������ӵ�socketͨѶ���¼������統���յ����ݡ�
���ӶϿ���ͨѶ���̷���������¼�
����ڲ�����:  lparam:������ָ�룬����ͨ���˲��������߳��д�����Ҫ�õ�����Դ��
���������ǽ�CTCPCustom_CE��ʵ��ָ�봫����
�����ڲ�����:  (��)
������  ֵ��:  ����ֵû���ر�����壬�ڴ����ǽ�����ֵ��Ϊ0��
-----------------------------------------------------------------*/
DWORD CTCPCustom_CE::SocketThreadFunc(PVOID lparam)
{
	CTCPCustom_CE *pSocket;
	//�õ�CTCPCustom��ʵ��ָ��
	pSocket = (CTCPCustom_CE*)lparam;
	//������¼�����
	fd_set fdRead;  
	int ret;
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
		//�ÿն��¼�����
		FD_ZERO(&fdRead);
		//��pSocket���ö��¼�
		FD_SET(pSocket->m_socket,&fdRead);
		//����select�������ж��Ƿ��ж��¼�����
		ret = select(0,&fdRead,NULL,NULL,&aTime);

		if (ret == SOCKET_ERROR)
		{
			if (pSocket->m_pTCPServer_CE->OnClientError)
			{
				//���������¼�
				pSocket->m_pTCPServer_CE->OnClientError(pSocket->m_pTCPServer_CE->m_pOwner,pSocket,1);
			}
			//�ر�socket
			closesocket(pSocket->m_socket);
			break;
		}

		if (ret > 0)
		{
			//�ж��Ƿ���¼�
			if (FD_ISSET(pSocket->m_socket,&fdRead))
			{
				char recvBuf[1024];
				int recvLen;
				ZeroMemory(recvBuf,1024);
				recvLen = recv(pSocket->m_socket,recvBuf, 1024,0); 
				if (recvLen == SOCKET_ERROR)
				{
					int nErrorCode = WSAGetLastError();
					//������ͻ��˶����ӵ�Socket����
					if (pSocket->m_pTCPServer_CE->OnClientError)
					{
						pSocket->m_pTCPServer_CE->OnClientError(pSocket->m_pTCPServer_CE->m_pOwner,pSocket,nErrorCode);
					}
					//������ͻ��˶����ӵ�Socket�ر��¼�
					if (pSocket->m_pTCPServer_CE->OnClientClose)
					{
						pSocket->m_pTCPServer_CE->OnClientClose(pSocket->m_pTCPServer_CE->m_pOwner,pSocket);
					}
					//�ر�socket
					closesocket(pSocket->m_socket);
					//�Ƴ��ͻ���
					pSocket->m_pTCPServer_CE->RemoteClient(pSocket);
					break;

				}
				//��ʾ�����Ѿ����ݹر�
				else if (recvLen == 0)
				{
					if (pSocket->m_pTCPServer_CE->OnClientClose)
					{
						pSocket->m_pTCPServer_CE->OnClientClose(pSocket->m_pTCPServer_CE->m_pOwner,pSocket);
					}
					//�ر�socket
					closesocket(pSocket->m_socket);
					//�Ƴ��ͻ���
					pSocket->m_pTCPServer_CE->RemoteClient(pSocket);
					break;
				}
				else
				{
					//������ͻ��˶����ӵ�Socket���¼�
					if (pSocket->m_pTCPServer_CE->OnClientRead)
					{
						pSocket->m_pTCPServer_CE->OnClientRead(pSocket->m_pTCPServer_CE->m_pOwner,pSocket,recvBuf,recvLen);
					}
				}
			}
		}
	}
	TRACE(L"�ͻ����߳��˳�\n");
	return 0;
}

/*-----------------------------------------------------------------
���������ܡ�: ��socket������ͨѶ�߳�
����ڲ�����:  pTCPServerָ��������˼���socket
�����ڲ�����:  (��)
������  ֵ��:  TRUE:�򿪳ɹ�;FALSE:��ʧ��
------------------------------------------------------------------*/
BOOL CTCPCustom_CE::Open(CTCPServer_CE *pTCPServer)
{
	CString strEvent;
	strEvent.Format(L"EVENT_CLIENT_THREAD_EXIT %d",m_socket);
	//�����߳��˳��¼�
	m_exitThreadEvent = CreateEvent(NULL,FALSE,FALSE,strEvent);

	//����ͨѶ�߳�
	m_tcpThreadHandle = CreateThread(NULL,0,SocketThreadFunc,this,0,NULL);
	if (m_tcpThreadHandle == NULL)
	{
		closesocket(m_socket);
		return FALSE;
	}
	//����ͨѶģʽΪ�첽ģʽ
	DWORD ul= 1;
	ioctlsocket(m_socket,FIONBIO,&ul);
	m_pTCPServer_CE = pTCPServer;
	return TRUE;
}

/*-----------------------------------------------------------------
���������ܡ�: �ر�socket���ر��̣߳��ͷ�Socket��Դ
����ڲ�����:  (��)
�����ڲ�����:  (��)
������  ֵ��:  TRUE:�ɹ��ر�;FALSE:�ر�ʧ��
------------------------------------------------------------------*/
BOOL CTCPCustom_CE::Close()
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
	//�رվ��
	CloseHandle(m_exitThreadEvent);
	//�ر�Socket���ͷ���Դ
	int err = closesocket(m_socket);
	if (err == SOCKET_ERROR)
	{
		return FALSE;
	}

	TRACE(L"�ͻ��˶��󱻳ɹ��ر�\n");
	return TRUE;
}

/*-----------------------------------------------------------------
���������ܡ�: ��ͻ��˷�������
����ڲ�����: buf:�����͵�����
dwBufLen:�����͵����ݳ���
�����ڲ�����: (��)
������  ֵ��: TRUE:�������ݳɹ�;FALSE:��������ʧ��
------------------------------------------------------------------*/
BOOL CTCPCustom_CE::SendData(const char * buf , DWORD dwBufLen)
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
			if (m_pTCPServer_CE->OnClientError)
			{
				m_pTCPServer_CE->OnClientError(m_pTCPServer_CE->m_pOwner,this,iErrorCode);
			}
			//������������˶Ͽ������¼�
			if (m_pTCPServer_CE->OnClientClose)
			{
				m_pTCPServer_CE->OnClientClose(m_pTCPServer_CE->m_pOwner,this);
			}
			//�ر�socket
			Close();
			return FALSE;
		}

		nSendBytes = nSendBytes + nBytes;

		if (nSendBytes < dwBufLen)
		{
			Sleep(1000);
		}
	} 
	return TRUE; 
}

/*-----------------------------------------
* Copyright (c) 2008 Eric Wong
* ����������߲ο������������κ���ҵ��Ϊ
*
* �ļ����ƣ� TCPServer_CE.cpp
* �ļ���ʶ�� 
* ժҪ�����ڷ�װTCP �����ͨѶ
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
#include "TCPServer_CE.h"

#include "TCPCustom_CE.h" 
#include <afxtempl.h>
//�洢�ͻ���Socket���
CPtrList m_ListClientSocket;

//���캯��
CTCPServer_CE::CTCPServer_CE()
{
	//�����߳��˳��¼����
	m_exitThreadEvent = CreateEvent(NULL,FALSE,FALSE,L"EVENT_SERVER_THREAD_QUIT");

	//�ͻ������ӽ����¼����ص�����
	OnClientConnect = NULL;
	//�ͻ������ӶϿ��¼����ص�����
	OnClientClose = NULL;
	//�ͻ��˽��������¼����ص�����
	OnClientRead = NULL;
	//�ͻ��˷��������¼����ص�����
	OnClientError = NULL;
	//�������˷��������¼�,�ص�����
	OnServerError = NULL;
}
//��������
CTCPServer_CE::~CTCPServer_CE()
{
	//�ر��߳��˳��¼����
	CloseHandle(m_exitThreadEvent);
}

/*-----------------------------------------------------------------
���������ܡ�:  ���߳����ڼ������׽����¼���
����ڲ�����:  lparam:������ָ�룬����ͨ���˲��������߳��д�����Ҫ�õ�����Դ��
			   ���������ǽ�CTCPServer_CE��ʵ��ָ�봫����
�����ڲ�����:  (��)
������  ֵ��:  ����ֵû���ر�����壬�ڴ����ǽ�����ֵ��Ϊ0��
------------------------------------------------------------------*/
DWORD CTCPServer_CE::SocketThreadFunc(PVOID lparam)
{
	CTCPServer_CE *pSocket;
	//�õ�CTCPServer_CEʵ��ָ��
	pSocket = (CTCPServer_CE*)lparam;
	//������¼�����
	fd_set fdRead;
	int ret;
	TIMEVAL	aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 1;
	while (TRUE)
	{
        //�յ��˳��¼��������߳�
		if (WaitForSingleObject(pSocket->m_exitThreadEvent,0) == WAIT_OBJECT_0)
		{
			break;
		}
		
		FD_ZERO(&fdRead);
		FD_SET(pSocket->m_ServerSocket,&fdRead);
		
		//�����¼�
		ret = select(0,&fdRead,NULL,NULL,&aTime);
		
		if (ret == SOCKET_ERROR)
		{
			//���������¼�
			int iErrorCode = WSAGetLastError();
			//����������socket�Ĵ����¼�
			if (pSocket->OnServerError)
			{
				pSocket->OnServerError(pSocket->m_pOwner,pSocket,iErrorCode);
			}
			//�رշ������׽��� 
			closesocket(pSocket->m_ServerSocket);
			break;
		}
		
		if (ret > 0)
		{
			//�ж��Ƿ���¼�
			if (FD_ISSET(pSocket->m_ServerSocket,&fdRead))
			{
				//���������Listen�����ʾ������OnAccept�¼�
				
				SOCKADDR_IN clientAddr;
				CTCPCustom_CE * pClientSocket = new CTCPCustom_CE();
				int namelen = sizeof(clientAddr);
				//�ȴ���������ͻ������ӵ��׽���
				pClientSocket->m_socket = accept(pSocket->m_ServerSocket, (struct sockaddr *)&clientAddr, &namelen);
				//���յ��ͻ�������
				if (pClientSocket->m_socket)
				{
					pClientSocket->m_RemoteHost = inet_ntoa(clientAddr.sin_addr);
					pClientSocket->m_RemotePort = ntohs(clientAddr.sin_port);
					
					//������ͻ��˽��������¼�
					if (pSocket->OnClientConnect)
					{
						pSocket->OnClientConnect(pSocket->m_pOwner,pClientSocket);
					}
					//��pClientSocket�����߳�
					pClientSocket->Open(pSocket);
					//��ӵ��ͻ������Ӷ�����
					m_ListClientSocket.AddTail(pClientSocket);
				}
				else
				{
					//ʧ�ܣ��ͷ��ڴ�
					delete pClientSocket;
					pClientSocket = NULL;
				}	
			}
		}
	}
	//
	TRACE(L"���������߳��˳�\n");
	return 0;
}

//ɾ���ͻ���
void CTCPServer_CE::RemoteClient(CTCPCustom_CE *pClient /*�ͻ��˶���*/)
{
	POSITION posPrior;
	POSITION pos = m_ListClientSocket.GetHeadPosition();

	while (pos != NULL)
	{
		posPrior = pos;
        CTCPCustom_CE *pTcpCustom = (CTCPCustom_CE*)m_ListClientSocket.GetNext(pos);
		if (pTcpCustom == pClient)
		{
			//�ͷ��ڴ�
			delete pTcpCustom;
			pTcpCustom = NULL;
			m_ListClientSocket.RemoveAt(posPrior);
			TRACE(L"�Ƴ���һ���ͻ��˶���\n");
			break;
		}
	}
}

/*------------------------------------------------------------------
���������ܡ�:  ��TCP����
����ڲ�����:  (��)
�����ڲ�����:  (��)
������  ֵ��:  <=0:��TCP����ʧ��; =1:��TCP����ɹ�
------------------------------------------------------------------*/
int CTCPServer_CE::Open()
{
	WSADATA wsa;
	
	//1.��ʼ��socket��Դ
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		return -1;//����ʧ��
	}
	
	//2.���������׽���
	if ((m_ServerSocket=socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		return -2;
	}
	
    SOCKADDR_IN  serverAddr;
	ZeroMemory((char *)&serverAddr,sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(m_LocalPort);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//3.�󶨼����׽���
	if (bind(m_ServerSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr))<0)
	{
		return -3 ;
	}
	//4.�����׽��ֿ�ʼ����
	if (listen(m_ServerSocket,8)!=0)
	{
		return -3;
	}
	
	//4.���ü����׽���ͨѶģʽΪ�첽ģʽ
	DWORD ul= 1;
	ioctlsocket(m_ServerSocket,FIONBIO,&ul);

	ResetEvent(m_exitThreadEvent);
	//5.����ͨѶ�̣߳����߳���ȴ��ͻ��˽���
	m_serverThreadHandle = CreateThread(NULL,0,SocketThreadFunc,this,0,NULL);
	if (m_serverThreadHandle == NULL)
	{
		closesocket(m_ServerSocket);
		return -1;
	}

	return 1;
}

/*-----------------------------------------------------------------
���������ܡ�:  �ر�TCP����
����ڲ�����:  (��)
�����ڲ�����:  (��)
������  ֵ��:  <=0:�ر�TCP����ʧ��; =1:�ر�TCP����ɹ�
------------------------------------------------------------------*/
int CTCPServer_CE::Close()
{
	//����ͨѶ�߳�
	SetEvent(m_exitThreadEvent);
	//�ȴ�1�룬������߳�û���˳�����ǿ���˳�
	if (WaitForSingleObject(m_serverThreadHandle,1000) == WAIT_TIMEOUT)
	{
		TerminateThread(m_serverThreadHandle,0);
		TRACE(L"ǿ����ֹ���������߳�\n");
	}
	m_serverThreadHandle = NULL;
	//�ر�Socket���ͷ���Դ
	int err = closesocket(m_ServerSocket);
	if (err == SOCKET_ERROR)
	{
		return -1;
	}

	//���ȣ��ر������пͻ�������
	POSITION pos = m_ListClientSocket.GetHeadPosition();
	while (pos != NULL)
	{
		//�õ��ͻ��˶���
        CTCPCustom_CE *pTcpCustom = (CTCPCustom_CE*)m_ListClientSocket.GetNext(pos);
		if (!pTcpCustom->Close())
		{
			TRACE(L"�رտͻ���socket����");
		}

		//�ͷ��ڴ�
		delete pTcpCustom;
		pTcpCustom = NULL;
	}

	m_ListClientSocket.RemoveAll();
	
	WSACleanup();
	return 1;
}   

/*-----------------------------------------------------------------
���������ܡ�:  ��������
����ڲ�����:  pCustomCE ���ͻ��˶���ָ��
			   buf : ������
			   dwBufLen : ����������
�����ڲ�����:  (��)
������  ֵ��:  TRUE : ���ͳɹ� �� FALSE : ����ʧ��
------------------------------------------------------------------*/
BOOL CTCPServer_CE::SendData(CTCPCustom_CE* pCustomCE, const char * buf , DWORD dwBufLen)
{
	BOOL bResult = FALSE;
	BOOL bExisted = FALSE;
	if (pCustomCE == NULL)
	{
		return FALSE;
	}

	//�жϴ˿ͻ����Ƿ����
	POSITION pos = m_ListClientSocket.GetHeadPosition();
	while (pos != NULL)
	{
        CTCPCustom_CE *pTcpCustom = (CTCPCustom_CE*)m_ListClientSocket.GetNext(pos);

		if (pCustomCE == pTcpCustom)
		{
			bExisted = TRUE;
			break;
		}
	}
	if (!bExisted)
	{
		return FALSE;
	}

	bResult =  pCustomCE->SendData(buf,dwBufLen);

	if (!bResult)
	{
		//
		RemoteClient(pCustomCE);
	}

	return bResult;
}

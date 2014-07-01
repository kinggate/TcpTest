// TCPServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TCPServer.h"
#include "TCPServerDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//����TCP �յ��ͻ���������Ϣ
#define WM_RECV_TCP_DATA WM_USER + 101
//����TCP�ͻ���������Ϣ
#define WM_TCP_CLIENT_CONNECT WM_USER + 102

// CTCPServerDlg �Ի���

CTCPServerDlg::CTCPServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTCPServerDlg::IDD, pParent)
	, m_localPort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTCPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_LOCALPORT, m_localPort);
}

BEGIN_MESSAGE_MAP(CTCPServerDlg, CDialog)
	ON_MESSAGE(WM_RECV_TCP_DATA,OnRecvTCPData)
	ON_MESSAGE(WM_TCP_CLIENT_CONNECT,OnClientConnect)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_LISTEN, &CTCPServerDlg::OnBnClickedBtnListen)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CTCPServerDlg::OnBnClickedBtnClose)
END_MESSAGE_MAP()


// CTCPServerDlg ��Ϣ�������

BOOL CTCPServerDlg::OnInitDialog()
{
	m_bFullScreen = FALSE; //��ֹȫ��
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//����Ĭ��ֵ
	m_localPort = 5000;
	UpdateData(FALSE);

	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


//�ͻ������ӽ����¼�������
void CALLBACK  CTCPServerDlg::OnClientConnect(void* pOwner,CTCPCustom_CE* pTcpCustom)
{
	TCHAR *szAddress =NULL;
	DWORD dwBufLen = pTcpCustom->m_RemoteHost.GetLength() + 1;
	szAddress = new TCHAR[dwBufLen];
	ZeroMemory(szAddress,dwBufLen*2);
	//�����ڴ棬�õ��ͻ���IP��ַ
	wcscpy(szAddress,pTcpCustom->m_RemoteHost);
	
	CTCPServerDlg *pThis = (CTCPServerDlg*)pOwner;
	
	//�����첽��Ϣ����ʾ�пͻ������ӣ���Ϣ���������Ҫ�ͷ��ڴ�
	pThis->PostMessage(WM_TCP_CLIENT_CONNECT,0,LPARAM(szAddress));
}

//�ͻ���SOCKET�ر��¼�������
void  CALLBACK CTCPServerDlg::OnClientClose(void* pOwner,CTCPCustom_CE* pTcpCustom)
{
	TCHAR *szAddress =NULL;
	DWORD dwBufLen = pTcpCustom->m_RemoteHost.GetLength() + 1;
	szAddress = new TCHAR[dwBufLen];
	ZeroMemory(szAddress,dwBufLen*2);
	//�����ڴ棬�õ��ͻ���IP��ַ
	wcscpy(szAddress,pTcpCustom->m_RemoteHost);

	CTCPServerDlg *pThis = (CTCPServerDlg*)pOwner;
	
	//�����첽��Ϣ����ʾ�пͻ������ӣ���Ϣ���������Ҫ�ͷ��ڴ�
	pThis->PostMessage(WM_TCP_CLIENT_CONNECT,1,LPARAM(szAddress));

}

//���������յ����Կͻ��˵�����
void CALLBACK CTCPServerDlg::OnClientRead(void* pOwner,CTCPCustom_CE* pTcpCustom,const char * buf,DWORD dwBufLen )
{
	DATA_BUF *pGenBuf = new DATA_BUF;
	char *pRecvBuf = NULL; //���ջ�����
	//�õ�������ָ��
	CTCPServerDlg* pThis = (CTCPServerDlg*)pOwner;
	//�����յĻ�����������pRecvBuf��
	pRecvBuf = new char[dwBufLen];
	CopyMemory(pRecvBuf,buf,dwBufLen);

	ZeroMemory(pGenBuf,sizeof(DATA_BUF));
	pGenBuf->dwBufLen = dwBufLen;
	pGenBuf->sBuf = pRecvBuf;
	
	//
	wcscpy(pGenBuf->szAddress,pTcpCustom->m_RemoteHost);

	//�����첽��Ϣ����ʾ�յ�TCP���ݣ���Ϣ�����꣬Ӧ�ͷ��ڴ�
	pThis->PostMessage(WM_RECV_TCP_DATA,WPARAM(pGenBuf),LPARAM(pTcpCustom));

   
}

//�ͻ���Socket�����¼�������
void CALLBACK CTCPServerDlg::OnClientError(void* pOwner,CTCPCustom_CE* pTcpCustom,int nErrorCode)
{
	
}

//��������Socket�����¼�������
void CALLBACK CTCPServerDlg::OnServerError(void* pOwner,CTCPServer_CE* pTcpServer_CE,int nErrorCode)
{
	
}

//TCP�������ݴ�����
LONG CTCPServerDlg::OnRecvTCPData(WPARAM wParam,LPARAM lParam)
{
	DATA_BUF *pGenBuf = (DATA_BUF*)wParam; //ͨ�û�����
	CTCPCustom_CE* pTcpCustom= (CTCPCustom_CE* )lParam; //TCP�ͻ��˶���
	//������ʾ�б�
	CListBox * pLstRecv = (CListBox*)GetDlgItem(IDC_LST_RECV);
	ASSERT(pLstRecv != NULL);
	//���յ�������
	CString strRecv;
	CString strLen;
	strLen.Format(L"%d",pGenBuf->dwBufLen);
	strRecv = CString(pGenBuf->sBuf);
	
	pLstRecv->AddString(_T("************************************"));
	pLstRecv->AddString(_T("����: ") + CString(pGenBuf->szAddress) );
	pLstRecv->AddString(_T("���ݳ���:")+strLen);
	pLstRecv->AddString(strRecv);

	//���ͻ�Ӧ����
	if (!m_tcpServer.SendData(pTcpCustom,"recv ok",strlen("recv ok")))
	{
		AfxMessageBox(_T("����ʧ��"));
	}

	//�ͷ��ڴ�
	delete[] pGenBuf->sBuf;
	pGenBuf->sBuf = NULL;
	delete pGenBuf;
	pGenBuf = NULL;
	return 0;
}

//�ͻ������ӶϿ���Ϣ����
LONG CTCPServerDlg::OnClientConnect(WPARAM wParam,LPARAM lParam)
{
	int iIndex;
	TCHAR *szAddress = (TCHAR*)lParam;
	CString strAddrss = szAddress;
	
	CListBox * pLstConn = (CListBox*)GetDlgItem(IDC_LST_CONN);
	ASSERT(pLstConn != NULL);

	if (wParam == 0)
	{
		pLstConn->AddString(strAddrss + _T("��������"));
	}
	else
	{
		iIndex = pLstConn->FindString(iIndex,strAddrss + _T("��������"));
		if (iIndex != LB_ERR)
		{
			pLstConn->DeleteString(iIndex); 
		}
	}

	//�ͷ��ڴ�
	delete[] szAddress;
	szAddress = NULL;
	return 0;
}


//����
void CTCPServerDlg::OnBnClickedBtnListen()
{
	UpdateData(TRUE);
	//����m_tcpServer����
   	m_tcpServer.m_LocalPort = m_localPort;
	m_tcpServer.m_pOwner = this;
	m_tcpServer.OnClientConnect = OnClientConnect;
	m_tcpServer.OnClientClose = OnClientClose;
	m_tcpServer.OnClientRead = OnClientRead;
	m_tcpServer.OnClientError = OnClientError;
	m_tcpServer.OnServerError = OnServerError;
	if (m_tcpServer.Open() <= 0)
	{
		AfxMessageBox(_T("����ʧ��"));
		return;
	}
}


//�ر�
void CTCPServerDlg::OnBnClickedBtnClose()
{
	CListBox * pLstConn = (CListBox*)GetDlgItem(IDC_LST_CONN);
	ASSERT(pLstConn != NULL);
	
	CListBox * pLstRecv = (CListBox*)GetDlgItem(IDC_LST_RECV);
	ASSERT(pLstRecv != NULL);
	
	//
	if (m_tcpServer.Close() <=0)
	{
		AfxMessageBox(_T("�ر�TCP������ʧ��"));
		return;
	}
	
	//����б�
	pLstConn->ResetContent();
	pLstRecv->ResetContent();
}

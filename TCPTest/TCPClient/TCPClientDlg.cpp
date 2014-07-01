// TCPClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TCPClient.h"
#include "TCPClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//����TCP �ͻ��˽��յ�������Ϣ
#define WM_RECV_TCP_DATA WM_USER + 101
//����TCP�ͻ������ӶϿ���Ϣ
#define WM_TCP_CLIENT_DISCONNECT WM_USER + 102

// CTCPClientDlg �Ի���

CTCPClientDlg::CTCPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTCPClientDlg::IDD, pParent)
	, m_remoteHost(_T(""))
	, m_remotePort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTCPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_REMOTEHOST, m_remoteHost);
	DDX_Text(pDX, IDC_EDT_REMOTEPORT, m_remotePort);


}

BEGIN_MESSAGE_MAP(CTCPClientDlg, CDialog)

	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RECV_TCP_DATA,OnRecvTCPData)
	ON_MESSAGE(WM_TCP_CLIENT_DISCONNECT,OnClientDisconnect)
	ON_BN_CLICKED(IDC_BTN_CONN, &CTCPClientDlg::OnBnClickedBtnConn)
	ON_BN_CLICKED(IDC_BTN_DISCONN, &CTCPClientDlg::OnBnClickedBtnDisconn)
	ON_BN_CLICKED(IDC_BTN_SENDDATA, &CTCPClientDlg::OnBnClickedBtnSenddata)
	ON_BN_CLICKED(IDC_BUTTON_SEND_FILE, &CTCPClientDlg::OnBnClickedButtonSendFile)
END_MESSAGE_MAP()


// CTCPClientDlg ��Ϣ�������

BOOL CTCPClientDlg::OnInitDialog()
{
	m_bFullScreen = FALSE; //��ֹȫ��
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//��ʼ������ֵ
	m_remoteHost = GetLocalIP();
	m_remotePort = 5000;
	UpdateData(FALSE);

	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

//���ӶϿ��¼�
void CALLBACK CTCPClientDlg::OnDisConnect(void* pOwner)
{
	//�õ�������ָ��
	CTCPClientDlg* pThis = (CTCPClientDlg*)pOwner;
	//������Ϣ��ʾ�ͻ������ӶϿ�
	pThis->PostMessage(WM_TCP_CLIENT_DISCONNECT,0,0);
}

//���ݽ����¼�
void CALLBACK CTCPClientDlg::OnRead(void* pOwner,const char * buf,DWORD dwBufLen )
{
	BYTE *pRecvBuf = NULL; //���ջ�����
	//�õ�������ָ��
	CTCPClientDlg* pThis = (CTCPClientDlg*)pOwner;
	//�����յĻ�����������pRecvBuf��
	pRecvBuf = new BYTE[dwBufLen];
	CopyMemory(pRecvBuf,buf,dwBufLen);

	//�����첽��Ϣ����ʾ�յ�TCP���ݣ���Ϣ�����꣬Ӧ�ͷ��ڴ�
	pThis->PostMessage(WM_RECV_TCP_DATA,WPARAM(pRecvBuf),dwBufLen);

}

//Socket�����¼�
void CALLBACK CTCPClientDlg::OnError(void* pOwner,int nErrorCode)
{
	TRACE(L"�ͻ���socket��������");
}

//TCP�������ݴ�����
LONG CTCPClientDlg::OnRecvTCPData(WPARAM wParam,LPARAM lParam)
{
	CString strOldRecv = L"";
	CString strRecv = L"";
	//���յ���BUF
	CHAR *pBuf = (CHAR*)wParam;
	//���յ���BUF����
	DWORD dwBufLen = lParam;
	//���տ�
	CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_EDT_RECV);
	ASSERT(pEdtRecvMsg != NULL);

	//�õ����տ��е���ʷ�ı�
	pEdtRecvMsg->GetWindowTextW(strOldRecv);
	//
	strRecv = CString(pBuf);
	//���½��յ����ı���ӵ����տ���
	strOldRecv = strOldRecv + strRecv + L"\r\n";
	pEdtRecvMsg->SetWindowTextW(strOldRecv);

	//�ͷ��ڴ�
	delete[] pBuf;
	pBuf = NULL;
	return 0;
}

//�ͻ������ӶϿ���Ϣ����
LONG CTCPClientDlg::OnClientDisconnect(WPARAM wParam,LPARAM lParam)
{
	//�õ�״̬����ǩ
	CStatic * pStatus = (CStatic *)GetDlgItem(IDC_LBL_CONNSTATUS);
	ASSERT(pStatus != NULL);

	pStatus->SetWindowText(_T("���ӶϿ�"));
	return 0;
}

//��������
void CTCPClientDlg::OnBnClickedBtnConn()
{
	UpdateData(TRUE);
	CStatic *pStatus = (CStatic*)GetDlgItem(IDC_LBL_CONNSTATUS);
	ASSERT(pStatus != NULL);
	//����m_tcpClient����
	m_tcpClient.m_remoteHost = m_remoteHost;
	m_tcpClient.m_port = m_remotePort;
	m_tcpClient.OnDisConnect = OnDisConnect;
	m_tcpClient.OnRead = OnRead;
	m_tcpClient.OnError = OnError;
	//�򿪿ͻ���socket
	m_tcpClient.Open(this);

	//�����������������
	if (m_tcpClient.Connect())
	{

		pStatus->SetWindowText(L"��������");
		UpdateData(FALSE);
	}
	else
	{
		AfxMessageBox(_T("��������ʧ��"));
		pStatus->SetWindowText(L"���ӶϿ�");
		return;
	}
}

//�Ͽ�����
void CTCPClientDlg::OnBnClickedBtnDisconn()
{
	CStatic *pStatus = (CStatic*)GetDlgItem(IDC_LBL_CONNSTATUS);
	ASSERT(pStatus != NULL);
	//�رտͻ����׽���
	if (m_tcpClient.Close())
	{
		pStatus->SetWindowText(L"���ӶϿ�");
	}
	else
	{
		AfxMessageBox(_T("���ӶϿ�ʧ��"));
	}	
}


//��������
void CTCPClientDlg::OnBnClickedBtnSenddata()
{
	CString strSendData;
	char * sendBuf;
	int sendLen=0;
	CEdit *pEdtSend = (CEdit*)GetDlgItem(IDC_EDT_SEND);
	pEdtSend->GetWindowTextW(strSendData);

	//���÷��ͻ�����
	sendLen = strSendData.GetLength()*2 + 2;
	sendBuf = new char[sendLen];
	ZeroMemory(sendBuf,sendLen);
	wcstombs(sendBuf,strSendData,sendLen);
	sendLen = strlen(sendBuf)+1;
	//��������
	if (!m_tcpClient.SendData(sendBuf,sendLen))
	{
		AfxMessageBox(_T("����ʧ��"));
	}
	//�ͷ��ڴ�
	delete[] sendBuf;
	sendBuf = NULL;		
}

//�õ����ص�IP��ַ
CString CTCPClientDlg::GetLocalIP()
{
	HOSTENT *LocalAddress;
	char	*Buff;
	TCHAR	*wBuff;
	CString strReturn = _T("");

	//�����µĻ�����
	Buff = new char[256];
	wBuff = new TCHAR[256];
	//�ÿջ�����
	memset(Buff, '\0', 256);
	memset(wBuff, TEXT('\0'), 256*sizeof(TCHAR));
	//�õ����ؼ������
	if (gethostname(Buff, 256) == 0)
	{
		//ת����˫�ֽ��ַ���
		mbstowcs(wBuff, Buff, 256);
		//�õ����ص�ַ
		LocalAddress = gethostbyname(Buff);
		//�ÿ�buff
		memset(Buff, '\0', 256);
		//��ϱ���IP��ַ
		sprintf(Buff, "%d.%d.%d.%d\0", LocalAddress->h_addr_list[0][0] & 0xFF,
			LocalAddress->h_addr_list[0][1] & 0x00FF, LocalAddress->h_addr_list[0][2] & 0x0000FF, LocalAddress->h_addr_list[0][3] & 0x000000FF);
		//�ÿ�wBuff
		memset(wBuff, TEXT('\0'), 256*sizeof(TCHAR));
		//ת����˫�ֽ��ַ���
		mbstowcs(wBuff, Buff, 256);
		//���÷���ֵ
		strReturn = wBuff;
	}
	else
	{
	}

	//�ͷ�Buff������
	delete[] Buff;
	Buff = NULL;
	//�ͷ�wBuff������
	delete[] wBuff;
	wBuff = NULL;
	return strReturn;
}

void CTCPClientDlg::OnBnClickedButtonSendFile() //send file
{
	CString filePath;

	CFileDialog dlg(TRUE,NULL,NULL,	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)TEXT("bin files (*.bin)|*.txt|All Files (*.*)|*.*||"),NULL);

	if(dlg.DoModal()==IDOK)
	{
		filePath = dlg.GetPathName();
		printf("file path=%S\n", filePath.GetBuffer());
	}
	else
	{
		return;
	}
}

// TCPServerDlg.h : ͷ�ļ�
//

#pragma once

#include "TCPServer_CE.h"
#include "TCPCustom_CE.h"

//����ͨ�û�����
typedef struct  _DATA_BUF
{
	DWORD dwBufLen;
	char* sBuf;	
	TCHAR szAddress[MAX_PATH];
}DATA_BUF,*PDATA_BUF;

// CTCPServerDlg �Ի���
class CTCPServerDlg : public CDialog
{
// ����
public:
	CTCPServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TCPSERVER_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	//TCP�������ݴ�����
	afx_msg LONG OnRecvTCPData(WPARAM wParam,LPARAM lParam);
	//�ͻ������ӶϿ���Ϣ����
	afx_msg LONG OnClientConnect(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	//����CTCPServer_CE����
	CTCPServer_CE m_tcpServer;

public:
	DWORD m_localPort;

private:
	//�ͻ������ӽ����¼�������
	static void CALLBACK	OnClientConnect(void* pOwner,CTCPCustom_CE *pTcpCustom);
	//�ͻ���SOCKET�ر��¼�������
	static void  CALLBACK OnClientClose(void* pOwner,CTCPCustom_CE*pTcpCustom);
	//���������յ����Կͻ��˵�����
	static  void CALLBACK OnClientRead(void* pOwner,CTCPCustom_CE* pTcpCustom,const char * buf,DWORD dwBufLen );
	//�ͻ���Socket�����¼�������
	static  void CALLBACK OnClientError(void* pOwner,CTCPCustom_CE* pTcpCustom,int nErrorCode);
	//��������Socket�����¼�������
	static void CALLBACK OnServerError(void* pOwner,CTCPServer_CE* pTcpServer_CE,int nErrorCode);

public:
	afx_msg void OnBnClickedBtnListen();
	afx_msg void OnBnClickedBtnClose();
};

// TCPClientDlg.h : ͷ�ļ�
//

#pragma once

#include "TCPClient_CE.h"

// CTCPClientDlg �Ի���
class CTCPClientDlg : public CDialog
{
// ����
public:
	CTCPClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TCPCLIENT_DIALOG };


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
	afx_msg LONG OnClientDisconnect(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	//����CTCPClient_CE����
	CTCPClient_CE m_tcpClient;
public:
	CString m_remoteHost;
	DWORD m_remotePort;
private:
	//���ӶϿ��¼�������
	static void CALLBACK OnDisConnect(void* pOwner);
	//�������ݽ����¼�������
	static void CALLBACK OnRead(void* pOwner,const char * buf,DWORD dwBufLen );
	//Socket�����¼�������
	static void CALLBACK OnError(void* pOwner,int nErrorCode);
public:
	afx_msg void OnBnClickedBtnConn();
	afx_msg void OnBnClickedBtnDisconn();
	afx_msg void OnBnClickedBtnSenddata();
private:
	//�õ����ص�IP��ַ
	CString GetLocalIP();
public:
	afx_msg void OnBnClickedButtonSendFile();
};

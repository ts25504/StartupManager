// StartupManagerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

// CStartupManagerDlg �Ի���
class CStartupManagerDlg : public CDialog
{
// ����
public:
	CStartupManagerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_STARTUPMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CListCtrl m_startup_list;
    afx_msg void OnBnClickedCancelButton();
    afx_msg void OnBnClickedApplyButton();
    afx_msg void OnBnClickedOkButton();
    void OnCancel();
};

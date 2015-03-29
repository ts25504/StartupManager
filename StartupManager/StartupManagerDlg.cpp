// StartupManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include <locale>
#include "StartupManager.h"
#include "StartupManagerDlg.h"
#include "RegistryRun.h"
#include "FileInfo.h"
#include "ListItems.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::vector<ValueInfo> vi_vec;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// �Ի�������
    enum { IDD = IDD_ABOUTBOX };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CStartupManagerDlg �Ի���




CStartupManagerDlg::CStartupManagerDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CStartupManagerDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStartupManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STARTUP_LIST, m_startup_list);
}

BEGIN_MESSAGE_MAP(CStartupManagerDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_APPLY_BUTTON, &CStartupManagerDlg::OnBnClickedApplyButton)
END_MESSAGE_MAP()

void CStartupManagerDlg::DisplayIconView()
{
    CRect rect;
    m_startup_list.GetClientRect(&rect);
    DWORD dw_style = m_startup_list.GetExtendedStyle();
    dw_style |= LVS_EX_FULLROWSELECT;
    dw_style |= LVS_EX_GRIDLINES;
    dw_style |= LVS_EX_CHECKBOXES;
    m_startup_list.SetExtendedStyle(dw_style);

    SHFILEINFO sfi = {0};
    HIMAGELIST h_image_list = 
        (HIMAGELIST)::SHGetFileInfo(L"", 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
    m_startup_list.SetImageList(CImageList::FromHandle(h_image_list), LVSIL_NORMAL);
    FileInfo fi;
    for (ULONG i = 0; i < vi_vec.size(); ++i)
    {
        if (wcscmp(vi_vec[i].sz_product_name, L"unknown") == 0)
            m_startup_list.InsertItem(i, vi_vec[i].sz_value_name, fi.GetIconIndex(vi_vec[i].sz_value));
        else
            m_startup_list.InsertItem(i, vi_vec[i].sz_product_name, fi.GetIconIndex(vi_vec[i].sz_value));
        m_startup_list.SetItemText(i, 1, vi_vec[i].sz_value);

        if (vi_vec[i].state)
        {
            ListView_SetCheckState(m_startup_list, i, TRUE);
        }
    }
}

void CStartupManagerDlg::DisplayReportView()
{
    CRect rect;
    m_startup_list.GetClientRect(&rect);
    DWORD dw_style = m_startup_list.GetExtendedStyle();
    dw_style |= LVS_EX_FULLROWSELECT;
    dw_style |= LVS_EX_GRIDLINES;
    dw_style |= LVS_EX_CHECKBOXES;
    m_startup_list.SetExtendedStyle(dw_style);

    m_startup_list.InsertColumn(0, L"������", LVCFMT_LEFT, rect.Width()/7, 0);
    m_startup_list.InsertColumn(1, L"·��", LVCFMT_LEFT, 2*rect.Width()/7, 0);
    m_startup_list.InsertColumn(2, L"��", LVCFMT_LEFT, rect.Width()/7, 0);
    m_startup_list.InsertColumn(3, L"λ��", LVCFMT_LEFT, 2*rect.Width()/7, 0);
    m_startup_list.InsertColumn(4, L"�������", LVCFMT_LEFT, rect.Width()/7, 0);

    for (ULONG i = 0; i < vi_vec.size(); ++i)
    {
        if (wcscmp(vi_vec[i].sz_product_name, L"unknown") == 0)
            m_startup_list.InsertItem(i, vi_vec[i].sz_value_name);
        else
            m_startup_list.InsertItem(i, vi_vec[i].sz_product_name);
        m_startup_list.SetItemText(i, 1, vi_vec[i].sz_value);

        if (vi_vec[i].h_key == HKEY_CLASSES_ROOT)
            m_startup_list.SetItemText(i, 2, L"HKEY_CLASSES_ROOT");
        else if (vi_vec[i].h_key == HKEY_CURRENT_USER)
            m_startup_list.SetItemText(i, 2, L"HKEY_CURRENT_USER");
        else if (vi_vec[i].h_key == HKEY_LOCAL_MACHINE)
            m_startup_list.SetItemText(i, 2, L"HKEY_LOCAL_MACHINE");
        else if (vi_vec[i].h_key == HKEY_USERS)
            m_startup_list.SetItemText(i, 2, L"HKEY_USERS");
        else if (vi_vec[i].h_key == HKEY_CURRENT_CONFIG)
            m_startup_list.SetItemText(i, 2, L"HKEY_CURRENT_CONFIG");
        else
            m_startup_list.SetItemText(i, 2, L"");

        m_startup_list.SetItemText(i, 3, vi_vec[i].sz_subkey);

        if (vi_vec[i].state)
        {
            ListView_SetCheckState(m_startup_list, i, TRUE);
        }
        else
        {
            m_startup_list.SetItemText(i, 4, L"�ѽ���");
        }
    }
}

// CStartupManagerDlg ��Ϣ�������

BOOL CStartupManagerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // ��������...���˵�����ӵ�ϵͳ�˵��С�

    // IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    //  ִ�д˲���
    SetIcon(m_hIcon, TRUE); // ���ô�ͼ��
    SetIcon(m_hIcon, FALSE); // ����Сͼ��

    // TODO: �ڴ���Ӷ���ĳ�ʼ������

    ::setlocale(LC_CTYPE, "");

    ListItems::GetInstance()->AddItems(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", vi_vec);
    ListItems::GetInstance()->AddItems(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", vi_vec);
    ListItems::GetInstance()->AddItems(HKEY_LOCAL_MACHINE, L"Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run", vi_vec);
    ListItems::GetInstance()->AddItems(vi_vec);
    ListItems::GetInstance()->ReadDisabledItemsFromFile(vi_vec);

    //DisplayIconView();
    DisplayReportView();

    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CStartupManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CStartupManagerDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // ʹͼ���ڹ��������о���
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // ����ͼ��
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CStartupManagerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CStartupManagerDlg::OnBnClickedApplyButton()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    for (int i = 0; i < m_startup_list.GetItemCount(); ++i)
    {
        if (!ListView_GetCheckState(m_startup_list, i))
        {
            m_startup_list.SetItemText(i, 4, L"�ѽ���");
            ListItems::GetInstance()->DeleteItem(vi_vec[i]);
        }
        else
        {
            m_startup_list.SetItemText(i, 4, L"");
            if (vi_vec[i].state == 0)
            {
                ListItems::GetInstance()->ResetItem(vi_vec[i]);
            }
        }
    }
}

void CStartupManagerDlg::OnCancel()
{
    ListItems::GetInstance()->WriteDisabledItemsToFile(vi_vec);
    CDialog::OnCancel();
}
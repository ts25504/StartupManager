// StartupManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <tchar.h>
#include "StartupManager.h"
#include "StartupManagerDlg.h"
#include "MyRegistry.h"
#include "FileInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::vector<ValueInfo> vi_vec;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// 对话框数据
    enum { IDD = IDD_ABOUTBOX };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CStartupManagerDlg 对话框




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
    ON_BN_CLICKED(IDC_CANCEL_BUTTON, &CStartupManagerDlg::OnBnClickedCancelButton)
    ON_BN_CLICKED(IDC_APPLY_BUTTON, &CStartupManagerDlg::OnBnClickedApplyButton)
    ON_BN_CLICKED(IDC_OK_BUTTON, &CStartupManagerDlg::OnBnClickedOkButton)
END_MESSAGE_MAP()

void ReadItems(std::vector<ValueInfo>& vi_vec)
{
    FILE* fp = NULL;
    fopen_s(&fp, "StartupItems.txt", "r+");
    if (feof(fp))
        return;
    TCHAR sz_key[MAX_PATH] = {0};
    size_t n = 0;
    _ftscanf_s(fp, TEXT("%d\n"), &n);
    for (ULONG i = 0; i < n; ++i)
    {
        ValueInfo vi = {0};
        _fgetts(vi.sz_value_name, MAX_VALUE_NAME, fp);
        vi.sz_value_name[_tcslen(vi.sz_value_name)-1] = TEXT('\0');
        _fgetts(vi.sz_value, MAX_VALUE, fp);
        vi.sz_value[_tcslen(vi.sz_value)-1] = TEXT('\0');
        _fgetts(sz_key, MAX_PATH, fp);
        sz_key[_tcslen(sz_key)-1] = TEXT('\0');
        _fgetts(vi.sz_subkey, MAX_PATH, fp);
        vi.sz_subkey[_tcslen(vi.sz_subkey)-1] = TEXT('\0');
        _ftscanf_s(fp, TEXT("%d\n"), &vi.state);
        if(_tcscmp(sz_key, TEXT("HKEY_CLASSES_ROOT")) == 0)
            vi.hkey = HKEY_CLASSES_ROOT;
        else if (_tcscmp(sz_key, TEXT("HKEY_CURRENT_USER")) == 0)
            vi.hkey = HKEY_CURRENT_USER;
        else if (_tcscmp(sz_key, TEXT("HKEY_LOCAL_MACHINE")) == 0)
            vi.hkey = HKEY_LOCAL_MACHINE;
        else if (_tcscmp(sz_key, TEXT("HKEY_USERS\n")) == 0)
            vi.hkey = HKEY_USERS;
        else if (_tcscmp(sz_key, TEXT("HKEY_CURRENT_CONFIG")) == 0)
            vi.hkey = HKEY_CURRENT_CONFIG;
        vi_vec.push_back(vi);
    }

    fclose(fp);
}

void WriteItems(std::vector<ValueInfo>& vi_vec)
{
    FILE* fp = NULL;
    fopen_s(&fp, "StartupItems.txt", "w+");
    TCHAR sz_key[MAX_PATH] = {0};
    size_t n = vi_vec.size();
    _ftprintf(fp, TEXT("%d\n"), n);
    for (ULONG i = 0; i < n; ++i)
    {
        if(vi_vec[i].hkey == HKEY_CLASSES_ROOT)
            _tcscpy_s(sz_key, MAX_PATH, TEXT("HKEY_CLASSES_ROOT"));
        else if (vi_vec[i].hkey == HKEY_CURRENT_USER)
            _tcscpy_s(sz_key, MAX_PATH, TEXT("HKEY_CURRENT_USER"));
        else if (vi_vec[i].hkey == HKEY_LOCAL_MACHINE)
            _tcscpy_s(sz_key, MAX_PATH, TEXT("HKEY_LOCAL_MACHINE"));
        else if (vi_vec[i].hkey == HKEY_USERS)
            _tcscpy_s(sz_key, MAX_PATH, TEXT("HKEY_USERS"));
        else if (vi_vec[i].hkey == HKEY_CURRENT_CONFIG)
            _tcscpy_s(sz_key, MAX_PATH, TEXT("HKEY_CURRENT_CONFIG"));

        _ftprintf(fp, TEXT("%s\n%s\n%s\n%s\n"), vi_vec[i].sz_value_name, vi_vec[i].sz_value,
            sz_key, vi_vec[i].sz_subkey);
        _ftprintf(fp, TEXT("%d\n"), vi_vec[i].state);
    }
    fclose(fp);
}

// CStartupManagerDlg 消息处理程序

BOOL CStartupManagerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
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

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码
    ReadItems(vi_vec);
    CRect rect;
    m_startup_list.GetClientRect(&rect);
    DWORD dw_style = m_startup_list.GetExtendedStyle();
    dw_style |= LVS_EX_FULLROWSELECT;
    dw_style |= LVS_EX_GRIDLINES;
    dw_style |= LVS_EX_CHECKBOXES;
    m_startup_list.SetExtendedStyle(dw_style);

    m_startup_list.InsertColumn(0, TEXT("启动项"), LVCFMT_LEFT, 1*rect.Width()/7, 0);
    m_startup_list.InsertColumn(1, TEXT("路径"), LVCFMT_LEFT, 2*rect.Width()/7, 1);
    m_startup_list.InsertColumn(2, TEXT("键"), LVCFMT_LEFT, 1*rect.Width()/7, 2);
    m_startup_list.InsertColumn(3, TEXT("位置"), LVCFMT_LEFT, 2*rect.Width()/7, 3);
    m_startup_list.InsertColumn(4, TEXT("状态"), LVCFMT_LEFT, rect.Width()/7, 4);

    FileInfo fi;
    //std::vector<ValueInfo> vi_vec;
    MyRegistry my_reg(HKEY_CURRENT_USER);
    my_reg.Open(TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), KEY_ALL_ACCESS);
    my_reg.Query(vi_vec);
    my_reg.Close();
    MyRegistry my_reg2(HKEY_LOCAL_MACHINE);
    my_reg2.Open(TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), KEY_ALL_ACCESS);
    my_reg2.Query(vi_vec);
    my_reg2.Close();
    MyRegistry my_reg3(HKEY_LOCAL_MACHINE);
    my_reg3.Open(TEXT("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"), KEY_ALL_ACCESS);
    my_reg3.Query(vi_vec);
    my_reg3.Close();

    for (ULONG i = 0; i < vi_vec.size(); ++i)
    {
        fi.Open(vi_vec[i].sz_value);
        m_startup_list.InsertItem(i, fi.QueryValue(TEXT("FileDescription")));
        m_startup_list.SetItemText(i, 1, vi_vec[i].sz_value);
        if(vi_vec[i].hkey == HKEY_CLASSES_ROOT)
            m_startup_list.SetItemText(i, 2, TEXT("HKEY_CLASSES_ROOT"));
        else if (vi_vec[i].hkey == HKEY_CURRENT_USER)
            m_startup_list.SetItemText(i, 2, TEXT("HKEY_CURRENT_USER"));
        else if (vi_vec[i].hkey == HKEY_LOCAL_MACHINE)
            m_startup_list.SetItemText(i, 2, TEXT("HKEY_LOCAL_MACHINE"));
        else if (vi_vec[i].hkey == HKEY_USERS)
            m_startup_list.SetItemText(i, 2, TEXT("HKEY_USERS"));
        else if (vi_vec[i].hkey == HKEY_CURRENT_CONFIG)
            m_startup_list.SetItemText(i, 2, TEXT("HKEY_CURRENT_CONFIG"));

        m_startup_list.SetItemText(i, 3, vi_vec[i].sz_subkey);
        fi.Close();
        if (vi_vec[i].state)
        {
            ListView_SetCheckState(m_startup_list, i, TRUE);
        }
        else
        {
            m_startup_list.SetItemText(i, 4, TEXT("已禁用"));
        }
    }

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CStartupManagerDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CStartupManagerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CStartupManagerDlg::OnBnClickedCancelButton()
{
    // TODO: 在此添加控件通知处理程序代码
    OnCancel();
}

void CStartupManagerDlg::OnBnClickedApplyButton()
{
    // TODO: 在此添加控件通知处理程序代码
    for (int i = 0; i < m_startup_list.GetItemCount(); ++i)
    {
        if (!ListView_GetCheckState(m_startup_list, i))
        {
            m_startup_list.SetItemText(i, 4, TEXT("已禁用"));
            vi_vec[i].state = 0;
            MyRegistry my_reg(vi_vec[i].hkey);
            my_reg.Open(vi_vec[i].sz_subkey, KEY_ALL_ACCESS);
            my_reg.DeleteValue(vi_vec[i].sz_value_name);
            my_reg.Close();
        }
        else
        {
            m_startup_list.SetItemText(i, 4, TEXT(""));
        }
    }
}

void CStartupManagerDlg::OnBnClickedOkButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CDialog::OnOK();
}

void CStartupManagerDlg::OnCancel()
{
    WriteItems(vi_vec);
    CDialog::OnCancel();
}
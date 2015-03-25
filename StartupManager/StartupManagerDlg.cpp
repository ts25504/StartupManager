// StartupManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "StartupManager.h"
#include "StartupManagerDlg.h"
#include "MyRegistry.h"
#include "FileInfo.h"

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

void ReadDisabledItemsFromFile(std::vector<ValueInfo>& vi_vec)
{
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DisabledItems.ini", "r+");
    if (err != 0)
        return;
    if (feof(fp))
        return;
    TCHAR sz_key[MAX_PATH] = {0};
    size_t num_of_disabled_items = 0;
    _ftscanf_s(fp, TEXT("%d\n"), &num_of_disabled_items);
    for (ULONG i = 0; i < num_of_disabled_items; ++i)
    {
        BOOL b_push = true;
        ValueInfo vi = {0};
        _fgetts(vi.sz_value_name, MAX_VALUE_NAME, fp); // fgetts() read \0.
        vi.sz_value_name[_tcslen(vi.sz_value_name)-1] = TEXT('\0');
        _fgetts(vi.sz_value, MAX_VALUE, fp);
        vi.sz_value[_tcslen(vi.sz_value)-1] = TEXT('\0');
        _fgetts(sz_key, MAX_KEY_LENGTH, fp);
        sz_key[_tcslen(sz_key)-1] = TEXT('\0');
        _fgetts(vi.sz_subkey, MAX_KEY_LENGTH, fp);
        vi.sz_subkey[_tcslen(vi.sz_subkey)-1] = TEXT('\0');
        _ftscanf_s(fp, TEXT("%d\n"), &vi.state);
        if(_tcscmp(sz_key, TEXT("HKEY_CLASSES_ROOT")) == 0)
            vi.h_key = HKEY_CLASSES_ROOT;
        else if (_tcscmp(sz_key, TEXT("HKEY_CURRENT_USER")) == 0)
            vi.h_key = HKEY_CURRENT_USER;
        else if (_tcscmp(sz_key, TEXT("HKEY_LOCAL_MACHINE")) == 0)
            vi.h_key = HKEY_LOCAL_MACHINE;
        else if (_tcscmp(sz_key, TEXT("HKEY_USERS\n")) == 0)
            vi.h_key = HKEY_USERS;
        else if (_tcscmp(sz_key, TEXT("HKEY_CURRENT_CONFIG")) == 0)
            vi.h_key = HKEY_CURRENT_CONFIG;
        for (ULONG i = 0; i < vi_vec.size(); ++i)
        {
            if (_tcscmp(vi_vec[i].sz_value_name, vi.sz_value_name) == 0 &&
                _tcscmp(vi_vec[i].sz_value, vi_vec[i].sz_value) == 0 &&
                _tcscmp(vi_vec[i].sz_subkey, vi.sz_subkey) == 0 &&
                vi_vec[i].h_key == vi.h_key)
            b_push = false;
        }
        if (b_push)
            vi_vec.push_back(vi);
    }

    fclose(fp);
}

void WriteDisabledItemsToFile(std::vector<ValueInfo>& vi_vec)
{
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DisabledItems.ini", "w+");
    if (err != 0)
        return;
    TCHAR sz_key[MAX_KEY_LENGTH] = {0};
    size_t num_of_disabled_items = 0;
    for (ULONG i = 0; i < vi_vec.size(); ++i)
    {
        if (vi_vec[i].state == 0)
            ++num_of_disabled_items;
    }
    _ftprintf(fp, TEXT("%d\n"), num_of_disabled_items);
    for (ULONG i = 0; i < vi_vec.size(); ++i)
    {
        if (vi_vec[i].state == 0)
        {
            if(vi_vec[i].h_key == HKEY_CLASSES_ROOT)
                _tcscpy_s(sz_key, MAX_KEY_LENGTH, TEXT("HKEY_CLASSES_ROOT"));
            else if (vi_vec[i].h_key == HKEY_CURRENT_USER)
                _tcscpy_s(sz_key, MAX_KEY_LENGTH, TEXT("HKEY_CURRENT_USER"));
            else if (vi_vec[i].h_key == HKEY_LOCAL_MACHINE)
                _tcscpy_s(sz_key, MAX_KEY_LENGTH, TEXT("HKEY_LOCAL_MACHINE"));
            else if (vi_vec[i].h_key == HKEY_USERS)
                _tcscpy_s(sz_key, MAX_KEY_LENGTH, TEXT("HKEY_USERS"));
            else if (vi_vec[i].h_key == HKEY_CURRENT_CONFIG)
                _tcscpy_s(sz_key, MAX_KEY_LENGTH, TEXT("HKEY_CURRENT_CONFIG"));

            _ftprintf(fp, TEXT("%s\n%s\n%s\n%s\n"), vi_vec[i].sz_value_name, vi_vec[i].sz_value,
                sz_key, vi_vec[i].sz_subkey);
            _ftprintf(fp, TEXT("%d\n"), vi_vec[i].state);
        }
    }
    fclose(fp);
}

void AddItems(HKEY h_key, const TCHAR* sz_subkey, std::vector<ValueInfo>& vi_vec)
{
    MyRegistry my_reg(h_key);
    my_reg.Open(sz_subkey, KEY_READ);
    my_reg.Query(vi_vec);
    my_reg.Close();
}

void DeleteItem(ValueInfo& vi)
{
    MyRegistry my_reg(vi.h_key);
    my_reg.Open(vi.sz_subkey, KEY_SET_VALUE);
    if (my_reg.DeleteValue(vi.sz_value_name))
        vi.state = 0;
    my_reg.Close();
}

void SetValueToReg(ValueInfo& vi)
{
    MyRegistry my_reg(vi.h_key);
    my_reg.Open(vi.sz_subkey, KEY_WRITE);
    if (my_reg.Write(vi.sz_value_name, vi.sz_value))
        vi.state = 1;
    my_reg.Close();
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
    CRect rect;
    m_startup_list.GetClientRect(&rect);
    DWORD dw_style = m_startup_list.GetExtendedStyle();
    dw_style |= LVS_EX_FULLROWSELECT;
    dw_style |= LVS_EX_GRIDLINES;
    dw_style |= LVS_EX_CHECKBOXES;
    m_startup_list.SetExtendedStyle(dw_style);

    SHFILEINFO sfi = {0};
    HIMAGELIST h_image_list = 
        (HIMAGELIST)::SHGetFileInfo(TEXT(""), 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
    m_startup_list.SetImageList(CImageList::FromHandle(h_image_list), LVSIL_NORMAL);
    FileInfo fi;
    AddItems(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), vi_vec);
    AddItems(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), vi_vec);
    AddItems(HKEY_LOCAL_MACHINE, TEXT("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"), vi_vec);
    ReadDisabledItemsFromFile(vi_vec);

    for (ULONG i = 0; i < vi_vec.size(); ++i)
    {
        fi.Open(vi_vec[i].sz_value);
        TCHAR sz_filename[MAX_PATH] = {0};
        _tcscpy_s(sz_filename, MAX_PATH, fi.GetProductName());
        fi.Close();
        if (_tcscmp(sz_filename, TEXT("unknown")) == 0)
            m_startup_list.InsertItem(i, vi_vec[i].sz_value_name, fi.GetIconIndex(vi_vec[i].sz_value));
        else
            m_startup_list.InsertItem(i, sz_filename, fi.GetIconIndex(vi_vec[i].sz_value));
        m_startup_list.SetItemText(i, 1, vi_vec[i].sz_value);

        if (vi_vec[i].state)
        {
            ListView_SetCheckState(m_startup_list, i, TRUE);
        }
    }

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
            DeleteItem(vi_vec[i]);
        }
        else
        {
            if (vi_vec[i].state == 0)
            {
                SetValueToReg(vi_vec[i]);
            }
        }
    }
}

void CStartupManagerDlg::OnCancel()
{
    WriteDisabledItemsToFile(vi_vec);
    CDialog::OnCancel();
}
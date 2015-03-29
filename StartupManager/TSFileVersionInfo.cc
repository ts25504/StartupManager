#include "stdafx.h"

#include <strsafe.h>

#include "TSFileVersionInfo.h"

TSFileVersionInfo::TSFileVersionInfo() : m_p_version_data(NULL)
{
}

TSFileVersionInfo::~TSFileVersionInfo()
{
    Close();
}

void TSFileVersionInfo::Close()
{
    if (m_p_version_data)
    {
        delete[] m_p_version_data;
        m_p_version_data = NULL;
    }
}

int TSFileVersionInfo::GetIconIndex(const wchar_t* p_file_path)
{
    SHFILEINFO s_info = {0};
    int i_ret = -1;
    if (p_file_path == NULL)
        goto exit;
    ::SHGetFileInfo(p_file_path, FILE_ATTRIBUTE_NORMAL, &s_info, sizeof(s_info),
        SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON | SHGFI_SYSICONINDEX);
    i_ret = s_info.iIcon;
exit:
    return i_ret;
}

bool TSFileVersionInfo::Open(const wchar_t* p_file_path)
{
    bool b_ret = false;
    DWORD dw_size = ::GetFileVersionInfoSize(p_file_path, NULL);
    if (dw_size == 0)
        goto exit;

    m_p_version_data = new byte[dw_size];
    if (m_p_version_data == NULL)
        goto exit;
    memset(m_p_version_data, 0, dw_size);

    long l_ret = ::GetFileVersionInfo(p_file_path, 0, dw_size, (void**)m_p_version_data);
    if (l_ret == 0)
        goto exit;

    b_ret = true;
exit:
    if (!b_ret)
        Close();
    return b_ret;
}

wchar_t* TSFileVersionInfo::QueryValue(const wchar_t* p_value_name)
{
    struct LANGANDCODEPAGE {
        WORD w_language;
        WORD w_codePage;
    } *p_translate;

    bool b_err = true;
    if (p_value_name == NULL)
        goto exit;
    if (m_p_version_data == NULL)
        goto exit;
    HRESULT hr = 0;
    UINT cb_translate = 0;
    wchar_t sz_sub_block[MAX_PATH] = {0};
    long l_ret = ::VerQueryValue(m_p_version_data, L"\\VarFileInfo\\Translation",
        (void**)&p_translate, &cb_translate);
    if (l_ret == 0)
        goto exit;

    for (ULONG i = 0; i < (cb_translate/sizeof(struct LANGANDCODEPAGE)); ++i)
    {
        hr = StringCchPrintf(sz_sub_block, MAX_PATH,
            L"\\StringFileInfo\\%04x%04x\\%s",
            p_translate[i].w_language,
            p_translate[i].w_codePage,
            p_value_name);
        if (FAILED(hr))
        {
            goto exit;
        }
    }

    void* p_data = NULL;
    UINT u_len = 0;
    l_ret = ::VerQueryValue(m_p_version_data, sz_sub_block, &p_data, &u_len);
    if (l_ret == 0)
        goto exit;
    b_err = false;
exit:
    if (b_err || p_data == NULL)
        return L"unknown";
    return (wchar_t*)p_data;
}

wchar_t* TSFileVersionInfo::GetFileDescription()
{
    return QueryValue(L"FileDescription");
}

wchar_t* TSFileVersionInfo::GetInternelName()
{
    return QueryValue(L"InternelName");
}

wchar_t* TSFileVersionInfo::GetLegalTradeMarks()
{
    return QueryValue(L"LegalTradeMarks");
}

wchar_t* TSFileVersionInfo::GetOriginalFileName()
{
    return QueryValue (L"OriginalFileName");
}

wchar_t* TSFileVersionInfo::GetProductName()
{
    return QueryValue(L"ProductName");
}

wchar_t* TSFileVersionInfo::GetProductVersion()
{
    return QueryValue(L"ProductVersion");
}
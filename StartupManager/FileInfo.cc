#include "stdafx.h"
#include <strsafe.h>
#include <Shlwapi.h>
#include "FileInfo.h"

FileInfo::FileInfo() : m_p_version_data(NULL)
{
}

FileInfo::~FileInfo()
{
    Close();
}

void FileInfo::Close()
{
    if (m_p_version_data)
    {
        delete[] m_p_version_data;
        m_p_version_data = NULL;
    }
}

TCHAR* FileInfo::ParsePath(TCHAR* p_file_path)
{
    TCHAR* p_parsed_path = new TCHAR[MAX_VALUE];
    if (p_parsed_path == NULL)
        return TEXT("error");
    memset(p_parsed_path, 0, MAX_VALUE);
    if (p_file_path[0] == TEXT('\"'))
        ++p_file_path;
    memcpy(p_parsed_path, p_file_path, _tcslen(p_file_path)*sizeof(TCHAR));
    for (ULONG i = 0; i < _tcslen(p_parsed_path); ++i)
    {
        if (p_parsed_path[i-3] == TEXT('.') &&
            (p_parsed_path[i-2] == TEXT('e') || p_parsed_path[i-2] == TEXT('E')) &&
            (p_parsed_path[i-1] == TEXT('x') || p_parsed_path[i-1] == TEXT('X')) &&
            (p_parsed_path[i] == TEXT('e') || p_parsed_path[i] == TEXT('E')))
        {
            _tcsncpy_s(p_parsed_path, _tcslen(p_parsed_path)*sizeof(TCHAR), p_file_path, i+1);
            break;
        }
    }
    return p_parsed_path;
}

int FileInfo::GetIconIndex(TCHAR* p_file_path)
{
    SHFILEINFO s_info = {0};
    int i_ret = -1;
    TCHAR* p_parsed_path = new TCHAR[MAX_VALUE];
    if (p_file_path == NULL || p_parsed_path == NULL)
        goto exit;
    memset(p_parsed_path, 0, MAX_VALUE);
    if (!::PathFileExists(p_file_path))
    {
        p_parsed_path = ParsePath(p_file_path);
        if (_tcscmp(p_parsed_path, TEXT("error")) == 0)
            goto exit;
    }
    else
        memcpy(p_parsed_path, p_file_path, _tcslen(p_file_path)*sizeof(TCHAR));
    ::SHGetFileInfo(p_parsed_path, FILE_ATTRIBUTE_NORMAL, &s_info, sizeof(s_info),
        SHGFI_USEFILEATTRIBUTES | SHGFI_LARGEICON | SHGFI_SYSICONINDEX);
    i_ret = s_info.iIcon;
exit:
    if (p_parsed_path)
    {
        delete[] p_parsed_path;
        p_parsed_path = NULL;
    }
    return i_ret;
}

bool FileInfo::Open(TCHAR* p_file_path)
{
    bool b_ret = false;
    TCHAR* p_parsed_path = new TCHAR[MAX_VALUE];
    if (p_file_path == NULL || p_parsed_path == NULL)
        goto exit;
    memset(p_parsed_path, 0, MAX_VALUE);
    if (!::PathFileExists(p_file_path))
    {
        p_parsed_path = ParsePath(p_file_path);
        if (_tcscmp(p_parsed_path, TEXT("error")) == 0)   // new error.
            goto exit;
    }
    else
        memcpy(p_parsed_path, p_file_path, _tcslen(p_file_path)*sizeof(TCHAR));

    DWORD dw_size = ::GetFileVersionInfoSize(p_parsed_path, NULL);
    if (dw_size == 0)
        goto exit;

    m_p_version_data = new byte[dw_size];
    if (m_p_version_data == NULL)
        goto exit;
    memset(m_p_version_data, 0, dw_size);

    long l_ret = ::GetFileVersionInfo(p_parsed_path, 0, dw_size, (void**)m_p_version_data);
    if (l_ret == 0)
        goto exit;

    b_ret = true;
exit:
    if (p_parsed_path)
    {
        delete[] p_parsed_path;
        p_parsed_path = NULL;
    }
    if (!b_ret)
        Close();
    return b_ret;
}

TCHAR* FileInfo::QueryValue(const TCHAR* p_value_name)
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
    TCHAR sz_sub_block[MAX_PATH] = {0};
    long l_ret = ::VerQueryValue(m_p_version_data, TEXT("\\VarFileInfo\\Translation"),
        (void**)&p_translate, &cb_translate);
    if (l_ret == 0)
        goto exit;

    for (ULONG i = 0; i < (cb_translate/sizeof(struct LANGANDCODEPAGE)); ++i)
    {
        hr = StringCchPrintf(sz_sub_block, MAX_PATH,
            TEXT("\\StringFileInfo\\%04x%04x\\%s"),
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
        return TEXT("unknown");
    return (TCHAR*)p_data;
}

TCHAR* FileInfo::GetFileDescription()
{
    return QueryValue(TEXT("FileDescription"));
}

TCHAR* FileInfo::GetInternelName()
{
    return QueryValue(TEXT("InternelName"));
}

TCHAR* FileInfo::GetLegalTradeMarks()
{
    return QueryValue(TEXT("LegalTradeMarks"));
}

TCHAR* FileInfo::GetOriginalFileName()
{
    return QueryValue(TEXT("OriginalFileName"));
}

TCHAR* FileInfo::GetProductName()
{
    return QueryValue(TEXT("ProductName"));
}

TCHAR* FileInfo::GetProductVersion()
{
    return QueryValue(TEXT("ProductVersion"));
}
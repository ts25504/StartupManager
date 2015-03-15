#include "stdafx.h"
#include <tchar.h>
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

bool FileInfo::Open(TCHAR* p_file_path)
{
    if (p_file_path == NULL)
        goto exit;
    if (!PathFileExists(p_file_path))
        PathRemoveArgs(p_file_path);
    if (!PathFileExists(p_file_path))
        PathUnquoteSpaces(p_file_path);
    bool b_ret = false;
    DWORD dw_size = ::GetFileVersionInfoSize(p_file_path, NULL);
    if (dw_size == 0)
        goto exit;

    m_p_version_data = new byte[dw_size];
    if (m_p_version_data == NULL)
        goto exit;
    memset(m_p_version_data, 0, dw_size);

    long l_ret = ::GetFileVersionInfo((TCHAR*)p_file_path, 0, dw_size, (void**)m_p_version_data);
    if (l_ret == 0)
        goto exit;

    b_ret = true;
exit:
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

    if (m_p_version_data == NULL)
        return TEXT("");
    HRESULT hr = 0;
    UINT cb_translate = 0;
    TCHAR sz_sub_block[MAX_PATH] = {0};
    long l_ret = ::VerQueryValue(m_p_version_data, TEXT("\\VarFileInfo\\Translation"),
        (void**)&p_translate, &cb_translate);
    if (l_ret == 0)
        return TEXT("");

    for (ULONG i = 0; i < (cb_translate/sizeof(struct LANGANDCODEPAGE)); ++i)
    {
        hr = StringCchPrintf(sz_sub_block, MAX_PATH,
            TEXT("\\StringFileInfo\\%04x%04x\\%s"),
            p_translate[i].w_language,
            p_translate[i].w_codePage,
            p_value_name);
        if (FAILED(hr))
        {
            return TEXT("");
        }
    }

    void* p_data = NULL;
    UINT u_len = 0;
    l_ret = ::VerQueryValue(m_p_version_data, sz_sub_block, &p_data, &u_len);
    if (l_ret == 0)
        return TEXT("");
    return (TCHAR*)p_data;
}
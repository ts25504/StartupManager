#include "stdafx.h"

#include "TSRegistry.h"
#include "TSFileVersionInfo.h"

TSRegistry::TSRegistry(HKEY h_key)
{
    m_h_key = h_key;
    m_h_origin_key = h_key;
    memset(m_sz_subkey, 0, c_dw_max_key_length);
}
TSRegistry::~TSRegistry()
{
    Close();
}

bool TSRegistry::CreateKey(const wchar_t* lp_subkey, REGSAM sam_desired)
{
    HKEY h_key = NULL;
    DWORD dw_disposition = 0;
    bool b_ret = false;
    long l_ret = ::RegCreateKeyEx(
        m_h_key,
        lp_subkey,
        0,
        NULL,
        REG_OPTION_VOLATILE,
        sam_desired | KEY_WOW64_64KEY,
        NULL,
        &h_key,
        &dw_disposition);

    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }
    m_h_key = h_key;
    b_ret = true;
exit:
    return b_ret;
}

bool TSRegistry::Open(const wchar_t* p_subkey, REGSAM sam_desired)
{
    HKEY h_key = NULL;
    bool b_ret = false;
    long l_ret = ::RegOpenKeyEx(
        m_h_key,
        p_subkey,
        0,
        sam_desired | KEY_WOW64_64KEY,
        &h_key);
    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }
    m_h_key = h_key;
    wcscpy_s(m_sz_subkey, c_dw_max_key_length, p_subkey);
    b_ret = true;
exit:
    return b_ret;
}

bool TSRegistry::DeleteValue(const wchar_t* p_value_name)
{
    long l_ret = ::RegDeleteValue(m_h_key, p_value_name);
    bool b_ret = false;
    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }
    b_ret = true; 
exit:
    return b_ret;
}

bool TSRegistry::DeleteKey(const wchar_t* p_subkey)
{
    long l_ret = ::RegDeleteKeyEx(m_h_key, p_subkey, KEY_WOW64_64KEY, 0);
    bool b_ret = false;
    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }
    b_ret = true;
exit:
    return b_ret;
}

bool TSRegistry::Read(const wchar_t* p_value_name, byte* p_data)
{
    DWORD dw_size = 0;
    DWORD dw_type = 0;
    bool b_ret = false;
    long l_ret = ::RegQueryValueEx(
        m_h_key,
        p_value_name,
        NULL,
        &dw_type,
        NULL,
        &dw_size);

    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }

    l_ret = ::RegQueryValueEx(
        m_h_key,
        p_value_name,
        NULL,
        &dw_type,
        (byte*)p_data,
        &dw_size);

    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }

    b_ret = true;
exit:
    return b_ret;
}

bool TSRegistry::Write(const wchar_t* p_value_name, const wchar_t* p_data)
{
    long l_ret = ::RegSetValueEx(
        m_h_key,
        p_value_name,
        0,
        REG_SZ,
        (byte*)p_data,
        (DWORD)(wcslen(p_data)*sizeof(wchar_t)));

    bool b_ret = false;

    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }

    b_ret = true;
exit:
    return b_ret;
}

void TSRegistry::Close()
{
    if (m_h_key)
    {
        ::RegCloseKey(m_h_key);
        m_h_key = NULL;
    }
    if (m_h_origin_key)
    {
        ::RegCloseKey(m_h_origin_key);
        m_h_origin_key = NULL;
    }
}

bool TSRegistry::Query(std::vector<ValueInfo>& vi_vec)
{ 
    wchar_t sz_ach_key[c_dw_max_key_length] = {0}; // buffer for subkey name
    DWORD dw_cb_name = 0;                          // size of name string
    wchar_t sz_ach_class[c_dw_max_path] = L"";     // buffer for class name
    DWORD dw_cch_classname = c_dw_max_path;        // size of class string
    DWORD dw_c_subkeys = 0;                        // number of subkeys
    DWORD dw_cb_max_subkey = 0;                    // longest subkey size
    DWORD dw_cch_max_class = 0;                    // longest class string
    DWORD dw_c_values = 0;                         // number of values for key
    DWORD dw_cch_max_value_name = 0;               // longest value name
    DWORD dw_cb_max_value = 0;                     // longest value data
    DWORD dw_cb_security_descriptor = 0;           // size of security descriptor
    FILETIME ft_lastwritetime = {0};               // last write time

    bool b_ret = false;
    long l_ret = 0;

    // Get the class name and the value count.
    l_ret = ::RegQueryInfoKey(
        m_h_key,                    // key handle
        sz_ach_class,               // buffer for class name
        &dw_cch_classname,          // size of class string
        NULL,                       // reserved
        &dw_c_subkeys,              // number of subkeys
        &dw_cb_max_subkey,          // longest subkey size
        &dw_cch_max_class,          // longest class string
        &dw_c_values,               // number of values for this key
        &dw_cch_max_value_name,          // longest value name
        &dw_cb_max_value,       // longest value data
        &dw_cb_security_descriptor, // security descriptor
        &ft_lastwritetime);         // last write time

    if (l_ret != ERROR_SUCCESS)
        goto exit;

    // Enumerate the key values

    if (dw_c_values)
    {
        EnumValue(vi_vec, dw_c_values, dw_cch_max_value_name*sizeof(wchar_t), dw_cb_max_value);
    }
    b_ret = true;
exit:
    return b_ret;
}

void TSRegistry::EnumValue(std::vector<ValueInfo>& vi_vec, DWORD dw_c_values, DWORD dw_cch_max_value_name, DWORD dw_cb_max_value)
{
    long l_ret = 0;
    TSFileVersionInfo fi;
    wchar_t* p_value_name = new wchar_t[dw_cch_max_value_name];
    if (p_value_name == NULL)
        goto exit;
    memset(p_value_name, 0, dw_cch_max_value_name);

    DWORD dw_cch_value_name = dw_cch_max_value_name;

    wchar_t* p_value = new wchar_t[dw_cb_max_value/sizeof(wchar_t)];
    if (p_value == NULL)
        goto exit;
    memset(p_value, 0, dw_cb_max_value/sizeof(wchar_t));

    DWORD dw_cb_value = dw_cb_max_value;

    for (ULONG i = 0, l_ret = ERROR_SUCCESS; i < dw_c_values; i++)
    {
        ValueInfo vi;
        dw_cch_value_name = dw_cch_max_value_name;
        dw_cb_value = dw_cb_max_value;
        vi.h_key = m_h_origin_key;
        vi.state = 1;
        vi.sz_subkey = m_sz_subkey;
        l_ret = ::RegEnumValue(
            m_h_key,
            i,
            p_value_name,
            &dw_cch_value_name,
            NULL,
            NULL,
            (byte*)p_value,
            &dw_cb_value);

        vi.sz_value_name = p_value_name;
        vi.sz_value = p_value;
        wchar_t* p_parsed_path = TSUtils::GetInstance()->ParsePath(vi.sz_value.c_str());
        fi.Open(p_parsed_path);
        vi.sz_product_name = fi.GetProductName();
        fi.Close();
        if (l_ret == ERROR_SUCCESS )
        {
            vi_vec.push_back(vi);
        }
    }
exit:
    if (p_value_name)
    {
        delete[] p_value_name;
        p_value_name = NULL;
    }
    if (p_value)
    {
        delete[] p_value;
        p_value = NULL;
    }
}

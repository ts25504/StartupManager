#include "stdafx.h"
#include "RegistryRun.h"
#include "FileInfo.h"

RegistryRun::RegistryRun(HKEY h_key)
{
    m_h_key = h_key;
    m_h_origin_key = h_key;
    memset(m_sz_subkey, 0, MAX_KEY_LENGTH);
}
RegistryRun::~RegistryRun()
{
    Close();
}

bool RegistryRun::CreateKey(const wchar_t* lp_subkey, REGSAM sam_desired)
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

bool RegistryRun::Open(const wchar_t* p_subkey, REGSAM sam_desired)
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
    wcscpy_s(m_sz_subkey, MAX_KEY_LENGTH, p_subkey);
    b_ret = true;
exit:
    return b_ret;
}

bool RegistryRun::DeleteValue(const wchar_t* p_value_name)
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

bool RegistryRun::DeleteKey(const wchar_t* p_subkey)
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

bool RegistryRun::Read(const wchar_t* p_value_name, byte* p_data)
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

bool RegistryRun::Write(const wchar_t* p_value_name, const wchar_t* p_data)
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

void RegistryRun::Close()
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

bool RegistryRun::Query(std::vector<ValueInfo>& vi_vec)
{ 
    wchar_t sz_ach_key[MAX_KEY_LENGTH] = {0};   // buffer for subkey name
    DWORD dw_cb_name = 0;                     // size of name string
    wchar_t sz_ach_class[MAX_PATH] = L"";  // buffer for class name
    DWORD dw_cch_classname = MAX_PATH;        // size of class string
    DWORD dw_c_subkeys = 0;                   // number of subkeys
    DWORD dw_cb_max_subKey = 0;               // longest subkey size
    DWORD dw_cch_max_class = 0;               // longest class string
    DWORD dw_c_values = 0;                    // number of values for key
    DWORD dw_cch_max_value = 0;               // longest value name
    DWORD dw_cb_max_valuedata = 0;            // longest value data
    DWORD dw_cb_security_descriptor = 0;      // size of security descriptor
    FILETIME ft_lastwritetime = {0};          // last write time

    FileInfo fi;

    DWORD i = 0;
    long l_ret = 0;
    bool b_ret = false;

    wchar_t sz_ach_value[MAX_VALUE_NAME]  = {0};
    DWORD dw_cch_value = MAX_VALUE_NAME;

    wchar_t sz_ach_data[MAX_VALUE] = {0};
    DWORD dw_cch_data = MAX_VALUE;

    // Get the class name and the value count.
    l_ret = ::RegQueryInfoKey(
        m_h_key,                    // key handle
        sz_ach_class,               // buffer for class name
        &dw_cch_classname,          // size of class string
        NULL,                       // reserved
        &dw_c_subkeys,              // number of subkeys
        &dw_cb_max_subKey,          // longest subkey size
        &dw_cch_max_class,          // longest class string
        &dw_c_values,               // number of values for this key
        &dw_cch_max_value,          // longest value name
        &dw_cb_max_valuedata,       // longest value data
        &dw_cb_security_descriptor, // security descriptor
        &ft_lastwritetime);         // last write time

    if (l_ret != ERROR_SUCCESS)
        goto exit;

    // Enumerate the key values.

    if (dw_c_values) 
    {
        for (i = 0, l_ret = ERROR_SUCCESS; i < dw_c_values; i++)
        {
            ValueInfo vi = {0};
            dw_cch_value = MAX_VALUE_NAME;
            dw_cch_data = MAX_VALUE;
            vi.sz_value_name[0] = '\0';
            vi.sz_value[0] = '\0';
            vi.h_key = m_h_origin_key;
            vi.state = 1;
            wcscpy_s(vi.sz_subkey, MAX_KEY_LENGTH, m_sz_subkey);
            l_ret = ::RegEnumValue(
                m_h_key,
                i,
                vi.sz_value_name,
                &dw_cch_value,
                NULL,
                NULL,
                (byte*)vi.sz_value,
                &dw_cch_data);

            fi.Open(vi.sz_value);
            wcscpy_s(vi.sz_product_name, MAX_PATH, fi.GetProductName());
            fi.Close();
            if (l_ret == ERROR_SUCCESS )
            {
                vi_vec.push_back(vi);
            }
        }
    }
    b_ret = true;
exit:
    return b_ret;
}

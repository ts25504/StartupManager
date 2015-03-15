#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include "MyRegistry.h"

MyRegistry::MyRegistry(HKEY hkey)
{
    m_hkey = hkey;
    m_origin_hkey = hkey;
    memset(m_sz_subkey, 0, MAX_PATH);
}
MyRegistry::~MyRegistry()
{
    Close();
}

bool MyRegistry::CreateKey(const TCHAR* lp_subkey)
{
    HKEY hkey = NULL;
    DWORD dw_disposition = 0;
    bool b_ret = false;
    long l_ret = ::RegCreateKeyEx(
        m_hkey,
        lp_subkey,
        0,
        NULL,
        REG_OPTION_VOLATILE,
        0,
        NULL,
        &hkey,
        &dw_disposition);

    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }
    m_hkey = hkey;
    b_ret = true;
exit:
    return b_ret;
}

bool MyRegistry::Open(const TCHAR* lp_subkey, REGSAM sam_desired)
{
    HKEY hkey = NULL;
    bool b_ret = false;
    long l_ret = ::RegOpenKeyEx(m_hkey, lp_subkey, 0, sam_desired, &hkey);
    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }
    m_hkey = hkey;
    _tcscpy_s(m_sz_subkey, MAX_PATH, lp_subkey);
    b_ret = true;
exit:
    return b_ret;
}

bool MyRegistry::DeleteValue(const TCHAR* lp_valuename)
{
    long l_ret = ::RegDeleteValue(m_hkey, lp_valuename);
    bool b_ret = false;
    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }
    b_ret = true; 
exit:
    return b_ret;
}

bool MyRegistry::DeleteKey(const TCHAR* lp_subkey)
{
    long l_ret = ::RegDeleteKey(m_hkey, lp_subkey);
    bool b_ret = false;
    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }
    b_ret = true;
exit:
    return b_ret;
}

bool MyRegistry::Read(const TCHAR* lp_valuename, byte* lp_data)
{
    DWORD dw_size = 0;
    DWORD dw_type = 0;
    bool b_ret = false;
    long l_ret = ::RegQueryValueEx(
        m_hkey,
        lp_valuename,
        NULL,
        &dw_type,
        NULL,
        &dw_size);

    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }

    l_ret = ::RegQueryValueEx(
        m_hkey,
        lp_valuename,
        NULL,
        &dw_type,
        (byte*)lp_data,
        &dw_size);

    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }

    b_ret = true;
exit:
    return b_ret;
}

bool MyRegistry::Write(const TCHAR* lp_valuename, const TCHAR* lp_data)
{
    long l_ret = ::RegSetValueEx(
        m_hkey,
        lp_valuename,
        0,
        REG_SZ,
        (byte*)lp_data,
        (DWORD)(_tcslen(lp_data)*sizeof(TCHAR)));

    bool b_ret = false;

    if (l_ret != ERROR_SUCCESS)
    {
        goto exit;
    }

    b_ret = true;
exit:
    return b_ret;
}

void MyRegistry::Close()
{
    if (m_hkey)
    {
        ::RegCloseKey(m_hkey);
        m_hkey = NULL;
    }
    if (m_origin_hkey)
    {
        ::RegCloseKey(m_origin_hkey);
        m_origin_hkey = NULL;
    }
}

bool MyRegistry::Query(std::vector<ValueInfo>& vi_vec)
{ 
    TCHAR    ach_key[MAX_KEY_LENGTH] = {0};   // buffer for subkey name
    DWORD    cb_name = 0;                   // size of name string
    TCHAR    ach_class[MAX_PATH] = TEXT("");  // buffer for class name
    DWORD    cch_classname = MAX_PATH;  // size of class string
    DWORD    c_subkeys = 0;               // number of subkeys
    DWORD    cb_max_subKey = 0;              // longest subkey size
    DWORD    cch_max_class = 0;              // longest class string
    DWORD    c_values = 0;              // number of values for key
    DWORD    cch_max_value = 0;          // longest value name
    DWORD    cb_max_valuedata = 0;       // longest value data
    DWORD    cb_security_descriptor = 0; // size of security descriptor
    FILETIME ft_lastwritetime = {0};      // last write time

    DWORD i = 0, ret = 0;
    bool b_ret = false;
    ValueInfo vi = {0};

    TCHAR ach_value[MAX_VALUE_NAME]  = {0};
    DWORD cch_value = MAX_VALUE_NAME;

    TCHAR ach_data[MAX_VALUE] = {0};
    DWORD cch_data = MAX_VALUE;

    // Get the class name and the value count.
    ret = ::RegQueryInfoKey(
        m_hkey,                    // key handle
        ach_class,                // buffer for class name
        &cch_classname,           // size of class string
        NULL,                    // reserved
        &c_subkeys,               // number of subkeys
        &cb_max_subKey,            // longest subkey size
        &cch_max_class,            // longest class string
        &c_values,                // number of values for this key
        &cch_max_value,            // longest value name
        &cb_max_valuedata,         // longest value data
        &cb_security_descriptor,   // security descriptor
        &ft_lastwritetime);       // last write time

    // Enumerate the subkeys, until RegEnumKeyEx fails.

    if (c_subkeys)
    {
        printf( "\nNumber of subkeys: %d\n", c_subkeys);

        for (i = 0; i < c_subkeys; i++) 
        {
            cb_name = MAX_KEY_LENGTH;
            ret = ::RegEnumKeyEx(
                m_hkey,
                i,
                ach_key,
                &cb_name,
                NULL,
                NULL,
                NULL,
                &ft_lastwritetime);
            if (ret == ERROR_SUCCESS)
            {
                _tprintf(TEXT("(%d) %s\n"), i+1, ach_key);
            }
        }
    }

    // Enumerate the key values.

    if (c_values) 
    {
        //printf( "\nNumber of values: %d\n", c_values);

        for (i = 0, ret = ERROR_SUCCESS; i < c_values; i++)
        {
            cch_value = MAX_VALUE_NAME;
            cch_data = MAX_VALUE;
            //ach_value[0] = '\0';
            //ach_data[0] = '\0';
            vi.sz_value_name[0] = '\0';
            vi.sz_value[0] = '\0';
            vi.hkey = m_origin_hkey;
            vi.state = 1;
            _tcscpy_s(vi.sz_subkey, MAX_PATH, m_sz_subkey);
            ret = ::RegEnumValue(
                m_hkey,
                i,
                //ach_value,
                vi.sz_value_name,
                &cch_value,
                NULL,
                NULL,
                //(byte*)ach_data,
                (byte*)vi.sz_value,
                &cch_data);

            if (ret == ERROR_SUCCESS )
            {
                //_tprintf(TEXT("(%d) %s %s\n"), i+1, vi.sz_value_name, vi.sz_value);
                bool b_push = true;
                
                for (ULONG i = 0; i < vi_vec.size(); ++i)
                {
                    if (_tcscmp(vi_vec[i].sz_value_name, vi.sz_value_name) == 0 &&
                        _tcscmp(vi_vec[i].sz_subkey, vi.sz_subkey) == 0)
                    b_push = false;
                }

                if (b_push)
                    vi_vec.push_back(vi);
                //_tprintf(TEXT("(%d) %s %s\n"), i+1, ach_value, ach_data);
            }
        }
    }
    return true;
}

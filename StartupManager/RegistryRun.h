#ifndef _MyRegistry_H_
#define _MyRegistry_H_

#include <vector>
#include "Constants.h"

struct ValueInfo {
    wchar_t sz_value_name[MAX_VALUE_NAME];
    wchar_t sz_value[MAX_VALUE];
    wchar_t sz_product_name[MAX_PATH];
    wchar_t sz_subkey[MAX_KEY_LENGTH];
    HKEY h_key;
    int state;
};

class RegistryRun
{
public:
    RegistryRun(HKEY h_key);
    ~RegistryRun();

    bool CreateKey(const wchar_t* p_subkey, REGSAM sam_desired);
    bool Open(const wchar_t* p_subkey, REGSAM sam_desired);
    bool DeleteValue(const wchar_t* p_value_name);
    bool DeleteKey(const wchar_t* p_subkey);
    bool Read(const wchar_t* p_value_name, byte* p_data);
    bool Write(const wchar_t* p_value_name, const wchar_t* p_data);
    bool Query(std::vector<ValueInfo>& vi_vec);
    void Close();

private:
    HKEY m_h_key;
    HKEY m_h_origin_key;
    wchar_t m_sz_subkey[MAX_KEY_LENGTH];
};

#endif
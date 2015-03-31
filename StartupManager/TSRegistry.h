#ifndef _TSRegistry_H_
#define _TSRegistry_H_

#include <vector>

#include "TSUtils.h"
#include "Constants.h"

class TSRegistry
{
public:
    TSRegistry(HKEY h_key);
    ~TSRegistry();

    bool CreateKey(const wchar_t* p_subkey, const REGSAM sam_desired);
    bool Open(const wchar_t* p_subkey, const REGSAM sam_desired);
    bool DeleteValue(const wchar_t* p_value_name);
    bool DeleteKey(const wchar_t* p_subkey);
    bool Read(const wchar_t* p_value_name, byte* p_data);
    bool Write(const wchar_t* p_value_name, const wchar_t* p_data);
    bool Query(std::vector<ValueInfo>& vi_vec);
    void EnumValue(std::vector<ValueInfo>& vi_vec, DWORD dw_c_values, DWORD dw_cch_max_value_name, DWORD dw_cb_max_value);
    void Close();

private:
    HKEY m_h_key;
    HKEY m_h_origin_key;
    wchar_t m_sz_subkey[c_dw_max_key_length];
};

#endif
#ifndef _TSRegistry_H_
#define _TSRegistry_H_

#include <vector>

#include "Utils.h"
#include "Constants.h"

class TSRegistry
{
public:
    TSRegistry(HKEY h_key);
    ~TSRegistry();

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
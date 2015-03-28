#ifndef _MyRegistry_H_
#define _MyRegistry_H_

#include <vector>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define MAX_VALUE 32767

struct ValueInfo {
    TCHAR sz_value_name[MAX_VALUE_NAME];
    TCHAR sz_value[MAX_VALUE];
    TCHAR sz_product_name[MAX_PATH];
    HKEY h_key;
    TCHAR sz_subkey[MAX_KEY_LENGTH];
    int state;
};

class RegistryRun
{
public:
    RegistryRun(HKEY h_key);
    ~RegistryRun();

    bool CreateKey(const TCHAR* lp_subkey, REGSAM sam_desired);
    bool Open(const TCHAR* lp_subkey, REGSAM sam_desired);
    bool DeleteValue(const TCHAR* lp_valuename);
    bool DeleteKey(const TCHAR* lp_subkey);
    bool Read(const TCHAR* lp_valuename, byte* lp_data);
    bool Write(const TCHAR* lp_valuename, const TCHAR* lp_data);
    bool Query(std::vector<ValueInfo>& vi_vec);
    void Close();

private:
    HKEY m_h_key;
    HKEY m_h_origin_key;
    TCHAR m_sz_subkey[MAX_KEY_LENGTH];
};

#endif
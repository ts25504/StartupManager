#ifndef _Utils_H_
#define _Utils_H_

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

class Utils
{
public:
    static Utils* GetInstance();
    void ReadDisabledItemsFromFile(std::vector<ValueInfo>& vi_vec);
    void WriteDisabledItemsToFile(std::vector<ValueInfo>& vi_vec);
    void AddItems(HKEY h_key, const wchar_t* sz_subkey, std::vector<ValueInfo>& vi_vec);
    void AddItems(std::vector<ValueInfo>& vi_vec);
    void DeleteItem(ValueInfo& vi);
    void ResetItem(ValueInfo& vi);
    wchar_t* ParsePath(const wchar_t* p_file_path);
private:
    void EnumFiles(std::vector<ValueInfo>& vi_vec, wchar_t* p_dir);
};

#endif
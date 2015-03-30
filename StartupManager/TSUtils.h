#ifndef _TSUtils_H_
#define _TSUtils_H_

#include <vector>
#include <string>

#include "Constants.h"

struct ValueInfo {
    ValueInfo() : h_key(NULL), state(0) { }
    std::wstring sz_value_name;
    std::wstring sz_value;
    std::wstring sz_product_name;
    std::wstring sz_subkey;
    HKEY h_key;
    int state;
};

class TSUtils
{
public:
    static TSUtils* GetInstance();
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
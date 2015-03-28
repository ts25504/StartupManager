#ifndef _ListItems_H_
#define _ListItems_H_

#include "RegistryRun.h"

class ListItems
{
public:
    static ListItems* GetInstance();
    void ReadDisabledItemsFromFile(std::vector<ValueInfo>& vi_vec);
    void WriteDisabledItemsToFile(std::vector<ValueInfo>& vi_vec);
    void AddItems(HKEY h_key, const TCHAR* sz_subkey, std::vector<ValueInfo>& vi_vec);
    void AddItems(std::vector<ValueInfo>& vi_vec);
    void DeleteItem(ValueInfo& vi);
    void ResetItem(ValueInfo& vi);
};

#endif
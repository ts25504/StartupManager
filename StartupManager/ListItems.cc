#include "stdafx.h"
#include <shlobj.h>
#include "ListItems.h"
#include "FileInfo.h"

ListItems* ListItems::GetInstance()
{
    static ListItems li;
    return &li;
}
void ListItems::ReadDisabledItemsFromFile(std::vector<ValueInfo>& vi_vec)
{
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DisabledItems.ini", "r+");
    if (err != 0)
        return;
    if (feof(fp))
        return;
    TCHAR sz_key[MAX_PATH] = {0};
    size_t num_of_disabled_items = 0;
    _ftscanf_s(fp, TEXT("%d\n"), &num_of_disabled_items);
    for (ULONG i = 0; i < num_of_disabled_items; ++i)
    {
        BOOL b_push = true;
        ValueInfo vi = {0};
        _fgetts(vi.sz_value_name, MAX_VALUE_NAME, fp); // fgetts() read \0.
        vi.sz_value_name[_tcslen(vi.sz_value_name)-1] = TEXT('\0');
        _fgetts(vi.sz_value, MAX_VALUE, fp);
        vi.sz_value[_tcslen(vi.sz_value)-1] = TEXT('\0');
        _fgetts(sz_key, MAX_KEY_LENGTH, fp);
        sz_key[_tcslen(sz_key)-1] = TEXT('\0');
        _fgetts(vi.sz_subkey, MAX_KEY_LENGTH, fp);
        vi.sz_subkey[_tcslen(vi.sz_subkey)-1] = TEXT('\0');
        _fgetts(vi.sz_product_name, MAX_PATH, fp);
        vi.sz_product_name[_tcslen(vi.sz_product_name)-1] = TEXT('\0');
        _ftscanf_s(fp, TEXT("%d\n"), &vi.state);
        if(_tcscmp(sz_key, TEXT("HKEY_CLASSES_ROOT")) == 0)
            vi.h_key = HKEY_CLASSES_ROOT;
        else if (_tcscmp(sz_key, TEXT("HKEY_CURRENT_USER")) == 0)
            vi.h_key = HKEY_CURRENT_USER;
        else if (_tcscmp(sz_key, TEXT("HKEY_LOCAL_MACHINE")) == 0)
            vi.h_key = HKEY_LOCAL_MACHINE;
        else if (_tcscmp(sz_key, TEXT("HKEY_USERS\n")) == 0)
            vi.h_key = HKEY_USERS;
        else if (_tcscmp(sz_key, TEXT("HKEY_CURRENT_CONFIG")) == 0)
            vi.h_key = HKEY_CURRENT_CONFIG;
        for (ULONG i = 0; i < vi_vec.size(); ++i)
        {
            if (_tcscmp(vi_vec[i].sz_value_name, vi.sz_value_name) == 0 &&
                _tcscmp(vi_vec[i].sz_value, vi_vec[i].sz_value) == 0 &&
                _tcscmp(vi_vec[i].sz_subkey, vi.sz_subkey) == 0 &&
                vi_vec[i].h_key == vi.h_key)
            b_push = false;
        }

        if (b_push)
            vi_vec.push_back(vi);
    }

    fclose(fp);
}

void ListItems::WriteDisabledItemsToFile(std::vector<ValueInfo>& vi_vec)
{
    FILE* fp = NULL;
    errno_t err = fopen_s(&fp, "DisabledItems.ini", "w+");
    if (err != 0)
        return;
    TCHAR sz_key[MAX_KEY_LENGTH] = {0};
    size_t num_of_disabled_items = 0;
    for (ULONG i = 0; i < vi_vec.size(); ++i)
    {
        if (vi_vec[i].state == 0)
            ++num_of_disabled_items;
    }
    _ftprintf(fp, TEXT("%d\n"), num_of_disabled_items);
    for (ULONG i = 0; i < vi_vec.size(); ++i)
    {
        if (vi_vec[i].state == 0)
        {
            if(vi_vec[i].h_key == HKEY_CLASSES_ROOT)
                _tcscpy_s(sz_key, MAX_KEY_LENGTH, TEXT("HKEY_CLASSES_ROOT"));
            else if (vi_vec[i].h_key == HKEY_CURRENT_USER)
                _tcscpy_s(sz_key, MAX_KEY_LENGTH, TEXT("HKEY_CURRENT_USER"));
            else if (vi_vec[i].h_key == HKEY_LOCAL_MACHINE)
                _tcscpy_s(sz_key, MAX_KEY_LENGTH, TEXT("HKEY_LOCAL_MACHINE"));
            else if (vi_vec[i].h_key == HKEY_USERS)
                _tcscpy_s(sz_key, MAX_KEY_LENGTH, TEXT("HKEY_USERS"));
            else if (vi_vec[i].h_key == HKEY_CURRENT_CONFIG)
                _tcscpy_s(sz_key, MAX_KEY_LENGTH, TEXT("HKEY_CURRENT_CONFIG"));

            _ftprintf(fp, TEXT("%s\n%s\n%s\n%s\n%s\n"), vi_vec[i].sz_value_name, vi_vec[i].sz_value,
                sz_key, vi_vec[i].sz_subkey, vi_vec[i].sz_product_name);
            _ftprintf(fp, TEXT("%d\n"), vi_vec[i].state);
        }
    }
    fclose(fp);
}

void ListItems::AddItems(HKEY h_key, const TCHAR* sz_subkey, std::vector<ValueInfo>& vi_vec)
{
    RegistryRun my_reg(h_key);
    my_reg.Open(sz_subkey, KEY_READ);
    my_reg.Query(vi_vec);
    my_reg.Close();
}

void EnumFiles(std::vector<ValueInfo>& vi_vec, TCHAR* p_dir)
{
    TCHAR sz_tmp[MAX_PATH] = {0};
    TCHAR sz_path[MAX_PATH] = {0};
    _tcscpy_s(sz_tmp, MAX_PATH, p_dir);
    _tcscat_s(sz_tmp, TEXT("\\*.*"));
    WIN32_FIND_DATA file_data = {0};
    FileInfo fi;
    HANDLE h_search = ::FindFirstFile(sz_tmp, &file_data);
    if (h_search == INVALID_HANDLE_VALUE)
        return;
    BOOL b_not_finish = TRUE;
    while (b_not_finish)
    {
        if (_tcscmp(file_data.cFileName, TEXT(".")) == 0 ||
            _tcscmp(file_data.cFileName, TEXT("..")) == 0 ||
            _tcscmp(file_data.cFileName, TEXT("desktop.ini")) == 0)
        {
            b_not_finish = ::FindNextFile(h_search, &file_data);
            continue;
        }
        _stprintf_s(sz_path, TEXT("%s\\%s"), p_dir, file_data.cFileName);
        ValueInfo vi;
        vi.h_key = NULL;
        _tcscpy_s(vi.sz_value_name, MAX_VALUE_NAME, file_data.cFileName);
        _tcscpy_s(vi.sz_value, MAX_VALUE, sz_path);
        _tcscpy_s(vi.sz_subkey, MAX_KEY_LENGTH, p_dir);
        fi.Open(vi.sz_value);
        _tcscpy_s(vi.sz_product_name, MAX_PATH, fi.GetProductName());
        fi.Close();
        vi.state = 1;
        vi_vec.push_back(vi);
        b_not_finish = ::FindNextFile(h_search, &file_data);
    }
    ::FindClose(h_search);
}

void ListItems::AddItems(std::vector<ValueInfo>& vi_vec)
{
    TCHAR sz_common_startup[MAX_PATH] = {0};
    TCHAR sz_startup[MAX_PATH] = {0};
    ::SHGetSpecialFolderPath(NULL, sz_common_startup, CSIDL_COMMON_STARTUP, 0);
    ::SHGetSpecialFolderPath(NULL, sz_startup, CSIDL_STARTUP, 0);
    EnumFiles(vi_vec, sz_common_startup);
    EnumFiles(vi_vec, sz_startup);
}

void ListItems::DeleteItem(ValueInfo& vi)
{
    if (vi.h_key)
    {
        RegistryRun my_reg(vi.h_key);
        my_reg.Open(vi.sz_subkey, KEY_SET_VALUE);
        if (my_reg.DeleteValue(vi.sz_value_name))
            vi.state = 0;
        my_reg.Close();
    }
    else
    {
        TCHAR sz_folder_name[] = TEXT("C:\\DisabledItemsFromStartUp");
        BOOL b_ret = ::CreateDirectory(sz_folder_name, NULL);
        if (b_ret == 0 && ::GetLastError() != ERROR_ALREADY_EXISTS)
            return;
        TCHAR sz_new_path[MAX_PATH] = {0};
        _stprintf_s(sz_new_path, TEXT("%s\\%s"), sz_folder_name, vi.sz_value_name);
        b_ret = ::MoveFileEx(vi.sz_value, sz_new_path, MOVEFILE_REPLACE_EXISTING);
        if (b_ret == 0)
            return;
        vi.state = 0;
    }
}

void ListItems::ResetItem(ValueInfo& vi)
{
    if (vi.h_key)
    {
        RegistryRun my_reg(vi.h_key);
        my_reg.Open(vi.sz_subkey, KEY_WRITE);
        if (my_reg.Write(vi.sz_value_name, vi.sz_value))
            vi.state = 1;
        my_reg.Close();
    }
    else
    {
        TCHAR sz_folder_name[] = TEXT("C:\\DisabledItemsFromStartUp");
        BOOL b_ret = ::CreateDirectory(sz_folder_name, NULL);
        if (b_ret == 0 && ::GetLastError() != ERROR_ALREADY_EXISTS)
            return;
        TCHAR sz_exist_path[MAX_PATH] = {0};
        _stprintf_s(sz_exist_path, TEXT("%s\\%s"), sz_folder_name, vi.sz_value_name);
        b_ret = ::MoveFileEx(sz_exist_path, vi.sz_value, MOVEFILE_REPLACE_EXISTING);
        if (b_ret == 0)
            return;
        vi.state = 1;
    }
}

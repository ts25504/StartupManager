#include "stdafx.h"

#include <Shlwapi.h>
#include <fstream>
#include <iostream>

#include "TSUtils.h"
#include "TSRegistry.h"
#include "TSFileVersionInfo.h"

TSUtils* TSUtils::GetInstance()
{
    static TSUtils utils;
    return &utils;
}

void TSUtils::ReadDisabledItemsFromFile(std::vector<ValueInfo>& vi_vec)
{
    std::wifstream fin("DisabledItems.ini", std::wifstream::in);
    std::wstring sz_key, sz_tmp;
    size_t num_of_disabled_items = 0;
    fin >> num_of_disabled_items;
    std::getline(fin, sz_tmp);
    for (ULONG i = 0; i < num_of_disabled_items; ++i)
    {
        bool b_push = true;
        ValueInfo vi;
        std::getline(fin, vi.sz_value_name);
        std::getline(fin, vi.sz_value);
        std::getline(fin, sz_key);
        std::getline(fin, vi.sz_subkey);
        std::getline(fin, vi.sz_product_name);
        fin >> vi.state;
        std::getline(fin, sz_tmp);
        if(sz_key == L"HKEY_CLASSES_ROOT")
            vi.h_key = HKEY_CLASSES_ROOT;
        else if (sz_key == L"HKEY_CURRENT_USER")
            vi.h_key = HKEY_CURRENT_USER;
        else if (sz_key == L"HKEY_LOCAL_MACHINE")
            vi.h_key = HKEY_LOCAL_MACHINE;
        else if (sz_key == L"HKEY_USERS")
            vi.h_key = HKEY_USERS;
        else if (sz_key == L"HKEY_CURRENT_CONFIG")
            vi.h_key = HKEY_CURRENT_CONFIG;
        else
            vi.h_key = NULL;
        for (ULONG i = 0; i < vi_vec.size(); ++i)
        {
            if ((vi_vec[i].sz_value_name == vi.sz_value_name) &&
                (vi_vec[i].sz_value == vi_vec[i].sz_value) &&
                (vi_vec[i].sz_subkey == vi.sz_subkey) &&
                (vi_vec[i].h_key == vi.h_key))
            b_push = false;
        }

        if (b_push)
            vi_vec.push_back(vi);
    }
}

void TSUtils::WriteDisabledItemsToFile(std::vector<ValueInfo>& vi_vec)
{
    std::wofstream fout("DisabledItems.ini", std::wofstream::out);
    size_t num_of_disabled_items = 0;
    std::wstring sz_key;
    for (ULONG i = 0; i < vi_vec.size(); ++i)
    {
        if (vi_vec[i].state == 0)
            ++num_of_disabled_items;
    }
    fout << num_of_disabled_items << std::endl;
    for (ULONG i = 0; i < vi_vec.size(); ++i)
    {
        if (vi_vec[i].state == 0)
        {
            if(vi_vec[i].h_key == HKEY_CLASSES_ROOT)
                sz_key = L"HKEY_CLASSES_ROOT";
            else if (vi_vec[i].h_key == HKEY_CURRENT_USER)
                sz_key = L"HKEY_CURRENT_USER";
            else if (vi_vec[i].h_key == HKEY_LOCAL_MACHINE)
                sz_key = L"HKEY_LOCAL_MACHINE";
            else if (vi_vec[i].h_key == HKEY_USERS)
                sz_key = L"HKEY_USERS";
            else if (vi_vec[i].h_key == HKEY_CURRENT_CONFIG)
                sz_key = L"HKEY_CURRENT_CONFIG";
            else
                sz_key = L"";

            fout << vi_vec[i].sz_value_name << std::endl
                << vi_vec[i].sz_value << std::endl
                << sz_key << std::endl
                << vi_vec[i].sz_subkey << std::endl
                << vi_vec[i].sz_product_name << std::endl
                << vi_vec[i].state << std::endl;
        }
    }
}

void TSUtils::AddItems(HKEY h_key, const wchar_t* sz_subkey, std::vector<ValueInfo>& vi_vec)
{
    TSRegistry my_reg(h_key);
    my_reg.Open(sz_subkey, KEY_READ);
    my_reg.Query(vi_vec);
    my_reg.Close();
}

void TSUtils::EnumFiles(std::vector<ValueInfo>& vi_vec, wchar_t* p_dir)
{
    wchar_t sz_tmp[c_dw_max_path] = {0};
    wchar_t sz_path[c_dw_max_path] = {0};
    wcscpy_s(sz_tmp, c_dw_max_path, p_dir);
    wcscat_s(sz_tmp, L"\\*.*");
    WIN32_FIND_DATA file_data = {0};
    TSFileVersionInfo fi;
    HANDLE h_search = ::FindFirstFile(sz_tmp, &file_data);
    if (h_search == INVALID_HANDLE_VALUE)
        return;
    BOOL b_not_finish = TRUE;
    while (b_not_finish)
    {
        if (wcscmp(file_data.cFileName, L".") == 0 ||
            wcscmp(file_data.cFileName, L"..") == 0 ||
            wcscmp(file_data.cFileName, L"desktop.ini") == 0)
        {
            b_not_finish = ::FindNextFile(h_search, &file_data);
            continue;
        }
        swprintf_s(sz_path, L"%s\\%s", p_dir, file_data.cFileName);
        ValueInfo vi;
        vi.h_key = NULL;
        vi.sz_value_name = file_data.cFileName;
        vi.sz_value = sz_path;
        vi.sz_subkey = p_dir;
        fi.Open(vi.sz_value.c_str());
        vi.sz_product_name = fi.GetProductName();
        fi.Close();
        vi.state = 1;
        vi_vec.push_back(vi);
        b_not_finish = ::FindNextFile(h_search, &file_data);
    }
    ::FindClose(h_search);
}

void TSUtils::AddItems(std::vector<ValueInfo>& vi_vec)
{
    wchar_t sz_common_startup[c_dw_max_path] = {0};
    wchar_t sz_startup[c_dw_max_path] = {0};
    ::SHGetSpecialFolderPath(NULL, sz_common_startup, CSIDL_COMMON_STARTUP, 0);
    ::SHGetSpecialFolderPath(NULL, sz_startup, CSIDL_STARTUP, 0);
    EnumFiles(vi_vec, sz_common_startup);
    EnumFiles(vi_vec, sz_startup);
}

void TSUtils::DeleteItem(ValueInfo& vi)
{
    if (vi.h_key)
    {
        TSRegistry my_reg(vi.h_key);
        my_reg.Open(vi.sz_subkey.c_str(), KEY_SET_VALUE);
        if (my_reg.DeleteValue(vi.sz_value_name.c_str()))
            vi.state = 0;
        my_reg.Close();
    }
    else
    {
        wchar_t sz_folder_name[] = L"C:\\DisabledItemsFromStartUp";
        BOOL b_ret = ::CreateDirectory(sz_folder_name, NULL);
        if (b_ret == 0 && ::GetLastError() != ERROR_ALREADY_EXISTS)
            return;
        wchar_t sz_new_path[c_dw_max_path] = {0};
        swprintf_s(sz_new_path, TEXT("%s\\%s"), sz_folder_name, vi.sz_value_name.c_str());
        b_ret = ::MoveFileEx(vi.sz_value.c_str(), sz_new_path, MOVEFILE_REPLACE_EXISTING);
        if (b_ret == 0)
            return;
        vi.state = 0;
    }
}

void TSUtils::ResetItem(ValueInfo& vi)
{
    if (vi.h_key)
    {
        TSRegistry my_reg(vi.h_key);
        my_reg.Open(vi.sz_subkey.c_str(), KEY_WRITE);
        if (my_reg.Write(vi.sz_value_name.c_str(), vi.sz_value.c_str()))
            vi.state = 1;
        my_reg.Close();
    }
    else
    {
        wchar_t sz_folder_name[] = TEXT("C:\\DisabledItemsFromStartUp");
        BOOL b_ret = ::CreateDirectory(sz_folder_name, NULL);
        if (b_ret == 0 && ::GetLastError() != ERROR_ALREADY_EXISTS)
            return;
        wchar_t sz_exist_path[c_dw_max_path] = {0};
        swprintf_s(sz_exist_path, L"%s\\%s", sz_folder_name, vi.sz_value_name.c_str());
        b_ret = ::MoveFileEx(sz_exist_path, vi.sz_value.c_str(), MOVEFILE_REPLACE_EXISTING);
        if (b_ret == 0)
            return;
        vi.state = 1;
    }
}

wchar_t* TSUtils::ParsePath(const wchar_t* p_file_path)
{
    if (::PathFileExists(p_file_path))
        return (wchar_t*)p_file_path;
    wchar_t* p_parsed_path = new wchar_t[c_dw_max_value];
    if (p_parsed_path == NULL)
        return L"error";
    memset(p_parsed_path, 0, c_dw_max_value);
    if (p_file_path[0] == L'\"')
        ++p_file_path;
    memcpy(p_parsed_path, p_file_path, wcslen(p_file_path)*sizeof(wchar_t));
    for (ULONG i = 0; i < wcslen(p_parsed_path); ++i)
    {
        if (p_parsed_path[i-3] == L'.' &&
            (p_parsed_path[i-2] == L'e' || p_parsed_path[i-2] == L'E') &&
            (p_parsed_path[i-1] == L'x' || p_parsed_path[i-1] == L'X') &&
            (p_parsed_path[i] == L'e' || p_parsed_path[i] == L'E'))
        {
            wcsncpy_s(p_parsed_path, wcslen(p_parsed_path)*sizeof(wchar_t), p_file_path, i+1);
            break;
        }
    }
    return p_parsed_path;
}

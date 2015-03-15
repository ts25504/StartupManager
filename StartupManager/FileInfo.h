#ifndef _FILEINFO_H_
#define _FILEINFO_H_

#include <Windows.h>

class FileInfo
{
public:
    FileInfo();
    ~FileInfo();
    bool Open(TCHAR* p_file_path);
    TCHAR* QueryValue(const TCHAR* p_value_name);
    void Close();
private:
    byte* m_p_version_data;
};

#endif
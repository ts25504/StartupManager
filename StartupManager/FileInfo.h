#ifndef _FILEINFO_H_
#define _FILEINFO_H_

#define MAX_VALUE 32767

class FileInfo
{
public:
    FileInfo();
    ~FileInfo();
    bool Open(TCHAR* p_file_path);
    TCHAR* GetFileDescription();
    TCHAR* GetInternelName();
    TCHAR* GetLegalTradeMarks();
    TCHAR* GetOriginalFileName();
    TCHAR* GetProductName();
    TCHAR* GetProductVersion();
    TCHAR* QueryValue(const TCHAR* p_value_name);
    int GetIconIndex(TCHAR* p_file_name);
    void Close();
private:
    TCHAR* ParsePath(TCHAR* p_file_path);
    byte* m_p_version_data;
};

#endif
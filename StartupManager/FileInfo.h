#ifndef _FILEINFO_H_
#define _FILEINFO_H_

#define MAX_VALUE 32767

class FileInfo
{
public:
    FileInfo();
    ~FileInfo();
    bool Open(wchar_t* p_file_path);
    wchar_t* GetFileDescription();
    wchar_t* GetInternelName();
    wchar_t* GetLegalTradeMarks();
    wchar_t* GetOriginalFileName();
    wchar_t* GetProductName();
    wchar_t* GetProductVersion();
    wchar_t* QueryValue(const wchar_t* p_value_name);
    int GetIconIndex(wchar_t* p_file_name);
    void Close();
private:
    wchar_t* ParsePath(wchar_t* p_file_path);
    byte* m_p_version_data;
};

#endif
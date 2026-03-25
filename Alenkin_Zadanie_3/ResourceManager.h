#pragma once
#include <windows.h>
#include <sqlext.h>
#include <string>
#include <iostream>
#include <regex>
#include <iomanip> 

class ResourceManager {
private:
    SQLHDBC hDbc;

    bool IsValidName(const std::wstring& name);
    bool IsValidExtension(const std::wstring& name);
    bool IsDuplicate(const std::wstring& name);
    std::wstring TruncateString(const std::wstring& str, size_t maxLength);

public:
    ResourceManager(SQLHDBC connectionHandle);
    bool AddResource(const std::wstring& name, SQLINTEGER size, SQLINTEGER categoryId, SQLINTEGER ownerId);
    void ShowResourcesPaged();
    void ShowStatistics();
};
#pragma once
#include <windows.h>
#include <sqlext.h>
#include <string>
#include <iostream>
#include <regex>

class ResourceManager {
private:
    SQLHDBC hDbc;

    bool IsValidName(const std::wstring& name);
    bool IsValidExtension(const std::wstring& name);
    bool IsDuplicate(const std::wstring& name);

public:
    ResourceManager(SQLHDBC connectionHandle);
    bool AddResource(const std::wstring& name, SQLINTEGER size, SQLINTEGER categoryId, SQLINTEGER ownerId);
};
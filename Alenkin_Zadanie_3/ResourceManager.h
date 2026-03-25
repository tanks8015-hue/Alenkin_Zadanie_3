#pragma once
#include <windows.h>
#include <sqlext.h>
#include <string>
#include <iostream>
#include <regex>

class ResourceManager {
private:
    SQLHDBC hDbc;

    // ќбъ€вл€ем проверки здесь, чтобы студи€ их видела
    bool IsValidName(const std::wstring& name);
    bool IsValidExtension(const std::wstring& name);

public:
    ResourceManager(SQLHDBC connectionHandle);
    bool AddResource(const std::wstring& name, SQLINTEGER size, SQLINTEGER categoryId, SQLINTEGER ownerId);
};
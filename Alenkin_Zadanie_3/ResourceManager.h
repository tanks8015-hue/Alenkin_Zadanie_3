#pragma once
#include <windows.h>
#include <sqlext.h>
#include <string>
#include <iostream>

class ResourceManager {
private:
    SQLHDBC hDbc;
public:
    ResourceManager(SQLHDBC connectionHandle);
    bool AddResource(const std::wstring& name, SQLINTEGER size, SQLINTEGER categoryId, SQLINTEGER ownerId);
};
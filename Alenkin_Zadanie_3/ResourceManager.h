#pragma once
#include <windows.h>
#include <sqlext.h>
#include <string>
#include <iostream>
#include <regex> // Нужно для проверки спецсимволов

class ResourceManager {
private:
    SQLHDBC hDbc;

    // Вот эта строка потерялась! Объявляем функцию валидации имени
    bool IsValidName(const std::wstring& name);

public:
    ResourceManager(SQLHDBC connectionHandle);
    bool AddResource(const std::wstring& name, SQLINTEGER size, SQLINTEGER categoryId, SQLINTEGER ownerId);
};
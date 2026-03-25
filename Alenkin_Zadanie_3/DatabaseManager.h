#pragma once
#include <windows.h>
#include <sqlext.h>
#include <iostream>
#include <string>

class DatabaseManager {
private:
    SQLHENV hEnv; 
    SQLHDBC hDbc; 
    void FreeHandles();
public:
    DatabaseManager();
    ~DatabaseManager();
    bool Connect(const std::wstring& connectionString);
    void Disconnect();
    bool Ping();

    SQLHDBC GetConnectionHandle() const { return hDbc; }
};
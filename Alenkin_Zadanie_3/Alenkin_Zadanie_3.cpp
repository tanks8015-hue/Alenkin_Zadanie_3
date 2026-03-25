#include <iostream>
#include "DatabaseManager.h"

int main() {
    setlocale(LC_ALL, "Russian");

    DatabaseManager db;
    std::wstring connStr = L"Driver={ODBC Driver 17 for SQL Server};Server=DESKTOP-PKN6175\\SQLEXPRESS;Database=ByteKeeperDB;Trusted_Connection=yes;";

    std::wcout << L"=== Тестирование подключения ===\n";
    if (db.Connect(connStr)) {
        db.Ping();
    }

    std::wcout << L"\n=== Тестирование смены БД на лету ===\n";
    db.Disconnect(); 

    return 0;
}
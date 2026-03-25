#include "DatabaseManager.h"

DatabaseManager::DatabaseManager() : hEnv(SQL_NULL_HENV), hDbc(SQL_NULL_HDBC) {
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
}

DatabaseManager::~DatabaseManager() {
    Disconnect();
    if (hEnv != SQL_NULL_HENV) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

bool DatabaseManager::Connect(const std::wstring& connectionString) {
    Disconnect();
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

    SQLWCHAR outConnStr[1024];
    SQLSMALLINT outConnStrLen;

    SQLRETURN retcode = SQLDriverConnectW(hDbc, NULL, (SQLWCHAR*)connectionString.c_str(), SQL_NTS,
        outConnStr, 1024, &outConnStrLen, SQL_DRIVER_NOPROMPT);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        std::wcout << L"[УСПЕХ] Подключено к БД!\n";
        return true;
    }
    else {
        std::wcerr << L"[ОШИБКА] Сбой подключения.\n";

        SQLWCHAR sqlState[6], message[256];
        SQLINTEGER nativeError;
        SQLSMALLINT textLength;
        SQLGetDiagRecW(SQL_HANDLE_DBC, hDbc, 1, sqlState, &nativeError, message, sizeof(message) / sizeof(SQLWCHAR), &textLength);

        std::wcerr << L"Состояние: " << sqlState << L"\nСообщение: " << message << L"\n";

        FreeHandles();
        return false;
    }
}

void DatabaseManager::Disconnect() {
    if (hDbc != SQL_NULL_HDBC) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        hDbc = SQL_NULL_HDBC;
        std::wcout << L"[ИНФО] Соединение закрыто.\n";
    }
}

bool DatabaseManager::Ping() {
    if (hDbc == SQL_NULL_HDBC) return false;
    SQLCHAR dbName[256];
    SQLINTEGER stringLength;
    SQLRETURN retcode = SQLGetConnectAttrA(hDbc, SQL_ATTR_CURRENT_CATALOG, dbName, sizeof(dbName), &stringLength);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        std::wcout << L"[ПИНГ] Сервер отвечает. Текущая БД: " << dbName << L"\n";
        return true;
    }
    else {
        SQLCHAR sqlState[6], message[256];
        SQLINTEGER nativeError;
        SQLSMALLINT textLength;

        SQLGetDiagRecA(SQL_HANDLE_DBC, hDbc, 1, sqlState, &nativeError, message, sizeof(message), &textLength);

        std::cerr << "[ОШИБКА] Пинг не прошел. Код: " << sqlState << ", Сообщение: " << message << "\n";
        return false;
    }
}

void DatabaseManager::FreeHandles() {
    if (hDbc != SQL_NULL_HDBC) {
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        hDbc = SQL_NULL_HDBC;
    }
}
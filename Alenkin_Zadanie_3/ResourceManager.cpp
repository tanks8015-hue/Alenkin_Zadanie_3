#include "ResourceManager.h"

ResourceManager::ResourceManager(SQLHDBC connectionHandle) {
    hDbc = connectionHandle;
}

bool ResourceManager::IsValidName(const std::wstring& name) {
    std::wregex illegalChars(L"[\\\\/:\\*]");
    if (std::regex_search(name, illegalChars)) {
        std::wcerr << L"[ОШИБКА] Имя файла содержит запрещенные символы!\n";
        return false;
    }
    return true;
}

bool ResourceManager::IsValidExtension(const std::wstring& name) {
    size_t dotPos = name.find_last_of(L".");
    if (dotPos == std::wstring::npos) return false;

    std::wstring ext = name.substr(dotPos);
    if (ext != L".txt" && ext != L".pdf" && ext != L".exe" && ext != L".jpg") {
        std::wcerr << L"[ОШИБКА] Расширение " << ext << L" запрещено!\n";
        return false;
    }
    return true;
}

bool ResourceManager::AddResource(const std::wstring& name, SQLINTEGER size, SQLINTEGER categoryId, SQLINTEGER ownerId) {
    if (!IsValidName(name) || !IsValidExtension(name)) {
        return false;
    }

    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    // Параметризованный запрос по ТЗ
    std::wstring query = L"INSERT INTO Resources (Name, Size, CategoryID, OwnerID) VALUES (?, ?, ?, ?)";
    SQLPrepareW(hStmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    // Биндим параметры
    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 255, 0, (SQLPOINTER)name.c_str(), name.length() * sizeof(wchar_t), NULL);
    SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &size, 0, NULL);
    SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &categoryId, 0, NULL);
    SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &ownerId, 0, NULL);

    SQLRETURN retcode = SQLExecute(hStmt);

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        std::wcout << L"[УСПЕХ] Файл добавлен в базу!\n";
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return true;
    }
    else {
        std::wcerr << L"[ОШИБКА SQL] Не удалось добавить файл.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }
}
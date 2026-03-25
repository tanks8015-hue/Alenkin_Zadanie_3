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

bool ResourceManager::IsDuplicate(const std::wstring& name) {
    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    std::wstring query = L"SELECT COUNT(*) FROM Resources WHERE Name = ?";
    SQLPrepareW(hStmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 255, 0, (SQLPOINTER)name.c_str(), name.length() * sizeof(wchar_t), NULL);

    SQLRETURN retcode = SQLExecute(hStmt);
    bool isDup = false;

    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        SQLINTEGER count = 0;
        SQLLEN cbCount = 0;
        SQLBindCol(hStmt, 1, SQL_C_SLONG, &count, sizeof(count), &cbCount);
        if (SQLFetch(hStmt) == SQL_SUCCESS && count > 0) {
            isDup = true;
            std::wcerr << L"[ОШИБКА] Файл с именем '" << name << L"' уже существует в базе!\n";
        }
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return isDup;
}
std::wstring ResourceManager::TruncateString(const std::wstring& str, size_t maxLength) {
    if (str.length() > maxLength) {
        return str.substr(0, maxLength) + L"...";
    }
    return str;
}
void ResourceManager::ShowResourcesPaged() {
    int offset = 0;
    int fetchSize = 10;
    bool keepPaging = true;

    while (keepPaging) {
        SQLHSTMT hStmt;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

        std::wstring query =
            L"SELECT r.ResourceID, r.Name, r.Size, u.UserName "
            L"FROM Resources r "
            L"JOIN Users u ON r.OwnerID = u.UserID "
            L"WHERE r.isDeleted = 0 "
            L"ORDER BY r.ResourceID "
            L"OFFSET ? ROWS FETCH NEXT ? ROWS ONLY";

        SQLPrepareW(hStmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
        SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &offset, 0, NULL);
        SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &fetchSize, 0, NULL);

        SQLRETURN retcode = SQLExecute(hStmt);

        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            std::wcout << L"\n--- Страница файлов (Смещение: " << offset << L") ---\n";
            std::wcout << std::left << std::setw(5) << L"ID"
                << std::setw(22) << L"Имя файла"
                << std::setw(10) << L"Размер"
                << std::setw(22) << L"Владелец" << L"\n";
            std::wcout << L"------------------------------------------------------------\n";

            SQLINTEGER id, size;
            SQLWCHAR name[256], owner[256];
            SQLLEN cbId, cbName, cbSize, cbOwner;

            SQLBindCol(hStmt, 1, SQL_C_SLONG, &id, sizeof(id), &cbId);
            SQLBindCol(hStmt, 2, SQL_C_WCHAR, name, sizeof(name), &cbName);
            SQLBindCol(hStmt, 3, SQL_C_SLONG, &size, sizeof(size), &cbSize);
            SQLBindCol(hStmt, 4, SQL_C_WCHAR, owner, sizeof(owner), &cbOwner);

            int rowCount = 0;
            while (SQLFetch(hStmt) == SQL_SUCCESS) {
                rowCount++;
                std::wstring truncatedName = TruncateString(name, 15);
                std::wstring truncatedOwner = TruncateString(owner, 15);

                std::wcout << std::left << std::setw(5) << id
                    << std::setw(22) << truncatedName
                    << std::setw(10) << size
                    << std::setw(22) << truncatedOwner << L"\n";
            }

            if (rowCount == 0 && offset > 0) {
                std::wcout << L"[ИНФО] Дальше записей нет.\n";
                offset -= fetchSize;
            }
            else if (rowCount == 0 && offset == 0) {
                std::wcout << L"[ИНФО] База данных пока пуста.\n";
            }
        }
        else {
            std::wcerr << L"[ОШИБКА SQL] Не удалось загрузить список файлов.\n";
        }
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        std::wcout << L"\n[Д]алее | [Н]азад | [В]ыход в главное меню: ";
        wchar_t choice;
        std::wcin >> choice;
        std::wcin.ignore((std::numeric_limits<std::streamsize>::max)(), L'\n');

        if (choice == L'Д' || choice == L'д' || choice == L'D' || choice == L'd') {
            offset += fetchSize;
        }
        else if (choice == L'Н' || choice == L'н' || choice == L'N' || choice == L'n') {
            if (offset >= fetchSize) offset -= fetchSize;
            else std::wcout << L"[ИНФО] Вы находитесь на первой странице!\n";
        }
        else if (choice == L'В' || choice == L'в' || choice == L'Q' || choice == L'q') {
            keepPaging = false;
        }
    }
}

bool ResourceManager::AddResource(const std::wstring& name, SQLINTEGER size, SQLINTEGER categoryId, SQLINTEGER ownerId) {
    if (!IsValidName(name) || !IsValidExtension(name)) return false;
    if (IsDuplicate(name)) return false;

    SQLHSTMT hStmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    std::wstring query = L"INSERT INTO Resources (Name, Size, CategoryID, OwnerID) VALUES (?, ?, ?, ?)";
    SQLPrepareW(hStmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
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
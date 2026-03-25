#include <iostream>
#include <string>
#include <limits>
#include <windows.h>
#include "DatabaseManager.h"
#include "ResourceManager.h"
#define NOMINMAX
void SetColor(int text, int background = 0) {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}
int GetIntInput(const std::wstring& prompt) {
    int value;
    while (true) {
        std::wcout << prompt;
        if (std::wcin >> value) {
            std::wcin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            return value;
        }
        else {
            SetColor(4);
            std::wcout << L"[ОШИБКА] Введено не число! Пожалуйста, повторите ввод.\n";
            SetColor(7); 

            std::wcin.clear(); 
            std::wcin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n'); // Очищаем мусор из буфера
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    DatabaseManager db;
    std::wstring connStr = L"Driver={ODBC Driver 17 for SQL Server};Server=DESKTOP-PKN6175\\SQLEXPRESS;Database=ByteKeeperDB;Trusted_Connection=yes;";

    SetColor(10);
    std::wcout << L"=== Подключение к базе данных ===\n";
    SetColor(7);

    if (!db.Connect(connStr)) {
        std::wcout << L"Нажмите Enter для выхода...\n";
        std::wcin.get();
        return 1;
    }

    ResourceManager resManager(db.GetConnectionHandle());
    int choice = 0;
    while (choice != 9) {
        SetColor(11);
        std::wcout << L"\n--- ByteKeeper: Управление активами ---\n";
        SetColor(7);
        std::wcout << L"1. Добавить новый файл\n";
        std::wcout << L"2. Пинг сервера (Проверка связи)\n";
        std::wcout << L"3. Показать список файлов (Постранично)\n";
        std::wcout << L"4. Статистика базы данных (COUNT/SUM)\n";
        std::wcout << L"5. Поиск файла по имени (LIKE)\n";
        std::wcout << L"6. Удалить файл в корзину (Soft Delete)\n";
        std::wcout << L"7. Восстановить файл из корзины\n";
        std::wcout << L"9. Выход\n";

        choice = GetIntInput(L"Выберите действие: ");

        switch (choice) {
        case 1: {
            std::wstring name;
            std::wcout << L"Введите имя файла (с расширением, напр. report.pdf): ";
            std::getline(std::wcin, name);

            int size = GetIntInput(L"Введите размер файла (в байтах): ");
            int catId = GetIntInput(L"Введите ID категории (например, 1): ");
            int ownerId = GetIntInput(L"Введите ID владельца (например, 1): ");

            SetColor(14);
            std::wcout << L"Добавление в базу...\n";
            SetColor(7);

            if (resManager.AddResource(name, size, catId, ownerId)) {
                SetColor(10);
                std::wcout << L"Операция успешно завершена!\n";
            }
            else {
                SetColor(4);
                std::wcout << L"Операция прервана из-за ошибки.\n";
            }
            SetColor(7);
            break;
        }
        case 2:
            db.Ping();
            break;
        case 3:
            resManager.ShowResourcesPaged(); 
            break;
        case 4:
            resManager.ShowStatistics();
            break;
        case 5: {
            std::wstring searchWord;
            std::wcout << L"Введите часть имени для поиска: ";
            std::getline(std::wcin, searchWord);
            resManager.SearchByName(searchWord);
            break;
        }
        case 6: {
            int delId = GetIntInput(L"Введите ID файла для перемещения в корзину: ");
            resManager.DeleteToTrash(delId);
            break;
        }
        case 7: {
            int resId = GetIntInput(L"Введите ID файла для восстановления: ");
            resManager.RestoreFromTrash(resId);
            break;
        }
        case 9:
            std::wcout << L"Выход из программы...\n";
            break;
        default:
            SetColor(4); 
            std::wcout << L"Неверный пункт меню!\n";
            SetColor(7);
            break;
        }
    }

    db.Disconnect();
    return 0;
}
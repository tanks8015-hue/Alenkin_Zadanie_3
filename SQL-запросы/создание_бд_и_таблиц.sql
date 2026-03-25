-- Создание базы данных для системы управления цифровыми активами
CREATE DATABASE ByteKeeperDB;
GO

USE ByteKeeperDB;
GO

-- Таблица Categories: хранит категории файлов [cite: 6]
CREATE TABLE Categories (
    CategoryID INT IDENTITY(1,1) PRIMARY KEY, -- Уникальный идентификатор категории, автоинкремент [cite: 7]
    CategoryName NVARCHAR(100) NOT NULL       -- Название категории (например: "Документы", "Медиа", "Архивы") [cite: 8]
);
GO

-- Таблица Users: хранит данные сотрудников [cite: 9]
CREATE TABLE Users (
    UserID INT IDENTITY(1,1) PRIMARY KEY, -- Уникальный идентификатор пользователя, автоинкремент [cite: 10]
    UserName NVARCHAR(100) NOT NULL       -- Имя сотрудника, загрузившего файл [cite: 11]
);
GO

-- Таблица Resources: главная таблица для хранения информации о файлах [cite: 12]
CREATE TABLE Resources (
    ResourceID INT IDENTITY(1,1) PRIMARY KEY, -- Уникальный идентификатор ресурса, автоинкремент [cite: 13]
    Name NVARCHAR(255) NOT NULL,              -- Имя файла [cite: 14]
    Size BIGINT NOT NULL,                     -- Размер файла в байтах [cite: 15]
    CategoryID INT NOT NULL,                  -- Внешний ключ: ссылка на категорию [cite: 16]
    OwnerID INT NOT NULL,                     -- Внешний ключ: ссылка на владельца (пользователя) [cite: 17]
    isDeleted BIT DEFAULT 0,                  -- Флаг для корзины (Soft Delete): 0 - активен, 1 - удален [cite: 23, 24]
    CreationDate DATETIME DEFAULT GETDATE(),  -- Дата загрузки ресурса (для функции очистки старых данных) 

    -- Ограничения внешних ключей (Foreign Keys). 
    -- По умолчанию используется ON DELETE NO ACTION. Это значит, что БД не даст удалить 
    -- категорию или пользователя, если к ним уже привязан хотя бы один файл[cite: 48].
    CONSTRAINT FK_Resources_Categories FOREIGN KEY (CategoryID) REFERENCES Categories(CategoryID),
    CONSTRAINT FK_Resources_Users FOREIGN KEY (OwnerID) REFERENCES Users(UserID)
);
GO

-- Таблица Logs: для логирования всех действий в системе [cite: 25]
CREATE TABLE Logs (
    LogID INT IDENTITY(1,1) PRIMARY KEY,      -- Уникальный идентификатор записи лога
    ActionDate DATETIME DEFAULT GETDATE(),    -- Дата и время совершения действия
    ActionDescription NVARCHAR(500) NOT NULL  -- Текстовое описание (что было добавлено или удалено) [cite: 25]
);
GO
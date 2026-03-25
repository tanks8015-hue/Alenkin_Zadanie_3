USE ByteKeeperDB;
GO

-- Добавляем три базовые категории из задания
INSERT INTO Categories (CategoryName) VALUES (N'Документы'), (N'Медиа'), (N'Архивы');
GO

-- Добавляем пару тестовых пользователей
INSERT INTO Users (UserName) VALUES (N'Иван Иванов'), (N'Алексей Попов');
GO
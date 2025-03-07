#include "utils/Database.h"
#include <iostream>
#include <stdexcept>

std::string Database::dbPath;
std::shared_ptr<sqlite3> Database::connection;

void Database::init(const std::string &dbPath) {
    Database::dbPath = dbPath;
    
    // 创建数据库连接
    sqlite3 *db;
    int rc = sqlite3_open(dbPath.c_str(), &db);
    
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error("Cannot open database: " + error);
    }
    
    // 使用shared_ptr管理连接，自动关闭
    connection = std::shared_ptr<sqlite3>(db, closeDatabase);
    
    // 启用外键约束
    execute(connection, "PRAGMA foreign_keys = ON");
    
    // 初始化表结构
    initializeTables();
    
    std::cout << "Database initialized: " << dbPath << std::endl;
}

std::shared_ptr<sqlite3> Database::getConnection() {
    if (!connection) {
        throw std::runtime_error("Database not initialized. Call Database::init() first.");
    }
    return connection;
}

bool Database::execute(std::shared_ptr<sqlite3> db, const std::string &sql) {
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db.get(), sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::string error = errMsg ? errMsg : "Unknown error";
        std::cerr << "SQL error: " << error << std::endl;
        std::cerr << "Failed query: " << sql << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

void Database::initializeTables() {
    // 创建用户表
    std::string createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id TEXT PRIMARY KEY,
            username TEXT NOT NULL,
            email TEXT NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            credits INTEGER DEFAULT 100,
            created_at TEXT NOT NULL,
            last_login TEXT
        );
        
        CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
    )";
    
    // 创建对话表
    std::string createConversationsTable = R"(
        CREATE TABLE IF NOT EXISTS conversations (
            id TEXT PRIMARY KEY,
            user_id TEXT NOT NULL,
            title TEXT NOT NULL,
            created_at TEXT NOT NULL,
            model_id TEXT NOT NULL,
            FOREIGN KEY (user_id) REFERENCES users(id)
        );
        
        CREATE INDEX IF NOT EXISTS idx_conversations_user_id ON conversations(user_id);
    )";
    
    // 创建消息表
    std::string createMessagesTable = R"(
        CREATE TABLE IF NOT EXISTS messages (
            id TEXT PRIMARY KEY,
            conversation_id TEXT NOT NULL,
            role TEXT NOT NULL,
            content TEXT NOT NULL,
            model_id TEXT NOT NULL,
            timestamp TEXT NOT NULL,
            FOREIGN KEY (conversation_id) REFERENCES conversations(id)
        );
        
        CREATE INDEX IF NOT EXISTS idx_messages_conversation_id ON messages(conversation_id);
        CREATE INDEX IF NOT EXISTS idx_messages_timestamp ON messages(timestamp);
    )";
    
    if (!execute(connection, createUsersTable)) {
        throw std::runtime_error("Failed to create users table");
    }
    
    if (!execute(connection, createConversationsTable)) {
        throw std::runtime_error("Failed to create conversations table");
    }
    
    if (!execute(connection, createMessagesTable)) {
        throw std::runtime_error("Failed to create messages table");
    }
    
    std::cout << "Database tables initialized" << std::endl;
}

void Database::closeDatabase(sqlite3 *db) {
    if (db) {
        sqlite3_close(db);
    }
}

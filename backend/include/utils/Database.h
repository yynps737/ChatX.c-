#pragma once

#include <string>
#include <memory>
#include <sqlite3.h>

class Database {
public:
    // 初始化数据库
    static void init(const std::string &dbPath);
    
    // 获取数据库连接
    static std::shared_ptr<sqlite3> getConnection();
    
    // 执行SQL
    static bool execute(std::shared_ptr<sqlite3> db, const std::string &sql);
    
    // 初始化表结构
    static void initializeTables();
    
private:
    static std::string dbPath;
    static std::shared_ptr<sqlite3> connection;
    
    // 关闭回调函数(用于shared_ptr)
    static void closeDatabase(sqlite3 *db);
};

// 新增 include/models/User.h
#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <sqlite3.h>
#include <ctime>

class User {
public:
    std::string id;
    std::string username;
    std::string email;
    std::string passwordHash;
    int credits;
    std::time_t createdAt;
    std::time_t lastLogin;
    
    User();
    
    // 保存用户
    bool save(std::shared_ptr<sqlite3> db);
    
    // 加载用户
    bool load(std::shared_ptr<sqlite3> db, const std::string &id);
    
    // 根据邮箱查找用户
    static std::optional<User> findByEmail(std::shared_ptr<sqlite3> db, const std::string &email);
    
    // 更新用户信息
    bool update(std::shared_ptr<sqlite3> db);
    
    // 扣除积分
    bool deductCredits(std::shared_ptr<sqlite3> db, int amount = 1);
};
// 新增 include/handlers/AuthHandler.h
#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <uWebSockets/App.h>
#include "models/User.h"

using json = nlohmann::json;

class AuthHandler {
public:
    AuthHandler();
    
    // 用户登录
    void login(uWS::HttpResponse<false> *res, const std::string &body);
    
    // 用户注册
    void registerUser(uWS::HttpResponse<false> *res, const std::string &body);
    
    // 验证JWT令牌
    bool verifyToken(const std::string &authHeader);
    
    // 从令牌中获取用户ID
    std::string getUserIdFromToken(const std::string &authHeader);
    
private:
    // 生成JWT令牌
    std::string generateToken(const User &user);
    
    // 哈希密码
    std::string hashPassword(const std::string &password);
    
    // 验证密码
    bool verifyPassword(const std::string &password, const std::string &hashedPassword);
    
    std::string jwtSecret;
};
// 优化后的main.cpp
#include <iostream>
#include "App.h"
#include "handlers/AuthHandler.h"
#include "handlers/ChatHandler.h"
#include "handlers/WSController.h"
#include "utils/Database.h"
#include "utils/Config.h"

int main() {
    std::cout << "ChatX Backend Starting..." << std::endl;
    
    // 加载环境变量
    Config::load(".env");
    
    // 确保数据目录存在
    std::string dbPath = Config::get("DB_PATH", "./data/chatx.db");
    std::filesystem::path dirPath(dbPath);
    if (!std::filesystem::exists(dirPath.parent_path())) {
        std::filesystem::create_directories(dirPath.parent_path());
    }
    
    // 初始化数据库
    try {
        Database::init(dbPath);
        std::cout << "Database initialized successfully." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error initializing database: " << e.what() << std::endl;
        return 1;
    }
    
    // 创建处理器实例
    AuthHandler authHandler;
    ChatHandler chatHandler;
    WSController wsController;
    
    // 获取配置
    int port = std::stoi(Config::get("PORT", "9001"));
    std::string host = Config::get("HOST", "0.0.0.0");
    
    // 创建uWebSockets应用
    uWS::App().get("/health", [](auto *res, auto *req) {
        // 健康检查端点
        res->writeHeader("Content-Type", "application/json");
        res->end("{\"status\":\"ok\",\"timestamp\":\"" + std::to_string(std::time(nullptr)) + "\"}");
    })
    
    // 用户认证API
    .post("/api/auth/register", [&authHandler](auto *res, auto *req) {
        // 获取请求体
        std::string buffer;
        res->onData([&authHandler, res, buffer = std::move(buffer)](std::string_view data, bool last) mutable {
            buffer.append(data.data(), data.length());
            if (last) {
                authHandler.registerUser(res, buffer);
            }
        });
        res->onAborted([]() {});
    })
    
    .post("/api/auth/login", [&authHandler](auto *res, auto *req) {
        std::string buffer;
        res->onData([&authHandler, res, buffer = std::move(buffer)](std::string_view data, bool last) mutable {
            buffer.append(data.data(), data.length());
            if (last) {
                authHandler.login(res, buffer);
            }
        });
        res->onAborted([]() {});
    })
    
    // 对话API
    .get("/api/conversations", [&chatHandler](auto *res, auto *req) {
        std::string auth = req->getHeader("authorization");
        chatHandler.getConversations(res, auth);
    })
    
    .post("/api/conversations", [&chatHandler](auto *res, auto *req) {
        std::string auth = req->getHeader("authorization");
        std::string buffer;
        res->onData([&chatHandler, res, auth, buffer = std::move(buffer)](std::string_view data, bool last) mutable {
            buffer.append(data.data(), data.length());
            if (last) {
                chatHandler.createConversation(res, buffer, auth);
            }
        });
        res->onAborted([]() {});
    })
    
    // 消息API
    .post("/api/chat", [&chatHandler](auto *res, auto *req) {
        std::string auth = req->getHeader("authorization");
        std::string buffer;
        res->onData([&chatHandler, res, auth, buffer = std::move(buffer)](std::string_view data, bool last) mutable {
            buffer.append(data.data(), data.length());
            if (last) {
                chatHandler.processChat(res, buffer, auth);
            }
        });
        res->onAborted([]() {});
    })
    
    // WebSocket处理
    .ws("/ws", {
        .open = [&wsController](auto *ws) {
            wsController.onConnection(ws);
        },
        .message = [&wsController](auto *ws, std::string_view message, uWS::OpCode opCode) {
            wsController.onMessage(ws, message, opCode);
        },
        .close = [&wsController](auto *ws, int code, std::string_view message) {
            wsController.onDisconnection(ws, code, message);
        }
    })
    
    // 静态文件服务
    .get("/*", [](auto *res, auto *req) {
        std::string path = std::string(req->getUrl());
        if (path == "/") {
            path = "/index.html";
        }
        
        std::string filePath = "./public" + path;
        
        // 检查文件是否存在
        if (std::filesystem::exists(filePath) && !std::filesystem::is_directory(filePath)) {
            // 读取文件内容
            std::ifstream file(filePath, std::ios::binary);
            if (file) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string content = buffer.str();
                
                // 设置内容类型
                std::string ext = std::filesystem::path(filePath).extension().string();
                std::string contentType = "text/plain";
                
                if (ext == ".html") contentType = "text/html";
                else if (ext == ".css") contentType = "text/css";
                else if (ext == ".js") contentType = "application/javascript";
                else if (ext == ".json") contentType = "application/json";
                else if (ext == ".png") contentType = "image/png";
                else if (ext == ".jpg" || ext == ".jpeg") contentType = "image/jpeg";
                else if (ext == ".svg") contentType = "image/svg+xml";
                
                res->writeHeader("Content-Type", contentType);
                res->end(content);
            } else {
                res->writeStatus("404 Not Found");
                res->end("File not found");
            }
        } else {
            res->writeStatus("404 Not Found");
            res->end("Not Found");
        }
    })
    
    .listen(host, port, [port, host](auto *listen_socket) {
        if (listen_socket) {
            std::cout << "Server listening on " << host << ":" << port << std::endl;
        } else {
            std::cout << "Failed to listen on " << host << ":" << port << std::endl;
        }
    }).run();
    
    return 0;
}
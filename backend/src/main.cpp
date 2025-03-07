#include <iostream>
#include <string>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "utils/Database.h"

// 创建数据目录（如果不存在）
void ensureDataDirectoryExists(const std::string &path) {
    std::filesystem::path dirPath(path);
    if (!std::filesystem::exists(dirPath.parent_path())) {
        std::cout << "Creating directory: " << dirPath.parent_path().string() << std::endl;
        std::filesystem::create_directories(dirPath.parent_path());
    }
}

// 读取静态文件
std::string readFile(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 获取内容类型
std::string getContentType(const std::string &path) {
    std::string ext = std::filesystem::path(path).extension().string();
    if (ext == ".html") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".json") return "application/json";
    if (ext == ".png") return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".svg") return "image/svg+xml";
    return "application/octet-stream";
}

int main() {
    std::cout << "ChatX Backend Starting..." << std::endl;
    
    // 确保数据目录存在
    const std::string dbPath = "./data/chatx.db";
    ensureDataDirectoryExists(dbPath);
    
    // 初始化数据库
    try {
        Database::init(dbPath);
        std::cout << "Database initialized successfully." << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error initializing database: " << e.what() << std::endl;
        return 1;
    }
    
    // 创建套接字
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }
    
    // 设置套接字选项
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "Failed to set socket options" << std::endl;
        close(server_fd);
        return 1;
    }
    
    // 设置服务器地址
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(9001);
    
    // 绑定套接字
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind to port 9001" << std::endl;
        close(server_fd);
        return 1;
    }
    
    // 监听连接
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Failed to listen on port 9001" << std::endl;
        close(server_fd);
        return 1;
    }
    
    std::cout << "Server listening on port 9001" << std::endl;
    
    // 接受连接并处理请求
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }
        
        // 读取请求
        char buffer[4096] = {0};
        read(client_fd, buffer, 4096);
        
        // 解析请求行
        std::string request(buffer);
        size_t firstLineEnd = request.find("\r\n");
        if (firstLineEnd == std::string::npos) {
            close(client_fd);
            continue;
        }
        
        std::string firstLine = request.substr(0, firstLineEnd);
        size_t methodEnd = firstLine.find(" ");
        size_t pathEnd = firstLine.find(" ", methodEnd + 1);
        
        std::string method = firstLine.substr(0, methodEnd);
        std::string path = firstLine.substr(methodEnd + 1, pathEnd - methodEnd - 1);
        
        std::cout << "Request: " << method << " " << path << std::endl;
        
        // 处理请求
        if (method == "GET") {
            if (path == "/health") {
                // 健康检查端点
                std::string timestamp = std::to_string(std::time(nullptr));
                std::string response = 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: application/json\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "{\"status\":\"ok\",\"timestamp\":\"" + timestamp + "\"}";
                
                send(client_fd, response.c_str(), response.size(), 0);
            } else {
                // 处理静态文件
                std::string filePath;
                if (path == "/") {
                    filePath = "./public/index.html";
                } else {
                    filePath = "./public" + path;
                }
                
                if (std::filesystem::exists(filePath) && !std::filesystem::is_directory(filePath)) {
                    std::string content = readFile(filePath);
                    std::string contentType = getContentType(filePath);
                    
                    std::string response = 
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: " + contentType + "\r\n"
                        "Content-Length: " + std::to_string(content.size()) + "\r\n"
                        "Connection: close\r\n"
                        "\r\n" + content;
                    
                    send(client_fd, response.c_str(), response.size(), 0);
                } else {
                    // 404 响应
                    std::string response = 
                        "HTTP/1.1 404 Not Found\r\n"
                        "Content-Type: text/plain\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                        "Not Found";
                    
                    send(client_fd, response.c_str(), response.size(), 0);
                }
            }
        } else {
            // 不支持的方法
            std::string response = 
                "HTTP/1.1 405 Method Not Allowed\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Method Not Allowed";
            
            send(client_fd, response.c_str(), response.size(), 0);
        }
        
        close(client_fd);
    }
    
    close(server_fd);
    return 0;
}

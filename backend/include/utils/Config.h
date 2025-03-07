// 新增 include/utils/Config.h
#pragma once

#include <string>
#include <unordered_map>
#include <fstream>

class Config {
public:
    static void load(const std::string &path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // 跳过注释和空行
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // 解析KEY=VALUE
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // 检查环境变量中是否已存在该变量
                char* envVal = getenv(key.c_str());
                if (envVal == nullptr) {
                    // 如果环境变量不存在，则设置到内部映射
                    configMap[key] = value;
                }
            }
        }
    }
    
    static std::string get(const std::string &key, const std::string &defaultValue = "") {
        // 首先检查系统环境变量
        char* envVal = getenv(key.c_str());
        if (envVal != nullptr) {
            return envVal;
        }
        
        // 然后检查内部映射
        auto it = configMap.find(key);
        if (it != configMap.end()) {
            return it->second;
        }
        
        return defaultValue;
    }
    
private:
    static inline std::unordered_map<std::string, std::string> configMap;
};
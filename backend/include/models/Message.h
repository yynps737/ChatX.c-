// 新增 include/models/Message.h
#pragma once

#include <string>
#include <memory>
#include <sqlite3.h>
#include <ctime>

class Message {
public:
    std::string id;
    std::string conversationId;
    std::string role;  // "user" 或 "assistant"
    std::string content;
    std::string modelId;
    std::time_t timestamp;
    
    Message();
    
    // 保存消息
    bool save(std::shared_ptr<sqlite3> db);
    
    // 加载消息
    bool load(std::shared_ptr<sqlite3> db, const std::string &id);
    
    // 根据对话ID查找消息
    static std::vector<Message> findByConversationId(std::shared_ptr<sqlite3> db, const std::string &conversationId);
};
// 新增 include/models/Conversation.h
#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <sqlite3.h>
#include <ctime>
#include "models/Message.h"

class Conversation {
public:
    std::string id;
    std::string userId;
    std::string title;
    std::time_t createdAt;
    std::string modelId;
    
    // 消息列表(非持久化字段)
    std::vector<Message> messages;
    
    Conversation();
    
    // 保存对话
    bool save(std::shared_ptr<sqlite3> db);
    
    // 加载对话
    bool load(std::shared_ptr<sqlite3> db, const std::string &id);
    
    // 加载消息
    bool loadMessages(std::shared_ptr<sqlite3> db);
    
    // 添加消息
    bool addMessage(std::shared_ptr<sqlite3> db, const Message &message);
    
    // 根据用户ID查找对话
    static std::vector<Conversation> findByUserId(std::shared_ptr<sqlite3> db, const std::string &userId, bool withMessages = false);
    
    // 更新对话标题
    bool updateTitle(std::shared_ptr<sqlite3> db, const std::string &newTitle);
    
    // 删除对话及其消息
    bool deleteWithMessages(std::shared_ptr<sqlite3> db);
};
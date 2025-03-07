// 新增 include/handlers/ChatHandler.h
#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <uWebSockets/App.h>
#include "models/Conversation.h"
#include "models/Message.h"
#include "handlers/AuthHandler.h"

using json = nlohmann::json;

class ChatHandler {
public:
    ChatHandler();
    
    // 获取用户所有对话
    void getConversations(uWS::HttpResponse<false> *res, const std::string &authHeader);
    
    // 创建新对话
    void createConversation(uWS::HttpResponse<false> *res, const std::string &body, const std::string &authHeader);
    
    // 处理聊天请求
    void processChat(uWS::HttpResponse<false> *res, const std::string &body, const std::string &authHeader);
    
private:
    // 调用AI模型API
    json callModelAPI(const std::string &modelName, const json &messages);
    
    // 存储消息到数据库
    void saveMessages(const std::string &userId, const std::string &conversationId, 
                     const std::string &modelId, const json &userMessage, 
                     const json &aiResponse);
    
    AuthHandler authHandler;
};
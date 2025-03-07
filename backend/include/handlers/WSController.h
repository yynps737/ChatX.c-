// 新增 include/handlers/WSController.h
#pragma once

#include <string>
#include <unordered_map>
#include <uWebSockets/App.h>
#include "handlers/AuthHandler.h"
#include "handlers/ChatHandler.h"

class WSController {
public:
    WSController();
    
    // WebSocket事件处理
    void onConnection(uWS::WebSocket<false, true> *ws);
    void onMessage(uWS::WebSocket<false, true> *ws, std::string_view message, uWS::OpCode opCode);
    void onDisconnection(uWS::WebSocket<false, true> *ws, int code, std::string_view message);
    
private:
    // 用户会话管理
    struct Session {
        std::string userId;
        std::string currentConversationId;
    };
    
    std::unordered_map<uWS::WebSocket<false, true> *, Session> sessions;
    
    // 处理认证消息
    void handleAuthentication(uWS::WebSocket<false, true> *ws, const std::string &message);
    
    // 处理聊天消息
    void handleChatMessage(uWS::WebSocket<false, true> *ws, const std::string &message);
    
    AuthHandler authHandler;
    ChatHandler chatHandler;
};
<!-- frontend/src/App.svelte -->
<script>
  import { onMount } from 'svelte';
  import Sidebar from './components/Sidebar.svelte';
  import ChatArea from './components/ChatArea.svelte';
  import { user, isAuthenticated } from './stores/user';
  import { conversations, activeConversationId } from './stores/conversations';
  import { isSidebarOpen, isMobileMenuOpen } from './stores/ui';
  import Login from './components/Login.svelte';
  import { initWebSocket } from './services/websocket';
  
  let loading = true;
  
  onMount(async () => {
    // 检查本地存储的认证状态
    const storedToken = localStorage.getItem('auth_token');
    const storedUser = localStorage.getItem('user');
    
    if (storedToken && storedUser) {
      $user = JSON.parse(storedUser);
      $isAuthenticated = true;
      
      // 初始化WebSocket连接
      initWebSocket();
      
      // 加载对话列表
      try {
        const response = await fetch('/api/conversations', {
          headers: {
            'Authorization': `Bearer ${storedToken}`
          }
        });
        
        if (response.ok) {
          const data = await response.json();
          if (data.status === 'success') {
            $conversations = data.data.conversations;
            
            if ($conversations.length > 0) {
              $activeConversationId = $conversations[0].id;
            }
          }
        }
      } catch (error) {
        console.error('Failed to load conversations:', error);
      }
    }
    
    loading = false;
  });
  
  async function handleCreateConversation() {
    const token = localStorage.getItem('auth_token');
    
    try {
      const response = await fetch('/api/conversations', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${token}`
        },
        body: JSON.stringify({
          title: '新对话',
          model_id: 'deepseek-chat'
        })
      });
      
      if (response.ok) {
        const data = await response.json();
        if (data.status === 'success') {
          $conversations = [$conversations, data.data.conversation];
          $activeConversationId = data.data.conversation.id;
          
          if ($isMobileMenuOpen) $isMobileMenuOpen = false;
        }
      }
    } catch (error) {
      console.error('Failed to create conversation:', error);
    }
  }
</script>

<main class="h-screen flex overflow-hidden bg-white">
  {#if loading}
    <div class="flex items-center justify-center w-full h-screen">
      <div class="animate-spin rounded-full h-12 w-12 border-t-2 border-b-2 border-blue-500"></div>
    </div>
  {:else if !$isAuthenticated}
    <Login />
  {:else}
    <!-- 移动设备遮罩层 -->
    {#if $isMobileMenuOpen}
      <div 
        class="fixed inset-0 z-20 bg-black bg-opacity-50 md:hidden" 
        on:click={() => $isMobileMenuOpen = false}
      ></div>
    {/if}
    
    <!-- 侧边栏 -->
    <Sidebar 
      isOpen={$isSidebarOpen} 
      isMobileOpen={$isMobileMenuOpen}
      onCreateConversation={handleCreateConversation}
    />
    
    <!-- 聊天区域 -->
    <ChatArea 
      conversation={$conversations.find(conv => conv.id === $activeConversationId)}
      isSidebarOpen={$isSidebarOpen}
    />
  {/if}
</main>

<style>
  :global(body) {
    margin: 0;
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen,
      Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;
  }
</style>
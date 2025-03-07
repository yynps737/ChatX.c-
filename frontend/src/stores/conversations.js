// frontend/src/stores/conversations.js
import { writable, derived } from 'svelte/store';

export const conversations = writable([]);
export const activeConversationId = writable(null);
export const isNewChat = writable(true);

export const activeConversation = derived(
  [conversations, activeConversationId],
  ([$conversations, $activeConversationId]) => {
    return $conversations.find(conv => conv.id === $activeConversationId) || null;
  }
);
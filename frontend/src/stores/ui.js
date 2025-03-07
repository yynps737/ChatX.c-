// frontend/src/stores/ui.js
import { writable } from 'svelte/store';

export const isSidebarOpen = writable(true);
export const isMobileMenuOpen = writable(false);
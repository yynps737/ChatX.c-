import { defineConfig } from 'vite';
import { svelte } from '@sveltejs/vite-plugin-svelte';

export default defineConfig({
  plugins: [svelte()],
  server: {
    port: 5000,
    proxy: {
      '/api': 'http://localhost:9001',
      '/health': 'http://localhost:9001'
    }
  }
});

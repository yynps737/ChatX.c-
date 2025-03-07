<script>
  import { onMount } from 'svelte';
  
  let status = 'Loading...';
  let timestamp = '';
  
  onMount(async () => {
    try {
      const response = await fetch('/health');
      const data = await response.json();
      status = data.status;
      timestamp = new Date(parseInt(data.timestamp) * 1000).toLocaleString();
    } catch (error) {
      status = 'Error';
      timestamp = error.message;
    }
  });
</script>

<main class="min-h-screen bg-gray-100 flex flex-col items-center justify-center p-4">
  <div class="bg-white rounded-lg shadow-md p-8 max-w-md w-full">
    <h1 class="text-3xl font-bold text-center mb-6">ChatX</h1>
    
    <div class="bg-gray-50 p-4 rounded mb-4">
      <h2 class="text-xl font-semibold mb-2">Server Status</h2>
      <p>Status: <span class="font-medium text-blue-600">{status}</span></p>
      <p>Time: <span class="font-medium">{timestamp}</span></p>
    </div>
    
    <p class="text-center text-gray-600">
      A multi-model intelligent chat application
    </p>
  </div>
</main>

<style>
  :global(body) {
    margin: 0;
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen,
      Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;
  }
</style>

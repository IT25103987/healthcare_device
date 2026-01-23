import io from 'socket.io-client';

const WS_URL = import.meta.env.VITE_WS_URL || 'http://localhost:3000';

let socket = null;

export const websocketService = {
  connect: (token) => {
    return new Promise((resolve, reject) => {
      socket = io(WS_URL, {
        auth: {
          token,
        },
        reconnection: true,
        reconnectionDelay: 1000,
        reconnectionDelayMax: 5000,
        reconnectionAttempts: 5,
      });

      socket.on('connect', () => {
        console.log('WebSocket connected');
        resolve(socket);
      });

      socket.on('connect_error', (error) => {
        console.error('WebSocket connection error:', error);
        reject(error);
      });
    });
  },

  disconnect: () => {
    if (socket) {
      socket.disconnect();
      socket = null;
    }
  },

  on: (event, callback) => {
    if (socket) {
      socket.on(event, callback);
    }
  },

  off: (event, callback) => {
    if (socket) {
      socket.off(event, callback);
    }
  },

  emit: (event, data) => {
    if (socket) {
      socket.emit(event, data);
    }
  },

  isConnected: () => {
    return socket && socket.connected;
  },

  subscribe: (channel) => {
    if (socket) {
      socket.emit('subscribe', channel);
    }
  },

  unsubscribe: (channel) => {
    if (socket) {
      socket.emit('unsubscribe', channel);
    }
  },
};

export default websocketService;

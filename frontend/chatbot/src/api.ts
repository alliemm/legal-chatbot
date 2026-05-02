import axios from "axios";

export const API_BASE = "https://legal-chatbot-4t8e.onrender.com";
// export const API_BASE = "https://localhost:18080";


axios.interceptors.request.use((config) => {
  const token = localStorage.getItem("user_token");
  if (token) {
    config.headers.Authorization = token;
  }
  return config;
});

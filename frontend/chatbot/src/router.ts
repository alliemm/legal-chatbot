import { createRouter, createWebHistory } from "vue-router";
import Home from "./pages/Home.vue";
import Login from "./pages/Login.vue";
import Signup from "./pages/Signup.vue";
import Survey from "./pages/Survey.vue";
import Dashboard from "./pages/Dashboard.vue";
import Chatbot from "./pages/Chatbot.vue";
import NotFound from "./pages/NotFound.vue";
import Profile from "./pages/Profile.vue";          

export const router = createRouter({
  history: createWebHistory(),
  routes: [
    { path: "/", component: Home },
    { path: "/login", component: Login },
    { path: "/signup", component: Signup },
    { path: "/survey", component: Survey },
    { path: "/dashboard", component: Dashboard },
    { path: "/chatbot", component: Chatbot },
    { path: "/profile", component: Profile },       
    { path: "/:pathMatch(.*)*", component: NotFound },
  ],
});
import { createRouter, createWebHistory } from 'vue-router'
import Home from '../views/Home.vue'
import Login from '../views/Login.vue'
import Signup from '../views/Signup.vue'
import Profile from '../views/Profile.vue'
import Survey from '../views/Survey.vue'

const routes = [
  { path: '/', component: Home },
  { path: '/login', component: Login },
  { path: '/signup', component: Signup },
  { path: '/profile', component: Profile },
  { path: '/survey', component: Survey }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

export default router

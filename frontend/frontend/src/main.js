import Vue from 'vue'
import App from './App.vue'
import vuetify from './plugins/vuetify'
import 'roboto-fontface/css/roboto/roboto-fontface.css'
import '@fortawesome/fontawesome-free/css/all.css'

Vue.config.productionTip = false


new Vue({
  vuetify,
  render: h => h(App)
}).$mount('#app')

window.apanalogval = [
  [1,1,1],
  [1,1,1]
]
window.staanalogval = [
  [1,1,1],
  [1,1,1]
]
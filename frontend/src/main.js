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
  [1,2,3,4,5,6,7,8,9,10],
  [1,2,3,4,5,6,7,8,9,10],
]
window.staanalogval = [
  [1,2,3,4,5,6,7,8,9,10],
  [1,2,3,4,5,6,7,8,9,10],
]
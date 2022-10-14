<template>
  <v-app>
    <v-app-bar
      app
      color="primary"
      dark
      dense
    >
      <div class="d-flex align-center">
        2022电赛E题 无线监视系统
      </div>

      <v-spacer></v-spacer>
      <v-btn @click="connectToAP">{{(!apconnected)?'连接到AP':'从AP断开'}}</v-btn>
      <v-btn @click="connectToSTA">{{(!staconnected)?'连接到STA':'从STA断开'}}</v-btn>


    </v-app-bar>

    <v-main>
      <v-container fluid>
        <v-row>
          <v-col cols="6">
            <Digital :num="apdigitalval" label="AP数字值"></Digital>
          </v-col>
          <v-col cols="6">
            <Digital :num="stadigitalval" label="STA数字值"></Digital>
          </v-col>
          <v-col cols="6">
            <Analog :val="apanalogval" label="AP模拟值" ref="apanal"></Analog>
          </v-col>
          <v-col cols="6">
            <Analog :val="staanalogval" label="STA模拟值" ref="staanal"></Analog>
          </v-col>
        </v-row>
      </v-container>
      
      
    </v-main>
  </v-app>
</template>

<script>
import Digital from './components/digital.vue';
import Analog from './components/analog.vue';

export default {
  name: 'App',

  components: {
    Digital,
    Analog
},

  data: () => ({
    apconnected: false,
    apws: null,
    staconnected: false,
    staws: null,
    apdigitalval:0,
    stadigitalval:0,
    apanalogval:[],
    staanalogval:[],
  }),

  methods: {
    connectToSTA(){
      console.log("connect to STA");
      if(this.staconnected){
        this.staws.close();
        this.staconnected = false;
      }else{
        this.staws = new WebSocket("ws://192.168.4.255/ws");
        this.staws.onopen = () => {
          this.staconnected = true;
        };
        this.staws.onmessage = (e) => {
          console.log(e.data);
        };
        this.staws.onclose = () => {
          this.staconnected = false;
        };
      }
    },
    connectToAP(){
      console.log("connect to AP");
      if(this.apconnected){
        this.apws.close();
        this.apconnected = false;
      }else{
        this.apws = new WebSocket("ws://192.168.4.1/ws");
        this.apws.onopen = () => {
          this.apconnected = true;
        };
        this.apws.onmessage = (e) => {
          e.data.arrayBuffer().then(buffer => {
            var dv = new DataView(buffer);
            let type = dv.getUint8(0);
            if (type == 0){// digital
              let val = dv.getUint8(1);
              this.apdigitalval = val;
            }
            else if(type == 1){ // analog
              if(this.apanalogval.length == 0){
                this.apanalogval = new Array((buffer.byteLength - 1) / 2)
              }
              for(let i = 0; i < (buffer.byteLength - 1) / 2; i++){
                this.apanalogval[i] = dv.getUint16(1+i*2,false);
              }
              console.log(dv.getUint16(1,false))
              this.$refs.apanal.updateVal();
            } 
          })
        };
        this.apws.onclose = () => {
          this.apconnected = false;
        };
      }
    }
  }
};
</script>

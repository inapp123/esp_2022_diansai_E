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



    </v-app-bar>

    <v-main>
      <v-container fluid>
        <v-row>
          <v-col cols="12">
            <v-card>
              <v-card-actions>
                <v-btn color="primary" @click="ishalfskip = !ishalfskip">{{ishalfskip? '跳过数据' : '完整显示'}}</v-btn>
                <v-btn color="primary" @click="connectToAP">{{(!apconnected)?'连接到AP':'从AP断开'}}</v-btn>
                <v-btn color="primary" @click="connectToSTA">{{(!staconnected)?'连接到STA':'从STA断开'}}</v-btn>
              </v-card-actions>
              

            </v-card>
          </v-col>
        </v-row>
        <v-row>
          <v-col cols="12" sm="12" md="6" lg="6" xl="6">
            <Digital :num="apdigitalval" label="AP数字值"></Digital>
          </v-col>
          <v-col cols="12" sm="12" md="6" lg="6" xl="6">
            <Digital :num="stadigitalval" label="STA数字值"></Digital>
          </v-col>
          <v-col cols="12" sm="12" md="6" lg="6" xl="6">
            <Analog :windowKey="'apanalogval'" label="AP模拟值" ref="apanal"></Analog>
          </v-col>
          <v-col cols="12" sm="12" md="6" lg="6" xl="6">
            <Analog :windowKey="'staanalogval'" label="STA模拟值" ref="staanal"></Analog>
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
    ishalfskip:false,
    staskipvar:0,
    apskipvar:0
  }),

  methods: {
    connectToSTA(){
      console.log("connect to STA");
      if(this.staconnected){
        this.staws.close();
        this.staconnected = false;
      }else{
        this.staws = new WebSocket("ws://192.168.4.250/ws");
        this.staws.onopen = () => {
          this.staconnected = true;
        };
        this.staws.onmessage = (e) => {
          if(this.ishalfskip){
            this.staskipvar++
            if(this.staskipvar > 10){
              this.staskipvar = 0
            }
            else{
              return
            }
          }
          e.data.arrayBuffer().then(buffer => {
            var dv = new DataView(buffer);
            let type = dv.getUint8(0);
            if (type == 0){// digital
              let val = dv.getUint8(1);
              this.stadigitalval = val;
            }
            else if(type == 1){ // analog
              for(let i = 0; i < (buffer.byteLength - 1) / 2; i++){
                let valreversed = dv.getUint16(1+i*2,true);
                window.staanalogval[1][i] = valreversed - 0x7000
              }
              this.$refs.staanal.updateVal();
            } 
          })
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
          if(this.ishalfskip){
            this.apskipvar++
            if(this.apskipvar > 10){
              this.apskipvar = 0
            }
            else{
              return
            }
          }
          e.data.arrayBuffer().then(buffer => {
            var dv = new DataView(buffer);
            let type = dv.getUint8(0);
            if (type == 0){// digital
              let val = dv.getUint8(1);
              this.apdigitalval = val;
            }
            else if(type == 1){ // analog
              for(let i = 0; i < (buffer.byteLength - 1) / 2; i++){
                let valreversed = dv.getUint16(1+i*2,true);
                window.apanalogval[1][i] = valreversed - 0x7000
              }
              // console.log(this.apanalogval)
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

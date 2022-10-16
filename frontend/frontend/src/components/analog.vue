<template>
    <v-card ref="uplot_parent" class="cont">
        <UplotVue
            :data="data"
            :options="options"
            :target="null"
            ref="uplot"
            @create="createPlot"
        />
        <v-divider></v-divider>
        <v-card-actions>
            缩放：{{zoomlevel}}
            <v-spacer></v-spacer>
            <v-btn :disabled="!ispause || (zoomlevel > 4)" @click="zoomlevel ++">放大</v-btn>
            <v-btn :disabled="!ispause || (zoomlevel < 1)" @click="zoomlevel --">缩小</v-btn>
            <v-btn @click="ispause = !ispause">{{ispause ? '继续' : '暂停'}}</v-btn>
        </v-card-actions>
    </v-card>
</template>

<script>

// eslint-disable-next-line no-unused-vars
import uPlot from 'uplot';
import UplotVue from 'uplot-vue';
import 'uplot/dist/uPlot.min.css';

const lowPassFilter = require('low-pass-filter').lowPassFilter;

export default {
    components: {
        UplotVue
    },
    props:{
        windowKey: String,
        label: String
    },
    data() {
        return {
            data: [
                [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
                [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
            ],
            zoomkeep:[],
            zoomlevel:0,
            ispause:false ,
            options: {
                title: this.label,
                width: 400,
                height: 300,
                series: [
                {},
                {
                // initial toggled state (optional)
                show: true,

                spanGaps: false,

                // in-legend display
                label: "值",
                value: (self, rawValue) => Math.floor(rawValue) + "(" + (rawValue / 4096 * 3.3).toFixed(2) + "V)",

                // series style
                stroke: "red",
                width: 1,
                fill: "rgba(255, 0, 0, 0.3)",
                dash: [10, 5],
                }
                ],
                cursor:{
                    drag:{
                        x: false,
                        y: false,
                        dist: 0
                    }
                },
                scales:{
                    x:{
                        time: false,
                        // auto: false,
                        // range: [0, ]
                    },
                    y:{
                        auto: false,
                        range: [0, 4096 ]
                    }
                }
            },
            target: null,
            chart: null,
        };
    },
    watch: {
        val: function () {
            this.updateVal()
        },
        zoomlevel: function(){
            if(!this.ispause){
                return;
            }
            if(this.zoomlevel == 0){
                this.chart.setData([window[this.windowKey][0],this.zoomkeep])
            }
            let originalPointCount = Math.ceil(this.zoomkeep.length / (this.zoomlevel + 1));
            let originalMid = Math.floor(this.zoomkeep.length / 2);
            let originalPoints = this.zoomkeep.slice(originalMid - originalPointCount / 2, originalMid + originalPointCount / 2);
            
            //interpolate points to 512.
            let interpolatePoints = new Array(512)
            for(let i = 0; i < 512; i++){
                interpolatePoints[i] = originalPoints[Math.floor(i * originalPointCount / 512)]
            }
            let interpolateIndex = new Array(512)
            for(let i = 0; i < 512; i++){
                interpolateIndex[i] = i
            }

            // butterworth low pass filter
            lowPassFilter(interpolatePoints, 22050, 44100 * (this.zoomlevel + 1) * (this.zoomlevel + 1), 1)
            console.log(interpolatePoints)
            let data = [interpolateIndex, interpolatePoints]
            this.chart.setData(data)
        },
        ispause: function(){
            this.zoomlevel = 0
            this.zoomkeep = Array.from(window[this.windowKey][1])
        }
    },
    methods:{
        updateVal:function(){
            if(this.ispause){
                return
            }
            if(window[this.windowKey][0].length != window[this.windowKey][1].length){
                window[this.windowKey][0] = []
                for(let i = 0; i < window[this.windowKey][1].length; i++){
                    window[this.windowKey][0].push(i);
                }
                console.log("updating label")
            }
            this.chart.setData(window[this.windowKey])
        },
        handleResize: function () {
            //get the parent element inner size 
            let parent = this.$refs.uplot_parent.$el;
            let parentWidth = parent.clientWidth;
            // let parentHeight = parent.clientHeight;
            // console.log(parentWidth,parentHeight)
            //set the chart size
            this.chart.setSize({ width: parentWidth, height: 300 })
        },
        createPlot: function (chart) {
            //save the chart object
            this.chart = chart;
            //add the resize listener
            window.addEventListener('resize', this.handleResize);
            this.handleResize();
        }
    },
}
</script>

<style>
    .cont{
        padding-top:10px
    }
</style>
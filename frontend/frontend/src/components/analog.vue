<template>
    <v-card ref="uplot_parent" class="cont">
        <UplotVue
            :data="data"
            :options="options"
            :target="null"
            ref="uplot"
            @create="createPlot"
        />
    </v-card>
</template>

<script>

// eslint-disable-next-line no-unused-vars
import uPlot from 'uplot';
import UplotVue from 'uplot-vue';
import 'uplot/dist/uPlot.min.css';

export default {
    components: {
        UplotVue
    },
    props:{
        val: Array,
        label: String
    },
    data() {
        return {
            data: [
                [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
                [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
            ],
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
                value: (self, rawValue) => rawValue + "(" + (rawValue / 4096 * 3.3).toFixed(2) + "V)",

                // series style
                stroke: "red",
                width: 1,
                fill: "rgba(255, 0, 0, 0.3)",
                dash: [10, 5],
                }
                ],
                cursor:{
                    drag:{
                        // x: false,
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
            chart: null
        };
    },
    watch: {
        val: function () {
            this.updateVal()
        }
    },
    methods:{
        updateVal:function(){
            this.data[1] = this.val;
            if(this.data[0].length != this.val.length){
                let newlabel = []
                for(let i = 0; i < this.val.length; i++){
                    newlabel.push(i);
                }
                this.data[0] = newlabel;
                console.log("updating label")
            }
            this.chart.setData(this.data)
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
        padding:10px
    }
</style>
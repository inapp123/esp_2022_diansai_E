<template>
    <v-card>
        <UplotVue
            :data="data"
            :options="options"
            :target="null"
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
                    {}
                ]
            },
            target: null
        };
    },
    watch: {
        val: function () {
            this.updateVal()
        }
    },
    methods:{
        updateVal:function(){
            this.chartData.datasets[0].data = this.val;
            this.$refs.bar.updateChart()
            if(this.chartData.labels.length != this.val.length){
                let newlabel = []
                for(let i = 0; i < this.val.length; i++){
                    newlabel.push(i);
                }
                this.chartData.labels = newlabel;
                console.log("updating label")
            }
        }
    }
}
</script>

<style>

</style>
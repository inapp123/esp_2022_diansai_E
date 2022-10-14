<template>
    <v-card>
        <v-card-title class="num">
            {{label}}
            <v-spacer></v-spacer>
            {{num}}
         </v-card-title>
        <v-divider></v-divider>
        <v-simple-table>
            <template v-slot:default>
                <thead>
                    <tr>
                        <th class="text-center" v-for="i in [0,1,2,3,4,5,6,7]" :key="label + 'tridxaa'+i">
                            {{i}}
                        </th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td class="text-center bold" v-for="i in [0,1,2,3,4,5,6,7]" :key="label + 'idxaa'+i">
                            {{getBit(num,i)?'1':'0'}}
                        </td>
                    </tr>
                </tbody>
            </template>
        </v-simple-table>
    </v-card>
</template>
  
<script>
export default {
    props:{
        num: Number,
        label: String
    },
    methods:{
        getBit(num,bit){
            return (num>>bit)&1;
        }
    },
}
</script>
  
<style scoped>
    .bold{
        font-weight: bold;
        font-size: 150%;
    }

    .num{
        font-size: 200%;
        height: 80px;
    }
</style>
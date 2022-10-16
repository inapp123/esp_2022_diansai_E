# esp_diansai

An ESP32-Based wifi analog and digital signal oscilloscope-like project, submit as 2022 diansai (TI Cup National Undergraduate Electronics Design Contest) question E

基于ESP32的wifi模拟-数字信号的类示波器项目。本项目是2022年TI杯电赛E题的一个实现。（我们这边不强制使用CC3200，所以我们用的是ESP做的）

前端基于Vue.js开发，后端基于esp-idf 4.4，使用ESP32 HTTP Server组件搭建。

## 使用方法

编辑main/config.h，对应修改为你的配置。

安装Vue CLI

```bash
npm install -g @vue/cli@4
```

进入frontend文件夹，构建前端HTML：

```bash
npm run build
```

将构建生成的frontend/dist/output.html复制到main/index.html

在esp-idf环境下，执行：

```bash
idf.py build
idf.py flash
```

构建后端并烧录到设备。

使用手机等设备连接wifi，应该就会自动跳转到界面了。（如果没有的话，请手动访问http://192.168.4.1 ）

## 直接嵌入代码的开源代码（Used Open-Source Codes）

[esp-idf-ssd1306](https://github.com/nopnop2002/esp-idf-ssd1306) ：ESP-IDF Component for interfacing with ssd1306-based screen. MIT License

[wifi-captive-portal-esp-idf](https://gitlab.com/defcronyke/wifi-captive-portal-esp-idf) ：An ESP-IDF firmware project for a wifi access point captive portal. MIT License

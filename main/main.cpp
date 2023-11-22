#include "dht20.h"
#include <thread>
#include <iostream>
#include <vector>
#include <array>


using namespace std;
#include <string>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lvgl.h"
#include "driver/i2c.h"
#include "esp_timer.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_vendor.h"






#define I2C_HOST 0
#define H_RES 128
#define V_RES 64

#include <sstream>

using namespace std;

void i2c_master_init(){
    //configure controller - ESP32-C6
    i2c_port_t i2c_master_port = I2C_NUM_0;

    //configure master
    //fixed order required, i2c.h is C code and C++ doesn't like it.
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 2,         
        .scl_io_num = 3,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {.clk_speed = 400000},             // 100kHz
        .clk_flags = 0,
    };

    i2c_param_config(i2c_master_port, &conf);

    i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

lv_disp_t* init_panel_lvgl(){
    //Panel IO
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = 0x3C,
        .control_phase_bytes = 1,
        .dc_bit_offset = 6,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_HOST, &io_config, &io_handle));

    //SDD1306/SDD1315 Driver
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .bits_per_pixel = 1,
    };

    esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle);
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_disp_on_off(panel_handle, true);

    //Init LVGL
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = H_RES * V_RES,
        .double_buffer = true,
        .hres = H_RES,
        .vres = V_RES,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }
    };
    
    return lvgl_port_add_disp(&disp_cfg);
}

extern void lvgl_ui(lv_disp_t *disp, vector<string> const &mytext, int ui_len);

string padString(string snip){
    stringstream ss;
    ss.precision(2);
    ss << snip;
    int n = snip.length();
    for(int i = n; i < 16; i++){
        ss << ' ';
    }
    return ss.str();
}

string formatTemp(float temperature){
    stringstream ss;
    ss.precision(4);
    ss << "Temp:";
    ss << temperature << "F";
    return ss.str(); 
}

string formatRH(float r_humidity){
    stringstream ss;
    ss.precision(4);
    ss << "RH:";
    ss << r_humidity << "%";
    return ss.str();
}

string formatCounter(uint count){
    stringstream ss;
    ss << "Cycles:";
    ss << count;
    return ss.str();
}

extern "C" void app_main(void){
    i2c_master_init();

    lv_disp_t *disp = init_panel_lvgl();
    lv_disp_set_rotation(disp, LV_DISP_ROT_180);
    
    DHT value;
    float faren;
    uint counter = 0;
    vector<string> myUI(4, "................");

    for(;;){
        value = get_Sensor(0x38);
        faren = value.celcius*1.8 + 32;

        if(value.valid){
            lv_obj_clean(lv_scr_act());

            counter += 1;

            myUI[0] = padString(formatTemp(faren));
            myUI[1] = padString(formatRH(value.r_humidity));
            myUI[2] = padString(formatCounter(counter));
            
            lvgl_ui(disp, myUI, 4);
        }

        this_thread::sleep_for(50ms);
    }
}
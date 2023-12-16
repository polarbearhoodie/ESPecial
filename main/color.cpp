#include <color.h>
#include <math.h>
#include <thread>           //wraps pthreads and vTaskDelay
#include <vector>

#include "esp_log.h"        // Configure LED
#include "driver/gpio.h"    // GPIO for LED
#include "led_strip.h"      // LED library

using namespace std;
static led_strip_handle_t led_strip;

/* LED strip initialization with the GPIO and pixels number*/
void configure_led(void){
    led_strip_config_t strip_config = {.strip_gpio_num = 8, .max_leds = 1};
    led_strip_rmt_config_t rmt_config = {.resolution_hz = 10 * 1000 * 1000};
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
}

rgb::rgb(int r1, int g1, int b1){
    this->red = r1;
    this->green = g1;
    this->blue = b1;
}

void set_RGB(rgb item){
    led_strip_set_pixel(led_strip, 0, item.red, item.green, item.blue);
    led_strip_refresh(led_strip);
}

void clear_led(){
    led_strip_clear(led_strip);
}

int linear_approx(int a, int b, int step, int total){
    return (a*(total-step) + b*step)/total;
}

int sigmoid_approx(int a, int b, int step, int total){
    float ratio = (step/total * 2) - 1;
    float multiplier = 1 / (1 + exp(ratio));
    //because c++ truncates, we add a 0.5 to have it round
    return int(a*multiplier + b*multiplier + 0.5);
}

rgb rgb_step(rgb A, rgb B, int step, int total, int (*func)(int, int, int, int)){
    int red_delta = func(A.red, B.red, step, total);
    int green_delta = func(A.green, B.green, step, total);
    int blue_delta = func(A.blue, B.blue, step, total);
    return rgb(red_delta, green_delta, blue_delta);   
}

void blink_sequence(vector<rgb> colorvec, chrono::microseconds msOn){
    for (rgb x : colorvec){
        set_RGB(x);
        this_thread::sleep_for(msOn);
    }
}

void continuous_sequence(vector<rgb> colorvec, int steps, chrono::milliseconds msStep){
    int vecsize = colorvec.size();
    for(int i = 0; i < vecsize; i ++){
        for(int j = 0; j < steps; j++){
            rgb tmpColor = rgb_step(colorvec[i], colorvec[(i+1)%vecsize], j, steps, &linear_approx);
            set_RGB(tmpColor);
            this_thread::sleep_for(msStep);
        }
    }
}

vector<rgb> rainbow(){
    vector<rgb> rainbow;

    rgb red = rgb(120, 0, 0);
    rgb green = rgb(0, 120, 0);
    rgb blue = rgb(0, 0, 120);

    for(int i = 0; i < 5; i++){
        rainbow.push_back(rgb_step(red, green, i, 4, &linear_approx));
    }

    for(int i = 0; i < 5; i++){
        rainbow.push_back(rgb_step(green, blue, i, 4, &linear_approx));
    }

    for(int i = 0; i < 5; i++){
        rainbow.push_back(rgb_step(blue, red, i, 4, &linear_approx));
    }

    rainbow.pop_back();

    return rainbow;
}

vector<rgb> trans(){
    vector<rgb> trans;

    rgb blue = rgb(20, 45, 50);
    rgb pink = rgb(60, 40, 45);
    rgb white = rgb(60, 60, 60);

    trans.push_back(blue);
    trans.push_back(pink);
    trans.push_back(white);
    trans.push_back(pink);
    trans.push_back(blue);
    trans.push_back(rgb(0, 0, 0));

    return trans;
}
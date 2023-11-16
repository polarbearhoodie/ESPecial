#pragma once
#include <driver/i2c.h>

struct DHT{
    float r_humidity = 0;
    float celcius = 0;
    bool valid = false;
};

void i2c_master_init();

void trigger_DHT20(int addr);

void read_DHT20(int addr, uint8_t* data_rd, size_t size);

DHT get_Temperature();
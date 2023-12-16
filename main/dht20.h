#pragma once

#include <driver/i2c.h>
#include <array>

struct DHT{
    float r_humidity = 0;
    float celcius = 0;
    bool valid = false;
};

void trigger_DHT20(int addr);

std::array<uint8_t, 7> read_DHT20(int addr);

DHT get_sensor(int addr);
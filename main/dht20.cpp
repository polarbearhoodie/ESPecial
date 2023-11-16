#include <driver/i2c.h>
#include <thread>
#include "dht20.h"

#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL I2C_MASTER_ACK                  /*!< I2C ack value */
#define NACK_VAL I2C_MASTER_NACK                /*!< I2C nack value */
#define WAIT 1                                  /*!< I2C wait period */

void i2c_master_init(){
    //configure controller - ESP32-C6
    i2c_port_t i2c_master_port = I2C_NUM_0;

    //configure master
    //fixed order required, i2c.h is C code and C++ doesn't like it.
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 5,         
        .scl_io_num = 4,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {.clk_speed = 100000},             // 100kHz
        .clk_flags = 0,
    };

    i2c_param_config(i2c_master_port, &conf);

    i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}


void trigger_DHT20(int addr){
    //init handle, begin command
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);

    //init comm and trigger measurement
    i2c_master_write_byte(cmd, (addr << 1) | WRITE_BIT, ACK_CHECK_EN); //i2c write
    i2c_master_write_byte(cmd, 0xAC, ACK_CHECK_EN); //trigger measurement
    i2c_master_write_byte(cmd, 0x33, ACK_CHECK_EN); //command parameter 1
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN); //command parameter 2

    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, WAIT);
    i2c_cmd_link_delete(cmd);
}


void read_DHT20(int addr, uint8_t* data_rd, size_t size){
    //init handle, begin command
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);

    //init communications
    i2c_master_write_byte(cmd, (addr << 1) | READ_BIT, ACK_CHECK_EN);

    //read data
    i2c_master_read(cmd, data_rd, size, ACK_VAL);
    i2c_master_read_byte(cmd, data_rd+size-1, NACK_VAL);

    //end and send command
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, WAIT);
    i2c_cmd_link_delete(cmd);
}


DHT get_Temperature(){
    
    // init sensor 
    trigger_DHT20(0x38);

    // wait for data collection, min 80ms
    std::chrono::milliseconds interval{100};
    std::this_thread::sleep_for(interval);

    // read data
    size_t data_len = 7;
    uint8_t data[data_len] = {0};
    read_DHT20(0x38, data, data_len);

    //aggregate the data 
    uint32_t humidity = 0;
    humidity += uint32_t(data[1]) << 12;
    humidity += uint32_t(data[2]) << 4;
    humidity += uint32_t(data[3]) >> 4;
    
    uint32_t temperature = 0;
    temperature += (uint32_t(data[3]) & 0x0F) << 16;
    temperature += uint32_t(data[4]) << 8;
    temperature += uint32_t(data[5]);

    //calculate the temperature

    DHT value;
    value.r_humidity = float(humidity)/1048575 * 100;
    value.celcius = float(temperature)/1048575 * 200 - 50;

    //check if the sensor was late
    if((data[0] & 0x18) == 0x18){
        //TODO***
        //CRC check
        value.valid = true;
    }

    return value;
}
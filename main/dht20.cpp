#include <driver/i2c.h>
#include <thread>
#include <array>
#include "dht20.h"

#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL I2C_MASTER_ACK                  /*!< I2C ack value */
#define NACK_VAL I2C_MASTER_NACK                /*!< I2C nack value */
#define WAIT 1                                  /*!< I2C wait period */

using namespace std;

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


array<uint8_t, 7> read_DHT20(int addr){

    const size_t size = 7;
    array<uint8_t, size> data_rd = {0};
    uint8_t* point =  &data_rd[0];

    //init handle, begin command
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);

    //init communications
    i2c_master_write_byte(cmd, (addr << 1) | READ_BIT, ACK_CHECK_EN);

    //read data
    i2c_master_read(cmd, point, size, ACK_VAL);
    i2c_master_read_byte(cmd, point+size-1, NACK_VAL);

    //end and send command
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, WAIT);
    i2c_cmd_link_delete(cmd);

    return data_rd;
}


DHT get_sensor(int addr){
    
    // init sensor 
    trigger_DHT20(0x38);

    // wait for data collection, min 80ms
    std::chrono::milliseconds interval{200};
    std::this_thread::sleep_for(interval);

    // read data
    array<uint8_t, 7> data = read_DHT20(0x38);

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
    value.relative_humidity = float(humidity)/1048575 * 100;
    value.celcius = float(temperature)/1048575 * 200 - 50;

    //check if the sensor was late
    if((data[0] & 0x18) == 0x18){
        //TODO***
        //Implement CRC check using parity bits in DHT20 specification
        value.valid = true;
    }

    return value;
}
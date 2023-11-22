# ESPecial
This project is a collection of files, designed to make building with the ESP32-C6 a little easier.

### color.h
color.h is a wrapper for led_strip.h, and makes it easier to make color gradients, as well as led transitions.

### dht20.h
dht20.h uses the I2C hardware to communicate with a DHT20 Temperature and Humidity Sensor.

# TODO
The following have not yet been tested or released to this repo.

### display.h
display.h wraps I2C to control a SDD1315 display, a varient of the standard SDD1306. Stable, but refresh rates > ~20HZ still cause errors.

### wifi.h
wifi.h allows basic access to wifi, as well as some prebuilt functions to grab data or send notifications. 

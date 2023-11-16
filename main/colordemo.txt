#include "color.h"

extern "C" void app_main(void){
    //configure RMT led 
    configure_led();
    auto flag = rainbow();

    for (;;){

        std::chrono::milliseconds interval{200};
        continuousSequence(flag, 30, interval);
    }

}

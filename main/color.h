#pragma once
#include <thread>
#include <vector>


void configure_led(void);


class rgb{
    public:
        int red;
        int green;
        int blue;

        rgb(int r1, int g1, int b1);
};


void setRGB(rgb item);
void clearLED();

int linearApprox(int a, int b, int step, int total);
int sigmoidApprox(int a, int b, int step, int total);

rgb rgbStep(rgb A, rgb B, int step, int total, int (*func)(int, int, int, int));

void blinkSequence(std::vector<rgb> colorvec, std::chrono::microseconds msOn);
void continuousSequence(std::vector<rgb> colorvec, int steps, std::chrono::milliseconds msStep);


std::vector<rgb> rainbow();
std::vector<rgb> trans();
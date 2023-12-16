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


void set_RGB(rgb item);
void clear_led();

int linear_approx(int a, int b, int step, int total);
int sigmoid_approx(int a, int b, int step, int total);

rgb rgb_step(rgb A, rgb B, int step, int total, int (*func)(int, int, int, int));

void blink_sequence(std::vector<rgb> colorvec, std::chrono::microseconds msOn);
void continuous_sequence(std::vector<rgb> colorvec, int steps, std::chrono::milliseconds msStep);


std::vector<rgb> rainbow();
std::vector<rgb> trans();
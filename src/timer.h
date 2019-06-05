#pragma once
#include "cpu.h"


class Timer
{
public:    
    void init();
    void add();
private: 
    int timer_add,time_1_16_step,div_inc;
};
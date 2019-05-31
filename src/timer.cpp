#include "timer.h"
#include "mmu.h"
#include "cpu.h"

extern Mem mem;
extern Cpu cpu;
Timer timer;

void Timer::add()
{
    time_add+=cpu.time;
    if (time_add>=16)
    {
        time_1_16_step++;
        time_add-=16;
        
        div_inc++;// 1/256 clock step
        if (div_inc==16)
        {
            mem.wb(0xff04,mem.rb(0xff04)+1);
            div_inc=0;
        }
    }
    unsign_8 tac=mem.rb(0xff07);
    int threshold;
    if (tac&4)
    {
        int temp=tac&3;
        if (temp==0) threshold=64;
        if (temp==1) threshold=1;// 1/16 clock step
        if (temp==2) threshold=4; 
        if (temp==3) threshold=16;
        if (time_1_16_step>=threshold) 
        {
            time_1_16_step=0;
            mem.wb(0xff05,mem.rb(0xff05)+1);
            if (mem.rb(0xff05)==0)
            {
                mem.wb(0xff05,mem.rb(0xff06));
                mem.wb(0xff0f,mem.rb(0xff0f)|4);
            }
        } 
    } 
}


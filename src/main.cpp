#include <cstdio>
#include <string>
#include <time.h>

#include "cpu.h"
#include "mem.h"
#include "gpu.h"
#include "timer.h"
#include "type.h"

extern Cpu cpu;
extern Mem mem;
extern Gpu gpu;
extern Timer timer;
int cas=1;
FILE *in;

void init()
{
    cpu.init();   
    mem.init();   
    gpu.init();  
    timer.init(); 
}

void execute_opcode()
{
    if (cpu.halt) {cpu._time=4; cpu.clocktime+=cpu._time; return;}
    unsign_8 op=mem.rb(cpu.reg_pc++);
    if (op==0xCB) 
    {
        op=mem.rb(cpu.reg_pc++); 
        cpu.cb_opcode[op]();
    }
    else cpu.opcode[op]();
    cpu.clocktime+=cpu._time;
}



void do_interrupt()
{
    cpu._time=0;
    unsign_8 interrupt_enable=mem.rb(0xffff),interrupt_flags=mem.rb(0xff0f);
    if(cpu.master_enable&&(interrupt_enable&interrupt_flags))
    {
        cpu.halt=0;
        unsign_8 tmp=interrupt_enable&interrupt_flags;
        if(tmp&1)
        {
            mem.wb(0xff0f,interrupt_flags&0xfe);
            cpu.rst40();
        }
        else if (tmp&2)
        {
            mem.wb(0xff0f,interrupt_flags&0xfd);
            cpu.rst48();
        }
        else if (tmp&4)
        {
            mem.wb(0xff0f,interrupt_flags&0xfb);
            cpu.rst50();
        }
        else if (tmp&16)
        {
            mem.wb(0xff0f,interrupt_flags&0xef);
            cpu.rst60();
        }
    }
    cpu.clocktime+=cpu._time;
}  


void execute()
{
    while(gpu.window.isOpen())
    {      
        do_interrupt();
        execute_opcode();
        gpu.timing();  
        timer.add();     
        if (cpu._time) 
        {   
            timer.add();   
        }
    }
}

void loading_in_game()
{    
    fread(mem.cartridge_memory,1,0x200000,in);
    fclose(in);

    switch(mem.cartridge_memory[0x147])
    {
        case 1:mem.memory_bank_controller_1=1; break;
        case 2:mem.memory_bank_controller_1=1; break;
        case 3:mem.memory_bank_controller_1=1; break;
        case 5:mem.memory_bank_controller_2=1; break;
        case 6:mem.memory_bank_controller_2=1; break;
        default: break;
    }
}

int main(int argc,char** argv)
{

    if (!(in=fopen(argv[1],"rb")))
    {
        puts("文件不存在！");
        exit(0);
    }
    
    init();
    
    loading_in_game();

    try
    {
        execute();
    }
    catch(const std::exception& e)
    {
        puts("执行异常，请检查rom文件是否合法");
    }
    
    return 0;
}

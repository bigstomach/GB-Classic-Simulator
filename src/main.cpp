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
    int a,b;
    a=cpu.reg_pc-1;
    b=op;
    if (op==0xCB) 
    {
        op=mem.rb(cpu.reg_pc++); 
        cpu.cb_opcode[op]();
        b=(b<<8)+op;
    }
    else cpu.opcode[op]();
    /* if (cas>=2000000)
    {
    printf("case:%d pc:%x op:%x hl:%x flag:%x sp:%x pc:%x a:%x bc:%x de:%x\n",
    cas,a,b,((int)cpu.reg_h<<8)+cpu.reg_l,cpu.reg_f&0xf0,cpu.reg_sp,cpu.reg_pc,cpu.reg_a,(cpu.reg_b<<8)+cpu.reg_c,(cpu.reg_d<<8)+cpu.reg_e);
    printf("0xff44 %d\n",mem.mmu[0xff44]);
    printf("time %d\n",cpu._time/4);
    }
    cas++;
    if (cas==4000000) exit(0);*/
    cpu.clocktime+=cpu._time;
}



void do_interrupt()
{
    cpu._time=0;
    unsign_8 interrupt_enable=mem.rb(0xffff),interrupt_flags=mem.rb(0xff0f);
    //printf("master_interrupt %x flag %x enable %x\n",cpu.master_enable,interrupt_flags,interrupt_enable);
    if(cpu.master_enable&&(interrupt_enable&interrupt_flags))
    {
        //printf("interrupt\n");
        cpu.halt=0;
        unsign_8 tmp=interrupt_enable&interrupt_flags;
        if(tmp&1)
        {
            mem.mmu[0xff0f]=interrupt_flags&0xfe;
            cpu.rst40();
        }
        else if (tmp&2)
        {
            mem.mmu[0xff0f]=interrupt_flags&0xfd;
            cpu.rst48();
        }
        else if (tmp&4)
        {
            mem.mmu[0xff0f]=interrupt_flags&0xfb;
            cpu.rst50();
        }
        else if (tmp&16)
        {
            mem.mmu[0xff0f]=interrupt_flags&0xef;
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
        case 1:
        case 2:
        case 3:
            mem.memory_bank_controller_1=1;
            break;
        case 5:
        case 6:
            mem.memory_bank_controller_2=1;
            break;
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            mem.memory_bank_controller_3=1;
        default: break;
    }

    mem.load_game();
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

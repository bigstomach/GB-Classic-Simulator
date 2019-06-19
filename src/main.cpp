#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>

#include "cpu.h"
#include "mem.h"
#include "gpu.h"
#include "timer.h"
#include "type.h"
#include "sound.h"

extern Cpu cpu;
extern Mem mem;
extern Gpu gpu;
extern Sounds sounds;
extern Timer timer;
int cas=1;
FILE *in;

void init()
{
    cpu.init();   
    mem.init();   
    gpu.init();  
    timer.init(); 
    sounds.init();
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
        if (cpu._time) 
        {   
            timer.add();   
        }
        execute_opcode();
        gpu.timing();  
        timer.add();       
    }
}

void print_message()
{

    std::cout<<"                      __________________________"<<std::endl;
    std::cout<<"                     |            Wish No Bugs  |"<<std::endl;
    std::cout<<"                     | .----------------------. |"<<std::endl;
    std::cout<<"                     | |  .----------------.  | |"<<std::endl;
    std::cout<<"                     | |  |                |  | |"<<std::endl;
    std::cout<<"                     | |()|                |  | |"<<std::endl;
    std::cout<<"                     | |  |                |  | |"<<std::endl;
    std::cout<<"                     | |  |                |  | |"<<std::endl;
    std::cout<<"                     | |  |                |  | |"<<std::endl;
    std::cout<<"                     | |  |                |  | |"<<std::endl;
    std::cout<<"                     | |  |                |  | |"<<std::endl;
    std::cout<<"                     | |  '----------------'  | |"<<std::endl;
    std::cout<<"                     | |__GAME BOY____________/ |"<<std::endl;
    std::cout<<"    Keyboard:Up↑ <--------+     ________        |"<<std::endl;
    std::cout<<"                     |    +    (Nintendo)       |"<<std::endl;
    std::cout<<"                     |  _| |_   \"\"\"\"\"\"\"\"   .-.  |"<<std::endl;
    std::cout<<"  Keyboard:Left← <----+[_   _]---+    .-. ( +---------> Keyboard:Z"<<std::endl;
    std::cout<<"                     |   |_|     |   (   ) '-'  |"<<std::endl;
    std::cout<<"                     |    +      |    '-'   A   |"<<std::endl;
    std::cout<<"  Keyboard:Down↓ <--------+ +----+     B+-------------> Keyboard:X"<<std::endl;
    std::cout<<"                     |      |   ___   ___       |"<<std::endl;
    std::cout<<"                     |      |  (___) (___)  ,., |"<<std::endl;
    std::cout<<"Keyboard:Right→ <-----------+ select start ;:;: |"<<std::endl;
    std::cout<<"                     |           +     |  ,;:;' /"<<std::endl;
    std::cout<<"                  jgs|           |     | ,:;:'.'"<<std::endl;
    std::cout<<"                     '-----------------------`"<<std::endl;
    std::cout<<"                                 |     |"<<std::endl;
    std::cout<<"           Keyboard:Backspace <--+     +-> Keyboard:Enter"<<std::endl;
    std::cout<<"_________________________________________________________________________________"<<std::endl;

    std::string s[5]={"ROM only","MBC1","MBC2","MBC3","MBC5"};
    std::string ram[5]={"None","1 bank","1 bank","4 banks","16 banks"};
    char name[20];
    
    for(int i=0x134; i<=0x142; i++)
        name[i-0x134]=mem.cartridge_memory[i];
    int version=mem.cartridge_memory[0x147];

    std::string mbc;
    if (version==0) mbc=s[0];
    if (version>=1&&version<=3) mbc=s[1];
    if (version>=5&&version<=6) mbc=s[2];
    if (version>=0x10&&version<=0x13) mbc=s[3];
    if (version>=0x19&&version<=0x1e) mbc=s[4];

    int rom_size_number=mem.cartridge_memory[0x148];
    int rom_size=(1<<(rom_size_number+1));

    int ram_size_number=mem.cartridge_memory[0x149];
    std::string ram_size=ram[ram_size_number];

    std::cout<<"                              FILE INFORMATION"<<std::endl;
    std::cout<<"                          "<<std::left<<std::setw(12)<<"Title:"<<name<<"(version "<<version<<")"<<std::endl;
    std::cout<<"                          "<<std::left<<std::setw(12)<<"MBC:"<<mbc<<std::endl;
    std::cout<<"                          "<<std::left<<std::setw(12)<<"ROM size:"<<rom_size<<" banks"<<std::endl;
    std::cout<<"                          "<<std::left<<std::setw(12)<<"RAM size:"<<ram_size<<std::endl;

    if (version>=0x19&&version<=0x1e)
    {
        std::cout<<"不支持的MBC类型: MBC5"<<std::endl;
        exit(0);
    }
}

void loading_in_game()
{    
    fread(mem.cartridge_memory,1,0x200000,in);
    fclose(in);

    print_message();
    
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
        std::cout<<"文件不存在！"<<std::endl;
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
        std::cout<<"执行异常，请检查rom文件是否合法"<<std::endl;
    }
    
    return 0;
}

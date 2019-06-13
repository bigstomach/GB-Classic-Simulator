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
    /*if (cas>=66000000)
    {
    printf("case:%d pc:%x op:%x hl:%x flag:%x sp:%x pc:%x a:%x bc:%x de:%x\n",
    cas,a,b,((int)cpu.reg_h<<8)+cpu.reg_l,cpu.reg_f&0xf0,cpu.reg_sp,cpu.reg_pc,cpu.reg_a,(cpu.reg_b<<8)+cpu.reg_c,(cpu.reg_d<<8)+cpu.reg_e);
    printf("0xff44 %d\n",mem.mmu[0xff44]);
    printf("time %d\n",cpu._time/4);
    }
    cas++;
    if (cas==66541490||cas==66794052||cas==67874752||cas==68226906) mem.button_state&=0x7;
    if (cas==67755028) mem.button_state&=0xe;
    if (cas==66549634||cas==66795291||cas==67876350||cas==68228063)mem.button_state|=0x8;
    if (cas==67756370) mem.button_state|=0x1;
    if (cas==70000000) exit(0);*/
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

void loading_in_game(char* filename)
{    
    FILE *in;
    in=fopen(filename,"rb");
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
	/*int tiles[160*144] =
    {
        
    };
    for (int i=1;i<10000;i++)tiles[rand()%(160*144)]=rand()%3+1;
	sf::Vector2u tileSize=sf::Vector2u(ratio, ratio);
	sf::Color fill[]={sf::Color(255,255,255),sf::Color(192,192,192),sf::Color(96,96,96),sf::Color(0,0,0)};	
	sf::RenderWindow window(sf::VideoMode(144*ratio,160*ratio), "test");
	int width=144,height=160;
       while (window.isOpen())
    {
		window.clear();
		for(int i=0; i<height; i++)
			for(int j=0; j<width; j++)
			{
			//	cout<<i<<' '<<j<<endl;
				sf::VertexArray quad(sf::Quads,4);
				int tileNumber = tiles[i*width+j];
				quad[0].position = sf::Vector2f(j * tileSize.x, i * tileSize.y);
                quad[1].position = sf::Vector2f((j + 1) * tileSize.x, i * tileSize.y);
                quad[2].position = sf::Vector2f((j + 1) * tileSize.x, (i + 1) * tileSize.y);
                quad[3].position = sf::Vector2f(j * tileSize.x, (i + 1) * tileSize.y);	
				quad[0].color=fill[tileNumber];
				quad[1].color=fill[tileNumber];
				quad[2].color=fill[tileNumber];
				quad[3].color=fill[tileNumber];
				window.draw(quad);
			}

		sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }
		window.display();
    }*/
    init();
    
    loading_in_game(argv[1]);
   
    execute();


    return 0;
}

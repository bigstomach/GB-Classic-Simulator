#include <cstdio>
#include <string>

#include "cpu.h"
#include "mem.h"
#include "gpu.h"
#include "timer.h"
#include "type.h"

extern Cpu cpu;
extern Mem mem;
extern Gpu gpu;
extern Timer timer;

void init()
{
    cpu.init();   
    mem.init();   
    gpu.init();  
    timer.init(); 
}

void execute_opcode()
{
    unsign_8 op=mem.rb(cpu.reg_pc++);
    int a,b;
    a=cpu.reg_pc-1;
    b=op;
   // printf("%x %x %x\n",a,b,(cpu.reg_f&(1<<7))?1:0);
    if (op==0xCB) 
    {
        op=mem.rb(cpu.reg_pc++); 
        cpu.cb_opcode[op]();
    }
    else cpu.opcode[op]();

    cpu.clocktime+=cpu._time;
     
}

void update_joypad()
{
    mem.wb(0xff00,0x3f);
    sf::Event event;
    while (gpu.window.pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            gpu.window.close();
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code==sf::Keyboard::Right)
            {
                mem.wb(0xff00,mem.rb(0xff00)&0xde);
            }
            if (event.key.code==sf::Keyboard::Left)
            {
                mem.wb(0xff00,mem.rb(0xff00)&0xdd);
            }
            if (event.key.code==sf::Keyboard::Up)
            {
                mem.wb(0xff00,mem.rb(0xff00)&0xdb);
            }
            if (event.key.code==sf::Keyboard::Down)
            {
                mem.wb(0xff00,mem.rb(0xff00)&0xd7);
            }
            if (event.key.code==sf::Keyboard::Z)
            {
                mem.wb(0xff00,mem.rb(0xff00)&0xee);
            }
            if (event.key.code==sf::Keyboard::X)
            {
                mem.wb(0xff00,mem.rb(0xff00)&0xed);
            }
            if (event.key.code==sf::Keyboard::Space)
            {
                mem.wb(0xff00,mem.rb(0xff00)&0xeb);
            }
            if (event.key.code==sf::Keyboard::Return)
            {
                mem.wb(0xff00,mem.rb(0xff00)&0xe7);
            }
        }
        if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code==sf::Keyboard::Right)
            {
                mem.wb(0xff00,mem.rb(0xff00)|0x21);
            }
            if (event.key.code==sf::Keyboard::Left)
            {
                mem.wb(0xff00,mem.rb(0xff00)|0x22);
            }
            if (event.key.code==sf::Keyboard::Up)
            {
                mem.wb(0xff00,mem.rb(0xff00)|0x24);
            }
            if (event.key.code==sf::Keyboard::Down)
            {
                mem.wb(0xff00,mem.rb(0xff00)|0x28);
            }
            if (event.key.code==sf::Keyboard::Z)
            {
                mem.wb(0xff00,mem.rb(0xff00)|0x21);
            }
            if (event.key.code==sf::Keyboard::X)
            {
                mem.wb(0xff00,mem.rb(0xff00)|0x22);
            }
            if (event.key.code==sf::Keyboard::Space)
            {
                mem.wb(0xff00,mem.rb(0xff00)|0x24);
            }
            if (event.key.code==sf::Keyboard::Return)
            {
                mem.wb(0xff00,mem.rb(0xff00)|0x28);
            }
        }
    }
}

void do_interrupt()
{
    timer.add();
    cpu._time=0;
    unsign_8 interrupt_enable=mem.rb(0xffff),interrupt_flags=mem.rb(0xff0f);
    if(cpu.master_enable&&interrupt_enable&&interrupt_flags)
    {
        unsign_8 tmp=interrupt_enable&interrupt_flags;
        if(tmp&1)
        {
            mem.wb(0xff0f,interrupt_flags&0xfe);
            cpu.rst40();
        }
        if (tmp&2)
        {
            mem.wb(0xff0f,interrupt_flags&0xfd);
            cpu.rst48();
        }
        if (tmp&4)
        {
            mem.wb(0xff0f,interrupt_flags&0xfb);
            cpu.rst50();
        }
        if (tmp&16)
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
        execute_opcode();
        gpu.timing(); 
        update_joypad();   
        do_interrupt();
        if (cpu._time) 
        {
            timer.add();   
            gpu.timing(); 
        }
    }
}

void loading_in_game()
{    
    FILE *in;
    in=fopen("tetris.gb","rb");
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

int main()
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
    
    loading_in_game();
   
    execute();


    return 0;
}

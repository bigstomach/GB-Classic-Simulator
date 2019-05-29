#include <iostream>
#include <string>

#include "cpu.h"
#include "mem.h"
#include "gpu.h"

extern Cpu cpu;
extern Mem mem;
extern Gpu gpu;

void execute()
{
    cpu.init();
    gpu.init();
    while(gpu.window.isOpen())
    {
        unsign_8 op=mem.rb(cpu.reg_pc);
        if (op==0xCB) {cpu.reg_pc++; op=mem.rb(cpu.reg_pc); cpu.cb_opcode[op]();}
        else cpu.opcode[op]();
        cpu.reg_pc++;
        cpu.clocktime+=cpu.time;
        unsign_8 interrupt_enable=mmu.rb(0xffff),interrupt_flags=mmu.rb(0xff0f);
        if(cpu.master_enabled&&interrupt_enable&&interrupt_flags)
        {
            unsign_8 vblank=interrupt_enable&interrupt_flags;
            if(vblank&1)
            {
                mmu.wb(0xff0f,interrupt_flags&254);
                cpu.rst40();
            }
        }
    cpu.clocktime+=cpu.time;
}
        mem.wb(0xff00,0x3f);
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
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
                if (event.key.code==sf::Keyboard::Enter)
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
                if (event.key.code==sf::Keyboard::Enter)
                {
                    mem.wb(0xff00,mem.rb(0xff00)|0x28);
                }

            }
        }
        gpu_timing();
    }
}

const int ratio=5;

int main()
{
	int tiles[160*144] =
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
    }

    return 0;
}

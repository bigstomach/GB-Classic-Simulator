#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>

#include "cpu.h"
#include "mem.h"
#include "gpu.h"

Cpu cpu;
Mem mem;

void execute()
{
    cpu.init();
    while(1)
    {
        unsign_8 op=mem.rb(cpu.reg_pc);
        if (op==0xCB) {cpu.reg_pc++; op=mem.rb(cpu.reg_pc); cpu.cb_opcode[op]();}
        else cpu.opcode[op]();
        cpu.reg_pc++;
        cpu.clocktime+=cpu.time;
        std::cout<<cpu.clocktime<<std::endl;
     //   gpu_timing();
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

#pragma once
#include <SFML/Graphics.hpp>

extern Cpu cpu;
extern Mem mem;

extern int ratio;

class Gpu
{
    sf::Vector2u tileSize;
    sf::RenderWindow window;
    unsign_8 gpu_mode,gpu_line,lcd_and_gpu_control,scroll_y,scroll_x,cur_scan_line;
    unsign_8 palette; // write only
    unsign_16 gpu_clock;
    void init();
    void timing();
    void renderscan();
    void updatetile();
    void cal_color();
};
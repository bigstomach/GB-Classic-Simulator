#pragma once
#include <SFML/Graphics.hpp>

extern Cpu cpu;
extern Mem mem;

extern int ratio;

class Gpu
{
public:
    sf::Vector2u tileSize;
    sf::RenderWindow window;
    void init();
    void timing();
private:
    int ratio=5;
    unsign_8 gpu_mode,gpu_line,lcd_and_gpu_control,scroll_y,scroll_x,cur_scan_line;
    unsign_8 palette; // write only
    unsign_16 gpu_clock;
    void renderscan();
    void updatetile();
    int cal_color(unsign_8 x);
    void update_joypad();
};
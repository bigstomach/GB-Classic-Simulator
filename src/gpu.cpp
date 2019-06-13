#include "mem.h"
#include "cpu.h"
#include "gpu.h"
#include "type.h"
#include <cstdio>

Gpu gpu;
extern Mem mem;

void Gpu::init()
{
    tileSize=sf::Vector2u(ratio, ratio);
    window.create(sf::VideoMode(160*ratio,144*ratio), "GB-Classic-Simulator");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);
    window.clear(sf::Color::White); 
    window.display();
    gpu_clock=0;
    gpu_line=0;
    gpu_mode=2;
}

//GPU frame timings
void Gpu::timing()
{
    gpu_clock+=cpu._time/4;
    int a=gpu_clock,b=gpu_mode,c=gpu_line;
    switch (gpu_mode)
    {
        case 2:
            if (gpu_clock>=80)
            {
                gpu_mode=3;
                gpu_clock%=80;
                mem.mmu[0xff41]|=0x2;
                mem.mmu[0xff41]|=0x1;
            }
            break;

        case 3:
            if (gpu_clock>=172)
            {
                gpu_mode=0;
                gpu_clock%=172;
                bool hblank_interrupt=mem.mmu[0xff41]&0x8;
                if (hblank_interrupt) mem.mmu[0xff0f]|=0x2;
                
                bool ly_interrupt=mem.mmu[0xff41]&0x40;
                bool ly_crash=(mem.mmu[0xff45]==mem.mmu[0xff44]);
                if (ly_crash&&ly_interrupt)
                    mem.mmu[0xff0f]|=0x2;
            
                if (ly_crash)
                mem.mmu[0xff41]|=0x4;
                else mem.mmu[0xff41]&=0xfb;
                mem.mmu[0xff41]&=0xfd;
                mem.mmu[0xff41]&=0xfe;    
            }
            break;

        case 0:
            if (gpu_clock>=204)
            {
                renderscan();
                gpu_line++;
                mem.mmu[0xff44]++;
                gpu_clock%=204;
                if (gpu_line==144)
                {
                    gpu_mode=1;
                    gpu_line=0;
                    mem.mmu[0xff41]&=0xfd;
                    mem.mmu[0xff41]|=0x1;
                    mem.mmu[0xff0f]|=0x1;
                    window.display();
                    // GPU._canvas.putImageData(GPU._scrn, 0, 0); ???
                }
                else
                {
                    mem.mmu[0xff41]|=0x2;
                    mem.mmu[0xff41]&=0xfe;
                    gpu_mode=2;
                }
            }
            break;

        case 1:
            if (gpu_clock>=4560)
            {
                gpu_mode=2;
                gpu_line=0;
                mem.mmu[0xff44]=0;
                mem.mmu[0xff41]|=0x2;
                mem.mmu[0xff41]&=0xfe;
                gpu_clock%=4560;
                update_joypad();
                window.clear(sf::Color::White); 
            }
            else mem.mmu[0xff44]=(gpu_clock/456)+144;
            break;

        default:
            break;
    }
}

int Gpu::cal_color(unsign_8 x)
{
    if (x==0)return 255;
    if (x==1)return 192;
    if (x==2)return 96;
    return 0;
}

void Gpu::renderscan()
{
    bool draw_window=false;
    lcd_and_gpu_control=mem.rb(0xff40);
    int background_on=lcd_and_gpu_control&1;
    int sprites_on=lcd_and_gpu_control&2;
    unsign_8 scanrow[160]={0};
    if (background_on)
    {
        unsign_8 scroll_y=mem.rb(0xff42);
        unsign_8 scroll_x=mem.rb(0xff43);
        unsign_8 window_y=mem.rb(0xff4a);
        unsign_8 window_x=mem.rb(0xff4b)-7;
        unsign_8 cur_scan_line=mem.rb(0xff44);
        unsign_8 palette=mem.rb(0xff47);

        if ((lcd_and_gpu_control&0x20)&&(window_y<=cur_scan_line)&&(cur_scan_line-window_y<=144))
            draw_window=true;

        unsign_16 mapoffset;
        if (draw_window)
            mapoffset=(lcd_and_gpu_control&0x40)?0x9c00:0x9800;
        else 
            mapoffset=(lcd_and_gpu_control&0x8)?0x9c00:0x9800;

        unsign_8 tile_set_number=(lcd_and_gpu_control&0x10)?1:0;
        unsign_16 background_tile_set=tile_set_number?0x8000:0x9000;

        unsign_8 y_in_map,x_in_map;
        if (draw_window)
            y_in_map=cur_scan_line-window_y;
        else 
            y_in_map=scroll_y+cur_scan_line;
        
        for (unsign_8 i=0;i<160;i++)
        {
            x_in_map=scroll_x+i;
            if (draw_window)
                x_in_map=i-window_x;       
            
            unsign_16 tile_x=x_in_map/8;
            unsign_16 tile_y=y_in_map/8;

            unsign_16 x_in_tile=x_in_map%8;
            unsign_16 y_in_tile=y_in_map%8;

            unsign_16 tile_index=tile_y*32+tile_x;

            unsign_16 tile_address=mapoffset+tile_index;

            int tile_pos;

            if (tile_set_number==0)
                tile_pos=sign_8(mem.rb(tile_address));
            else 
                tile_pos=mem.rb(tile_address);

            unsign_16 tile_line=background_tile_set+tile_pos*16+y_in_tile*2;
            unsign_8 color1=mem.rb(tile_line);
            unsign_8 color2=mem.rb(tile_line+1);
            unsign_8 now_color=((color1>>(7-x_in_tile))&1)+((color2>>(7-x_in_tile))&1)*2;
            
            int color_number=scanrow[i]=(palette>>(2*now_color))&3;
            int color_real=cal_color(color_number);
            
            sf::Color color(color_real,color_real,color_real);
            sf::VertexArray quad(sf::Quads,4);
            quad[0].color=quad[1].color=quad[2].color=quad[3].color=color;
            quad[0].position = sf::Vector2f(i * tileSize.x, gpu_line * tileSize.y);
            quad[1].position = sf::Vector2f((i + 1) * tileSize.x, gpu_line * tileSize.y);
            quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (gpu_line + 1) * tileSize.y);
            quad[3].position = sf::Vector2f(i * tileSize.x, (gpu_line + 1) * tileSize.y);	
            window.draw(quad);
        }
    }

    if (sprites_on)
    {
        for(int i=39; i>=0; i--)
        {
            unsign_16 pos=0xfe00+i*4;
            unsign_8 sprite_y=mem.rb(pos)-16;
            unsign_8 sprite_x=mem.rb(pos+1)-8;
            unsign_8 sprite_tile=mem.rb(pos+2);
            unsign_8 sprite_options=mem.rb(pos+3);    
            unsign_8 sprite_size=(lcd_and_gpu_control&0x4)?16:8;
            
            if (sprite_y<=gpu_line&&(sprite_y+sprite_size)>gpu_line)
            {
                int sprite_pal=(sprite_options&0x10)?mem.rb(0xff49):mem.rb(0xff48);
                int x_flip=sprite_options&0x20;
                int y_flip=sprite_options&0x40; 
                int background_priority=sprite_options&0x80;

                int tile_pos=0x8000+sprite_tile*0x10;
                unsign_8 color1,color2;

                int pixel_y=gpu_line-sprite_y;

                if (y_flip)
                    pixel_y=sprite_size-pixel_y-1;

                color1=mem.rb(tile_pos+pixel_y*2);
                color2=mem.rb(tile_pos+pixel_y*2+1);
                for(int i=0;i<8; i++)
                {
                    if ((i+sprite_x>=0&&i+sprite_x<=160)&&(!background_priority||!scanrow[sprite_x+i]))
                        {
                            int color_number;
                            if (x_flip)
                                color_number=((color1>>i)&1)+((color2>>i)&1)*2;
                            else 
                                color_number=((color1>>(7-i))&1)+((color2>>(7-i))&1)*2;
                            int color_real=cal_color((sprite_pal>>(2*color_number))&3);
                            
                            if (color_number!=0)
                            {
                                sf::VertexArray quad(sf::Quads,4);
                                sf::Color color(color_real,color_real,color_real);
                                quad[0].color=quad[1].color=quad[2].color=quad[3].color=color;
                                quad[0].position = sf::Vector2f((sprite_x+i) * tileSize.x, gpu_line * tileSize.y);
                                quad[1].position = sf::Vector2f((sprite_x+i+1) * tileSize.x, gpu_line * tileSize.y);
                                quad[2].position = sf::Vector2f((sprite_x+i+1) * tileSize.x, (gpu_line + 1) * tileSize.y);
                                quad[3].position = sf::Vector2f((sprite_x+i) * tileSize.x, (gpu_line + 1) * tileSize.y);	
                                window.draw(quad);
                            }
                        }
                }
            }
        }
    }   
}

void Gpu::update_joypad()
{
    sf::Event event;
    while (gpu.window.pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            gpu.window.close();
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code==sf::Keyboard::Right)
            {
                //printf("press right\n");
                mem.direction_state&=0xe;
            }
            if (event.key.code==sf::Keyboard::Left)
            {
                //printf("press left\n");
                mem.direction_state&=0xd;
            }
            if (event.key.code==sf::Keyboard::Up)
            {
                //printf("press up\n");
                mem.direction_state&=0xb;
            }
            if (event.key.code==sf::Keyboard::Down)
            {
                //printf("press down\n");
                mem.direction_state&=0x7;
            }
            if (event.key.code==sf::Keyboard::Z)
            {
                //printf("press z\n");
                mem.button_state&=0xe;
            }
            if (event.key.code==sf::Keyboard::X)
            {
                //printf("press x\n");
                mem.button_state&=0xd;
            }
            if (event.key.code==sf::Keyboard::Space)
            {
                //printf("press space\n");
                mem.button_state&=0xb;
            }
            if (event.key.code==sf::Keyboard::Return)
            {
                //printf("press return\n");
                mem.button_state&=0x7;
            }
        }
        if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code==sf::Keyboard::Right)
            {
                //printf("release right\n");
                mem.direction_state|=0x1;
            }
            if (event.key.code==sf::Keyboard::Left)
            {
                //printf("release left\n");
                mem.direction_state|=0x2;
            }
            if (event.key.code==sf::Keyboard::Up)
            {
                //printf("release up\n");
                mem.direction_state|=0x4;
            }
            if (event.key.code==sf::Keyboard::Down)
            {
                //printf("release down\n");
                mem.direction_state|=0x8;
            }
            if (event.key.code==sf::Keyboard::Z)
            {
                //printf("release z\n");
                mem.button_state|=0x1;
            }
            if (event.key.code==sf::Keyboard::X)
            {
                //printf("release x\n");
                mem.button_state|=0x2;
            }
            if (event.key.code==sf::Keyboard::Space)
            {
                //printf("release space\n");
                mem.button_state|=0x4;
            }
            if (event.key.code==sf::Keyboard::Return)
            {
                //printf("release return\n");
                mem.button_state|=0x8;
            }
        }
    }  
}

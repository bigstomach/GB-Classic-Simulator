#include "mem.h"
#include "cpu.h"
#include "gpu.h"

Gpu gpu;

void Gpu::init()
{
    tileSize=sf::Vector2u(ratio, ratio);
    window=new window(sf::VideoMode(144*ratio,160*ratio), "test");
}

//GPU frame timings
void Gpu::timing()
{
    gpu_clock+=cpu.time;

    switch (gpu_mode)
    {
        case 2:
            if (gpu_clock>=80)
            {
                gpu_mode=3;
                gpu_clock=0;
            }
            break;

        case 3:
            if (gpu_clock>=172)
            {
                gpu_mode=0;
                gpu_clock=0;
                renderscan();
            }
            break;

        case 0:
            if (gpu_mode>=204)
            {
                gpu_line++;
                gpu_clock=0;
                if (gpu_line==143)
                {
                    gpu_mode=1;
                    gpu_line=0;
                    // GPU._canvas.putImageData(GPU._scrn, 0, 0); ???
                }
                else
                {
                    gpu_mode=2;
                }
            }
            break;

        case 1:
            if (gpu_clock>=456)
            {
                gpu_line++;
                gpu_clock=0;
                if (gpu_line>153)
                {
                    gpu_mode=2;
                    gpu_line=0;
                }
            }
            break;

        default:
            break;
    }
}

int cal_color(unsign_8 x)
{
    if (x==0)return 255;
    if (x==1)return 192;
    if (x==2)return 96;
    return 0;
}

void Gpu::renderscan()
{
    lcd_and_gpu_control=mem.rb(0xff40);
    int background_on=lcd_and_gpu_control&1;
    int sprites_on=lcd_and_gpu_control&2;
    unsign_8 scanrow[160];
    if (background_on)
    {
        unsign_8 scroll_y=mem.rb(0xff42);
        unsign_8 scroll_x=mem.rb(0xff43);
        unsign_8 cur_scan_line=mem.rb(0xff44);
        unsign_8 palette=mem.rb(0xff47);

        unsign_16 mapoffset=(lcd_and_gpu_control&0x8)?0x9c00:0x9800;
        mapoffset+=(gpu_line+scroll_y)>>3;

        unsign_16 lineoffset=scroll_x>>3;

        unsign_8 y=(gpu_line+scroll_y)&0x7;
        unsign_8 x=scroll_x&0x7;

        unsign_8 tile_pos=((mapoffset+lineoffset)<<4)+0x8000;
        int tx=0;

        //if ((lcd_and_gpu_control&0x10)&&tile<128)
        // TODO;

        for (int i=0;i<160;i++)
        {
            unsign_8 now_color=(mem.rb(tile_pos+y*2)+mem.rb(tile_pos+y*2)<<1)&(1<<tx);
            
            scanrow[i]=palette>>(2*now_color))&3;
            int color_real=cal_color(scanrow[i]);
            sf::Color color(color_real,color_real,color_real);
            
            sf::VertexArray quad(sf::Quads,4);
            quad[0]=quad[1]=quad[2]=quad[3]=color;
            quad[0].position = sf::Vector2f(i * tileSize.x, gpu_line * tileSize.y);
            quad[1].position = sf::Vector2f((i + 1) * tileSize.x, gpu_line * tileSize.y);
            quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (gpu_line + 1) * tileSize.y);
            quad[3].position = sf::Vector2f(i * tileSize.x, (gpu_line + 1) * tileSize.y);	
            window.draw(quad);
            tx++;
            if (tx==8)
            {
                tx=0;
                lineoffset=(lineoffset+1)&31;
                tile_pos=((mapoffset+lineoffset)<<4)+0x8000;
            }
        }
    }

    if (sprites_on)
    {
        for(int i=0; i<40; i++)
        {
            unsign_16 pos=0xfe00+i*4;
            unsign_8 sprite_y=mmu.rb(pos)-16;
            unsign_8 sprite_x=mmu.rb(pos+1)-8;
            unsign_8 sprite_tile=mmu.rb(pos+2);
            unsign_8 sprite_options=mmu.rb(pos+3);    
            
            if (sprite_y<=gpu_line&&(sprite_y+8)>gpu_line)
            {
                unsign_8 sprite_pal=(sprite_options&0x10)?mmu.rb(0xff49):mmu.rb(0xff48);
                int x_flip=sprite_options&0x20;
                int y_flip=sprite_options&0x40; 
                int background_priority=sprite_options&0x80;

                int tile_pos=0x8000+sprite_tile*0x10;
                if (y_flip)
                    fill=mmu.rb(tile_pos+7-(gpu.line-sprite_y));
                else 
                    fill=mmu.rb(tile_pos+(gpu_line-sprite_y));
                for(int i=0;i<8; i++)
                {
                    if (i+sprite_x>=0&&i+sprite_x<=160&&(fill&(1<<(7-i)))&&(!background_priority||!sanrow[sprite_x+i]))
                        {
                            int color_number=(fill&(x_flip:((1<<x)?(1<<7-x));
                            int color_real=cal_color((sprite_pal>>(2*color_number))&3);
                            
                            sf::VertexArray quad(sf::Quads,4);
                            sf::Color color(color_real,color_real,color_real);
                            quad[0]=quad[1]=quad[2]=quad[3]=color;
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

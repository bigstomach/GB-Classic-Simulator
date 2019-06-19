#include "sound.h"
#include "type.h"
#include "mem.h"

Sounds sounds;
extern Mem mem;

void Channel::load()
{
    initialize(1,sample_rate);
}

void Sounds::init()
{
    mem.wb(0xff10, 0x80); // nr10
    mem.wb(0xff11, 0xbf); // nr11
    mem.wb(0xff12, 0xf3); // nr12
    mem.wb(0xff13, 0xff); // nr13
    mem.wb(0xff14, 0xbf); // nr14

    mem.wb(0xff16, 0x3f); // nr21
    mem.wb(0xff17, 0x00); // nr22
    mem.wb(0xff18, 0xff); // nr23
    mem.wb(0xff19, 0xbf); // nr24

    mem.wb(0xff1A, 0x7f); // nr30
    mem.wb(0xff1B, 0xff); // nr31
    mem.wb(0xff1C, 0x9f); // nr32
    mem.wb(0xff1D, 0xff); // nr33
    mem.wb(0xff1E, 0xbf); // nr34

    mem.wb(0xff20, 0xff); // nr41
    mem.wb(0xff21, 0x00); // nr42
    mem.wb(0xff22, 0x00); // nr43
    mem.wb(0xff23, 0xbf); // nr44

    mem.wb(0xff24, 0x77); // nr50
    mem.wb(0xff25, 0xf3); // nr51
    mem.wb(0xff26, 0xf1); // nr52

    on=true;

    channel1.on=channel2.on=channel3.on=false;

    channel1.type=1;
    channel2.type=2;
    channel3.type=3;
    channel4.type=4;

    for(int i=0;i<32;i++) channel3.wave[i]=0;

    channel1.load();
    channel2.load();
    channel3.load();
    channel4.load();
    
    channel1.play();
    channel2.play();
    channel3.play();
    channel4.play();
}

void Sounds::deal(unsign_16 address,unsign_8 n)
{
    if (address>=0xff30&&address<=0xff3f)
    {
        channel3.wave[address-0xff30]=(n>>4);
        channel3.wave[address-0xff2f]=(n&0xf);
    }
    switch (address)
    {
    case 0xff25:
        bool bit[8];
        for(int i=0; i<8; i++)
        bit[i]=(n>>i)&0x1;
        if (!bit[0]&&!bit[4]) channel1.on=0;
        if (!bit[1]&&!bit[5]) channel2.on=0;
        if (!bit[2]&&!bit[6]) channel3.on=0;
        if (!bit[3]&&!bit[7]) channel4.on=0;
        break;
    case 0xff26:
        on=n>>7;
        break;
    //channel 1
    case 0xff11:
        channel1.wave_duty=(n>>6)+1;
        break;
    case 0xff14:
        if (n&0x80)
        {
            channel1.reset();

            channel1.envelope_increase=mem.rb(0xff12)&0x8;
            channel1.envelope_initial=(mem.rb(0xff12)&0xf0)>>4;
            channel1.envelope_sweep_num=mem.rb(0xff12)&0x7;
            channel1.volume=channel1.envelope_initial*gain;

            channel1.sweep_increase=!(mem.rb(0xff10)&0x8);
            channel1.sweep_num=mem.rb(0xff10)&0x7;
            channel1.sweep_time=(mem.rb(0xff10)&0x70)>>4;

            channel1.frequency_last=static_cast<unsign_16>((n&0x7)<<8)+mem.rb(0xff13);

            channel1.not_loop=n&0x40;
            channel1.frequency=131072/(2048-channel1.frequency_last);
            channel1.tick_unit=sample_rate/static_cast<double>(channel1.frequency);

            channel1.wave_duty=(mem.rb(0xff11)>>6)+1;
            channel1.duration=(64-static_cast<double>(mem.rb(0xff11)&0x3f))/256.0;

            channel1.on=true;
        }
        break;
    //channel 2
    case 0xff18:
        channel2.frequency_lo=n;
        channel2.frequency=131072/(2048-(static_cast<unsign_16>(channel2.frequency_hi<<8)+channel2.frequency_lo));
        channel2.tick_unit=sample_rate/static_cast<double>(channel2.frequency);
        break;
    case 0xff19:
        if (n&0x80)
        {
            channel2.reset();
            channel2.envelope_increase=mem.rb(0xff17)&0x8;
            channel2.envelope_initial=(mem.rb(0xff17)&0xf0)>>4;
            channel2.envelope_sweep_num=mem.rb(0xff17)&0x7;
            channel2.volume=channel1.envelope_initial*gain;

            channel2.not_loop=n&0x40;
            channel2.frequency_hi=n&0x7;
            channel2.frequency=131072/(2048-(static_cast<unsign_16>(channel2.frequency_hi<<8)+channel2.frequency_lo));
            channel2.tick_unit=sample_rate/static_cast<double>(channel2.frequency);

            channel2.wave_duty=(mem.rb(0xff16)>>6)+1;
            channel2.duration=(64-static_cast<double>(mem.rb(0xff16)&0x3f))/256.0;

            channel2.on=true;
        }
        break;
    //channel 3
    case 0xff1a:
        if (n&0x80)
            channel3.on=true;
        else 
            channel3.on=false;
        break;
    case 0xff1c:
        switch ((n&0x60)>>5)
        {
            case 0x00:
                channel3.volume=0; break;
            case 0x01:
                channel3.volume=30000; break; 
            case 0x02:
                channel3.volume=15000; break;
            case 0x03:
                channel3.volume=7500; break;
        }
        break;
    case 0xff1e:
        if (n&0x80)
        {
            channel3.reset();
            channel3.frequency=65536/(2048-(static_cast<int>((n&0x7)<<8)+mem.rb(0xff1d)));
            channel3.not_loop=n&0x40;
            channel3.wave_duty=1;
            channel3.duration=(256-static_cast<double>(mem.rb(0xff1b)))/256.0;
            channel3.on=1;
        }
        break;
    //channel 4
    case 0xff22:
        unsign_8 shift_clock_frequency;
        shift_clock_frequency=static_cast<unsign_8>((n&0xf0)>>4);
        double divide_ratio;
        divide_ratio=static_cast<double>(n&0x7);
        if (divide_ratio==0)
            divide_ratio=0.5;
        channel4.frequency=static_cast<int>(524288/divide_ratio/(1<<(shift_clock_frequency+1)));
        break;
    case 0xff23:
        if (n&0x80)
        {
            channel4.on=true;
            channel4.envelope_tick=0;
            channel4.last_envelope=0;
            channel4.tick=0;

            channel4.envelope_increase=(mem.rb(0xff21)>>3)&1;
            channel4.envelope_initial=(mem.rb(0xff21)&0xf0)>>4;
            channel4.envelope_sweep_num=mem.rb(0xff21)&0x7;
            channel4.volume=channel4.envelope_initial*gain;
            channel4.wave_duty=1;

            channel4.duration=(64-static_cast<double>(mem.rb(0xff20)&0x3f))/256.0;
            channel4.not_loop=n&0x40;   
        }
        break;
    }
}

void Channel::reset()
{
    on=false;
    frequency=envelope_tick=last_envelope=tick=0;
}

void Channel::sweep()
{
    sweep_tick+=1.0/sample_rate;
    if (sweep_num>0)
    {
        if (sweep_tick-last_sweep>=sweep_time/128.0)
            if (frequency>0)
            {
                int tmp_freq=0;
                if (sweep_increase)
                    tmp_freq=frequency_last+frequency_last/(1<<sweep_num);
                else 
                    tmp_freq=frequency_last-frequency_last/(1<<sweep_num);
                frequency_last=tmp_freq;
                frequency=131072/(2048-tmp_freq);
                frequency_last=sweep_tick;
            }
    }
}

void Channel::envelope()
{
    envelope_tick+=1.0/sample_rate;
    if (envelope_sweep_num>0)
    {
        double step=static_cast<double>(envelope_sweep_num)/64.0;
        if (envelope_tick-last_envelope>=step)
        {
            if (envelope_initial>0)
            {
                envelope_initial--;
                if (envelope_increase)
                    volume=(0xf-envelope_initial)*gain;
                else 
                    volume=envelope_initial*gain;
                last_envelope=envelope_tick;
            }
        }
    }
}

bool Channel::need_play()
{
    if (!sounds.on||frequency<=0||!on||wave_duty==0||(not_loop&&duration<=0))
        return 0;
    return 1; 
}

bool Channel::onGetData(Chunk& data)
{
    data.sampleCount=tick_per_buffer;
    for (int i=0;i<tick_per_buffer;i++)
    {
        tick+=static_cast<double>(frequency)/sample_rate;
        if (need_play())
        {
            double tick_in_cycle=tick*2*pi;
            switch (type)
            {
            case 1:
            case 2:
                if (sin(tick_in_cycle)<=wave_duty_map[wave_duty])
                {
                    samples[i]=volume;
                }
                else
                {
                    samples[i]=0;
                }
                break;
            case 3:
                int sample_id;
                sample_id=32*(tick-floor(tick));
                samples[i]=static_cast<sf::Int16>(wave[sample_id])*volume/16;
                break;
            case 4:
                if (abs(tick-last_gen_tick)>1)
                {
                    int rnd=rand()%4096;
                    samples[i]=rnd*volume/4096;
                    last_gen=rnd;
                    last_gen_tick=tick;
                }
                else samples[i]=last_gen*volume/4096;
                break;
            }
            duration-=second_per_tick;
        }
        else samples[i]=0;
        envelope();
        sweep();
    }
    data.samples=samples;
    return true;
}

void Channel::onSeek(sf::Time timeOffset){}

#pragma once
#include "type.h"

#include <cmath>
#include <vector>

#include <SFML/Audio.hpp>

const unsigned sample_rate=44800;
const double second_per_tick=1.0/sample_rate;
const unsigned tick_per_buffer=sample_rate/32;
const unsigned gain=2000;
const double pi=acos(-1.0);
const std::vector<double> wave_duty_map={-1,sin(11*pi/8),sin(5*pi/4),0,sin(pi/4)};

class Channel:public sf::SoundStream
{
public:
    void load();
    void reset();
    void sweep();
    void envelope();
    bool need_play();
    void stream();

    bool on;
    

    bool envelope_increase;
    sign_8 envelope_initial,envelope_sweep_num;
    double last_envelope,envelope_tick;

    bool sweep_increase;
    int sweep_num,sweep_time;
    double last_sweep,sweep_tick;
    int frequency_last;

    bool not_loop;

    int frequency;
    unsign_8 frequency_lo,frequency_hi;

    unsign_8 wave_duty;

    unsign_16 volume;

    double duration,tick,tick_unit;

    double last_gen=0,last_gen_tick=0;

    int type;

    sf::Int16 samples[tick_per_buffer];
    unsign_8 wave[32];

    sf::SoundBuffer buffer;
    sf::Sound sound;

private:
    virtual bool onGetData(Chunk& data);
    virtual void onSeek(sf::Time timeOffset);
};

class Sounds
{
public:
    Channel channel1,channel2,channel3,channel4;
    bool on;

    void init();
    void deal(unsign_16 address,unsign_8 n);
};

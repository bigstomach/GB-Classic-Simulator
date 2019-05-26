#pragma once

#include "cpu.h"


class Mem
{
public:
    unsign_8 mmu[0xffff];
    unsign_8 rb(unsign_16 address);
    unsign_16 rw(unsign_16 address);
    void wb(unsign_16 address, unsign_8 n);
    void ww(unsign_16 address, unsign_16 n);

};
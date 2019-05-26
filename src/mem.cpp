#include "mem.h"

unsign_8 Mem::rb(unsign_16 address)
{
    return mmu[address];
}

unsign_16 Mem::rw(unsign_16 address)
{
    return unsign_16(mmu[address+1]<<8)+mmu[address];
}

void Mem::wb(unsign_16 address, unsign_8 n)
{
    mmu[address]=n;
}

void Mem::ww(unsign_16 address, unsign_16 n)
{
    mmu[address+1]=(n>>8); 
    mmu[address]=n&0xff;
}
#include "mem.h"
#include "type.h"
#include "cpu.h"
#include <cstring>
#include <iostream>

Mem mem;
extern Cpu cpu;

void Mem::init()
{
    memset(mmu,0,sizeof(mmu));
    memset(cartridge_memory,0,sizeof(cartridge_memory));
    memset(ram_banks,0,sizeof(ram_banks));
    memory_bank_controller_1=0;
    memory_bank_controller_2=0;
    current_rom_bank=1;
    current_ram_bank=0;
    inbios=1;
    mmu[0xff05]=0x00;mmu[0xff06]=0x00;mmu[0xff07]=0x00;mmu[0xff10]=0x80;
    mmu[0xff11]=0xbf;mmu[0xff12]=0xf3;mmu[0xff14]=0xbf;mmu[0xff16]=0x3f;
    mmu[0xff17]=0x00;mmu[0xff19]=0xbf;mmu[0xff1a]=0x7f;mmu[0xff1b]=0xff;
    mmu[0xff1c]=0x9f;mmu[0xff1e]=0xbf;mmu[0xff20]=0xff;mmu[0xff21]=0x00;
    mmu[0xff22]=0x00;mmu[0xff23]=0xbf;mmu[0xff24]=0x77;mmu[0xff25]=0xf3;
    mmu[0xff26]=0xf1;mmu[0xff40]=0x91;mmu[0xff42]=0x00;mmu[0xff43]=0x00;
    mmu[0xff45]=0x00;mmu[0xff47]=0xfc;mmu[0xff48]=0xff;mmu[0xff49]=0xff;
    mmu[0xff4a]=0x00;mmu[0xff4b]=0x00;mmu[0xffff]=0x00;
}

unsign_8 Mem::rb(unsign_16 address)
{
    if (address>=0x0000&&address<0x100)
    {
        return bios[address];
    }
    else if (address>=0x100&&address<0x4000)
    {
        return cartridge_memory[address];      
    }
    else if (address>=0x4000&&address<=0x7fff)
    {
        unsign_16 tmp_address=address-0x4000;
        return cartridge_memory[tmp_address+(current_rom_bank*0x4000)];
    }
    else if (address>=0xA000&&address<=0xbfff)
    {
        unsign_16 tmp_address=address-0xA000;
        return ram_banks[tmp_address+(current_ram_bank*0x2000)];
    }   
    return mmu[address];
}

unsign_16 Mem::rw(unsign_16 address)
{
    return (unsign_16(rb(address+1))<<8)+rb(address);
}

void Mem::do_ram_enable(unsign_16 address,unsign_8 n)
{
    if (memory_bank_controller_2)
        if (address&0x10) return;
    enable_ram=((n*0xf)==0xa)?1:0;
}

void Mem::do_rom_change_1(unsign_8 n)
{
    if (memory_bank_controller_2)
    {
        current_rom_bank=n&0xf;
        if (!current_rom_bank) current_rom_bank++;
        return ;
    }
    n&=0x1f;
    if (!n) n=1;
    current_rom_bank=(current_rom_bank&0x60)+n;
}

void Mem::do_rom_change_2(unsign_8 n)
{
    current_rom_bank=(current_rom_bank&0x1f)+((n&3)<<5);
    if (!current_rom_bank) current_rom_bank++;
}

void Mem::do_ram_change(unsign_8 n)
{
    current_ram_bank&=3;
}

void Mem::do_mode_switch(unsign_8 n)
{
    ram_mode=(n&1);
    if (!ram_mode)
        current_ram_bank=0;   
}

void Mem::deal_banking(unsign_16 address, unsign_8 n)
{
    if(address<0x2000)
    {
        if (memory_bank_controller_1||memory_bank_controller_2)
            do_ram_enable(address,n);
    }
    else if (address>=0x2000&&address<0x4000)
    {
        if(memory_bank_controller_1||memory_bank_controller_2)
            do_rom_change_1(n);
    }
    else if (address>=0x4000&&address<0x6000)
    {
        if (memory_bank_controller_1)
        {
            if (ram_mode)
                do_ram_change(n);      
            else 
                do_rom_change_2(n);
        }
    }
    else if (address>=0x6000&&address<0x8000)
    {
        if (memory_bank_controller_1)
            do_mode_switch(n);
    }

}


void Mem::dma_transfer(unsign_8 n)
{
    unsign_16 address=unsign_16(n)<<8;
    for(int i=0; i<0xa0; i++)
        wb(0xfe00+i,rb(address+i));
}


void Mem::wb(unsign_16 address, unsign_8 n)
{
    
    if (address<0x8000)
    {
        deal_banking(address,n);
    }
    else if (address>=0xa000&&address<0xc000)
    {
        if (enable_ram)
        {
            unsign_16 tmp_address=address-0xa000;
            ram_banks[tmp_address+(current_ram_bank*0x2000)]=n;
        }
    }
    else if (address>=0xe000&&address<0xfe00) 
   { 
       mmu[address]=n;
       wb(address-0x2000,n); 
   } 
   else if (address==0xff46)
   {
       dma_transfer(n);
   }
   else mmu[address]=n;
   if (address==0xff40) mmu[address]=0;
}

void Mem::ww(unsign_16 address, unsign_16 n)
{
    wb(address+1,(n>>8)); 
    wb(address,n&0xff);
}
#include "cpu.h"
#include "mem.h"
#include "type.h"
#include <iostream>
Cpu cpu;
extern Mem mem;

void Cpu::reset()
{
    reg_a=0x1; reg_b=0; reg_c=0x13; reg_d=0; reg_e=0xd8; reg_f=0xb0; reg_h=0x1; reg_l=0x4d;
    reg_pc=0x00; reg_sp=0xfffe;
    halt=0; master_enable=1;
    _time=0;
    clocktime=0;
}

void Cpu::rst40()
{
    master_enable=0;
    reg_sp-=2;
    mem.ww(reg_sp,reg_pc);
    reg_pc=0x0040;
    _time=12;
}
void Cpu::rst48()
{
    master_enable=0;
    reg_sp-=2;
    mem.ww(reg_sp,reg_pc);
    reg_pc=0x0048;
    _time=12;
}
void Cpu::rst50()
{
    master_enable=0;
    reg_sp-=2;
    mem.ww(reg_sp,reg_pc);
    reg_pc=0x0050;
    _time=12;
}
void Cpu::rst60()
{
    master_enable=0;
    reg_sp-=2;
    mem.ww(reg_sp,reg_pc);
    reg_pc=0x0060;
    _time=12;
}

void Cpu::zero_flag(bool flag)
{
    if (flag) reg_f|=128;
    else reg_f&=127;
}

void Cpu::subtract_flag(bool flag)
{
    if (flag) reg_f|=64;
    else reg_f&=191;
}

void Cpu::half_carry_flag(bool flag)
{
    if (flag) reg_f|=32;
    else reg_f&=223;
}

void Cpu::carry_flag(bool flag)
{
    if (flag) reg_f|=16;
    else reg_f&=239;
}

void Cpu::add(unsign_8 n)
{
    zero_flag(reg_a+n==0); 
    subtract_flag(0); 
    half_carry_flag((reg_a&0xf)+(n&0xf)>15); 
    carry_flag(reg_a+n>255); 
    reg_a+=n; 
    _time=4;
}

void Cpu::adc(unsign_8 n)
{
    unsign_8 cf=(reg_f&0x10)?1:0,value=reg_a+n+cf; 
    zero_flag(!value); 
    subtract_flag(0); 
    half_carry_flag((reg_a&0xf)+(n&0xf)+(cf&0xf)>15); 
    carry_flag(reg_a+n+cf>255); 
    reg_a=value; 
    _time=4;
}

void Cpu::sub(unsign_8 n)
{
     zero_flag(reg_a-n==0); 
     subtract_flag(1); 
     half_carry_flag((reg_a&0xf)-(n&0xf)>0);
     carry_flag(reg_a>n); 
     reg_a-=n; 
     _time=4;
}

void Cpu::sbc(unsign_8 n)
{
    unsign_8 cf=(reg_f&0x10)?1:0,value=reg_a-n-cf; 
    zero_flag(!value); 
    subtract_flag(1);
    half_carry_flag((reg_a&0xf)-(n&0xf)-(cf&0xf)>0); 
    carry_flag(reg_a-n-cf>0); 
    reg_a=value; 
    _time=4;
}

void Cpu::_and(unsign_8 n)
{
    reg_a&=n;
    zero_flag(!reg_a);
    subtract_flag(0);
    half_carry_flag(1);
    carry_flag(0);
    _time=4;
}

void Cpu::_or(unsign_8 n)
{
    reg_a|=n;
    zero_flag(!reg_a);
    subtract_flag(0);
    half_carry_flag(0);
    carry_flag(0);
    _time=4;
}

void Cpu::_xor(unsign_8 n)
{
    reg_a^=n;
    zero_flag(!reg_a);
    subtract_flag(0);
    half_carry_flag(0);
    carry_flag(0);
    _time=4;
}

void Cpu::cp(unsign_8 n)
{
    zero_flag(reg_a==n);
    subtract_flag(1);
    half_carry_flag((0xf&reg_a)-(0xf&n)<0);
    carry_flag(reg_a<n);
    _time=4;
} 

void Cpu::inc(unsign_8 &n)
{
    zero_flag(!(n+1));
    subtract_flag(0);
    half_carry_flag((0xf&n)+1>15);
    n++;
    _time=4;
}

void Cpu::dec(unsign_8 &n)
{
    zero_flag(!(n-1));
    subtract_flag(1);
    half_carry_flag((0xf&n)-1>0);
    n--;
    _time=4;
}

void Cpu::add_hl(unsign_16 n)
{
    unsign_16 tmp=mem.rw((reg_h<<8)+reg_l);
    subtract_flag(0);
    half_carry_flag((0xfff&n)+(0xfff&tmp)>4095);
    carry_flag(n+tmp>65535);
    reg_l=(n+tmp)&255;
    reg_h=(n+tmp)>>8;
    _time=8;
}

void Cpu::inc_16(unsign_8 &n,unsign_8 &m)
{
    if (m==255){m=0; n++;}else m++;
    _time=8;
}

void Cpu::dec_16(unsign_8 &n,unsign_8 &m)
{
    if (m==0){m=255;n--;}else m--;
    _time=8;
}

void Cpu::swap_n(unsign_8 &n)
{
    int upper=n>>4,lower=0xf&n;
    n=upper+(lower<<4);
    zero_flag(!n);
    subtract_flag(0);
    half_carry_flag(0);
    carry_flag(0);
}

void Cpu::rlc(unsign_8 &n)
{
    carry_flag(n&0x80);
    n=n<<1|((n&0x80)>>7);
    zero_flag(n==0);
    subtract_flag(0);
    half_carry_flag(0);
    _time=4;
}

void Cpu::rl(unsign_8 &n)
{
    carry_flag(n&0x80);
    n=(n<<1)|((reg_f&0x10)>>5);
    zero_flag(n==0);
    subtract_flag(0);
    half_carry_flag(0);
    _time=4;
}

void Cpu::rrc(unsign_8 &n)
{
    carry_flag(n&1);
    n=(n>>1)+((n&1)?0x80:0);
    zero_flag(n==0);
    subtract_flag(0);
    half_carry_flag(0);
    _time=4;
}

void Cpu::rr(unsign_8 &n)
{
    carry_flag(n&1);
    n=(n>>1)+((reg_f&0x10)?0x80:0);
    zero_flag(n==0);
    subtract_flag(0);
    half_carry_flag(0);
    _time=4;
}

void Cpu::sla(unsign_8 &n)
{
    carry_flag(n&0x80);
    n=(n<<1);
    zero_flag(n==0);
    subtract_flag(0);
    half_carry_flag(0);
    _time=8;    
}

void Cpu::sra(unsign_8 &n)
{
    carry_flag(n&1);
    n=(n>>1)+(n&0x80);
    zero_flag(n==0);
    subtract_flag(0);
    half_carry_flag(0);
    _time=8;
}

void Cpu::srl(unsign_8 &n)
{
    carry_flag(n&1);
    n=(n>>1);
    zero_flag(n==0);
    subtract_flag(0);
    half_carry_flag(0);
    _time=8;
}

void Cpu::bit(int b, unsign_8 n)
{
    zero_flag(n&(1<<b));
    subtract_flag(0);
    half_carry_flag(1);
    _time=8;
}

void Cpu::set(int b,unsign_8 &n)
{
    n|=(1<<b);
    _time=8;
}

void Cpu::res(int b,unsign_8 &n)
{
    n&=(~(1<<b));
    _time=8;
}

void Cpu::jp()
{
    reg_pc=mem.rw(reg_pc); 
    _time=12;
}

void Cpu::jr()
{
    reg_pc=+unsign_16(mem.rb(reg_pc));
    _time=8;
}

void Cpu::call()
{
    reg_sp-=2; mem.ww(reg_sp,reg_pc+2);  jp();
}

void Cpu::rst(unsign_8 n)
{
    reg_sp-=2; mem.ww(reg_sp,reg_pc); reg_pc=n; _time=32;
}

void Cpu::ret()
{
    reg_pc=mem.rw(reg_sp); reg_sp+=2; _time=8;
}

unsign_8 Cpu::readhl()
{
    return mem.rb(unsign_16(reg_h<<8)+reg_l);
}

void Cpu::writehl(unsign_8 n)
{
    mem.wb(unsign_16(reg_h<<8)+reg_l,n);
}

void Cpu::init()
{
    reset();
    
    //8-bit loads
    //LD nn,n
    opcode[0x06]=[&]{reg_b=mem.rb(reg_pc); reg_pc++; _time=8;};
    opcode[0x0e]=[&]{reg_c=mem.rb(reg_pc); reg_pc++; _time=8;};
    opcode[0x16]=[&]{reg_d=mem.rb(reg_pc); reg_pc++; _time=8;};
    opcode[0x1e]=[&]{reg_e=mem.rb(reg_pc); reg_pc++; _time=8;};
    opcode[0x26]=[&]{reg_h=mem.rb(reg_pc); reg_pc++; _time=8;};
    opcode[0x2e]=[&]{reg_l=mem.rb(reg_pc); reg_pc++; _time=8;};

    //LD r1,r2
    opcode[0x7f]=[&]{_time=4;};
    opcode[0x78]=[&]{reg_a=reg_b; _time=4;};
    opcode[0x79]=[&]{reg_a=reg_c; _time=4;};
    opcode[0x7a]=[&]{reg_a=reg_d; _time=4;};
    opcode[0x7b]=[&]{reg_a=reg_e; _time=4;};
    opcode[0x7c]=[&]{reg_a=reg_h; _time=4;};
    opcode[0x7d]=[&]{reg_a=reg_l; _time=4;};
    opcode[0x7e]=[&]{reg_a=readhl();_time=8;};
    
    opcode[0x40]=[&]{_time=4;};
    opcode[0x41]=[&]{reg_b=reg_c; _time=4;};
    opcode[0x42]=[&]{reg_b=reg_d; _time=4;};
    opcode[0x43]=[&]{reg_b=reg_e; _time=4;};
    opcode[0x44]=[&]{reg_b=reg_h; _time=4;};
    opcode[0x45]=[&]{reg_b=reg_l; _time=4;};
    opcode[0x46]=[&]{reg_b=readhl(); _time=8;};

    opcode[0x48]=[&]{reg_c=reg_b; _time=4;};
    opcode[0x49]=[&]{_time=4;};
    opcode[0x4a]=[&]{reg_c=reg_d; _time=4;};
    opcode[0x4b]=[&]{reg_c=reg_e; _time=4;};
    opcode[0x4c]=[&]{reg_c=reg_h; _time=4;};
    opcode[0x4d]=[&]{reg_c=reg_l; _time=4;};
    opcode[0x4e]=[&]{reg_c=readhl(); _time=8;};

    opcode[0x50]=[&]{reg_d=reg_b; _time=4;};
    opcode[0x51]=[&]{reg_d=reg_c; _time=4;};
    opcode[0x52]=[&]{_time=4;};
    opcode[0x53]=[&]{reg_d=reg_e; _time=4;};
    opcode[0x54]=[&]{reg_d=reg_h; _time=4;};
    opcode[0x55]=[&]{reg_d=reg_l; _time=4;};
    opcode[0x56]=[&]{reg_d=readhl(); _time=8;};

    opcode[0x58]=[&]{reg_e=reg_b; _time=4;};
    opcode[0x59]=[&]{reg_e=reg_c; _time=4;};
    opcode[0x5a]=[&]{reg_e=reg_d; _time=4;};
    opcode[0x5b]=[&]{_time=4;};
    opcode[0x5c]=[&]{reg_e=reg_h; _time=4;};
    opcode[0x5d]=[&]{reg_e=reg_l; _time=4;};
    opcode[0x5e]=[&]{reg_e=readhl(); _time=8;};

    opcode[0x60]=[&]{reg_h=reg_b; _time=4;};
    opcode[0x61]=[&]{reg_h=reg_c; _time=4;};
    opcode[0x62]=[&]{reg_h=reg_d; _time=4;};
    opcode[0x63]=[&]{reg_h=reg_e; _time=4;};
    opcode[0x64]=[&]{_time=4;};
    opcode[0x65]=[&]{reg_h=reg_l; _time=4;};
    opcode[0x66]=[&]{reg_h=readhl(); _time=8;};

    opcode[0x68]=[&]{reg_l=reg_b; _time=4;};
    opcode[0x69]=[&]{reg_l=reg_c; _time=4;};
    opcode[0x6a]=[&]{reg_l=reg_d; _time=4;};
    opcode[0x6b]=[&]{reg_l=reg_e; _time=4;};
    opcode[0x6c]=[&]{reg_l=reg_h; _time=4;};
    opcode[0x6d]=[&]{_time=4;};
    opcode[0x6e]=[&]{reg_l=readhl(); _time=8;};

    opcode[0x70]=[&]{writehl(reg_b); _time=8;};
    opcode[0x71]=[&]{writehl(reg_c); _time=8;};
    opcode[0x72]=[&]{writehl(reg_d); _time=8;};
    opcode[0x73]=[&]{writehl(reg_e); _time=8;};
    opcode[0x74]=[&]{writehl(reg_h); _time=8;};
    opcode[0x75]=[&]{writehl(reg_l); _time=8;};
    opcode[0x36]=[&]{mem.wb(unsign_16(reg_h<<8)+reg_l,mem.rb(reg_pc)); reg_pc++; _time=12;};

    //LD A,n
    opcode[0x0a]=[&]{reg_a=mem.rb((reg_b<<8)+reg_c); _time=8;};
    opcode[0x1a]=[&]{reg_a=mem.rb((reg_d<<8)+reg_e); _time=8;};
    opcode[0xfa]=[&]{reg_a=mem.rw(reg_pc); reg_pc+=2; _time=16;};
    opcode[0x3e]=[&]{reg_a=mem.rb(reg_pc); reg_pc++; _time=8;};

    //LD n,A
    opcode[0x47]=[&]{reg_b=reg_a; _time=4;};
    opcode[0x4f]=[&]{reg_c=reg_a; _time=4;};
    opcode[0x57]=[&]{reg_d=reg_a; _time=4;};
    opcode[0x5f]=[&]{reg_e=reg_a; _time=4;};
    opcode[0x67]=[&]{reg_h=reg_a; _time=4;};
    opcode[0x6f]=[&]{reg_l=reg_a; _time=4;};
    opcode[0x02]=[&]{mem.wb(unsign_16(reg_b<<8)+reg_c,reg_a); _time=8;};
    opcode[0x12]=[&]{mem.wb(unsign_16(reg_d<<8)+reg_e,reg_a); _time=8;};
    opcode[0x77]=[&]{mem.wb(unsign_16(reg_h<<8)+reg_l,reg_a); _time=8;};
    opcode[0xea]=[&]{mem.wb(mem.rw(reg_pc),reg_a); reg_pc+=2; _time=16;};

    //LD A,(C)
    opcode[0xf2]=[&]{reg_a=mem.rb(0xff00+reg_c); _time=8;};

    //LD (C),A
    opcode[0xe2]=[&]{mem.wb(0xff00+reg_c,reg_a); _time=8;};

    //LDD A,(HL)
    opcode[0x3a]=[&]{reg_a=readhl(); if (!reg_l) {reg_l=255; reg_h--;} else reg_l--; _time=8;};
    
    //LDD (HL),A
    opcode[0x32]=[&]{writehl(reg_a); if (!reg_l) {reg_l=255; reg_h--;} else reg_l--; _time=8;};

    //LDI A,(HL)
    opcode[0x2a]=[&]{reg_a=readhl(); if(reg_l==255){reg_l=0; reg_h++;} else reg_l++; _time=8;};

    //LDI (HL),A
    opcode[0x22]=[&]{writehl(reg_a); if(reg_l==255){reg_l=0; reg_h++;} else reg_l++; _time=8;};

    //LDH (n),A
    opcode[0xf0]=[&]{reg_a=mem.rb(0xff00+mem.rb(reg_pc)); reg_pc++; _time=12;};

    
    //16-bit loads
    //LD n,nn
    opcode[0x01]=[&]{reg_c=mem.rb(reg_pc); reg_b=mem.rb(reg_pc+1); reg_pc+=2; _time=12;};
    opcode[0x11]=[&]{reg_d=mem.rb(reg_pc); reg_e=mem.rb(reg_pc+1); reg_pc+=2; _time=12;};
    opcode[0x21]=[&]{reg_h=mem.rb(reg_pc); reg_l=mem.rb(reg_pc+1); reg_pc+=2; _time=12;};
    opcode[0x31]=[&]{reg_sp=mem.rw(reg_pc); reg_pc+=2; _time=12;};

    //LD SP,HL
    opcode[0xf9]=[&]{reg_sp=unsign_16(reg_h<<8)+reg_l; _time=8;};

    //LDHL SP,n
    opcode[0xf8]=[&]{unsign_8 n=mem.rb(reg_pc); int x; if (n>127) x=-(~n+1);else x=n; reg_pc++; int value=x+reg_sp; reg_h=(value>>8); reg_l=value&255;
    zero_flag(0); subtract_flag(0); half_carry_flag((x&0xf)+(reg_sp&0xf)>15); carry_flag(value>255); _time=12;};

    //LD (nn),SP
    opcode[0x08]=[&]{mem.ww(mem.rw(reg_pc),reg_sp); reg_pc+=2; _time=20;};

    //PUSH nn
    opcode[0xf5]=[&]{reg_sp--; mem.wb(reg_sp,reg_a); reg_sp--; mem.wb(reg_sp,reg_f); _time=16;};
    opcode[0xc5]=[&]{reg_sp--; mem.wb(reg_sp,reg_b); reg_sp--; mem.wb(reg_sp,reg_c); _time=16;};
    opcode[0xd5]=[&]{reg_sp--; mem.wb(reg_sp,reg_d); reg_sp--; mem.wb(reg_sp,reg_e); _time=16;};
    opcode[0xe5]=[&]{reg_sp--; mem.wb(reg_sp,reg_h); reg_sp--; mem.wb(reg_sp,reg_l); _time=16;};

    //POP nn
    opcode[0xf1]=[&]{reg_f=mem.rb(reg_sp); reg_sp++; reg_a=mem.rb(reg_sp); reg_sp++; _time=12;};
    opcode[0xc1]=[&]{reg_c=mem.rb(reg_sp); reg_sp++; reg_b=mem.rb(reg_sp); reg_sp++; _time=12;};
    opcode[0xd1]=[&]{reg_e=mem.rb(reg_sp); reg_sp++; reg_d=mem.rb(reg_sp); reg_sp++; _time=12;};
    opcode[0xe1]=[&]{reg_l=mem.rb(reg_sp); reg_sp++; reg_h=mem.rb(reg_sp); reg_sp++; _time=12;};



    //8-bit ALU
    //ADD A,n
    opcode[0x87]=[&]{add(reg_a);};
    opcode[0x80]=[&]{add(reg_b);};
    opcode[0x81]=[&]{add(reg_c);};
    opcode[0x82]=[&]{add(reg_d);};
    opcode[0x83]=[&]{add(reg_e);};
    opcode[0x84]=[&]{add(reg_h);};
    opcode[0x85]=[&]{add(reg_l);};
    opcode[0x86]=[&]{unsign_8 value=readhl(); add(value); _time=8;};
    opcode[0xc6]=[&]{unsign_8 value=mem.rb(reg_pc); reg_pc++; add(value); _time=8;};

    //ADC A,n
    opcode[0x8f]=[&]{adc(reg_a);};
    opcode[0x88]=[&]{adc(reg_b);};
    opcode[0x89]=[&]{adc(reg_c);};
    opcode[0x8a]=[&]{adc(reg_d);};
    opcode[0x8b]=[&]{adc(reg_e);};
    opcode[0x8c]=[&]{adc(reg_h);};
    opcode[0x8d]=[&]{adc(reg_l);};
    opcode[0x8e]=[&]{unsign_8 tmp=readhl(); adc(tmp); _time=8;};
    opcode[0xce]=[&]{unsign_8 tmp=mem.rb(reg_pc); reg_pc++; adc(tmp); _time=8;};

    //SUB n
    opcode[0x97]=[&]{sub(reg_a);};
    opcode[0x90]=[&]{sub(reg_b);};
    opcode[0x91]=[&]{sub(reg_c);};
    opcode[0x92]=[&]{sub(reg_d);};
    opcode[0x93]=[&]{sub(reg_e);};
    opcode[0x94]=[&]{sub(reg_h);};
    opcode[0x95]=[&]{sub(reg_l);};
    opcode[0x96]=[&]{unsign_8 value=readhl(); sub(value); _time=8; };
    opcode[0xd6]=[&]{unsign_8 value=mem.rb(reg_pc); reg_pc++; sub(value); _time=8; };

    //SBC A,n
    opcode[0x9f]=[&]{sbc(reg_a);};
    opcode[0x98]=[&]{sbc(reg_b);};
    opcode[0x99]=[&]{sbc(reg_c);};
    opcode[0x9a]=[&]{sbc(reg_d);};
    opcode[0x9b]=[&]{sbc(reg_e);};
    opcode[0x9c]=[&]{sbc(reg_h);};
    opcode[0x9d]=[&]{sbc(reg_l);};
    opcode[0x9e]=[&]{unsign_8 value=readhl(); sbc(value); _time=8;};

    //AND n
    opcode[0xa7]=[&]{_and(reg_a);};
    opcode[0xa0]=[&]{_and(reg_b);};
    opcode[0xa1]=[&]{_and(reg_c);};
    opcode[0xa2]=[&]{_and(reg_d);};
    opcode[0xa3]=[&]{_and(reg_e);};
    opcode[0xa4]=[&]{_and(reg_h);};
    opcode[0xa5]=[&]{_and(reg_l);};
    opcode[0xa6]=[&]{unsign_8 value=readhl(); _and(value); _time=8;};
    opcode[0xe6]=[&]{unsign_8 value=mem.rb(reg_pc); reg_pc++; _and(value); _time=8;};

    //OR n
    opcode[0xb7]=[&]{_or(reg_a);};
    opcode[0xb0]=[&]{_or(reg_b);};
    opcode[0xb1]=[&]{_or(reg_c);};
    opcode[0xb2]=[&]{_or(reg_d);};
    opcode[0xb3]=[&]{_or(reg_e);};
    opcode[0xb4]=[&]{_or(reg_h);};
    opcode[0xb5]=[&]{_or(reg_l);};
    opcode[0xb6]=[&]{unsign_8 value=readhl(); _or(value); _time=8;};
    opcode[0xf6]=[&]{unsign_8 value=mem.rb(reg_pc); reg_pc++; _or(value); _time=8;};

    //XOR n
    opcode[0xaf]=[&]{_xor(reg_a);};
    opcode[0xa8]=[&]{_xor(reg_b);};
    opcode[0xa9]=[&]{_xor(reg_c);};
    opcode[0xaa]=[&]{_xor(reg_d);};
    opcode[0xab]=[&]{_xor(reg_e);};
    opcode[0xac]=[&]{_xor(reg_h);};
    opcode[0xad]=[&]{_xor(reg_l);};
    opcode[0xae]=[&]{unsign_8 value=readhl(); _xor(value); _time=8;};
    opcode[0xee]=[&]{unsign_8 value=mem.rb(reg_pc); reg_pc++; _xor(value); _time=8;};

    //CP n
    opcode[0xbf]=[&]{cp(reg_a);};
    opcode[0xb8]=[&]{cp(reg_b);};
    opcode[0xb9]=[&]{cp(reg_c);};
    opcode[0xba]=[&]{cp(reg_d);};
    opcode[0xbb]=[&]{cp(reg_e);};
    opcode[0xbc]=[&]{cp(reg_h);};
    opcode[0xbd]=[&]{cp(reg_l);};
    opcode[0xbe]=[&]{unsign_8 value=readhl(); cp(value); _time=8;};
    opcode[0xfe]=[&]{unsign_8 value=mem.rb(reg_pc); reg_pc++; cp(value); _time=8;};

    //INC n
    opcode[0x3c]=[&]{inc(reg_a);};
    opcode[0x04]=[&]{inc(reg_b);};
    opcode[0x0c]=[&]{inc(reg_c);};
    opcode[0x14]=[&]{inc(reg_d);};
    opcode[0x1c]=[&]{inc(reg_e);};
    opcode[0x24]=[&]{inc(reg_h);};
    opcode[0x2c]=[&]{inc(reg_l);};
    opcode[0x34]=[&]{unsign_8 value=readhl(); writehl(value+1); inc(value); _time=12;};

    //DEC n
    opcode[0x3d]=[&]{dec(reg_a);};
    opcode[0x05]=[&]{dec(reg_b);};
    opcode[0x0d]=[&]{dec(reg_c);};
    opcode[0x15]=[&]{dec(reg_d);};
    opcode[0x1d]=[&]{dec(reg_e);};
    opcode[0x25]=[&]{dec(reg_h);};
    opcode[0x2d]=[&]{dec(reg_l);};
    opcode[0x35]=[&]{unsign_8 value=readhl(); writehl(value-1); dec(value); _time=12;};

    //16-bit Arithmetic
    //ADD HL,n
    opcode[0x09]=[&]{unsign_16 bc=mem.rw(unsign_16(reg_b<<8)+reg_c); add_hl(bc);}; 
    opcode[0x19]=[&]{unsign_16 de=mem.rw(unsign_16(reg_d<<8)+reg_e); add_hl(de);}; 
    opcode[0x29]=[&]{unsign_16 bc=mem.rw(unsign_16(reg_h<<8)+reg_l); add_hl(bc);}; 
    opcode[0x39]=[&]{unsign_16 sp=mem.rw(reg_sp); reg_sp+=2; add_hl(sp);};

    //ADD SP,n
    opcode[0xe8]=[&]{unsign_8 n=mem.rb(reg_pc); reg_pc++; int x; if (n>127) x=-(~n+1);else x=n;
    zero_flag(0);subtract_flag(0);half_carry_flag((0xfff&x)+(0xfff&reg_sp)>4095);carry_flag(x+reg_sp>65535);};

    //INC nn
    opcode[0x03]=[&]{inc_16(reg_b,reg_c);};
    opcode[0x13]=[&]{inc_16(reg_d,reg_e);};
    opcode[0x23]=[&]{inc_16(reg_h,reg_l);};
    opcode[0x33]=[&]{reg_sp++;};

    //DEC nn
    opcode[0x0b]=[&]{dec_16(reg_b,reg_c);};
    opcode[0x1b]=[&]{dec_16(reg_d,reg_e);};
    opcode[0x2b]=[&]{dec_16(reg_h,reg_l);};
    opcode[0x3b]=[&]{reg_sp--;};

    //Miscellaneous
    //SWAP n
    cb_opcode[0x37]=[&]{swap_n(reg_a);};
    cb_opcode[0x30]=[&]{swap_n(reg_b);};
    cb_opcode[0x31]=[&]{swap_n(reg_c);};
    cb_opcode[0x32]=[&]{swap_n(reg_d);};
    cb_opcode[0x33]=[&]{swap_n(reg_e);};
    cb_opcode[0x34]=[&]{swap_n(reg_h);};
    cb_opcode[0x35]=[&]{swap_n(reg_l);};
    cb_opcode[0x36]=[&]{unsign_8 tmp=readhl();swap_n(tmp);mem.wb((reg_h<<8)+reg_l,tmp);_time=16;};

    //DAA
    opcode[0x27]=[&]{
        unsign_8 tmp=0;
        if (reg_f&32||(reg_a&0xf)>9)
        tmp+=0x06;
        if (reg_f&16||(reg_a>>4)>9)
        tmp+=0x60;
        if (reg_f&64)
        reg_a+=tmp;
        else reg_a-=tmp;
        zero_flag(reg_a==0);
        half_carry_flag(0);
        carry_flag(((int)tmp<<2)&0x100);
        _time=4;
    };

    //CPL
    opcode[0x2f]=[&]{reg_a=~reg_a; subtract_flag(1); half_carry_flag(1); _time=4;};

    //CCF
    opcode[0x3f]=[&]{subtract_flag(0); half_carry_flag(0); carry_flag((reg_f&16)==0); _time=4;};

    //SCF
    opcode[0x37]=[&]{subtract_flag(0); half_carry_flag(0); carry_flag(1); _time=4;};

    //NOP
    opcode[0x00]=[&]{_time=4;};

    //HALT
    opcode[0x76]=[&]{halt=1; _time=4;};

    //STOP
    opcode[0x10]=[&]{halt=1; _time=4;};

    //DI
    opcode[0xf3]=[&]{master_enable=0; _time=4;};

    //EI
    opcode[0xfb]=[&]{master_enable=1; _time=4;};

    //ROTATES & SHIFTS
    //RLCA
    opcode[0x07]=[&]{rlc(reg_a);};

    //RLA
    opcode[0x17]=[&]{rl(reg_a);};

    //RRCA
    opcode[0x0f]=[&]{rrc(reg_a);};

    //RRA
    opcode[0x1f]=[&]{rr(reg_a);};

    //RLC n
    cb_opcode[0x07]=[&]{rlc(reg_a); _time=8;};
    cb_opcode[0x00]=[&]{rlc(reg_b); _time=8;};
    cb_opcode[0x01]=[&]{rlc(reg_c); _time=8;};
    cb_opcode[0x02]=[&]{rlc(reg_d); _time=8;};
    cb_opcode[0x03]=[&]{rlc(reg_e); _time=8;};
    cb_opcode[0x04]=[&]{rlc(reg_h); _time=8;};
    cb_opcode[0x05]=[&]{rlc(reg_l); _time=8;};
    cb_opcode[0x06]=[&]{unsign_8 tmp=readhl(); rlc(tmp); writehl(tmp); _time=16;};

    //RL n
    cb_opcode[0x17]=[&]{rl(reg_a); _time=8;};
    cb_opcode[0x10]=[&]{rl(reg_b); _time=8;};
    cb_opcode[0x11]=[&]{rl(reg_c); _time=8;};
    cb_opcode[0x12]=[&]{rl(reg_d); _time=8;};
    cb_opcode[0x13]=[&]{rl(reg_e); _time=8;};
    cb_opcode[0x14]=[&]{rl(reg_h); _time=8;};
    cb_opcode[0x15]=[&]{rl(reg_l); _time=8;};
    cb_opcode[0x16]=[&]{unsign_8 tmp=readhl(); rl(tmp); writehl(tmp); _time=16;};

    //RRC n
    cb_opcode[0x0f]=[&]{rrc(reg_a); _time=8;};
    cb_opcode[0x08]=[&]{rrc(reg_b); _time=8;};
    cb_opcode[0x09]=[&]{rrc(reg_c); _time=8;};
    cb_opcode[0x0a]=[&]{rrc(reg_d); _time=8;};
    cb_opcode[0x0b]=[&]{rrc(reg_e); _time=8;};
    cb_opcode[0x0c]=[&]{rrc(reg_h); _time=8;};
    cb_opcode[0x0d]=[&]{rrc(reg_l); _time=8;};
    cb_opcode[0x0e]=[&]{unsign_8 tmp=readhl(); rrc(tmp); writehl(tmp); _time=16;};

    //RR n
    cb_opcode[0x1f]=[&]{rr(reg_a); _time=8;};
    cb_opcode[0x18]=[&]{rr(reg_b); _time=8;};
    cb_opcode[0x19]=[&]{rr(reg_c); _time=8;};
    cb_opcode[0x1a]=[&]{rr(reg_d); _time=8;};
    cb_opcode[0x1b]=[&]{rr(reg_e); _time=8;};
    cb_opcode[0x1c]=[&]{rr(reg_h); _time=8;};
    cb_opcode[0x1d]=[&]{rr(reg_l); _time=8;};
    cb_opcode[0x1e]=[&]{unsign_8 tmp=readhl(); rr(tmp); writehl(tmp); _time=16;};

    //SLA n
    cb_opcode[0x27]=[&]{sla(reg_a);};
    cb_opcode[0x20]=[&]{sla(reg_b);};
    cb_opcode[0x21]=[&]{sla(reg_c);};
    cb_opcode[0x22]=[&]{sla(reg_d);};
    cb_opcode[0x23]=[&]{sla(reg_e);};
    cb_opcode[0x24]=[&]{sla(reg_h);};
    cb_opcode[0x25]=[&]{sla(reg_l);};
    cb_opcode[0x26]=[&]{unsign_8 tmp=readhl(); sla(tmp); writehl(tmp); _time=16;};

    //SRA n
    cb_opcode[0x2f]=[&]{sra(reg_a);};
    cb_opcode[0x28]=[&]{sra(reg_b);};
    cb_opcode[0x29]=[&]{sra(reg_c);};
    cb_opcode[0x2a]=[&]{sra(reg_d);};
    cb_opcode[0x2b]=[&]{sra(reg_e);};
    cb_opcode[0x2c]=[&]{sra(reg_h);};
    cb_opcode[0x2d]=[&]{sra(reg_l);};
    cb_opcode[0x23]=[&]{unsign_8 tmp=readhl(); sra(tmp); writehl(tmp); _time=16;};

    //SRL n
    cb_opcode[0x3f]=[&]{srl(reg_a);};
    cb_opcode[0x38]=[&]{srl(reg_b);};
    cb_opcode[0x39]=[&]{srl(reg_c);};
    cb_opcode[0x3a]=[&]{srl(reg_d);};
    cb_opcode[0x3b]=[&]{srl(reg_e);};
    cb_opcode[0x3c]=[&]{srl(reg_h);};
    cb_opcode[0x3d]=[&]{srl(reg_l);};
    cb_opcode[0x3e]=[&]{unsign_8 tmp=readhl(); srl(tmp); writehl(tmp); _time=16;};

    //Bit Opcodes
    //BIT b,r
    for(int i=0; i<8; i++)
    {
        cb_opcode[0x40+i*8]=[&]{bit(i,reg_b);};
        cb_opcode[0x41+i*8]=[&]{bit(i,reg_c);};
        cb_opcode[0x42+i*8]=[&]{bit(i,reg_d);};
        cb_opcode[0x43+i*8]=[&]{bit(i,reg_e);};
        cb_opcode[0x44+i*8]=[&]{bit(i,reg_h);};
        cb_opcode[0x45+i*8]=[&]{bit(i,reg_l);};
        cb_opcode[0x46+i*8]=[&]{unsign_8 tmp=readhl();bit(i,tmp);_time=16;};
        cb_opcode[0x47+i*8]=[&]{bit(i,reg_a);};

    }

    //SET b,r
    for(int i=0; i<8; i++)
    {
        cb_opcode[0xc0+i*8]=[&]{set(i,reg_b);};
        cb_opcode[0xc1+i*8]=[&]{set(i,reg_c);};
        cb_opcode[0xc2+i*8]=[&]{set(i,reg_d);};
        cb_opcode[0xc3+i*8]=[&]{set(i,reg_e);};
        cb_opcode[0xc4+i*8]=[&]{set(i,reg_h);};
        cb_opcode[0xc5+i*8]=[&]{set(i,reg_l);};
        cb_opcode[0xc6+i*8]=[&]{unsign_8 tmp=readhl(); set(i,tmp);_time=16;};
        cb_opcode[0xc7+i*8]=[&]{set(i,reg_a);};
    }

    //RES b,r
    for(int i=0; i<8 ;i++)
    {
        cb_opcode[0x80+i*8]=[&]{res(i,reg_b);};
        cb_opcode[0x81+i*8]=[&]{res(i,reg_c);};
        cb_opcode[0x82+i*8]=[&]{res(i,reg_d);};
        cb_opcode[0x83+i*8]=[&]{res(i,reg_e);};
        cb_opcode[0x84+i*8]=[&]{res(i,reg_h);};
        cb_opcode[0x85+i*8]=[&]{res(i,reg_l);};
        cb_opcode[0x86+i*8]=[&]{unsign_8 tmp=readhl(); res(i,tmp);_time=16;};
        cb_opcode[0x87+i*8]=[&]{res(i,reg_a);};
    }

    //Jumps
    //JP nn
    opcode[0xc3]=[&]{jp();};

    //JP cc,nn
    opcode[0xc2]=[&]{if ((reg_f&128)==0) jp();};
    opcode[0xca]=[&]{if (reg_f&128) jp();};
    opcode[0xd2]=[&]{if ((reg_f&16)==0) jp();};
    opcode[0xda]=[&]{if (reg_f&16) jp();};

    //JP (hl)
    opcode[0xe9]=[&]{reg_pc=unsign_16(reg_h<<8)|reg_l; _time=4;};

    //JR n
    opcode[0x18]=[&]{jr();};

    //JR cc,n
    opcode[0x20]=[&]{if ((reg_f&128)==0) jr();};
    opcode[0x28]=[&]{if (reg_f&128) jr();};
    opcode[0x30]=[&]{if ((reg_f&16)==0) jr();};
    opcode[0x38]=[&]{if (reg_f&16) jr();};

    //Calls
    //CALL nn
    opcode[0xcd]=[&]{call();};

    //CALL cc,nn
    opcode[0xc4]=[&]{if ((reg_f&128)==0) call();};
    opcode[0xcc]=[&]{if (reg_f&128) call();};
    opcode[0xd4]=[&]{if ((reg_f&16)==0) call();};
    opcode[0xdc]=[&]{if (reg_f&16) call();};

    //Restarts
    //RST n
    opcode[0xc7]=[&]{rst(0x00);};
    opcode[0xcf]=[&]{rst(0x08);};
    opcode[0xd7]=[&]{rst(0x10);};
    opcode[0xdf]=[&]{rst(0x18);};
    opcode[0xe7]=[&]{rst(0x20);};
    opcode[0xef]=[&]{rst(0x28);};
    opcode[0xf7]=[&]{rst(0x30);};
    opcode[0xff]=[&]{rst(0x38);};

    //Returns
    //RET
    opcode[0xc9]=[&]{ret();};

    //RET cc
    opcode[0xc0]=[&]{if ((reg_f&128)==0) ret();};
    opcode[0xc8]=[&]{if (reg_f&128) ret();};
    opcode[0xd0]=[&]{if ((reg_f&16)==0) ret();};
    opcode[0xd8]=[&]{if (reg_f&16) ret();};

    //RETI
    opcode[0xd9]=[&]{ret(); master_enable=1; _time=8;};
}
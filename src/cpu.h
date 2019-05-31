#pragma once

#include <functional>

typedef unsigned char unsign_8;
typedef unsigned short unsign_16;

extern Mem mem;

class Cpu
{
public:
    void init();  
    unsign_8 reg_a,reg_b,reg_c,reg_d,reg_e,reg_h,reg_l,reg_f;
    unsign_16 reg_sp,reg_pc;
    unsign_8 time;
    bool halt,master_enable;
    unsign_16 clocktime;
    std::function<void()> opcode[0xff],cb_opcode[0xff];    
private:   
    void reset();
    void zero_flag(bool flag);
    void subtract_flag(bool flag);
    void half_carry_flag(bool flag);
    void carry_flag(bool flag);
    void add(unsign_8 n);
    void adc(unsign_8 n);
    void sub(unsign_8 n);
    void sbc(unsign_8 n);
    void _and(unsign_8 n);
    void _or(unsign_8 n);
    void _xor(unsign_8 n);
    void cp(unsign_8 n);
    void inc(unsign_8 &n);
    void dec(unsign_8 &n);
    void add_hl(unsign_16 n);
    void inc_16(unsign_8 &n,unsign_8 &m);
    void dec_16(unsign_8 &n,unsign_8 &m);
    void swap_n(unsign_8 &n);
    void rlc(unsign_8 &n);
    void rl(unsign_8 &n);
    void rrc(unsign_8 &n);
    void rr(unsign_8 &n);
    void sla(unsign_8 &n);
    void sra(unsign_8 &n);
    void srl(unsign_8 &n);
    void bit(int b, unsign_8 n);
    void set(int b,unsign_8 &n);
    void res(int b,unsign_8 &n);
    void jp();
    void jr();
    void call();
    void rst(unsign_8 n);
    void ret();
    unsign_8 readhl();
    void writehl(unsign_8 n);  
};







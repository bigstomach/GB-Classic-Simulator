#include "mem.h"
#include "cpu.h"
#include "gpu.h"

static Cpu cpu;

//GPU frame timings
void Gpu::timing()
{
    gpu_clock+=cpu.time;
}


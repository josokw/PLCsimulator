#ifndef TIMER_H
#define TIMER_H

#include <vector>

class Memory;

/// Timer, if a timer is enabled, every tick will increment TN.
/// All status regs and vars of timer are memory mapped.
class Timer
{
public:
   Timer();

   /// Set timer id (0..N)
   void setID(int ID) { id = ID; }
   /// Maps timer to memory and initialises all internal registers.
   void mapToMemory(Memory& mem, int offset);
   /// Timer tick.
   void tick();

private:
   Memory* memory;
   int id;
   int offset;
   int TEnable;
   int TValue;
   int TN;
   int TS;
   int TC;
};

#endif // TIMER_H

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
   virtual ~Timer() = default;
   /// Set timer id (0..N)
   void setID(int ID) { _id = ID; }
   /// Maps timer to memory and initialises all internal registers.
   void mapToMemory(Memory& mem, int _offset);
   /// Timer tick.
   void tick();
private:
   Memory* _pMemory;
   int _id;
   int _offset;
   int _TEnable;
   int _TValue;
   int _TN;
   int _TS;
   int _TC;
};

#endif // TIMER_H

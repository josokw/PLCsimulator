#ifndef TIMER_H
#define TIMER_H

#include <cstdint>
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
   void setID(int32_t ID) { _id = ID; }
   /// Maps timer to memory and initialises all internal registers.
   void mapToMemory(Memory& mem, int32_t _offset);
   /// Timer tick.
   void tick();
private:
   Memory* _pMemory;
   int32_t _id;
   int32_t _offset;
   int32_t _TEnable;
   int32_t _TValue;
   int32_t _TN;
   int32_t _TS;
   int32_t _TC;
};

#endif // TIMER_H

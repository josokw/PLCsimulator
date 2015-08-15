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
   Timer() = default;
   Timer(Memory& memory, int id);
   Timer(const Timer& other) = default;
   Timer& operator=(const Timer& other) = default;
   ~Timer() = default;
   /// Maps timer to memory and initialises all internal registers.
   void mapToMemory() const;
   /// Timer tick.
   void tick() const;
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

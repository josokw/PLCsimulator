#include "Timer.h"
#include "FunctionConfig.h"
#include "Memory.h"
#include "MemoryConfig.h"
#include "ProcessorConfig.h"

#include <iostream>

using namespace std;

Timer::Timer()
:  memory(0)
,  id(0)
{
}

void Timer::mapToMemory(Memory& mem, int offst)
{
   memory = &mem;
   offset = offst;

   //TEnable = MemoryConfig::TIMERS + id * MemoryConfig::TIMER_SIZE + FunctionConfig::TEnable;
   TEnable = offset + FunctionConfig::TEnable;
   TValue = offset + FunctionConfig::TValue;
   TN = offset + FunctionConfig::TN;
   TS = offset + FunctionConfig::TS;
   TC = offset + FunctionConfig::TC;
   (*memory)[TEnable].actual = false;
   (*memory)[TEnable].previous = false;
   (*memory)[TValue].integer = 0;
   (*memory)[TN].integer = 0;
   (*memory)[TS].actual = false;
   (*memory)[TC].actual = true;
   (*memory)[TC].previous = true;
}

void Timer::tick()
{
   if ((*memory)[TEnable].actual)
   {
      cerr << "[Timer" << id << "] tick(): "
            << (*memory)[TC].previous << " "
            << (*memory)[TC].actual << " "
            << (*memory)[TEnable].actual << " "
            << (*memory)[TN].integer << " "
            << (*memory)[TValue].integer << " "
            << (*memory)[TS].actual << endl;
   }

   if ((*memory)[TC].actual == false) {
      cerr << "RESET\n";
      (*memory)[TC].set(true);
      (*memory)[TN].integer = 0;
      (*memory)[TS].actual = false;
   }
   else {
   if ((*memory)[TEnable].actual) {
      ++((*memory)[TN].integer);
      if (((*memory)[TN].integer) > ((*memory)[TValue].integer)) {
         (*memory)[TN].integer = (*memory)[TValue].integer;
         (*memory)[TS].actual = true;
      }
   }
   else {
      (*memory)[TN].integer = 0;
      (*memory)[TS].actual = false;
   }
   }
}

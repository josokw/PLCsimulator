#include "Timer.h"
#include "FunctionConfig.h"
#include "Memory.h"
#include "MemoryConfig.h"
#include "ProcessorConfig.h"

#include <iostream>

using namespace std;

Timer::Timer()
:  _pMemory(0)
,  _id(0)
{
}

void Timer::mapToMemory(Memory& mem, int offst)
{
   _pMemory = &mem;
   _offset = offst;

   //TEnable = MemoryConfig::TIMERS + id * MemoryConfig::TIMER_SIZE + FunctionConfig::TEnable;
   _TEnable = _offset + FunctionConfig::TEnable;
   _TValue = _offset + FunctionConfig::TValue;
   _TN = _offset + FunctionConfig::TN;
   _TS = _offset + FunctionConfig::TS;
   _TC = _offset + FunctionConfig::TC;
   (*_pMemory)[_TEnable].actual = false;
   (*_pMemory)[_TEnable].previous = false;
   (*_pMemory)[_TValue].integer = 0;
   (*_pMemory)[_TN].integer = 0;
   (*_pMemory)[_TS].actual = false;
   (*_pMemory)[_TC].actual = true;
   (*_pMemory)[_TC].previous = true;
}

void Timer::tick()
{
   if ((*_pMemory)[_TEnable].actual)
   {
      cerr << "[Timer" << _id << "] tick(): "
            << (*_pMemory)[_TC].previous << " "
            << (*_pMemory)[_TC].actual << " "
            << (*_pMemory)[_TEnable].actual << " "
            << (*_pMemory)[_TN].integer << " "
            << (*_pMemory)[_TValue].integer << " "
            << (*_pMemory)[_TS].actual << endl;
   }

   if ((*_pMemory)[_TC].actual == false) {
      cerr << "RESET\n";
      (*_pMemory)[_TC].set(true);
      (*_pMemory)[_TN].integer = 0;
      (*_pMemory)[_TS].actual = false;
   }
   else {
   if ((*_pMemory)[_TEnable].actual) {
      ++((*_pMemory)[_TN].integer);
      if (((*_pMemory)[_TN].integer) > ((*_pMemory)[_TValue].integer)) {
         (*_pMemory)[_TN].integer = (*_pMemory)[_TValue].integer;
         (*_pMemory)[_TS].actual = true;
      }
   }
   else {
      (*_pMemory)[_TN].integer = 0;
      (*_pMemory)[_TS].actual = false;
   }
   }
}

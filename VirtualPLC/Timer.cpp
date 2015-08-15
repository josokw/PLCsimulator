#include "Timer.h"
#include "FunctionConfig.h"
#include "Memory.h"
#include "MemoryConfig.h"
#include "ProcessorConfig.h"

#include <iostream>

using namespace std;

Timer::Timer(Memory &memory, int id)
  :  _pMemory{&memory}
  ,  _id{id}
  ,  _TEnable{MemoryConfig::TIMERS
              + id * MemoryConfig::TIMER_SIZE + FunctionConfig::TEnable}
  ,  _TValue{MemoryConfig::TIMERS
             + id * MemoryConfig::TIMER_SIZE + FunctionConfig::TValue}
  ,  _TN{MemoryConfig::TIMERS
         + id * MemoryConfig::TIMER_SIZE + FunctionConfig::TN}
  ,  _TS{MemoryConfig::TIMERS
         + id * MemoryConfig::TIMER_SIZE + FunctionConfig::TS}
  ,  _TC{MemoryConfig::TIMERS
         + id * MemoryConfig::TIMER_SIZE + FunctionConfig::TC}
{}

void Timer::mapToMemory() const
{
   (*_pMemory)[_TEnable].actual = false;
   (*_pMemory)[_TEnable].previous = false;
   (*_pMemory)[_TValue].integer = 0;
   (*_pMemory)[_TN].integer = 0;
   (*_pMemory)[_TS].actual = false;
   (*_pMemory)[_TC].actual = true;
   (*_pMemory)[_TC].previous = true;
}

void Timer::tick() const
{
   if ((*_pMemory)[_TEnable].actual) {
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

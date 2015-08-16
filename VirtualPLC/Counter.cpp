#include "Counter.h"
#include "FunctionConfig.h"
#include "Memory.h"
#include <algorithm>

using namespace std;

Counter::Counter(Memory& memory, int id)
  :  _pMemory{&memory}
  ,  _id{id}
  ,  _CEnable{MemoryConfig::COUNTERS
              + _id * MemoryConfig::COUNTER_SIZE + FunctionConfig::CEnable}
  ,  _CValue{MemoryConfig::COUNTERS
             + _id * MemoryConfig::COUNTER_SIZE + FunctionConfig::CValue}
  ,   _CN{MemoryConfig::COUNTERS
          + _id * MemoryConfig::COUNTER_SIZE + FunctionConfig::CN}
  ,   _CS{MemoryConfig::COUNTERS
          + _id * MemoryConfig::COUNTER_SIZE + FunctionConfig::CS}
  ,   _CC{MemoryConfig::COUNTERS
          + _id * MemoryConfig::COUNTER_SIZE + FunctionConfig::CS}
{}

void Counter::mapToMemory() const
{
   (*_pMemory)[_CEnable].actual = false;
   (*_pMemory)[_CEnable].previous = false;
   (*_pMemory)[_CValue].integer = 0;
   (*_pMemory)[_CN].integer = 0;
   (*_pMemory)[_CS].actual = false;
   (*_pMemory)[_CC].actual = false;
   (*_pMemory)[_CC].previous = true;
}

void Counter::checkReset() const
{
   if ((*_pMemory)[_CC].actual == false) {
      (*_pMemory)[_CN].integer = 0;
      (*_pMemory)[_CC].set(true);
   }
}

void Counter::checkIncrement() const
{
   if ((*_pMemory)[_CEnable].risingEdge()) {
      ++((*_pMemory)[_CN].integer);
      (*_pMemory)[_CEnable].set(false);
   }
   (*_pMemory)[_CS].set((*_pMemory)[_CN].integer
                        >= (*_pMemory)[_CValue].integer);
}

void Counter::clear() const
{
  copy(&(*_pMemory)[MemoryConfig::COUNTERS_INIT]
      + _id * MemoryConfig::COUNTER_SIZE,
       &(*_pMemory)[MemoryConfig::COUNTERS_INIT
       + (_id + 1) * MemoryConfig::COUNTER_SIZE],
       &(*_pMemory)[MemoryConfig::COUNTERS_INIT]
      + _id * MemoryConfig::COUNTER_SIZE);
}

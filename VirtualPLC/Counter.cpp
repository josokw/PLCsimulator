#include "Counter.h"
#include "FunctionConfig.h"
#include "Memory.h"

Counter::Counter()
  :  _memory{nullptr}
  ,  _id{0}
{
}

void Counter::mapToMemory(Memory& mem, int offst)
{
   _memory = &mem;
   _offset = offst;
   _CEnable = _offset + FunctionConfig::CEnable;
   _CValue = _offset + FunctionConfig::CValue;
   _CN = _offset + FunctionConfig::CN;
   _CS = _offset + FunctionConfig::CS;
   _CC = _offset + FunctionConfig::CC;
   (*_memory)[_CEnable].actual = false;
   (*_memory)[_CEnable].previous = false;
   (*_memory)[_CValue].integer = 0;
   (*_memory)[_CN].integer = 0;
   (*_memory)[_CS].actual = false;
   (*_memory)[_CC].actual = false;
   (*_memory)[_CC].previous = true;
}

void Counter::checkReset()
{
   if ((*_memory)[_CC].actual == false)
   {
      (*_memory)[_CN].integer = 0;
      (*_memory)[_CC].set(true);
   }
}

void Counter::checkIncrement()
{
   if ((*_memory)[_CEnable].risingEdge())
   {
      ++((*_memory)[_CN].integer);
      (*_memory)[_CEnable].set(false);
   }
   (*_memory)[_CS].set((*_memory)[_CN].integer >= (*_memory)[_CValue].integer);
}

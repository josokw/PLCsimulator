#include "Counter.h"
#include "FunctionConfig.h"
#include "Memory.h"

Counter::Counter()
  :  _pMemory{nullptr}
  ,  _id{0}
{
}

void Counter::mapToMemory(Memory& mem, int32_t offst)
{
   _pMemory = &mem;
   _offset = offst;
   _CEnable = _offset + FunctionConfig::CEnable;
   _CValue = _offset + FunctionConfig::CValue;
   _CN = _offset + FunctionConfig::CN;
   _CS = _offset + FunctionConfig::CS;
   _CC = _offset + FunctionConfig::CC;
   (*_pMemory)[_CEnable].actual = false;
   (*_pMemory)[_CEnable].previous = false;
   (*_pMemory)[_CValue].integer = 0;
   (*_pMemory)[_CN].integer = 0;
   (*_pMemory)[_CS].actual = false;
   (*_pMemory)[_CC].actual = false;
   (*_pMemory)[_CC].previous = true;
}

void Counter::checkReset()
{
   if ((*_pMemory)[_CC].actual == false)
   {
      (*_pMemory)[_CN].integer = 0;
      (*_pMemory)[_CC].set(true);
   }
}

void Counter::checkIncrement()
{
   if ((*_pMemory)[_CEnable].risingEdge())
   {
      ++((*_pMemory)[_CN].integer);
      (*_pMemory)[_CEnable].set(false);
   }
   (*_pMemory)[_CS].set((*_pMemory)[_CN].integer >= (*_pMemory)[_CValue].integer);
}

#include <iomanip>
#include <iostream>
#include <string>

#include "LibInfoVPLC.h"
#include "Memory.h"
#include "MemoryConfig.h"

using namespace std;

Memory::Memory()
  :  _outOfMemory{false}
  ,  _memory{0}
  ,  _constAddress{MemoryConfig::CONSTANTS}
  ,  _varAddress{MemoryConfig::VARS}
  ,  _tmpvarAddress{MemoryConfig::TMPVARS}
  ,  _codeAddress{MemoryConfig::ENTRYPOINT}
{
  clear();
}

void Memory::clear()
{
  for (auto& m: _memory)
  {
    m.integer = 0;
  }
  _constAddress = MemoryConfig::CONSTANTS;
  _varAddress = MemoryConfig::VARS;
  _tmpvarAddress = MemoryConfig::TMPVARS;
  _codeAddress = MemoryConfig::ENTRYPOINT;
  addNextCONST(stoi(vplc::libMajorVersion));
  addNextCONST(stoi(vplc::libMinorVersion));
  addNextCONST(stoi(vplc::libRevisionVersion));
}

void Memory::addNextCONST(int value)
{
  _memory[_constAddress++].integer = value;
}

void Memory::addNextCONST(bool value)
{
  _memory[_constAddress].actual = value;
  _memory[_constAddress++].previous = value;
}

void Memory::addNextVAR(int value)
{
  _memory[_varAddress++].integer = value;
}

void Memory::addNextVAR(bool value)
{
  _memory[_varAddress].actual = value;
  _memory[_varAddress++].previous = value;
}

void Memory::addNextTMPVAR(bool value)
{
  _memory[_varAddress].actual = value;
  _memory[_varAddress++].previous = value;
}

void Memory::addNextTMPVAR(int value)
{
  _memory[_tmpvarAddress++].integer = value;
}

void Memory::addNextCODE(int value)
{
  _memory[_codeAddress++].integer = value;
}

data_t Memory::operator[](int index) const
{
  return _memory.at(index);
}

data_t& Memory::operator[](int index)
{
  return _memory.at(index);
}

void Memory::dump(ostream& os) const
{
  int index = 0;

  for (int i = 0; i < MemoryConfig::COUNTERS; ++i)
  {
    if (10 == index)
    {
      os << endl;
      index = 0;
    }
    os << setw(10) << _memory[i].integer;
    ++index;
  }
  os << endl << endl;
  index = 0;
  for (int i = MemoryConfig::COUNTERS; i < MemoryConfig::TIMERS; ++i)
  {
    if (MemoryConfig::COUNTER_SIZE == index)
    {
      os << endl;
      index = 0;
    }
    os << setw(10) << _memory[i].integer;
    ++index;
  }
  os << endl << endl;
  index = 0;
  for (int i = MemoryConfig::TIMERS; i < MemoryConfig::OUTPUT_Y_LATCH; ++i)
  {
    if (MemoryConfig::TIMER_SIZE == index)
    {
      os << endl;
      index = 0;
    }
    os << setw(10) << _memory[i].integer;
    ++index;
  }
  os << endl << endl;
  os << setw(10) << _memory[MemoryConfig::OUTPUT_Y_LATCH].integer;
  os << setw(10) << _memory[MemoryConfig::INPUT_X_LATCH].integer;
  os << setw(10) << _memory[MemoryConfig::OUTPUT_Y].integer;
  os << setw(10) << _memory[MemoryConfig::INPUT_X].integer;
  os << endl;
}

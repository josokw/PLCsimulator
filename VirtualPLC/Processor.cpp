#include "Processor.h"
#include "Debug.h"
#include "Counter.h"
#include "MemoryConfig.h"
#include "ProcessorConfig.h"

#include <algorithm>
#include <bitset>
#include <stdexcept>
#include <iostream>
#include <limits>

using namespace std;

Processor::Processor(Memory &memory,
                     timers_t &timers,
                     counters_t &counters)
  :  _memory(memory)
  ,  _timers(timers)
  ,  _counters(counters)
  ,  _pConfig{new ProcessorConfig}
  ,  _PC{MemoryConfig::ENTRYPOINT}
  ,  _IR{0}
  ,  _SP{MemoryConfig::STACK}
  ,  _SR{0}
  ,  _instructionLUT()
  ,  _endOfProgramLoop{false}
{
  clearMemory();
  fillInstructionLUT();
}

void Processor::run()
{
  try {
    _SR = 0;
    while (!_endOfProgramLoop && _SR == 0)
    {
      if (decode())
      {
        execute();
      }
    }
    _PC = MemoryConfig::ENTRYPOINT;
    _SP = MemoryConfig::STACK;
    _endOfProgramLoop = false;
  }
  catch(std::exception& x)
  {
    clog << "[Processor] ERROR " << x.what() << endl;
  }
  catch(...)
  {
    clog << "[Processor] CRASH" << endl;
  }
}

void Processor::clearMemory()
{
  _memory.clear();
}

void Processor::loadMemory(const Memory& mem)
{
  for (size_t i = 0; i < mem.size(); ++i)
  {
    _memory[i] = mem[i];
  }
}

void Processor::fillInstructionLUT()
{
  for (size_t i = 0; i < _instructionLUT.size(); ++i)
  {
    addToInstructionLUT("NOTDEF", &Processor::NOTDEF);
  }
  addToInstructionLUT("LATCH", &Processor::LATCH);
  addToInstructionLUT("RESTORE", &Processor::RESTORE);
  addToInstructionLUT("INC", &Processor::INC);
  addToInstructionLUT("DEC", &Processor::DEC);
  addToInstructionLUT("AND", &Processor::AND);
  addToInstructionLUT("OR", &Processor::OR);
  addToInstructionLUT("NOT", &Processor::NOT);
  addToInstructionLUT("SCNM", &Processor::SCNM);
  addToInstructionLUT("SCNG", &Processor::SCNG);
  addToInstructionLUT("SCST", &Processor::SCST);
  addToInstructionLUT("SCRS", &Processor::SCRS);
  addToInstructionLUT("CPY", &Processor::CPY);
  addToInstructionLUT("SPUSH", &Processor::SPUSH);
  addToInstructionLUT("SPOP", &Processor::SPOP);
  addToInstructionLUT("SPOPB", &Processor::SPOPB);
  addToInstructionLUT("SDUP", &Processor::SDUP);
  addToInstructionLUT("SDROP", &Processor::SDROP);
  addToInstructionLUT("SCTOP", &Processor::SCTOP);
  addToInstructionLUT("SAND", &Processor::SAND);
  addToInstructionLUT("SOR", &Processor::SOR);
  addToInstructionLUT("SXOR", &Processor::SXOR);
  addToInstructionLUT("READX", &Processor::READX);
  addToInstructionLUT("SETY", &Processor::SETY);
  addToInstructionLUT("SNOT", &Processor::SNOT);
  addToInstructionLUT("EOPL", &Processor::EOPL);
  addToInstructionLUT("SHX", &Processor::SHX);
  addToInstructionLUT("SHY", &Processor::SHY);
  addToInstructionLUT("SGT", &Processor::GT);
  addToInstructionLUT("SGE", &Processor::GE);
  addToInstructionLUT("SEQ", &Processor::EQ);
  addToInstructionLUT("SLE", &Processor::LE);
  addToInstructionLUT("SLT", &Processor::LT);
  addToInstructionLUT("SNE", &Processor::NE);
  addToInstructionLUT("CCNTS", &Processor::CCNTS);
}

void Processor::addToInstructionLUT(const string& InstructionName,
                                    instruction ins)
{
  if ((InstructionName != "NOTDEF")
      && (_pConfig->getIC("NOTDEF") == _pConfig->getIC(InstructionName)))
  {
    throw out_of_range("** Instruction '" + InstructionName + "' not found.");
  }
  _instructionLUT[_pConfig->getIC(InstructionName)] = ins;
}

bool Processor::decode()
{
  _IR = _memory[_PC].integer;
  if (_IR < 0 || static_cast<size_t>(_IR) >= _instructionLUT.size())
  {
    setSRbit(ProcessorConfig::SR_STATUS_BIT::UNKNOWN_INSTRUCTION);
    return false;
  }
  ++_PC;
  return true;
}

void Processor::execute()
{
  (this->*_instructionLUT[_IR])();
}


void Processor::push(int value)
{
  _memory[_SP++].integer = value;
}

void Processor::push(bool value)
{
  _memory[_SP++].actual = value;
}

Data<int> Processor::pop()
{
  return _memory[--_SP];
}

void Processor::setSRbit(ProcessorConfig::SR_STATUS_BIT stat)
{
  _SR |= static_cast<int32_t>(stat);
}

void Processor::NOTDEF()
{
  logDebug(clog, "NOTDEF");
}

void Processor::LATCH()
{
  _memory[MemoryConfig::INPUT_X_LATCH] =
      _memory[MemoryConfig::INPUT_X];
  _memory[MemoryConfig::OUTPUT_Y_LATCH] =
      _memory[MemoryConfig::OUTPUT_Y];
}

void Processor::RESTORE()
{
  _memory[MemoryConfig::OUTPUT_Y] =
      _memory[MemoryConfig::OUTPUT_Y_LATCH];
  //_memory[MemoryConfig::INPUT_X] =
  //   _memory[MemoryConfig::INPUT_X_LATCH];
}

void Processor::INC()
{
  logDebug(clog, "INC");
  int addressRHS = _memory[_PC].integer;
  ++_PC;
  ++_memory[addressRHS].integer;
}

void Processor::DEC()
{
  logDebug(clog, "DEC");
  auto addressRHS = _memory[_PC].integer;
  ++_PC;
  --_memory[addressRHS].integer;
}

void Processor::AND()
{
  logDebug(clog, "AND");
  auto addressLHS = _memory[_PC++].integer;
  auto addressRHS = _memory[_PC++].integer;
  push(_memory[addressRHS].actual && _memory[addressLHS].actual);
}

void Processor::OR()
{
  logDebug(clog, "OR");
  auto addressLHS = _memory[_PC++].integer;
  auto addressRHS = _memory[_PC++].integer;
  push(_memory[addressRHS].actual || _memory[addressLHS].actual);
}

void Processor::NOT()
{
  logDebug(clog,"NOT");
  auto addressRHS = _memory[_PC].integer;
  ++_PC;
  push(!_memory[addressRHS].actual);
}

void Processor::SCNM()
{
  logDebug(clog, "SCNM");
  auto destination = _memory[_PC++].integer;
  auto value = _memory[_SP-1].actual;
  _memory[destination].set(value);
}

void Processor::SCNG()
{
  clog << "SCNG" << endl;
  auto destination = _memory[_PC++].integer;
  auto value = _memory[_SP-1].actual;
  _memory[destination].set(!value);
}

void Processor::SCST()
{
  clog << "SCST ";
  auto destination = _memory[_PC++].integer;
  auto value = _memory[_SP-1].actual;
  if (_memory[_SP-1].risingEdge())
  {
    _memory[destination].set(true);
    clog << value << " to address " << destination;
  }
  clog << endl;
}

void Processor::SCRS()
{
  clog << "SCRS ";
  int32_t destination;
  bool value;

  _memory[_PC++].get(destination);
  _memory[_SP-1].get(value);

  if (_memory[_SP-1].risingEdge())
  {
    _memory[destination].set(false);
    clog << value << " to address " << destination;
  }
  clog << endl;
}

void Processor::CPY()
{
  clog << "CPY ";
  auto source = _memory[_PC++].integer;
  auto destination = _memory[_PC++].integer;
  _memory[destination] = _memory[source];
  clog << source << " to address " << destination << endl;
}

void Processor::SPUSH()
{
  clog << "SPUSH ";
  int32_t address{0};
  _memory[_PC++].get(address);
  if (_SP >= MemoryConfig::ENTRYPOINT - 1)
  {
    setSRbit(ProcessorConfig::SR_STATUS_BIT::STACK_OVERFLOW);
    clog << "** ERROR stack overflow" << endl;
  }
  else
  {
    _memory[_SP++] = _memory[address];
    clog << _memory[address].integer << " from address " << address << endl;
  }
}

void Processor::SPOP()
{
  clog << "SPOP ";
  int32_t address{0};
  _memory[_PC++].get(address);
  if (_SP <= MemoryConfig::STACK - 1)
  {
    setSRbit(ProcessorConfig::SR_STATUS_BIT::STACK_UNDERFLOW);
    clog << "** ERROR stack underflow" << endl;
  }
  else
  {
    _memory[address] = _memory[--_SP];
    clog << _memory[address].integer << " to address " << address << endl;
  }
}

void Processor::SPOPB()
{
  clog << "SPOPB ";
  int32_t address{0};
  _memory[_PC++].get(address);
  if (_SP <= MemoryConfig::STACK - 1)
  {
    setSRbit(ProcessorConfig::SR_STATUS_BIT::STACK_UNDERFLOW);
  }
  else
  {
    clog << _memory[address].previous << " "
         << _memory[address].actual << " changed to ";
    _memory[address].set(_memory[--_SP].actual);
    clog << _memory[address].previous << " " << _memory[address].actual
         << " to address " << address << endl;
  }
}

void Processor::SDUP()
{
  logDebug(clog, "SDUP");
  _memory[_SP] = _memory[_SP - 1];
  ++_SP;
}

void Processor::SDROP()
{
  if (_SP <= MemoryConfig::STACK - 1)
  {
    setSRbit(ProcessorConfig::SR_STATUS_BIT::STACK_UNDERFLOW);
  }
  logDebug(clog, "SDROP");
  --_SP;
}

void Processor::SCTOP()
{
  clog << "SCTOP ";
  int32_t address{0};
  _memory[_PC++].get(address);
  _memory[address] = _memory[_SP-1];
  clog << _memory[_SP-1].integer << " to address " << address << endl;
}

void Processor::SAND()
{
  logDebug(clog, "SAND");
  _memory[_SP-2].set(_memory[_SP-2].actual &&
      _memory[_SP-1].actual);
  --_SP;
}

void Processor::SOR()
{
  logDebug(clog, "SOR");
  _memory[_SP-2].set(_memory[_SP-2].actual ||
      _memory[_SP-1].actual);
  --_SP;
}

void Processor::SXOR()
{
  logDebug(clog, "SXOR");
  _memory[_SP-2].set((_memory[_SP-2].actual || _memory[_SP-1].actual)
      && !(_memory[_SP-2].actual && _memory[_SP-1].actual));
  --_SP;
}

void Processor::SNOT()
{
  logDebug(clog, "SNOT");
  _memory[_SP-1].set(!_memory[_SP-1].actual);
}

void Processor::STB()
{
  logDebug(clog, "STB");
  auto addressTarget = _memory[_PC++].integer;
  auto bitNr = _memory[_PC++].integer;
  auto address = _memory[_PC++].integer;
  if (_memory[address].actual)
  {
    _memory[addressTarget].integer |= (1 << bitNr);
  }
  else
  {
    _memory[addressTarget].integer |= ~(1 << bitNr);
  }
}

void Processor::READX()
{
  clog << "READX ";
  auto address = MemoryConfig::INPUT_X_LATCH;
  auto bitNr = _memory[_PC++].integer;
  auto addressTarget = _memory[_PC++].integer;
  if (_memory[address].integer & (1 << bitNr))
  {
    _memory[addressTarget].set(true);
  }
  else
  {
    _memory[addressTarget].set(false);
  }
  clog << address << " contents " << _memory[address].integer
       << " bitNr: " << bitNr << " to " << addressTarget << endl;
}

void Processor::SETY()
{
  clog << "SETY ";
  auto addressTarget = MemoryConfig::OUTPUT_Y_LATCH;
  auto bitNr =  _memory[_PC++].integer;
  auto address = _memory[_PC++].integer;
  if (_memory[address].actual)
  {
    _memory[addressTarget].integer |= (1 << bitNr);
  }
  else
  {
    _memory[addressTarget].integer &= ~(1 << bitNr);
  }
  clog << address << " " << _memory[address].integer << " "
       << bitNr << " " << addressTarget << endl;
}

void Processor::EOPL()
{
  if (_SP != MemoryConfig::STACK)
  {
    setSRbit(ProcessorConfig::SR_STATUS_BIT::STACK_CORRUPTED);
  }
  _endOfProgramLoop = true;
  logDebug(clog,"EOPL");
#ifndef NDEBUG
  _memory.dump(cerr);
#endif
}

void Processor::SHX()
{
  const bitset<numeric_limits<int>::digits + 1>
      bs(_memory[MemoryConfig::INPUT_X].integer);
  const string binaryFormat(bs.to_string());
  clog << "SHX " << binaryFormat << endl;
}

void Processor::SHY()
{
  const bitset<numeric_limits<int>::digits + 1>
      bs(_memory[MemoryConfig::OUTPUT_Y].integer);
  const string binaryFormat(bs.to_string());
  clog << "SHY " << binaryFormat << endl;
}

void Processor::GT()
{
  logDebug(clog, "GT");
  _memory[_SP-2].set(_memory[_SP-2].integer > _memory[_SP-1].integer);
  --_SP;
}

void Processor::GE()
{
  logDebug(clog, "GE");
  _memory[_SP-2].set(_memory[_SP-2].integer >= _memory[_SP-1].integer);
  --_SP;
}

void Processor::EQ()
{
  logDebug(clog, "EQ");
  _memory[_SP-2].set(_memory[_SP-2].integer == _memory[_SP-1].integer);
  --_SP;
}

void Processor::LE()
{
  logDebug(clog, "LE");
  _memory[_SP-2].set(_memory[_SP-2].integer <= _memory[_SP-1].integer);
  --_SP;
}

void Processor::LT()
{
  logDebug(clog, "LT");
  _memory[_SP-2].set(_memory[_SP-2].integer < _memory[_SP-1].integer);
  --_SP;
}

void Processor::NE()
{
  logDebug(clog, "NE");
  _memory[_SP-2].set(_memory[_SP-2].integer != _memory[_SP-1].integer);
  --_SP;
}

void Processor::CCNTS()
{
  logDebug(clog, "CCNTS");
  for(auto& c: _counters)
  {
    c.checkIncrement();
    c.checkReset();
  }
}

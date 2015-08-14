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

Processor::Processor(Memory* pMemory,
                     std::array<Counter, MemoryConfig::nCOUNTERS>* pCounters)
  :  _pMemory{pMemory}
  ,  _pCounters{pCounters}
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
  _pMemory->clear();
}

void Processor::loadMemory(const Memory& mem)
{
  for (size_t i = 0; i < mem.size(); ++i)
  {
    (*_pMemory)[i] = mem[i];
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
  _IR = (*_pMemory)[_PC].integer;
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
  (*_pMemory)[_SP++].integer = value;
}

void Processor::push(bool value)
{
  (*_pMemory)[_SP++].actual = value;
}

Data<int> Processor::pop()
{
  return (*_pMemory)[--_SP];
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
  (*_pMemory)[MemoryConfig::INPUT_X_LATCH] =
      (*_pMemory)[MemoryConfig::INPUT_X];
  (*_pMemory)[MemoryConfig::OUTPUT_Y_LATCH] =
      (*_pMemory)[MemoryConfig::OUTPUT_Y];
}

void Processor::RESTORE()
{
  (*_pMemory)[MemoryConfig::OUTPUT_Y] =
      (*_pMemory)[MemoryConfig::OUTPUT_Y_LATCH];
  //(*pMemory)[MemoryConfig::INPUT_X] =
  //   (*pMemory)[MemoryConfig::INPUT_X_LATCH];
}

void Processor::INC()
{
  logDebug(clog, "INC");
  int addressRHS = (*_pMemory)[_PC].integer;
  ++_PC;
  ++(*_pMemory)[addressRHS].integer;
}

void Processor::DEC()
{
  logDebug(clog, "DEC");
  int addressRHS = (*_pMemory)[_PC].integer;
  ++_PC;
  --(*_pMemory)[addressRHS].integer;
}

void Processor::AND()
{
  logDebug(clog, "AND");
  int addressLHS = (*_pMemory)[_PC++].integer;
  int addressRHS = (*_pMemory)[_PC++].integer;
  push((*_pMemory)[addressRHS].actual && (*_pMemory)[addressLHS].actual);
}

void Processor::OR()
{
  logDebug(clog, "OR");
  int addressLHS = (*_pMemory)[_PC++].integer;
  int addressRHS = (*_pMemory)[_PC++].integer;
  push((*_pMemory)[addressRHS].actual || (*_pMemory)[addressLHS].actual);
}

void Processor::NOT()
{
  logDebug(clog,"NOT");
  int addressRHS = (*_pMemory)[_PC].integer;
  ++_PC;
  push(!(*_pMemory)[addressRHS].actual);
}

void Processor::SCNM()
{
  logDebug(clog, "SCNM");
  int destination = (*_pMemory)[_PC++].integer;
  bool value = (*_pMemory)[_SP-1].actual;
  (*_pMemory)[destination].set(value);
}

void Processor::SCNG()
{
  clog << "SCNG" << endl;
  int destination = (*_pMemory)[_PC++].integer;
  bool value = (*_pMemory)[_SP-1].actual;
  (*_pMemory)[destination].set(!value);
}

void Processor::SCST()
{
  clog << "SCST ";
  int destination = (*_pMemory)[_PC++].integer;
  bool value = (*_pMemory)[_SP-1].actual;
  if ((*_pMemory)[_SP-1].risingEdge())
  {
    (*_pMemory)[destination].set(true);
    clog << value << " to address " << destination;
  }
  clog << endl;
}

void Processor::SCRS()
{
  clog << "SCRS ";
  int destination;
  bool value;

  (*_pMemory)[_PC++].get(destination);
  (*_pMemory)[_SP-1].get(value);

  if ((*_pMemory)[_SP-1].risingEdge())
  {
    (*_pMemory)[destination].set(false);
    clog << value << " to address " << destination;
  }
  clog << endl;
}

void Processor::CPY()
{
  clog << "CPY ";
  int source = (*_pMemory)[_PC++].integer;
  int destination = (*_pMemory)[_PC++].integer;
  (*_pMemory)[destination] = (*_pMemory)[source];
  clog << source << " to address " << destination << endl;
}

void Processor::SPUSH()
{
  clog << "SPUSH ";
  int address;
  (*_pMemory)[_PC++].get(address);
  if (_SP >= MemoryConfig::ENTRYPOINT - 1)
  {
    setSRbit(ProcessorConfig::SR_STATUS_BIT::STACK_OVERFLOW);
    clog << "** ERROR stack overflow" << endl;
  }
  else
  {
    (*_pMemory)[_SP++] = (*_pMemory)[address];
    clog << (*_pMemory)[address].integer << " from address " << address << endl;
  }
}

void Processor::SPOP()
{
  clog << "SPOP ";
  int address;
  (*_pMemory)[_PC++].get(address);
  if (_SP <= MemoryConfig::STACK - 1)
  {
    setSRbit(ProcessorConfig::SR_STATUS_BIT::STACK_UNDERFLOW);
    clog << "** ERROR stack underflow" << endl;
  }
  else
  {
    (*_pMemory)[address] = (*_pMemory)[--_SP];
    clog << (*_pMemory)[address].integer << " to address " << address << endl;
  }
}

void Processor::SPOPB()
{
  clog << "SPOPB ";
  int address;
  (*_pMemory)[_PC++].get(address);
  if (_SP <= MemoryConfig::STACK - 1)
  {
    clog << "** ERROR stack underflow" << endl;
    setSRbit(ProcessorConfig::SR_STATUS_BIT::STACK_UNDERFLOW);
  }
  else
  {
    clog << (*_pMemory)[address].previous << " "
         << (*_pMemory)[address].actual << " changed to ";
    (*_pMemory)[address].set((*_pMemory)[--_SP].actual);
    clog << (*_pMemory)[address].previous << " " << (*_pMemory)[address].actual
         << " to address " << address << endl;
  }
}

void Processor::SDUP()
{
  logDebug(clog, "SDUP");
  (*_pMemory)[_SP] = (*_pMemory)[_SP - 1];
  ++_SP;
}

void Processor::SDROP()
{
  if (_SP <= MemoryConfig::STACK - 1)
  {
    clog << "** ERROR stack underflow" << endl;
    setSRbit(ProcessorConfig::SR_STATUS_BIT::STACK_UNDERFLOW);
  }
  clog << "SDROP" << endl;
  --_SP;
}

void Processor::SCTOP()
{
  clog << "SCTOP ";
  int address;
  (*_pMemory)[_PC++].get(address);
  (*_pMemory)[address] = (*_pMemory)[_SP-1];
  clog << (*_pMemory)[_SP-1].integer << " to address " << address << endl;
}

void Processor::SAND()
{
  logDebug(clog, "SAND");
  (*_pMemory)[_SP-2].set((*_pMemory)[_SP-2].actual &&
      (*_pMemory)[_SP-1].actual);
  --_SP;
}

void Processor::SOR()
{
  logDebug(clog, "SOR");
  (*_pMemory)[_SP-2].set((*_pMemory)[_SP-2].actual ||
      (*_pMemory)[_SP-1].actual);
  --_SP;
}

void Processor::SXOR()
{
  logDebug(clog, "SXOR");
  (*_pMemory)[_SP-2].set(((*_pMemory)[_SP-2].actual || (*_pMemory)[_SP-1].actual)
      && !((*_pMemory)[_SP-2].actual && (*_pMemory)[_SP-1].actual));
  --_SP;
}

void Processor::SNOT()
{
  logDebug(clog, "SNOT");
  (*_pMemory)[_SP-1].set(!(*_pMemory)[_SP-1].actual);
}

void Processor::STB()
{
  logDebug(clog, "STB");
  int addressTarget = (*_pMemory)[_PC++].integer;
  int bitNr =  (*_pMemory)[_PC++].integer;
  int address = (*_pMemory)[_PC++].integer;
  if ((*_pMemory)[address].actual)
  {
    (*_pMemory)[addressTarget].integer |= (1 << bitNr);
  }
  else
  {
    (*_pMemory)[addressTarget].integer |= ~(1 << bitNr);
  }
}

void Processor::READX()
{
  clog << "READX ";
  int address = MemoryConfig::INPUT_X_LATCH;
  int bitNr =  (*_pMemory)[_PC++].integer;
  int addressTarget = (*_pMemory)[_PC++].integer;
  if ((*_pMemory)[address].integer & (1 << bitNr))
  {
    (*_pMemory)[addressTarget].set(true);
  }
  else
  {
    (*_pMemory)[addressTarget].set(false);
  }
  clog << address << " contents " << (*_pMemory)[address].integer
       << " bitNr: " << bitNr << " to " << addressTarget << endl;
}

void Processor::SETY()
{
  clog << "SETY ";
  int addressTarget = MemoryConfig::OUTPUT_Y_LATCH;
  int bitNr =  (*_pMemory)[_PC++].integer;
  int address = (*_pMemory)[_PC++].integer;
  if ((*_pMemory)[address].actual)
  {
    (*_pMemory)[addressTarget].integer |= (1 << bitNr);
  }
  else
  {
    (*_pMemory)[addressTarget].integer &= ~(1 << bitNr);
  }
  clog << address << " " << (*_pMemory)[address].integer << " "
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
  _pMemory->dump(cerr);
#endif
}

void Processor::SHX()
{
  const bitset<numeric_limits<int>::digits + 1>
      bs((*_pMemory)[MemoryConfig::INPUT_X].integer);
  const string binaryFormat(bs.to_string());
  clog << "SHX " << binaryFormat << endl;
}

void Processor::SHY()
{
  const bitset<numeric_limits<int>::digits + 1>
      bs((*_pMemory)[MemoryConfig::OUTPUT_Y].integer);
  const string binaryFormat(bs.to_string());
  clog << "SHY " << binaryFormat << endl;
}

void Processor::GT()
{
  logDebug(clog, "GT");
  (*_pMemory)[_SP-2].set((*_pMemory)[_SP-2].integer >
      (*_pMemory)[_SP-1].integer);
  --_SP;
}

void Processor::GE()
{
  logDebug(clog, "GE");
  (*_pMemory)[_SP-2].set((*_pMemory)[_SP-2].integer >=
      (*_pMemory)[_SP-1].integer);
  --_SP;
}

void Processor::EQ()
{
  logDebug(clog, "EQ");
  (*_pMemory)[_SP-2].set((*_pMemory)[_SP-2].integer ==
      (*_pMemory)[_SP-1].integer);
  --_SP;
}

void Processor::LE()
{
  logDebug(clog, "LE");
  (*_pMemory)[_SP-2].set((*_pMemory)[_SP-2].integer <=
      (*_pMemory)[_SP-1].integer);
  --_SP;
}

void Processor::LT()
{
  logDebug(clog, "LT");
  (*_pMemory)[_SP-2].set((*_pMemory)[_SP-2].integer <
      (*_pMemory)[_SP-1].integer);
  --_SP;
}

void Processor::NE()
{
  logDebug(clog, "NE");
  (*_pMemory)[_SP-2].set((*_pMemory)[_SP-2].integer !=
      (*_pMemory)[_SP-1].integer);
  --_SP;
}

void Processor::CCNTS()
{
  if (nullptr == _pCounters)
  {
    setSRbit(ProcessorConfig::SR_STATUS_BIT::NO_COUNTERS);
  }
  else
  {
    logDebug(clog, "CCNTS");
    for(auto& c: *_pCounters)
    {
      c.checkIncrement();
      c.checkReset();
    }
  }
}

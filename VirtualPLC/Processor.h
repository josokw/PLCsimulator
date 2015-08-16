#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "Data.h"
#include "Memory.h"
#include "ProcessorConfig.h"

#include <map>
#include <memory>
#include <string>

class Counter;
class Processor;
class Timer;

using timers_t = std::array<Timer, MemoryConfig::nTIMERS>;
using counters_t = std::array<Counter, MemoryConfig::nCOUNTERS>;
typedef void (Processor::*instruction)();

/// Processor is able to use variables and constants, timers and counters.
/// Internal registers: 32 bits.
/// A processor executes instructions in memory.
class Processor
{
public:
  Processor(Memory& memory,
            timers_t& _timers,
            counters_t& counters);
  Processor(Processor const& other) = delete;
  Processor& operator=(Processor const& other) = delete;
  ~Processor() = default;
  /// Executes program in memory
  void run();
  /// Loads program in memory
  void loadMemory(const Memory& memory);
  /// Returns actual value Program Counter
  int32_t getPC() const { return _PC; }
  /// Returns actual value Status Register
  int32_t getSR() const { return _SR; }
private:
  Memory& _memory;
  timers_t& _timers;
  counters_t& _counters;
  std::unique_ptr<ProcessorConfig> _pConfig;
  /// Program Counter
  int32_t _PC;
  /// Instruction Register
  int32_t _IR;
  /// Stack Pointer
  int32_t _SP;
  /// Status Register, contains run time error status
  int32_t _SR;
  /// Instruction Look Up Table (LUT)
  std::array<instruction, ProcessorConfig::MAX_INSTRUCTIONS> _instructionLUT;
  /// Check for end of program loop.
  bool _endOfProgramLoop;
  /// Clear memory: all valued 0x0000
  void clearMemory();
  /// Fill instruction look up table (pointers to class member functions).
  void fillInstructionLUT();
  void addToInstructionLUT(const std::string& InstructionName, instruction ins);
  /// Decode instruction
  /// @return Valid instruction status
  bool decode();
  /// Excecute decoded instruction
  /// @pre Valid instruction code
  void execute();
  /// Push to stack
  void push(int value);
  /// Push to stack
  void push(bool value);
  /// Pull from stack
  data_t pop();
  /// Set SR bit
  void setSRbit(ProcessorConfig::SR_STATUS_BIT stat);
  //--------------------------------------------------------------- Instructions
  /// Not defined instruction
  void NOTDEF();
  /// Latching X en Y values
  void LATCH();
  /// Restoring Y values
  void RESTORE();
  /// Inc var
  void INC();
  void DEC();
  void AND();
  void OR();
  void NOT();
  /// Copy top of stack: normal, Stack Copy NorMal
  void SCNM();
  /// Copy top of stack: negation, Stack Copy NeGation
  void SCNG();
  /// Copy top of stack: set, Stack Copy SeT
  void SCST();
  /// Copy top of stack: reset, Stack Copy ReSet
  void SCRS();
  /// CPY: Copy <source address> <destination address>
  void CPY();
  /// SP: Push memory contents to stack, Stack PUSH
  void SPUSH();
  /// SP: Pop top of stack INT to memory, Stack POP
  void SPOP();
  /// SP: Pop top of stack BOOL to memory, preserving edge detection, Stack POPB
  void SPOPB();
  /// SP: Duplicate top of stack, Stack DUPlicate
  void SDUP();
  /// SP: Drop top of stack, Stack DROP
  void SDROP();
  /// SP: Copies top of stack
  void SCTOP();
  /// SP: AND two top of stack
  void SAND();
  /// SP: OR two top of stack
  void SOR();
  /// SP: XOR two top of stack
  void SXOR();
  /// SP: NOT top of stack
  void SNOT();
  /// Set X[i] to value at address
  void READX();
  /// Set Y[i] to value at address
  void SETY();
  /// Set bit: STB addr bit addr
  void STB();
  /// End of program loop
  void EOPL();
  /// Debugging: show X in binary format
  void SHX();
  /// Debugging: show Y in binary format
  void SHY();
  /// Greater than (>)
  void GT();
  /// Greater than / equal to (>=)
  void GE();
  /// Equals (==)
  void EQ();
  /// Less than /equals (<=)
  void LE();
  /// Less than (<)
  void LT();
  /// Not equal (!=)
  void NE();
  /// Check counters (increment counter value if enable input has a rising edge).
  void CCNTS();
};

#endif // PROCESSOR_H

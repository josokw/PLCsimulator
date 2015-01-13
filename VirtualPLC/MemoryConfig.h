#ifndef MEMORYCONFIG_H
#define MEMORYCONFIG_H

#include "FunctionConfig.h"

class MemoryConfig
{
public:
  // Memory block sizes
  static const int K = 1024;
  static const int MEM_SIZE = 4 * K;
  static const int CONSTANTS_SIZE = 100;
  static const int VARS_SIZE = 100;
  static const int TMPVARS_SIZE = 100;
  static const int STACK_SIZE = 200;
  static const int TIMER_SIZE = FunctionConfig::TSize;
  static const int COUNTER_SIZE = FunctionConfig::CSize;
  // Number of timers and counters
  static const int nTIMERS = 10;
  static const int nCOUNTERS = 5;
  // Memory offsets
  static const int CONSTANTS = 0;
  static const int VARS_INIT = CONSTANTS + CONSTANTS_SIZE;
  static const int TIMERS_INIT = VARS_INIT + VARS_SIZE;
  static const int COUNTERS_INIT = TIMERS_INIT + nTIMERS * TIMER_SIZE;
  static const int VARS = COUNTERS_INIT + nCOUNTERS * COUNTER_SIZE;
  static const int TMPVARS = VARS + VARS_SIZE;
  static const int STACK = TMPVARS + TMPVARS_SIZE;
  static const int ENTRYPOINT = STACK + STACK_SIZE;
  // INPUTs and OUTPUTs offsets
  static const int INPUT_X = MEM_SIZE - 1;
  static const int OUTPUT_Y = INPUT_X - 1;
  static const int INPUT_X_LATCH = OUTPUT_Y - 1;
  static const int OUTPUT_Y_LATCH = INPUT_X_LATCH - 1;
  // Timers and Counters offsets
  static const int TIMERS = OUTPUT_Y_LATCH - nTIMERS * TIMER_SIZE;
  static const int COUNTERS = TIMERS - nCOUNTERS * COUNTER_SIZE;
};

#endif // MEMORYCONFIG_H

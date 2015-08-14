#ifndef MEMORYCONFIG_H
#define MEMORYCONFIG_H

#include "FunctionConfig.h"

class MemoryConfig
{
public:
  // Memory block sizes
  static const int K = 1024;
  static const int32_t MEM_SIZE = 4 * K;
  static const int32_t CONSTANTS_SIZE = 100;
  static const int32_t VARS_SIZE = 100;
  static const int32_t TMPVARS_SIZE = 100;
  static const int32_t STACK_SIZE = 200;
  static const int32_t TIMER_SIZE = FunctionConfig::TSize;
  static const int32_t COUNTER_SIZE = FunctionConfig::CSize;
  // Number of timers and counters
  static const int32_t nTIMERS = 10;
  static const int32_t nCOUNTERS = 5;
  // Memory offsets
  static const int32_t CONSTANTS = 0;
  static const int32_t VARS_INIT = CONSTANTS + CONSTANTS_SIZE;
  static const int32_t TIMERS_INIT = VARS_INIT + VARS_SIZE;
  static const int32_t COUNTERS_INIT = TIMERS_INIT + nTIMERS * TIMER_SIZE;
  static const int32_t VARS = COUNTERS_INIT + nCOUNTERS * COUNTER_SIZE;
  static const int32_t TMPVARS = VARS + VARS_SIZE;
  static const int32_t STACK = TMPVARS + TMPVARS_SIZE;
  static const int32_t ENTRYPOINT = STACK + STACK_SIZE;
  // INPUTs and OUTPUTs offsets
  static const int32_t INPUT_X = MEM_SIZE - 1;
  static const int32_t OUTPUT_Y = INPUT_X - 1;
  static const int32_t INPUT_X_LATCH = OUTPUT_Y - 1;
  static const int32_t OUTPUT_Y_LATCH = INPUT_X_LATCH - 1;
  // Timers and Counters offsets
  static const int32_t TIMERS = OUTPUT_Y_LATCH - nTIMERS * TIMER_SIZE;
  static const int32_t COUNTERS = TIMERS - nCOUNTERS * COUNTER_SIZE;
};

#endif // MEMORYCONFIG_H

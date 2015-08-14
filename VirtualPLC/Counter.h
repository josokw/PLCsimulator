#ifndef COUNTER_H
#define COUNTER_H

#include <cstdint>

class Memory;

class Counter
{
public:
  Counter();
  ~Counter() = default;
  /// Set counter id [0..N)
  void setID(int32_t id) { _id = id; }
  /// Maps counter to memory and initialises all internal registers.
  void mapToMemory(Memory& mem, int32_t offset);
  /// Check reset CC
  void checkReset();
  /// Check increment
  void checkIncrement();
private:
  Memory *_pMemory;
  int32_t _id;
  int32_t _offset;
  int32_t _CEnable;
  int32_t _CValue;
  int32_t _CN;
  int32_t _CS;
  int32_t _CC;
};

#endif // COUNTER_H

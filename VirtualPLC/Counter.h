#ifndef COUNTER_H
#define COUNTER_H

#include <cstdint>

class Memory;

class Counter
{
public:
  Counter() = default;
  Counter(Memory& memory, int id);
  Counter(const Counter& other) = default;
  Counter& operator=(const Counter& other) = default;
  ~Counter() = default;
  /// Maps counter to memory and initialises all internal registers.
  void mapToMemory() const;
  /// Check reset CC
  void checkReset() const;
  /// Check increment
  void checkIncrement() const;
private:
  Memory *_pMemory;
  int32_t _id;
  int32_t _CEnable;
  int32_t _CValue;
  int32_t _CN;
  int32_t _CS;
  int32_t _CC;
};

#endif // COUNTER_H

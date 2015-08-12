#ifndef COUNTER_H
#define COUNTER_H

class Memory;

class Counter
{
public:
  Counter();
  ~Counter() = default;
  /// Set counter id [0..N)
  void setID(int id) { _id = id; }
  /// Maps counter to memory and initialises all internal registers.
  void mapToMemory(Memory& mem, int offset);
  /// Check reset CC
  void checkReset();
  /// Check increment
  void checkIncrement();
private:
  Memory *_pMemory;
  int _id;
  int _offset;
  int _CEnable;
  int _CValue;
  int _CN;
  int _CS;
  int _CC;
};

#endif // COUNTER_H

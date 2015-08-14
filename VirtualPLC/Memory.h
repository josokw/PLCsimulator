#ifndef MEMORY_H
#define MEMORY_H

#include "Data.h"
#include "MemoryConfig.h"

#include <array>
#include <iosfwd>

using data_t = Data<int32_t>;

class MemoryConfig;

/// Memory represents the RAM memory connected to the processor.
/// Memory contents type: int and bool (union)
/// Memory address type: int
/// This class contains the memory organisation data in MemoryConfig.
class Memory
{
public:
  /// Constructor, creates MEM_SIZE memory locations.
  Memory();
  // Returns memory size
  size_t size() const { return _memory.size(); }
  /// Fills memory with 0 values.
  void clear();
  /// Index operator.
  data_t operator[](int32_t index) const;
  /// Index opeartor.
  data_t& operator[](int32_t index);
  /// Add next CONST INT
  void addNextCONST(int32_t value);
  /// Add next CONST BOOL
  void addNextCONST(bool value);
  /// Add next VAR INT
  void addNextVAR(int32_t value);
  /// Add next VAR BOOL
  void addNextVAR(bool value);
  /// Add next TMPVAR INT
  void addNextTMPVAR(int32_t value);
  /// Add next TMPVAR BOOL
  void addNextTMPVAR(bool value);
  /// Add next CODE
  void addNextCODE(int32_t value);
  /// Get next available CONST address
  int32_t getNextCONSTaddress() const { return _constAddress; }
  /// Get next available VAR address
  int32_t getNextVARaddress() const { return _varAddress; }
  /// Get next available TMPVAR address
  int32_t getNextTMPVARaddress() const { return _tmpvarAddress; }
  /// Get next available CODE address
  int32_t getNextCODEaddress() const { return _codeAddress; }
  /// Dump memory contents
  void dump(std::ostream& os) const;
private:
  bool _outOfMemory;
  std::array<data_t, MemoryConfig::MEM_SIZE> _memory;
  int32_t _constAddress;
  int32_t _varAddress;
  int32_t _tmpvarAddress;
  int32_t _codeAddress;
};

#endif // MEMORY_H

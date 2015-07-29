#ifndef COMPILER_H
#define COMPILER_H

#include "Memory.h"

#include <fstream>
#include <string>
#include <tuple>
#include <vector>

/// Compiler for building PLC programs.
/// Input: script containing constants and variables declarations,
/// mappings variables to in-- and output, expressions and LD lines.
/// An LD line contains 1 contact and >= 1 coils.
class Compiler
{
public:
  /// char: S(ystem error), E(rror), W(arning), I(nfo)
  /// int: corresponding line number
  /// const char*: position in input stream
  /// string: message
  typedef std::tuple<char, int, const char*, std::string> message_t;

  Compiler();
  virtual ~Compiler();

  void clear();
  bool readFile(const std::string& fileName);
  bool writeFile(const std::string& fileName) const;
  bool readBinFile(const std::string& fileName);
  bool writeBinFile(const std::string& fileName) const;
  void loadMemory(const Memory& mem);
  void parse();
  const char* getScript() const { return &_plcScript[0]; }
  void setScript(const std::string& scriptText);
  const std::vector<message_t>& getMessages() const { return _messages; }
  bool containsErrors() const { return _memoryContainsErrors; }
  const Memory& getResult() const { return _memory; }

private:
  bool _memoryContainsErrors;
  std::vector<message_t> _messages;
  std::ifstream _plcScriptFile;
  mutable std::ofstream _plcScriptFileOut;
  std::ifstream _plcBinFile;
  mutable std::ofstream _plcBinFileOut;
  std::vector<char> _plcScript;
  /// Memory contains compiler output.
  Memory _memory;
};

#endif // COMPILER_H

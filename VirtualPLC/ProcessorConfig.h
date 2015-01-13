#ifndef PROCESSORCONFIG_H
#define PROCESSORCONFIG_H

#include <map>
#include <string>

/// Contains the mapping of symbolic instruction names to
/// the corresponding instruction code.
class ProcessorConfig
{
public:
  /// Maximal number of instructions
  static const int MAX_INSTRUCTIONS = 128;
  /// SR status bits index
  static const int STACK_OVERFLOW = 0;
  static const int STACK_UNDERFLOW = 1;
  static const int UNKNOWN_INSTRUCTION = 2;
  /// Constructor maps symbolic names to instruction codes.
  ProcessorConfig();
  ProcessorConfig(const ProcessorConfig&) = delete;
  ProcessorConfig& operator=(const ProcessorConfig&) = delete;
  ~ProcessorConfig() = default;
  /// Maps a symbolic name to the corresponding next available instruction code.
  void setMapping(const std::string& symbolicName);
  /// Gets corresponding instruction code.
  /// @pre Instruction NOTDEF must be defined.
  int getIC(std::string const& symbolicName) const;
private:
  /// Next available instruction code
  int _nextIC;
  /// Mapping symbolic instruction names to instruction code (>= 0).
  std::map<std::string, int> _symbolicInstructionToIC;
};

#endif // PROCESSORCONFIG_H

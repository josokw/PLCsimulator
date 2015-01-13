#include "ProcessorConfig.h"

ProcessorConfig::ProcessorConfig()
  :   _nextIC(0)
  ,   _symbolicInstructionToIC()
{
  setMapping("EOPL");
  setMapping("LATCH");
  setMapping("RESTORE");
  setMapping("INC");
  setMapping("DEC");
  setMapping("AND");
  setMapping("OR");
  setMapping("NOT");
  setMapping("SCNM");
  setMapping("SCNG");
  setMapping("SCST");
  setMapping("SCRS");
  setMapping("CPY");
  setMapping("SPUSH");
  setMapping("SPOP");
  setMapping("SPOPB");
  setMapping("SDUP");
  setMapping("SDROP");
  setMapping("SCTOP");
  setMapping("SAND");
  setMapping("SOR");
  setMapping("SXOR");
  setMapping("SNOT");
  setMapping("READX");
  setMapping("SETY");
  setMapping("SHX");
  setMapping("SHY");
  setMapping("SGT");
  setMapping("SGE");
  setMapping("SEQ");
  setMapping("SLE");
  setMapping("SLT");
  setMapping("SNE");
  setMapping("CCNTS");
  setMapping("NOTDEF");
}

void ProcessorConfig::setMapping(const std::string& symbolicName)
{
  auto cit(_symbolicInstructionToIC.find(symbolicName));
  if (cit == _symbolicInstructionToIC.cend())
  {
    _symbolicInstructionToIC[symbolicName] = _nextIC;
    ++_nextIC;
  }
}

int ProcessorConfig::getIC(const std::string& symbolicName) const
{
  auto cit(_symbolicInstructionToIC.find(symbolicName));
  if (cit == _symbolicInstructionToIC.cend())
  {
    cit = _symbolicInstructionToIC.find("NOTDEF");
  }
  return cit->second;
}

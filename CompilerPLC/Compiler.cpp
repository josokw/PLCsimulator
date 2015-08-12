#include "Compiler.h"
#include "Parser.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include <iterator>

using namespace std;
namespace bsp = boost::spirit::classic;

Compiler::Compiler()
   :  _memoryContainsErrors{false}
   ,  _messages{}
   ,  _plcScriptFile{}
   ,  _plcScriptFileOut{}
   ,  _plcBinFile{}
   ,  _plcBinFileOut{}
   ,  _plcScript{}
   ,  _memory{}
{
}

void Compiler::clear()
{
   _messages.clear();
   _plcScript.clear();
   _memory.clear();
}

bool Compiler::readFile(const std::string& fileName)
{
   _plcScriptFile.open(fileName.c_str());
   if (!_plcScriptFile) {
      return false;
   }
   else {
      _plcScript.clear();
      _plcScriptFile >> noskipws;
      copy(istream_iterator<char>(_plcScriptFile), istream_iterator<char>(), back_inserter(_plcScript));
      _plcScript.push_back(0);
   }
   _plcScriptFile.close();

   return true;
}

bool Compiler::writeFile(const std::string& fileName) const
{
   _plcScriptFileOut.open(fileName.c_str());
   if (!_plcScriptFileOut) {
      return false;
   }
   for(const auto& s: _plcScript) {
      _plcScriptFileOut << s;
   }
   //for (size_t i = 0; i < _plcScript.size(); ++i) {
   //   _plcScriptFileOut << _plcScript[i];
   //}
   _plcScriptFileOut.close();
   return true;
}

bool Compiler::readBinFile(const std::string& fileName)
{
   _plcBinFileOut.open(fileName.c_str());
   if (!_plcBinFileOut) {
      return false;
   }
   //copy(istream_iterator<char>(plcScript), istream_iterator<char>(), ostream_iterator(plcScriptFileOut));
   for (int i = 0; i < MemoryConfig::MEM_SIZE; ++i) {
      _plcBinFileOut << _memory[i].integer;
   }
   _plcBinFileOut.close();
   return true;
}

bool Compiler::writeBinFile(const std::string& fileName) const
{
   _plcBinFileOut.open(fileName.c_str());
   if (!_plcBinFileOut) {
      return false;
   }
   //copy(istream_iterator<char>(plcScript), istream_iterator<char>(), ostream_iterator(plcScriptFileOut));
   for (int i = 0; i < MemoryConfig::MEM_SIZE; ++i) {
      _plcBinFileOut << _memory[i].integer << " ";
      if ((i + 1) % 10 == 0) {
         _plcBinFileOut << "\n";
      }
   }
   _plcBinFileOut.close();
   return true;
}

void Compiler::loadMemory(const Memory& mem)
{
   _memory = mem;
}

void Compiler::parse()
{
   _memoryContainsErrors = false;
   _messages.clear();
   VirtualPLCBuilder vpb(_messages);
   skipParser skip;
   PLCscriptParser parser(vpb);

   try {
      if (_plcScript.empty()) {
         _messages.push_back(message_t('E', 0, 0, "Parse script is empty"));
         std::cout << "-- [Compiler] parse script is empty" << std::endl;
         _memoryContainsErrors = true;
         return;
      }
      bsp::parse_info<> info = bsp::parse(&_plcScript[0], parser, skip);
      if (info.full) {
         std::cout << "-- [Compiler] parse successful" << std::endl;
      }
      else  {
         _memoryContainsErrors = true;
         cerr << "-- [Compiler] parsing failed" << std::endl;
         cerr.write(&_plcScript[0], info.stop - &_plcScript[0]);
         cerr << std::endl;

         _messages.push_back(message_t('E', 0, info.stop, "syntax error, parsing failed"));

         const char* pBegin = info.stop;
         if (pBegin > &_plcScript[0]) {
            --pBegin;
         }
         while (pBegin >= &_plcScript[0] && (pBegin[0] != '\n')) {
            pBegin--;
         }
         cerr << "-- previous line: " << string(pBegin, info.stop) << endl;
         cerr << "-- stopped at: '" << info.stop << "'" << endl << "--" << endl;
      }
      _memory = vpb.getResult();
      _memory.dump(cerr);
   }
   catch(std::exception& x)
   {
      clog << "[Compiler] SYSTEM ERROR " << x.what() << endl;
      _messages.push_back(message_t('S', 0, 0, x.what()));
   }
   catch(...)
   {
      clog << "[Compiler] SYSTEM CRASH" << endl;
      _messages.push_back(message_t('S', 0, 0, "CRASH"));
   }

   for (size_t i = 0; i < _messages.size(); ++i) {
      int lineNumber = count(static_cast<const char*>(&_plcScript[0]), std::get<2>(_messages[i]), '\n') + 1;
      std::get<1>(_messages[i]) = lineNumber;
   }
}

void Compiler::setScript(const std::string& scriptText)
{
   _plcScript.clear();
   _plcScript.assign(scriptText.begin(), scriptText.end());
   _plcScript.push_back(0);
}

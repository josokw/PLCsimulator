#ifndef VIRTUALPLC_H
#define VIRTUALPLC_H

#include "Counter.h"
#include "Memory.h"
#include "MemoryConfig.h"
#include "Processor.h"
#include "Timer.h"
#include "PeriodicTask.h"

#include <atomic>
#include <array>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

/// A VirtualPLC contains a processor, memory, timers and counters,
/// and is able to execute the program stored in memory.
/// Based on Mitsubishi Melsec PLC: FX1N
class VirtualPLC
{
public:
   VirtualPLC();
   VirtualPLC(const VirtualPLC& other) = delete;
   VirtualPLC& operator=(const VirtualPLC& other) = delete;
   virtual ~VirtualPLC();
   /// Stop PLC and put PLC in initial state (all vars, timers and counters
   /// get initial value)
   void clear();
   bool readBinFile(const std::string& fileName);
   bool writeBinFile(const std::string& fileName);
   void loadMemory(const Memory& mem);
   /// Put PLC in running mode
   void run();
   /// Test run status
   bool isRunning() const { return _isRunning_; }
   /// PLC continues running
   void start();
   /// Halt PLC, stop running
   void stop();
   /// Get output by index (0..nY)
   bool getY(int index) const { return _memory[MemoryConfig::OUTPUT_Y].integer
         & (1 << index); }
   /// Set input by index (0..nX) en status (true/false)
   void setX(int index, bool status);
   int getYs() const { return _memory[MemoryConfig::OUTPUT_Y].integer; }
   void setXs(int status) { _memory[MemoryConfig::INPUT_X].integer = status; }
   int getMaxX() const { return nX; }
   int getMaxY() const { return nY; }
private:
   /// Number of inputs
   static const int nX = 8;
   /// Number of outputs
   static const int nY = 6;
   static const int TICK = 100; // in msec
   static const int RUN = 50; // in msec
   std::mutex _mutexIO;
   std::ifstream _plcBinFile;
   std::ofstream _plcBinFileOut;
   Memory _memory;
   std::array<Timer, MemoryConfig::nTIMERS> _timers;
   std::array<Counter, MemoryConfig::nCOUNTERS> _counters;
   Processor _processor;
   /// Stores memory status
   bool _memoryIsLoaded;
   /// Stores running status
   std::atomic<bool> _isRunning_;
   /// Stores the number of loops executed.
   unsigned long long _step;
   PeriodicTask _tickTask;
   PeriodicTask _runTask;
   /// Updates all timers.
   void tick();
   void resetXs() { _memory[MemoryConfig::INPUT_X].integer = 0; }
   void resetYs() { _memory[MemoryConfig::OUTPUT_Y].integer = 0; }
};

#endif // VIRTUALPLC_H

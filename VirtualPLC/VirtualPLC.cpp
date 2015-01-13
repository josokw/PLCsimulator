#include "VirtualPLC.h"
#include "Data.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include <iterator>

using namespace std;

VirtualPLC::VirtualPLC()
  :  _mutexIO()
  ,  _plcBinFile()
  ,  _plcBinFileOut()
  ,  _memory()
  ,  _timers(MemoryConfig::nTIMERS)
  ,  _counters(MemoryConfig::nCOUNTERS)
  ,  _processor(&_memory, &_counters)
  ,  _memoryIsLoaded(false)
  ,  _isRunning_(false)
  ,  _step(0L)
  ,  _tickTask(this, &VirtualPLC::tick, TICK)
  ,  _runTask(this, &VirtualPLC::run, RUN)
{
  // Map all timers to memory
  size_t i = 0;
  for (auto& timer: _timers)
  {
    timer.setID(i);
    timer.mapToMemory(_memory,
                      MemoryConfig::TIMERS
                      + i * MemoryConfig::TIMER_SIZE);
    ++i;
  }
  // Map all counters to memory
  i = 0;
  for (auto& counter: _counters)
  {
    counter.setID(i);
    counter.mapToMemory(_memory,
                        MemoryConfig::COUNTERS
                        + i * MemoryConfig::COUNTER_SIZE);
    ++i;
  }
}

VirtualPLC::~VirtualPLC()
{
  _tickTask.stop();
  _runTask.stop();
}

void VirtualPLC::clear()
{
  lock_guard<std::mutex> lock(_mutexIO);
  copy(&_memory[MemoryConfig::VARS_INIT],
      &_memory[MemoryConfig::VARS_INIT + MemoryConfig::VARS_SIZE],
      &_memory[MemoryConfig::VARS]);
  copy(&_memory[MemoryConfig::COUNTERS_INIT],
      &_memory[MemoryConfig::COUNTERS_INIT
      + MemoryConfig::nCOUNTERS * MemoryConfig::COUNTER_SIZE],
      &_memory[MemoryConfig::COUNTERS]);
  copy(&_memory[MemoryConfig::TIMERS_INIT],
      &_memory[MemoryConfig::TIMERS_INIT
      + MemoryConfig::nTIMERS * MemoryConfig::TIMER_SIZE],
      &_memory[MemoryConfig::TIMERS]);
}

bool VirtualPLC::readBinFile(const std::string& fileName)
{
  stop();
  _plcBinFile.open(fileName.c_str());
  if (!_plcBinFile)
  {
    return false;
  }
  size_t i = 0;
  while (!_plcBinFile.eof() && i < _memory.size())
  {
    _plcBinFile >> _memory[i].integer;
    ++i;
  }
  _plcBinFile.close();
  clear();
  i == _memory.size() ? _memoryIsLoaded = true : _memoryIsLoaded = false;
  return _memoryIsLoaded;
}

bool VirtualPLC::writeBinFile(const std::string& fileName)
{
  stop();
  clear();
  resetXs();
  resetYs();
  _plcBinFileOut.open(fileName.c_str());
  if (!_memoryIsLoaded || !_plcBinFileOut)
  {
    return false;
  }
  for (size_t i = 0; i < _memory.size(); ++i)
  {
    _plcBinFileOut << _memory[i].integer << " ";
    if ((i + 1) % 10 == 0)
    {
      _plcBinFileOut << "\n";
    }
  }
  _plcBinFileOut.close();
  return true;
}

void VirtualPLC::loadMemory(const Memory& mem)
{
  lock_guard<std::mutex> lock(_mutexIO);
  _processor.loadMemory(mem);
  _memoryIsLoaded = true;
}

void VirtualPLC::run()
{
  if (_memoryIsLoaded)
  {
    ++_step;
    cerr << "-- [VirtualPLC] run(), step: " << _step << endl;
    _processor.run();
  }
  else
  {
    cerr << "-- [VirtualPLC] memory is not loaded" << endl;
  }
}

void VirtualPLC::start()
{
  if (_memoryIsLoaded)
  {
    _tickTask.start();
    _runTask.start();
    _isRunning_ = true;
  }
  else
  {
    cerr << "-- [VirtualPLC] memory is not loaded" << endl;
  }
}

void VirtualPLC::stop()
{
  if (_memoryIsLoaded)
  {
    _tickTask.stop();
    _runTask.stop();
    _isRunning_ = false;
  }
}

void VirtualPLC::setX(int index, bool status)
{
  lock_guard<std::mutex> lock(_mutexIO);
  _memory[MemoryConfig::INPUT_X].integer =
      status
      ? (_memory[MemoryConfig::INPUT_X].integer | (1 << index))
      : (_memory[MemoryConfig::INPUT_X].integer & ~(1 << index));
}

void VirtualPLC::tick()
{
  for_each(begin(_timers), end(_timers), mem_fun_ref(&Timer::tick));
}

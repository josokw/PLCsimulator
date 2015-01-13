#ifndef PERIODICTASK_H
#define PERIODICTASK_H

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

/// Represents a periodic task. Sample time in milliseconds.
class PeriodicTask
{
public:
  template <typename C, typename F>
  PeriodicTask(C* object, F mf, int tms)
    :  _tms(tms)
    ,  _isAlive(true)
    ,  _isRunning(false)
    ,  _mutex()
    ,  _condition()
    ,  _thread((std::bind(&PeriodicTask::execute<C, F>, this, object, mf)))
  {}

  ~PeriodicTask()
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _condition.notify_one();
    _isAlive = false;
    lock.unlock();
    _thread.join();
  }

  template <typename C, typename F>
  void execute(C* object, F mf)
  {
    std::unique_lock<std::mutex> lock(_mutex);
    while (_isAlive)
    {
      while (!_isRunning)
      {
        _condition.wait(lock);
      }
      if (_isAlive)
      {
        lock.unlock();
        //const boost::system_time timeout(boost::get_system_time() +
        //                                 boost::posix_time::milliseconds(_t));
        (object->*mf)();
        std::this_thread::sleep_for(_tms);
        lock.lock();
      }
    }
  }

  void start()
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _isRunning = true;
    _condition.notify_one();
  }

  void stop()
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _isRunning = false;
  }

private:
  std::chrono::milliseconds _tms;
  volatile bool _isAlive;
  volatile bool _isRunning;
  std::mutex _mutex;
  std::condition_variable _condition;
  std::thread _thread;
};

#endif // PERIODICTASK_H

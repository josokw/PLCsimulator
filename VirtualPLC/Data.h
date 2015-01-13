#ifndef DATA_H
#define DATA_H

#include <type_traits>

template<typename T>
union Data
{ 
  static_assert(std::is_integral<T>::value, "Data requires integral type");

  T integer;
  struct {
    bool actual;
    bool previous;
  };

  Data(): integer(0) { }
  Data(T value): integer(value) { }
  void set(T value) { integer = value; }
  void set(bool value) { previous = actual; actual = value; }
  void get(T& value) const { value = integer; }
  void get(bool& value) const { value = actual; }
  bool risingEdge() const { return actual && !previous; }
  bool fallingEdge() const { return !actual && previous; }
};

#endif // DATA_H

#ifndef FUNCTIONCONFIG_H
#define FUNCTIONCONFIG_H

#include <cstdint>

class FunctionConfig
{
public:
  /// Timer
  static const int32_t TSize = 5;
  static const int32_t TEnable = 0;
  static const int32_t TValue = 1;
  static const int32_t TN = 2;
  static const int32_t TS = 3;
  static const int32_t TC = 4;
  /// Counter
  static const int32_t CSize = 5;
  static const int32_t CEnable = 0;
  static const int32_t CValue = 1;
  static const int32_t CN = 2;
  static const int32_t CS = 3;
  static const int32_t CC = 4;
};

#endif // FUNCTIONCONFIG_H

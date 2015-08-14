#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <string>

inline
void logDebug(std::ostream& os, const std::string& message)
{
#ifndef NDEBUG
   os << message << std::endl;
#endif
}

#endif // DEBUG_H

#ifndef LIBINFO_H
#define LIBINFO_H

#include <string>

namespace vplc
{

std::string libName();
std::string libMajorVersion();
std::string libMinorVersion();
std::string libRevisionVersion();
std::string libVersion();

}

#endif // LIBINFO_H

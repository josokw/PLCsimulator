#ifndef LIBINFOVPLC_H
#define LIBINFOVPLC_H

#include <string>

namespace vplc
{

const std::string libName{"VirtualPLC"};
const std::string libMajorVersion{"1"};
const std::string libMinorVersion{"2"};
const std::string libRevisionVersion{"5"};
const std::string libVersion{libMajorVersion
                             + "."  + libMinorVersion
                             + "." + libRevisionVersion};
}

#endif // LIBINFOVPLC_H

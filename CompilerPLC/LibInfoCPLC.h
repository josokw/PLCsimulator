#ifndef LIBINFOCPLC_H
#define LIBINFOCPLC_H

#include <string>

namespace cplc
{

const std::string libName{"CompilerPLC"};
const std::string libMajorVersion{"1"};
const std::string libMinorVersion{"0"};
const std::string libRevisionVersion{"1"};
const std::string libVersion{libMajorVersion
                             + "."  + libMinorVersion
                             + "." + libRevisionVersion};
}

#endif // LIBINFOCPLC_H

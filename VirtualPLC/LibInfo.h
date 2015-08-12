#ifndef LIBINFO_H
#define LIBINFO_H

#include <string>

namespace vplc
{

const std::string libName{"VirtualPLC"};
const std::string libMajorVersion{"1"};
const std::string libMinorVersion{"2"};
const std::string libRevisionVersion{"1"};
const std::string libVersion{libMajorVersion
                             + "."  + libMinorVersion
                             + "." + libRevisionVersion};
}

#endif // LIBINFO_H

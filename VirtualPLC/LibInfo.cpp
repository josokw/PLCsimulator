#include "LibInfo.h"

#define LIBNAME "VirtualPLC"
#define MAJOR_VERSION "1"
#define MINOR_VERSION "2"
#define REVISION_VERSION "1"
#define VERSION MAJOR_VERSION "." MINOR_VERSION "." REVISION_VERSION

std::string vplc::libName() {
  return {LIBNAME};
}

std::string vplc::libMajorVersion() {
  return {MAJOR_VERSION};
}

std::string vplc::libMinorVersion() {
  return {MINOR_VERSION};
}

std::string vplc::libRevisionVersion() {
  return {REVISION_VERSION};
}

std::string vplc::libVersion() {
  return {VERSION};
}

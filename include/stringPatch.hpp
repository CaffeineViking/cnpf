#ifndef STRING_PATCH_H
#define STRING_PATCH_H

#include <string>
#include <sstream>

namespace patch {
  template <typename T>
  inline std::string to_string(const T& in){
    std::stringstream ss;
    ss << in;
    return ss.str();
  }
};

#endif // STRING_PATCH_H

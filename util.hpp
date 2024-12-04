#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include "common.h"

class Util {

public:
static std::string hex2str(BYTE n);
static std::string hex2str(WORD n);
static std::string hex2str_upper(BYTE n);
static std::string hex2str_upper(WORD n);
static std::string dec2str(BYTE n);
static std::string dec2str(WORD n);

private:

};

#endif
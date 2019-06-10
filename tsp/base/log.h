#pragma once

#include <iostream>

using namespace std;

#define enable_logging 1

constexpr static char class_name_[] = "Solution";

#define LOG(message) if (enable_logging) { clog << "[" << class_name_ << "::" << __func__ << "]: " << message << endl; }

#ifdef DEBUG
#define DBG(message) if (enable_logging) { clog << "<" << class_name_ << "::" << __func__ << ">: " << message << endl; }
#else
#define DBG(message)
#endif

#define ASSERT(condition, message) { if (!(condition)) { LOG(message); exit(0); } }

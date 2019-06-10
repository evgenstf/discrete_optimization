#pragma once

long long __state = 43112351;

inline long long rnd() {
  __state = (214013 * __state +  2531011);
  return (__state >> 16) & 0x7FFF;
}

#pragma once


#ifdef DEBUG

#include "log.h"

#include <algorithm>
#include <vector>
#include <chrono>
#include <map>
#include <string>

using Nanoseconds = std::chrono::nanoseconds;

long long get_ns() {
  return std::chrono::time_point_cast<Nanoseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
}

static std::map<std::string, long long> __perf_times_;

void __print_statistics() {
  std::clog << "\n---------PERF STATISTICS--------\n";
  std::vector<std::pair<long long, std::string>> items;
  for (const auto& [target, time] : __perf_times_) {
    items.emplace_back(time, target);
  }
  std::sort(items.rbegin(), items.rend());
  for (const auto& [time, target] : items) {
    auto diff_s = (time) / 1000000000;
    auto diff_ms = (time) / 1000000;
    auto diff_us = (time) / 1000;
    auto diff_ns = (time) % 1000;
    if (diff_s > 0) {
      std::clog << target << " time: " << diff_s << "." << diff_ms << " s\n";
    } else if (diff_ms > 0) {
      std::clog << target << " time: " << diff_ms << "." << diff_us << " ms\n";
    } else if (diff_us > 0) {
      std::clog << target << " time: " << diff_us << "." << diff_ns << " us\n";
    } else {
      std::clog << target << " time: " << diff_ns << " ns\n";
    }
  }
}

class PerfCalculator {
public:
  PerfCalculator(std::string target):
      target_(std::move(target)),
      start_time_(get_ns()) {}

  ~PerfCalculator() {
    auto current_time = get_ns();
    __perf_times_[target_] += (get_ns() - start_time_);
  }

  
private:
  const std::string target_;
  const long long start_time_;

  constexpr static char class_name_[] = "PerfCalculator";

};

#define PRINT_PERF_STATISTICS() __print_statistics();

#define PERF() PerfCalculator __perf_calculator(class_name_ + std::string("::") + std::string(__func__));

#else
#define PERF(message)
#define PRINT_PERF_STATISTICS(message)
#endif

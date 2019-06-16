// evgenii kazakov
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
#pragma once

long long __state = 43112351;

inline long long rnd() {
  __state = (214013 * __state +  2531011);
  return (__state >> 16) & 0x7FFF;
}
#pragma once


#ifdef DEBUG


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
    auto diff_s = (time) / 1000000000 % 1000;
    auto diff_ms = (time) / 1000000 % 1000;
    auto diff_us = (time) / 1000 % 1000;
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
#define CPERF(target) PerfCalculator __perf_calculator(class_name_ + std::string("::") + std::string(__func__) + std::string("::") + target);

#else
#define PERF(message)
#define CPERF(message)
#define PRINT_PERF_STATISTICS(message)
#endif
#pragma once
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <vector>
#include <set>

using namespace std;

double dist(double* l, double* r) {
  return std::hypot(l[0] - r[0], l[1] - r[1]);
}

double dist(const pair<double, double>& l, const pair<double, double>& r) {
  return std::hypot(l.first - r.first, l.second- r.second);
}

template<int kSize>
class Graph {
public: 

auto distation() const { return distation_; }

auto size() const { return N; }

void init() {
  PERF();
  ASSERT(scanf("%d", &N), "cannot read from input");
  for (int i = 0; i < N; ++i) {
    ASSERT(scanf("%lf %lf", &points_[i].first.first, &points_[i].first.second), "cannot read from input");
    points_[i].second = i;
  }
  DBG("read grap size: " << N);
}

void print() {
  printf("%lf\n", distation_);
  for (int i = 0; i < N; ++i) {
    printf("%d ", points_[i].second + 1);
  }
}

void set_rand_permutation() {
  PERF();
  for (int i = 0; i < N; ++i) {
    perm_[i] = i;
  }
  for (int i = 0; i < N; ++i) {
    std::swap(perm_[i], perm_[(i + rnd()) % N]);
  }
  perm_cnt = N;
}

void set_dummy_permutation() {
  PERF();
  for (size_t i = 0; i < N; i++) {
    perm_[i] = i;
  }
  perm_cnt = N;
}

void set_greed_permutation() {
  PERF();

  set_dummy_permutation();

  recalculate_distation();
  DBG("dist: " << distation_);

  size_t start =
      std::min_element(
          points_,
          points_ + N,
          [](const auto& lhs, const auto& rhs) {
              return (lhs.first.first + rhs.first.second) < (rhs.first.first + rhs.first.second);
          }
      ) - points_;

  std::swap(points_[start], points_[0]);
  for (size_t i = 0; i < N - 1; i++) {
    size_t mini = i + 1;
    double minx = dist(points_[i].first, points_[i + 1].first);
    for (size_t j = i + 2; j < N; j++) {
      auto qwe = dist(points_[i].first, points_[j].first);
      if (minx > qwe) {
        minx = qwe;
        mini = j;
      }
    }
    std::swap(points_[i + 1], points_[mini]);
  }
}

void set_2_optimal_permutation() {
  PERF();
  dsu_init();
  int cnt = 0;
  {
    CPERF("init");
  for (int i = 0; i < N; ++i) {
    used_[i] = false;
    qwe_cnt = 0;
    for (int j = i + 1; j < N; ++j) {
      qwe[qwe_cnt++] = make_pair(dist(points_[i], points_[j]), make_pair(i, j));
      if (qwe_cnt >= sqrt(kSize)) {
        sort(qwe, qwe + qwe_cnt);
        qwe_cnt = 10;
      }
    }
    for (int j = 0; j < 10; ++j) {
      edges_[cnt++] = qwe[j];
    }
  }
  }
  sort(edges_, edges_ + cnt);
  DBG("sorted");
  G.resize(N);
  for (int i = 0; i < cnt; ++i) {
    double dd = edges_[i].first;
    int a = edges_[i].second.first;
    int b = edges_[i].second.second;

    if (dsu_what(a) != dsu_what(b)) {
      dsu_merge(a, b);
      G[a].push_back(b);
      G[b].push_back(a);
    }
  }

  for (int i = 0; i < N; ++i) {
    if (!used_[i]) {
      DBG("go from i: " << i)
      go(i);
    }
  }
}

void recalculate_distation() {
  PERF();
  distation_ = 0;
  for (int i = 0; i < N; ++i) {
    distation_ += dist(points_[i].first, points_[(i + 1) % N].first);
  }
}

double swap_profit(int a, int b) {
  PERF();

  if (((a - 1) % N + N) % N == b) {
    return 0;
  }

  return
      (
        dist(points_[a].first, points_[((a - 1) % N + N) % N].first) -
        dist(points_[a].first, points_[((b + 1) % N + N) % N].first)
      ) +
      (
        dist(points_[b].first, points_[((b + 1) % N + N) % N].first) -
        dist(points_[b].first, points_[((a - 1) % N + N) % N].first)
      );
}

void swap(int a, int b, double profit) {
  PERF();
  if (b < a) {
    b += N;
  }
  for (int i = a, j = b; i < j; ++i, --j) {
    std::swap(points_[i % N], points_[j % N]);
  }
  distation_ -= profit;
}

private:
  inline void dsu_init() {
    for (int i = 0; i < N; ++i) {
      parent_[i] = i;
      rnk_[i] = 0;
    }
  }

  inline int dsu_what(int a) {
    if (parent_[a] == a) {
      return parent_[a];
    }
    return parent_[a] = dsu_what(parent_[a]);
  }

  inline void dsu_merge(int a, int b) {
    a = dsu_what(a);
    b = dsu_what(b);
    if (rnk_[a] > rnk_[b]) {
      parent_[b] = a;
      ++rnk_[a];
    } else {
      parent_[a] = b;
      ++rnk_[b];
    }
  }

  int N;
  pair<pair<double, double>, int> points_[kSize];

  int perm_cnt{0};
  int perm_[kSize];
  
  double distation_{0};

  int parent_[kSize];
  int rnk_[kSize];
  pair<double, pair<int, int>> edges_[kSize * 10];


  vector<vector<int>> G;
  bool used_[kSize];

  vector<int> way_;
  int qwe_cnt;
  pair<double, pair<int, int>> qwe[kSize];


  void go(int v) {
    used_[v] = true;
    perm_[perm_cnt++] = v;
    for (auto to : G[v]) {
      if (!used_[to]) {
        go(to);
      }
    }
  }

  constexpr static char class_name_[] = "Graph";
};
#include <ctime>

Graph<20000> graph;

std::clock_t start;

double get_duration() {
  return ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
}

int main() {
  start = std::clock();

  graph.init();

  //graph.set_rand_permutation();
  graph.set_greed_permutation();
  //graph.set_2_optimal_permutation();
  graph.recalculate_distation();



  int improve_cnt = 0;
  int total_cnt = 0;
  double temp = 10;
  double coolingRate = 0.9999999;
  while (temp > 1) {
    CPERF("main while");
    ++total_cnt;
    auto a = rnd() % graph.size();
    auto b = rnd() % graph.size();
    auto profit = graph.swap_profit(a, b);

    double rnd_prob = (1.0 * (rnd() % 10000)) / 10000.0 + 0.1;

    if (profit > 0) {
      graph.swap(a, b, profit);
      ++improve_cnt;
    }
    else if (
        profit != 0 &&
        rnd_prob < exp((profit) / temp)
    ) {
      DBG(
          "distation: " << graph.distation() <<
          " temp: " << temp <<
          " a: " << a <<
          " b: " << b << 
          " accept prob: " << exp((profit) / temp) <<
          " rnd_prob: " << rnd_prob <<
          " profit: " << profit
      );
      graph.swap(a, b, profit);
    }

    temp *= coolingRate;
  }

  while (get_duration() < 9.985) {
    for (int i = 0; i < 1000; ++i) {
      auto a = rnd() % graph.size();
      auto b = rnd() % graph.size();
      auto profit = graph.swap_profit(a, b);

      double rnd_prob = (1.0 * (rnd() % 10000)) / 10000.0 + 0.1;

      if (profit > 0) {
        graph.swap(a, b, profit);
        ++improve_cnt;
      }
    }
  }

  DBG("improve/total: " << improve_cnt << "/" << total_cnt << " part: " << 1.0 * improve_cnt / total_cnt);

  //std::cout << "final: " << graph.distation() << std::endl;;
  graph.recalculate_distation();
  //std::cout << "distation 2: " << graph.distation() << std::endl;
  //

  graph.print();


  PRINT_PERF_STATISTICS();
}

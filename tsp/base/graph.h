#pragma once
#include "log.h"
#include "perf.h"
#include "rand.h"
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

template<int kSize>
class Graph {
public: 

auto distation() const { return distation_; }

auto size() const { return N; }

void init() {
  PERF();
  ASSERT(scanf("%d", &N), "cannot read from input");
  for (int i = 0; i < N; ++i) {
    ASSERT(scanf("%lf %lf", &points_[i][0], &points_[i][1]), "cannot read from input");
  }
  DBG("read grap size: " << N);
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
              return (lhs[0] + rhs[1]) < (rhs[0] + rhs[1]);
          }
      ) - points_;

  std::swap(perm_[start], perm_[0]);
  for (size_t i = 0; i < N - 1; i++) {
    size_t mini = i + 1;
    double minx = dist(points_[perm_[i]], points_[perm_[i + 1]]);
    for (size_t j = i + 2; j < N; j++) {
      auto qwe = dist(points_[perm_[i]], points_[perm_[j]]);
      if (minx > qwe) {
        minx = qwe;
        mini = j;
      }
    }
    std::swap(perm_[i + 1], perm_[mini]);
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
    distation_ += dist(points_[perm_[i]], points_[perm_[(i + 1) % N]]);
  }
}

double swap_profit(int a, int b) {
  PERF();

  if (((a - 1) % N + N) % N == b) {
    return 0;
  }

  return
      (
        dist(points_[perm_[a]], points_[perm_[((a - 1) % N + N) % N]]) -
        dist(points_[perm_[a]], points_[perm_[((b + 1) % N + N) % N]])
      ) +
      (
        dist(points_[perm_[b]], points_[perm_[((b + 1) % N + N) % N]]) -
        dist(points_[perm_[b]], points_[perm_[((a - 1) % N + N) % N]])
      );
}

void swap(int a, int b, double profit) {
  PERF();
  if (b < a) {
    b += N;
  }
  for (int i = a, j = b; i < j; ++i, --j) {
    std::swap(perm_[i % N], perm_[j % N]);
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
  double points_[kSize][2];

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

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
    left_[i] = -1;
    right_[i] = -1;
  }
  DBG("read grap size: " << N);
}

void set_rand_permutation() {
  PERF();
  std::vector<int> perm(N);
  for (int i = 0; i < N; ++i) {
    perm[i] = i;
  }
  /*
  for (int i = 0; i < N; ++i) {
    std::swap(perm[i], perm[(i + rnd()) % N]);
  }
  */

  for (int i = 0; i < N; ++i) {
    int v = perm[i];
    left_[v] = perm[((i - 1) + N) % N];
    right_[v] = perm[(i + 1) % N];
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


  for (int i = 0; i < N; ++i) {
    int v = way_[i];
    left_[v] = way_[((i - 1) + N) % N];
    right_[v] = way_[(i + 1) % N];
  }
}

void set_v(int v, short position) {
  PERF();
  left_[position] = v;
  right_[v] = position;
}

void recalculate_distation() {
  PERF();
  int now = 0;
  distation_ = 0;
  do {
    distation_ += dist(points_[now], points_[right_[now]]);
    now = right_[now];
  } while (now != 0);
}

double swap_profit(int a, int b) {
  PERF();
  if (left_[a] == b || right_[a] == b || a == b) {
    return 0;
  }
  /*
  for (int i = 0; i < N; ++i) {
    std::clog << "i: " << i << " point: " << points_[i][0] << ' ' << points_[i][1] << std::endl;
  }
  std::clog << "---------\n";
  std::clog << "a : " << a << " b: " << b << std::endl;
  std::clog << "a : " << points_[a][0] << ' ' << points_[a][1] <<
      " b: " << points_[b][0] << ' ' << points_[b][1] << std::endl;
  std::clog << "right_a : " << right_[a] << " left_b: " << left_[b] << std::endl;

  std::clog << "right_a : " << points_[right_[a]][0] << ' ' << points_[right_[a]][1] <<
      " left_b: " << points_[left_[b]][0] << ' ' << points_[left_[b]][1] << std::endl;

  std::clog << "a : old_dist: " << dist(points_[a], points_[right_[a]]) <<
      " new_dist: " << dist(points_[a], points_[left_[b]]) << std::endl;
  */
  return (
      (
        dist(points_[a], points_[left_[a]]) +
        dist(points_[a], points_[right_[a]])
      ) - (
        dist(points_[a], points_[left_[b]]) +
        dist(points_[a], points_[right_[b]])
      )
  ) + (
      (
        dist(points_[b], points_[left_[b]]) +
        dist(points_[b], points_[right_[b]])
      ) - (
        dist(points_[b], points_[left_[a]]) +
        dist(points_[b], points_[right_[a]])
      )
  );
}

void swap(int a, int b, double profit) {
  PERF();
  std::swap(left_[a], left_[b]);
  std::swap(right_[a], right_[b]);

  left_[right_[b]] = b;
  right_[left_[b]] = b;

  left_[right_[a]] = a;
  right_[left_[a]] = a;

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
  int left_[kSize];
  int right_[kSize];
  
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
    way_.push_back(v);
    for (auto to : G[v]) {
      if (!used_[to]) {
        go(to);
      }
    }
  }

  constexpr static char class_name_[] = "Graph";
};

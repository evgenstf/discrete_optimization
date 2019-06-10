#pragma once
#include "log.h"
#include "perf.h"
#include "rand.h"
#include <iostream>
#include <cstdio>
#include <cmath>

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
  int N;
  double points_[kSize][2];
  int left_[kSize];
  int right_[kSize];
  
  double distation_{0};

  constexpr static char class_name_[] = "Graph";
};

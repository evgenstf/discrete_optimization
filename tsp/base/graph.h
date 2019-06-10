#pragma once
#include "log.h"
#include "perf.h"
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
    ASSERT(scanf("%lf %lf", &points_[i][1], &points_[i][2]), "cannot read from input");
    position_by_v_[i] = -1;
    v_by_position_[i] = -1;
  }
  DBG("read grap size: " << N);
}

void set_v(short v, short position) {
  PERF();
  v_by_position_[position] = v;
  position_by_v_[v] = position;
}

void recalculate_distation() {
  PERF();
  short first = -1;
  short previous = -1;
  distation_ = 0;
  for (short v = 0; v < kSize; ++v) {
    if (v_by_position_[v] != -1) {
      if (first == -1) {
        first = v;
      }
      if (previous != -1) {
        distation_ += dist(points_[previous], points_[v]);
      }
      previous = v;
    }
  }
}

private:
  int N;
  double points_[kSize][2];
  short position_by_v_[kSize];
  short v_by_position_[kSize];
  
  double distation_{0};

  constexpr static char class_name_[] = "Graph";
};

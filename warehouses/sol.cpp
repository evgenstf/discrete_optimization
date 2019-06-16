#include <cstdio>
#include <iostream>
#include <unistd.h>

#include <algorithm>
#include <vector>
#include <chrono>
#include <map>
#include <string>
#include "glpk.h"


#ifdef DEBUG



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



struct Warehouse {
  double open_cost;
  double capacity;
};

vector<Warehouse> warehouses;
vector<double> demands;


vector<vector<double>> use_cost;


using Glpk = glp_prob;
using Parameters = glp_iocp;

namespace {

void init_cols(Glpk* glpk, int n, int m) {
  glp_add_cols(glpk, n * (m + 1));

  for (int i = 0; i < n; ++i) {
    glp_set_obj_coef(glpk, n * m + i + 1, warehouses[i].open_cost);
    glp_set_col_kind(glpk, n * m + i + 1, GLP_BV);
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      glp_set_obj_coef(glpk, i * m + j + 1, use_cost[i][j]);
      glp_set_col_bnds(glpk, i * m + j + 1, GLP_LO, 0, 0);
    }
  }
}

void init_rows(Glpk* glpk, int n, int m) {
  glp_add_rows(glpk, n + m);

  for (int i = 0; i < m; ++i) {
    glp_set_row_bnds(glpk, n + i + 1, GLP_FX, 1, 1);
  }

  for (int i = 0; i < n; ++i) {
    glp_set_row_bnds(glpk, i + 1, GLP_UP, 0, 0);
  }
}

}  // namespace

Glpk* init_glpk(int n, int m) {
  DBG("start init_glpk");
  PERF();
  Glpk* result = glp_create_prob();

  init_rows(result, n, m);
  init_cols(result, n, m);

  glp_set_obj_dir(result, GLP_MIN);

  return result;
}

Parameters init_parameters() {
  Parameters result;

  glp_init_iocp(&result);


  result.binarize = 1;
  result.bt_tech = GLP_BT_BPH;
  result.clq_cuts = 1;
  result.cov_cuts = 1;
  result.mir_cuts = 1;
  result.msg_lev = GLP_MSG_OFF;
  result.presolve = 1;
  result.tm_lim = 10000;

  return result;
}


void init_matrix(Glpk* glpk, int n, int m) {
  DBG("start init_matrix");
  PERF();
  int len = n * (2 * m + 1);

  int qwe[40000], asd[40000];
  double zxc[40000];

  int cnt = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      asd[cnt + 1] = m * i + j + 1;
      qwe[cnt + 1] = i + 1;
      zxc[cnt + 1] = demands[j];
      ++cnt;
    }
    asd[cnt + 1] = n * m + i + 1;
    qwe[cnt + 1] = i + 1;
    zxc[cnt + 1] = -warehouses[i].capacity;
    ++cnt;
  }
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      asd[cnt + 1] = m * j + i + 1;
      qwe[cnt + 1] = n + i + 1;
      zxc[cnt + 1] = 1;
      ++cnt;
    }
  }
  glp_load_matrix(glpk, len, qwe, asd, zxc);
}

void start_glpk(Glpk* glpk, Parameters parameters) {
  DBG("starting glpk...");
  PERF();
  int dupp = dup(1);
  close(1);
  glp_intopt(glpk, &parameters);
  dup2(dupp, 1);
}


int main() {
  int n, m;
  ASSERT(scanf("%d%d", &n, &m), "cannot read from input");

  warehouses.resize(n);
  demands.resize(m);
  use_cost.assign(n, vector<double>(m));


  for (auto& ws : warehouses) {
    cin >> ws.capacity >> ws.open_cost;
  }

  for (auto& x : demands) {
    cin >> x;
  }


  for (auto& row : use_cost) {
    for (auto& x : row) {
      cin >> x;
    }
  }
  
  DBG("read warehouses: " << n << " demands: " << m);

  auto* glpk = init_glpk(n, m);
  DBG("glpk inited");


  init_matrix(glpk, n, m);
  DBG("matrix inited");


  start_glpk(glpk, init_parameters());
  DBG("glpk finished");


  vector<int> answer;

  for (int i = 0; i < n; ++i) {
    if (glp_mip_col_val(glpk, n * m + i + 1)) {
      answer.push_back(i);
    }
  }
  
  cout << answer.size() << endl;

  for (auto x : answer) {
    cout << x + 1 << ' ';
  }

  cout << endl;

  for (auto x : answer) {
    for (int i = 0; i < m; ++i) {
      cout << glp_mip_col_val(glpk, x * m + i + 1) << ' ';
    }
    cout << endl;
  }

  cout << endl;


  PRINT_PERF_STATISTICS();
}

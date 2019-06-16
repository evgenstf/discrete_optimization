#include "base/graph.h"
#include "base/rand.h"

Graph<20000> graph;

int main() {
  graph.init();

  //graph.set_rand_permutation();
  graph.set_greed_permutation();
  //graph.set_2_optimal_permutation();
  graph.recalculate_distation();

  std::cout << "distation: " << graph.distation() << std::endl;


  int improve_cnt = 0;
  int total_cnt = 0;
  double temp = 10;
  double coolingRate = 0.9999999;
  while (temp > 1) {
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
  DBG("improve/total: " << improve_cnt << "/" << total_cnt << " part: " << 1.0 * improve_cnt / total_cnt);

  std::cout << "final: " << graph.distation() << std::endl;;
  graph.recalculate_distation();
  std::cout << "distation 2: " << graph.distation() << std::endl;

  PRINT_PERF_STATISTICS();
}

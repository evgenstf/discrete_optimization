#include "base/graph.h"
#include "base/rand.h"


int main() {
  Graph<20000> graph;
  graph.init();

  graph.set_rand_permutation();
  graph.recalculate_distation();

  std::cout << "distation: " << graph.distation() << std::endl;

  const int kIterationsCount = 1e8;
  for (int i = 0; i < kIterationsCount; ++i) {
    auto a = rnd() % graph.size();
    auto b = rnd() % graph.size();
    auto profit = graph.swap_profit(a, b);
    if (profit > 0) {
      //DBG("swap: " << a << ' ' << b << " profit: " << profit << " new distance: " << graph.distation() - profit);
      graph.swap(a, b, profit);
    }
  }

  graph.recalculate_distation();
  std::cout << "distation 2: " << graph.distation() << std::endl;

  PRINT_PERF_STATISTICS();
}

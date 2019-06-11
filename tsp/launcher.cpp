#include "base/graph.h"
#include "base/rand.h"

Graph<20000> graph;

int main() {
  graph.init();

  graph.set_2_optimal_permutation();
  graph.recalculate_distation();

  std::cout << "distation: " << graph.distation() << std::endl;

  int kMaxTemperature = 2e9;
  for (int temperature = kMaxTemperature; temperature > 0; --temperature) {
    auto a = rnd() % graph.size();
    auto b = rnd() % graph.size();
    auto profit = graph.swap_profit(a, b);
    if ((profit > 0)) {
      graph.swap(a, b, profit);
    }
    if (temperature % 1000000 == 0) {
      DBG("distation: " << graph.distation());
    }
    /*
    else if (
        profit != 0 &&
        (-profit) / graph.distation() < (0.00001 * temperature / kMaxTemperature)
    ) {
      DBG(
          "profit: " << profit <<
          " ratio: " << (-profit) / graph.distation() <<
          " threshold: " << (0.0001 * temperature / kMaxTemperature)
      );
      graph.swap(a, b, profit);
    }
  */
  }

  graph.recalculate_distation();
  std::cout << "distation 2: " << graph.distation() << std::endl;

  PRINT_PERF_STATISTICS();
}

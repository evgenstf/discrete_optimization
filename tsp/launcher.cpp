#include "base/graph.h"


int main() {
  Graph<20000> graph;
  graph.init();

  for (int i = 0; i < graph.size(); ++i) {
    graph.set_v(i, i);
  }

  graph.recalculate_distation();

  std::cout << "distation: " << graph.distation() << std::endl;

  PRINT_PERF_STATISTICS();
}

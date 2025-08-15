#include <adf.h>
#include "histogram_graph.h"

using namespace adf;

histogram_parallel_graph  mygraph;

#if defined(__AIESIM__) || defined(__X86SIM__)
  int main(void) {
    mygraph.init();
    mygraph.run(1);
    mygraph.end();
    return 0;
  }
#endif

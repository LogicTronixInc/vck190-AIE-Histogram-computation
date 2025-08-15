#include "histogram_kernel.h"
#include <adf.h>
#include <aie_api/aie.hpp>
#include <aie_api/aie_adf.hpp>
#include <aie_api/utils.hpp>
using namespace adf;

#define SIZE 76800 //640*480/4

// template<int SIZE>
void histogram_partial(input_stream<uint32>* data_in,
                       output_buffer<int32, extents<256>> & __restrict hist_out) {
  int32 histogram[256] = {0};
  auto outIter = aie::begin(hist_out);

  for (int i = 0; i < SIZE; ++i) {
    uint8 val = readincr(data_in);
    histogram[val]++;
  }
  for (int i = 0; i < 256; ++i) {
   *outIter++ = histogram[i];
  }
}

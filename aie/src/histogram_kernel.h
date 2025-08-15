#ifndef FUNCTION_KERNELS_H
#define FUNCTION_KERNELS_H

// void aie_vadd_stream(input_stream_int32 *in0, input_stream_int32 *in1, output_stream_int32 *out);
#include <adf.h>

using namespace adf;

void histogram_partial(input_stream<uint32>* data_in, output_buffer<int32, extents<256>> & __restrict hist_out);
void histo_accum_buffer(input_buffer<int32, extents<256>> & __restrict in0, input_buffer<int32, extents<256>> & __restrict in1, output_buffer<int32, extents<256>> & __restrict out);
void histo_accum_stream(input_buffer<int32, extents<256>> & __restrict in0,
 input_buffer<int32, extents<256>> & __restrict in1, 
 output_stream<int32>* out);

#endif 
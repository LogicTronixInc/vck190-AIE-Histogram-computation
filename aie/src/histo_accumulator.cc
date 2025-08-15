#include <aie_api/aie.hpp>
#include <aie_api/aie_adf.hpp>
#include <aie_api/utils.hpp>

using namespace adf;

void histo_accum_buffer(input_buffer<int32, extents<256>> & __restrict in0,
 input_buffer<int32, extents<256>> & __restrict in1, 
 output_buffer<int32, extents<256>> & __restrict out){
    auto in0Iter = aie::begin_vector<8>(in0);
    auto in1Iter = aie::begin_vector<8>(in1);
    auto outIter = aie::begin_vector<8>(out);
    
    aie::vector<int32,8> datain0;
    aie::vector<int32,8> datain1;
	aie::vector<int32,8> dataout;

    for (unsigned int i=0; i< 256/8; i++) {
        datain0 = *in0Iter++;
        datain1 = *in1Iter++;
        dataout = aie::add(datain0, datain1);
        *outIter++ = dataout;
    }
}
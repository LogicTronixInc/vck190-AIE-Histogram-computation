#pragma once

#include <adf.h>
#include "histogram_kernel.h"

using namespace adf;

class histogram_parallel_graph : public graph {
public:
  input_plio in0, in1, in2, in3;
  output_plio  acc_out2;

  kernel k0, k1, k2, k3, histo_acc0, histo_acc1, histo_acc2;

  histogram_parallel_graph() {
    // Input from files
    in0 = input_plio::create("input0", plio_32_bits, "data/input0.txt");
    in1 = input_plio::create("input1", plio_32_bits, "data/input1.txt");
    in2 = input_plio::create("input2", plio_32_bits, "data/input2.txt");
    in3 = input_plio::create("input3", plio_32_bits, "data/input3.txt");

     acc_out2 = output_plio::create("acc_output2", plio_32_bits, "data/acc_output2.txt");

    // Kernel instantiation
    k0 = kernel::create(histogram_partial);
    k1 = kernel::create(histogram_partial);
    k2 = kernel::create(histogram_partial);
    k3 = kernel::create(histogram_partial);

    histo_acc0 = kernel::create(histo_accum_buffer);
    histo_acc1 = kernel::create(histo_accum_buffer);
    histo_acc2 = kernel::create(histo_accum_stream);

    // Connect
    connect<stream>(in0.out[0], k0.in[0]);
    connect<stream>(in1.out[0], k1.in[0]);
    connect<stream>(in2.out[0], k2.in[0]);
    connect<stream>(in3.out[0], k3.in[0]);


    connect(k0.out[0], histo_acc0.in[0]);
    connect(k1.out[0], histo_acc0.in[1]);
    connect(k2.out[0], histo_acc1.in[0]);
    connect(k3.out[0], histo_acc1.in[1]);

    connect(histo_acc0.out[0], histo_acc2.in[0]);
    connect(histo_acc1.out[0], histo_acc2.in[1]);

    connect<stream>(histo_acc2.out[0], acc_out2.in[0]);

    // Kernel source
    source(k0) = "histogram_kernel.cc";
    source(k1) = "histogram_kernel.cc";
    source(k2) = "histogram_kernel.cc";
    source(k3) = "histogram_kernel.cc";

    source(histo_acc0) = "histo_accumulator.cc";
    source(histo_acc1) = "histo_accumulator.cc";
    source(histo_acc2) = "histo_accumulator_stream_out.cc";

    // Runtime ratio
    runtime<ratio>(k0) = 0.9;
    runtime<ratio>(k1) = 0.9;
    runtime<ratio>(k2) = 0.9;
    runtime<ratio>(k3) = 0.9;
    runtime<ratio>(histo_acc0) = 0.9;
    runtime<ratio>(histo_acc1) = 0.9;
    runtime<ratio>(histo_acc2) = 0.9;
  }
};

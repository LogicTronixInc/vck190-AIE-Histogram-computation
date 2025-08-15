

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fstream>
#include "input.h"
#include "golden.h"
#include <cstring>

#include <iostream>
#include <chrono>

#include "experimental/xrt_kernel.h"
#include "experimental/xrt_graph.h"

#include "adf/adf_api/XRTConfig.h"

#define INPUT_SIZE  76800
#define OUTPUT_SIZE 256

#define INPUT_OFFSET 76800


#define SAMPLES 256

int main(int argc, char ** argv)
{

	//////////////////////////////////////////
	// Open xclbin
	//////////////////////////////////////////	

	char* xclbinFile=argv[1];
	auto device = xrt::device(0);
    if(device == nullptr)
    throw std::runtime_error("No valid device handle found. Make sure using right xclOpen index.");
    auto xclbin_uuid = device.load_xclbin(xclbinFile);

    printf("loaded xclbin\n");

    int sizeIn = INPUT_SIZE;
	int sizeOut = OUTPUT_SIZE;

	//////////////////////////////////////////
	// input memory 0
	// Allocating the input size of sizeIn to MM2S
	// MM2S module transfers input data from PL to the AI Engine
	//////////////////////////////////////////
		
	auto in0_bohdl = xrt::bo(device, sizeIn * sizeof(int32_t), 0, 0);
	auto in0_bomapped = in0_bohdl.map<uint32_t*>();
    printf("input buffer mapped \n");
	memcpy(in0_bomapped, dataInput, sizeIn * sizeof(int32_t) );
	printf("Input memory virtual addr 0x%px\n", in0_bomapped);

	in0_bohdl.sync(XCL_BO_SYNC_BO_TO_DEVICE);
	
	////////////////////////////////////////////////////////
	// mm2s ip  - Creating kernel handle using xrt::kernel API
	///////////////////////////////////////////////////////	
	
	auto mm2s0_khdl = xrt::kernel(device, xclbin_uuid, "mm2s:{mm2s_1}");
	auto mm2s0_rhdl = mm2s0_khdl(in0_bohdl, nullptr, sizeIn);
	printf("run mm2s_1\n");

	/////////////////////////////////////////
	// input memory 1
	// Allocating the input size of sizeIn to MM2S
	// MM2S module transfers input data from PL to the AI Engine
	//////////////////////////////////////////
		
	auto in1_bohdl = xrt::bo(device, sizeIn * sizeof(int32_t), 0, 0);
	auto in1_bomapped = in1_bohdl.map<uint32_t*>();
    printf("input buffer mapped \n");
	memcpy(in1_bomapped, dataInput+INPUT_OFFSET, sizeIn * sizeof(int32_t) );
	printf("Input memory virtual addr 0x%px\n", in1_bomapped);

	in1_bohdl.sync(XCL_BO_SYNC_BO_TO_DEVICE);
	

	////////////////////////////////////////////////////////
	// mm2s ip  - Creating kernel handle using xrt::kernel API
	///////////////////////////////////////////////////////	
	
	auto mm2s1_khdl = xrt::kernel(device, xclbin_uuid, "mm2s:{mm2s_2}");
	auto mm2s1_rhdl = mm2s1_khdl(in1_bohdl, nullptr, sizeIn);
	printf("run mm2s_2\n");	

	//////////////////////////////////////////
	// input memory 2
	// Allocating the input size of sizeIn to MM2S
	// MM2S module transfers input data from PL to the AI Engine
	//////////////////////////////////////////
		
	auto in2_bohdl = xrt::bo(device, sizeIn * sizeof(int32_t), 0, 0);
	auto in2_bomapped = in2_bohdl.map<uint32_t*>();
    printf("input buffer mapped \n");
	memcpy(in2_bomapped, dataInput+INPUT_OFFSET*2, sizeIn * sizeof(int32_t) );
	printf("Input memory virtual addr 0x%px\n", in2_bomapped);

	in2_bohdl.sync(XCL_BO_SYNC_BO_TO_DEVICE);	

	////////////////////////////////////////////////////////
	// mm2s ip  - Creating kernel handle using xrt::kernel API
	///////////////////////////////////////////////////////	
	
	auto mm2s2_khdl = xrt::kernel(device, xclbin_uuid, "mm2s:{mm2s_3}");
	auto mm2s2_rhdl = mm2s2_khdl(in2_bohdl, nullptr, sizeIn);
	printf("run mm2s_3\n");


	//////////////////////////////////////////
	// input memory 3
	// Allocating the input size of sizeIn to MM2S
	// MM2S module transfers input data from PL to the AI Engine
	//////////////////////////////////////////
		
	auto in3_bohdl = xrt::bo(device, sizeIn * sizeof(int32_t), 0, 0);
	auto in3_bomapped = in3_bohdl.map<uint32_t*>();
    printf("input buffer mapped \n");
	memcpy(in3_bomapped, dataInput+INPUT_OFFSET*3, sizeIn * sizeof(int32_t) );
	printf("Input memory virtual addr 0x%px\n", in3_bomapped);

	in3_bohdl.sync(XCL_BO_SYNC_BO_TO_DEVICE);

	//////////////////////////////////////////
	// output memory
	// Allocating the output size of sizeOut to S2MM
	// S2MM module receives the output data from AI Engine 
	//////////////////////////////////////////
	
	auto out3_bohdl = xrt::bo(device, sizeOut * sizeof(int), 0, 0);
	auto out3_bomapped = out3_bohdl.map<uint32_t*>();
	memset(out3_bomapped, 0xABCDEF00, sizeOut * sizeof(int));
	printf("Output memory virtual addr 0x%px\n", out3_bomapped);

	////////////////////////////////////////////////////////
	// mm2s ip - Creating kernel handle using xrt::kernel API
	///////////////////////////////////////////////////////	
	
	auto mm2s3_khdl = xrt::kernel(device, xclbin_uuid, "mm2s:{mm2s_4}");
	auto mm2s3_rhdl = mm2s3_khdl(in3_bohdl, nullptr, sizeIn);
	printf("run mm2s_4\n");

	////////////////////////////////////////////////////////
	// s2mm ip - Creating kernel handle using xrt::kernel API
	///////////////////////////////////////////////////////		
	
	auto s2mm3_khdl = xrt::kernel(device, xclbin_uuid, "s2mm:{s2mm_1}");
	auto s2mm3_rhdl = s2mm3_khdl(out3_bohdl, nullptr, sizeOut);
	printf("run s2mm4\n");


	//////////////////////////////////////////
	// graph execution for AIE
	//////////////////////////////////////////	
	
	//Obtains the graph handle from the XCLBIN that is loaded into the device
	auto cghdl = xrt::graph(device,xclbin_uuid,"mygraph");
	
	printf("graph run\n");
	//Run th graph for 1 iteration
	cghdl.run(1);
	
    printf("waiting for aie graph run to end\n");
	//Graph end
	cghdl.end();
	printf("graph end\n");	

	
	//////////////////////////////////////////
	// wait for mm2s1 done
	//////////////////////////////////////////	
	
	mm2s0_rhdl.wait();
	

	//////////////////////////////////////////
	// wait for mm2s2 done
	//////////////////////////////////////////	
	
	mm2s1_rhdl.wait();

	//////////////////////////////////////////
	// wait for mm2s3 done
	//////////////////////////////////////////	
	
	mm2s2_rhdl.wait();

	//////////////////////////////////////////
	// wait for mm2s4 done
	//////////////////////////////////////////	
	
	mm2s3_rhdl.wait();

	//////////////////////////////////////////
	// wait for s2mm done
	//////////////////////////////////////////	
	
	s2mm3_rhdl.wait();

	out3_bohdl.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

	
	//////////////////////////////////////////
	// Comparing the execution data to the golden data
	//////////////////////////////////////////	
	
	int errorCount = 0;
	{
		for (int i = 0; i < sizeOut; i++)
		{
				if ((signed)out3_bomapped[i] != golden[i])
				{
					printf("Error found @ %d, %d != %d\n", i, out3_bomapped[i], golden[i]);
					errorCount++;
				}
		}

		if (errorCount)
			printf("Test failed with %d errors\n", errorCount);
		else
			printf("TEST PASSED\n");
	}

	for (int i = 0; i < sizeOut; i++)
		{
				printf("Count @ %d, %d \n", i, out3_bomapped[i]);
		}

	
    
	std::cout << "Releasing remaining XRT objects...\n";	
	
	return errorCount;
}

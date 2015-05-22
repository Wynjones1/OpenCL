#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "config.h"
#include "support.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

int main()
{
	cl_device_id     device_id     = NULL;
	cl_context       context       = NULL;
	cl_command_queue cq            = NULL;
	cl_program       program       = NULL;
	cl_platform_id   platform_id   = NULL;
	cl_int           ret           = 0;

	std::vector<cl_kernel> kernels;
	std::vector<cl_mem>    mem_objs;

	setup(&context, &platform_id, &device_id, &cq);

	/* Create Kernel Program from the source */
	program = create_program_from_file(context, KERNEL_PATH "/glm_test0.cl");
	/* Build Kernel Program */
	build_program(program, 1, &device_id, NULL, NULL, NULL);
	/* Create OpenCL Kernel */
	kernels.push_back(clCreateKernel(program, "glm_input", &ret));

#if 0
	int a, b;
	a = 10;
	clSetKernelArg(kernels[0], 0, sizeof(int), (void*)&a);
	clSetKernelArg(kernels[0], 0, sizeof(int), (void*)&b);
#else
	glm::vec4 input[4];
	glm::vec4 output[4];
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			input[i][j] = static_cast<float>(i * 4 + j);
		}
	}
	mem_objs.push_back(clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(input), input, &ret));
	mem_objs.push_back(clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(output), NULL, &ret));

	clSetKernelArg(kernels[0], 0, sizeof(mem_objs[0]), &mem_objs[0]);
	clSetKernelArg(kernels[0], 1, sizeof(mem_objs[1]), &mem_objs[1]);

	size_t t = 4;
	size_t global_size[] = { t, t };
	size_t local_size[]  = { t, t };
	clEnqueueNDRangeKernel(cq, kernels[0], 2, NULL, global_size, local_size, 0, NULL, NULL);

	clEnqueueReadBuffer(cq, mem_objs[1], CL_TRUE, 0, sizeof(output), output, 0, NULL, NULL);

	for (int i = 0; i < 4; i++)
	{
		std::cout
			<< input[i][0] << ","
			<< input[i][1] << ","
			<< input[i][2] << ","
			<< input[i][3] << std::endl;
	}
	std::cout << std::endl;
	for (int i = 0; i < 4; i++)
	{
			std::cout
				<< output[i][0] << ","
				<< output[i][1] << ","
				<< output[i][2] << ","
				<< output[i][3] << std::endl;
	}

#if 0
	clSetKernelArg(kernels[0], 0, sizeof(mem_objs[0]), &mem_objs[0]);
	clSetKernelArg(kernels[0], 1, sizeof(mem_objs[1]), &mem_objs[1]);

	for (int i = 0; i < 100; i++)
	{
		size_t work_size = 1;
		clEnqueueWriteBuffer(cq, mem_objs[0], CL_TRUE, 0, sizeof(int), &i, 0, NULL, NULL);

#if 0
		clEnqueueTask(cq, kernels[0], 0, NULL, NULL);
#else
		clEnqueueNDRangeKernel(cq, kernels[0], 1, &work_size, &work_size, &work_size, 0, NULL, NULL);
#endif

		int *b = (int*)clEnqueueMapBuffer(cq, mem_objs[1], CL_TRUE, CL_MAP_READ, 0, sizeof(int), 0, NULL, NULL, &ret);
		std::cout << "Value of b = " << *b << std::endl;
		clEnqueueUnmapMemObject(cq, mem_objs[1], b, 0, NULL, NULL);
	}
#endif
#endif

	/* Finalization */
	cleanup(cq, kernels, mem_objs, program, context);
	return 0;
}
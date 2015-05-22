#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include "config.h"
#include "support.h"

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
	program = create_program_from_file(context, KERNEL_PATH "/pass_through.cl");
	/* Build Kernel Program */
	build_program(program, 1, &device_id, NULL, NULL, NULL);
	/* Create OpenCL Kernel */
	kernels.push_back(clCreateKernel(program, "pass_through", &ret));

#if 0
	int a, b;
	a = 10;
	clSetKernelArg(kernels[0], 0, sizeof(int), (void*)&a);
	clSetKernelArg(kernels[0], 0, sizeof(int), (void*)&b);
#else
	int a = 154;
	mem_objs.push_back(clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(a), &a, &ret));
	mem_objs.push_back(clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(a), NULL, &ret));

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

	/* Finalization */
	cleanup(cq, kernels, mem_objs, program, context);
	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <iostream>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)

char *read_file(const char *filename)
{
	/* Load the source code containing the kernel*/
	FILE *fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	char *source_str = (char*)malloc(MAX_SOURCE_SIZE + 1);
	size_t size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	source_str[size] = 0;
	fclose(fp);
	return source_str;
}

void setup(cl_context *context, cl_platform_id *platform_id, cl_device_id *device_id)
{
	cl_int ret;
	cl_uint temp;
	ret = clGetPlatformIDs(1, platform_id, &temp);
	ret = clGetDeviceIDs(*platform_id, CL_DEVICE_TYPE_DEFAULT, 1, device_id, &temp);
	*context = clCreateContext(NULL, 1, device_id, NULL, NULL, &ret);
}

cl_program create_program_from_file(cl_context context, const char *filename)
{
	char *source = read_file(filename);
	cl_int ret;
	cl_program prog = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &ret);
	free(source);
	return prog;
}

void cleanup()
{

}

int main()
{
	cl_device_id     device_id = NULL;
	cl_context       context = NULL;
	cl_command_queue command_queue = NULL;
	cl_program       program = NULL;
	cl_kernel        kernel = NULL;
	cl_platform_id   platform_id = NULL;
	cl_int           ret = 0;

	cl_mem memobj[3];
	float A[4][4];
	float B[4][4];
	float C[4][4];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			A[i][j] = i * 4 + j + 1;
			B[i][j] = j * 4 + i + 1;
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << i << "," << j << " : " << A[i][j] << "  " << B[i][j] << std::endl;
		}
	}

	setup(&context, &platform_id,  &device_id);

	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* Create Memory Buffer */
	for (int i = 0; i < 3; i++)
	{
		memobj[i] = clCreateBuffer(context, CL_MEM_READ_WRITE, 16 * sizeof(float), NULL, &ret);
	}

	//Copy the data to the device
	clEnqueueWriteBuffer(command_queue, memobj[0], CL_TRUE, 0, 16 * sizeof(float), A, 0, NULL, NULL);
	clEnqueueWriteBuffer(command_queue, memobj[1], CL_TRUE, 0, 16 * sizeof(float), B, 0, NULL, NULL);

	/* Create Kernel Program from the source */
	program = create_program_from_file(context, KERNEL_PATH "/data_parallel.cl");

	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, "data_parallel", &ret);

	for (int i = 0; i < 3; i++)
	{
		ret = clSetKernelArg(kernel, i, sizeof(cl_mem), (void*)(memobj + i));
	}

	size_t global_item_size = 4;
	size_t local_item_size  = 1;

	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(command_queue, memobj[2], CL_TRUE, 0, 16 * sizeof(float), C, 0, NULL, NULL);

	/* Display Results */
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%7.2f ", C[i][j]);
		}
		printf("\n");
	}


	/* Finalization */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	for (int i = 0; i < 3; i++)
	{
		ret = clReleaseMemObject(memobj[i]);
	}
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	return 0;
}
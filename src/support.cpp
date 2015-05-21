#include "support.h"
#include <stdio.h>
#include <stdlib.h>

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

void setup(cl_context *context, cl_platform_id *platform_id, cl_device_id *device_id, cl_command_queue *command_queue)
{
	cl_int ret;
	cl_uint temp;
	ret = clGetPlatformIDs(1, platform_id, &temp);
	ret = clGetDeviceIDs(*platform_id, CL_DEVICE_TYPE_DEFAULT, 1, device_id, &temp);
	*context = clCreateContext(NULL, 1, device_id, NULL, NULL, &ret);
	*command_queue = clCreateCommandQueue(*context, *device_id, NULL, &ret);
}

cl_program create_program_from_file(cl_context context, const char *filename)
{
	char *source = read_file(filename);
	cl_int ret;
	cl_program prog = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, &ret);
	free(source);
	return prog;
}

void cleanup(cl_command_queue       &command_queue,
	std::vector<cl_kernel> &kernels,
	std::vector<cl_mem>    &mem_objs,
	cl_program             &program,
	cl_context             &context)
{
	clFlush(command_queue);
	clFinish(command_queue);
	for (auto kernel : kernels)
		clReleaseKernel(kernel);
	clReleaseProgram(program);
	for (auto mem : mem_objs)
		clReleaseMemObject(mem);
	clReleaseCommandQueue(command_queue);
	clReleaseContext(context);
}

char *get_build_log(cl_program program, cl_device_id device)
{
	size_t log_size;
	char *program_log;
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	program_log = (char*)calloc(log_size + 1, sizeof(char));
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
	return program_log;
}
void build_program(
	cl_program          program,
	cl_uint             num_devices,
	const cl_device_id *devices,
	const char         *options,
	void (CL_CALLBACK  *notify_func)(cl_program, void *),
	void               *user_data)
{
	cl_int ret = clBuildProgram(program, num_devices, devices, options, notify_func, user_data);
	if (ret)
	{
		fprintf(stderr, "Building Program Failed\n");
		for (auto i = 0u; i < num_devices; i++)
		{
			char *log = get_build_log(program, devices[i]);
			fprintf(stderr, "Device %d\n%s", i, log);
			free(log);
		}
		exit(-1);
	}
}
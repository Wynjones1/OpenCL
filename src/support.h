#ifndef SUPPORT_H
#define SUPPORT_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <vector>

char      *read_file(const char *filename);
void       setup(cl_context *context, cl_platform_id *platform_id, cl_device_id *device_id, cl_command_queue *command_queue);
cl_program create_program_from_file(cl_context context, const char *filename);

void cleanup(cl_command_queue       &command_queue,
	std::vector<cl_kernel> &kernels,
	std::vector<cl_mem>    &mem_objs,
	cl_program             &program,
	cl_context             &context);

char *get_build_log(cl_program program, cl_device_id device);
void build_program(
	cl_program          program,
	cl_uint             num_devices,
	const cl_device_id *devices,
	const char         *options,
	void (CL_CALLBACK  *notify_func)(cl_program, void *),
	void               *user_data);

#endif
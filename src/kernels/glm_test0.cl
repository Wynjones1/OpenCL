__kernel void glm_input(__global float *input, __global float *output)
{
	uint x = get_local_id(0);
	uint y = get_local_id(1);

	output[y * 4 + x] = input[y * 4 + 3 - x];
}
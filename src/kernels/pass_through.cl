__kernel void pass_through(__global int *a, __global int *b)
{
	*b = *a;
}
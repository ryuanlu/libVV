#include <math.h>

void mat4_load_idendity(float* matrix)
{
	int i;
	for(i = 0;i < 16;++i)
		matrix[i] = 0.0f;
	
	matrix[0] = 1.0f;
	matrix[5] = 1.0f;
	matrix[10] = 1.0f;
	matrix[15] = 1.0f;
}


void mat4_set_frustum(float* matrix, float left, float right, float bottom, float top, float znear, float zfar)
{
	float temp, temp2, temp3, temp4;
	temp = 2.0 * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	matrix[0] = temp / temp2;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;
	matrix[4] = 0.0;
	matrix[5] = temp / temp3;
	matrix[6] = 0.0;
	matrix[7] = 0.0;
	matrix[8] = (right + left) / temp2;
	matrix[9] = (top + bottom) / temp3;
	matrix[10] = (-zfar - znear) / temp4;
	matrix[11] = -1.0;
	matrix[12] = 0.0;
	matrix[13] = 0.0;
	matrix[14] = (-temp * zfar) / temp4;
	matrix[15] = 0.0;
}


void mat4_set_perspective(float* matrix, float fovy, float aspect, float near, float far)
{
	float ymax, xmax;
	ymax = near * tanf(fovy * 3.14159265358979323846 / 360.0);
	xmax = ymax * aspect;
	mat4_set_frustum(matrix, -xmax, xmax, -ymax, ymax, near, far);
}


void mat4_mul(float* out, float* A, float* B)
{
	int row, col, i;
	float C[16];

	for(row = 0;row < 4;++row)
	{
		for(col = 0;col < 4;++col)
		{
			C[col * 4 + row] = 0.0;
			for(i = 0;i < 4;++i)
				C[col * 4 + row] += A[i * 4 + row] * B[col * 4 + i];
		}
	}

	out = out ? out : A;

	for(i = 0;i < 16;++i)
		out[i] = C[i];
}


void mat4_mul_vec4(float* out, float* A, float* B)
{
	int row, i;
	float C[4];

	for(row = 0;row < 4;++row)
	{
		C[row] = 0.0;
		for(i = 0;i < 4;++i)
			C[row] += A[i * 4 + row] * B[i];
	}

	out = out ? out : B;

	for(i = 0;i < 4;++i)
		out[i] = C[i];
}


void vec4_set(float* vector, float x, float y, float z, float w)
{
	vector[0] = x;
	vector[1] = y;
	vector[2] = z;
	vector[3] = w;
}

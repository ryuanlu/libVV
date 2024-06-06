#include <math.h>
#include "matrix.h"

#ifndef M_PI
#define M_PI	(3.1415926535)
#endif

void mat4_set_identity(float* mat4)
{
	mat4_set_scaling(mat4, 1.0f, 1.0f, 1.0f);
}

void mat4_set_zero(float* mat4)
{
	for(int i = 0;i < 16;++i)
		mat4[i] = 0.0f;
}

void mat4_set_scaling(float* mat4, const float x, const float y, const float z)
{
	mat4_set_zero(mat4);

	mat4[0] = x;
	mat4[5] = y;
	mat4[10] = z;
	mat4[15] = 1.0f;
}

void mat4_set_translate(float* mat4, const float x, const float y, const float z)
{
	mat4_set_zero(mat4);

	mat4[0] = 1.0f;
	mat4[5] = 1.0f;
	mat4[10] = 1.0f;
	mat4[12] = x;
	mat4[13] = y;
	mat4[14] = z;
	mat4[15] = 1.0f;
}

void mat4_set_rotation(float* mat4, const float x, const float y, const float z, const float arc)
{
	float vec[3] = {x, y, z};
	float cos = cosf(arc);
	float sin = sinf(arc);
	float one_minus_cos = (1.0f - cos);

	vec_normalize(vec, 3);
	mat4_set_zero(mat4);

	float xx = vec[0] * vec[0];
	float yy = vec[1] * vec[1];
	float zz = vec[2] * vec[2];

	float xy = vec[0] * vec[1];
	float xz = vec[0] * vec[2];
	float yz = vec[1] * vec[2];


	mat4[0] = cos + xx * one_minus_cos;
	mat4[1] = xy * one_minus_cos + vec[2] * sin;
	mat4[2] = xz * one_minus_cos - vec[1] * sin;

	mat4[4] = xy * one_minus_cos - vec[2] * sin;
	mat4[5] = cos + yy * one_minus_cos;
	mat4[6] = yz * one_minus_cos + vec[0] * sin;

	mat4[8] = xz * one_minus_cos + vec[1] * sin;
	mat4[9] = yz * one_minus_cos - vec[0] * sin;
	mat4[10] = cos + zz * one_minus_cos;

	mat4[15] = 1.0f;
}

void mat4_set_frustum(float* mat4, const float left, const float right, const float bottom, const float top, const float znear, const float zfar)
{
	float temp, temp2, temp3, temp4;

	temp = 2.0 * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;
	mat4[0] = temp / temp2;
	mat4[1] = 0.0;
	mat4[2] = 0.0;
	mat4[3] = 0.0;
	mat4[4] = 0.0;
	mat4[5] = temp / temp3;
	mat4[6] = 0.0;
	mat4[7] = 0.0;
	mat4[8] = (right + left) / temp2;
	mat4[9] = (top + bottom) / temp3;
	mat4[10] = (-zfar - znear) / temp4;
	mat4[11] = -1.0;
	mat4[12] = 0.0;
	mat4[13] = 0.0;
	mat4[14] = (-temp * zfar) / temp4;
	mat4[15] = 0.0;
}

void mat4_set_perspective(float* mat4, const float fovyInDegrees, const float aspectRatio, const float znear, const float zfar)
{
	float ymax, xmax;
	ymax = znear * tanf(fovyInDegrees * M_PI / 360.0);
	xmax = ymax * aspectRatio;
	mat4_set_frustum(mat4, -xmax, xmax, -ymax, ymax, znear, zfar);
}

void mat4_multiplied_by_mat4(float* mat4_out, const float* mat4_A, const float* mat4_B)
{
	float tmp[16];
	int i, j;

	for(i = 0;i < 16;++i)
	{
		tmp[i] = 0.0f;

		for(j = 0;j < 4; ++j)
			tmp[i] += mat4_A[j * 4 + (i % 4)] * mat4_B[(i / 4) * 4 + j];
	}

	for(i = 0;i < 16;++i)
		mat4_out[i] = tmp[i];
}

void mat4_multiplied_by_vec4(float* vec4_out, const float* mat4, const float* vec4)
{
	float tmp[4];
	int i, j;

	for(i = 0;i < 4;++i)
	{
		tmp[i] = 0.0f;
		for(j = 0;j < 4;++j)
			tmp[i] += mat4[j * 4 + i] * vec4[j];
	}

	for(i = 0;i < 4;++i)
		vec4_out[i] = tmp[i];

}

float vec_length(const float* vec, const int n)
{
	float length = 0.0f;

	for(int i = 0;i < n;++i)
		length += vec[i] * vec[i];

	length = sqrt(length);

	return length;
}

void vec_normalize(float* vec, const int n)
{
	float length;

	length = vec_length(vec, n);

	for(int i = 0;i < n;++i)
		vec[i] /= length;
}

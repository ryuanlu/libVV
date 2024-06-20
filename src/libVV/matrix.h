#ifndef __MATRIX_H__
#define __MATRIX_H__

#ifndef M_PI
#define M_PI	(3.1415926535)
#endif

void mat4_set_identity(float* mat4);
void mat4_set_zero(float* mat4);

void mat4_set_scaling(float* mat4, const float x, const float y, const float z);
void mat4_set_translate(float* mat4, const float x, const float y, const float z);
void mat4_set_rotation(float* mat4, const float x, const float y, const float z, const float arc);

void mat4_set_frustum(float* mat4, const float left, const float right, const float bottom, const float top, const float znear, const float zfar);
void mat4_set_perspective(float* mat4, const float fovyInDegrees, const float aspectRatio, const float znear, const float zfar);

void mat4_multiplied_by_mat4(float* mat4_out, const float* mat4_A, const float* mat4_B);
void mat4_multiplied_by_vec4(float* vec4_out, const float* mat4, const float* vec4);

float vec_length(const float* vec, const int n);
void vec_normalize(float* vec, const int n);

#endif /* __MATRIX_H__ */

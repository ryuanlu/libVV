#ifndef __MATRIX_H__
#define __MATRIX_H__


void	mat4_load_idendity	(float* matrix);
void	mat4_set_frustum	(float* matrix, float left, float right, float bottom, float top, float znear, float zfar);
void	mat4_set_perspective	(float *matrix, float fovy, float aspect, float near, float far);
void	mat4_mul		(float *A, float *B);

#endif /* __MATRIX_H__ */
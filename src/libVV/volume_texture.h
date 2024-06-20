#ifndef __VOLUME_TEXTURE_H__
#define __VOLUME_TEXTURE_H__

#include <GLES3/gl32.h>

struct volume;

struct volume_texture
{
	GLuint	volume_texture;
	GLuint	gradient;

	struct volume*	volume;
};


struct volume_texture* volume_texture_create(struct volume* volume);
int volume_texture_destroy(struct volume_texture* volume_texture);


#endif /* __VOLUME_TEXTURE_H__ */

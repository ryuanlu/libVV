#include <stdlib.h>
#include "volume.h"
#include "volume_texture.h"


struct volume_texture* volume_texture_create(struct volume* volume)
{
	struct volume_texture* volume_texture = NULL;

	volume_texture = calloc(1, sizeof(struct volume_texture));
	volume_texture->volume = volume;

	glGenTextures(1, &volume_texture->volume_texture);
	glGenTextures(1, &volume_texture->gradient);

	glBindTexture(GL_TEXTURE_3D, volume_texture->volume_texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	switch(volume->params.voxelformat)
	{
		case VOXEL_FORMAT_UNSIGNED_8:
		glTexImage3D(GL_TEXTURE_3D, 0, GL_R8UI, volume->params.width, volume->params.height, volume->params.depth, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, volume->data.u8);
		break;
		case VOXEL_FORMAT_UNSIGNED_16_LE:
		glTexImage3D(GL_TEXTURE_3D, 0, GL_R16UI, volume->params.width, volume->params.height, volume->params.depth, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, volume->data.u16);
		break;
		default:
		break;
	}

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_3D, volume_texture->gradient);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, volume->params.width, volume->params.height, volume->params.depth, 0, GL_RGBA, GL_FLOAT, volume->gradient);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return volume_texture;
}

int volume_texture_destroy(struct volume_texture* volume_texture)
{
	if(!volume_texture)
		return 1;

	glDeleteTextures(1, &volume_texture->volume_texture);
	glDeleteTextures(1, &volume_texture->gradient);

	free(volume_texture);

	return 0;
}


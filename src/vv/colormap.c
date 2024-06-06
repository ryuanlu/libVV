#include <math.h>
#include "colormap.h"


static void hsv_to_rgb(float* rgb, const float hue, const float saturation, const float value)
{
	int i;
	float f, p, q, t;
	float r = 0.0f, g = 0.0f, b = 0.0f;

	i = floor(hue * 6);
	f = hue * 6 - i;
	p = value* (1 - saturation);
	q = value* (1 - f * saturation);
	t = value* (1 - (1 - f) * saturation);

	switch(i % 6)
	{
	case 0:
		r = value;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = value;
		b = p;
		break;
	case 2:
		r = p;
		g = value;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = value;
		break;
	case 4:
		r = t;
		g = p;
		b = value;
		break;
	case 5:
		r = value;
		g = p;
		b = q;
		break;
	}

	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}


void colormap_gen_hsv(unsigned char* colormap, const int width, const int height)
{
	float rgb[3];

	for(int x = 0;x < width;++x)
	{
		hsv_to_rgb(rgb, (float) x / width, 1.0f, 1.0f);

		for(int y = 0;y < height;++y)
		{
			colormap[4 * width* y + 4 * x + 0] = rgb[0] * 255;
			colormap[4 * width* y + 4 * x + 1] = rgb[1] * 255;
			colormap[4 * width* y + 4 * x + 2] = rgb[2] * 255;
			colormap[4 * width* y + 4 * x + 3] = 255;
		}
	}
}


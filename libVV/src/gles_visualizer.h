#ifndef __GLES_VISUALIZER_H__
#define __GLES_VISUALIZER_H__



enum vv_result gles_visualizer_create(struct vv_visualizer* visualizer);
enum vv_result gles_visualizer_destroy(struct vv_visualizer* visualizer);

enum vv_result gles_visualizer_set_viewport(struct vv_visualizer* visualizer, const int width, const int height);
enum vv_result gles_visualizer_set_volume(struct vv_visualizer* visualizer, struct vv_memory* volume);
enum vv_result gles_visualizer_set_colormap(struct vv_visualizer* visualizer, struct vv_memory* colormap);

enum vv_result gles_visualizer_render(struct vv_visualizer* visualizer);
enum vv_result gles_visualizer_get_pixels(struct vv_visualizer* visualizer, char* pixels);


#endif /* __GLES_VISUALIZER_H__ */
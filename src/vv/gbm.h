#ifndef __GBM_H__
#define __GBM_H__


struct gbm_context;
struct gbm_fb;

struct gbm_context* gbm_context_create(const char* device);
void gbm_context_destroy(struct gbm_context* context);
struct gbm_fb* gbm_fb_create(struct gbm_context* context, const int width, const int height);
void gbm_fb_destroy(struct gbm_context* context, struct gbm_fb* fb);
char* gbm_fb_read_pixels(struct gbm_fb* fb);
char* gbm_fb_map(struct gbm_fb* fb);
void gbm_fb_unmap(struct gbm_fb* fb);
int gbm_make_current(struct gbm_context* context, struct gbm_fb* fb);


#endif /* __GBM_H__ */
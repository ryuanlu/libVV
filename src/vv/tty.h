#ifndef __TTY_H__
#define __TTY_H__


struct tty_device;
struct widget;

struct tty_device* tty_device_open(const char* device);
void tty_device_close(struct tty_device* tty_device);
void tty_device_flush(struct tty_device* device);
void tty_device_write(struct tty_device* device, const char* data);
void tty_device_write_n(struct tty_device* device, const char* data, const int repeat);
void tty_device_set_cursor_position(struct tty_device* device, const int x, const int y);
void tty_device_set_fg_color(struct tty_device* device, const int r, const int g, const int b);
void tty_device_set_bg_color(struct tty_device* device, const int r, const int g, const int b);
void tty_device_set_title(struct tty_device* device, const char* title);
void tty_device_set_root_widget(struct tty_device* device, struct widget* widget);
void tty_device_redraw(struct tty_device* device);
int tty_device_dispatch(struct tty_device* device);


#endif /* __TTY_H__ */

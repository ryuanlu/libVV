#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <termios.h>
#include "tty.h"
#include "panel.h"


#define DEFAULT_TTY_DEVICE	"/dev/tty"
#define TTY_BUFFER_SIZE		(4096)

struct tty_device
{
	int	fd;
	struct termios	original;

	char*	buffer;
	int	buffer_size;
	int	data_length;
	struct widget* root;
};

struct tty_device* tty_device_open(const char* device)
{
	const char default_device[] = DEFAULT_TTY_DEVICE;
	struct tty_device* tty = NULL;
	struct termios termios;


	tty = calloc(1, sizeof(struct tty_device));

	tty->fd = open(device ? device : default_device, O_RDWR|O_NONBLOCK);
	fcntl(tty->fd, F_SETFD, FD_CLOEXEC);

	tcgetattr(tty->fd, &tty->original);

	termios = tty->original;
	termios.c_iflag &= ~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);
	termios.c_oflag &= ~(OPOST);
	termios.c_cflag |= (CS8);
	termios.c_lflag &= ~(ECHO|ICANON|IEXTEN|ISIG);
	termios.c_cc[VMIN] = 0;
	termios.c_cc[VTIME] = 1;

	tcsetattr(tty->fd, TCSAFLUSH, &termios);

	tty->buffer = calloc(1, TTY_BUFFER_SIZE);
	tty->buffer_size = TTY_BUFFER_SIZE;

	tty_device_set_fg_color(tty, 255, 255, 255);
	tty_device_set_bg_color(tty, 53, 53, 53);
	tty_device_write(tty, "\x1b[?25l\x1b[?1049h\x1b[?1003h\x1b[?1006h");

	return tty;
}


void tty_device_close(struct tty_device* tty_device)
{
	tty_device_write(tty_device, "\x1b[?25h\x1b[?1049l\x1b[?1003l\x1b[?1006l");
	tty_device_flush(tty_device);
	tcsetattr(tty_device->fd, TCSAFLUSH, &tty_device->original);
	close(tty_device->fd);
	free(tty_device->buffer);
	free(tty_device);
}

void tty_device_flush(struct tty_device* device)
{
	if(!device->data_length)
		return;

	struct pollfd pollfd;
	pollfd.fd = device->fd;
	pollfd.events = POLLOUT;
	pollfd.revents = 0;

	poll(&pollfd, 1, -1);
	write(device->fd, device->buffer, device->data_length);
	device->data_length = 0;
}

void tty_device_write(struct tty_device* device, const char* data)
{
	int length = strlen(data);

	if(device->data_length + length >= device->buffer_size)
		tty_device_flush(device);

	memcpy(&device->buffer[device->data_length], data, length);
	device->data_length += length;
}

void tty_device_write_n(struct tty_device* device, const char* data, const int repeat)
{
	for(int i = 0;i < repeat;++i)
		tty_device_write(device, data);
}

void tty_device_set_cursor_position(struct tty_device* device, const int x, const int y)
{
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "\x1b[%d;%dH", y, x);
	tty_device_write(device, buffer);
}


void tty_device_set_fg_color(struct tty_device* device, const int r, const int g, const int b)
{
	char buffer[32];
	snprintf(buffer, sizeof(buffer), "\x1b[38;2;%d;%d;%dm", r, g, b);
	tty_device_write(device, buffer);
}

void tty_device_set_bg_color(struct tty_device* device, const int r, const int g, const int b)
{
	char buffer[32];
	snprintf(buffer, sizeof(buffer), "\x1b[48;2;%d;%d;%dm", r, g, b);
	tty_device_write(device, buffer);
}


void tty_device_set_title(struct tty_device* device, const char* title)
{
	tty_device_write(device, "\x1b]0;");
	tty_device_write(device, title);
	tty_device_write(device, "\a");
}


void tty_device_set_root_widget(struct tty_device* device, struct widget* widget)
{
	device->root = widget;
}

void tty_device_redraw(struct tty_device* device)
{
	device->root->redraw(device->root, device);
}


static int handle_mouse(struct tty_device* device, const char* buffer)
{
	int btn = 0, x = 0, y = 0;
	char m = 0;
	int button = 0, pressed = 0, modifier = 0;

	if(buffer[0] != 0x1b || buffer[1] != '[' || buffer[2] != '<')
		return 1;

	sscanf(buffer, "\x1b[<%d;%d;%d%c", &btn, &x, &y, &m);

	if((btn & 0x03) == 0x00) button = MOUSE_BUTTON_LEFT;
	if((btn & 0x03) == 0x02) button = MOUSE_BUTTON_RIGHT;
	if((btn & 0x03) == 0x01) button = MOUSE_BUTTON_MIDDLE;
	if((btn & 0x03) == 0x03) button = MOUSE_BUTTON_NONE;

	pressed = (button != MOUSE_BUTTON_NONE) && m == 'M';

	modifier |= ((btn & (1 << 2)) ? MODIFIER_SHIFT : MODIFIER_NONE);
	modifier |= ((btn & (1 << 3)) ? MODIFIER_ALT : MODIFIER_NONE);
	modifier |= ((btn & (1 << 4)) ? MODIFIER_CTRL : MODIFIER_NONE);

	if(btn & (1 << 5))
	{
		device->root->mouse_motion(device->root, device, x, y, button, pressed, modifier);
	}else if(btn & (1 << 6))
	{
		device->root->mouse_wheel(device->root, device, x, y, (btn & 1) ? -1 : 1, modifier);
	}else
	{
		device->root->mouse_button(device->root, device, x, y, button, pressed, modifier);
	}

	return 0;
}

static int handle_keyboard(struct tty_device* device, const char* buffer)
{
	if(buffer[0] == 27)
		return 1;


	int key, modifier = MODIFIER_NONE;

	if(buffer[0] < 27)
	{
		key = buffer[0] + 96;
		modifier = MODIFIER_CTRL;
	}else
	{
		key = buffer[0];
	}

	device->root->keyboard(device->root, device, key, modifier);

	return 0;
}

int tty_device_dispatch(struct tty_device* device)
{
	int ret;
	char buf[32];
	struct pollfd pollfd;

	pollfd.fd = device->fd;
	pollfd.events = POLLIN;
	pollfd.revents = 0;

	ret = poll(&pollfd, 1, 0);

	if(ret <= 0)
		return 1;

	ret = read(device->fd, buf, sizeof(buf) - 1);

	if(ret > 0)
		buf[ret] = 0;


	ret = handle_mouse(device, buf);
	ret = handle_keyboard(device, buf);

	return 0;
}
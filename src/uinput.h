#ifndef UINPUT_H
#define UINPUT_H
#include <libevdev/libevdev-uinput.h>

struct libevdev_uinput *uinput_initialize();
int uinput_send_touch(struct libevdev_uinput *uinput,int phase, int slot, int trackid, int x, int y); 
#endif

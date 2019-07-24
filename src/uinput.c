#include "uinput.h"
#include <stddef.h>

struct libevdev_uinput *uinput_initialize() {
	// Create a evdev first to describe the features
	struct libevdev *evdev = libevdev_new();
	libevdev_set_name(evdev, "Simulated Touch Surface");
	libevdev_enable_event_type(evdev, EV_ABS);
	libevdev_enable_event_type(evdev, EV_KEY);
	libevdev_enable_event_code(evdev, EV_KEY, BTN_TOUCH, NULL);
	libevdev_enable_event_code(evdev, EV_KEY, BTN_TOOL_FINGER, NULL);
	struct input_absinfo absxinfo = {
		.value = 0,	
		.minimum = 0,
		.maximum = 1024,
		.resolution = 72
	};
	libevdev_enable_event_code(evdev, EV_ABS, ABS_X, &absxinfo);
	libevdev_enable_event_code(evdev, EV_ABS, ABS_Y, &absxinfo);
	struct input_absinfo absmtslotinfo = {
		.value = 0,	
		.minimum = 0,
		.maximum = 10,
	};
	libevdev_enable_event_code(evdev, EV_ABS, ABS_MT_SLOT, &absmtslotinfo);
	libevdev_enable_event_code(evdev, EV_ABS, ABS_MT_POSITION_X, &absxinfo);
	libevdev_enable_event_code(evdev, EV_ABS, ABS_MT_POSITION_Y, &absxinfo);
	struct input_absinfo absmttrackinginfo = {
		.value = 0,	
		.minimum = 0,
		.maximum = 65535,
	};
	libevdev_enable_event_code(evdev, EV_ABS, ABS_MT_TRACKING_ID, &absmttrackinginfo);
	libevdev_enable_event_code(evdev, EV_ABS, ABS_MT_TRACKING_ID, &absmttrackinginfo);
	// Initialize uinput device from the evdev descriptor
	struct libevdev_uinput *uinput = NULL;
	if (libevdev_uinput_create_from_device(evdev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uinput) != 0) {
		uinput = NULL;
	}
	// We don't need the fake evdev anymore.
	libevdev_free(evdev);
	return uinput;
}

int uinput_send_touch(struct libevdev_uinput *uinput, int phase, int slot, int trackid, int x, int y) {
	int res = 0;

	res = libevdev_uinput_write_event(uinput, EV_ABS, ABS_MT_SLOT, slot);
	if (res != 0) goto end;
	res = libevdev_uinput_write_event(uinput, EV_ABS, ABS_MT_POSITION_X, x);
	if (res != 0) goto end;
	res = libevdev_uinput_write_event(uinput, EV_ABS, ABS_MT_POSITION_Y, y);
	if (res != 0) goto end;
	res = libevdev_uinput_write_event(uinput, EV_ABS, ABS_MT_TRACKING_ID, trackid);
	if (res != 0) goto end;

	if (phase == 0) { // Touch down
		res = libevdev_uinput_write_event(uinput, EV_ABS, ABS_MT_TRACKING_ID, trackid);
		if (res != 0) goto end;
		res = libevdev_uinput_write_event(uinput, EV_KEY, BTN_TOUCH, 1);
		if (res != 0) goto end;
		res = libevdev_uinput_write_event(uinput, EV_KEY, BTN_TOOL_FINGER, 1);
		if (res != 0) goto end;
	} else if (phase == 2) { // Touch up
		res = libevdev_uinput_write_event(uinput, EV_ABS, ABS_MT_TRACKING_ID, -1);
		if (res != 0) goto end;
		res = libevdev_uinput_write_event(uinput, EV_KEY, BTN_TOUCH, 0);
		if (res != 0) goto end;
		res = libevdev_uinput_write_event(uinput, EV_KEY, BTN_TOOL_FINGER, 0);
		if (res != 0) goto end;
	}

	if (slot == 0) {
		res = libevdev_uinput_write_event(uinput, EV_ABS, ABS_X, x);
		if (res != 0) goto end;
		res = libevdev_uinput_write_event(uinput, EV_ABS, ABS_Y, y);
		if (res != 0) goto end;
	}
	res = libevdev_uinput_write_event(uinput, EV_SYN, SYN_REPORT, 0);
	end:
	return res;
}

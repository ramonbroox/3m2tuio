#ifndef _TS3M_EVENT_SOURCE_H_
#define _TS3M_EVENT_SOURCE_H_

#include "TS3M.h"
#include <cstring>

#include "hidapi/hidapi.h"

#include "TUIO/TuioServer.h"
extern "C" {
#include "uinput.h"
}

#ifdef WIN32
#include <windows.h>
#endif

using namespace TUIO;
using namespace std;

class TS3MEventSource
{

    TuioServer          *_tuioServer;
    TuioCursor			*_touch_list[TOUCH_MAXID + 1];
    struct libevdev_uinput *_uinput;

public:
    TS3MEventSource(struct hid_device_info*	device, const string& name, bool verbose=false, unsigned width=TS3M_X_RESOL, unsigned height=TS3M_Y_RESOL);
    virtual	~TS3MEventSource();

    int		init(struct hid_device_info* device);
    int		destroy();
    int		start(bool detach=true);
    int		stop();

    bool	isOk();

private:
    string			_name;

    unsigned		_device_width, _device_height;
    bool            _is_running;                    ///< True if reader is running
    bool			_is_verbose;					///< True if verbose mode is on

    hid_device*     _handle;

    bool			_is_big_endian;					///< True if architecture is big endian

#ifndef WIN32
    pthread_t _thread;
#else
    HANDLE _thread;
#endif
    //pthread_t		_thread;						///< Reading thread
#ifndef WIN32
    static void *_read_thread(void* arg);
#else
    static DWORD WINAPI _read_thread( LPVOID obj );
#endif
    //static void*	_read_thread(void*);			///< Reading thread function

    void			_read_loop();					///< Actual device reading loop
};

#endif

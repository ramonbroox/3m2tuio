#include "TS3MEventSource.h"
#include <time.h>
#include "hidapi/hidapi.h"
#include "Utilities.h"

TS3MEventSource::TS3MEventSource(struct hid_device_info* device,
                                 const string& name,
                                 bool verbose,
                                 unsigned width,
                                 unsigned height) : _name(name), _tuioServer(NULL), _uinput(NULL),
    _device_width(width), _device_height(height),
    _is_running(false), _is_verbose(verbose),
    _handle(NULL), _is_big_endian(isBigEndian())
{
    this->init(device);

    //init touch list tab
    for (int i = 0; i < 256; i++) {
        _touch_list[i] = NULL;
    }

    _is_big_endian = isBigEndian();
}

TS3MEventSource::~TS3MEventSource()
{
    this->destroy();
}

bool TS3MEventSource::isOk()
{
    return (_handle != NULL);
}

int TS3MEventSource::start(bool detach)
{
    if (!this->isOk() || _is_running)
        return -1;

    _is_running = true;
    cerr << "about to create the thread, detach value is : " << detach << endl;
    if (detach) {
#ifndef WIN32
        pthread_create(&_thread, NULL, TS3MEventSource::_read_thread, (void*)this);
#else
        DWORD threadId;
        _thread = CreateThread( 0, 0, TS3MEventSource::_read_thread, this, 0, &threadId);
#endif
    } else {
        this->_read_loop();
    }
    return 0;
}

int TS3MEventSource::stop(void)
{
    if (!this->isOk() || !_is_running)
        return -1;

    _is_running = false;
#ifndef WIN32
    pthread_cancel(_thread);
#else
    TerminateThread(_thread, 0);
#endif

    return 0;
}

int TS3MEventSource::init(struct hid_device_info * device)
{

    if (!(_handle = hid_open_path(device->path))) {
        cerr << "Error opening 3M device" << endl;
        exit(1);
    }

    if (_is_verbose) {
        cerr << "3M Touch Screen found" << endl;
    }

    //_uinput = uinput_initialize();

    // Start TUIO server
    _tuioServer = new TuioServer("0.0.0.0", 4444);
    //_tuioServer->setSourceName(_name.c_str());
   // _tuioServer->enableCursorProfile(true);
    TuioTime::initSession();

    return 0;
}

int TS3MEventSource::destroy(void)
{
    delete _tuioServer;
    _tuioServer = NULL;

    stop();

    if (_handle) {
        hid_close(_handle);
    }

    return 0;
}

#ifndef WIN32
void *TS3MEventSource::_read_thread(void* arg)
{
#else
DWORD WINAPI TS3MEventSource::_read_thread( LPVOID arg )
{
#endif
//void *TS3MEventSource::_read_thread(void* arg) {
    TS3MEventSource	*source = (TS3MEventSource*)arg;

    cerr << "thread started" << endl;

    if (source != NULL)
        source->_read_loop();
    return NULL;
}

void TS3MEventSource::_read_loop()
{

    int 		length = 64;
    unsigned char * data = new unsigned char[length];
    struct timeval	tv, tv_init;
    int dataTransfered = 0;

    int activeTouch = 0;
    int transferredTouch = 0;

    gettimeofday(&tv_init, NULL);
    if (_is_verbose)
	    cerr << "loop will begin is runnin value : " << _is_running << endl;


    while (_is_running) {

        memset(data, 0x00, length);
	cout << "Reading..." << endl;
        int dataTransfered = hid_read_timeout(_handle, data, length, 10);
	cout << "Read " << dataTransfered << endl;

        if (dataTransfered < 0) {
            cerr << "error reading data : " << hid_error(_handle) << endl;
        }

        if (dataTransfered > 0) {
            gettimeofday(&tv, NULL);
            tv.tv_sec -= tv_init.tv_sec;
            tv.tv_usec -= tv_init.tv_usec;
            if (tv.tv_usec < 0) {
                tv.tv_usec += 1000000;
                tv.tv_sec--;
            }
            double date = (double)tv.tv_sec + ((double)tv.tv_usec)/1000000.;
            uint8_t report_type = data[0];

            if (report_type == TS3M_TOUCH_REPORT_13 || report_type == TS3M_TOUCH_REPORT_17) { //this is a touch info report

                struct _3M_coordinate_report_x13 * report = ( struct _3M_coordinate_report_x13 *)data;
                struct _3M_coordinate_report_x17 * report17 = ( struct _3M_coordinate_report_x17 *)data;
                int maxtouches = 6;
                struct _3M_touch_report *touches = report->touchs;
                if (report_type == TS3M_TOUCH_REPORT_17) {
                    maxtouches = 10;
                    touches = report17->touchs;
                }


                if (activeTouch == 0) {
		  
		    if (report_type == TS3M_TOUCH_REPORT_13) {
			    activeTouch = report->actual_count;
		    } else {
		    	activeTouch = report17->actual_count;
		    }
                    if (_is_verbose)
			    cerr << "Actual count: " << activeTouch << endl;
		}

                //check if we have a new frame
                if (transferredTouch == 0) {
                    _tuioServer->initFrame(TuioTime::getSessionTime());
                    if (_is_verbose)
                        cerr << "frame pending..." << endl;
                }

                for (int i = 0; i < maxtouches; i++) { //update for each touch tranfered infos
                    _3M_touch_report * currentTouch = &(touches[i]);
                    if (currentTouch->status != TS3M_INVALID_TOUCH) { //if it's a valid touch
                        transferredTouch++;
                        //compute coordonates
                        float touchX;
                        float touchY;

                        uint16_t xCoord;
                        uint16_t yCoord;

                        if (_is_big_endian) {
                            ((uint8_t*)&xCoord)[1] = currentTouch->x_lsb;
                            ((uint8_t*)&xCoord)[0] = currentTouch->x_msb;
                            ((uint8_t*)&yCoord)[1] = currentTouch->y_lsb;
                            ((uint8_t*)&yCoord)[0] = currentTouch->y_msb;
                        } else {
                            ((uint8_t*)&xCoord)[0] = currentTouch->x_lsb;
                            ((uint8_t*)&xCoord)[1] = currentTouch->x_msb;
                            ((uint8_t*)&yCoord)[0] = currentTouch->y_lsb;
                            ((uint8_t*)&yCoord)[1] = currentTouch->y_msb;
                        }

                        touchX = (double)xCoord / TS3M_X_RESOL;
                        touchY = (double)yCoord / TS3M_Y_RESOL;


                        if (_is_verbose)
                            cout << "#" << (int)currentTouch->touch_id << " "
                                 << (currentTouch->status != TS3M_NOT_TOUCHING? ((_touch_list[currentTouch->touch_id] != NULL)? "update" : "down"):"up")
                                 << " | X=" << touchX << " | Y=" << touchY << " | @" << date << " | Rx=" << xCoord << "| Ry=" << yCoord << endl;

                        if (!_touch_list[currentTouch->touch_id]) { //new touch
			    cout << "NTS TUIO" << endl;
                            _touch_list[currentTouch->touch_id] = _tuioServer->addTuioCursor(touchX, touchY);
			    cout << "NTS UINPUT" << endl;
			    //uinput_send_touch(_uinput, 0, currentTouch->touch_id, _touch_list[currentTouch->touch_id]->getCursorID(), xCoord, yCoord);
			    cout << "NTS DONE" << endl;
                        } else {
                            if (currentTouch->status == TS3M_NOT_TOUCHING) { //removed touch
				cout << "REM TUIO" << endl;
                                _tuioServer->removeTuioCursor(_touch_list[currentTouch->touch_id]);
				cout << "REM UINPUT" << endl;
			        //uinput_send_touch(_uinput, 2, currentTouch->touch_id, _touch_list[currentTouch->touch_id]->getCursorID(), xCoord, yCoord);
				cout << "REM DONE" << endl;
                                _touch_list[currentTouch->touch_id] = NULL;
                            } else { //update position
				cout << "UPD TUIO" << endl;
                                _tuioServer->updateTuioCursor(_touch_list[currentTouch->touch_id], touchX, touchY);
				cout << "UPD UINPUT" << endl;
			        //uinput_send_touch(_uinput, 1, currentTouch->touch_id, _touch_list[currentTouch->touch_id]->getCursorID(), xCoord, yCoord);
				cout << "UPD DONE" << endl;
                            }
                        }
                    }
                }

		if (_is_verbose) 
			cerr << "active touch" << activeTouch << "     transferred touch " << transferredTouch << endl;

                //check if we received all touch, if yes send the frame
                if(activeTouch == transferredTouch) {
                    transferredTouch = 0;
                    activeTouch = 0;

                    if (_is_verbose)
                        cerr << "commit frame" << endl;
                    _tuioServer->commitFrame();
                }
            } else {
		    if (_is_verbose)
			    cerr << "Received unhandled report " << report_type << endl;;
	    }
        } else {
		static timespec req;
		req.tv_sec = 0;
		req.tv_nsec = 16*1000000; // 16ms
		nanosleep(&req, NULL);
	}
    }
}




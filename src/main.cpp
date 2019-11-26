#include <iostream>
#include <sstream>
#include "hidapi/hidapi.h"

#include "TS3M.h"
#include "TS3MEventSource.h"
#include "stdio.h"


void print_devs(struct hid_device_info *devs);
unsigned int init_devs(bool verbose);

using namespace std;

int main (int argc, char * const argv[])
{
    std::cerr << "3M Touch Screen to TUIO server v0.1\n";
    std::cerr << "Type q to quit properly (for now...)" << endl;

    if (argc > 1 && strcmp(argv[1],"--help")==0) {
        // print help and exit
        cerr << "usage : " << argv[0] << " [-v]" << endl;
        cerr << "\t-v : verbose mode" << endl;
        cerr << "\tNote: use with root privilege on Linux and admin privilege on MacOS X" << endl;
        exit(0);
    }



    bool verbose = ((argc > 1 && argv[1][1] == 'v'));

    unsigned int TS3MSourceCount= init_devs(verbose);

    if (TS3MSourceCount > 0)
        while (getchar() != 'q');
    else {
        cerr << "No 3M device connected" << endl;
    }



    return 0;
}

/**
 *  look for all device, and launch the event source for supported ones
 *  \param verbose print debug infos
 *  \return number of active devices
 */
unsigned int init_devs(bool verbose)
{
    TS3MEventSource      *TS3MSource[8];
    unsigned int TS3MSourceCount=0;

    struct hid_device_info *devs, *cur_dev;

    //get device list
    devs = hid_enumerate(0x0, 0x0);

    if (!devs) {
        cerr << " Error : No HID device found" << endl;
        hid_free_enumeration(devs);
        return 0;
    }

    cerr << "HID devices found : " << endl;
    print_devs(devs);

    //launch supported devices
    cur_dev = devs;
    while (cur_dev) {
#ifdef __APPLE__
        if (cur_dev->usage == 1)
#endif
#ifdef WIN32
            if (cur_dev->usage == 4)
#endif
                if (cur_dev->vendor_id == TS3M_VENDOR_ID && cur_dev->product_id == TS3M_PRODUCT_ID) {

                    ostringstream os;
                    os << "TS3M_" << ++TS3MSourceCount << endl;
                    cerr << "Opening " << os.str() << " ...";
                    TS3MSource[TS3MSourceCount] = new TS3MEventSource(cur_dev, os.str(), verbose);

                    if (TS3MSource[TS3MSourceCount] != NULL && TS3MSource[TS3MSourceCount]->isOk()) {
                        cerr << "...succeeded" << endl;
                        TS3MSource[TS3MSourceCount]->start(true); // run in background thread
                        cerr << "3M source " << TS3MSourceCount << " started " << endl;
                    } else {
                        TS3MSourceCount--;
                        cerr << "...failed" << endl;
                    }

                }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);

    return TS3MSourceCount;
}

/**
 *  print devices informations (product id, vendor id, manufacturer
 *  \param devs list of devices
 */
void print_devs(struct hid_device_info *devs)
{
    struct hid_device_info *cur_dev;
    cur_dev = devs;
    while (cur_dev) {
        printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls",
               cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
        printf("\n");
        printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
        printf("  Product:      %ls\n", cur_dev->product_string);
        printf("  usage:      %d\n", cur_dev->usage);
        printf("  usage_page:      %d\n", cur_dev->usage_page);
        printf("\n");
        cur_dev = cur_dev->next;
    }
}

#include "ThisThread.h"
#include "mbed.h"
#include <iostream>

#define MOCKING

using namespace std;
class StorageDevice {
    private:
    string fn;

    public:
    StorageDevice() {
        cout << "Storage devices instantiated" << endl;
    }

    int openDevice(string filename)
    {
        fn = filename;
        cout << "Opening " << filename << endl;
        #ifdef MOCKING
        return 0;
        #endif

        // Open SD card
        //
        // TO BE DONE
        return 0;
    }

    int closeDevice() 
    {
        cout << "Closing " << fn << endl;
        #ifndef MOCKING
        //Close the SD card
        #endif
        return 0;
    }

    int write(int data)
    {
        if (fn.empty()) {
            cout << "Error - forgot to open device!";
            return -1;
        }
        cout << "Attempting to write " << data << " to file storage" << endl;

        #ifdef MOCKING
        //Simulating
        ThisThread::sleep_for(1ms);
        cout << data << endl;
        return 0;
        #else
        //REAL CODE
        // TBD - write to SD card - return result
        cout << "Attempting to writing to SD card" << endl;

        #endif

        return 0;
    }

};
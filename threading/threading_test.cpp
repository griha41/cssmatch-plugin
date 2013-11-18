
#include "threading.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace threading;
using namespace std;

//Event evt;

class MyThread : public Thread
{
private:
    Mutex guard;
public:
    void run()
    {
        cout << "MyThread::run() enter" << endl;
        cout << "MyThread::locking()" << endl;
        guard.lock();
        cout << "MyThread::locked()" << endl;
        cout << "MyThread::unlocking()" << endl;
        guard.unlock();
        cout << "MyThread::unlocked()" << endl;
        //for (int i = 0; i < 2; i++)
        //{
        //    cout << "Try " << i << endl;
        //    EventWaitResult result;
        //    do
        //    {
        //        cout << "MyThread::run() tick" << endl;
        //        result = evt.wait(500);
        //    }
        //    while (result == THREADING_EVENT_TIMEOUT);
        //}
        cout << "MyThread::run() exit" << endl;
    }
};

class MyThreadEvt : public Thread
{
public:
    void run()
    {
        for (int i = 0; i < 3; i++)
        {
            threading::sleep(2000);
            //evt.set();
        }
    }
};

int main( int argc, char ** argv)
{
    MyThread thr;
    thr.start();

    MyThreadEvt thrEvt;
    thrEvt.start();

    thr.join();
    
    cout << "Finished! Press enter to close the window." << endl;
    char dump = cin.get();
    return 0;
}

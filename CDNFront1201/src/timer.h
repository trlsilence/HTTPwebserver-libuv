//第一个参数which可选的值为：
/*
	函数原型：int setitimer(int which, const struct itimerval *new_value,  struct itimerval *old_value);  
	ITIMER_REAL：以系统真实的时间来计算，它送出SIGALRM信号。
	ITIMER_VIRTUAL：以该进程在用户态下花费的时间来计算，它送出SIGVTALRM信号。
	ITIMER_PROF：以该进程在用户态下和内核态下所费的时间来计算，它送出SIGPROF信号
 */
#include <iostream>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>

using namespace std;

volatile static long counter = 0;
void uninit_time(int sec);
static void notify(int signum);

typedef void(*ProcessHandler)(int);

class Timer
{
    protected:
    struct sigaction  _act;
    struct sigaction  _oldact;
    struct itimerval  _tim_ticks;
    void wait( long timeout_ms )
    {
            struct timespec spec;
            spec.tv_sec  = timeout_ms / 1000;
            spec.tv_nsec = (timeout_ms % 1000) * 1000000;
            nanosleep(&spec,NULL);
    }
   
    public:
    Timer(int sec,int usec)
	{
            _tim_ticks.it_value.tv_sec = sec;
            _tim_ticks.it_value.tv_usec = usec;
            _tim_ticks.it_interval.tv_sec = sec;
            _tim_ticks.it_interval.tv_usec = usec;
    }

    void setHandler(ProcessHandler handler)
    {
            sigemptyset( &_act.sa_mask );
            _act.sa_flags = 0;
            _act.sa_handler = handler;
    }

    bool reset()
	{
        int res = sigaction( SIGALRM, &_act, &_oldact );
        if (res) 
		{
            cout<<"Fail to install handle: "<<endl;
            return false;
        }
        res = setitimer( ITIMER_REAL, &_tim_ticks, 0 );
        if(res)
		{
            cout<<"Fail to set timer: "<<endl;
            sigaction(SIGALRM, &_oldact, 0 );
            return false;
        }
        for ( ; ; )
		{
            wait(1000);
        }
    }
};

static void notify( int signum )
{
    assert( signum == SIGALRM );
    ++ counter;
    cout << "------------counter = " << counter << endl;
    if(counter == 5)
    {
        uninit_time(0);
        cout<<"-----------定时器关闭了，重新发送数据！---------"<<endl;
        counter = 0;
        notify(0);
    }
}

/*int timertest()
{
        Timer timer(1,0);
        timer.setHandler(notify);
        timer.reset();
        return 0;
}*/


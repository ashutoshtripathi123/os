#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include<signal.h>
#include<sys/types.h>
#include<unistd.h>
#include <errno.h>
#include <string.h>

/*as per gamedev algo*/

#define PI  3.142
#define WHEEL_DIAMETER 0.00031 //its 0.5 Meters in Miles

float getMin(float a, float b)
{
	printf("return of getMin = %f \n", (a<b)?a:b);
	return ((a<b)?a:b);
}

float getMax(float a, float b)
{
	printf("return of getMax = %f \n", (a>b)?a:b);
	return ((a>b)?a:b);
}

float rpm2mph(int rpm)
{
    return(WHEEL_DIAMETER*PI*rpm*60);
}

float mph2rpm(float mph)
{
	return((mph/(WHEEL_DIAMETER*PI*60)));
}

static void sig_handler(int sig, siginfo_t *si, void *uc)
{
	printf("sig_handler ...\n");
}

static void sig_handler_1(int sig, siginfo_t *si, void *uc)
{
	printf("sig_handler_1 ...\n");
}

static void sig_handler_2(int sig, siginfo_t *si, void *uc)
{
	printf("sig_handler_2 ...\n");
}

void main()
{
	#if 0
	float a,b,x;
	float X,A,B;
	printf("a,b - min, max values of ur desired range (float) ...\n");
	printf("A,B - min, max values of original range (float) ...\n");
	printf("X - input value (float) ...\n");
	printf("x - normalized output...\n");
	scanf("%f%f%f%f%f",&a,&b,&A,&B,&X);
	x = (X-A) * (b - a)/(B - A);
	printf("Normalized value = %f ...\n", x);
    float m,n,o,p;
	float ret = (getMin(getMax(-6,-100),100) / 100) * 24;
	printf("ret = %f \n", ret);
	#endif
	int rpm = -200;//at 171 we get 10mph
	float mph=0; 
	float userSpeed = 10.0;
	struct sigevent sig_ev, sig_ev1, sig_ev2;
	struct itimerspec timer_spec, timer_spec1, timer_spec2;
	timer_t timerid, timerid1, timerid2;
	//sigset_t mask;
	struct sigaction action;
	struct sigaction action1;
	struct sigaction action2;
	memset(&action, 0, sizeof(action));
	memset(&action1, 0, sizeof(action1));
	memset(&action2, 0, sizeof(action2));
    
	sig_ev.sigev_notify = SIGEV_SIGNAL;
	sig_ev.sigev_signo = SIGRTMIN;
	sig_ev.sigev_value.sival_ptr = &timerid;
	
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = sig_handler;
	if(sigaction((SIGRTMIN), &action, NULL) == -1) {
	  printf( "\t %d Error establishing signal handler. Exiting...\n", __LINE__);
	  return;
	} else {
	  printf( "\tTimer routine attached as signal handler\n" );
	}

	if(timer_create(CLOCK_REALTIME, &sig_ev, &timerid) == -1) {
	  printf( "\tUnable to create timer.  Exiting...\n" );
	  return;
	} else {
	  printf( "\tTimer created. (ID = 0x%lx)\n", (long)timerid );
	}

    timer_spec.it_value.tv_sec = 0;
	timer_spec.it_value.tv_nsec = 100000000;//1000000;//1 ms timer tick
	timer_spec.it_interval.tv_sec = timer_spec.it_value.tv_sec;
	timer_spec.it_interval.tv_nsec = timer_spec.it_value.tv_nsec;

	if(timer_settime(timerid, 0, &timer_spec, NULL) == -1) {
	  printf( "\tError during timer setup and starting\n" );
	} else {
	  printf( "\tTimer setup and started.\n" );
	}
	
    #if 1
//timer1
	sig_ev1.sigev_notify = SIGEV_SIGNAL;
	sig_ev1.sigev_signo = SIGUSR1;
	sig_ev1.sigev_value.sival_ptr = &timerid1;
	
	action1.sa_flags = SA_SIGINFO;
	action1.sa_sigaction = sig_handler_1;
	if(sigaction(SIGUSR1, &action1, NULL) == -1) {
	  printf( "\t %d Error establishing signal handler. Exiting... %d \n", __LINE__, errno );
	  return;
	} else {
	  printf( "\tTimer routine attached as signal handler\n" );
	}

	if(timer_create(CLOCK_REALTIME, &sig_ev1, &timerid1) == -1) {
	  printf( "\tUnable to create timer.  Exiting...\n" );
	  return;
	} else {
	  printf( "\tTimer created. (ID = 0x%lx)\n", (long)timerid1 );
	}

    timer_spec1.it_value.tv_sec = 2;
	timer_spec1.it_value.tv_nsec = 0;//1000000;//1 ms timer tick
	timer_spec1.it_interval.tv_sec = timer_spec1.it_value.tv_sec;
	timer_spec1.it_interval.tv_nsec = timer_spec1.it_value.tv_nsec;

	if(timer_settime(timerid1, 0, &timer_spec1, NULL) == -1) {
	  printf( "\tError during timer setup and starting\n" );
	} else {
	  printf( "\tTimer setup and started.\n" );
	}

//timer 2
	sig_ev2.sigev_notify = SIGEV_SIGNAL;
	sig_ev2.sigev_signo = SIGUSR2;
	sig_ev2.sigev_value.sival_ptr = &timerid2;
	
	action2.sa_flags = SA_SIGINFO;
	action2.sa_sigaction = sig_handler_2;
	if(sigaction(SIGUSR2, &action2, NULL) == -1) {
	  printf( "\t %d Error establishing signal handler. Exiting...\n", __LINE__);
	  return;
	} else {
	  printf( "\tTimer routine attached as signal handler\n" );
	}

	if(timer_create(CLOCK_REALTIME, &sig_ev2, &timerid2) == -1) {
	  printf( "\tUnable to create timer.  Exiting...\n" );
	  return;
	} else {
	  printf( "\tTimer created. (ID = 0x%lx)\n", (long)timerid2);
	}

    timer_spec2.it_value.tv_sec = 3;
	timer_spec2.it_value.tv_nsec = 0;//1000000;//1 ms timer tick
	timer_spec2.it_interval.tv_sec = timer_spec2.it_value.tv_sec;
	timer_spec2.it_interval.tv_nsec = timer_spec2.it_value.tv_nsec;

	if(timer_settime(timerid2, 0, &timer_spec2, NULL) == -1) {
	  printf( "\tError during timer setup and starting\n" );
	} else {
	  printf( "\tTimer setup and started.\n" );
	}
    #endif
	mph = rpm2mph(rpm);
	printf("mph=%f\n", mph);
	rpm = mph2rpm(mph);
	printf("rpm=%d\n", rpm);
	if(abs((int)mph) > (int )userSpeed)
	{
		printf("Caught ... \n");
	}
	rpm = rpm>>1;
	mph = rpm2mph(rpm);
	printf("mph=%f\n", mph);
	rpm = mph2rpm(mph);
	printf("rpm=%d\n", rpm);
	while(1);
}






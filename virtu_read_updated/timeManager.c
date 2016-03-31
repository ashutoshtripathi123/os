#include "global.h"
void timeManager()
{
	/*********** file write and print on display running every 1 sec ****************/
	sig_ev_file.sigev_notify = SIGEV_SIGNAL;
	sig_ev_file.sigev_signo = SIGRTMIN;
	sig_ev_file.sigev_value.sival_ptr = &timerid;
	
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = file_writer;
	if(sigaction(SIGRTMIN, &action, NULL) == -1) {
	  printf( "\tError establishing signal handler. Exiting...\n" );
	  return;
	} else {
	  printf( "\tTimer routine attached as signal handler\n" );
	}

	if(timer_create(CLOCK_REALTIME, &sig_ev_file, &timerid) == -1) {
	  printf( "\tUnable to create timer.  Exiting...\n" );
	  return;
	} else {
	  printf( "\tTimer created. (ID = 0x%lx)\n", (long)timerid );
	}

    timer_spec.it_value.tv_sec = 1;
	timer_spec.it_value.tv_nsec = 0;
	timer_spec.it_interval.tv_sec = timer_spec.it_value.tv_sec;
	timer_spec.it_interval.tv_nsec = timer_spec.it_value.tv_nsec;

	if(timer_settime(timerid, 0, &timer_spec, NULL) == -1) {
	  printf( "\tError during timer setup and starting for file write\n" );
	} else {
	  printf( "\tTimer setup and started for file write.\n" );
	}

	/*********** chairControllerLogic running every 15msec ****************/
	sig_ev_chair.sigev_notify = SIGEV_SIGNAL;
	sig_ev_chair.sigev_signo = SIGUSR1;
	sig_ev_chair.sigev_value.sival_ptr = &timerid1;
	
	action1.sa_flags = SA_SIGINFO;
	action1.sa_sigaction = chairControllerLogic;
	if(sigaction(SIGUSR1, &action1, NULL) == -1) {
	  printf( "\tError establishing signal handler. Exiting...\n" );
	  return;
	} else {
	  printf( "\tTimer routine attached as signal handler\n" );
	}

	if(timer_create(CLOCK_REALTIME, &sig_ev_chair, &timerid1) == -1) {
	  printf( "\tUnable to create timer1.  Exiting...\n" );
	  return;
	} else {
	  printf( "\tTimer created. (ID = 0x%lx)\n", (long)timerid1 );
	}

    timer_spec1.it_value.tv_sec = 0;
	timer_spec1.it_value.tv_nsec = 15000000;
	timer_spec1.it_interval.tv_sec = timer_spec1.it_value.tv_sec;
	timer_spec1.it_interval.tv_nsec = timer_spec1.it_value.tv_nsec;

	if(timer_settime(timerid1, 0, &timer_spec1, NULL) == -1) {
	  printf( "\tError during timer setup and starting for CHAIR \n" );
	} else {
	  printf( "\tTimer setup and started for CHAIR.\n" );
	}

	/*********** printOnScreen running every 100msec ****************/
	sig_ev_print.sigev_notify = SIGEV_SIGNAL;
	sig_ev_print.sigev_signo = SIGUSR2;
	sig_ev_print.sigev_value.sival_ptr = &timerid2;
	
	action2.sa_flags = SA_SIGINFO;
	action2.sa_sigaction = printOnScreen;
	if(sigaction(SIGUSR2, &action2, NULL) == -1) {
	  printf( "\tError establishing signal handler. Exiting...\n" );
	  return;
	} else {
	  printf( "\tTimer routine attached as signal handler\n" );
	}

	if(timer_create(CLOCK_REALTIME, &sig_ev_print, &timerid2) == -1) {
	  printf( "\tUnable to create timer2.  Exiting...\n" );
	  return;
	} else {
	  printf( "\tTimer created. (ID = 0x%lx)\n", (long)timerid2 );
	}

    timer_spec2.it_value.tv_sec = 0;
	timer_spec2.it_value.tv_nsec = 100000000;
	timer_spec2.it_interval.tv_sec = timer_spec2.it_value.tv_sec;
	timer_spec2.it_interval.tv_nsec = timer_spec2.it_value.tv_nsec;

	if(timer_settime(timerid2, 0, &timer_spec2, NULL) == -1) {
	  printf( "\tError during timer setup and starting for CHAIR \n" );
	} else {
	  printf( "\tTimer setup and started for CHAIR.\n" );
	}
}

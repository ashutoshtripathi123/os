/*#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <math.h>
*/
#include "global.h"

float userSpeed = 10.0;
char fileop[25]="output.csv";
static void file_writer(int sig, siginfo_t *si, void *uc);
void read_adc(){
	off_t offset = 0xc0000000;
    //size_t len = 40;
	 size_t pagesize = sysconf(_SC_PAGE_SIZE);
    off_t page_base = (offset / pagesize) * pagesize;
    off_t pageOffset = offset - page_base;
	
	myADC.Seat_Height = ((uint8_t)mem[pageOffset+0+0]<<8 |(uint8_t)mem[pageOffset+0+1]);
	myADC.Joystic_X = ((uint8_t)mem[pageOffset+2+0]<<8 |(uint8_t)mem[pageOffset+2+1]);
	myADC.Joystic_Y = ((uint8_t)mem[pageOffset+4+0]<<8 |(uint8_t)mem[pageOffset+4+1]);
	myADC.LWD_MTR_SPD = ((uint8_t)mem[pageOffset+6+0]<<8 |(uint8_t)mem[pageOffset+6+1]);
	myADC.RWD_MTR_SPD = ((uint8_t)mem[pageOffset+8+0]<<8 |(uint8_t)mem[pageOffset+8+1]);
	myADC.TEST_WRAP_INP = ((uint8_t)mem[pageOffset+10+0]<<8 |(uint8_t)mem[pageOffset+10+1]);
	myADC.Spare_ADC_6 = ((uint8_t)mem[pageOffset+12+0]<<8 |(uint8_t)mem[pageOffset+12+1]);
	myADC.Spare_ADC_7 = ((uint8_t)mem[pageOffset+14+0]<<8 |(uint8_t)mem[pageOffset+14+1]);

    myADC.Seat_Height_Volt = calcVolt(myADC.Seat_Height, 0.25);
    myADC.Joystic_X_Volt = calcVolt(myADC.Joystic_X, 0.25);
    myADC.Joystic_Y_Volt = calcVolt(myADC.Joystic_Y, 0.25);
    myADC.LWD_MTR_SPD_Volt = calcVolt(myADC.LWD_MTR_SPD, 0.25);
    myADC.RWD_MTR_SPD_Volt = calcVolt(myADC.RWD_MTR_SPD, 0.25);
    myADC.TEST_WRAP_INP_Volt = calcVolt(myADC.TEST_WRAP_INP, 0.25);
	myADC.Spare_ADC_6_Volt = calcVolt(myADC.Spare_ADC_6, 0.25);
	myADC.Spare_ADC_7_Volt = calcVolt(myADC.Spare_ADC_7, 0.25);

	myADC.Seat_Height_Engg_Unit = calcEnggUnitForChair(myADC.Seat_Height_Volt);
	myADC.Joystic_X_Engg_Unit = calcEnggUnit(myADC.Joystic_X_Volt, -100);
	myADC.Joystic_Y_Engg_Unit = calcEnggUnit(myADC.Joystic_Y_Volt, -100);
	myADC.LWD_MTR_SPD_Engg_Unit = calcEnggUnitForSpd(myADC.LWD_MTR_SPD_Volt, -200);
	myADC.RWD_MTR_SPD_Engg_Unit = calcEnggUnitForSpd(myADC.RWD_MTR_SPD_Volt, -200);
	myADC.TEST_WRAP_INP_Engg_Unit = calcEnggUnit(myADC.TEST_WRAP_INP_Volt, -100);
	myADC.Spare_ADC_6_Engg_Unit = calcEnggUnit(myADC.Spare_ADC_6_Volt, -100);
	myADC.Spare_ADC_7_Engg_Unit = calcEnggUnit(myADC.Spare_ADC_7_Volt, -100);

	myADC.left_motor_speed_demand = 2.0 * (myADC.Joystic_Y_Engg_Unit + myADC.Joystic_X_Engg_Unit / 4.0);
    myADC.right_motor_speed_demand = 2.0 * (myADC.Joystic_Y_Engg_Unit - myADC.Joystic_X_Engg_Unit / 4.0);

    myADC.left_wheel_speed = rpm2mph(myADC.left_motor_speed_demand);
    myADC.right_wheel_speed = rpm2mph(myADC.right_motor_speed_demand);

	if(myADC.Seat_Height_Volt < 0.25 || myADC.Joystic_X_Volt < 0.25 || myADC.Joystic_Y_Volt < 0.25)
	{
		printf("The input value from the plant model is too low !!! \n");
	}
	if(myADC.Seat_Height_Volt > 4.8 || myADC.Joystic_X_Volt > 4.8 || myADC.Joystic_Y_Volt > 4.8)
	{
		printf("The input value from the plant model is too High !!! \n");
	}

}
void write_dac(){
	int dac0,dac1,dac2,dac3;
	int upper, lower;
	off_t offset = 0xc0000020;
	size_t pagesize = sysconf(_SC_PAGE_SIZE);
    off_t page_base = (offset / pagesize) * pagesize;
    off_t pageOffset = offset - page_base;

	//After all the motor_speed_demand is nothing but engg unit
	speedLimitCheck();
	
	#if 0
	switch(methodOfCalculatingDac)
	{
		case 0:
		{
			myDAC.left_motor_drive_gain = GAIN*myADC.left_motor_speed_demand;
	        myDAC.right_motor_drive_gain = GAIN*myADC.right_motor_speed_demand;
		}
		break;
		case 1:
		{
			//Integration Algorithm
			myDAC.left_motor_drive_gain = (myADC.left_motor_speed_demand*(GAIN/**userDurationExpired*/))+output_state_x;
	        myDAC.right_motor_drive_gain = (myADC.right_motor_speed_demand*(GAIN/**userDurationExpired*/))+output_state_y;
			if(myDAC.left_motor_drive_gain > 200)
			{
				output_state_x = 200;
			}
			if(myDAC.right_motor_drive_gain > 200)
			{
				output_state_y = 200;
			}
			if(myDAC.left_motor_drive_gain < -200 && myDAC.right_motor_drive_gain < -200)
			{
				output_state_x = -200;
			}
			if(myDAC.right_motor_drive_gain < -200)
			{
				output_state_y = -200;
			}
			else
			{
				output_state_x =  myDAC.left_motor_drive_gain;
				output_state_y =  myDAC.right_motor_drive_gain;
			}
		}
			break;
		case 2:
			//Derivative Algorithm
			myDAC.left_motor_drive_gain = (myADC.left_motor_speed_demand - input_state_x);//*(1/userDurationExpired);
	        myDAC.right_motor_drive_gain = (myADC.right_motor_speed_demand - input_state_y);//*(1/userDurationExpired);
			input_state_x = myDAC.left_motor_drive_gain;
			input_state_y = myDAC.right_motor_drive_gain;
			break;
	}
	#endif
	myDAC.left_motor_drive_gain = GAIN*myADC.left_motor_speed_demand;
	myDAC.right_motor_drive_gain = GAIN*myADC.right_motor_speed_demand;

    myDAC.LWD_MTR_DMND_Volt = myDAC.left_motor_drive_gain/8.33;
	myDAC.LWD_MTR_DMND_Engg_Unit = (1023*(myDAC.LWD_MTR_DMND_Volt + 24))/48;
	
	dac0 = myDAC.LWD_MTR_DMND_Engg_Unit;
	upper=0;
	lower=0;
	upper= (int)(dac0>>8);
	lower= (int)dac0;
	*(mem+pageOffset+0) = upper;
	*(mem+pageOffset+1) = lower;

    myDAC.RWD_MTR_DMND_Volt = myDAC.right_motor_drive_gain/8.33;
	myDAC.RWD_MTR_DMND_Engg_Unit = (1023*(myDAC.RWD_MTR_DMND_Volt + 24))/48;
    dac1 = myDAC.RWD_MTR_DMND_Engg_Unit;
		
	upper=0;
	lower=0;
	upper= (int)(dac1>>8);
	lower= (int)dac1;
	*(mem+pageOffset+2) = upper;
	*(mem+pageOffset+3) = lower;

	dac2 = 0;

	upper=0;
	lower=0;
	upper= (int)(dac2>>8);
	lower= (int)dac2;
	*(mem+pageOffset+4) = upper;
	*(mem+pageOffset+5) = lower;

	dac3 = 0;

	upper=0;
	lower=0;
	upper= (int)(dac3>>8);
	lower= (int)dac3;
	*(mem+pageOffset+6) = upper;
	*(mem+pageOffset+7) = lower;
}
void read_dac(){
	off_t offset = 0xc0000020;
	size_t pagesize = sysconf(_SC_PAGE_SIZE);
    off_t page_base = (offset / pagesize) * pagesize;
    off_t pageOffset = offset - page_base;
	
	myDAC.LWD_MTR_DMND = ((int)mem[pageOffset+0+0]<<8 |(int)mem[pageOffset+0+1]);
	myDAC.RWD_MTR_DMND = ((int)mem[pageOffset+2+0]<<8 |(int)mem[pageOffset+2+1]);
	myDAC.TEST_WRP_OP = ((int)mem[pageOffset+4+0]<<8 |(int)mem[pageOffset+4+1]);
	myDAC.SPARE_DAC_3 = ((int)mem[pageOffset+6+0]<<8 |(int)mem[pageOffset+6+1]);
}

void read_gpio()
{
	int num1=0,num2=0;
	off_t offset = 0xc0000018;
    size_t pagesize = sysconf(_SC_PAGE_SIZE);
	off_t page_base = (offset / pagesize) * pagesize;
    off_t page_offset = offset - page_base;
	num1 = (int)mem[page_offset  + 1];
	num2 = (int)mem[page_offset  + 3];

	keys.seat_switch = (num1 & 128)>>7;
	keys.seat_occupy = ((num1 & 4 )== 4)?1:0;

	keys.kill_switch_fit = (num2 & 16)>>4;
	keys.kill_key_pressed = (num2 & 48)>>5;
	keys.kill_k_pressed = (num2 & 112)>>6;
}

int main(int argc, char *argv[]) {

	if (process_args(argc,argv))
	{
		return 0; 
	}
	signal(SIGINT, INThandler);
	sig_ev.sigev_notify = SIGEV_SIGNAL;
	sig_ev.sigev_signo = SIGRTMIN;
	sig_ev.sigev_value.sival_ptr = &timerid;
	
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = file_writer;
	//action.sa_handler = sig_handler;
	if(sigaction(SIGRTMIN, &action, NULL) == -1) {
	  printf( "\tError establishing signal handler. Exiting...\n" );
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

    timer_spec.it_value.tv_sec = 1.0;
	timer_spec.it_value.tv_nsec = 0;//1 ms timer tick
	timer_spec.it_interval.tv_sec = timer_spec.it_value.tv_sec;
	timer_spec.it_interval.tv_nsec = timer_spec.it_value.tv_nsec;

	if(timer_settime(timerid, 0, &timer_spec, NULL) == -1) {
	  printf( "\tError during timer setup and starting\n" );
	} else {
	  printf( "\tTimer setup and started.\n" );
	}

	printf("\033[2J");
   printf("\033[2H");
    off_t offset = 0xc0000000;
    size_t len = 40;
    // Truncate offset to a multiple of the page size, or mmap will fail.
    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    off_t page_base = (offset / pagesize) * pagesize;
    off_t page_offset = offset - page_base;
	if(access(fileop,F_OK)!=-1)
			remove(fileop);
	fptr = fopen(fileop, "a");
	fprintf(fptr, "ADC0, ADC1, ADC2, ADC3, ADC4, ADC5, ADC6, ADC7, DAC0, DAC1, DAC2, DAC3, Time\n");
	fclose(fptr);

    fd = open("/dev/mem", O_RDWR |  O_SYNC);
    mem = mmap(NULL, page_offset + len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, page_base);

    if (mem == MAP_FAILED) {
        perror("Can't map memory");
        return -1;
    }
	while(1){
	read_adc();		
			
	read_dac();
    
    printf("\n============ADC=============\n");
    printf("X=%d :: Y = %d \n", myADC.Joystic_X,  myADC.Joystic_Y);
	printf("Seat Height = %d \n", myADC.Seat_Height_Engg_Unit);
	printf("Demand (RPM):: Left = %d :: Right = %d \n",
		myADC.left_motor_speed_demand, myADC.right_motor_speed_demand);

	printf("\n============DAC=============\n");
	printf("\nLEFT DMND = %d :: LEFT Volt = %f \n"
	       " RIGHT DMND = %d :: RIGHT Volt = %f \n",
	myDAC.LWD_MTR_DMND, myDAC.LWD_MTR_DMND_Volt, myDAC.RWD_MTR_DMND, myDAC.RWD_MTR_DMND_Volt);
	
	read_gpio();

    if(keys.kill_k_pressed == 1)
	{
		keys.kill_switch_fit = 0;
	}
    printf("\n============GPIO=============\n");
	printf("seat_switch = %d, seat_occupy = %d, kill_switch_fit = %d, kill_key_pressed = %d, K_KEY_pressed = %d \n", 
    keys.seat_switch, keys.seat_occupy,keys.kill_switch_fit, keys.kill_key_pressed, keys.kill_k_pressed);

	findFault();
	controlManager();
	printf("\n");
	#if 0
	fptr = fopen(fileop, "a");
	fprintf(fptr, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d \n",
		    myADC.Seat_Height,
			myADC.Joystic_X,
			myADC.Joystic_Y,
			myADC.LWD_MTR_SPD,
			myADC.RWD_MTR_SPD,
			myADC.TEST_WRAP_INP,
			myADC.Spare_ADC_6 ,
			myADC.Spare_ADC_7,
			myDAC.LWD_MTR_DMND,
			myDAC.RWD_MTR_DMND ,
			myDAC.TEST_WRP_OP ,
			myDAC.SPARE_DAC_3); // write to csv in each update interval
	fclose(fptr);
	#endif
	printf("\033[2J");
   printf("\033[2H");
   sleep(1);
	if(userDurationExpired == userDuration)
	{
		munmap(mem, 40);
		close(fd);
		exit(1);
	}
  }//while(1) ends
    return 0;

}
void  INThandler(int sig)
{
     char  c;

     signal(sig, SIG_IGN);
     if (1)//(c == 'y' || c == 'Y')
	 {
		 munmap(mem, 40);
		 printf("Exiting.......\n");
		close(fd);
		
          exit(0);
	 }
     else
          signal(SIGINT, INThandler);
     getchar(); // Get new line character
}

static void file_writer(int sig, siginfo_t *si, void *uc)
{
	fptr = fopen(fileop, "a");
	fprintf(fptr, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d \n",
		    myADC.Seat_Height,
			myADC.Joystic_X,
			myADC.Joystic_Y,
			myADC.LWD_MTR_SPD,
			myADC.RWD_MTR_SPD,
			myADC.TEST_WRAP_INP,
			myADC.Spare_ADC_6 ,
			myADC.Spare_ADC_7,
			myDAC.LWD_MTR_DMND,
			myDAC.RWD_MTR_DMND ,
			myDAC.TEST_WRP_OP ,
			myDAC.SPARE_DAC_3,
		    userDurationExpired); // write to csv in each update interval
	fclose(fptr);
	++userDurationExpired;
}
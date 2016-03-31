
#include "global.h"
unsigned int count1sec=1;
float userSpeed = 10.0;
char fileop[25]="output.csv";
float KP = 100;//2.2;
float KI = 0;//3.2;
float KD = 0;//0.3;
static float error_left, integral_left, derivative_left, error_left_previous; 
static float error_right, integral_right, derivative_right, error_right_previous;
//static void file_writer(int sig, siginfo_t *si, void *uc);

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
	
	#if 1
	switch(methodOfCalculatingDac)
	{
		case 0:
		{   //Straight Through
			myDAC.left_motor_drive_gain = gain*myADC.left_motor_speed_demand;
	        myDAC.right_motor_drive_gain = gain*myADC.right_motor_speed_demand;
		}
		break;
		case 1:
		{
			#if 1
			//PID Calculation:
			//Integration Algorithm
			//kp=1.2;
			kix = (myADC.left_motor_speed_demand*(gain*1))+output_state_x; // userDurationExpired has been replaced with 0.1
	        kiy = (myADC.right_motor_speed_demand*(gain*1))+output_state_y;
			if(kix > 200)
			{
				output_state_x = 200;
			}
			else if(kix < -200)
			{
				output_state_x = -200;
			}
			else
			{
				output_state_x = kix;
			}

			if(kiy > 200)
			{
				output_state_y = 200;
			}
			else if(kiy < -200)
			{
				output_state_y = -200;
			}
			else
			{
				output_state_y =  kiy;
			}
	
			//Derivative Algorithm
			kdx = (myADC.left_motor_speed_demand - input_state_x)*(1/1);
	        kdy = (myADC.right_motor_speed_demand - input_state_y)*(1/1);
			input_state_x = kdx;
			input_state_y = kdy;

			kpx = 1;//(myADC.LWD_MTR_SPD - myADC.left_motor_speed_demand));
			kpy = 1;//2.76;//100;//*(myADC.RWD_MTR_SPD - myADC.right_motor_speed_demand));
	     }
		 printf("Speed Demand = %d, Left Motor Speed = %d \n", myADC.left_motor_speed_demand, myADC.LWD_MTR_SPD);
         myDAC.left_motor_drive_gain = kpx+(kix)+(kdx);
         myDAC.right_motor_drive_gain = kpy+(kiy)+(kdy);
		 #endif
		break;
	}
	#endif

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

	timeManager();
    off_t offset = 0xc0000000;
    size_t len = 40;
    // Truncate offset to a multiple of the page size, or mmap will fail.
    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    off_t page_base = (offset / pagesize) * pagesize;
    off_t page_offset = offset - page_base;
	if(access(fileop,F_OK)!=-1)
			remove(fileop);
	fptr = fopen(fileop, "a");
	fprintf(fptr, "ADC0, ADC1, ADC2, ADC3, ADC4, ADC5, ADC6, ADC7, DAC0, DAC1, DAC2, DAC3, TimeLapsed, ControllerCount, PrintCount, FileCount\n");
	fclose(fptr);

    fd = open("/dev/mem", O_RDWR |  O_SYNC);
    mem = mmap(NULL, page_offset + len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, page_base);

    if (mem == MAP_FAILED) {
        perror("Can't map memory");
        return -1;
    }

	while(1)
	{
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
		 if(fd)
			close(fd);
         exit(0);
	 }
     else
          signal(SIGINT, INThandler);
     getchar(); // Get new line character
}

void file_writer(int sig, siginfo_t *si, void *uc)
{
	struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
	static float old=0;
	fptr = fopen(fileop, "a");
	fprintf(fptr, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d \n",
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
		    userDurationExpired,
		    controlManagerCount, 
		    printCount,
		    fileWriteCount); // write to csv in each update interval
	fclose(fptr);
	userDurationExpired++;
	fileWriteCount++;
	clock_gettime(CLOCK_REALTIME, &end);
    fileWriteDiff = ((float)(end.tv_nsec-start.tv_nsec)/1000000);
	fileWriteMinorExec = fileWriteMinorExec + fileWriteDiff;
	old = old + fileWriteDiff;
	fileWriteAvg = old/fileWriteCount;
	fileWriteMinorSave = fileWriteMinorSave + (1000 - fileWriteDiff);
}
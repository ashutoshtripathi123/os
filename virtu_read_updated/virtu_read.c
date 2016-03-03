#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <poll.h>

#define SEAT_HEIGHT_ADDR 0xC0000000 // ADC 0 memory address
#define JSTIC_X_ADDR 0xC0000002 // ADC 1 memory address
#define JSTIC_Y_ADDR 0xC0000004 // ADC 2 memory address
#define LWD_MTR_SPD_ADDR 0xC0000006 // ADC 3 memory address
#define RWD_MTR_SPD_ADDR 0xC0000008 // ADC 4 memory address
#define TEST_WRP_INP_ADDR 0xC000000A // ADC 5 memory address
#define SPARE_ADC_6_ADDR 0xC000000C // ADC 6 memory address
#define SPARE_ADC_7_ADDR 0xC000000E // ADC 7 memory address

#define GPIO_1_ADDR 0XC0000018 // GPIO 0 ADDRESS
#define GPIO_2_ADDR 0XC000001C // GPIO 1 ADDRESS

#define LWD_MTR_DMND_ADDR 0xC0000020 // DAC 0 memory address
#define RWD_MTR_DMND_ADDR 0xC0000022 // DAC 1 memory address
#define TEST_WRP_OP_ADDR 0xC0000024 // DAC 2 memory address
#define SPARE_DAC_3_ADDR 0xC0000026 // DAC 3 memory address

#define MIN_COUNT_RANGE 51
#define MAX_COUNT_RANGE 970

#define MIN_VOLT_RANGE 0.25
#define MAX_VOLT_RANGE 4.75

#define MIN_SEAT_HEIGHT_RANGE -1
#define MAX_SEAT_HEIGHT_RANGE 6

#define MIN_ENGG_UNIT_RANGE -100
#define MAX_ENGG_UNIT_RANGE 100

#define MIN_ENGG_UNIT_SPD_RANGE -200
#define MAX_ENGG_UNIT_SPD_RANGE 200

#define MIN_DAC_VOLT_RANGE -24
#define MAX_DAC_VOLT_RANGE 24

#define MIN_DAC_ENGG_UNIT_RANGE 0
#define MAX_DAC_ENGG_UNIT_RANGE 1023

void     INThandler(int);

struct ADC
{
	uint16_t Seat_Height ;
	uint16_t Joystic_X;
	uint16_t Joystic_Y;
	uint16_t LWD_MTR_SPD;
	uint16_t RWD_MTR_SPD;
	uint16_t TEST_WRAP_INP;
	uint16_t Spare_ADC_6 ;
	uint16_t Spare_ADC_7 ;
	float Seat_Height_Volt;
	float Joystic_X_Volt;
	float Joystic_Y_Volt;
	float LWD_MTR_SPD_Volt;
	float RWD_MTR_SPD_Volt;
	float TEST_WRAP_INP_Volt;
	float Spare_ADC_6_Volt ;
	float Spare_ADC_7_Volt ;
	int Seat_Height_Engg_Unit;
	int Joystic_X_Engg_Unit;
	int Joystic_Y_Engg_Unit;
	int LWD_MTR_SPD_Engg_Unit;
	int RWD_MTR_SPD_Engg_Unit;
	int TEST_WRAP_INP_Engg_Unit;
	int Spare_ADC_6_Engg_Unit;
	int Spare_ADC_7_Engg_Unit;
};

struct DAC{
	int LWD_MTR_DMND ;
	int RWD_MTR_DMND ;
	int TEST_WRP_OP ;
	int SPARE_DAC_3 ;
	float LWD_MTR_DMND_Volt ;
	float RWD_MTR_DMND_Volt ;
	int LWD_MTR_DMND_Engg_Unit ;
	int RWD_MTR_DMND_Engg_Unit ;
};

struct 
{
	int seat_switch;
	int seat_occupy;
	int kill_switch_fit;
	int kill_key_pressed;
}keys;

void findFault()
{
	printf("seat_switch = %d, seat_occupy = %d, kill_switch_fit = %d, kill_key_pressed = %d \n", 
	keys.seat_switch, keys.seat_occupy,keys.kill_switch_fit, keys.kill_key_pressed);
	
	if((keys.kill_switch_fit == 0) && (keys.kill_key_pressed == 1))
	{
		printf("FAULT DETECTED: Kill Switch Pressed When Switch Not Present !!! \n");
		return;
	}
	
	if((keys.seat_switch == 0) && (keys.seat_occupy == 1))
	{
		printf("FAULT DETECTED: Seat Occupied When Switch Not Present !!! \n");
		return;
	}
}

struct ADC myADC ;
struct DAC myDAC;
int fd ;
FILE *fptr;
char fileop[25]="output.csv";
unsigned char *mem;
struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };

float calcVolt(int input, float offset)
{
	float result;
	result =  (input - MIN_COUNT_RANGE)*(MAX_VOLT_RANGE - MIN_VOLT_RANGE)/(MAX_COUNT_RANGE-MIN_COUNT_RANGE) + offset;
	return result;
}

int calcEnggUnit(float input, int offset)
{
	int result;
	result = (input - MIN_VOLT_RANGE)*(MAX_ENGG_UNIT_RANGE - MIN_ENGG_UNIT_RANGE)/(MAX_VOLT_RANGE-MIN_VOLT_RANGE) + offset;
	return result;
}

int calcEnggUnitForChair(float input)
{
    int result;
	result = (input - MIN_VOLT_RANGE)*(MAX_SEAT_HEIGHT_RANGE-MIN_SEAT_HEIGHT_RANGE)/(MAX_VOLT_RANGE-MIN_VOLT_RANGE) -1;
	return result;
}

int calcEnggUnitForSpd(float input, int offset)
{
    int result;
	result = (input - MIN_VOLT_RANGE)*(MAX_ENGG_UNIT_SPD_RANGE-MIN_ENGG_UNIT_SPD_RANGE)/(MAX_VOLT_RANGE-MIN_VOLT_RANGE) +offset;
	return result;
}

int calcDACEnggUnit(float input)
{
    int result;
	result = (input - MIN_DAC_VOLT_RANGE)*(MAX_DAC_ENGG_UNIT_RANGE-MIN_DAC_ENGG_UNIT_RANGE)/(MAX_DAC_VOLT_RANGE-MIN_DAC_VOLT_RANGE);
	return result;
}

float getMin(float a, float b)
{
	return (a<b)?a:b;
}

float getMax(float a, float b)
{
	return (a>b)?a:b;
}

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
}
void write_dac(){
	int dac0,dac1,dac2,dac3;
	int upper, lower;
	off_t offset = 0xc0000020;
	size_t pagesize = sysconf(_SC_PAGE_SIZE);
    off_t page_base = (offset / pagesize) * pagesize;
    off_t pageOffset = offset - page_base;

    myDAC.LWD_MTR_DMND_Volt = (getMin(getMax(myADC.Joystic_X_Engg_Unit,-100),100) / 100) * 24;
	myDAC.LWD_MTR_DMND_Engg_Unit = calcDACEnggUnit(myDAC.LWD_MTR_DMND_Volt);
	//printf("myDAC.LWD_MTR_DMND_Volt = %f :: myDAC.LWD_MTR_DMND_Engg_Unit = %d \n", myDAC.LWD_MTR_DMND_Volt, myDAC.LWD_MTR_DMND_Engg_Unit);
	dac0 = myDAC.LWD_MTR_DMND_Engg_Unit;
	upper=0;
	lower=0;
	upper= (int)(dac0>>8);
	lower= (int)dac0;
	*(mem+pageOffset+0) = upper;
	*(mem+pageOffset+1) = lower;

    myDAC.RWD_MTR_DMND_Volt = (getMin(getMax(myADC.Joystic_Y_Engg_Unit,-100),100) / 100) * 24;
	myDAC.RWD_MTR_DMND_Engg_Unit = calcDACEnggUnit(myDAC.RWD_MTR_DMND_Volt);
	//printf("myDAC.RWD_MTR_DMND_Volt = %f :: myDAC.RWD_MTR_DMND_Engg_Unit = %d \n", myDAC.RWD_MTR_DMND_Volt, myDAC.RWD_MTR_DMND_Engg_Unit);
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
	
	/**(mem+pageOffset+0) = 0x03;
	*(mem+pageOffset+1) = 0x80;
	*(mem+pageOffset+2) = 0x03;
	*(mem+pageOffset+3) = 0x80;
    *(mem+pageOffset+4) = 0x03;
	*(mem+pageOffset+5) = 0x80;
	*(mem+pageOffset+6) = 0x03;
	*(mem+pageOffset+7) = 0x80;*/
	//} //end of if(poll)
	
	
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


int main(int argc, char *argv[]) {

    if ((argc < 3)||(!strcmp(argv[2],"-of"))) {
        printf("Usage: %s -of <filename.csv>\n", argv[0]);
        return 0;
    }
	strcpy(fileop,argv[2]);
	 signal(SIGINT, INThandler);
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
	fprintf(fptr, "ADC0, ADC1, ADC2, ADC3, ADC4, ADC5, ADC6, ADC7, DAC0, DAC1,DAC2,DAC3\n");
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

	printf(" (ADC 0) :: Count = %d :: Volt = %f :: Engg = %d  \n" 
		   " (ADC 1) :: Count = %d :: Volt = %f :: Engg = %d  \n"
		   " (ADC 2) :: Count = %d :: Volt = %f :: Engg = %d  \n"
		   " (ADC 3) :: Count = %d :: Volt = %f :: Engg = %d  \n"
		   " (ADC 4) :: Count = %d :: Volt = %f :: Engg = %d  \n",
		   //" (ADC 5) :: Count = %d :: Volt = %f :: Engg = %d  \n"
		   //" (ADC 6) :: Count = %d :: Volt = %f :: Engg = %d  \n"
		   //" (ADC 7) :: Count = %d :: Volt = %f :: Engg = %d  \n",
	myADC.Seat_Height, myADC.Seat_Height_Volt, myADC.Seat_Height_Engg_Unit,
	myADC.Joystic_X, myADC.Joystic_X_Volt, myADC.Joystic_X_Engg_Unit,
	myADC.Joystic_Y, myADC.Joystic_Y_Volt, myADC.Joystic_Y_Engg_Unit,
	myADC.LWD_MTR_SPD, myADC.LWD_MTR_SPD_Volt, myADC.LWD_MTR_SPD_Engg_Unit,
	myADC.RWD_MTR_SPD, myADC.RWD_MTR_SPD_Volt, myADC.RWD_MTR_SPD_Engg_Unit
	//myADC.TEST_WRAP_INP, myADC.TEST_WRAP_INP_Volt, myADC.TEST_WRAP_INP_Engg_Unit,
	//myADC.Spare_ADC_6, myADC.Spare_ADC_6_Volt, myADC.Spare_ADC_6_Engg_Unit,
	//myADC.Spare_ADC_7, myADC.Spare_ADC_7_Volt, myADC.Spare_ADC_7_Engg_Unit
	);
	
	printf(" \n (DAC 0) = %d \n (DAC 1) = %d \n (DAC 2) = %d \n (DAC 3) = %d\n\n",
	myDAC.LWD_MTR_DMND,
	myDAC.RWD_MTR_DMND ,
	myDAC.TEST_WRP_OP ,
	myDAC.SPARE_DAC_3);

	
	
    int i=0,j=0;
	char result1[4];
	char result2[2];
    int num1=0,num2=0;
	//char gpio1[15];
	//char gpio2[15];
	memset(&result1, 0, sizeof(char)*4);
	memset(&result2, 0, sizeof(char)*4);
	offset = 0xc0000018;//strtoul(argv[1], NULL, 0);
    len = 8;//strtoul(argv[2], NULL, 0);
	 page_base = (offset / pagesize) * pagesize;
     page_offset = offset - page_base;
	

		sprintf(result1,"%02x%02x%02x%02x", (int)mem[page_offset + i], (int)mem[page_offset + i + 1],(int)mem[page_offset + i + 2], (int)mem[page_offset + i + 3]);
		sprintf(result2,"%02x%02x", (int)mem[page_offset + i + 2], (int)mem[page_offset + i + 3]);
		num1 = (int)mem[page_offset + i + 1];
		num2 = (int)mem[page_offset + i + 3];

        keys.seat_switch = (num1 & 128)>>7;
		keys.seat_occupy = ((num1 & 132)== 132)?1:0;
		keys.kill_switch_fit = (num2 & 16)>>4;
		keys.kill_key_pressed = (num2 & 48)>>5;

		findFault();
	
	printf("\n");
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
	write_dac();
	printf("\033[2J");
   printf("\033[2H");
   sleep(1);
	}
    return 0;

}
void  INThandler(int sig)
{
     char  c;

     signal(sig, SIG_IGN);
     //printf("\nDid you hit Ctrl-C?\n"
       //     "Do you really want to quit? [y/n] ");
    // c = getchar();
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
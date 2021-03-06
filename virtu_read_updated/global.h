#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>

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

#define PI  3.142
#define WHEEL_DIAMETER 0.00031 //its 0.5 Meters in Miles
#define GAIN 1

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
	int left_motor_speed_demand; //nothing but RPM
	int right_motor_speed_demand; //nothing but RPM
	float left_wheel_speed;//mph
	float right_wheel_speed;//mph
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
	int left_motor_drive_gain;
	int right_motor_drive_gain;
};

struct KEY
{
	int seat_switch;
	int seat_occupy;
	int kill_switch_fit;
	int kill_key_pressed;
	int kill_k_pressed;
};

void INThandler(int);
struct ADC myADC ;
struct DAC myDAC;
struct KEY keys;
int fd ;
FILE *fptr;
extern char fileop[25];
unsigned char *mem;
extern float userSpeed;
float gain;
static int faultFlag;
unsigned int userDuration;
unsigned int userDurationExpired;
struct sigevent sig_ev_file, sig_ev_chair, sig_ev_print;
struct itimerspec timer_spec, timer_spec1, timer_spec2;
timer_t timerid, timerid1, timerid2;
sigset_t mask;
struct sigaction action, action1, action2;
int methodOfCalculatingDac;//Will be either Straight Through, Integration, or Derivative
static int output_state_x;//to be used by integration
static int output_state_y;//to be used by integration

static int input_state_x;//to be used by derivative
static int input_state_y;//to be used by derivative

int kix,kiy,kdx,kdy,kpx, kpy;

int controlManagerCount, fileWriteCount, printCount;
float controlManagerAvg, fileWriteAvg, printAvg;
float controlManagerDiff, fileWriteDiff, printDiff;
float ctrlMinorSave, fileWriteMinorSave, printMinorSave;
float ctrlMinorExec, fileWriteMinorExec, printMinorExec;

void write_dac();
int process_args(int argc,char *argv[]);
float rpm2mph(int rpm);
float mph2rpm(float mph);
void speedLimitCheck();
float getMax(float a, float b);
float getMin(float a, float b);
int calcDACEnggUnit(float input);
int calcEnggUnitForSpd(float input, int offset);
int calcEnggUnitForChair(float input);
int calcEnggUnit(float input, int offset);
float calcVolt(int input, float offset);
void controlManager();
void chairControllerLogic();
void printOnScreen();
void findFault();
void fault_sig_handler(int sig, siginfo_t *si, void *uc);
float limit(float input, float upper_limit, float lower_limit);
void file_writer(int sig, siginfo_t *si, void *uc);
#include "global.h"
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

float rpm2mph(int rpm)
{
    return(WHEEL_DIAMETER*PI*rpm*60);
}

float mph2rpm(float mph)
{
	return((mph/(WHEEL_DIAMETER*PI*60)));
}

float getMin(float a, float b)
{
	return (a<b)?a:b;
}

float getMax(float a, float b)
{
	return (a>b)?a:b;
}

void speedLimitCheck()
{
	if(abs((int)myADC.left_wheel_speed) > (int)userSpeed )
	{
	   printf("Performing speed penalty on Left, going beyond permissible range...\n");
       myADC.left_motor_speed_demand = mph2rpm(userSpeed);
	}
	
	if( abs((int)myADC.right_wheel_speed) > (int)userSpeed )
	{
       printf("Performing speed penalty on Right, going beyond permissible range...\n");
       myADC.right_motor_speed_demand = mph2rpm(userSpeed);
	}
}

float limit(float input, float upper_limit, float lower_limit)
{
	if(input > upper_limit) return upper_limit;
	if(input < lower_limit) return lower_limit;
	return input;
}
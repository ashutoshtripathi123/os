#include "global.h"
void controlManager()
{
	//check if button press fault has been logged
	//remember to call write_dac from here, and not from main
	//Speed constraint comes here

    if(keys.seat_switch == 0)
	{
		printf("Don't forget to switch on the seat switch ... \n");
	}

	if(faultFlag == 1)
	{
		myADC.left_motor_speed_demand = 0;
        myADC.right_motor_speed_demand = 0;
		write_dac();
		return;
	}
	//RID 17
	if(keys.seat_switch == 1 && keys.seat_occupy == 0)
	{
		printf("Wheelchair movement not allowed... Seat Unoccupied \n");
		myADC.left_motor_speed_demand = 0;
        myADC.right_motor_speed_demand = 0;
	}
    //RID 18 - if seat changes from occupied to unoccupied
	if(keys.seat_occupy == 0)
	{
		myADC.left_motor_speed_demand = 0;
        myADC.right_motor_speed_demand = 0;	
	}

	//RID 20 - don't move, if kill switch is true
	if(keys.kill_switch_fit == 1 && keys.kill_key_pressed == 1)
	{
		printf("Wheelchair movement not allowed... Kill Switch Pressed \n");
		myADC.left_motor_speed_demand = 0;
        myADC.right_motor_speed_demand = 0;	
	}

	if(keys.kill_k_pressed == 1)
	{
		printf("Wheelchair movement not allowed... K Key Pressed \n");
		myADC.left_motor_speed_demand = 0;
        myADC.right_motor_speed_demand = 0;
	} 
	
	//RID 21 - The controller shall reduce the chair speed by 50% if the seat position is above 5 inches
	if(myADC.Seat_Height_Engg_Unit > 5)
	{
		printf("Reducing Speed by HALF as height beyong 5 Inches \n");
		myADC.left_motor_speed_demand = myADC.left_motor_speed_demand>>1;
        myADC.right_motor_speed_demand = myADC.right_motor_speed_demand>>1;
		myADC.left_wheel_speed = rpm2mph(myADC.left_motor_speed_demand);
        myADC.right_wheel_speed = rpm2mph(myADC.right_motor_speed_demand);
	}
    
	write_dac();
}




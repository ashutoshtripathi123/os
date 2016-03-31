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

void chairControllerLogic()
{
		struct timespec start, end;
		static float old = 0;
        clock_gettime(CLOCK_REALTIME, &start);
		read_adc();		
				
		read_dac();
		
		read_gpio();

		if(keys.kill_k_pressed == 1)
		{
			keys.kill_switch_fit = 0;
		}    

		findFault();
		controlManager();
		if(userDurationExpired == userDuration)
		{
			munmap(mem, 40);
			close(fd);
			exit(1);
		}
		controlManagerCount++;
		clock_gettime(CLOCK_REALTIME, &end);
        controlManagerDiff = ((float)(end.tv_nsec-start.tv_nsec)/1000000);
		ctrlMinorExec = ctrlMinorExec + controlManagerDiff;
		old = old + controlManagerDiff;
		controlManagerAvg = old/controlManagerCount;
		ctrlMinorSave = ctrlMinorSave + (15 - controlManagerDiff);
}


void printOnScreen()
{
	struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
	static float old = 0;
	printf("\033[2J");
	printf("\033[2H");
	//printf("\n============ADC=============\n");
    printf("X=%d :: Y = %d \n", myADC.Joystic_X,  myADC.Joystic_Y);
	printf("Seat Height = %d \n", myADC.Seat_Height_Engg_Unit);
	printf("Demand (RPM):: Left = %d :: Right = %d \n",
		myADC.left_motor_speed_demand, myADC.right_motor_speed_demand);

	//printf("\n============DAC=============\n");
	printf("\nLEFT DMND = %d :: LEFT Volt = %f \n"
	       " RIGHT DMND = %d :: RIGHT Volt = %f \n",
	myDAC.LWD_MTR_DMND, myDAC.LWD_MTR_DMND_Volt, myDAC.RWD_MTR_DMND, myDAC.RWD_MTR_DMND_Volt);
	
    //printf("\n============GPIO=============\n");
	//printf("seat_switch = %d, seat_occupy = %d, kill_switch_fit = %d, kill_key_pressed = %d, K_KEY_pressed = %d \n", 
    //keys.seat_switch, keys.seat_occupy,keys.kill_switch_fit, keys.kill_key_pressed, keys.kill_k_pressed);

	//printf("\n============TIME=============\n");
	printf("Ctrl.......=> Avg \t Count \t Diff(ms) \t Exec(ms) \t Save(ms) \n");
	printf("\t %f \t %d \t %f \t %f \t %f \n\n", controlManagerAvg, controlManagerCount, controlManagerDiff, ctrlMinorExec, ctrlMinorSave);
	printf("File.......=>Avg \t Count \t Diff(ms) \t Exec(ms) \t Save(ms) \n");
	printf("\t %f \t %d \t %f \t %f \t %f \n\n", fileWriteAvg, fileWriteCount, fileWriteDiff, fileWriteMinorExec, fileWriteMinorSave);
	printf("Print......=>Avg \t Count \t Diff(ms) \t Exec(ms) \t Save(ms) \n");
	printf("\t %f \t %d \t %f \t %f \t %f \n\n", printAvg, printCount, printDiff, printMinorExec, printMinorSave);
	printCount++;
	clock_gettime(CLOCK_REALTIME, &end);
    printDiff = ((float)(end.tv_nsec-start.tv_nsec)/1000000);
	printMinorExec = printMinorExec + printDiff;
	old = old + printDiff;
	printAvg=old/printCount;
	printMinorSave = printMinorSave + (100 - printDiff); //saving the time function didnt do anything. Subtracting with 100, because this is running every 100ms
}

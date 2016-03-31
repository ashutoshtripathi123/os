#include <stdio.h>
#include "global.h"
void findFault()
{
    //printf("\n================= MESSAGE ======================\n");
	if(keys.kill_k_pressed == 1)
	{
		printf("Key K has been pressed - kill switch released ... \n");
		faultFlag = 1;
		return;
	}

	//RID 24A
	if((keys.kill_switch_fit == 0) && (keys.kill_key_pressed == 1))
	{
		printf("FAULT DETECTED: Kill Switch Pressed When Switch Not Present !!! \n");
		faultFlag = 1;
		return;
	}
	
	//RID 24B
	if((keys.seat_switch == 0) && (keys.seat_occupy == 1))
	{
		printf("FAULT DETECTED: Seat Occupied When Switch Not Present !!! \n");
		faultFlag = 1;
		return;
	}
}

void fault_sig_handler(int sig, siginfo_t *si, void *uc)
{
	printf("raising fault ...\n");
	faultFlag = 1;
}

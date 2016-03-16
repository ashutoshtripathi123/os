//#include <stdio.h>
#include "global.h"
int process_args(int argc,char *argv[])
{
	int i;
	short error = 0;
	if (argc==1)
	{
		error=1;
		printf("Insufficient argument\n");
		//return error;
	}	
	for( i=1; i<argc; ++i )
	{
		if(!strcmp(argv[i],"-g"))
		{
			/* user specified gain parameter */
			++i;
			if(i<argc)
			{
				gain = atoi(argv[i]);
				//if ((gain<0 )|| (gain >1))
				//{
				//	printf( "ERROR: -g requires a value 0 to 1\n" );
				//	error = 1;
				//}
				
				
			}
			else
			{
				printf( "ERROR: -g requires a value after it\n" );
				error = 1;
			}
		}
		else if(!strcmp(argv[i],"-S"))
		{
			/* user specified Speed parameter */
			++i;
			if(i<argc)
			{
				userSpeed = atoi(argv[i]);
				if ((userSpeed <1 )|| (userSpeed >10))
				{
					printf( "ERROR: -S requires a value 1 to 10 mph\n" );
					error = 1;
				}
				
				
			}
			else
			{
				printf( "ERROR: -S requires a value after it\n" );
				error = 1;
			}
		}
		
		
		else if(!strcmp(argv[i],"-of"))
		{
			/* Filename provided */
			++i;
			if((i<argc)&&(((strcmp(argv[i],"-g"))||(strcmp(argv[i],"-S")))))
			{
				
				strcpy(fileop,argv[i]);
				
				
				
			}
			else
			{
				printf( "ERROR: -of requires a proper filename after it. check the commnad\n" );
				error = 1;
			}
		}
		else if(!strcmp(argv[i],"-d"))
		{
			/* Duration Provided */
			++i;
			if(i<argc)
			{
				userDuration = atoi(argv[i]);
				if ((userDuration < 1 ))
				{
					printf( "Duration cannot be less than 1 sec \n" );
					error = 1;
				}
				//printf("UserDuration = %d \n", userDuration);
			}
			else
			{
				printf( "ERROR: -S requires a value after it\n" );
				error = 1;
			}
		}
		else if(!strcmp(argv[i],"-m"))
		{
			/* Method of Calculating DAC is Provided */
			++i;
			if(i<argc)
			{
				methodOfCalculatingDac = atoi(argv[i]);
				if ((methodOfCalculatingDac < 0 ))
				{
					printf( "Method os calculating DAC cannot be less than 1 \n" );
					error = 1;
				}
			}
			else
			{
				printf( "ERROR: -S requires a value after it\n" );
				error = 1;
			}
			printf("methodOfCalculatingDac = %d \n", methodOfCalculatingDac);
		}
		else if(!strcmp(argv[i],"-h"))
		{
			/* command line args description requested */
			error = 1;
		}
		else
		{
			printf( "ERROR: unknown commandline option - %s\n", argv[i] );
			error = 1;
		}
	}
	if(error)
	{
		/* Print commandline arg help message and return true */
		printf( "Usage: ./virtu_read [options]\n"
		  "Allowed commandline options \n"
		  "\n"
		  "\t-S userSpeed     User specified userSpeed from 1 to 10 mph\n"
		  "\t-g gain      gain from 0 to 1\n"
		  "\t-of fileName output filename in *.csv format.\n"
		  "\t-d Timer value to Run the program. \n"
		  "\t-M Method {0=Straight;1=Integration;2=Derivative} \n"
		  "\t-h           Display this help message\n"
		  "\n" );
	}
	return error;
}

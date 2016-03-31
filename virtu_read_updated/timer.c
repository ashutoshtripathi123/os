#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
void main()
{
	#if 0 //this approach works only in seconds
	time_t start, end;
	double duration = 0;
	time(&start);
	// your codes here
	sleep(2);
	time(&end);
	duration = difftime(end, start);         // time in milliseconds
	printf("Duration = %lf \n", duration);
	#endif
	#if 0
	clock_t begin, end;
	double time_spent;
	begin = clock();
	/* here, do your time-consuming job */
	sleep(2);
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Time_spent = %f \n", time_spent);
	#endif
    
	/********************* Working *******************************/
	#if 0 //works only with int and not float because timeb struct has only longs and shorts
    struct timeb start, end;
    int diff;
    int i = 0;
    ftime(&start);

    while(i++ < 999) {
        /* do something which takes some time */
        printf(".");    
    }
    usleep(10);
    ftime(&end);
    diff = (int) (1000.0 * (end.time - start.time)
        + (end.millitm - start.millitm));

    printf("\nOperation took %u milliseconds\n", diff);
    //return 0;
	#endif
	int i=0;
    struct timespec start, end;
	clock_gettime(CLOCK_REALTIME, &start);
	while(i++ < 999) {
     /* do something which takes some time */
     printf("..");    
    }
	usleep(1);
	clock_gettime(CLOCK_REALTIME, &end);
	printf("diff = %lu sec and %f msec and %lu nsec\n",end.tv_sec-start.tv_sec,((float)(end.tv_nsec - start.tv_nsec)/1000000), end.tv_nsec - start.tv_nsec );

}
#include <stdio.h>
/*as per gamedev algo*/
void main()
{
	float a,b,x;
	float X,A,B;
	printf("a,b - min, max values of ur desired range (float) ...\n");
	printf("A,B - min, max values of original range (float) ...\n");
	printf("X - input value (float) ...\n");
	printf("x - normalized output...\n");
	scanf("%f%f%f%f%f",&a,&b,&A,&B,&X);

	x = (X-A) * (b - a)/(B - A);
	printf("Normalized value = %f ...\n", x);

}






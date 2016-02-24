#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
char *hw_addr = (char *) 0xC0000000;

int main(int argc, char *argv[])
{
    int   i;
    int   devmem;
    off_t PageOffset, PageAddress;

    devmem = open("/dev/mem", O_RDWR | O_SYNC);
    PageOffset = (off_t) hw_addr % getpagesize();
    PageAddress = (off_t) (hw_addr - PageOffset);

    hw_addr = (char *) mmap(0, 4, PROT_READ|PROT_WRITE, MAP_SHARED, devmem, PageAddress);
    printf("ADC 0 = %x%x \n", *hw_addr,*(hw_addr+1));
	printf("ADC 1 = %x%x \n", *(hw_addr+2),*(hw_addr+3));
	printf("ADC 2 = %x%x \n", *(hw_addr+4),*(hw_addr+5));
	printf("ADC 3 = %x%x \n", *(hw_addr+6),*(hw_addr+7));
	printf("ADC 4 = %x%x \n", *(hw_addr+8),*(hw_addr+9));
	printf("ADC 5 = %x%x \n", *(hw_addr+10),*(hw_addr+11));
	printf("ADC 6 = %x%x \n", *(hw_addr+12),*(hw_addr+13));
	printf("ADC 7 = %x%x \n", *(hw_addr+14),*(hw_addr+15));

	printf("GPIO 0 = %2x%2x%2x%2x \n", *(hw_addr+24),*(hw_addr+25),*(hw_addr+26),*(hw_addr+27));
	printf("GPIO 0 = %2x%2x%2x%2x \n", *(hw_addr+28),*(hw_addr+29),*(hw_addr+30),*(hw_addr+31));
	
	*(hw_addr+32) = 0x11;
	*(hw_addr+34) = 0x12;
    *(hw_addr+36) = 0x13;
	*(hw_addr+38) = 0x14;

	
}

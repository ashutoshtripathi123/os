#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Usage: %s <phys_addr> <Length>\n", argv[0]);
        return 0;
    }

    off_t offset = strtoul(argv[1], NULL, 0);
    size_t len = strtoul(argv[2], NULL, 0);

    // Truncate offset to a multiple of the page size, or mmap will fail.
    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    off_t page_base = (offset / pagesize) * pagesize;
    off_t page_offset = offset - page_base;

    int fd = open("/dev/mem", O_RDWR |  O_SYNC);
    unsigned char *mem = mmap(NULL, page_offset + len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, page_base);

    if (mem == MAP_FAILED) {
        perror("Can't map memory");
        return -1;
    }

    size_t i;
	char result[4];
    for (i = 0; i < len; i=i+2)
	{
        printf("%x%x \n", (int)mem[page_offset + i], (int)mem[page_offset + i + 1]);
		//printf("%d", mem[i]);
		sprintf(result,"%x%x",(int)mem[page_offset + i],(int)mem[page_offset + i + 1]);
		printf("%ld\n",strtol(result,NULL,16));
	}
	printf("\n");
    return 0;

}
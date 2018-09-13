#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include<string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>


int main(int argc, char *argv[]){
    int fd = open(argv[1], O_RDWR);
    struct stat buffer;
    int status = fstat(fd, &buffer);
    
    //tamplate:   pa=mmap(addr, len, prot, flags, fildes, off);
    //c will implicitly cast void* to char*, while c++ does NOT
    char* address=mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    int blocksize=0;
    memcpy(&blocksize,address+8,2);
    printf("Super block information:\n");
    blocksize=htons(blocksize);
	int blockCount = 0;
	memcpy(&blockCount, address+10, 4);
	blockCount=htonl(blockCount);
    printf("Block size: %d\n",blocksize);
    printf("Block count: %d\n",blockCount);
	int fatStart = 0;
	memcpy(&fatStart, address+14, 4);
	fatStart=htonl(fatStart);
	printf("FAT starts: %d\n",fatStart);
	int fatBlocks = 0;
	memcpy(&fatBlocks, address+18, 4);
	fatBlocks=htonl(fatBlocks);
	printf("FAT blocks: %d\n",fatBlocks);
	int rootStart = 0;
	memcpy(&rootStart, address+22, 4);
	rootStart=htonl(rootStart);
	printf("Root directory start: %d\n",rootStart);
	int rootBlocks = 0;
	memcpy(&rootBlocks, address+26, 4);
	rootBlocks=htonl(rootBlocks);
	printf("Root directory blocks: %d\n",rootBlocks);
	printf("\n\nFAT information:\n");
	int freecount = 0;
	int reservecount=0;
	int allocatedCount=0;
	int end=0;
	address = address+2*512;
	do{
          int temp = 0;
		 memcpy(&temp, address, 4);
		 switch(htonl(temp)){
		 case 0:
			 freecount++;
			 address+=4;
			 end+=1;
                 continue;
			break;
		 case 1:
			 reservecount++;
			 address+=4;
			 end+=1;
                 continue;
			break;
		 default:
                 if((htonl(temp)>=0x000000002) && htonl(temp) <= 0xFFFFFFFF){
                     allocatedCount++;
                     end+=1;
                 }
               	address+=4;
			break;
		 }
	 }while(end<blockCount);
	printf("Free blocks: %d\n",freecount);
    printf("Reserved blocks: %d\n",reservecount);
	 printf("Allocated blocks: %d\n",allocatedCount);
    
    munmap(address,buffer.st_size);
    close(fd);


	
    
}

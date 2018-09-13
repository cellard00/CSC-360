#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include<string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>


int main(int argc, char *argv[]){
	
	char * lookFile = argv[3];
	 char directory[30];
    int fd = open(argv[1], O_RDWR);
    struct stat buffer;
    int status = fstat(fd, &buffer);
    
    //tamplate:   pa=mmap(addr, len, prot, flags, fildes, off);
    //c will implicitly cast void* to char*, while c++ does NOT
    char* address=mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  	char* add = address;
	char* fatStart = address+2*512;
	char* new = fatStart;
	address= address + 512 * 53;
	int temp = 0;
	int startBlock = 0;
	int numBlocks = 0;
	int nextBlock =0;
	int fileFound =0;
	int size = 0;
  do{
	   char fileType;
	 
	  memcpy(&temp, address,1);
	  
	  if(address[0] == 0x03){
		  fileType ='F';
	  }
	  else
		  fileType = 'D';
	  memcpy(&size, address+9, 4);
	  size = htonl(size);
	  temp = htonl(temp);
	  int year = 0;
	  memcpy(&year, address+20, 2);
	  year = htons(year);
	  int month = 0;
	  month = address[22];
	    int day = 0;
	  day= address[23];
	  
	  int hour = 0;
	  hour = address[24];
	  int min = 0;
	  min = address[25];
	  int sec = 0;
	  sec = address[26];
	  char filename[30];
	  memcpy(filename, address+27, 31);
	  if(strcmp(lookFile, filename) == 0){
	  	fileFound = 1;
	  	memcpy(&startBlock, address+1, 4);
	  	startBlock = htonl(startBlock);
	  	numBlocks = htonl(numBlocks);
	  	break;
	  	}
	  address+=64;
	  
	  
  }while(htonl(temp)!=0x00);
  if(fileFound == 1){
	FILE *out; 
	out = fopen(lookFile,"w");
  	nextBlock=startBlock;
  	int count=0;
  	address = add+512*nextBlock;
	new = fatStart+nextBlock*4;
  	int outputAdd = 0;
	char output[512];
  	while(count <= size-512){
		int i;
		 memcpy(output, address, 512);
  	 	fwrite(output, sizeof(char), 512, out);
		int nextBlock = 0;
		memcpy(&nextBlock, new, 4);
		nextBlock = htonl(nextBlock);
		new = fatStart+4*nextBlock;
		count+=512;
		address = add+512*nextBlock;
	}
	 memcpy(output, address, size-count);
	 fwrite(output, sizeof(char), size-count, out);
	 fclose(out);
	 
	 }
	 else{
	 printf("File not found\n");
	 exit(1);
	 }
    munmap(address,buffer.st_size);
    close(fd);
    
    
    
    
}

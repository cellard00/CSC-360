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
	
	 char directory[30];
    int fd = open(argv[1], O_RDWR);
    struct stat buffer;
    int status = fstat(fd, &buffer);
    
    //tamplate:   pa=mmap(addr, len, prot, flags, fildes, off);
    //c will implicitly cast void* to char*, while c++ does NOT
    char* address=mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
   address= address + 512 * 53;
   int temp = 0;
  do{
	   char fileType;
	 
	  memcpy(&temp, address,1);
	  
	  if(address[0] == 0x03){
		  fileType ='F';
	  }
	  else
		  fileType = 'D';
	  int size = 0;
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
	  if(size>0){
	  	printf("%c %10d %30s %04d/%02d/%02d %02d:%02d:%02d\n", fileType, size,filename, year,month,day, hour,min,sec);
	  	}
	  address+=64;
	 
	  
	  
  }while(htonl(temp)!=0x00);
    munmap(address,buffer.st_size);
    close(fd);
    
    
    
    
}

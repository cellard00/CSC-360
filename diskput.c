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
	
	char * lookFile = "No_file";
	int fp = open(argv[2], O_RDWR);
	if(fp == -1){
		printf("Input file does not exist\n");
		exit(-1);
	}
		
	struct stat fileStats;
	 fstat(fp, &fileStats);
	 char directory[30];
	 char* filesizestring;
    int fd = open(argv[1], O_RDWR);
    struct stat buffer;
    int status = fstat(fd, &buffer);
     int inSize = fileStats.st_size;
    //tamplate:   pa=mmap(addr, len, prot, flags, fildes, off);
    //c will implicitly cast void* to char*, while c++ does NOT
    char* address=mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  	char* add = address;
	char* new = address;
	char* temps = address;
	char* fatStart = address+2*512;
	char* next = fatStart;
   address= address + 512 * 53;
   int temp = 0;
   int startBlock = 0;
   int numBlocks = 0;
   int nextBlock =0;
   int fileFound =0;
   int size = 0;
  	int blockNext= 0;
   int upperfirstbyte = (inSize & 0xFF000000) >> 24; 
   int lowerfirstbyte = (inSize & 0x00FF0000) >> 16; 
	int uppersecondbyte = (inSize & 0x0000FF00) >> 8; 
	int lowersecondbyte = (inSize & 0x000000FF); 
	time_t t = time(NULL);
	struct tm *tmp = localtime(&t);
	int inyear = tmp->tm_year+1900;
	int inYearUpper=(inyear & 0xFF00) >> 8;
	int inYearLower=(inyear & 0x00FF);
	int inmonth = tmp->tm_mon+1;
	int inday = tmp->tm_mday;
	int inhours = tmp->tm_hour;
	int inmins = tmp->tm_min;
	int insecs = tmp->tm_sec;
	char inName[30];
	strcpy(inName, argv[2]);
  do{
	   memcpy(&temp, address,1);
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
	  if(strcmp(lookFile, filename) == 0){
	  	fileFound = 1;
		address[0] = 03;
		new = address+27;
		memcpy(new, inName, 31);
		address[9]=upperfirstbyte;
		address[10]=lowerfirstbyte;
		address[11]=uppersecondbyte;
		address[12]=lowersecondbyte;
		address[20] = inYearUpper;
		address[21] = inYearLower;
		address[22] = inmonth;
		address[23] = inday;
		address[24] = inhours;
		address[25] = inmins;
		address[26] = insecs;
	  	
	  	memcpy(&numBlocks, address +5, 4);
	  	numBlocks = htonl(numBlocks);
		 blockNext= 0;
		 while(1){
			 if(((next[0] == 0x00) && (next[1] == 0x00)) && ((next[2] == 0x00) && (next[3] == 0x00)))
				break;
			else
			blockNext++;
			next = next+4;
		}
		int startBlock = blockNext;
		blockNext= 0;
		next[0] = (blockNext & 0xFF000000) >> 24; 
		next[1] = (blockNext & 0x00FF0000) >> 16; 
		next[2] = (blockNext & 0x0000FF00) >> 8; 
		next[3] = (blockNext & 0x000000FF); 
		temps = fatStart;
		 while(1){
			 if(((temps[0] == 0x00) && (temps[1] == 0x00)) && ((temps[2] == 0x00) && (temps[3] == 0x00)))
				break;
			else
			blockNext++;
			temps = temps+4;
		}
		next[0] = (blockNext & 0xFF000000) >> 24; 
		next[1] = (blockNext & 0x00FF0000) >> 16; 
		next[2] = (blockNext & 0x0000FF00) >> 8; 
		next[3] = (blockNext & 0x000000FF); 
		address[1] = (startBlock & 0xFF000000) >> 24; 
		address[2] = (startBlock & 0x00FF0000) >> 16; 
		address[3] = (startBlock & 0x0000FF00) >> 8; 
		address[4] = (startBlock & 0x000000FF); 
		address = add + 512*startBlock;
	  	break;
	}
	  	
	  address+=64;
	  
	  
	}while(htonl(temp)!=0x00);
  
   if(fileFound == 1){
		FILE *in; 
		in = fopen(argv[2],"r");
		int numToWrite = inSize;
		int count=0;
		char output[512];
		
		if(inSize < 512){
			temps = next;
			fread(output, sizeof(char), inSize, in);
			memcpy(address, output, inSize);
			while(1){
				if(((temps[0] == 0x00) && (temps[1] == 0x00)) && ((temps[2] == 0x00) && (temps[3] == 0x00)))
					break;
				else
					blockNext++;
					temps = temps+4;
			}
			next[0] = (blockNext & 0xFF000000) >> 24; 
			next[1] = (blockNext & 0x00FF0000) >> 16; 
			next[2] = (blockNext & 0x0000FF00) >> 8; 
			next[3] = (blockNext & 0x000000FF); 
			temps[0]=0xFF;
			temps[1]=0xFF;
			temps[2]=0xFF;
			temps[3]=0xFF;
			temps[4]=0xFF;
			temps[5]=0xFF;
			temps[5]=0xFF;
			
		}
		else{
			fread(output, 1, 512, in);
			memcpy(address, output, sizeof(output));
			numToWrite-=512;
			blockNext = 0;
			next =temps;
			temps = fatStart;
			while(1){
				if(((temps[0] == 0x00) && (temps[1] == 0x00)) && ((temps[2] == 0x00) && (temps[3] == 0x00)))
					break;
				else
					blockNext++;
					temps = temps+4;
			}
			next[0] = (blockNext & 0xFF000000) >> 24; 
			next[1] = (blockNext & 0x00FF0000) >> 16; 
			next[2] = (blockNext & 0x0000FF00) >> 8; 
			next[3] = (blockNext & 0x000000FF); 
			next = temps;
			address= add + 512*blockNext;
		while(numToWrite > 512){
			fread(output, sizeof(char), 512/sizeof(char), in);
			int blockNext= 0;
			blockNext =0;
			temps = fatStart;
			while(1){
				if(((temps[0] == 0x00) && (temps[1] == 0x00)) && ((temps[2] == 0x00) && (temps[3] == 0x00)))
					break;
				else
		
					blockNext++;
					temps = temps+4;
			}
			next[0] = (blockNext & 0xFF000000) >> 24; 
			next[1] = (blockNext & 0x00FF0000) >> 16; 
			next[2] = (blockNext & 0x0000FF00) >> 8; 
			next[3] = (blockNext & 0x000000FF); 
			
			int startBlock = blockNext;
			memcpy(address, output, 512);
			address = add + 512*blockNext;
			next = temps;
			numToWrite-=512;
		}

		fread(output, sizeof(char), numToWrite, in);
		int blockNext= 0;
		temps = fatStart;
		blockNext =0;
		 while(1){
			 if(((temps[0] == 0x00) && (temps[1] == 0x00)) && ((temps[2] == 0x00) && (temps[3] == 0x00)))
				break;
			else
			blockNext++;
			temps = temps+4;
		}
		next[0] = (blockNext & 0xFF000000) >> 24; 
		next[1] = (blockNext & 0x00FF0000) >> 16; 
		next[2] = (blockNext & 0x0000FF00) >> 8; 
		next[3] = (blockNext & 0x000000FF);
		address = add + 512*blockNext;
		 memcpy(address, output, numToWrite);
		 blockNext= 0;
		 next = temps;
		temps = next;
		blockNext = 0;
		 while(1){
			 if(((temps[0] == 0x00) && (temps[1] == 0x00)) && ((temps[2] == 0x00) && (temps[3] == 0x00)))
				break;
			else
			blockNext++;
			temps = temps+4;
		}
		next[0] = (blockNext & 0xFF000000) >> 24; 
		next[1] = (blockNext & 0x00FF0000) >> 16; 
		next[2] = (blockNext & 0x0000FF00) >> 8; 
		next[3] = (blockNext & 0x000000FF); 
		address = add + 512*blockNext;
		int FF = 255;
		temps[0]=0xFF;
		temps[1]=0xFF;
		temps[2]=0xFF;
		temps[3]=0xFF;
		temps[4]=0xFF;
		temps[5]=0xFF;
		temps[5]=0xFF;
		fclose(in);
	 }
   }
	 else{
	 printf("File not found");
	 exit(1);
	 }
	 
    munmap(address,buffer.st_size);
    close(fd);
	close(fp); 
    
}

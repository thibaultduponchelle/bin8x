#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

int main(int,char**);
unsigned char LL(unsigned short int);
unsigned char HH(unsigned short int);

int main(int argc,char** argv){

	FILE *infile, *outfile;
   unsigned short int i, filesize;
   char buffer;
   unsigned char programData[24000];
   unsigned short int checksum;
   char * temp;

   /*these vars are for writing to the file*/
                char headerStart [8] = "**TI83**"; //this means that it is an 8xp file
		unsigned char h1 = 0x1A;
		unsigned char h2 = 0x0A;
      unsigned char h3 = 0x00;                                              
		unsigned char comment[42] = "Comment: assembly programm................";
		unsigned char fileLenLL = 0x00; //  program length + 0x13
      unsigned char fileLenHH = 0x00; // /
      unsigned char varHeadLL = 0x0B;
      unsigned char varHeadHH = 0x00;
      unsigned char dataLenLL = 0x00;
      unsigned char dataLenHH = 0x00;
		unsigned char progtype = 0x06; // this means that the program will be protected, 0x05 means unprotected
		char name [9] = "\0\0\0\0\0\0\0\0\0";
		//DatalenLL
      //DatalenHH
      unsigned char programLenLL = 0x00;
      unsigned char programLenHH = 0x00;

	/**/

	if(argc < 3){
      printf("Bin83 v1.0 TI-83 squisher for Linux\n by Peter Martijn Kuipers (central@hyperfield.com)\nUsage bin83 inputfile outputfile name\n\n");
      printf("inputfile    : Tasm BINARY object file)\n");
		printf("outputfile   : The name of the destination .83p file\n");
		printf("name         : The internal (TI-83) name of the program, only the first 8 cha-\n");
      printf("               racters will be used. If none specified, bin83 will give an error\n\n");
      printf("Publised under the Gnu Public License\n");
		return(1);
		}

   printf("Bin83 Ti-83 squisher for Linux, by Peter Martijn Kuipers\n");
   printf("Publised under the Gnu Public License\n");

   if(argc >= 4){       // this is a custom strcpy routine, to make sure the first 8 bytes are copied, and the rest
   							// remains \0
   	for(i=0;i<8;i++){
      	if(argv[3][i] != '\0'){
         	name[i] = argv[3][i];
            }
         else{
         	break;
         	}
	      }

		}
   else{						// if no name is specified, complain
     printf("Error: Please specify a name as the third parameter\n");
     return(2);
   }

   if(( infile = fopen(argv[1], "r") ) == NULL){   // opening inputfile
      printf("Error opening inputfile\n");         // if the file is corrupt, quit
      return(2);
		}

   if(( outfile = fopen(argv[2], "w") ) == NULL){  // opening outputfile
      printf("Error opening outputfile\n");        // if the file is corrupt, quit
      return(3);
		}

   filesize = 0;

   while(!feof(infile)){ //((buffer = fgetc(infile)) != EOF){
     buffer = fgetc(infile);
     programData[filesize] = buffer;  //  put this byte in the data array, and increase the count
     filesize++;                      // /
      }
   filesize--;

   fileLenHH = HH(filesize + 0x11);			//  the file length = the size of the data array + 17 (0x11)
   fileLenLL = LL(filesize + 0x11);       // /

   dataLenHH = HH(filesize +2); 				//  the length of the data includes the 2 checksum bytes
   dataLenLL = LL(filesize +2);				// /

   programLenHH = HH(filesize);				//  the program length is just the filesize
   programLenLL = LL(filesize);				// /


	checksum = 0x00;								// begin with the checksum set on zero

   for(i=0;i<8;i++){								//   write the **TI83** part
		fputc(headerStart[i],outfile);		//  /
   	}                                   // /

	fputc(h1,outfile);							// write the constants defined above
	fputc(h2,outfile);
	fputc(h3,outfile);

   for(i=0;i<42;i++){							// write th the comment
   	fputc(comment[i],outfile);
   	}

	fputc(fileLenLL,outfile);              // file length
	fputc(fileLenHH,outfile);

	fputc(varHeadLL,outfile);              // length of variable header
	checksum += varHeadLL; 						// from this point onward we will be checksumming all that we write
	fputc(varHeadHH,outfile);
	checksum += varHeadHH;

	fputc(dataLenLL,outfile);					// length of data
	checksum += dataLenLL;
	fputc(dataLenHH,outfile);
	checksum += dataLenHH;

	fputc(progtype,outfile);					// type of program (normal 0x05, or protected 0x06)
	checksum += progtype;

   for(i=0;i<8;i++){         					// write the name of the variable
   	fputc(name[i],outfile);
		checksum += name[i];
   	}

   	fputc(dataLenLL,outfile);					// again, write the length of data
	checksum += dataLenLL;
	fputc(dataLenHH,outfile);
	checksum += dataLenHH;

	fputc(programLenLL,outfile);           // length of program
	checksum += programLenLL;
	fputc(programLenHH,outfile);
	checksum += programLenHH;


   for(i=0;i<filesize;i++){					// now write the data array to the file
   	fputc(programData[i],outfile);
      checksum += programData[i];			// and add it to the checksum
      }


 	fputc(LL(checksum),outfile);				// write  the checksum to the file
 	fputc(HH(checksum),outfile);


   // we're done, close all
   fclose(infile);
	fclose(outfile);

   return(0); // no problems
 	}



// I use the following functions to ensure that the words (16 bit) are stored in LLHH order,
// (0xAAFF is stored as 0xFF 0xAA), since I don't know on what platforms this will be compiled


// function to return the lower byte of the word (word consists of two bytes)

unsigned char LL(unsigned short int toSplit){ 
	unsigned char LL;
	LL = (toSplit & 0x00FF);

   return(LL);
   }

// function to return the upper byte of the word (word consists of two bytes)

unsigned char HH(unsigned short int toSplit){
	unsigned char HH;
   toSplit >>= 8;
   HH = (toSplit & 0x00FF);

   return(HH);
   }









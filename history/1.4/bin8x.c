 /* Bin8x v1.4 for Linux,*BSD,...
  * Copyright (C) 2001 Peter Martijn Kuipers <central@hyperfield.com>
  * Copyright (C) 2003 Tijl Coosemans <tijl@ulyssis.org>
  * Copyright (C) 2004 Guillaume Hoffmann <guillaume.h@ifrance.com>
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  * 
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU Library General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  * 
  * Thanks to: Solignac Julien for optimizing the code and making it look more 
  * like devpac.  
  *
  *
  * Compiling the source for your unix:
  * with gcc use: gcc bin8x.c -o bin8x
  */
  
  
/* TI-83 program format :
 Byte(s) (in decimal)             What It Contains
  ------------------------------------------------------
  1-11                      "**TI83**"[26][10][0]
  12-53                     Comment
  54-55                     file length - 57  =  Size of all data in the .8?? file,
                            from byte 55 to last byte before the checksum
  56-57                     [11][0]
  58-59                     Length of data (word)
  60                        program type : 5 (6 for protected)
  61-68                     program name (0-filled) 
  69-70                     Length of data
  71-72                     Length of program
  73-(n-2)                  Actual Program
  (n-1)-n 	            Checksum
 
 * All the length are one word, with Least Significant Byte first
 * Length of program = the length of all the program data (incredible !)
 * Length of data = length of program + 2 (cause it is length of the datablock,
   which contains program length(2 bytes) + program data)
 * Word at 53-54 = length of program + 17
 * The checksum is one word, the sum of all the bytes from byte 55 to
   the last byte before the checksum, modulo 2^16-1 to fit in one word
*/
  
 /* TI-82 program format :
 Byte(s) (in decimal)             What It Contains
  ------------------------------------------------------
  1-11                             **TI82**[26][10][0]
  12-53                            Comment
  54-55                            (69-70)+15 or (71-72)+17
  56-57                            [11][0]
  58-59                            Repeats 69-70
  60                               [5] unprotected
                                   [6] protected
  61-68                            Name of program
  69-70                            (71-72)+2
  71-72                            Length of file to end minus Checksum
  73-(n-2)                         Actual program
  (n-1)-(n)                        Checksum
*/
  
  
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#define true 1
#define false 0

#define indef 1
#define outdef 2
#define namedef 4


unsigned char LL(unsigned short int);
unsigned char HH(unsigned short int);
void complain(void);

int main (int argc, char *argv[])
{
  
  struct utsname system;
  FILE *infile, *outfile;
  unsigned short int i,n, filesize, destcalc = 0;
  short int uppercase,executable = false;
  unsigned char buffer, optiondefs = 0;
  unsigned char programData[28000];
  unsigned short int checksum;
  char fileio[12] = "--------.---";
  char fileXin[255] = "";
  char fileXout[255] = "";
  const char inext[] = ".bin";
  const char ext83[] = ".83p";
  const char ext8x[] = ".8xp";
  const char ext82[] = ".82p";
  
  

  /* --- Header File Values ---*/
  unsigned char header[11] = {'*', '*', 'T', 'I', '8', '3', '*', '*', 0x1A, 0x0A, 0x00};
  unsigned char comment[42] = "File created under ";
  unsigned char fileLenLL = 0x00;
  unsigned char fileLenHH = 0x00;
  unsigned char varHeadLL = 0x00;
  unsigned char varHeadHH = 0x00;
  unsigned char dataLenLL = 0x00;
  unsigned char dataLenHH = 0x00;
  unsigned char progtype = 0x06;
  char name[9] = "\0\0\0\0\0\0\0\0\0";
  unsigned char programLenLL = 0x00;
  unsigned char programLenHH = 0x00;
  if (uname(&system) == -1)
    strcat(comment, "unknown system");
  else
    strncat(comment, system.sysname, 20);
  /* --- Header File Values ---*/

  /* Here comes the option parsing routine */

  for(i=0;i<argc;i++){

    if(!strcmp(argv[i],"-o") || !strcmp(argv[i],"--output")){
      strcpy(fileXout,argv[++i]);
      optiondefs ^= outdef;
    }
    else if(!strcmp(argv[i],"-i") || !strcmp(argv[i],"--input")){
      strcpy(fileXin,argv[++i]);
      optiondefs ^= indef;
    }
    else if(!strcmp(argv[i],"-n") || !strcmp(argv[i],"--calcname")){
      i++;
      
      for(n=0;n<8;n++){                /* custom strcpy routine, used */
	if(argv[i][n] != '\0'){        /* to make copy only the first */
	  name[n] = argv[i][n];        /* 8 bytes at maximum          */
	}
	else{
	  break;
	}
      }
      
      optiondefs ^= namedef;
      
    }
    else if(!strcmp(argv[i],"-83") || !strcmp(argv[i],"--ti83")){
      if(destcalc == 0 || destcalc == 83){
	destcalc = 83;
      }
      else{
	puts("You can't specity different calculators");
	return(1);
      }
    }
    else if(!strcmp(argv[i],"-83p") || !strcmp(argv[i],"--ti83plus")){
      if(destcalc == 0 || destcalc == 84){ /* I use 84 because an int can't 
					    * have "83+" and 
					    * no TI-84 exists anyway... 
					    */
	destcalc = 84;
      }
      else{
	puts("You can't specity different calculators");
	return(1);
      }
    }
    else if(!strcmp(argv[i],"-82") || !strcmp(argv[i],"--ti82")){
      if(destcalc == 0 || destcalc == 82){
	destcalc = 82;
      }
      else{
	puts("You can't specity different calculators");
	return(1);
      }
    }   
      
    else if(!strcmp(argv[i],"-u") || !strcmp(argv[i],"--uppercase")){
      uppercase = true;
    }
    else if(!strcmp(argv[i],"-x") || !strcmp(argv[i],"--executable")){
      executable = true;
    }
    else if(!strcmp(argv[i],"-e") || !strcmp(argv[i],"--unprotected")){
      progtype = 0x05;
    }
    else if(!strcmp(argv[i],"-?") || !strcmp(argv[i],"--help")){
      complain();
      return(0);
    }
    
  }

  if(optiondefs & indef){
    if(!(optiondefs & outdef)){
      puts("Please use -n and -o in conjunction with -i!!");
      return(2);
    }
    else if(!(optiondefs & namedef)){
      puts("Please use -n and -o in conjunction with -i!!");
      return(2);
    }
  }

  if (argc < 2) {
    complain();
    return(1);
  }

  if (strlen(argv[1])>8 && !(optiondefs & indef)) {
    puts("Please use short filenames (8.3) or -i!");
    return(2);
  }

  /* End of the option parsing routine */


  puts("Bin8x v1.3 Ti-82/83/83+ squisher");
  puts("Copyright (C) 2001 Peter Martijn Kuipers <central@hyperfield.com>");
  puts("Copyright (C) 2003 Tijl Coosemans <tijl@ulyssis.org>\n");

  if(!(optiondefs & namedef)){
    strcpy(name, argv[1]); 
  }

  /* Optional if you want uppercase program names */
  if(uppercase == true){ 
    for(i=0;i<strlen(name);i++){
      if(islower(name[i])){
	name[i] = toupper(name[i]);
      }
    }
  }
  
  /* for ti-82 CrASH, put an inverted lowercase 'a' as first character */
  
  if(destcalc == 82 ){
    for(i=7;i;i--){
      name[i]=name[i-1];
    }
    name[0] = 220;
  }
  
  if(optiondefs & indef){
    if (!(infile = fopen(fileXin, "r"))) {
      puts("Error opening inputfile!");
      printf("File: %s\n",fileXin);
      return(2);
    }
    printf("Using inputfile        : %s\n",fileXin);
  }
  else{
    
    strcpy(fileio, argv[1]);
    strcat(fileio, inext);
    
    if (!(infile = fopen(fileio, "r"))) {
      puts("Error opening inputfile!");
      printf("File: %s\n",fileio);
      return(2);
    }
    printf("Using inputfile        : %s\n",fileio);
  }

  strcpy(fileio, argv[1]);
  
  if(destcalc == 84){
    header[6] = 'F';
    strcat(fileio, ext8x);
  }
  else if(destcalc == 83){
    strcat(fileio, ext83);
  }
  else if(destcalc == 82){
    header[5] = '2';
    strcat(fileio, ext82);
  }

  if(optiondefs & outdef){
    if (!(outfile = fopen(fileXout, "w"))) {
      puts("Error opening outputfile!");
      return(3);
    }
    printf("Using outputfile       : %s\n",fileXout);
  }
  else{
    if (!(outfile = fopen(fileio, "w"))) {
      puts("Error opening outputfile!");
      return(3);
    }
    printf("Using outputfile       : %s\n",fileio);
  }

  printf("Filename on calculator : %s\n",name);

  filesize = -1;

  if(executable && destcalc == 84){
    filesize++;
    programData[filesize] = 0xBB;
    filesize++;
    programData[filesize] = 0x6D;
  }
  
  // UNTESTED : (works only for CrASH(19006))
  if (destcalc == 82){
    filesize++;
    programData[filesize] = 0xD5;
    filesize++;
    programData[filesize] = 0x00;
    filesize++;
    programData[filesize] = 0x11;  
  }


  while (!feof(infile)) {
    filesize++;
    buffer = fgetc(infile);
    programData[filesize] = buffer; 	/* put this byte in the data array, 
					 * and increase the count */
  }

  if(destcalc == 83){
    printf("Size on calculator     : %u bytes\n",filesize+strlen(name)+6);
    fileLenHH = HH(filesize + 0x11);	/* the file length = the size of 
					 * the data array + 17 (0x11) */
    fileLenLL = LL(filesize + 0x11);
    varHeadLL = 0x0B;
  }
  else if(destcalc == 84){
    printf("Size on calculator     : %u bytes\n",filesize+strlen(name)+8);
    fileLenHH = HH(filesize + 0x13);	/* the file length = the size of 
					 * the data array + 19 (0x13) */
    fileLenLL = LL(filesize + 0x13);
    varHeadLL = 0x0D;
  }
  else if(destcalc == 82){   // I'M ABSOLUTELY NOT SURE THAT IT WORKS
      printf("Size on calculator     : %u bytes\n",filesize+strlen(name)+6);
    fileLenHH = HH(filesize + 0x11);	/* the file length = the size of 
					 * the data array + 17 (0x11) */
    fileLenLL = LL(filesize + 0x11);
    varHeadLL = 0x0B;
  }
    
  dataLenHH = HH(filesize + 2); 	/* the length of the data includes 
					 * the 2 checksum bytes */
  dataLenLL = LL(filesize + 2);

  programLenHH = HH(filesize);
  programLenLL = LL(filesize);

  checksum = 0x00;			/* begin with the checksum set
					 *  on zero */

  for(i=0; i<11; i++)			/* write the static header */
    fputc(header[i], outfile);	

  for(i=0; i<42; i++)
    fputc(comment[i], outfile);

  fputc(fileLenLL, outfile);		/* file length */
  fputc(fileLenHH, outfile);

  fputc(varHeadLL, outfile);		/* length of variable header */
  checksum += varHeadLL;		/* checksum calculs start here */
  fputc(varHeadHH, outfile);
  checksum += varHeadHH;

  fputc(dataLenLL, outfile);		/* length of data */
  checksum += dataLenLL;
  fputc(dataLenHH, outfile);
  checksum += dataLenHH;

  fputc(progtype, outfile);		/* protected program */
  checksum += progtype;

  

  for(i=0; i<8; i++) {         		/* write the name of the variable */
    fputc(name[i], outfile);
    checksum += name[i];
  }

  
  if(destcalc == 84){
    puts("\nTI-83 Plus file made!!");

    fputc(0x01,outfile);
    checksum += 0x01;
    fputc(0x00,outfile);
    /*checksum += 0x00;*/
  }    
  else if(destcalc == 83){
    puts("\nPlain TI-83 file made!!");
  }
  else if(destcalc == 82){
    puts("\nTI-82 file made!!");
  }
  
  fputc(dataLenLL, outfile);
  checksum += dataLenLL;
  fputc(dataLenHH, outfile);
  checksum += dataLenHH;

  fputc(programLenLL, outfile);		/* length of program */
  checksum += programLenLL;
  fputc(programLenHH, outfile);
  checksum += programLenHH;


  for(i=0; i<filesize; i++) {		/* now write the data array 
					 * to the file */
    fputc(programData[i], outfile);
    checksum += programData[i];		/* and add it to the checksum */
  }


  fputc(LL(checksum), outfile);		/* write  the checksum to the file */
  fputc(HH(checksum), outfile);


  /* Close all Handles */
  fclose(infile);
  fclose(outfile);

  return(0);
}


/*
 * I use the following functions to ensure that the words 
 * (16 bit) are stored in LLHH order,
 * since we don't know what type of Endian the target platform uses
 */

unsigned char LL(unsigned short int toSplit)
{ 
  unsigned char LL;
  LL = (toSplit & 0x00FF);
  return(LL);
}

unsigned char HH(unsigned short int toSplit)
{
  unsigned char HH;
  toSplit >>= 8;
  HH = (toSplit & 0x00FF);
  return(HH);
}

void complain(){

  printf("Bin8x v1.4 Ti-82/83/83+ squisher\n");
  printf("Copyright (C) 2001 Peter Martijn Kuipers <central@hyperfield.com>\n");
  printf("Copyright (C) 2003 Tijl Coosemans <tijl@ulyssis.org>\n");
  printf("Copyright (C) 2004 Guillaume Hoffmann <guillaume.h@ifrance.com>\n\n");
  printf("\033[01mSyntax:\n");
  printf("\033[01mbin8x\033[0m binfile [options]\n");
  printf("\033[01mbin8x\033[0m -i infile -o outfile -n name [options]\n\n");
  printf("binfile  Binary output from your assembler, the .bin extension is assumed when\n");
  printf("         not using the -i option. You must not give it.\n\n");
  printf("\033[01mOptions:\033[0m\n");
  printf("-o   or --output       specific outputfile ([binfile].8?p if none given.\n");
  printf("-n   or --calcname     specific filename on the calculator ([binfile] if\n");
  printf("                       none given.\n");
  printf("-i   or --input        specific inputfile, you must use -o and -n if you\n");
  printf("                       use it. Also give the complete filename with extension.\n");
  printf("-82  or --ti82         specify ti-82 file (CrASH(19006)).\n");
  printf("-83  or --ti83         specify ti-83 file.\n");
  printf("-83p or --ti83plus     specify ti-83 plus file.\n");
  printf("-u   or --uppercase    convert calculator file name to uppercase.\n");
  printf("-x   or --executable   force \"TIOS-executable\" bytes to the TI-83 Plus program\n");
  printf("-e   or --unprotected  generate unprotected files (I don't see any use for\n");
  printf("                       that, but you might...).\n");
  printf("-?   or --help         display this screen.\n\n");
  printf("You can only specify one calculator (with -82/ -83 / -83p) at a time.\n\n");
  printf("Publised under the Gnu Public License - Greets to the Ti8x Assembly Scene\n");
}

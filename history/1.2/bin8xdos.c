 /* Bin8x v1.2
  * Copyright (C) 2001 Peter Martijn Kuipers <central@hyperfield.com>
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
  */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <io.h>
#include <conio.h>
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
  
  FILE *infile, *outfile;
  unsigned short int i,n, filesize, destcalc = 0;
  short int uppercase,executable = false;
  unsigned char buffer, optiondefs = 0;
  unsigned char programData[24000];
  unsigned short int checksum;
  char fileio[12] = "--------.---";
  char fileXin[255] = "";
  char fileXout[255] = "";
  const char inext[] = ".bin";
  const char ext83[] = ".83p";
  const char ext8x[] = ".8xp";
  
  

  /* --- Header File Values ---*/
  unsigned char header[11] = {'*', '*', 'T', 'I', '8', '3', '*', '*', 0x1A, 0x0A, 0x00};
  unsigned char comment[42] = "File created under Linux!\0                ";
  unsigned char fileLenLL = 0x00;
  unsigned char fileLenHH = 0x00;
  unsigned char varHeadLL = 0x0B;
  unsigned char varHeadHH = 0x00;
  unsigned char dataLenLL = 0x00;
  unsigned char dataLenHH = 0x00;
  unsigned char progtype = 0x06;
  char name[9] = "\0\0\0\0\0\0\0\0\0";
  unsigned char programLenLL = 0x00;
  unsigned char programLenHH = 0x00;
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
	puts("You can't use both the -83 and -83p option");
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
	puts("You can't use both the -83 and -83p option");
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


  /* End of the option parsing routine */

  if (argc < 2) {
    complain();
    return(1);
  }

  printf("Bin8x v1.2 Ti-83/83+ squisher for Linux\n");
  printf("By Peter Martijn Kuipers <central@hyperfield.com>\n\n");

  if (strlen(argv[1])>8 && !(optiondefs & indef)) {
    puts("Please use short filenames (8.3) or -i!\n\n");
    complain();
    return(2);
  }

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
  
  
  if(optiondefs & indef){
    if ((infile = fopen(fileXin, "r")) == false) {
      puts("Error opening inputfile!");
      printf("File: %s\n",fileXin);
      return(2);
    }
    printf("Using inputfile         : %s\n",fileXin);
  }
  else{
    
    strcpy(fileio, argv[1]);
    strcat(fileio, inext);
    
    if ((infile = fopen(fileio, "r")) == false) {
      puts("Error opening inputfile!");
      printf("File: %s\n",fileio);
      return(2);
    }
    printf("Using inputfile         : %s\n",fileio);
  }

  setmode(fileno(infile), O_BINARY); /* in dos we need this to prevent weird output */


  strcpy(fileio, argv[1]);
  if(destcalc == 0){
    strcpy(fileio, argv[1]);
    if (fgetc(infile) == 0xBB) {
      strcat(fileio, ext8x);
      header[6] = 'F';
      destcalc = 84;
    } else {
      strcat(fileio, ext83);
      destcalc = 83;
    }
    rewind(infile);
  }
  else if(destcalc == 84){
    header[6] = 'F';
    strcat(fileio, ext8x);
  }
  else{
    strcat(fileio, ext83);
  }


  if(optiondefs & outdef){
    if ((outfile = fopen(fileXout, "w")) == false) {
      puts("Error opening outputfile!");
      return(3);
    }
    printf("Using outputfile        : %s\n",fileXout);
  }
  else{
    if ((outfile = fopen(fileio, "w")) == false) {
      puts("Error opening outputfile!");
      return(3);
    }
    printf("Using outputfile        : %s\n",fileio);
  }

  setmode(fileno(outfile), O_BINARY); /* again, it's needed in dos */


  printf("Filename on calculator  : %s",name);

  filesize = -1;

  if(executable && destcalc == 84){
    filesize++;
    programData[filesize] = 0xBB;
    filesize++;
    programData[filesize] = 0x6D;
  }


  while (!feof(infile)) {
    filesize++;
    buffer = fgetc(infile);
    programData[filesize] = buffer; 	/* put this byte in the data array, 
					 * and increase the count */
  }

  if(destcalc == 83){
    fileLenHH = HH(filesize + 0x11);	/* the file length = the size of 
					 * the data array + 17 (0x11) */
    fileLenLL = LL(filesize + 0x11);
  }
  else{
    fileLenHH = HH(filesize + 0x13);	/* the file length = the size of 
					 * the data array + 19 (0x13) */
    fileLenLL = LL(filesize + 0x13);
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
  else{
    puts("\nPlain TI-83 file made!!");
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

  printf("\033[m");

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

  puts("Bin8x 1.2 for DOS.  TI-83/83+ squisher by Peter Martijn Kuipers\n");
  puts("Syntax:");
  puts("bin8x binfile [-options]\n or");
  puts("bin8x -i infile -o outfile -n name [options]\n");
  puts("binfile     : Binary output from your assembler, the .bin extension");
  puts("              is assumed when not using the -i option,");
  puts("              you must not give it ");
  puts("Options:");
  puts("-o   or --output      : specific outputfile ([binfile].8?p if none given.");
  puts("-n   or --calcname    : specific filename on the calculator ([binfile] if");
  puts("                        none given.");
  puts("-i   or --input       : specific inputfile, you must use -o and -n if you");
  puts("                        use it. Also give the complete filename with extension.");
  puts("-83  or --ti83        : force ti-83 file (normally bin8x will autodetect).");
  puts("-83p or --ti83plus    : force ti-83 plus file (normally bin8x will autodetect).");
  puts("-u   or --uppercase   : convert calculator file name to uppercase.");
  puts("-x   or --executable  : force \"TIOS-executable\" bytes to the TI-83 Plus program");
  puts("-e   or --unprotected : generate unprotected files (I don't see any use for");
  puts("                        that, but you might...).");
  puts("-?   or --help        : display this screen.");
  puts("--Press any key to continue--");
  getch();
  puts("\nYou can only force one calculator (with -83 / -83p) at a time.\n");
  puts("Filetype 83/83+ is autodetected, thanks to Solignac Julien\n");
  puts("One last note on the autodetection: If you start with the TI-83Plus-OS");
  puts("                                    executable bytes:");
  puts("                                    \".org progstart-2\"");
  puts("                                    \".db $BB,$6D\"");
  puts("                                    the program is autodetected as an 83+");
  puts("                                    program, otherwise it is considered an");
  puts("                                    83 program.");
  puts("                                    If you write TI-83 Plus programs without");
  printf("                                    these bytes, use the -83p or --ti83plus\n");
  puts("                                    option!\n");
  puts("Publised under the Gnu Public License - Greets to the Ti8x Open Source Assembly Scene");


}

#include <stdio.h> 
#include <malloc.h> 
#include <string.h> 
/* You should use spasm or maybe z80asm to compile your files */
/* zasm seems not working ?!?*/
/* After generate the new *****.bin, you must use bin2var (or bin8x) to generate a 83p file */
/* You could also use bin8x improved which include this tool (when you use -q option) */

int main(int argc, char * argv[]) {

	FILE * fpr;
	FILE * fp;
	int e;
	char * outfilename;
	char c;
	char end[] = {0x3F ,0xD4  ,0x3F ,0x30 ,0x30 ,0x30  ,0x30 ,0x3F ,0xD4};

	printf("bin2bin83 by Thibault Duponchelle <t.duponchelle@gmail.com>\n\n");
	/* open bin files */

	/* We don't need to use getopt for a so tiny file, isn't it ;) */
	if(argc<2) {
		printf("*******Not enough args*******\n");
		printf("usage : bin2bin83 input.bin [output] \n");
		printf("output is not necessary, by default \"output.bin\" will be generated.\n");
		printf("output parameter must be like this : \"WORM\" NOT like \"WORM.BIN\".\n");
		printf("Example : 'bin2bin83 foo.bin BAR'\n");
		printf("The output will be \"BAR83.bin\".\n(note : '83.bin' have been automatically added).\n");
		return 0;
	} else {
		if((fpr = fopen(argv[1], "r+b"))) {
			if(argc>2) {
				outfilename=malloc(strlen(argv[2])+6);
				strcpy(outfilename,argv[2]);
				strcat(outfilename,"83.bin");
				if(!(fp=fopen(outfilename,"w+b"))) {
					printf("CAN NOT open output.bin");
					fclose(fpr);
					return 1;
				}
			}else {
				outfilename=malloc(strlen("output.bin")+1);
				strcpy(outfilename,"output.bin");
				if(!(fp=fopen(outfilename,"w+b"))) {
					printf("CAN NOT open output.bin\n");
					fclose(fpr);
				return 1;
				}
			}
			
			while((e=fgetc(fpr))!=EOF)
			{
				//printf("%c= %02X ",c, c); //Print hexa code could bug the console ;D
				fprintf(fp,"%02X",e);
			}
			fwrite(&end,9,1,fp);
			printf("output : %s\n",outfilename);
			fclose(fp);
			fclose(fpr);

		} else {
			printf("CAN NOT open %s\n",argv[1]);
		}
	printf("Please send me your feedback : <t.duponchelle@gmail.com>\n");
	return 0;
	}

}
/* That's all ! */

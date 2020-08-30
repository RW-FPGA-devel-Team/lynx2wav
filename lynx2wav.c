/* lynx2wav RetroWiki tape converter*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define PI 3.14159265


FILE *in, *out;
int file_size;
int speed = 22050;
int tape_1 =50;
struct
{
	char sig[4];
	int riff_size;
	char typesig[4];
	char fmtsig[4];
	int fmtsize;
	short tag;
	short channels;
	int freq;
	int bytes_per_sec;
	short byte_per_sample;
	short bits_per_sample;
	char samplesig[4];
	int datalength;

}sample_riff= { "RIFF",0,"WAVE","fmt ",16,1,1,22050,22050,1,8,"data",0 }; 

int stricmp(const char *a, const char *b)
{
	int ca, cb;
	do
	{
		ca = (unsigned char)*a++;
		cb = (unsigned char)*b++;
		ca = tolower(toupper(ca));
		cb = tolower(toupper(cb));
	} while (ca == cb && ca != '\0');
	return ca - cb;
}


//static int current_level=223;
//void inverse_level() { current_level=255-current_level; }

void emit_level(int size, int nBit)
{
    int i;
	double x, ret, val, boost;
	int value;

    for (i=0;i<size;i++) {
		      x=i*360/size;
			  val = PI / 180.0;
			  value=(sin(x*val)*1.25);
              
			  boost=1;

			  if (nBit)
			     boost=1.2;
			  
			  
			  value=(sin(x*val)* -100 );
              
			  fputc((value*boost)+128,out);
			  			
	}
	file_size+=size;
}


void emit_standard_short_level() { emit_level(tape_1 / 2 ,0); }
void emit_standard_long_level() { emit_level(tape_1,1);  }


void emit_bit(int bit)
{
	int nBit = bit;
	if (nBit)
	{
		emit_standard_long_level();
	}		
		else {
				emit_standard_short_level();
			}
	
}

void emit_byte(unsigned short n)
{
    int x;

    for(x=0;x<8;x++)
    {
        emit_bit( n & 0x80 ? 1 : 0);
        n <<= 1;

    }
    
}



int init(int argc, char *argv[])
{
	int i;
	if (argc < 3)
		return 1;
	for (i = 1; i < argc - 2; i++)
	{
		if (strcmp(argv[i], "-1") == 0)
			tape_1 = 33;
		else if (strcmp(argv[i], "-2") == 0)
			tape_1 = 25;
		else if (strcmp(argv[i], "-3") == 0)
			tape_1 = 20;
		else if (strcmp(argv[i], "-4") == 0)
			tape_1 = 16;
		else if (strcmp(argv[i], "-5") == 0)
			tape_1 = 14;
		else
		{
			printf("Bad option\n\n");
			return 1;
		}
	}
	sample_riff.freq = sample_riff.bytes_per_sec = speed;
	in = fopen(argv[argc - 2], "rb");
	if (in == NULL)
	{
		printf("Cannot open %s file\n\n", argv[argc - 2]);
		return 1;
	}
	out = fopen(argv[argc - 1], "wb");
	if (out == NULL)
	{
		printf("Cannot create %s file\n\n", argv[argc - 1]);
		return 1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int i, size;
	unsigned char name;

	if (init(argc, argv))
	{
		printf("Usage: %s [ -2 | -5 ] [ -N ] <.TAP file> <.WAV file>\n", argv[0]);
		printf("Options: -1 produces a TAPE 1 WAV file\n");
		printf("         -2 produces a TAPE 2 WAV file\n");
		printf("         -3 produces a TAPE 3 WAV file\n");
		printf("         -4 produces a TAPE 4 WAV file\n");
		printf("         -5 produces a TAPE 5 WAV file  (default is TAPE 0)\n");
		exit(1);
	}

	fwrite(&sample_riff, 1, sizeof(sample_riff), out);

	
			while (!feof(in))
			{

				
				/* first pilot with 128 0x00 char */
				for (i=0; i<128; i++)
					emit_byte(0x00);

	            /* 0xA5 for syncro */
		
				emit_byte(0xA5);
				
				/* Print and save the program name */
				//emit_bit(0);
                emit_byte(0x22);
				putchar ('"');

				fgetc(in);
                while (name!='"'){
                     name=fgetc(in);
				     emit_byte(name);
					 putchar(name);
				}
                
				
				/* second pilot with 128 0x00 char and 0xA5*/
				for (i=0; i<771; i++)
					emit_byte(0x00);

                emit_byte(0xA5);
				
                char tap_type  = fgetc(in);
				printf ("Tipo: %c\n",tap_type);
				emit_byte(tap_type);

				/* Play until end of file */
				while (!feof(in))
				 {
					 emit_byte(fgetc(in));
				 }

				/* Done */
				printf("end.\n");
			}
		
	
	fclose(in);

	sample_riff.datalength = file_size;
	sample_riff.riff_size = sample_riff.datalength + 8 + sample_riff.fmtsize + 12;
	fseek(out, 0, SEEK_SET);
	fwrite(&sample_riff, 1, sizeof(sample_riff), out);

	fclose(out);
}

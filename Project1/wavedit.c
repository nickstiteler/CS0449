#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Nicholas Stiteler
//njs74
//cs449
//project 1

typedef struct WAVH{
	char riff_id[4];
	unsigned int file_size;
	char wave_id[4];
	char fmt_id[4];
	unsigned int fmt_size;
	unsigned short data_format;
	unsigned short number_of_channels;
	unsigned int samples_per_second;
	unsigned int bytes_per_second;
	unsigned short block_alignment;
	unsigned short bits_per_sample;
	char data_id[4];
	unsigned int data_size;

}WAVH;

int streq_nocase(const char* a, const char* b)
{
	for(;*a && *b; a++,b++) if(tolower(*a) != tolower(*b)) return 0;
	return *a == 0 && *b == 0;
}

FILE* checked_fopen(const char* name, const char* mode)
{
	FILE* f = fopen(name,mode);

	if(f == NULL)
	{
		fprintf(stderr, "could not open file '%s'\n",name);
		exit(1);
	}

	return f;
}
int check_wav(const WAVH wav)
{
	if(strncmp(wav.riff_id,"RIFF",4) != 0)
	{
		printf("INVALID WAV FILE\n");
		exit(1);
	}
	if(strncmp(wav.wave_id,"WAVE",4) != 0)
	{
		printf("INVALID WAV FILE\n");
		exit(1);
	}
	if(strncmp(wav.fmt_id,"fmt ",4) != 0)
	{
		printf("INVALID WAV FILE\n");
		exit(1);
	}
	if(strncmp(wav.data_id,"data",4) != 0)
	{
		printf("INVALID WAV FILE\n");
		exit(1);
	}
	if(wav.fmt_size != 16)
	{
		printf("INVALID WAV FILE\n");
		exit(1);
	}
	if(wav.data_format != 1)
	{
		printf("INVALID WAV FILE\n");
		exit(1);
	}
	if(wav.number_of_channels != 1 && wav.number_of_channels != 2)
	{
		printf("INVALID WAV FILE\n");
		exit(1);
	}
	if(wav.samples_per_second <= 0 || wav.samples_per_second > 192000)
	{
		printf("INVALID WAV FILE\n");
		exit(1);
	}
	if(wav.bits_per_sample != 8 && wav.bits_per_sample != 16)
	{
		printf("INVALID WAV FILE\n");
		exit(1);
	}
	if(wav.bytes_per_second != wav.samples_per_second*(wav.bits_per_sample)/8*wav.number_of_channels)
	{
		printf("INVALID WAV FILE\n");
		exit(1);
	}
	if(wav.block_alignment != (wav.bits_per_sample)/8*wav.number_of_channels)
	{
		printf("INVALID WAV FILE\n");
		exit(1);
	}
	return 0;
}

int main(int argc,char** argv)
{
	if(argc == 1)
	{
		printf("\nNo arguments detected.\n");
		printf("Please enter either:\n");
		printf("\t--'./wavedit filename.wav' with filename being any file available\n");
		printf("\t--'./wavedit filename.wav -rate 22050' to change the \n\t\tspeed to a number of your choice\n");
		printf("\t--'./wavedit filename.wav -reverse' to reverse the sound\n");
		return 0;
	}
	else if(argc == 2)
	{
		FILE* f = checked_fopen(argv[1], "rb");
		WAVH wav;

		fread(&wav,sizeof(wav),1,f);
		printf("\n\n%ld\n\n",sizeof(wav));

		check_wav(wav);

		char* sound_type;
		if(wav.number_of_channels == 1)
		{
			sound_type = "mono";
		}
		else
		{
			sound_type = "stereo";
		}

		printf("This is a %hu-bit %dHz %s sound.\n",wav.bits_per_sample,wav.samples_per_second,sound_type);

		int samp_len = wav.data_size/wav.block_alignment;
		float samp_sec_len = samp_len/(float)wav.samples_per_second;
		printf("It is %d samples (%.3f seconds) long\n",samp_len,samp_sec_len);
		/*
		printf("riff_id = %.4s\n",wav.riff_id);
		printf("file_size = %d\n",wav.file_size);
		printf("wave_id = %.4s\n",wav.wave_id);
		printf("fmt_id = %.4s\n",wav.fmt_id);
		printf("fmt_size = %d\n",wav.fmt_size);
		printf("data_format = %hu\n",wav.data_format);
		printf("number_of_channels = %hu\n",wav.number_of_channels);
		printf("samples_per_second = %d\n",wav.samples_per_second);
		printf("bytes_per_second = %d\n",wav.bytes_per_second);
		printf("block_alignment = %hu\n",wav.block_alignment);
		printf("bits_per_sample = %hu\n",wav.bits_per_sample);
		printf("data_id = %.4s\n",wav.data_id);
		printf("data_size = %d\n",wav.data_size);
		*/

		fclose(f);
	}
	else if(argc == 4)
	{
		if(streq_nocase(argv[2],"-rate")!=1)
		{
			printf("ERROR: Please enter something similar to:\n");
			printf("./wavedit 'filename' -rate (0<num>=192000)\n");
			exit(1);
		}
		else
		{
			if(*argv[3] > 0 && *argv[3] <=192000)
			{
				FILE* f = checked_fopen(argv[1], "rb+");
				WAVH wav;

				fread(&wav,sizeof(wav),1,f);

				check_wav(wav);

				wav.samples_per_second = *argv[3];

				int calc_new = wav.samples_per_second*(wav.bits_per_sample)/8*wav.number_of_channels;
				wav.bytes_per_second = calc_new;

				fseek(f,0,SEEK_SET);
				fwrite(&wav,sizeof(wav),1,f);

				fclose(f);
			}
			else
			{
				printf("ERROR: Please enter a rate greater than 0 and less than or equal to 192000\n");
				exit(1);
			}
		}
	}
	else if(argc == 3)
	{
		if(streq_nocase(argv[2],"-reverse") != 1)
		{
			printf("ERROR: Please enter something similar to:\n");
			printf("./wavedit 'filename' -reverse\n");
			exit(1);
		}
		else
		{
			FILE* f = checked_fopen(argv[1], "rb+");
			WAVH wav;

			fread(&wav,sizeof(wav),1,f);

			check_wav(wav);

			int samp_len = wav.data_size/wav.block_alignment;

			
			if(wav.number_of_channels == 1)
			{
				if(wav.bits_per_sample == 8)
				{
					char samples[samp_len];
					fread(&samples,sizeof(samples),1,f);

					int i;
					for(i = 0;i<samp_len/2;i++)
					{ 
						int lower = samples[i];
						int higher = samples[samp_len-i-1];

						samples[i] = higher;
						samples[samp_len-i-1] = lower;
					}

					fseek(f,44,SEEK_SET);
					fwrite(&samples,sizeof(samples),1,f);
				}
				else
				{
					short samples[samp_len];
					fread(&samples,sizeof(samples),1,f);

					int i;
					for(i = 0;i<samp_len/2;i++)
					{ 
						int lower = samples[i];
						int higher = samples[samp_len-i-1];

						samples[i] = higher;
						samples[samp_len-i-1] = lower;
					}

					fseek(f,44,SEEK_SET);
					fwrite(&samples,sizeof(samples),1,f);
				}
			}
			else
			{
				if(wav.bits_per_sample == 8)
				{
					short samples[samp_len];
					fread(&samples,sizeof(samples),1,f);

					int i;
					for(i = 0;i<samp_len/2;i++)
					{ 
						int lower = samples[i];
						int higher = samples[samp_len-i-1];

						samples[i] = higher;
						samples[samp_len-i-1] = lower;
					}

					fseek(f,44,SEEK_SET);
					fwrite(&samples,sizeof(samples),1,f);
				}
				else
				{
					int samples[samp_len];
					fread(&samples,sizeof(samples),1,f);

					int i;
					for(i = 0;i<samp_len/2;i++)
					{ 
						int lower = samples[i];
						int higher = samples[samp_len-i-1];

						samples[i] = higher;
						samples[samp_len-i-1] = lower;
					}
					fseek(f,44,SEEK_SET);
					fwrite(&samples,sizeof(samples),1,f);
				}
			}
			fclose(f);
		}
	}
	else
	{
		printf("\nToo many arguments detected.\n");
		printf("Please enter either:\n");
		printf("\t--'./wavedit filename.wav' with filename being any file available\n");
		printf("\t--'./wavedit filename.wav -rate 22050' to change the \n\t\tspeed to a number of your choice\n");
		printf("\t--'./wavedit filename.wav -reverse' to reverse the sound\n");
		return 0;
	}
	//WAVHeader wav;
	//printf("%ld",sizeof(wav));
	return 0;
}
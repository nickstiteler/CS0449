#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//Nicholas Stiteler
//njs74
//cs449
//proj3

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
int main(int argc,char** argv)
{
	FILE* f = checked_fopen(argv[1], "rb");

	int c,minStr;
	char str[4];
	minStr = 0;
	while((c = fgetc(f)) != EOF)
	{
		if(c>=32 && c<=126)
		{
			if(minStr < 4)
			{
				str[minStr] = c;
				minStr++;
			}
			else if(minStr == 4)
			{
				int i;
				for(i=0;i<4;i++)
				{
					printf("%c",str[i]);
				}
				minStr++;
				printf("%c",c);
			}
			else
			{
				minStr++;
				printf("%c",c);
			}
		}
		else
		{
			if(minStr == 4)
			{
				int i;
				for(i=0;i<4;i++)
				{
					printf("%c",str[i]);
				}
				printf("\n");
			}
			else if(minStr >4)
			{
				printf("\n");
			}
			
			minStr = 0;
		}
	}

	fclose(f);
}
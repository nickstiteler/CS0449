#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>


//Nicholas Stiteler
//njs74
//cs449
//proj1

int random_range(int low,int high)
{	
	return rand() % (high-low)+low;
}
int streq_nocase(const char* a, const char* b)
{
	for(;*a && *b; a++,b++) if(tolower(*a) != tolower(*b)) return 0;
	return *a == 0 && *b == 0;
}


int main(int argc,char** argv)
{
	srand((unsigned int)time(NULL));

	//File IO
	FILE* f = fopen("dictionary.txt","r");

	if(f == NULL)
	{
		printf("dictionary.txt doesn't exist!\n");
		return 1;
	}
	char num_words[20];
	fgets(num_words,sizeof(num_words),f);
	
	int length;
	sscanf(num_words,"%d",&length);

	//printf("%d\n",length);

	char words[length][20];
	
	int i;
	for(i = 0;i<length;i++)
	{
		fgets(words[i],20,f);
	}
	int j = 0;
	for(i = 0;i<length;i++)
	{
		j = 0;
		while (j<20)
		{
			if(words[i][j] == '\n')
			{
				//printf("%dDELETING???\n",i);
				words[i][j] = '\0';
				break;
			}
			j++;
		}
	}
	
	fclose(f);
	//END FILE IO

	int randNum;
	char* pickedWord;
	int wordLen;
	if(argc >1)
	{
		pickedWord = argv[1];
		wordLen = strlen(argv[1]);
	}
	else
	{
		randNum = random_range(0,length);
		pickedWord = words[randNum];
		//printf("%s\n",pickedWord);
		wordLen = strlen(words[randNum]);
	}

	printf("Welcome to hangman! Your word has %d letters:\n",wordLen);

	char input[20];
	char outStr[20]; //str that gets displayed to user with _'s
	for(i=0;i<wordLen;i++)
	{
		outStr[i] = '_';
		printf("_ ");
	}
	printf("Guess a letter or type the whole word: ");
	fgets(input,20,stdin);

	for(i = 0;i<20;i++)
	{
		if(input[i] == '\n')
		{
			//printf("%dDELETING???\n",i);
			input[i] = '\0';
			break;
		}
	}

	int strikes = 0;
	int matched = 0;
	int enteredLength;
	while(strikes<5)
	{	
		enteredLength = strlen(input);
		//printf("\n%d\n",enteredLength);

		if(enteredLength >1) //if they guess a word
		{
			if(enteredLength == wordLen)//if word lengths are equal
			{
				//printf("you are here\n");
				if(streq_nocase(input,pickedWord) == 1)
				{
					printf("You got it! The word was '%s'.",pickedWord);
					break;
				}
				else
				{
					strikes++;
					printf("Strike %d!\n",strikes);
				}
			}
			else // if word lengths aren't equal
			{
				strikes++;
				printf("Strike %d!\n",strikes);
			}
		}
		else if(enteredLength == 1) //if the guess a letter
		{
			char* tempInput = &input[0];
			bool found = false;
			
			for(i=0;i<wordLen;i++)
			{
				if(tempInput[0]<97) // convert to lower
				{
					tempInput[0] += 32;
				}
				//printf("%d  %d\n",pickedWord[0],tempInput[0]);
				if(pickedWord[i]==tempInput[0])
				{
					if(outStr[i] != tempInput[0])
					{
						matched++;
					}
					outStr[i] = tempInput[0];
					found = true;
				}
			}
			if(found == false)
			{
				strikes++;
				printf("Strike %d!\n",strikes);
			}
		}
		else
		{
			//they didn't guess anything so do nothing
		}

		if(matched == wordLen)
		{
			printf("You got it! The word was '%s'.",pickedWord);
			break;
		}
		if(strikes ==5)
		{
			break;
		}
		else 
		{
			for(i=0;i<wordLen;i++)
			{
				printf("%c ",outStr[i]);
			}
			printf("Guess a letter or type the whole word: ");
			fgets(input,20,stdin);

			for(i = 0;i<20;i++)
			{
				if(input[i] == '\n')
				{
					//printf("%dDELETING???\n",i);
					input[i] = '\0';
					break;
				}
			}
		}
		
	}
	if(strikes == 5)
	{
		printf("Sorry, you lost! The word was '%s'.",pickedWord);
	}
	

	printf("\n");
	return 0;
}
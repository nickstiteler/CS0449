#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

//Nicholas Stiteler
//cs0449
//proj4


int getNumTokens(char *input) // gets the number of tokens to be used in mallocing 
{
	int num_tokens = (strlen(input) / 2) + 1; // worst case...
	char* tokens[num_tokens];

	const char* delim = " \t\n";
	char* token = strtok(input, delim);
	int i;
	for(i = 0; token != NULL; i++)
	{
		tokens[i] = token;
		token = strtok(NULL, delim);
	}
	return i;
}

int splitter(char *input, char** cmds_arr) // this function takes in an array and parses it into cmds_arr
{
	int num_tokens = (strlen(input) / 2) + 1; // worst case...
	char* tokens[num_tokens];

	const char* delim = " \t\n";
	char* token = strtok(input, delim);
	int i;
	for(i = 0; token != NULL; i++)
	{
		tokens[i] = token;
		token = strtok(NULL, delim);
	}

	tokens[i] = NULL; 

	int num_cmds = i;
	
	for(i = 0;i<num_cmds;i++)
	{
		cmds_arr[i] = tokens[i];
	}
	
	return num_cmds;
}


int main(int argc, char** argv)
{
	signal(SIGINT, SIG_IGN);
	while(1)
	{
		// declaring variables
		printf("myshell> ");
		char input[300];
		char** cmds_arr;
		fgets(input,sizeof(input),stdin);

		char input2[300];
		strcpy(input2,input); // copies input into another string to be used later
		int num_cmds = getNumTokens(input); // function call

		cmds_arr = malloc(20*num_cmds); // mallocs 
	
		int j;
		for(j = 0;j<num_cmds;j++)
		{
			cmds_arr[j] = malloc(20); //malloc for each array
		}

		splitter(input2,cmds_arr);	// function call	

		if(num_cmds != 0)
		{
			if(strcmp(cmds_arr[0],"exit") == 0) //if exit is entered in the first cmd
			{
				if(cmds_arr[1] != NULL) //if something is entered after exit
				{
					int exInt;
					char* garbo;
					exInt = strtol(cmds_arr[1],&garbo,10); // parses cmds_arr[1] and puts the int in exInt
					exit(exInt);
				}
				else //if nothing is entered after exit then exit with 0
				{
					exit(0);
				}
			}

			if(strcmp(cmds_arr[0],"cd") == 0) // if cd is entered 
			{
				chdir(cmds_arr[1]); // cds to whatever is in arg 1
			}
			else
			{
				if(fork() == 0) 
				{
					int i;
					int gThan, lThan;
					gThan = 0;
					lThan = 0;
					for(i = 0;i<num_cmds;i++) // this for loop checks to make sure the user is not trying redirect more than once
					{
						if(strcmp(cmds_arr[i],">") == 0)
						{
							gThan++;
						}
						else if(strcmp(cmds_arr[i],"<") == 0)
						{
							lThan++;
						}

					}

					if(gThan > 1 || lThan > 1) // exits with code 2 if it tries to redirect more than once
					{
						printf("You can only stdout/stdin ONCE\n");\
						exit(2);
					}
					else
					{
						for(i= num_cmds-1;i>=0;i--)
						{
							//printf("%d\n",i);
							if(strcmp(cmds_arr[i],">") == 0)
							{
								//printf("%s\n",cmds_arr[i] );
								freopen(cmds_arr[i+1],"w",stdout);
								cmds_arr[i] = NULL;
								
							}
							else if(strcmp(cmds_arr[i],"<") == 0)
							{
								//printf("%s\n",cmds_arr[i]);
								freopen(cmds_arr[i+1],"r",stdin);
								cmds_arr[i] = NULL;
							}
						}
					}

					signal(SIGINT, SIG_DFL);

					execvp(cmds_arr[0], &cmds_arr[0]);

					perror("Error: ");
					exit(1);
				} 
				else 
				{
					int status;
					int childpid = waitpid(-1, &status, 0);

					if(childpid == -1) // if a negative 1 error is recieved 
					{
						perror("Error: ");
					}
					else if(status == 0) // if the program runs successfully 
					{
						//printf("Program exited successfully!\n");
					}
					else if(status != 0 && WIFEXITED(status)) //if the program exits with a custom code
					{
						//printf("%d\n",WEXITSTATUS(status) );
						//WEXITSTATUS(status)
						printf("Program exited with error code %d\n",WEXITSTATUS(status));
					}
					else if(signal(SIGINT, SIG_DFL),"SIG_ERR") 
					{
						printf("\n");
						printf("Program terminated due to signal: %s\n",strsignal(SIGINT));
					}
					else
					{
						printf("Program terminated some other way!\n");
					}
				}
			}
		}
	}
	return 0;
}

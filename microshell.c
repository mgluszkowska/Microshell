#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"
#define MAGENTA  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define RED "\x1B[31m"

#define SPECIAL_CHAR " \t\r\n\a"

void dollar() {
    char current_dir[256];
    char username[50];
    getlogin_r(username, 50);
    getcwd(current_dir, sizeof(current_dir));
    printf(MAGENTA "%s" GREEN "%s $ " RESET, username, current_dir);
}

void write_to_file(char * file, char ** words, int position) {

	int fd;
	fd = open(file, O_WRONLY | O_APPEND | O_CREAT, 0666);
	for (int i = 1; i < position; i++) {		
		char buffer[strlen(words[i])+2];
		sprintf(buffer, "%s ", words[i]);
		write(fd, buffer, sizeof(buffer));			
	}
	close(fd);
	printf("Text written to file successfully\n");
} 

void overwrite_file(char * file, char ** words, int position) {

	int fd;
	fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0666);
	for (int i = 1; i < position; i++) {
		char buffer[strlen(words[i])+2];
		sprintf(buffer, "%s ", words[i]);
		write(fd, buffer, sizeof(buffer));		
	}
	close(fd);
	printf("Text written to file successfully\n");
} 

int main() {

    char * line = NULL;
    ssize_t buffersize = 0;
    char ** words;
    char *word;
    int wordcount;
    int pid;

    while(1) {
    	
	/*Clear the previous commands */
    	for (int i=0; i<=wordcount; i++)
    		words[i] = NULL;
    		
        /* Prompt user with the username, current directory and $ */
        dollar();
        
        /*Read a command line*/
	getline(&line, &buffersize, stdin);
             
        /*Split the command line into separate words*/
        wordcount = 0;
        word = strtok(line, SPECIAL_CHAR);
        while (word != NULL) {
        	words[wordcount] = word;
        	wordcount++;
        	word = strtok(NULL, SPECIAL_CHAR);
        	
        }     
        
        /* Handle the command */
        
        /* exit */
        if (strcmp(words[0], "exit") == 0) 
            return 0;
            
        /* cd */
        else if (strcmp(words[0], "cd") == 0) {
            if (chdir(words[1]) != 0) 
                perror(RED "Error: unable to change the directory" RESET);
        }
        
        /* help */
        else if(strcmp(words[0], "help") == 0) {
            printf(KCYN "************************************************\n");
            printf("This Microshell was written by Marta Głuszkowska\n\n");
            printf("Built in commands:\n");
            printf("\tcd [path] - changes the current directory to the one stated in [path]\n");
            printf("\texit - finishes Microshell\n");
            printf("\trm [file] - deletes the given file\n");
            printf("\techo [text] - displays [text] on the screen\n");
            printf("\techo > [file] - creates an empty file with the name [file]\n");
            printf("\techo [text] > [file] - writes [text] into the [file]\n");
            printf("\techo [text] >> [file] - writes [text] into the end of [file]\n");
            printf("\thelp - displays information about the author and a list of available commands\n");
            printf("\nYou can also use other Linux commands. For more info use man.\n");
            printf("************************************************\n" RESET);
        }
        
        /* rm */
        else if (strcmp(words[0], "rm") == 0) {
                if (remove(words[1]) == 0 )
                    printf("File deleted successfully\n");
                else
                    perror(RED "Error while deleting the file" RESET);
        }
        
        /* echo */
        else if (strcmp(words[0], "echo") == 0) {
        	char decision = 'p';
        	int i;
        	int position = 0;
        	for (int i=0; i<wordcount; i++) {
        		if (strcmp(words[i], ">") == 0) {
        			decision = 'o';
        			position = i;
        			break;     			
        		}
        		else if (strcmp(words[i], ">>") == 0) {
        			decision = 'w';
        			position = i;
        			break;      			
        		}       
        	}
        	switch (decision) {
        		case 'p':
        			for (int j=1; j<wordcount; j++)
        				printf("%s ", words[j]);
        			printf("\n");
        			break;
        		case 'o':
				
        			if (wordcount == position +1)
        				printf(RED "No file name given.\n" RESET);
        			else 
        				overwrite_file(words[position+1], words, position);
        			break;
        		case 'w':
        			if (wordcount == position +1)
        				printf(RED "No file name given." RESET);
        			else 
        				write_to_file(words[position+1], words, position);
        			break;
        	}
        }
        
        /* other commands */
        else {
        	pid = fork();
        	
        	/* child process executes the command */
        	if (pid == 0) { 
        		if (execvp(words[0], words) == -1) {
					perror( RED "Error" RESET);
				}
        	}
        	
        	/* parent waits for the child */
        	else { 
        		wait(NULL);
        	}
        }		
    }    
}
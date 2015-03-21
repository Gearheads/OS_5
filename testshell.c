/**
 * Rob Casale, Mariam Tsilosani, Richard McEwan, Victor Kaiser-Pendergast,
 * and Lucas Rivera
 * CS 416
 * Section 4
 * Assignment 5
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

struct builtins {
    char *name;               /* name of function */
    int (*f)();               /* function to execute for the built-in command */
};

typedef struct commandnode {
    char* com;                /* pointer to the command */
    struct commandnode* next; /* pointer to the next command */
}node;

node *commands[50];           /* linked list of all the commands */
char buffer[50];              /* buffer to read in the commands */
int counter = 0;

/**
 * cd [dirname]
 *   The cd command changes the current working directory to that
 *   specified by the first and only argument. An error is printed
 *   if more than one arguments are present or if changing the
 *   directory fails. Otherwise, the full pathname of the new directory
 *   is printed. See the chdir system call man page and the getcwd
 *   library function. If there is no argument given to the cd command
 *   then change to the user's home directory. You can get this by
 *   reading the envrionment variable HOME via getenv("HOME").
 */
int localcd(int argc, char **argv) {
    if(argc > 1) {
        printf("Error: max of one argument");
    }
    else if(argc == 0)
        chdir(getenv("HOME"));
    else {
        char my_cwd[1024];
	getcwd(my_cwd, 1024);
	char* input = strcat(my_cwd, argv[0]);
	int retVal = chdir(input);
	if(retVal== -1)
	    printf("Error: not able to change to directory specified.");
	else {
	    getcwd(my_cwd, 1024);
	    printf("%s",my_cwd);
	}
    }
    return 0;
}

/**
 * exit [value]
 *   The exit command causes the shell process to exit by calling
 *   the exit library function (which, in turn, calls the _exit system call).
 *   If no parameter is provided, then exit returns with an exit code of 0.
 *   Otherwise, exit returns with the error code provided by value. Note that
 *   the exit code will be truncated to eight bits (0..255). No need to print
 *   an error here since chances are that we really want to exit the process,
 *   even if the syntax is wrong.
 */
void localexit(int argc, char **argv) {
    if(argc < 2) {
        exit(0);
    }
    else {
	long val = 0;
	char *temp;
	val = strtol(argv[1], &temp, 0);
	if(*temp != '\0') {
		// Bad argument functionality identical to Bash
		printf("exit: %s: numeric argument required\n", argv[1]);
		exit(255);
	} else {
        	exit(val);	
	}
    }
}

/**
 * insertNode inserts a new command found in the corresponding place in the command array
 */
void insertNode(int insert, node* new) {
    int i = 0;
    while(i < strlen(new->com)) {
        if(new->com[i] != ' ') {
            break;
        }
	i++;
    }
    if(i == strlen(new->com)) {
        free(new->com);
	free(new);
	return;
    }
    node* temp = commands[insert];
    // insert the first command
    if(temp == NULL) {
        commands[insert] = new;
	commands[insert]->next = NULL;
	return;
    }
    // add all other commands to the end
    while(temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new;
    new->next = NULL;
    return;
}
/**
 * printList prints all the commands found in the given line
 */
void printList() {
    node* temp;
    int i = 0;
    while(commands[i] != NULL && i < 50) {
        printf("next command\n");
        temp = commands[i];
        while(temp != NULL) {
            printf("%s\n",temp->com);
	    temp = temp->next;
        }
	i++;
    }
}

/**
 * Parses the command that was readin from the terminal
 */
void parseCommand(char* line) {
    int i;
    for(i = 0; i < 50; i++) {
        commands[i] = NULL;
    }
    char c;
    int counter = 0;
    int insert = 0;
    int bufferCounter = 0;
    int boolean = 0;
    while(counter < strlen(line)) {
        c = line[counter];
	if(counter == strlen(line) - 1) {
	    buffer[bufferCounter] = c;
	    buffer[bufferCounter] = '\0';
	    node* new = malloc(sizeof(node));
            new->com = malloc(strlen(buffer)+1);
            strcpy(new->com,buffer);
            insertNode(insert,new);
	    break;
	}
	if(boolean == 0) {
	    if(c == '"') {
	        boolean = 1;
		counter++;
		continue;
	    }
	}
	if(boolean == 1) {
            if(c == '"') {
	        boolean = 0;
		buffer[bufferCounter] = '\0';
                node* new = malloc(sizeof(node));
                new->com = malloc(strlen(buffer)+1);
                strcpy(new->com,buffer);
                insertNode(insert,new);
		bufferCounter = 0;
		counter++;	
		continue;
	    }
	    buffer[bufferCounter] = c;
	    counter++;
            bufferCounter++;
	    continue;
	} 
	if(c != ' ' && c != '|') {
	    buffer[bufferCounter] = c;
	    bufferCounter++;
	}
	else if(c == '|') {
	    buffer[bufferCounter] = '\0';
	    node* new = malloc(sizeof(node));
 	    new->com = malloc(strlen(buffer)+1);
	    strcpy(new->com,buffer);
	    insertNode(insert,new);
            bufferCounter = 0;
	    insert++;
	}
	else {
	    buffer[bufferCounter] = '\0';
	    node* new = malloc(sizeof(node));
	    new->com = malloc(strlen(buffer)+1);
            strcpy(new->com,buffer);
            insertNode(insert,new);
	    bufferCounter = 0;
	}
	counter++;
    }
    printList();
}

/**
 * Starts the shell that we created and will return the
 * exit status of the process.
 * 
 * Get a command line from the user. You may assume that
 * the entire command is on one line. This line may be a
 * pipeline of one or more commands. 
 * 
 * Break the command and arguments into an argument list
 * (an array of char *). You may assume that no command
 * will have more than 50 arguments.
 *
 * Built-in commands must be implemented as a table of
 * pointers to functions.
 *
 * Each built-in command accepts the same parameters that
 * any external program will get: (int argc, char **argv).
 *
 * Neither of the built-in commands have to function in a
 * pipeline of commands.
 */
int startShell() {
    char *line = NULL;
    size_t size = 0;
    ssize_t read;
    printf("$ ");
    while((read = getline(&line, &size, stdin)) != -1) {
        parseCommand(line);
        if (isatty(0)) {
            fputs(line, stderr);
            printf("the standard input is from a terminal\n");
        } 
        else {
            printf("the standard input is NOT from a terminal\n");
        }
        if (isatty(1)) {
            printf("the standard output is to a terminal\n");
        }
        else {
            printf("the standard input is NOT to a terminal\n");
        }

	// Wait for all child processes to complete
	int status = 0;
	while(wait(&status) != -1) {
		printf("Command exited with status %d\n", status);
	}
	
        printf("$ ");
    }
    printf("\n");
    return 0;
}

/**
 * Your assignment will be a simple shell that will run one command or a
 * pipeline of commands. When each process terminates, the shell will print
 * the exit status for the process.
 * 
 * Your goal in this assignment is to become familiar with the basic set of
 * system calls that let you create processes, establish pipes between them,
 * and detect when a child process has died. This assignment will use the
 * fork, execve, wait, pipe, dup2, chdir, and exit system calls.
 * 
 * You do not have to implement multi-line commands, environment variables,
 * or I/O redirection. 
 */
int main(int argc, char** argv) {
    if(argc > 1) {
        printf("Error: no arguments are needed\n");
        return 1;
    }
    else {
        startShell();
	return 0;
    }
}

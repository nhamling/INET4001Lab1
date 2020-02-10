#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <unistd.h>
#include <grp.h>
#include <alloca.h>
#include <errno.h>
#include <sched.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXCOM 1000
#define MAXLIST 100
#define clear() printf("\033[H\033[J")
const int u = 65534;

//child function for cloning commands (Not Working)
/*
static int childFunc() {
	int ret;
	mount("proc", "/proc", "proc", 0, "");
	setgid(u);
	setgroups(0, NULL);
	setuid(u);
	ret = execl("/bin/bash", "/bin/bash", NULL);
	return ret;
}
*/

//Function that provides a display at the start of the shell.
void startPrompt() {
	clear();
	printf("\n\n**Welcome to HamShell**\n");
	sleep(1);
	clear();
}

//prints current directory
void curDir() {
	char cd[1024];
	getcwd(cd, sizeof(cd));
	printf("\nDirectory: %s", cd);
}

//Shell Help
void openHelp() {
	puts("\n**Help Menu**"
	"\nBuilt in Commands:"
	"\n-->cd <arg>"
	"\n-->exit"
	"\n-->help"
	"\n-->user"
	"\n**All other Linux commands are available**"
	);
}

//Function that takes user input
int takeIn(char* str) {
	char* buffer;

	buffer = readline("\n--> ");
	if (strlen(buffer) != 0) {
		add_history(buffer);
		strcpy(str, buffer);
		return 0;
	}
	else {
		return 1;
	}
}

//Function that executes system command
void execArg(char** parsed){
	//fork child
	int i = fork();
	
	if (i == -1) {
		printf("\nFailed to fork child");
		return;
	}
	else if (i == 0) {
		if (execvp(parsed[0], parsed) < 0) {
			printf("\nFailed to execute command, type 'help' for command list");
		}
		exit(0);
	}
	//wait to for child to end
	else {
		wait(NULL);
		return;
	}
}

//Executes Piped commands
void execPipedArg(char** parsed, char** parsedpipe) { 

    int pipefd[2];  
    pid_t p1, p2; 
  
    if (pipe(pipefd) < 0) { 
        printf("\nPipe could not be initialized"); 
        return; 
    } 
    p1 = fork(); 
    if (p1 < 0) { 
        printf("\nCould not fork"); 
        return; 
    } 
  
    if (p1 == 0) { 
        // Child 1 executing
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]); 
  
        if (execvp(parsed[0], parsed) < 0) { 
            printf("\nCould not execute command 1.."); 
            exit(0); 
        } 
    } 
    else { 
        // Parent executing 
        p2 = fork(); 
        if (p2 < 0) { 
            printf("\nCould not fork"); 
            return; 
        } 
        // Child 2 executing 
        if (p2 == 0) { 
            close(pipefd[1]); 
            dup2(pipefd[0], STDIN_FILENO); 
            close(pipefd[0]); 
            if (execvp(parsedpipe[0], parsedpipe) < 0) { 
                printf("\nCould not execute command 2.."); 
                exit(0); 
            } 
        } 
        else { 
            // parent executing
            wait(NULL); 
            wait(NULL); 
        } 
    } 
} 

//function that executes built in commands
int bICmd(char** parsed) {

	int cmdCount = 5;
	int switchArg = 0;
	char* cmdList[cmdCount];
	char* username = getenv("USER");
	int r;
	pid_t mypid;
	void *mem;
	
	cmdList[0] = "exit";
	cmdList[1] = "cd";
	cmdList[2] = "help";
	cmdList[3] = "user";
	//Not Working
	cmdList[4] = "clone";

	for (int i = 0; i < cmdCount; i++) {
		if (strcmp(parsed[0], cmdList[i]) == 0) {
			switchArg = i + 1;
			break;
		}
	}	
	switch (switchArg) {
	case 1:
		printf("\nSee you next time!\n");
		exit(0);
	case 2:
		chdir(parsed[1]);
		return 1;
	case 3:
		openHelp();
		return 1;
	case 4:
		printf("\n\nCurrent USER is: @%s", username);
		return 1;
	//Not Working
	case 5:
		/*
		m = alloca(sysconf(_SC_PAGESIZE)) + sysconf(_SC_PAGESIZE);
		mypid = clone(childFunc, m, SIGCHLD | CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWUSER | CLONE_NEWNET, NULL);
		while (waitpid(mypid, &r, 0) < 0 && errno == EINTR) {
			continue;
		}
		if (WIFEXITED(r)) {
		return WEXITSTATUS(r);
		}
		*/
		return 1;
	default:
		break;
	}
	return 0;
}

void parseCmd(char* str, char** parsed) {
	for(int i = 0; i < MAXLIST; i++) {
		parsed[i] = strsep(&str, " ");
		if(parsed[i] == NULL)
			break;
		if(strlen(parsed[i]) == 0)
			i--;
	}
}

//finds pipe
int parsepipe(char* str, char** pipedstr) {
	for(int i = 0; i < 2; i++) {
		pipedstr[i] = strsep(&str, "|");
		if(pipedstr[i] == NULL)
			break;
	}
	//pipe not found
	if(pipedstr[1] == NULL)
		return 0;
	else
		return 1;
}

//Function to process string
int processStr(char* str, char** parsed, char** ppipe) {
	char* strpiped[2];
	int piped = 0;
	piped = parsepipe(str, strpiped);

	if (piped) {
		parseCmd(strpiped[0], parsed);
		parseCmd(strpiped[1], ppipe);
	}
	else {
		parseCmd(str, parsed);
	}
	if (bICmd(parsed))
		return 0;
	else
		return 1 + piped;
}


void main() {
	startPrompt();
	char inputStr[MAXCOM];
	char* parsedArgs[MAXLIST];
	char* parsedPipedArgs[MAXLIST];
	int exFlag = 0;

	while(1){
		curDir();
		if(takeIn(inputStr))
			continue;
		exFlag = processStr(inputStr, parsedArgs, parsedPipedArgs);

		if(exFlag == 1)	
			execArg(parsedArgs);
		if(exFlag == 2)
			execPipedArg(parsedArgs, parsedPipedArgs);
	}
	return;
}
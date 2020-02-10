---------------------------------------------------------------------------

							HamShell Mini-Shell
							  By Nate Hamling

---------------------------------------------------------------------------

Run Instructions:

	- Compile using 'gcc shell.c -lreadline'.
	- Run shell using './a.out'.

Command List:

	cd <arg> - Change current directory to <arg>.

	exit - Quits out of HamShell and returns user to Linux terminal.

	help - Prints command list.

	user - Prints the user name of the current user. 

	clone <Not Working>

	**All Linux commands also work within HamShell**

Helper Functions:

	childFunc() <Not Working> - child function for cloning commands.

	startPrompt() - Clears terminal screen and provides a display when
					opening the shell.

	curDir() - Gets the current working directory and prints it.

	openHelp() - Prints out the shell help menu. 

	takeIn(char* str) - Reads in and temporarily stores user input in 
						history. 

	execArg(char** parsed) - Executes system commands by forking a child 
							 and calling execvp on a parsed command. 

	execPipedArg(char** parsed, char** parsedpipe) - Executes piped 
													 commands when they are present.

	bICmd(char** parsed) - takes in a parsed command and executes the 
						   corresponding built in command. 

	parseCmd(char* str, char** parsed) - parses users command str and 
										 stores it in parsed.

	parsepipe(char* str, char** pipedstr) - Finds and parses piped 
											commands by locating the "|" indicator.

	processStr(char* str, char** parsed, char** ppipe) - 
		Utilizes the parsepipe(), parseCmd, and bICmd() functions to process strings. 

void main() process:
	
	- Start off by running the start prompt and creating variables for the input string, parsed arguments, piped parsed arguments, and execute flag. 

	- We then run an endless while loop until the command is given to exit the shell. 

	- At each iteration of the loop, we display the current working directory. We then take in the users imput and process the commands. If no pipes are found, we run our normal execArg() function on the parsed arguments and if pipes are found, we run execPipedArg on the piped and non-piped parsed arguments. 

	- This process is repeated until the shell is terminated. 

Disfunctional Code:

	- Everything in shell.c operates correctly except for the clone 
	  feature. All of the cloning code compiles without errors, but does not behave as expected within the shell. I spent alot of time editing this code to try to make it function properly, but could not finish it before the deadline. 



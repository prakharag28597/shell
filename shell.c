// C Program to design a shell in Linux
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

#define clear() printf("\033[H\033[J")
// function to initiallize the shell
void initialize_shell(){
	clear();
	printf("*******************************\n");
	printf("\n");
	printf("\n");
	printf("***********MY SHELL************\n");
	printf("\n");
	printf("\n");
	char* user=getenv("USER");
	printf("User: @%s\n",user);
	printf("\n");
	printf("\n");
	printf("*******************************\n");	
}

// prints the current directory
void current_dir(){
	printf("%s",getcwd(NULL, NULL));
}

// function to take input returns 0 if input is read else 1
int take_input(char* str){
	char *command;
 	command=readline("$ ");
 	if(strlen(command) !=0){
 	        strcpy (str,command);
 		//readline library command that saves the command
 		add_history(command);
 		free(command);
 		return 0;
 	}else{
 		free(command);
 		return 1;
 	}
}

//help function
void helper_function(){
	 puts("\n***WELCOME TO MY SHELL HELP***"
        "\nCopyright @prakhar"
        "\n-Use the shell at your own risk..."
        "\nList of Commands supported:"
        "\n>cd"
        "\n>ls"
        "\n>exit"
        "\n>all other general commands available in UNIX shell"
        "\n>pipe handling"
        "\n>improper space handling");
	
}

// function to execute user commands
int user_command(char **command_arr){
	int i,no_of_commands=4;
	char *command_list[no_of_commands];
	char *username;
	command_list[0]="exit";
	command_list[1]="help";
	command_list[2]="cd";
	command_list[3]="hello";
	for(i=0;i<no_of_commands;++i){
		if(strcmp(command_list[i], command_arr[0])==0)
		break;
	}
	switch(i){
		case 0:printf("Bye!!\n");
			exit(0);
		case 1:
		helper_function();
			return 1;
		case 2:chdir(command_arr[1]);
			return 1;
		case 3:username=getenv("USER");
			printf("User is %s\n",username);
			return 1;
		default:
			break;
	}
	return 0;
}

// function to separate space
void parse_space(char *str, char **parsed){
	int i;
	for(i=0;i<1000;++i){
		parsed[i]=strsep(&str, " ");
		if(parsed[i]==NULL)
		break;
	}
}
//function to separate on pipe | 
int parse_pipe(char *command, char **parsed_command_arr){
	//maximum 1 pipe allowed
	int i;
	for(i=0;i<2;++i){
		//strsep function used to separate wid | as the delimiter
		parsed_command_arr[i]=strsep(&command,"|");
		if(parsed_command_arr[i]==NULL)
		break;
	}
	// strsep help-https://stackoverflow.com/questions/6865963/strsep-usage-and-its-alternative
	// if no pipe is found return 0 else 1
	if(parsed_command_arr[1]==NULL)
	return 0;
	else 
	return 1;
}

// function to parse command
int parse_command(char *command, char **parsed_first, char **parsed_second){
	char* command_piped[2];
	int piped=parse_pipe(command, command_piped);
	if(piped){
		//parse first part
		parse_space(command_piped[0],parsed_first);
		//parse second part
		parse_space(command_piped[1],parsed_second);
	}else{
		parse_space(command,parsed_first);
	}
	if(user_command(parsed_first))
		return 0;
	else 
		return piped +1;
}

// function to execute simple commands
void execute_simple_function(char** parsed_first){
	int pid=(int)fork();
	if(pid<0)
		printf("Problem while forking\n");
	else if(pid==0)
	{
		printf("%s>>",parsed_first[0]);
		if(execvp(parsed_first[0],parsed_first)<0)
		printf("Error!\n");
		
		exit(0);
	}else{
		wait(NULL);
	}
}

//function to execute piped commands
void execute_piped_commands(char **parsed_first, char **parsed_second){
	//printf("hi%s \n",parsed_first[0]);
	//printf("hi%s \n",parsed_second[0]);
	//file descriptor 
	// 0 is for read 
	// 1 is for write
	int fd[2];
	if(pipe(fd)<0)
	{
		printf("Pipe could not be created\n");
		return;
	}
	int p1=(int)fork();
	//printf("%d\n",(int)p1);
	if(p1<0)
	{
		printf("Could not fork\n");
		return;
	}

	if(p1==0){
		// first child
		// executes the first command
		// this part writes
		//wait(NULL);
		//printf("Child 1}}}");
		close(fd[0]);
		dup2(fd[1],STDOUT_FILENO);
		close(fd[1]);
		//printf("Child 1%s",parsed_first[0]);
		if(execvp(parsed_first[0],parsed_first)<0){
			printf("Could not execute first command\n");
			exit(0);
		}
	}else{
		//Parent
		// executes the second part
		//printf("IN PARENT");
		wait(NULL);
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		//printf("Child 2");
		if(execvp(parsed_second[0],parsed_second)<0){
			printf("Could not execute second command\n");
			exit(0);
		}
		/*pid_t p2=fork();
		if(p2<0)
		{
			printf("Could not fork\n");
			return;
		}
		//child 2 executing 
		// this only reads
		// at read end at fd 0
		if(p2==0){
		printf("Child 17777");
			close(fd[1]);
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			printf("Child 2");
			if(execvp(parsed_second[0],parsed_second)<0){
				printf("Could not execute first command\n");
			
				exit(0);
			}
		}
		else{
			//Parent process waits for both the process to end
			wait(NULL);
			//wait(NULL);
		}*/

	}
}

int main(){
	//intializing the shell
	initialize_shell();
	char command[100];
	char *parsed_first[100]={NULL},*parsed_second[100];
	int flag_command;
	while(1){
		// print the current directory
		current_dir();
		//taking the input using readline and adding it to history
		if(take_input(command))
		continue;
		//parsing the string detecting for pipes
		//printf("COmmand%s\n",command);
		flag_command=parse_command(command,parsed_first,parsed_second);	
		// flag_command is 0 for own or built in commands
		// flag_command is 1 for simple commands
		// flag_command is 2 for piped commands
		//printf("%d",flag_command);
		if(flag_command==0)
			continue;
		else if(flag_command==1)
			execute_simple_function(parsed_first);
		else 
			execute_piped_commands(parsed_first, parsed_second);
	}

}

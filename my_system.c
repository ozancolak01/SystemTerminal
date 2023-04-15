#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

void logChild(pid_t childPID, char* command, int childNo);
void handleSigChild(int sig);

int mySystem(char *command) {
    int comCount;
    int i = 0;
    pid_t childPid;
    
    char *commands[20];
    
    struct sigaction sa;
    memset (&sa, 0, sizeof(sa));
    sa.sa_handler = &handleSigChild;
    sa.sa_flags = SA_RESTART;
    
    //Tokenizing the given command
    char *token = strtok(command, "|");
    commands[i++] = token;
    while(token != NULL && i < 20){
    	token = strtok(NULL, "|");
    	commands[i++] = token;
    }

    comCount = i-1;
    commands[comCount] = NULL;
    pid_t pids[comCount];
    
    //Creating pipes for all commands
    int fd[comCount][2];
    for(i = 0; i < comCount; i++)
	    if(pipe(fd[i]) == -1){
	    	printf("Pipe error!\n");
	    	return -1;
	    }
    
    
    //Start of forking and executing
    for(i = 0; i < comCount; i++){
    	pids[i] = fork();
    	
    	if(pids[i] == -1){
    		printf("Fork error!\n");
    		return -1;
    	}

    	if(pids[i] == 0){
    		//Some signal preperation for childs.
    		if (sigemptyset(&sa.sa_mask) == -1){
    		    printf("Signal handlers failed.\n");
    		}
    		else{
    		    sigaction(SIGINT, &sa, NULL);
    		    sigaction(SIGTSTP, &sa, NULL);
    		    sigaction(SIGTERM, &sa, NULL);
    		    sigaction(SIGQUIT, &sa, NULL);
    		}
		
		//closing every file except those will be used.
    		for(int j = 0; j < comCount; j++){
    			if(i != j)
    				close(fd[j][0]);
    			if(i + 1 != j)
    				close(fd[j][1]);
    		}
    		
    		//If there is only one command
	    	if(comCount == 1){
	    		close(fd[i][0]);
	    		
	    	}
	    	
	    	//If it is the first command
	    	else if(i==0){
	    		close(fd[i][0]);
	    		dup2(fd[i+1][1], STDOUT_FILENO);
	    		close(fd[i+1][1]);
	    	}
	    	
	    	//If it is the last command
	    	else if(i==comCount-1){
	    		dup2(fd[i][0], STDIN_FILENO);
	    		close(fd[i][0]);
	    		
	    	}
	    	
	    	//If it is one of the middle commands
	    	else{
	    		dup2(fd[i][0], STDIN_FILENO);
	    		dup2(fd[i+1][1], STDOUT_FILENO);
	    		close(fd[i][0]);
	    		close(fd[i+1][1]);
	    	}
	    	
	    	logChild(getpid(), commands[i], i+1);
	    	execl("/bin/sh", "sh", "-c", commands[i], (char *) NULL);
    	}
    	
    }

    for(i = 0; i < comCount; i++){
    	close(fd[i][0]);
    	close(fd[i][1]);
    }
    		
    for (i = 0; i < comCount; i++) {
    	wait(NULL);
    }
    
    
    return 1;
}

void logChild(pid_t childPID, char* command, int childNo){
	char filename[50];
	char endStr[8];
	time_t t;
	struct tm *timeinfo;
	
	time(&t);
	timeinfo = localtime(&t);
	strftime(filename, sizeof(filename), "log_%Y-%m-%d_%H-%M-%S", timeinfo);
	sprintf(endStr,  "_p%d.txt", childNo);
	strcat(filename, endStr);
	FILE *logFile = fopen(filename, "a");
	
	fprintf(logFile, "PID: %d, Command: %s\n", childPID, command);
	
	fclose(logFile);
}

void handleSigChild(int sig){
    if(sig==2)
	printf("\nSIGINT received. All child processes has been terminated.\n");
	
    else if(sig==3)
	printf("\nSIGQUIT received. All child processes has been terminated.\n");
	
    else if(sig == 15)
	printf("\nSIGTERM received. All child processes has been terminated.\n");
	
    else if(sig == 20)
	printf("\nSIGSTP received. All child processes has been terminated.\n");
	
    else
	printf("Unknown signal catched.\n$ ");
	
    fflush(stdout);
    kill(getpid(), SIGKILL); //Killing the child process.
}

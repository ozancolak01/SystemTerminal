#include <sys/wait.h>
#include "my_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


void handleSig(int sig);

int main(int argc, char *argv[]){
    if(argc != 1){
    	printf("\nError of program usage.\nUsage: ./terminal\n");
    	exit(-1);
    }
    char str[250];
    int status;
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handleSig;
    sa.sa_flags = SA_RESTART;
    if (sigemptyset(&sa.sa_mask) == -1){
    	printf("Signal handlers failed.\n");
    }
    else{
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
    }

    while(1){
	printf("$ ");
	fflush(stdout);
	if(fgets(str, 250, stdin)==NULL)
	    break;
	else if(strcmp(str, ":q\n") == 0)
	    break;
		
	status = mySystem(str);
		
	if(status == -1){
	    printf("Process has encountered an error!\n");
	    exit(-1);
	}
    }
    
    exit(0);
}

void handleSig(int sig){
    if(sig==2)
	printf("\nYou can not terminate with interrupt (SIGINT) signal.\n$ ");
	
    else if(sig==3)
	printf("\nYou can not terminate with quit (SIGQUIT) signal.\n$ ");
	
    else if(sig == 15)
	printf("\nYou can not terminate with terminate (SIGTERM) signal.\n$ ");
	
    else if(sig == 20)
	printf("\nYou can not terminate with stop (SIGTSTP) signal.\n$ ");
	
    else
	printf("Unknown signal catched.\n$ ");
	
    fflush(stdout);
}

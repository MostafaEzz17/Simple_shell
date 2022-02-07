/**
 * Basic shell program to execute commands based on linux OS structure
 * */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>


void sigHandler();
int countWords(char* buffer);
void prepareCmdV(char *pcmdv[],int words, char *cmdatring);

int main() {
    signal(SIGCHLD, sigHandler);
    // Initializing a Buffer to store the command line arguments in
    size_t n = 1;
    char* cmdBuffer= malloc(n*sizeof(char));
    pid_t pid;
    bool parentWait = true;

    if(cmdBuffer == NULL){
        printf("Unable to allocate buffer");
        exit(1);
    }

    while(1){
        getline (&cmdBuffer, &n, stdin); // Read the command from shell
        if(cmdBuffer == NULL){
            printf("Unable to allocate buffer");
            exit(1);
        }
        if (strcmp(cmdBuffer , "exit\n")){ // Execute the command
            int cmdArgs =  countWords(cmdBuffer) +1 ; // count command arguments +1 (Null element)
            char* command[cmdArgs] ;                  // declaration of command vector
            prepareCmdV(command,cmdArgs, cmdBuffer);  // tranform the command from a string to vector of strings
            command[cmdArgs-1] = NULL;

            if (strcmp(command[cmdArgs-2] , "&")==0){
                command[cmdArgs-2] = NULL;
                parentWait = false;
            }

            pid = fork();

            if (pid < 0){
                exit(2);// Fork failure
            }else if (pid == 0){
                execvp(command[0],command);
            }else{
                if(parentWait)
                    wait(NULL);
            }
        }else{
            free(cmdBuffer); // free cmdBuffer from Memory
            exit(0);   // Exit shell
        }
    }
}

/* Build a pcmdv[] -pointer to command verctor - which each element of it points to a word in cmdstring
 * cmdstring contains the command as a string
 * */
void prepareCmdV(char *pcmdv[],int words, char *cmdstring) {
    int spaces = words -2; // -1 & - (null element),
    cmdstring[strlen(cmdstring) - 1] = '\0';
    int j = 0;
    pcmdv[j] = &cmdstring[0];
    j++;

    for (int i = 1; spaces > 0  ; i++){
        if (cmdstring[i] == ' '){
            pcmdv[j] = cmdstring+i+1 ;
            j++;
            spaces --;
        }
    }
    spaces = words -2;
    for (int i = 1; spaces > 0  ; i++){
        if (cmdstring[i] == ' '){
            cmdstring[i] = '\0';
            spaces --;
        }
    }
}

int countWords(char* buffer) {
    int words = 0;

    for (int i = 0; buffer[i] != '\n'; i++){
        if (buffer[i] == ' ')
            words ++;
    }
    return words + 1;
}
void sigHandler() {
    FILE *fp;
    fp = fopen("shellogger.txt","a");
    fprintf(fp,"child has terminated\n");
    fclose(fp);
}

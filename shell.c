#include  <stdio.h>
#include  <unistd.h>
#include  <stdlib.h>
#include  <signal.h>
#include  <string.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include  <fcntl.h>
#define   __KERNEL__
#include  <linux/stat.h>
#define  MAX_LINE  80

//signal handling for ctrl+c
void sig_handler (int signo)
{
    printf("\nosh>");
    fflush(stdout);
}

int  main(void)
{
    char *args[MAX_LINE/2 + 1];
    char *args1[MAX_LINE/2 + 1];
    char *args2[MAX_LINE/2 + 1];
    char *args3[MAX_LINE/2 + 1];
    char *args4[MAX_LINE/2 + 1];
    signal(SIGINT, sig_handler);
    int should_run  =  1;
    pid_t pid;
    pid_t pid2;
    
    while  (should_run)  {
        //allocating all array of pointers and set them equal to null
        int size;
        for (size = 0; size < MAX_LINE/2 + 1; size++) {
            args[size] = calloc(MAX_LINE, sizeof(char));
            args[size] = NULL;
            args1[size] = calloc(MAX_LINE, sizeof(char));
            args1[size] = NULL;
            args2[size] = calloc(MAX_LINE, sizeof(char));
            args2[size] = NULL;
            args3[size] = calloc(MAX_LINE, sizeof(char));
            args3[size] = NULL;
            args4[size] = calloc(MAX_LINE, sizeof(char));
            args4[size] = NULL;
        }
        
        printf("osh>");
        fflush(stdout);
        char myString[MAX_LINE];
        
        //get command from stdin using fgets and eliminate the unecessary newline
        fgets (myString, MAX_LINE, stdin);
        int len = strlen(myString);
        if( len > 0 && myString[len-1] == '\n')
            myString[len-1] = '\0';
        
        //separate input string into tokens and store in args[]
        int i = 0;
        char *temp;
        temp = strtok(myString," ");
        while (temp != NULL) {
            args[i++] = temp;
            temp = strtok (NULL," ");
        }
        
        //if there is redirection simbol, put all the tokens for cmd1 into args1
        //otherwise put all the tokens to args1
        int j;
        int background = 0;
        char *reDirect1 = "";
        
        for (j = 0; args[j] != NULL; j++){
            if (!strcmp(args[j],">")){
                reDirect1 = ">";
                break;
            }
            else if (!strcmp(args[j],"1>")){
                reDirect1 = "1>";
                break;
            }
            else if (!strcmp(args[j],"2>")){
                reDirect1 = "2>";
                break;
            }
            else if (!strcmp(args[j],">>")){
                reDirect1 = ">>";
                break;
            }
            else if (!strcmp(args[j],"2>>")){
                reDirect1 = "2>>";
                break;
            }
            else if (!strcmp(args[j],"&>")){
                reDirect1 = "&>";
                break;
            }
            else if (!strcmp(args[j],"<")){
                reDirect1 = "<";
                break;
            }
            else if (!strcmp(args[j],"|")){
                reDirect1 = "|";
                break;
            }
            else if (!strcmp(args[j],"&")){
                background = 1;
                break;
            }
            else
                args1[j] = args[j];
        }
        
        //if there is redirection symbol, put all the tokens for cmd2 into args2
        int k = 0;
        j++;
        for (; args[j] != NULL; j++){
            if (!strcmp(args[j],"&")){
                background = 1;
                break;
            }
            else
                args2[k++] = args[j];
        }
        
        //if the input is ctrl+d terminates the program
        if (feof(stdin))
            break;
        
        
        //fork() & exec() & redirect & pipe
        //depend on weather there is a redirection symbol or pipe symbol
        //child process and parent process act differently
        
        
        int file;
        pid = fork();
        if (pid < 0){
            printf("Fork Failed\n");
        }
        else if (pid == 0) { /* child process */
            if (!strcmp(reDirect1,"")){
                execvp(args1[0], args1); //execute command
            }
            else if (!strcmp(reDirect1,">") || !strcmp(reDirect1,"1>")){
                file = open(args2[0], O_CREAT | O_WRONLY , S_IRUSR | S_IWUSR | S_IXUSR);
                dup2(file,1);
                execvp(args1[0], args1); //execute command
            }
            else if (!strcmp(reDirect1,"2>")) {
                file = open(args2[0], O_CREAT | O_WRONLY , S_IRUSR | S_IWUSR | S_IXUSR);
                dup2(file,2);
                execvp(args1[0], args1); //execute command
            }
            else if (!strcmp(reDirect1,">>")) {
                file = open(args2[0], O_CREAT | O_WRONLY | O_APPEND , S_IRUSR | S_IWUSR | S_IXUSR);
                dup2(file,1);
                execvp(args1[0], args1); //execute command
            }
            else if (!strcmp(reDirect1,"2>>")) {
                file = open(args2[0], O_CREAT | O_WRONLY | O_APPEND , S_IRUSR | S_IWUSR | S_IXUSR);
                dup2(file,2);
                execvp(args1[0], args1); //execute command
            }
            else if (!strcmp(reDirect1,"&>")) {
                file = open(args2[0], O_CREAT | O_WRONLY , S_IRUSR | S_IWUSR | S_IXUSR);
                dup2(file,1);
                dup2(file,2);
                execvp(args1[0], args1); //execute command
            }
            else if (!strcmp(reDirect1,"<")) {
                file = open(args2[0], O_CREAT | O_RDONLY , S_IRUSR | S_IWUSR | S_IXUSR);
                dup2(file,0);
                execvp(args1[0], args1); //execute command
            }
            else if (!strcmp(reDirect1,"|")) {
                int filedes2[2];
                pipe(filedes2);
                pid2 = fork();
                if (pid2 < 0){
                    printf("Fork2 Failed\n");
                }
                else if (pid2 == 0){
                    dup2(filedes2[1],1);
                    close(filedes2[0]);
                    execvp(args1[0], args1);
                }
                else {
                    dup2(filedes2[0],0);
                    close(filedes2[1]);
                    execvp(args2[0], args2);
                }
            }
            
        }
        else if (!background){ /* parent process */
            pid_t childpid;
            int status;
            childpid = wait(&status); //wait the child process to finish
            if (childpid < 0){
                printf("wait error");
                exit(1);
            }
        }
    }
    
    return  0;
}


#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
int main() 
{    
    pid_t pid;    
    int status;    
    char path[1000];    
    char *arg[100];     
    char line[1000];   
    char arrays[100][100]={""} ; 
    while(1) 
    {        
        int i = 0, j = 0, k=0, pos=0;   
        fgets(line, 1000, stdin);  
        int lenght = strlen(line); 
        if (lenght == 1)        
        {           
            return -1;       
        }        
        for (i = 0; i < lenght; i++)
        {           
            if (line[i] == ' '|| line[i] == '\0' || line[i] == '\n')      
            {               
                arrays[j][i-pos] = '\0';     
                pos=i+1;           
                arg[j]=arrays[j];   
                j++;            
                if(j>=99)  break ;
            }            
            else            
            {              
                arrays[j][i-pos] = line[i];  
            }       
            
        }
        arg[j]=NULL; 
        switch(pid = fork()) 
        {
            case -1:
                printf("fork error\n");
            case 0:
                execvp(arrays[0], arg);   
            default:
                wait(&status); 
                printf("Ret code: %d\n", WEXITSTATUS(status));
        } 
    }
    return 0;
    
}


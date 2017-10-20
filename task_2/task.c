/*
Write a duplex pipe implementation.
typedef struct {
  int txd[2]; 
  int rxd[2]; 
} dpipe_t;
1) This pipe should connect some process with his child, for continuous communication.
2) Be careful with opened descriptors.
3) When one process is terminated, the other should also exit.
*/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
 
typedef struct {
    int txd[2];
    int rxd[2];
} dpipe_t;
 
pid_t   childpid, parentpid;
 
void chld_sig_handler(int signo){
  if (signo == SIGCHLD) {
    printf("received SIGCHLD, aborting\n");
    exit(0);
  }
}
 
void main_sig_handler(int signo) {
    if (signo == SIGKILL || signo == SIGTERM) {
        printf("received terminating signal, killing child\n");
        kill(childpid, signo);
        exit(0);
    }
}
 
int main() {
    dpipe_t conn;
    int nbytes;
    char    string1[] = "Hello, parent!\n";
    char    string2[] = "Hello, son!\n";
    char    readbuffer[80];
 
   
    signal(SIGKILL, main_sig_handler);
    signal(SIGTERM, main_sig_handler);
    signal(SIGCHLD, chld_sig_handler);
   
    if(pipe(conn.txd) < 0) {
        printf("Can\'t create pipe txd\n");
        exit(1);
    }
    if(pipe(conn.rxd) < 0) {
        printf("Can\'t create pipe rxd\n");
        exit(1);
    }
   
    parentpid = getpid();
   
    if((childpid = fork()) == -1) {
        perror("fork");
        exit(1);
    }
   
    if(childpid == 0) {
        close(conn.txd[0]);
        write(conn.txd[1], string1, (strlen(string1)+1));
        close(conn.rxd[1]);
        memset(readbuffer, 0, sizeof(readbuffer));
        nbytes = read(conn.rxd[0], readbuffer, sizeof(readbuffer));
        printf("Received string from parent: %s", readbuffer);
        sleep(60);
        exit(0);
    } else {
        close(conn.txd[1]);
        memset(readbuffer, 0, sizeof(readbuffer));
        nbytes = read(conn.txd[0], readbuffer, sizeof(readbuffer));
        printf("Received string from child: %s", readbuffer);
        close(conn.rxd[0]);
        write(conn.rxd[1], string2, (strlen(string2)+1));
        sleep(60);
    }
       
   
    return 0;
}

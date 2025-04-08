#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]){
      int p[2];
      pipe(p);
      if(fork()==0){
            //child process
            int pid_child=getpid();
            char buffer[10];
            //read from the pipe
            if(read(p[0],buffer,1)==-1){
                  fprintf(1,"Error reading from the pipe.\n");
                  exit(1);
            }
            printf("%d:recived ping\n", pid_child);
            //write the byte to the parent via pipe
            close(p[0]); //close the read side of pipe instantly
            if(write(p[1],buffer,1)==-1){
                  fprintf(1,"Error writing to the pipe.\n");
                  exit(1);
            }
            close(p[1]);
            exit(0);

      }
      else{
            //parent process
            char info[2];
            info[0]='1'; 
            //send one byte to pipe
            if(write(p[1],info,1)==-1){
                  fprintf(1,"Error writing to the pipe.\n");
                  exit(1);
            }
            close(p[1]);//close the write side of pipe instantly

            //wait for child process to write to the pipe and the parent process to read from the pipe
            wait(0);//suspends execution of the calling thread until one of its children terminates. 
            char buffer[10];
            if(read(p[0],buffer,1)==-1){
                  fprintf(1,"Error reading from the pipe.\n");
                  exit(1);
            }
            int pid_parent=getpid();
            printf("%d: received pong\n",pid_parent);
            close(p[0]);
            exit(0);

      }
      exit(0);
}



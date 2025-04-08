#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void selector(int p[2]){
      close(p[1]);
      int prime;//读取的pipe中第一个数字就是本次要打印的prime
      //打印传入的第一个参数
      if(read(p[0],&prime,sizeof(prime))==0){
            //读取失败
            printf("Error reading from pipe: reading is closed.\n");
            exit(1);
      }
      printf("prime %d\n", prime);

      int flag=0;
      int n;
      //read函数返回值为读出的大小则为正确，返回值为0表示管道无法读出了
      if(read(p[0],&n,sizeof(n))==sizeof(n)){
            flag=1;
      }

      if(flag){
            //递归的终止条件：从上一个pipe中读不出数据了
            int newp[2];
            pipe(newp);//创建新的管道用于给新的子进程通信
            if(fork()==0){
                  //子进程
                  selector(newp);
            }
            else{
                  //父进程：写入下一个pipe
                  close(newp[0]);
                  if(n%prime!=0){
                        write(newp[1],&n,sizeof(n));
                  }
                  int num;
                  while(read(p[0],&num,sizeof(num))==sizeof(num)){
                        //检查能否被除，能的话则写入下一个pipe中
                        if(num%prime!=0){
                              //写入pipe
                              
                              if(write(newp[1],&num,sizeof(num))==-1){
                                    //写入失败
                                    printf("Error writing to pipe.\n");
                                    exit(1);
                              }

                        }
                  }
                  close(newp[1]);
                  close(p[1]);
                  wait(0);
                  exit(0);
            }
      }
      

      
}
int main(int argc, char* argv[]){
      int p[2];//pipeline：p[0]是读出，p[1]是写入
      pipe(p);
      if(fork()==0){
            //child process： 打印传入的第一个参数，检查传入的int中哪些不能被第一个数(prime)整除，传入下一个pipe
            selector(p);

      }
      else{
            //写入2-34
            close(p[0]);
            int arr[34];
            for(int i=0;i<34;i++){
                  arr[i]=i+2;
            }
            if(write(p[1], arr, sizeof(arr))==-1){
                  //写入失败
                  printf("Error writing to pipe.\n");
                  exit(1);
            }
            close(p[1]);
            
            wait(0);//等待子进程执行完
            exit(0);
      }

      exit(0);
}
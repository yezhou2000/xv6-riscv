#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
  char buf[512];
  int i;
  char *xargv[MAXARG];

  // 将原始命令参数拷贝到 xargv 中
  for (i = 1; i < argc; i++) {
    xargv[i - 1] = argv[i];
  }

  int base = i - 1; // 固定参数的数量
  int n = 0;
  char ch;

  // 用来读取当前输入的一行
  while (read(0, &ch, 1) == 1) {
    if (ch == '\n') {
      buf[n] = '\0';
      n = 0;

      // 把这行作为一个参数追加
      xargv[base] = buf;
      xargv[base + 1] = 0;

      // fork+exec 执行
      if (fork() == 0) {
        exec(xargv[0], xargv);
        fprintf(2, "exec failed\n");
        exit(1);
      } else {
        wait(0);
      }

    } else {
      buf[n++] = ch;
    }
  }

  exit(0);
}

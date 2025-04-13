#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *target)
{
      /*
      步骤	说明
      1	打开 <path> 目录
      2	遍历目录中的所有条目（用 read(fd, &de, sizeof(de))）
      3	如果是普通文件且文件名等于 <target_name>，就打印路径
      4	如果是目录，且不是 "." 或 ".."，就递归进入
      5	在子目录中重复查找 递归思想
      */
      char buf[512];
      char *p;
      int fd;
      struct dirent de;
      struct stat st;

      if ((fd = open(path, 0)) < 0)
      {
            fprintf(2, "find: cannot open %s\n", path);
            return;
      }

      if (fstat(fd, &st) < 0)
      {
            // fsstat gets file system info
            fprintf(2, "find: cannot stat %s\n", path);
            close(fd);
            return;
      }

      strcpy(buf, path);

      switch (st.type)
      {
      case T_FILE: // 处理当前路径是否是文件
            // 提取路径中最后一个/之后的内容，也就是要比较的文件名
            char *filename = path;

            for (p = path; *p != '\0'; p++)
            {
                  if (*p == '/')
                  {
                        filename = p + 1;
                  }
            }
            int tmp = strcmp(filename, target); // 相同返回0
            if (!tmp)
            {
                  printf("%s\n", buf);
            }
            break;

      case T_DIR:
            while (read(fd, &de, sizeof(de)) == sizeof(de))
            {
                  if (strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0 || de.inum == 0)
                  {
                        continue;
                  }

                  // 拼接新的路径：buf里存的是path，de.name是当前遍历到的文件名称
                  // 新的buf里存的是子文件的路径
                  // 方法一：snprintf函数
                  // snprintf(buf, sizeof(buf), "%s/%s", path, de.name);

                  // 方法二：手动实现拼接，得到path+de.name的子项路径
                  // char buf[512];
                  memmove(buf, path, strlen(path));
                  buf[strlen(path)] = '/';
                  memmove(buf + strlen(path) + 1, de.name, DIRSIZ);
                  buf[strlen(path) + 1 + DIRSIZ] = '\0'; // 终止符

                  // 需要重新stat获取子项信息
                  if (stat(buf, &st) < 0)
                  { // 获取buf路径的文件的状态，并保存在st结构体中
                        printf("find: cannot stat %s\n", buf);
                        continue;
                  }

                  if (st.type == T_DIR)
                  {
                        find(buf, target); // 进入迭代
                  }
                  else if (st.type == T_FILE && strcmp(de.name, target) == 0)
                  {
                        // 判断子文件是否为file
                        printf("%s\n", buf);
                  }
                  
            }
            break;
      }
}

int main(int argc, char *argv[])
{
      if (argc != 3)
      {
            // 合理的参数为3个，第零个是find程序名本身，第一个是寻找文件的路径starting path，第二个是target file
            printf("Error parameters.\n");
            exit(1);
      }
      char *path = argv[1];
      char *target = argv[2];
      find(path, target);
      exit(0);
}
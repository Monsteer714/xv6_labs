#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"


char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = '\0';
  return buf;
}

void
find(char *path, char *name)
{
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    
    if (st.type == T_DIR) {
        char buf[512], *p;
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("ls: path too long\n");
            return;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        //printf("this is dir %s\n", path);
        while ((read(fd, &de, sizeof(de)) == sizeof(de))) {
            if (de.inum == 0) {
                continue;
            }
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if ((stat(buf, &st) < 0)) {
                fprintf(2, "find: cannot stat %s\n", buf);
            } 
            if (st.type == T_DIR) {
                find(buf, name);
            }
            else if (st.type == T_FILE) {
                //printf("this is file %s %s\n", fmtname(buf), buf);
                if (strcmp(fmtname(buf), name) == 0) {
                    printf("%s\n", buf);
                    int ffd = -1;
                    if ((ffd = open(buf, O_RDONLY)) < 0) {
                        fprintf(2, "find: cannot open %s\n", buf);
                    } else {
                        char fbuf[512];
                        int read_bytes = 0;
                        int n;
                        while ((n = read(ffd, fbuf + read_bytes, sizeof(fbuf) - read_bytes)) > 0) {
                            read_bytes += n;
                        }
                        printf("read %d bytes from file %s\n", read_bytes, buf);
                        fbuf[read_bytes] = '\0';
                        printf("content of %s: %s\n", buf, fbuf);
                        close(ffd);
                    }
                }
            }
        } 
        return;
    }
}


int
main(int argc, char *argv[])
{
    if (argc <= 1) {
        fprintf(2, "Usage: find [path] [name]\n");
        exit(1);
    } else if (argc <= 2) {
        find(".", argv[1]);
    } else {
        find(argv[1], argv[2]);
    }
    printf("find ran\n");

    exit(0);
}

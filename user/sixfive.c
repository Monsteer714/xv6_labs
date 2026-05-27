#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const char* separators = "-\r\n/,";
const char* digits = "0123456789";

int isSeperator(char c) {
    return strchr(separators, c) == 0 ? 0 : 1;
}

int isDigit(char c) {
    return strchr(digits, c) == 0 ? 0 : 1;
}

int isMultipleOf(int num, int divisor) {
    return num % divisor == 0;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(2, "Usage: sixfive file...\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        char buf[1024];
        int fd = open(argv[i], 0);
        if (fd < 0) {
            fprintf(2, "sixfive: failed to open %s\n", argv[i]);
            exit(1);
        }

        int n = read(fd, buf, sizeof(buf));
        if (n < 0) {
            fprintf(2, "sixfive: failed to read %s\n", argv[i]);
            exit(1);
        }

        int num = 0;
        for (int j = 0; j < n; j++) {
            char c = buf[j];
            if (isSeperator(c)) {
                if (isMultipleOf(num, 6) || isMultipleOf(num, 5)) {
                    printf("%d\n", num);
                }
                num = 0;
            }
            else if (isDigit(c)) {
                num = num * 10 + c - '0';
            }
            else {
                num = 0;
            }
        }
        if (isMultipleOf(num, 6) || isMultipleOf(num, 5)) {
            printf("%d\n", num);
        }

        close(fd); 
    }
    exit(0);
}
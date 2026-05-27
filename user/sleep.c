//
// Created by Hanhong Wong on 2026/5/27.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(2, "Usage: sleep <seconds>....\n");
        exit(1);
    }

    int seconds = atoi(argv[1]);
    pause(seconds);

    exit(0);
}
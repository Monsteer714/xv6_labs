#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//
// Created by Hanhong Wong on 2026/5/28.
//
void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
    if(argc == 1){
        printf("Example 1:\n");
        int a[2] = { 61810, 2025 };
        memdump("ii", (char*) a);

        printf("Example 2:\n");
        memdump("S", "a string");

        printf("Example 3:\n");
        char *s = "another";
        //type of &s clearly is char**,so to get &s just need to cast data to char**
        //then *data,you could get s;
        memdump("s", (char *) &s);

        struct sss {
            char *ptr;
            int num1;
            short num2;
            char byte;
            char bytes[8];
        } example;

        example.ptr = "hello";
        example.num1 = 1819438967;
        example.num2 = 100;
        example.byte = 'z';
        strcpy(example.bytes, "xyzzy");

        printf("Example 4:\n");
        memdump("pihcS", (char*) &example);

        printf("Example 5:\n");
        memdump("sccccc", (char*) &example);

        printf("Example 7:\n");
        memdump("S", "abcdefgh12345678\0");
        printf("Example 6:\n");
        memdump("p", "abcdefgh12345678");
        printf("Example 8:\n");
        memdump("ii", "abcdefgh12345678");
    }
//    else if(argc == 2){
//        // format in argv[1], up to 512 bytes of data from standard input.
//        char data[512];
//        int n = 0;
//        memset(data, '\0', sizeof(data));
//        while(n < sizeof(data)){
//            int nn = read(0, data + n, sizeof(data) - n);
//            if(nn <= 0)
//                break;
//            n += nn;
//        }
//        memdump(argv[1], data);
//    } else {
//        printf("Usage: memdump [format]\n");
//        exit(1);
//    }
    exit(0);
};

void
memdump(char *fmt, char *data)
{
    void* temp = data;
    for (int i = 0; i < strlen(fmt); i++) {
        char c = fmt[i];
        if (c == 'i') {
            printf("%d\n", *((int*)temp));
            temp += sizeof(int);
        }
        if (c == 'h') {
            printf("%d\n", *((short*)temp));
            temp += sizeof(short);
        }
        if (c == 's') {
            printf("%s\n", *(char **)temp);
            temp += sizeof(char *);
        }
        if (c == 'p') {
            printf("%X\n", *((int*)temp));
            temp += sizeof(long);
        }
        if (c == 'c') {
            printf("%c\n", *((char *)temp));
            temp += sizeof(char);
        }
        if (c == 'S') {
            printf("%s\n", (char *)temp);
            temp += sizeof(char *);
        }
    }
}
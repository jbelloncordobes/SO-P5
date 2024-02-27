#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>   // for usleep waits for microsec
#include <unistd.h> // for sleep: waits for seconds
#include "crc.h"
#include "fileLock.h"
#define N 4

typedef struct {
    int nBlock;
    int isGet;
} Request;

typedef struct {
    int nBlock;
    short int crc;
} Result;


int main(int argc, char * argv[]) {
    int pipeA[2], pipeB[2];
    // Create two pipes. Also remember to close the channels when needed, otherwise it will not work!

    for (int i = 0; i < N; ++i) {
        int n = fork();
        if (n == 0) {
            int fd = open(argv[1], O_RDONLY);
            int fdCRC =open(argv[2], O_RDWR);
            Request r;
            while( /* Read a request from the pipe */) {
                if (!r.isGet) {

                    /* Recompute the CRC, use lseek to get the correct datablock,
                    and store it in the correct position of the CRC file. Remember to use approppriate locks! */
                    usleep(rand()%1000 *1000); // Make the computation a bit slower

                }
                else{
                    usleep(rand()%1000 *1000);
                    Result res;
                    res.nBlock = r.nBlock;
                    // Read the CRC from the CRC file, using lseek + read. Remember to use the correct locks!

                    //Write the result in pipeB!
                }
            }

            exit(0);
        }
    }
    char s[100];
    int nBytesRead;
    /* Read until the standard output*/
    while((nBytesRead = read(0, s, 100) ) > 0) {
        char op[200];
        s[nBytesRead] = '\0';
        int nBlock;

        sscanf(s, "%s %d", op, &nBlock);
        Request r;
        r.nBlock = nBlock;
        r.isGet = strcmp(op, "get") == 0;
        // Write r in the pipe!
    }

    printf("FINISHED\n");
    while(wait(NULL) == -1);

    // Now that is finished, write all the results
    Result res;
    while((nBytesRead = read(pipeB[0], &res, sizeof(res)) ) > 0) {
        printf("The CRC of block #%d is %d \n", res.nBlock, res.crc);
    }
}
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
#define N 1

typedef struct {
    int nBlock;
    int isGet;
} Request;

typedef struct {
    int nBlock;
    unsigned short crc;
} Result;


int main(int argc, char * argv[]) {
    int pipeA[2], pipeB[2];
    // Create two pipes. Also remember to close the channels when needed, otherwise it will not work!
    pipe(pipeA);
    pipe(pipeB);
    for (int i = 0; i < N; ++i) {
        int n = fork();
        if (n == 0) {
            close(pipeA[1]);
            close(pipeB[0]);
            int fd = open(argv[1], O_RDONLY);
            int fdCRC =open(argv[2], O_RDWR);
            Request r;
            int nBytesReadHijo;
            while( (nBytesReadHijo = read(pipeA[0], &r, sizeof(r)) ) > 0) {
                //printf("%d\n", nBytesReadHijo);
                int vamoAleer = 1; // para leer chill
                if (!r.isGet) {
                    // printf("Entramoooooooooos\n");
                    /* Recompute the CRC, use lseek to get the correct datablock,
                    and store it in the correct position of the CRC file. Remember to use approppriate locks! */
                    int correctDatablock = r.nBlock*256;
                    int correctCRCblock = r.nBlock*2;
                    off_t punteroFd = lseek(fd, correctDatablock, SEEK_SET);
                    off_t punteroFdCRC = lseek(fdCRC, correctCRCblock, SEEK_SET);
                    char buff[257];
                    file_lock_read(fd, correctDatablock, punteroFd+256);
                    vamoAleer = read(fd, buff, 256);
                    file_unlock(fd, correctDatablock, punteroFd+256);
                    //printf("%d - %s\n", strlen(buff), buff);
                    unsigned short crcBlockNum = crcSlow(buff, strlen(buff));
                    printf("%hu\n", crcBlockNum);
                    file_lock_write(fdCRC, r.nBlock, punteroFdCRC+2);
                    if (write(fdCRC, &crcBlockNum, sizeof(crcBlockNum)) == -1){
                        file_unlock(fdCRC, r.nBlock, punteroFdCRC+2);
                        printf("No estamos imprimiendo el crc bien ;(\n");
                    }
                    file_unlock(fdCRC, r.nBlock, punteroFdCRC+2);
                    // printf("Se ha imprimido en el fichero %s el CRC en el bloque %d ;)\n", argv[2], r.nBlock);
                    usleep(rand()%1000 *1000); // Make the computation a bit slower

                }
                else{
                    Result res;
                    res.nBlock = r.nBlock;
                    int correctCRCblock = r.nBlock*2;
                    // Read the CRC from the CRC file, using lseek + read. Remember to use the correct locks!
                    off_t punteroFdCRC = lseek(fdCRC, correctCRCblock, SEEK_SET);  
                    unsigned short crcBlockNumLect;
                    file_lock_read(fdCRC, r.nBlock, punteroFdCRC+2);
                    vamoAleer = read(fdCRC, &crcBlockNumLect, sizeof(unsigned short));
                    file_unlock(fdCRC, r.nBlock, punteroFdCRC+2);
                    //printf("El crc QUE TE LO DICE TU HIJITO es de: %hu\n", crcBlockNumLect);
                    //Write the result in pipeB!
                    res.crc = crcBlockNumLect;
                    write(pipeB[1], &res, sizeof(res));
                    usleep(rand()%1000 *1000);

                }
            }
            //printf("%d\n", nBytesReadHijo);
            printf("Terminado\n");
            close(pipeB[1]);
            close(fd);
            close(fdCRC);
            exit(0);
        }
    }
    close(pipeA[0]);
    close(pipeB[1]);
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
        write(pipeA[1], &r, sizeof(r));
    }

    printf("FINISHED\n");
    close(pipeA[1]); // Señal para que los hijos salgan del while
    while(wait(NULL) == -1);
    printf("Succesfully waited\n");
    // Now that is finished, write all the results
    Result res;
    while((nBytesRead = read(pipeB[0], &res, sizeof(res)) ) > 0) { //No hace este while, supongo que no escribe bien en la pipeB
        printf("The CRC of block #%d is %hu \n", res.nBlock, res.crc);
    }
}
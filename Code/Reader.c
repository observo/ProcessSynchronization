#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define SHARED_MEMORY_SIZE 536870912
#define BUFFER_SIZE 16384

int main(int argc, char* argv[]){
    if(argc==2){
        char* FileName=argv[1];
        struct MemData{
            sem_t FullMutex;
            sem_t FreeMutex;
            int ReadPointer;
            int WritePointer;
            int NumberOfFileBuffers;
            char Data[SHARED_MEMORY_SIZE];//MEMORY BLOCK SIZE: 500 KB
        };
        int SD;
        struct MemData *M;
        int BufferSize=BUFFER_SIZE;
        int NumberOfBuffers=SHARED_MEMORY_SIZE/BUFFER_SIZE;
        int BufferCount=0;
        
        SD= shm_open("/program.shared", O_RDWR|O_CREAT, S_IREAD|S_IWRITE);
        if(SD< 0){
            printf("\nshm_open() error \n");
            return EXIT_FAILURE;
        }
        M=(struct MemData*)mmap(NULL, sizeof(struct MemData), PROT_READ|PROT_WRITE, MAP_SHARED, SD, 0);
        if(M== MAP_FAILED){
            printf("mmap() error");
            return EXIT_FAILURE;
        }else{
            FILE *FP= fopen(FileName, "wb");
            if(FP!= NULL){
                //READ
                while(1){
                    sem_wait(&M->FullMutex);
                    int FreeMutexValue;
                    sem_getvalue(&M->FreeMutex, &FreeMutexValue);
                    int FullMutexValue;
                    sem_getvalue(&M->FullMutex, &FullMutexValue);
                    printf("\nMutexes-Free: %d and Full: %d", FreeMutexValue, FullMutexValue);
                    printf("\nBuffer Writing: %d", BufferCount);
                    printf("\nReadPointer: %d and WritePointer: %d", M->ReadPointer, M->WritePointer);
                    printf("\nBuffer Writing: %d\n", BufferCount);
                    fseek(FP, BufferCount*BufferSize, SEEK_SET);
                    fwrite(&M->Data[M->ReadPointer*BufferSize], sizeof(unsigned char), BufferSize, FP);
                    BufferCount++;
                    M->ReadPointer=(M->ReadPointer+1)%NumberOfBuffers;
                    sem_post(&M->FreeMutex);
                    if(BufferCount==M->NumberOfFileBuffers){
                        fclose(FP);
                        break;
                    }
                    //sem_post(&M->FreeMutex);
                }
            }else{
                printf("\n%s", "Please Give the Right Path Name.");
                close(SD);
                exit(0);
            }
            munmap(M,sizeof(struct MemData));
            close(SD);
        }
    }else{
        printf("\nThe Usage Command: %s", "./Reader FileName");
    }
    return 0;
}
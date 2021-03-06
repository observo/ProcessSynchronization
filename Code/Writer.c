#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
        if(access(FileName, F_OK)!= -1){
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
            unsigned char Buf[BufferSize];
            int BufferCount=0;
            int NumberOfFileBuffers=0;
            long long FilePosition=0;
            long long FileSize=0;
            long long CopyableMemorySize=0;

            SD= shm_open("/program.shared", O_RDWR|O_CREAT, S_IREAD|S_IWRITE);
            if(SD< 0){
                printf("\nshm_open() error \n");
                return EXIT_FAILURE;
            }
            fchmod(SD, S_IRWXU|S_IRWXG|S_IRWXO);
            if(ftruncate(SD, sizeof(struct MemData))< 0){
                printf ("ftruncate() error \n");
                return EXIT_FAILURE;
            }
            M=(struct MemData*)mmap(NULL, sizeof(struct MemData), PROT_READ|PROT_WRITE, MAP_SHARED, SD, 0);
            if(M== MAP_FAILED){
                printf("mmap() error");
                return EXIT_FAILURE;
            }else{
                FILE *FP= fopen(FileName, "rb");
                if(FP!= NULL){
                    struct stat StatBuf;
                    if(stat(FileName, &StatBuf)==-1){
                        printf("failed to fstat %s\n", FileName);
                        exit(EXIT_FAILURE);
                    }
                    FileSize=StatBuf.st_size;
                    printf("\nFile Size: %lld", FileSize);
                    FilePosition=ftell(FP);
                    FilePosition=ftell(FP);
                    CopyableMemorySize=FileSize-FilePosition;
                    printf("\nCopyable File Size: %lld", CopyableMemorySize);
                    int NumberOfFileBuffers=ceil(CopyableMemorySize/BufferSize);
                    printf("\nNumber Of File Buffers: %d\n", NumberOfFileBuffers);
                    
                    //INITIALIZATION
                    sem_init(&M->FullMutex, 1, 0);
                    sem_init(&M->FreeMutex, 1, NumberOfBuffers);
                    M->ReadPointer=0;
                    M->WritePointer=0;
                    M->NumberOfFileBuffers=NumberOfFileBuffers;
                    memset(M->Data, '\0', sizeof(M->Data));
                    
                    //WRITE
                    while(1){
                        sem_wait(&M->FreeMutex);
                        fseek(FP, BufferCount*BufferSize, SEEK_SET);
                        fread(Buf, sizeof(unsigned char), BufferSize, FP);
                        int FreeMutexValue;
                        sem_getvalue(&M->FreeMutex, &FreeMutexValue);
                        int FullMutexValue;
                        sem_getvalue(&M->FullMutex, &FullMutexValue);
                        printf("\nMutexes-Free: %d and Full: %d\n", FreeMutexValue, FullMutexValue);
                        printf("\nBuffer Writing: %d\n", BufferCount);
                        memcpy(&M->Data[M->WritePointer*BufferSize], Buf, BufferSize);
                        BufferCount++;
                        M->WritePointer=(M->WritePointer+1)%NumberOfBuffers;
                        sem_post(&M->FullMutex);      
                        if(BufferCount==M->NumberOfFileBuffers){
                            fclose(FP);
                            break;
                        }
                        //sem_post(&M->FullMutex);
                    }
                }
                close(SD);
            }
        }else{
            printf("\n%s", "Please Give the Right Path Name.");
            exit(0);
        }
    }else{
        printf("\nThe Usage Command: %s", "./Writer FileName");
    }
    return 0;
}
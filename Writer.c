#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(void){
    char FileName[128]="aaa.txt";
    struct MemData{
        sem_t FullMutex;
        sem_t FreeMutex;
        int ReadPointer;
        int WritePointer;
        int NumberOfFileBuffers;
        char Data[512000];//MEMORY BLOCK SIZE: 500 KB
    };
    int SD;
    struct MemData *M;
    int NumberOfBuffers=10;
    //int BufferSize=51200;//FILE BUFFER SIZE 50 KB
    int BufferSize=2;//EXPERIMENATION
    unsigned char Buf[BufferSize];
    int BufferCount=0;

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
            long long FileSize=StatBuf.st_size;
            printf("\nFile Size: %lld", FileSize);
            long long FilePosition=ftell(FP);
            FilePosition=ftell(FP);
            long long CopyableMemorySize=FileSize-FilePosition;
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
    return 0;
}
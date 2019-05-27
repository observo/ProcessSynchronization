#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char* argv[]){
    if(argc==2){
        char* FileName=argv[1];
        //SHARED MEMORY HANDLING VARIABLES
        struct MemData{
            sem_t FullMutex;
            sem_t FreeMutex;
            int ReadPointer;
            int WritePointer;
            int NumberOfFileBuffers;
            //char Data[512000];//MEMORY BLOCK SIZE: 500 KB
            char Data[536870912];
        };
        int SD;
        struct MemData *M;
        //int BufferSize=51200;//FILE BUFFER SIZE 50 KB
        //int BufferSize=2;//EXPERIMENATION
        int BufferSize=16384;
        unsigned char Buf[BufferSize];
        int BufferCount=0;
        int NumberOfFileBuffers=0;
        long long FilePosition=0;
        long long FileSize=0;
        long long CopyableMemorySize=0;
        
        //LINE HANDLING VARIABLES
        int KeyCount=0;
        int ENDPresent=0;
        char Line[128];
        char KVSeparator[]=":";
        int LineCount=0;
        const char* Keys[]={"AIRBUS GNSS", "Header version", "Receiver name", "Firmware version", "No. Channels", "Sample rate", "Channel bandwidth", "Frequency band", "Bitwidth", "IQ order", "Raw samples", "Timestamp", "Comment", "END"};
            
        char Delimeter=':';
        
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
                //READ LINES
                while((fgets(Line, sizeof(Line), FP)!= NULL)&& (LineCount<20)){
                    printf("\n%s", Line);
                    LineCount++;
                    char* Position=strchr(Line, ':');
                    if(Position!=NULL){
                        char *Splice = strtok(Line, KVSeparator);
                        //STOP READING LINES IF 'END' IS READ IN A LINE
                        if(strcmp(Splice, "END")==0){
                            ENDPresent=1;
                            break;
                        }
                            
                        //COMPARE WITH KEYWORDS USUALLY COME IN AS HEADER
                        int i=0;
                        while(Keys[i]){
                            if(strcmp(Keys[i], Splice) == 0){
                                KeyCount++;
                                break;
                            }
                            i++;
                        }
                    }
                }
                //fputs(Splice, stdout);
                    
                //CHECK VALID FILE
                if(KeyCount>0 && ENDPresent){
                    printf("\n%s", "This is a valid file.");
                    FilePosition=ftell(FP);
                        
                    struct stat StatBuf;
                    if(stat(FileName, &StatBuf)==-1){
                        printf("failed to fstat %s\n", FileName);
                        exit(EXIT_FAILURE);
                    }
                    FileSize=StatBuf.st_size;
                    printf("\n File Size: %lld", FileSize);
                    CopyableMemorySize=FileSize-FilePosition;
                    printf("\nCopyable File Size: %lld", CopyableMemorySize);
                    NumberOfFileBuffers=ceil(CopyableMemorySize/BufferSize);
                    printf("\nNumber Of File Buffers: %d\n", NumberOfFileBuffers);
                }else{
                    printf("\n%s\n", "This is an Invalid File.");
                    exit(0);
                }
                                
                //INITIALIZATION
                sem_init(&M->FullMutex, 1, 0);
                sem_init(&M->FreeMutex, 1, NumberOfFileBuffers);
                M->ReadPointer=0;
                M->WritePointer=0;
                M->NumberOfFileBuffers=NumberOfFileBuffers;
                memset(M->Data, '\0', sizeof(M->Data));
                
                //WRITE
                while(1){
                    sem_wait(&M->FreeMutex);
                    long long Position=BufferCount*BufferSize+FilePosition;
                    printf("Position:%lld", Position);
                    fseek(FP, Position, SEEK_SET);
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
                    sleep(0.03);// WAIT, UNNECESSARY THOUGH
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
        printf("\nThe Usage Command: %s", "./Writer aaa.txt");
    }
    return 0;
}
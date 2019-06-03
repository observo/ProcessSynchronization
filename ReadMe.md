This is Process Synchronization in C using Reader-Writer Paradigm.
We have used Shared Memory(/program.shared) and Ring Buffer.
Good Software requires to understand Lock and Synchronization very well.

Usually we need to Protect Data. If Multiple Processes of Same Types(Readers/Writers), we need to Protect Code too.
We have not Handled Race and Deadlock since this is a Single Reader, Single Writer arrangement.
Race happens on same side(Multiple Readers or Multiple Writers).
Deadlock happens on any side or both sides.
As there is no Deadlock Handling, we have not identified Critical Section or Mutual Exclusion.
Critical Section is basically Protecting Code from Multiple Accesses Simultaneously.
Only One Process can enter into Critical Section and this is called Mutual Exclusion.


For a Detail Description See http://web.mit.edu/6.031/www/sp19/classes/21-locks/


The HeaderSkippingWriter.c uses Time Waiting.


These Programs will run on Linux and Mac. We have checked on Linux Ubuntu Distribution.

The aaa.txt has No Header.
The ccc.txt has Header.
The xaa has Header.


Define SHARED_MEMORY_SIZE and BUFFER_SIZE as per your requirements in Programming Source Files.


Open a Terminal in Linux(Ubuntu) by pressing Alt+Ctl+T.

Go to Code Directory.

The Compilation Commands are-
For Whole File Copying-

 
    g++ Writer.c -o Writer -lpthread -lrt
    g++ Reader.c -o Reader -lpthread -lrt
    
Run 

    ./Writer FileName(Give the Right Path)

    
Or for Header Skipping Copying-

    
    g++ HeaderSkippingWriter.c -o Writer -lpthread -lrt
    g++ Reader.c -o Reader -lpthread -lrt
    
Run 

    ./Writer FileName(Give the Right Path)

The Reader is same for both types of Writing.    
Now open another Terminal  by pressing Alt+Ctl+T.
Run there-

    ./Reader FileName(Give the Right Path)
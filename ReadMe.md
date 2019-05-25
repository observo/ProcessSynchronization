This is Process Synchronization in C using Reader-Writer Paradigm.
We have used Shared Memory(/program.shared) and Ring Buffer.
Good Software requires to understand Lock and Synchronization very well.
See http://web.mit.edu/6.031/www/sp19/classes/21-locks/


Open a Terminal in Linux(Ubuntu) by pressing Alt+Ctl+T.
The Compilation Commands are-
For Whole File Copying-

 
    g++ Writer.c -o Writer -lpthread -lrt
    g++ Reader.c -o Reader -lpthread -lrt
    
    
Or for Header Skipping Copying-

    
    g++ AfterHeaderWriter.c -o Writer -lpthread -lrt
    g++ Reader.c -o Reader -lpthread -lrt
    
Run 

    ./Writer aaa.txt

Now open another Terminal  by pressing Alt+Ctl+T.
Run there 

    ./Reader bbb.txt
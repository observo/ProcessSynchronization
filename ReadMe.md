This is Process Synchronization in C using Reader-Writer Paradigm.
We have used Shared Memory(/program.shared) and Ring Buffer.
Good Software requires to understand Lock and Synchronization very well.
See http://web.mit.edu/6.031/www/sp19/classes/21-locks/

The aaa.txt has No Header.
The ccc.txt has Header.
The xaa has Header.

Open a Terminal in Linux(Ubuntu) by pressing Alt+Ctl+T.

Go to Code Directory.

The Compilation Commands are-
For Whole File Copying-

 
    g++ Writer.c -o Writer -lpthread -lrt
    g++ Reader.c -o Reader -lpthread -lrt
    
Run 

    ./Writer ../Files/aaa.txt

    
Or for Header Skipping Copying-

    
    g++ HeaderSkippingWriter.c -o Writer -lpthread -lrt
    g++ Reader.c -o Reader -lpthread -lrt
    
Run 

    ./Writer ../Files/ccc.txt or ./Writer ../Files/xaa

The Reader is same for both types of Writing.    
Now open another Terminal  by pressing Alt+Ctl+T.
Run there-

    ./Reader ../Files/bbb.txt
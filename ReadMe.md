This is Process Synchronization in C using Reader-Writer Paradigm.
We have used Shared Memory(/program.shared) and Ring Buffer.

Open a Terminal in Linux(Ubuntu) by pressing Alt+Ctl+T.
The Compilation Commands are-

    g++ Writer.c -o Writer -lpthread -lrt
    g++ Reader.c -o Reader -lpthread -lrt
    
Run 

    ./Writer

Now open another Terminal  by pressing Alt+Ctl+T.
Run there 

    ./Reader
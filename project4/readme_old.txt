to compile:
g++ main.cpp -o main.out -pthread
to run:
./main.out inputs_small.txt

to compile query:
g++ query.cpp -o query.out
to run query:
./query.out [some compressed file.txt] [key value]

some observations to comment on:
multithreading does not seem to have much of an impact on the compression time
    overhead is high: need to split up files, dispatch threads, write output files then recombine
    process is mostly constrained by input and output writing speeds (moving memory around between ram and ssd)
frequency dictionary compression does notn make a big difference in compression Size    
    takes more time
    the files that we are compressing have small words, so there is not a lot of space that is being saved
    would be better for files with very frequent large words 
compare to zstd?
was tested using larger files where each line was copy and pasted 3 extra times
    this improve compression ratio substantially
    these are mod small and mod medium
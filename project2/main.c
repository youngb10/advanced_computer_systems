#include <stdlib.h>   // For malloc, free, exit
#include <stdio.h>    // For printf, perror, fopen, etc.
#include <string.h>   // For strlen, strcat, memset, strerror
#include <pthread.h>  // For multithreading
#include <zstd.h>     // For compressing
#define SRCSIZE 16384 // Size of uncompressed block
#define MAXSIZE 16504 // Max size of compressed block

size_t comp_size[14888] = {0};


static void* malloc_orDie(size_t size)
{
    void* const buff = malloc(size);
    if (buff) return buff;
    /* Error */
    perror("malloc");
    exit(1);
}

static FILE* fopen_orDie(const char *filename, const char *instruction)
{
    FILE* const inFile = fopen(filename, instruction);
    if (inFile) return inFile;
    /* Error */
    perror(filename);
    exit(3);
}

static size_t fread_orDie(void* buffer, size_t sizeToRead, FILE* file)
{
    size_t const readSize = fread(buffer, 1, sizeToRead, file);
    if (readSize == sizeToRead) return readSize;
    if (feof(file)) return readSize;
    /* Error */
    perror("fread");
    exit(4);
}

static size_t fwrite_orDie(const void* buffer, size_t sizeToWrite, FILE* file)
{
    size_t const writtenSize = fwrite(buffer, 1, sizeToWrite, file);
    if (writtenSize == sizeToWrite) return sizeToWrite;   /* good */
    /* Error */
    perror("fwrite");
    exit(5);
}

static size_t fclose_orDie(FILE* file)
{
    if (!fclose(file)) return 0;
    /* Error */
    perror("fclose");
    exit(6);
}

// struct allows for moving multiple arguments into new threads
// https://hpc-tutorials.llnl.gov/posix/passing_args/
struct thread_data
{
    int thread_id;
    FILE* fin;
    int* temp_out;
    int* num_threads_ptr;
    int* iter_ptr;
    size_t* last_frame_ptr;
    int* last_thread_ptr;
};

void *compress(void *threadarg)
{
    // struct declaration
    struct thread_data *my_data;
    my_data = (struct thread_data*) threadarg;
    int tid;
    FILE* fin;
    int* temp_out;
    int* num_threads_ptr;
    int* iter_ptr;
    size_t* last_frame_ptr;
    int* last_thread_ptr;

    // Retrieves variables from struct
    tid = my_data -> thread_id;
    fin = my_data -> fin;
    temp_out = my_data -> temp_out;
    num_threads_ptr = my_data -> num_threads_ptr;
    iter_ptr = my_data -> iter_ptr;
    last_frame_ptr = my_data -> last_frame_ptr;
    last_thread_ptr = my_data -> last_thread_ptr;

    //printf("Hello World! It's me, thread #%d!\n", tid);

    // Declares the necessary variables for iterating
    int* dstAddr;
    void* srcAddr;
    int num_threads = *num_threads_ptr;
    int overall;
    int block;
    // Iterates through the necessary compresses
    for (block = 0; block < iter_ptr[tid]; block++)
    {
        // Solves for the dst and src addresses
        overall = tid + (num_threads - 1)*block;
        dstAddr = temp_out + overall*MAXSIZE;
        srcAddr = fin + overall*SRCSIZE;

        // Compresses
        if ( (tid == *last_thread_ptr) && (block == iter_ptr[tid] - 1) )
        {   // Compresses if it's the last block, not full block size
            comp_size[overall] = ZSTD_compress(dstAddr, MAXSIZE, srcAddr, *last_frame_ptr, 3);
        }
        else if (block < 10)
        {   // Compresses any other block
            comp_size[overall] = ZSTD_compress(dstAddr, MAXSIZE, srcAddr, SRCSIZE, 3);
        }
    }
    // Must exit at the end of a thread
    pthread_exit(NULL);
}


int main (void)
{
    // Declares variables for multithreading
    int num_threads;
    int rc;
    long t;

    // Determines how many simultaneous threads the user wants
    printf("How many simultaneous\n");
    printf("threads do you want? => ");
    scanf("%d", &num_threads);
    printf("\n");
    while (num_threads < 2)
    {
        printf("You must have at\n");
        printf("least two threads. => ");
        scanf("%d", &num_threads);
        printf("\n");
    }

    // Opens the file to be compressed and creates an output file
    FILE* const fin = fopen_orDie("silesia.txt", "r");
    FILE* const fout = fopen_orDie("silesia.zst", "w");

    // Determines how large the file to be compressed is
    fseek(fin, 0, SEEK_END);     // Seek to end of file
    size_t finSize = ftell(fin); // Get current file pointer
    fseek(fin, 0, SEEK_SET);     // Seek back to beginning of file
    printf("silesia.txt is %ld bytes while uncompressed.\n\n", finSize);

    // Number of frames to be compressed
    size_t num_frames = finSize/16384;
    if (finSize%16384 != 0) num_frames++; // Must round up if there is a partially filled frame
    printf("%ld frames will be compressed.\n\n", num_frames);
    int* num_threads_ptr = &num_threads; // Creates a pointer so the other threads may use it

    // Size of the last frame in the file
    size_t last_frame = finSize%16384;
    if (finSize%16384 == 0) last_frame = 16384; // The last frame cannot be empty
    size_t* last_frame_ptr = &last_frame; // Creates a pointer so the other threads may use it

    // Allocates memory for the compressed blocks to temporarily gather
    int* temp_out = malloc_orDie(num_frames*MAXSIZE);

    // Determines how many times each thread will compress a block
    int all_iter = num_frames/(num_threads - 1);
    int leftover = num_frames%(num_threads - 1);
    int num_iter[num_threads - 1];
    int i;
    for (i = 0; i < num_threads - 1; i++)
    {
        int last = 0;
        if (leftover > i) last = 1; // If there are leftover frames, add them to the early threads
        num_iter[i] = all_iter + last;
    }
    int* iter_ptr = num_iter; // Creates a pointer so the other threads may use it

    // Determines which thread will have the small last block
    int last_thread;
    if (leftover != 0) last_thread = leftover - 1;
    if (leftover == 0) last_thread = num_threads - 2;
    int* last_thread_ptr = &last_thread; // Creates a pointer so the other threads may use it

    // Prints how many iterations each thread will go through
    int j;
    for (j = 0; j < num_threads - 1; j++)
    {
        printf("Thread %d will iterate %d compressions.\n", j, iter_ptr[j]);
    }
    printf("The last thread is for controlling I/O.\n\n");

    // Creates the necessary threads for compressing
    pthread_t threads[num_threads];
    struct thread_data thread_data_array[num_threads];
    for(t = 0; t < num_threads - 1; t++)
    {
        //printf("In main: creating thread %ld\n", t);

        thread_data_array[t].thread_id = t;
        thread_data_array[t].fin = fin;
        thread_data_array[t].temp_out = temp_out;
        thread_data_array[t].num_threads_ptr = num_threads_ptr;
        thread_data_array[t].iter_ptr = iter_ptr;
        thread_data_array[t].last_frame_ptr = last_frame_ptr;
        thread_data_array[t].last_thread_ptr = last_thread_ptr;
        rc = pthread_create(&threads[t], NULL, compress, (void*) &thread_data_array[t]);
        
        // Returns an error if a thread is not created successfully
        if (rc)
        {
            printf("ERROR; Return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Main control thread starts activities here:
    printf("Hello World! It's me, the main thread.\n");

    // Checks for when the next compression finishes
    int inc = 0;
    for (inc = 0; inc < 14888; inc++)
    {
        printf("Entering loop:\n");
        while (!comp_size[inc]){}
        printf("Exited loop.\n");
        fwrite_orDie(temp_out, comp_size[inc], fout);
    }

    // Closes the open files
    fclose_orDie("silesia.txt");
    fclose_orDie("silesia.zst");

    // main() must exit as well
    pthread_exit(NULL);
}

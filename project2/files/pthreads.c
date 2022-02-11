#include <pthread.h>

#include <stdlib.h>    // malloc, free, exit
#include <stdio.h>     // fprintf, perror, fopen, etc.
#include <string.h>    // strlen, strcat, memset, strerror
#include <errno.h>     // errno
#include <sys/stat.h>  // stat
#include <zstd.h>      // presumes zstd library is installed
//#include </home/allan/advanced_computer_systems/project2/zstd-1.1.3/lib/compress/zstdmt_compress.c>
#define NUM_THREADS     2


static void* malloc_orDie(size_t size)
{
    void* const buff = malloc(size);
    if (buff) return buff;
    /* error */
    perror("malloc:");
    exit(1);
}

static FILE* fopen_orDie(const char *filename, const char *instruction)
{
    FILE* const inFile = fopen(filename, instruction);
    if (inFile) return inFile;
    /* error */
    perror(filename);
    exit(3);
}

static size_t fread_orDie(void* buffer, size_t sizeToRead, FILE* file)
{
    size_t const readSize = fread(buffer, 1, sizeToRead, file);
    if (readSize == sizeToRead) return readSize;   /* good */
    if (feof(file)) return readSize;   /* good, reached end of file */
    /* error */
    perror("fread");
    exit(4);
}

static size_t fwrite_orDie(const void* buffer, size_t sizeToWrite, FILE* file)
{
    size_t const writtenSize = fwrite(buffer, 1, sizeToWrite, file);
    if (writtenSize == sizeToWrite) return sizeToWrite;   /* good */
    /* error */
    perror("fwrite");
    exit(5);
}

static size_t fclose_orDie(FILE* file)
{
    if (!fclose(file)) return 0;
    /* error */
    perror("fclose");
    exit(6);
}

static void compressFile_orDie(const char* fname, const char* outName, int cLevel)
{
    FILE* const fin  = fopen_orDie(fname, "rb");
    FILE* const fout = fopen_orDie(outName, "wb");
    size_t const buffInSize = ZSTD_CStreamInSize();    /* can always read one full block */
    void*  const buffIn  = malloc_orDie(buffInSize);
    size_t const buffOutSize = ZSTD_CStreamOutSize();  /* can always flush a full block */
    void*  const buffOut = malloc_orDie(buffOutSize);

    ZSTD_CStream* const cstream = ZSTD_createCStream();
    if (cstream==NULL) { fprintf(stderr, "ZSTD_createCStream() error \n"); exit(10); }
    size_t const initResult = ZSTD_initCStream(cstream, cLevel);
    if (ZSTD_isError(initResult)) { fprintf(stderr, "ZSTD_initCStream() error : %s \n", ZSTD_getErrorName(initResult)); exit(11); }

    size_t read, toRead = buffInSize;
    while( (read = fread_orDie(buffIn, toRead, fin)) ) {
        ZSTD_inBuffer input = { buffIn, read, 0 };
        while (input.pos < input.size) {
            ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
            toRead = ZSTD_compressStream(cstream, &output , &input);   /* toRead is guaranteed to be <= ZSTD_CStreamInSize() */
            if (ZSTD_isError(toRead)) { fprintf(stderr, "ZSTD_compressStream() error : %s \n", ZSTD_getErrorName(toRead)); exit(12); }
            if (toRead > buffInSize) toRead = buffInSize;   /* Safely handle case when `buffInSize` is manually changed to a value < ZSTD_CStreamInSize()*/
            fwrite_orDie(buffOut, output.pos, fout);
        }
    }

    ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
    size_t const remainingToFlush = ZSTD_endStream(cstream, &output);   /* close frame */
    if (remainingToFlush) { fprintf(stderr, "not fully flushed"); exit(13); }
    fwrite_orDie(buffOut, output.pos, fout);

    ZSTD_freeCStream(cstream);
    fclose_orDie(fout);
    fclose_orDie(fin);
    free(buffIn);
    free(buffOut);
}


static const char* createOutFilename_orDie(const char* filename)
{
    size_t const inL = strlen(filename);
    size_t const outL = inL + 5;
    void* outSpace = malloc_orDie(outL);
    memset(outSpace, 0, outL);
    strcat(outSpace, filename);
    strcat(outSpace, ".zst");
    return (const char*)outSpace;
}

void *PrintHello(void *threadid)
{
    long tid;
    tid = (long)threadid;
    printf("Hello World! It's me, thread #%ld!\n", tid);
    pthread_exit(NULL);
}

struct thread_data{
   int  thread_id;
   char *arg1;
   char *arg2;
   int  arg3;
};

//struct thread_data thread_data_array[NUM_THREADS];
struct thread_data thread_data_array[1];

void *arg_helper(void *threadarg){
    // https://hpc-tutorials.llnl.gov/posix/passing_args/
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    long int  taskid;
    char *arg1;
    char *arg2;
    int  arg3;
    taskid = my_data->thread_id;
    arg1 = my_data->arg1;
    arg2 = my_data->arg2;
    arg3 = my_data->arg3;
    printf("Hello World! It's me, thread #%ld!\n", taskid);
    //compressFile_orDie(arg1, arg2, arg3);
    pthread_exit(NULL);
}

int main (int argc, char *argv[])
    {
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;
    for(t=0; t<NUM_THREADS; t++){
        const char* const exeName = argv[0];
        const char* const inFilename = argv[1];
        if (argc!=2) {
            printf("wrong arguments\n");
            printf("usage:\n");
            printf("%s FILE\n", exeName);
            return 1;
        }
        const char* const outFilename = createOutFilename_orDie(inFilename);
        //compressFile_orDie(inFilename, outFilename, 1);

        thread_data_array[0].thread_id = 1;
        thread_data_array[0].arg1 = inFilename;
        thread_data_array[0].arg2 = outFilename;
        thread_data_array[0].arg3 = 1;
        thread_data_array[1].thread_id = 2;
        thread_data_array[1].arg1 = inFilename;
        thread_data_array[1].arg2 = outFilename;
        thread_data_array[1].arg3 = 1;
        static ZSTDMT_bufferPool* a;
        a = ZSTDMT_createBufferPool(2);


        // printf("In main: creating thread %ld\n", t);
        // //rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
        // rc = pthread_create(&threads[t], NULL, arg_helper, 
        //     (void *) &thread_data_array[t]);
        // if (rc){
        //     printf("ERROR; return code from pthread_create() is %d\n", rc);
        //     exit(-1);
        // }
    }

    /* Last thing that main() should do */
    pthread_exit(NULL);
}
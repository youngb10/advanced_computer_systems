/*
 * Copyright (c) Martin Liska, SUSE, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */

#include <stdio.h>     // printf
#include <stdlib.h>    // free
#include <string.h>    // memset, strcat, strlen
#include <zstd.h>      // presumes zstd library is installed
#include "common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()
#include <math.h>      // ceil
#include <pthread.h>
#include <time.h>
//#define ZSTD_STATIC_LINKING_ONLY
// modified version of zstd 1.5.2 examples, streaming_compression_thread_pool

typedef struct compress_args
{
  const char *fname;
  char *outName;
  int cLevel;
  // int nbThreads;
  int threadId;
  int buffInSize;
  int buffOutSize;
  int bytesToRead;
  FILE* fin;
  FILE* fout;   
  ZSTD_CCtx* cctx;
#if defined(ZSTD_STATIC_LINKING_ONLY)
  ZSTD_threadPool *pool;
#endif
} compress_args_t;

static void *compressFile_orDie(void *data)
{
    //const int nbThreads = 12;
    
    compress_args_t *args = (compress_args_t *)data;
    // this should only be called once. being called multiple times...
    fprintf (stderr, "  Compressing thread #%u; bytes to compress: %u\n", args->threadId, args->bytesToRead);
    
    // find out how many passes needed to complete the read
    int num_of_reads = 0;
    int bytes_read = 0;
    int num_of_even_passes = ceil( (args->bytesToRead) / (args->buffInSize) );
    int last_pass_size = args->bytesToRead - (num_of_even_passes * args->buffInSize);
    fprintf (stderr, "  Compressing thread #%u; requires: %u equal passes plus 1 unequal pass of size: %u\n", args->threadId, num_of_even_passes, last_pass_size);

    void*  const buffIn  = malloc_orDie(args->buffInSize);
    //size_t const buffOutSize = ZSTD_CStreamOutSize();
    void*  const buffOut = malloc_orDie(args->buffOutSize);
    
    // /* Create the context. */
    ZSTD_CCtx* const cctx = ZSTD_createCCtx();
    CHECK(cctx != NULL, "ZSTD_createCCtx() failed!");

#if defined(ZSTD_STATIC_LINKING_ONLY)
    size_t r = ZSTD_CCtx_refThreadPool(cctx, args->pool);
    CHECK(r == 0, "ZSTD_CCtx_refThreadPool failed!");
#endif

    // /* Set any parameters you want.
    //  * Here we set the compression level, and enable the checksum.
    //  */
    // // this is probably not using multithreading 
    // context is doing something weird
    CHECK_ZSTD( ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, args->cLevel) );
    //CHECK_ZSTD( ZSTD_CCtx_setParameter(cctx, ZSTD_c_checksumFlag, 1) );
    //ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, nbThreads);
    //ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, 1);
    /* This loop reads from the input file, compresses that entire chunk,
     * and writes all output produced to the output file.
     */
    // want to split up reading based on thread number 
    // so want n and n + 1(buffer length)
    //  ^ this implementation is way too complicated
    //    requires accessing internal file pointer and moving it around...
    size_t toRead = args->buffInSize;
    //for (int i = 0; i < 2;i++) {
    for (;;) {
        if( num_of_reads <= num_of_even_passes ){
            num_of_reads += 1;
        }
        else{
            toRead = last_pass_size;
        }
        // if all the bytes that need to be read have been read and processed, exit function
        if( bytes_read == args->bytesToRead ){
            fprintf (stderr, "  Compressing thread #%u; exiting bytes read %u / %u\n", args->threadId,bytes_read, args->bytesToRead);
            free(buffIn);
            free(buffOut);
            return NULL;
        }
        size_t read = fread_orDie(buffIn, toRead, args->fin);
        bytes_read += read;
        fprintf (stderr, "  Compressing thread #%u; bytes read %lu\n", args->threadId,read);
        /* Select the flush mode.
         * If the read may not be finished (read == toRead) we use
         * ZSTD_e_continue. If this is the last chunk, we use ZSTD_e_end.
         * Zstd optimizes the case where the first flush mode is ZSTD_e_end,
         * since it knows it is compressing the entire source in one pass.
         */
        int const lastChunk = (read < toRead);
        fprintf (stderr, "  Compressing thread #%u; last chunk %u\n", args->threadId,lastChunk);
        ZSTD_EndDirective const mode = lastChunk ? ZSTD_e_end : ZSTD_e_continue;
        /* Set the input buffer to what we just read.
         * We compress until the input buffer is empty, each time flushing the
         * output.
         */
        ZSTD_inBuffer input = { buffIn, read, 0 };
        int finished;
        do {
            /* Compress into the output buffer and write all of the output to
             * the file so we can reuse the buffer next iteration.
             */
            ZSTD_outBuffer output = { buffOut, args->buffOutSize, 0 };
            size_t const remaining = ZSTD_compressStream2(cctx, &output , &input, mode);
            fprintf (stderr, "  Compressing thread #%u; remaining flush %lu\n", args->threadId,remaining);
            CHECK_ZSTD(remaining);
            fwrite_orDie(buffOut, output.pos, args->fout);
            /* If we're on the last chunk we're finished when zstd returns 0,
             * which means its consumed all the input AND finished the frame.
             * Otherwise, we're finished when we've consumed all the input.
             */
            finished = lastChunk ? (remaining == 0) : (input.pos == input.size);
        } while (!finished);
        CHECK(input.pos == input.size,
              "Impossible: zstd only returns 0 when the input is completely consumed!");

        if (lastChunk) {
            fprintf (stderr, "  Compressing thread #%u; exiting by falling off last chunk\n", args->threadId);
            break;
        }
    }

    // fclose_orDie(fout);
    //fclose_orDie(fin);
    ZSTD_freeCCtx(cctx);
    free(buffIn);
    free(buffOut);
    //free(args->outName);
    
    return NULL;
}


static char* createOutFilename_orDie(const char* filename)
{
    size_t const inL = strlen(filename);
    size_t const outL = inL + 5;
    void* const outSpace = malloc_orDie(outL);
    memset(outSpace, 0, outL);
    strcat(outSpace, filename);
    strcat(outSpace, ".zst");
    return (char*)outSpace;
}

int main(int argc, const char** argv)
{
    const char* const exeName = argv[0];

    if (argc!=2) {
        printf("wrong arguments\n");
        printf("usage:\n");
        printf("%s inputs.txt\n", exeName);
        printf("where inputs.txt has the following format:\n");
        printf("# The number of pool threads\n");
        printf("12\n");
        printf("# The buffer size in bytes\n");
        printf("16000\n");
        printf("# The compression level\n");
        printf("1\n");
        printf("# The file to compress (relative path)\n");
        printf("../data_set.tar\n");
        return 1;
    }
    //printf("%s\n",argv[1]);
    FILE* const fin_parameters  = fopen_orDie(argv[1], "rb");
    
    size_t const buffInSize1 = fsize_orDie(argv[1]);
    void*  const buffIn1  = malloc_orDie(buffInSize1);
    size_t const toRead1 = buffInSize1;
    size_t read = fread_orDie(buffIn1, toRead1, fin_parameters);
    
    // https://www.educative.io/edpresso/splitting-a-string-using-strtok-in-c
    int pool_size;
    int nbThreads;
    int buff_in_size;
    int level;
    char* file_name;
    char * line = strtok(buffIn1, "\n");
    // loop through the string to extract all other tokens
    while( 1 ) {
        //printf( " %s\n", line ); //printing each line
        // discard first value, then every other value
        //line = strtok(NULL, "\n");
        line = strtok(NULL, "\n");
        // read pool size
        pool_size = strtol(line,NULL,10);
        line = strtok(NULL, "\n");
        line = strtok(NULL, "\n");
        // read number of threads
        nbThreads = strtol(line,NULL,10);
        line = strtok(NULL, "\n");
        line = strtok(NULL, "\n");
        // read buffer size
        buff_in_size = strtol(line,NULL,10);
        line = strtok(NULL, "\n");
        line = strtok(NULL, "\n");
        // read compression level
        level = strtol(line,NULL,10);
        line = strtok(NULL, "\n");
        line = strtok(NULL, "\n");
        // read file to compress name
        file_name = line;
        break;        
    }
    // fix print statement for pools
    printf("To compress: %s; initial size: %lu bytes\n",file_name,fsize_orDie(file_name));
    //CHECK(pool_size != 0, "can't parse POOL_SIZE!");
    printf("  number of pool threads: %u\n", pool_size);
    CHECK(nbThreads != 0, "can't parse NBTHREADS!");
    printf("  number of threads: %u\n", nbThreads);
    //printf("%s\n",buffIn);
    CHECK(buff_in_size != 0, "can't parse BUFF_SIZE!");
    printf("  buff size: %u\n", buff_in_size);
    CHECK(level != 0, "can't parse LEVEL!");
    printf("  compression level: %u\n", level);

    size_t const buffInSize = buff_in_size;
    size_t const buffOutSize = ZSTD_CStreamOutSize();

    // equal to number of inputs - 1
    argc -= 1;
    argv += 1;

    // start time
    time_t start_time = time(NULL);

#if defined(ZSTD_STATIC_LINKING_ONLY)
    ZSTD_threadPool *pool = ZSTD_createThreadPool (pool_size);
    CHECK(pool != NULL, "ZSTD_createThreadPool() failed!");
    fprintf (stderr, "Using shared thread pool of size %d\n", pool_size);
#else
    //fprintf (stderr, "All threads use its own thread pool\n");
#endif
    printf("\n");
    //argc = 12;
    // what does this even do???
    // pthread_t *threads = malloc_orDie(argc * sizeof(pthread_t));
    // compress_args_t *args = malloc_orDie(argc * sizeof(compress_args_t));
    pthread_t *threads = malloc_orDie(1 * sizeof(pthread_t));
    //pthread_t *threads = malloc_orDie(1 );
    compress_args_t *args = malloc_orDie(1 * sizeof(compress_args_t));

    /* This loop reads from the input file, compresses that entire chunk,
     * and writes all output produced to the output file.
     */
    // may need to fix divisability -> add 1 to last item
    // also with remainder...
    int bytes_per_thread = (fsize_orDie(file_name) / nbThreads);

    fprintf (stderr, "Starting compression of %s with level %d, using %d threads\n", file_name, level, nbThreads);
    char *outName = createOutFilename_orDie(file_name);

    /* Open the input and output files. */
    FILE* const fout = fopen_orDie(outName, "wb");
    // need to open n files for n threads
    //  then seek to beginning of their reading using buffers
    //  may run into memory issues with large files, but there are work arounds like using multiple passes and buffers
    //    can implement similar type used in the compression function to manage size and location 
    
    // fin read first half; don't need to worry about end. is managed in compression 
    FILE* const fin  = fopen_orDie(file_name, "rb");
    // fin2 read second half
    // FILE* const fin2 = fopen_orDie(file_name, "rb");
    // void*  const buffInTestA  = malloc_orDie(bytes_per_thread);
    // size_t readTestA = fread_orDie(buffInTestA, bytes_per_thread, fin2);
    // free(buffInTestA);

    // not sure why this is a loop, may delete later
    // argc should be 1, so this shouldnt matter... either way why would
    // the number of args matter? maybe if there are multiple files?
    printf("number of threads entering pthread loop: %u\n",nbThreads);
    // somehow this is calling itself multiple times...
    for (int i = 0; i < nbThreads; i++)
    {
      //args[i].fname = argv[i];
      //args[i].fname = file_name;
      //args[i].outName = createOutFilename_orDie(file_name);
      args[i].outName = outName;
      args[i].cLevel = level;
      args[i].threadId = i;
      args[i].buffInSize = buffInSize;
      args[i].buffOutSize = buffOutSize;
      args[i].bytesToRead = bytes_per_thread;
      args[i].fin = fin;
      // i = 0 breaking things... this is very weird. 
      if(i = 0){
          printf("i = 0");
          //args[i].fin = fin;
      }
      else{
          //args[i].fin = fin2;
      }      
      args[i].fout = fout;
      //args[i].cctx = cctx;
#if defined(ZSTD_STATIC_LINKING_ONLY)
      args[i].pool = pool;
#endif
      printf("i: %u\n",i);
      pthread_create (&threads[i], NULL, compressFile_orDie, &args[i]);
    }
    
    for (unsigned i = 0; i < nbThreads; i++){
        pthread_join (threads[i], NULL);
    }
    
    // fix; for some reason outName dies... not sure why. It shouldnt. dies after threads...
    fprintf (stderr, "Finishing compression of %s\n", createOutFilename_orDie(file_name));
    fprintf (stderr, "File size after compression: %lu\n", fsize_orDie(createOutFilename_orDie(file_name)));
    fprintf (stderr, "Final compression ratio: %.6f\n", 1.0*fsize_orDie(file_name)/fsize_orDie(createOutFilename_orDie(file_name)));
    
    // might not need this free for outName
    free(outName);
    //ZSTD_freeCCtx(cctx);
    fclose_orDie(fout);
    fclose_orDie(fin);

#if defined(ZSTD_STATIC_LINKING_ONLY)
    ZSTD_freeThreadPool (pool);
#endif

    time_t end_time = time(NULL);
    time_t delta = end_time-start_time;
    printf("time to compress: %lu seconds\n",delta);

    return 0;
}

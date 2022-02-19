/*
 * Copyright (c) Martin Liska, SUSE, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 * You may select, at your option, one of the above-listed licenses.
 */

// todo: add (lower) limits for buffer size
// add malloc temp as input?

#include <stdio.h>     // printf
#include <stdlib.h>    // free
#include <string.h>    // memset, strcat, strlen
#include <zstd.h>      // presumes zstd library is installed
#include "common.h"    // Helper functions, CHECK(), and CHECK_ZSTD()
#include <math.h>      // ceil
#include <pthread.h>
#include <time.h>
#include <locale.h>
//#define ZSTD_STATIC_LINKING_ONLY
// modified version of zstd 1.5.2 examples, streaming_compression_thread_pool

// prototype definitions
static char* createOutFilename_orDie(const char* filename);
static void *compressFile_orDie(void *data);

typedef struct compress_args
{
  char *outName;
  int cLevel;
  int debug;
  int threadId;
  unsigned long long buffInSize;
  void* buffIn;
  int buffOutSize;
  void* buffOut;
  unsigned long long bytesToRead;
  FILE* fin;
  FILE* fout;   
  ZSTD_CCtx* cctx;
} compress_args_t;

static void *compressFile_orDie(void *data)
{   
    compress_args_t *args = (compress_args_t *)data;
    fprintf (stderr, "  Starting compressing thread # %u; bytes to compress: %llu\n", args->threadId, args->bytesToRead);
    
    // making file names
    char char_thread[100];
    sprintf(char_thread,"%u",(args->threadId));
    char out_name[] = "temp/";
    strcat(out_name,char_thread);
    (args->outName) = createOutFilename_orDie(out_name);
    (args->fout) = fopen_orDie((args->outName), "wb");
    
    // find out how many passes needed to complete the read
    int num_of_reads = 0;
    unsigned long long bytes_read = 0;
    unsigned long long num_of_even_passes = ceil( (args->bytesToRead) / (args->buffInSize) );
    unsigned long long last_pass_size = (args->bytesToRead) - (num_of_even_passes * (args->buffInSize));
    if(args->debug){
        fprintf (stderr, "  Compressing thread #%u; requires: %llu equal passes plus 1 unequal pass of size: %llu\n", args->threadId, num_of_even_passes, last_pass_size);
    }
    args->buffIn  = malloc_orDie(args->buffInSize);
    args->buffOutSize = ZSTD_CStreamOutSize();
    args->buffOut = malloc_orDie(args->buffOutSize);

    // /* Create the context. */
    args->cctx = ZSTD_createCCtx();
    CHECK(args->cctx != NULL, "ZSTD_createCCtx() failed!");

    // /* Set any parameters you want.
    //  * Here we set the compression level, and enable the checksum.
    //  */
    CHECK_ZSTD( ZSTD_CCtx_setParameter(args->cctx, ZSTD_c_compressionLevel, args->cLevel) );
    CHECK_ZSTD( ZSTD_CCtx_setParameter(args->cctx, ZSTD_c_checksumFlag, 1) );
    //ZSTD_CCtx_setParameter(args->cctx, ZSTD_c_nbWorkers, 1);
    /* This loop reads from the input file, compresses that entire chunk,
     * and writes all output produced to the output file.
     */
    size_t toRead = (args->buffInSize);

    for (;;) {
        if( num_of_reads <= num_of_even_passes ){
            num_of_reads += 1;
        }
        else{
            toRead = last_pass_size;
        }
        // if all the bytes that need to be read have been read and processed, exit function
        if( bytes_read == (args->bytesToRead) ){
            if(args->debug){fprintf (stderr, "  Compressing thread #%u; exiting bytes read %llu / %llu\n", args->threadId,bytes_read, args->bytesToRead);}
            fclose_orDie(args->fout);
            fclose_orDie(args->fin);
            ZSTD_freeCCtx(args->cctx);
            free(args->buffIn);
            free(args->buffOut);  
            return NULL;
        }
        size_t read = fread_orDie((args->buffIn), toRead, (args->fin));
        bytes_read += read;
        if(args->debug){fprintf (stderr, "  Compressing thread #%u; bytes to read %lu; bytes read %lu; in total %llu\n", args->threadId,toRead,read,bytes_read);}
        /* Select the flush mode.
         * If the read may not be finished (read == toRead) we use
         * ZSTD_e_continue. If this is the last chunk, we use ZSTD_e_end.
         * Zstd optimizes the case where the first flush mode is ZSTD_e_end,
         * since it knows it is compressing the entire source in one pass.
         */
        int const lastChunk = (read < toRead);
        if(args->debug){fprintf (stderr, "  Compressing thread #%u; last chunk %u\n", args->threadId,lastChunk);}
        ZSTD_EndDirective const mode = lastChunk ? ZSTD_e_end : ZSTD_e_continue;
        /* Set the input buffer to what we just read.
         * We compress until the input buffer is empty, each time flushing the
         * output.
         */
        ZSTD_inBuffer input = { (args->buffIn), read, 0 };
        int finished;
        do {
            /* Compress into the output buffer and write all of the output to
             * the file so we can reuse the buffer next iteration.
             */
            ZSTD_outBuffer output = { (args->buffOut), (args->buffOutSize), 0 };
            size_t const remaining = ZSTD_compressStream2((args->cctx), &output , &input, mode);
            if(args->debug){fprintf (stderr, "  Compressing thread #%u; remaining flush %lu; buffout pos %lu\n", args->threadId, remaining, output.pos);}
            CHECK_ZSTD(remaining);
            fwrite_orDie((args->buffOut), output.pos, args->fout);
            /* If we're on the last chunk we're finished when zstd returns 0,
             * which means its consumed all the input AND finished the frame.
             * Otherwise, we're finished when we've consumed all the input.
             */
            finished = lastChunk ? (remaining == 0) : (input.pos == input.size);
        } while (!finished);
        CHECK(input.pos == input.size,
              "Impossible: zstd only returns 0 when the input is completely consumed!");
        if (lastChunk) {
            if(args->debug){fprintf (stderr, "  Compressing thread #%u; exiting by falling off last chunk\n", args->threadId);}
            break;
        }
    }

    fclose_orDie(args->fout);
    fclose_orDie(args->fin);
    ZSTD_freeCCtx(args->cctx);
    free(args->buffIn);
    free(args->buffOut);    
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
    setlocale(LC_ALL, "en_US.UTF-8");

    const char* const exeName = argv[0];
    if (argc!=2) {
        printf("wrong arguments\n");
        printf("usage:\n");
        printf("%s inputs.txt\n", exeName);
        printf("where inputs.txt has the following format:\n");
        printf("# Debug enabled (1 or 0)\n");
        printf("1\n");
        printf("# The buffer size in bytes\n");
        printf("16000\n");
        printf("# The compression level\n");
        printf("10\n");
        printf("# The file to compress (relative path)\n");
        printf("../your_file.tar\n");
        return 1;
    }

    // read in inputs.txt
    FILE* const fin_parameters  = fopen_orDie(argv[1], "rb");
    size_t const buffInSize1 = fsize_orDie(argv[1]);
    void*  const buffIn1  = malloc_orDie(buffInSize1);
    size_t const toRead1 = buffInSize1;
    size_t read1 = fread_orDie(buffIn1, toRead1, fin_parameters);
    fclose_orDie(fin_parameters);
    
    // https://www.educative.io/edpresso/splitting-a-string-using-strtok-in-c
    int debug;
    int nbThreads;
    int buff_in_size;
    int level;
    char* file_name;
    char* line = strtok(buffIn1, "\n");
    // loop through the string to extract all other tokens
    while( 1 ) {
        // discard first value, then every other value
        line = strtok(NULL, "\n");
        // read pool size
        debug = strtol(line,NULL,10);
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
    free(buffIn1);
    // fix print statement for pools
    printf("To compress: %s; initial size: %lu bytes\n",file_name,fsize_orDie(file_name));
    printf("  debug: %u\n", debug);
    CHECK(nbThreads != 0, "can't parse DEBUG!");
    printf("  number of threads: %u\n", nbThreads);
    CHECK(buff_in_size != 0, "can't parse BUFF_SIZE!");
    printf("  buff size: %u\n", buff_in_size);
    CHECK(level != 0, "can't parse LEVEL!");
    printf("  compression level: %u\n", level);

    size_t const buffInSize = buff_in_size;
    size_t const buffOutSize = ZSTD_CStreamOutSize();

    // start time
    time_t start_time = time(NULL);

    printf("\n");
    //argc = 12;
    // what does this even do???
    // pthread_t *threads = malloc_orDie(argc * sizeof(pthread_t));
    // compress_args_t *args = malloc_orDie(argc * sizeof(compress_args_t));
    pthread_t *threads = malloc_orDie((nbThreads+1) * sizeof(pthread_t));
    //pthread_t *threads = malloc_orDie(1 );
    compress_args_t *args = malloc_orDie((nbThreads+1) * sizeof(compress_args_t));

    /* This loop reads from the input file, compresses that entire chunk,
     * and writes all output produced to the output file.
     */
    // may need to fix divisability -> add 1 to last item
    // also with remainder...
    unsigned long long bytes_per_thread = (fsize_orDie(file_name) / nbThreads);
    if(debug){printf("bytes per thread: %llu\n",bytes_per_thread);}

    fprintf (stderr, "Starting compression of %s with level %d, using %d threads\n", file_name, level, nbThreads);
    char *outName = createOutFilename_orDie(file_name);

    // not sure why this is a loop, may delete later
    // argc should be 1, so this shouldnt matter... either way why would
    // the number of args matter? maybe if there are multiple files?
    if(debug){printf("number of threads entering pthread loop: %u\n",nbThreads);}
    
    FILE* finTemp = fopen_orDie(file_name, "rb");
    for (int i = 0; i < nbThreads; i++)
    {
    // need to open n files for n threads
    // don't want to open Nlog(N) files
    // so, will read input file, read the relevant section
    // write that section, and discard the rest. 

    // archaic way of creating output names, courtesy of C
    int threadId = i;
    char threadIdChar[100];
    sprintf(threadIdChar,"%u",i);
    char out_name[] = "temp/threadIn";
    strcat(out_name,threadIdChar);      
    char* out_thread_name = createOutFilename_orDie(out_name);

    // ** for better memory & reliability may want to chunk the inputs to separate items
    // this breaks for the 40gb file... need to chunk it
    //FILE* finTemp = fopen_orDie(file_name, "rb");
    // create temporary output to write to, then close and reopen as the input file
    FILE* foutTemp = fopen_orDie(out_thread_name, "wb");

    // read into buffer the relevant part of the in file
    // j is -1 because when i = 0, it needs to read the first chunk (whole file) 
    // if the bytes that need to be read is larger than the buffer
    unsigned long long temp_size = buff_in_size;
    if(debug){printf("bytes per thread: %llu\n",bytes_per_thread);}
    if(bytes_per_thread < temp_size){
        if(debug){printf("trying to allocate %llu (bytes per thread)\n",bytes_per_thread);}
        void* buffInTemp = malloc_orDie(bytes_per_thread);
        fseek(finTemp,i*bytes_per_thread,SEEK_CUR);
        size_t readTemp = fread_orDie(buffInTemp, bytes_per_thread, finTemp);
        fwrite_orDie(buffInTemp, bytes_per_thread, foutTemp); 
        fseek(finTemp,0,SEEK_SET);
        //fclose_orDie(finTemp);
        fclose_orDie(foutTemp);
        free(buffInTemp);
    }
    else{
        if(debug){printf("trying to allocate %llu (temp size)\n",temp_size);}
        void* buffInTemp = malloc_orDie(temp_size);
        for(int j = -1; j < i; j++){
            // now on lass pass, we want to save everything past this
            if( (j+1) == i){
                int flag = 0;
                int num_of_reads = 1;
                int num_of_even_passes = ceil( bytes_per_thread / temp_size );
                int last_pass_size = bytes_per_thread - (num_of_even_passes * temp_size);
                while(!flag){
                    if( num_of_reads <= num_of_even_passes ){
                        num_of_reads += 1;
                    }
                    else{
                        temp_size = last_pass_size;
                        flag = 1;
                    }
                    size_t readTemp = fread_orDie(buffInTemp, temp_size, finTemp);
                    fwrite_orDie(buffInTemp, temp_size, foutTemp);   
                }   
            }
            // seek through the file until you want to start saving data
            int flag = 0;
            int num_of_reads = 1;
            int num_of_even_passes = ceil( bytes_per_thread / temp_size );
            unsigned long long last_pass_size = bytes_per_thread - (num_of_even_passes * temp_size);
            while(!flag){
                if( num_of_reads <= num_of_even_passes ){
                    num_of_reads += 1;
                }
                else{
                    temp_size = last_pass_size;
                    flag = 1;
                }
                fseek(finTemp,temp_size,SEEK_CUR);
            }
        }
        fseek(finTemp,0,SEEK_SET);
        //fclose_orDie(finTemp);
        fclose_orDie(foutTemp);
        free(buffInTemp);
    }
    FILE* finThread = fopen_orDie(out_thread_name, "rb");
    args[i].threadId = i;
    args[i].debug = debug;
    args[i].fin = finThread;
    args[i].cLevel = level;
    args[i].buffInSize = buffInSize;
    args[i].buffOutSize = buffOutSize;
    args[i].bytesToRead = bytes_per_thread;

    pthread_create (&threads[i], NULL, compressFile_orDie, &args[i]);
    }
    
    for (unsigned i = 0; i < nbThreads; i++){
        pthread_join (threads[i], NULL);
    }

    /* Open the output file. */
    FILE* const fout = fopen_orDie(outName, "wb");
    // combine all files in order
    for (int i = 0; i < nbThreads; i++)
    {
        int threadId = i;
        char threadIdChar[100];
        sprintf(threadIdChar,"%u",i);
        char out_name[] = "temp/";
        strcat(out_name,threadIdChar);      
        char* out_thread_name = createOutFilename_orDie(out_name); 
        //printf("char thread %s\n",out_thread_name);
        int size_thread_output = fsize_orDie(out_thread_name);
        FILE* const compressed_output_thread  = fopen_orDie(out_thread_name, "rb");
        void* const buffInThread  = malloc_orDie(size_thread_output);
        size_t readThread = fread_orDie(buffInThread, size_thread_output, compressed_output_thread);
        
        fwrite_orDie(buffInThread, size_thread_output, fout);
        fclose_orDie(compressed_output_thread);
        free(buffInThread);
    }

    // final statistics. may want to add more
    fprintf (stderr, "Finishing compression of %s\n", createOutFilename_orDie(file_name));
    fprintf (stderr, "File size after compression: %lu\n", fsize_orDie(createOutFilename_orDie(file_name)));
    fprintf (stderr, "Final compression ratio: %.6f\n", 1.0*fsize_orDie(file_name)/fsize_orDie(createOutFilename_orDie(file_name)));
    
    fclose_orDie(fout);

    time_t end_time = time(NULL);
    time_t delta = end_time-start_time;
    printf("time to compress: %lu seconds\n",delta);

    return 0;
}

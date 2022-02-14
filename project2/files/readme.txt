make -C  ../zstd-1.5.2/lib libzstd.a
cc   pthreads_streaming_compression.o -pthread ../zstd-1.5.2/lib/libzstd.a   -o pthreads_streaming_compression
./pthreads_streaming_compression 1 1 ../github_users_sample_set.tar

based of streaming_compression_thread_pool example code 

important functions:
ZSTD_CStream 
stuff with cctx 
ZSTD_e_continue 
ZSTD_e_flush
ZSTD_compressContinue

has instructions
zstd.h
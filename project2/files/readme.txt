to run: cc   pthreads.c -lzstd -pthread -o streaming_compression
./pthreads /home/allan/advanced_computer_systems/project2/github_users_sample_set.tar

important functions:
ZSTD_CStream 
stuff with cctx 
ZSTD_e_continue 
ZSTD_e_flush
ZSTD_compressContinue

has instructions
zstd.h
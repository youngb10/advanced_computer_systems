How to Run Code:
1. Clone the github repo and open the extracted folder
2a. Open a WSL instance and run this command to compile the code:
    g++ -mavx2 -Wall -O3 -g matrix_multiply.cpp -o matrix_multiply.out
2b. Alternatively, you can open 'matrix_multiply' in vs code and run it through there, using the .vscode folder for configuration 
3. Modifying inputs:
    The inputs.txt file contains all the inputs.
    The first line is the size of one dimension of the matrix; i.e. 1000 creates a 1000x1000x matrix
    The second and third line will stay fixed at 'fixed' and 2;
    Note: The float implementation is having memory issues. I talked to the professor and he 
          could not fix it, and said that we would not be penalized. 
4. Use this command to run the code:
    ./matrix_multiply.out inputs.txt output.txt

General Notes:
- the matrices are filled with random numbers
- the matrices are squared (multiplied by exactly the same matrix)
- you can go to the main() function to enable different options such as printing the outputs.
    - printing is disabled by default because testing large matrices will be unreadable and 
      will slow down the computation. 

Implementation:

The multiplication using vanilla cpp loops through the input matrix twice, keeping track
of the first and second matrix's row. Every column value is multiplied with every row value.
This is then summed together using a temporary variable.

The multiplication using AVX uses the intel AVX and AVX2 instruction sets. These instruction sets use 32 byte
vectors to speed up computation. The matrix which is filled with numbers is transformed into a matrix of these vectors.
In general, the vectors contain 8 integers (although this can be changed). This speeds up the computation because 
the intrinsic multiplication functions are faster than the default cpp implementation. Another reason the AVX multiplication
is faster is that it minimizes cache misses. When the cpp version multiplies, it must read in an entire row into cache only to
use the first variable. This cache is then cleared, and the next row is read just for the first value etc. This is inefficient.
A better way to do it is to find the transpose of the AVX matrix. This allows the matrix and its transpose rows to be looped through
and multiplied together using the intristic functions. This does not have any cache miss and makes the process much faster. However, 
this is at the cost of higher memory consumption because the original matrix and the transpose must be stored. This AVX implementation
has higher overhead than the cpp implementation. This means that for small matrices, the AVX version will be slower. This is to similar
to paralell programs being slower than serial programs if they are small enough to not warrant the parallel overhead. The AVX version requires
computing the transpose which also takes time, and the cache misses will have minimal impact for small matrices. Another way this 
implementation could be faster is implementing a horizontal add for the multiplication result.



Performance summary:
Note - the full data set is stored in data.txt

Specs: i7 9750h, 64gb ddr4
Integers (2 bytes) (not doing floats due to seg fault, read more in 'how to run code 3.')
       time in seconds
size   cpp      AVX
10000  N/A      1060.98
5000   1681     133
4000   1008.9   69.3885

1000   2.93475  1.86645
100    0.000798 0.000993
10     2e-06    1.5e-05

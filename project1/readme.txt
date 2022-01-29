How to Run Code:

1. Clone the github repo and open the extracted folder
2a. Open a WSL instance and run this command to compile the code:
    g++ -mavx2 -Wall -O3 -g matrix_multiply.cpp -o matrix_multiply.out
2b. Alternatively, you can open 'matrix_multiply' in vs code and run it through there, using the .vscode folder for configuration 
3. Modifying inputs:
    The inputs.txt file contains all the inputs.
    The first line is the size of one dimension of the matrix; i.e. 1000 creates a 1000x1000 matrix
    The second and third line will stay fixed at 'fixed' and '2';
    Note: The float implementation is having memory issues. I talked to the professor and he 
          could not fix it, and said that we would not be penalized. 
4. Use this command to run the code:
    ./matrix_multiply.out inputs.txt output.txt


General Notes:
- the matrices are filled with randomly generated numbers
- the matrices are squared (multiplied by exactly the same matrix)
- you can go to the main() function to enable different options such as printing the outputs.
    - printing is disabled by default because testing large matrices will be unreadable and 
      will slow down the computation. 


Implementation:

The multiplication using vanilla cpp loops through the input matrix twice, keeping track
of the first and second matrix's row. Every column value is multiplied with every row value.
This is then summed together using a temporary variable.

The multiplication with SIMD instructions uses the Intel AVX and AVX2 instruction set extensions. These extensions use 32-byte
vectors to speed up computation. The matrix is filled with randomly generated numbers and is transformed into a matrix of these vectors.
In general, the vectors contain 8 integers (efficiency could be improved with 16-integer implementation). This speeds up the computation
because the intrinsic multiplication instructions are faster than the default cpp implementation. One single SIMD instruction is able
to multiply 8 elements together at the same time with a lower latency than typical multiplication. Another reason using SIMD can make
it faster is that it decreases cache misses. When the non-SIMD version multiplies, it must read in an entire row into cache only to
use the first variable. This cache is then cleared, and the next row is read just for the first value etc. This is inefficient, and
becomes increasingly so as the matrix size increases. A better way to do it is to find the transpose of the AVX matrix. This allows the
matrix and its transpose rows to be looped through and multiplied together with significantly fewer cache misses. Fewer misses makes the
overall process much faster. However, the SIMD instruction implementation has greater overhead than the non-SIMD implementation.
Significant time must be taken to set up the AVX matrix to be multiplied. As the matrix size increases, this time becomes insignificant.
But at a very small matrix size, this extra overhead can make the non-SIMD implementation more efficient than the SIMD implementation. 
This is to similar to paralell programs being slower than serial programs if they are small enough to not warrant the parallel overhead.
The SIMD implementation also requires computing the transpose, which also adds to the overhead. Cache miss avoidance will be inconsequential
for smaller matrices. 


Performance Summary:
Note: The full data set is stored in data.txt

Specs: i9 8-core in Macbook Pro, 16MB Cache, 32GB DDR4
Integers 2-byte (Large floats not available due to seg faults, read more in 'How to Run Code 3.')

          time (s)
Size   No SIMD  SIMD
10000  N/A      252
8000   N/A      139
6000   N/A      58.4
4000   608      20.5
2000   27.8     1.78
1000   1.32     .133
100    .000557  .000508
10     ~0       1.2e-05


Analysis and Conclusion:

Explicit SIMD instructions can be used to drastically improve program speed. Improved performance is obtained through both increased
paralellism and significantly improved cache efficiency. SIMD instructions directly improve parallelism by operating on multiple
elements of data at the same time. SIMD instructions indirectly improve cache efficiency by allowing greater control over the
movement of data and greater usage of spacial and temporal locality. 

However, the implementation of these instructions can come with increased overhead, resulting in decreased performance for incredibly
small matrices. Though, when dealing with small data sets, performance is not typically a top priority, and modern computing and data
centers require manipulation of large amounts of data on a large scale. 

#include <iostream>       // For I/O
#include <fstream>        // For I/O
#include <iomanip>        // For setw
#include <stdio.h>        // std
#include <stdlib.h>       // std
#include <ctime>          // Timekeeping
#include <ratio>          // Timekeeping
#include <chrono>         // Timekeeping
#include <vector>         // For vectors
#include <immintrin.h>    // For AVX
#include <xmmintrin.h>    // For AVX
#include <cmath>          // For floor
#include <stdint.h>
#include <string.h>
#include <sys/resource.h>

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;

// Useful Links:
// https://www.cs.fsu.edu/~engelen/courses/HPC-adv/intref_cls.pdf
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956&techs=AVX&text=mult
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956,6144&techs=AVX&text=set_ps
// https://chryswoods.com/vector_c++/immintrin.html

// VERY useful link to initializing intel intrinsics
// https://github.com/Triple-Z/AVX-AVX2-Example-Code#initialization-intrinsics
// VERY useful for optimization
// https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
/* in launch
 to compile:  g++ -mavx2 -Wall -O1 -g matrix_multiply.cpp -o matrix_multiply.out
 to run: ./matrix_multiply.out inputs.txt output.txt
 "args": [
                "inputs.txt",
                "output.txt"
            ],
"args": [ in tasks
                "-mavx"
                "-fdiagnostics-color=always",
                "-g",
                "${file}",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}"
            ],
*/

// AVX Instructions

// For floats:
__m256 _mm256_set1_ps(float a);
__m256 _mm256_set_ps(float e7, float e6, float e5, float e4, float e3, float e2, float e1, float e0);
__m256 _mm256_mul_ps(__m256 a, __m256 b);
void _mm256_storeu_ps(float *mem_addr, __m256 a);

// For ints:
__m256i _mm256_set_epi32(int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0);
__m256i _mm256_mullo_epi32(__m256i a, __m256i b);
void _mm256_store_si256(__m256i *mem_addr, __m256i a);

// For short ints:
__m256i _mm256_madd_epi16(__m256i a, __m256i b);

// Other instructions
__m256 _mm256_castsi256_ps(__m256i a);
__m256i _mm256_castps_si256(__m256 a);
__m256i _mm256_setr_epi32(int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0);
void _mm256_storeu_si256(__m256i *mem_addr, __m256i a);


// my function prototypes (will be using overloading)
float get_float();
short int get_int();
void print_cpp(ofstream &output, float **in_matrix_cpp, uint matrix_size);
void print_cpp(ofstream &output, short int **in_matrix_cpp, uint matrix_size);
void transpose(float **in_matrix_cpp, float **in_matrix_cpp_transpose, uint matrix_size);
void transpose(short int **in_matrix_cpp, short int **in_matrix_cpp_transpose, uint matrix_size);
static inline __m256 hsums(__m256 const& v);
static inline float hadd(__m256 const& v);
uint32_t hsum_prep(__m128i x);
uint32_t hsum_8x32(__m256i v);
void multiply_cpp(ofstream &output, float **in_matrix_cpp, float **out_matrix_cpp, uint matrix_size, bool print_output);
void multiply_cpp(ofstream &output, short int **in_matrix_cpp, short int **out_matrix_cpp, uint matrix_size, bool print_output);
void multiply_avx(ofstream &output, float **out_matrix_cpp, float **in_matrix_cpp, float **in_matrix_cpp_transpose,uint matrix_size, bool print_transpose, bool print_output);
void multiply_avx(ofstream &output, short int **out_matrix_cpp, short int **in_matrix_cpp, short int **in_matrix_cpp_transpose,uint matrix_size, bool print_transpose, bool print_output);



float get_float()
{
    // float will be between 0 and 1.
    float rndflt = float(rand()) / (float(RAND_MAX));
    return rndflt;
}

short int get_int()
{
    // short int will be between 0 and 255.
    short int rndflt = (short int)(rand()) / 255;
    return rndflt;
}

void print_cpp(ofstream &output, float **in_matrix_cpp, uint matrix_size)
{
    for (uint i = 0; i < matrix_size; i++)
    {
        for (uint j = 0; j < matrix_size; j++)
        {
            cout << left << setfill(' ') << setw(11);
            cout << in_matrix_cpp[i][j];
            output << left << setfill(' ') << setw(11);
            output << in_matrix_cpp[i][j];
        }
        cout << endl;
        output << endl;
    }
    cout << endl;
    output << endl;
};

void print_cpp(ofstream &output, short int **in_matrix_cpp, uint matrix_size)
{
    for (uint i = 0; i < matrix_size; i++)
    {
        for (uint j = 0; j < matrix_size; j++)
        {
            cout << left << setfill(' ') << setw(11);
            cout << in_matrix_cpp[i][j];
            output << left << setfill(' ') << setw(11);
            output << in_matrix_cpp[i][j];
        }
        cout << endl;
        output << endl;
    }
    cout << endl;
    output << endl;
};

void transpose(float **in_matrix_cpp, float **in_matrix_cpp_transpose, uint matrix_size)
{
    // creates transpose of input matrix
    // this is used for processing in multiplying avx
    for (uint i = 0; i < matrix_size; i++)
    {
        for (uint j = 0; j < matrix_size; j++)
        {
            in_matrix_cpp_transpose[j][i] = in_matrix_cpp[i][j];
        }
    }
};

void transpose(short int **in_matrix_cpp, short int **in_matrix_cpp_transpose, uint matrix_size)
{
    // creates transpose of input matrix
    // this is used for processing in multiplying avx
    for (uint i = 0; i < matrix_size; i++)
    {
        for (uint j = 0; j < matrix_size; j++)
        {
            in_matrix_cpp_transpose[j][i] = in_matrix_cpp[i][j];
        }
    }
};

// Obtains the horizontal sum of all floats in a __m256
static inline __m256 hsums(__m256 const& v)
{
    auto x = _mm256_permute2f128_ps(v, v, 1);
    auto y = _mm256_add_ps(v, x);
    x = _mm256_shuffle_ps(y, y, _MM_SHUFFLE(2, 3, 0, 1));
    x = _mm256_add_ps(x, y);
    y = _mm256_shuffle_ps(x, x, _MM_SHUFFLE(1, 0, 3, 2));
    return _mm256_add_ps(x, y);
}

// Uses the hsums function and then obtains the value of the sum as a float
static inline float hadd(__m256 const& v)
{
    return _mm_cvtss_f32(_mm256_castps256_ps128(hsums(v)));
}

// Begins horizontally summing an integer vector
uint32_t hsum_prep(__m128i x)
{
    __m128i hi64  = _mm_unpackhi_epi64(x, x);           // 3-operand non-destructive AVX lets us save a byte without needing a movdqa
    __m128i sum64 = _mm_add_epi32(hi64, x);
    __m128i hi32  = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));    // Swap the low two elements
    __m128i sum32 = _mm_add_epi32(sum64, hi32);
    return _mm_cvtsi128_si32(sum32);       // movd
}

// Finishes horizontally summing an integer vector
uint32_t hsum_8x32(__m256i v)
{
    __m128i sum128 = _mm_add_epi32(
                 _mm256_castsi256_si128(v),
                 _mm256_extracti128_si256(v, 1)); // silly GCC uses a longer AXV512VL instruction if AVX512 is enabled :/
    return hsum_prep(sum128);
}

void multiply_avx(ofstream &output, float **out_matrix_cpp, float **in_matrix_cpp, float **in_matrix_cpp_transpose, uint matrix_size, bool print_transpose = false, bool print_output = false)
{
    // using transpose to minimize cache misses
    // only works on square matrices
    // set print_transpose or print_output to true for printing
    uint num_of_regs = ceil(matrix_size / 8.0);
    __m256 in_matrix_avx[matrix_size][num_of_regs];
    //__m256 out_matrix_avx[matrix_size][num_of_regs];
    __m256 in_matrix_avx_transpose[matrix_size][num_of_regs];
    // take input matrix (vector form) and create the vectorized version using __m256
    // since matrix is square, the indecies are the same for the transpose
    for (uint vert = 0; vert < matrix_size; vert++)
    {
        for (uint horiz = 0; horiz < num_of_regs; horiz++)
        {
            // looping through each element in register and checking if it exists
            // cout << "vert " << vert << " horiz " << horiz << endl;
            vector<float> temp_vector;
            vector<float> temp_vector_transpose;
            for (uint reg_i = 0; reg_i < 8; reg_i++)
            {
                uint real_i = horiz * 8 + reg_i;
                // cout << real_i << " ";
                if (real_i < matrix_size)
                {
                    temp_vector.push_back(in_matrix_cpp[vert][real_i]);
                    temp_vector_transpose.push_back(in_matrix_cpp_transpose[vert][real_i]);
                    if (print_transpose)
                    {
                        cout << left << setfill(' ') << setw(11);
                        cout << in_matrix_cpp[vert][real_i];
                        //output << left << setfill(' ') << setw(11);
                        //output << in_matrix_cpp[vert][real_i];
                    }
                }
                else
                {
                    temp_vector.push_back(0);
                    temp_vector_transpose.push_back(0);
                    if (print_transpose)
                    {
                        cout << left << setfill(' ') << setw(11);
                        cout << " 0 ";
                        //output << left << setfill(' ') << setw(11);
                        //output << " 0 ";
                    }
                }
            }
            *(*(in_matrix_avx + vert) + horiz) = _mm256_set_ps(temp_vector[7], temp_vector[6], temp_vector[5], temp_vector[4],
                                                               temp_vector[3], temp_vector[2], temp_vector[1], temp_vector[0]);
            *(*(in_matrix_avx_transpose + vert) + horiz) = _mm256_set_ps(temp_vector_transpose[7], temp_vector_transpose[6], temp_vector_transpose[5], temp_vector_transpose[4],
                                                                         temp_vector_transpose[3], temp_vector_transpose[2], temp_vector_transpose[1], temp_vector_transpose[0]);
        };
        if (print_transpose)
        {
            cout << endl;
            cout << endl;
            //output << endl;
            //output << endl;
        }
    };

    // Computes the multiplication of the first matrix and the second matrix's transpose
    float sum = 0;
    __m256 vector_sum = _mm256_set_ps(0, 0, 0, 0, 0, 0, 0, 0);
    // Loops through the rows of the first matrix
    for (uint vert = 0; vert < matrix_size; vert++)
    {
        // Loops through the rows of the second matrix
        for (uint vert_t = 0; vert_t < matrix_size; vert_t++)
        {
            vector_sum = _mm256_set_ps(0, 0, 0, 0, 0, 0, 0, 0);
            
            // Loops through the columns of vectors
            for (uint horiz = 0; horiz < num_of_regs; horiz++)
            {
                // Multiplies the registers of the first and second matrices
                __m256 mulresult = _mm256_mul_ps(in_matrix_avx[vert][horiz], in_matrix_avx_transpose[vert_t][horiz]);
                
                // Horizontally sums the multiplied register, adds to the output
                vector_sum = _mm256_add_ps(vector_sum, mulresult);
            }
            
            sum = hadd(vector_sum);
            out_matrix_cpp[vert][vert_t] = sum;
            
        }
    };
    
    if (print_output)
    {
        print_cpp(output, out_matrix_cpp, matrix_size);
    }
    
};

void multiply_avx(ofstream &output,  short int **out_matrix_cpp, short int **in_matrix_cpp, short int **in_matrix_cpp_transpose,uint matrix_size, bool print_transpose = false, bool print_output = false)
{
    // using transpose to minimize cache misses
    // only works on square matrices
    // set print_transpose or print_output to true for printing
    
    uint num_of_regs = ceil(matrix_size / 8.0);
    
    __m256i **in_matrix_avx;
    __m256i **in_matrix_avx_transpose;
    //__m256i **out_matrix_avx;

    in_matrix_avx = new __m256i *[matrix_size];
    in_matrix_avx_transpose = new __m256i *[matrix_size];
    //out_matrix_avx = new __m256i *[matrix_size];

    // myPointer = new int;
    // delete myPointer; //freed memory
    // myPointer = NULL; //pointed dangling ptr to NULL

    // take input matrix (vector form) and create the vectorized version using __m256
    // since matrix is square, the indecies are the same for the transpose
    for (uint vert = 0; vert < matrix_size; vert++)
    {   // need to double reg size because only half of the data is multiplied at once
        in_matrix_avx[vert] = new __m256i[num_of_regs*2];
        in_matrix_avx_transpose[vert] = new __m256i[num_of_regs*2];
        //out_matrix_avx[vert] = new __m256i[num_of_regs*2];
        for (uint horiz = 0; horiz < num_of_regs; horiz++)
        {
            // looping through each element in register and checking if it exists
            short int *temp1;
            temp1 = new short int[matrix_size];
            short int *temp1_t;
            temp1_t = new short int[matrix_size];
            for (uint reg_i = 0; reg_i < 8; reg_i++)
            {
                uint real_i = horiz * 8 + reg_i;
                if (real_i < matrix_size)
                {
                    temp1[reg_i] = in_matrix_cpp[vert][real_i];
                    temp1_t[reg_i] = in_matrix_cpp_transpose[vert][real_i];

                    if (print_transpose)
                    {
                        cout << left << setfill(' ') << setw(11);
                        cout << in_matrix_cpp[vert][real_i];
                        output << left << setfill(' ') << setw(11);
                        output << in_matrix_cpp[vert][real_i];
                    }
                }
                else
                {
                    temp1[reg_i] = 0;
                    temp1_t[reg_i] = 0;
                    if (print_transpose)
                    {
                        cout << left << setfill(' ') << setw(11);
                        cout << " 0 ";
                        output << left << setfill(' ') << setw(11);
                        output << " 0 ";
                    }
                }
            }

            __m256i temp_reg = _mm256_set_epi32(temp1[7], temp1[6], temp1[5], temp1[4], temp1[3], temp1[2], temp1[1], temp1[0]);
            __m256i *ptr =(__m256i*) &*(*(in_matrix_avx+vert)+horiz);
            _mm256_storeu_si256(ptr,temp_reg);
            __m256i temp_reg_t = _mm256_set_epi32(temp1_t[7], temp1_t[6], temp1_t[5], temp1_t[4], temp1_t[3], temp1_t[2], temp1_t[1], temp1_t[0]);
            __m256i *ptr_t =(__m256i*) &*(*(in_matrix_avx_transpose+vert)+horiz);
            _mm256_storeu_si256(ptr_t,temp_reg_t);

            delete temp1;
            temp1 = NULL;
            delete temp1_t;
            temp1_t = NULL;
        };
        if (print_transpose)
        {
            cout << endl;
            cout << endl;
            output << endl;
            output << endl;
        }
    };
    
    // Computes the multiplication of the first matrix and the second matrix's transpose
    short int sum = 0;
    __m256i vector_sum = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, 0);
    // Loops through the rows of the first matrix
    for (uint vert = 0; vert < matrix_size; vert++)
    {
        // Loops through the rows of the second matrix
        for (uint vert_t = 0; vert_t < matrix_size; vert_t++)
        {
            vector_sum = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, 0);
            
            // Loops through the columns of vectors
            for (uint horiz = 0; horiz < num_of_regs; horiz++)
            {

                //##########################################################################################################
                // THIS CODE IS ONLY FOR USE WITH LENGTH-16 REGISTERS
                
                // Multiplies the registers of the first and second matrices, adds every other result together
                //__m256i mulresult = _mm256_madd_epi16(in_matrix_avx[vert][horiz], in_matrix_avx_transpose[vert_t][horiz]);
                
                // Gathers all multiplied vectors and adds them
                //vector_sum = _mm256_add_epi32(vector_sum, mulresult);
                
                //##########################################################################################################
                
                
                //##########################################################################################################
                // THIS CODE IS ONLY FOR USE WITH LENGTH-32 REGISTERS
                
                // Multiplies the registers of the first and second matrices
                __m256i mulresult = _mm256_mullo_epi32(in_matrix_avx[vert][horiz], in_matrix_avx_transpose[vert_t][horiz]);
                
                // Gathers all multiplied vectors and adds them
                vector_sum = _mm256_add_epi32(vector_sum, mulresult);
                
                //##########################################################################################################

            }
            
            // Horizontally sums the sum vector
            sum = hsum_8x32(vector_sum);
            out_matrix_cpp[vert][vert_t] = sum;
            
        }
    };
    
    if (print_output)
    {
        print_cpp(output, out_matrix_cpp, matrix_size);
    }
    
};

void multiply_cpp(ofstream &output, float **in_matrix_cpp, float **out_matrix_cpp, uint matrix_size, bool print_output = false)
{
    // only works for square matrices
    for (uint i = 0; i < matrix_size; i++)
    {
        for (uint j = 0; j < matrix_size; j++)
        {
            // grab left matrix row; in_matrix[i][1]
            // grab right matrix column; in_matrix[1][j]
            // multiply each value and add; so in_matrix[i][1]*in_matrix[1][j]
            // then in_matrix[i][2]*in_matrix[2][j] etc.
            float temp = 0;
            for (uint x = 0; x < matrix_size; x++)
            {
                temp += in_matrix_cpp[i][x] * in_matrix_cpp[x][j];
            }
            out_matrix_cpp[i][j] = temp;
        }
    }
    if (print_output)
    {
        print_cpp(output, out_matrix_cpp, matrix_size);
    }
}

void multiply_cpp(ofstream &output, short int **in_matrix_cpp, short int **out_matrix_cpp, uint matrix_size, bool print_output = false)
{
    // only works for square matrices
    for (uint i = 0; i < matrix_size; i++)
    {
        for (uint j = 0; j < matrix_size; j++)
        {
            // grab left matrix row; in_matrix[i][1]
            // grab right matrix column; in_matrix[1][j]
            // multiply each value and add; so in_matrix[i][1]*in_matrix[1][j]
            // then in_matrix[i][2]*in_matrix[2][j] etc.
            short int temp = 0;
            for (uint x = 0; x < matrix_size; x++)
            {
                temp += in_matrix_cpp[i][x] * in_matrix_cpp[x][j];
            }
            out_matrix_cpp[i][j] = temp;
        }
    }
    if (print_output)
    {
        print_cpp(output, out_matrix_cpp, matrix_size);
    }
}

int main(int argc, char *argv[])
{
    // this is stack overflow
    // int* array1234 = new int[1000000000000];
    const rlim_t kStackSize = 1000 * 1024 * 1024; // min stack size = 16 MB
    struct rlimit rl;
    //#pragma comment(linker, "/STACK: 2000000")

    srand(1);

    // file input scheme from cs1200
    ifstream inputs(argv[1]); // input file
    ofstream output(argv[2]); // output file

    // check that the files are loaded
    if (!inputs.good())
    { // pulled from submitty resources on helpful c++ programming information
        std::cerr << "Can't open the inputs.txt file: " << argv[1] << " ; please try again" << std::endl;
        exit(true);
    }
    if (!output.good())
    { // pulled from submitty resources on helpful c++ programming information
        std::cerr << "Can't open the output.txt file: " << argv[1] << " ; please try again" << std::endl;
        exit(true);
    }

    uint matrix_size;
    inputs >> matrix_size; // read in first value in inputs
    string numtype;
    inputs >> numtype;
    uint numsize;
    inputs >> numsize;

    cout << "Inputs Accepted!" << endl;
    cout << "Matrix Size: " << matrix_size << " x " << matrix_size << endl;
    cout << "Number Type: " << numtype << ", " << numsize << " bytes " << endl;
    cout << endl;
    output << "Inputs Accepted!" << endl;
    output << "Matrix Size: " << matrix_size << " x " << matrix_size << endl;
    output << "Number Type " << numtype << ", " << numsize << " bytes " << endl;
    output << endl;

    if ((numtype == "float") && (numsize == 4))
    {

        float **in_matrix_cpp;
        float **in_matrix_cpp_transpose;
        float **out_matrix_cpp;
        //float **out_matrix_avx;

        in_matrix_cpp = new float *[matrix_size];
        in_matrix_cpp_transpose = new float *[matrix_size];
        out_matrix_cpp = new float *[matrix_size];
        //out_matrix_avx = new float *[matrix_size];

        // construct float matrix for regular cpp
        // also make empty array for out matrix and in matrix transpose
        for (uint i = 0; i < matrix_size; i++)
        {
            in_matrix_cpp[i] = new float[matrix_size];
            in_matrix_cpp_transpose[i] = new float [matrix_size];
            out_matrix_cpp[i] = new float[matrix_size];
            //out_matrix_avx[i] = new float[matrix_size];
            
            for (uint j = 0; j < matrix_size; j++)
            {
                in_matrix_cpp[i][j] = get_int();
                in_matrix_cpp_transpose[i][j] = 0;
                out_matrix_cpp[i][j] = 0;
                //out_matrix_avx[i][j] = 0;
            }
        }
        // transpose in matrix for computation in avx multiply
        transpose(in_matrix_cpp, in_matrix_cpp_transpose,matrix_size);


        high_resolution_clock::time_point start1 = high_resolution_clock::now(); // Starts CPP Time
        
        multiply_cpp(output, in_matrix_cpp, out_matrix_cpp, matrix_size, false); // CPP Multiplication
        
        high_resolution_clock::time_point end1 = high_resolution_clock::now();   // Stops CPP Time
        duration<double, std::milli> t1 = (end1 - start1) / 1000;
        cout << "Multiplication without SIMD took: " << t1.count() << endl;      // Displays CPP Time

        high_resolution_clock::time_point start2 = high_resolution_clock::now(); // Starts AVX Time
        multiply_avx(output, out_matrix_cpp, in_matrix_cpp, in_matrix_cpp_transpose, matrix_size, false, false);
        // multiply_avx(output, in_matrix_cpp,in_matrix_cpp_transpose,out_matrix_avx,false,true);
        high_resolution_clock::time_point end2 = high_resolution_clock::now();   // Stops AVX Time
        duration<double, std::milli> t2 = (end2 - start2) / 1000;
        cout << "Multiplication with SIMD took: " << t2.count() << endl;         // Displays AVX Time


    }
    else if ((numtype == "fixed") && (numsize == 2))
    {
        short int **in_matrix_cpp;
        short int **in_matrix_cpp_transpose;
        short int **out_matrix_cpp;

        in_matrix_cpp = new short int *[matrix_size];
        in_matrix_cpp_transpose = new short int *[matrix_size];
        out_matrix_cpp = new short int *[matrix_size];

        // construct float matrix for regular cpp
        // also make empty array for out matrix and in matrix transpose
        for (uint i = 0; i < matrix_size; i++)
        {
            in_matrix_cpp[i] = new short int[matrix_size];
            in_matrix_cpp_transpose[i] = new short int[matrix_size];
            out_matrix_cpp[i] = new short int[matrix_size];
            
            for (uint j = 0; j < matrix_size; j++)
            {
                in_matrix_cpp[i][j] = get_int();
                in_matrix_cpp_transpose[i][j] = 0;
                out_matrix_cpp[i][j] = 0;
            }
        }
        // transpose in matrix for computation in avx multiply
        transpose(in_matrix_cpp, in_matrix_cpp_transpose,matrix_size);
        

        high_resolution_clock::time_point start1 = high_resolution_clock::now(); // Starts CPP Time
        
        multiply_cpp(output, in_matrix_cpp, out_matrix_cpp, matrix_size, false); // CPP Multiply
        
        high_resolution_clock::time_point end1 = high_resolution_clock::now();   // Stops CPP Time
        duration<double, std::milli> t1 = (end1 - start1) / 1000;
        cout << "Multiplication without SIMD took: " << t1.count() << endl;      // Displays CPP Time
        
        
        high_resolution_clock::time_point start2 = high_resolution_clock::now(); // Starts AVX Time
        
        multiply_avx(output, out_matrix_cpp, in_matrix_cpp, in_matrix_cpp_transpose, matrix_size, false, false); // AVX Multiply
        
        high_resolution_clock::time_point end2 = high_resolution_clock::now();   // Stops AVX Time
        duration<double, std::milli> t2 = (end2 - start2) / 1000;
        cout << "Multiplication using SIMD took: " << t2.count() << endl;        // Displays AVX Time
    }
    else
    {
        cout << "Functionality not added yet." << endl;
        output << "Functionality not added yet." << endl;
    }
    
    cout << endl;
    output << endl;

    output.close();
    return 0;
};

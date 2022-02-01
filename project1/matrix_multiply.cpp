#include <malloc.h>
#include <iostream>
#include <iomanip>        // setw
#include <stdio.h>
#include <stdlib.h>
#include <ctime>          // Timekeeping
#include <ratio>          // Timekeeping
#include <chrono>         // Timekeeping
#include <fstream>        // input output files
#include <immintrin.h>    // avx stuff
#include <cmath>          // for floor
#include <sys/resource.h>
#ifdef __GNUC__
  #define ALIGN(x) x __attribute__((aligned(32)))
#elif defined(_MSC_VER)
  #define ALIGN(x) __declspec(align(32))
#endif
#pragma comment(linker, "/STACK:2000000000")
#pragma comment(linker, "/HEAP:2000000000")
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;

// Useful Links:
// https://www.cs.fsu.edu/~engelen/courses/HPC-adv/intref_cls.pdf
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956&techs=AVX&text=mult
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956,6144&techs=AVX&text=set_ps
// https://chryswoods.com/vector_c++/immintrin.html
// https://www.codeproject.com/Articles/874396/Crunching-Numbers-with-AVX-and-AVX

// VERY useful link to initializing intel intrinsics
// https://github.com/Triple-Z/AVX-AVX2-Example-Code#initialization-intrinsics
// VERY useful for optimization
// https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
/* in launch
 to compile:  g++ -mavx2 -Wall -O3 -g matrix_multiply.cpp -o matrix_multiply.out
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

// my function prototypes (will be using overloading)
float get_float();
short int get_int();

void print_cpp(ofstream &output, float **in_matrix_cpp, uint matrix_size);
void print_cpp(ofstream &output, short int **in_matrix_cpp, uint matrix_size);
void print_cpp(ofstream &output, int **in_matrix_cpp, uint matrix_size);

void transpose(float **in_matrix_cpp, float **in_matrix_cpp_transpose, uint matrix_size);
void transpose(short int **in_matrix_cpp, short int **in_matrix_cpp_transpose, uint matrix_size);

void multiply_avx(ofstream &output, float **in_matrix_cpp, float **in_matrix_cpp_transpose,uint matrix_size, bool print_transpose, bool print_output);
void multiply_avx(ofstream &output, short int **in_matrix_cpp, short int **in_matrix_cpp_transpose,uint matrix_size, bool print_transpose, bool print_output);

void multiply_cpp(ofstream &output, short int **in_matrix_cpp, int **out_matrix_cpp, uint matrix_size, bool print_output);
void multiply_cpp(ofstream &output, short int **in_matrix_cpp, int **out_matrix_cpp, uint matrix_size, bool print_output);

static inline __m256 hsums(__m256 const& v);
static inline float hadd(__m256 const& v);
uint hsum_8x32(__m256i v);

float get_float()
{
    // float will be between 0 and 1.
    float rndflt = float(rand()) / (float(RAND_MAX));
    return rndflt;
}

short int get_int()
{
    // want 2 byte fixed point, so short int
    // float will be between 0 and 1.
    // unsigned short int rndflt = (unsigned short int)(rand())/((unsigned short int)(RAND_MAX));
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

void print_cpp(ofstream &output, int **in_matrix_cpp, uint matrix_size)
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

void multiply_avx(ofstream &output, float **in_matrix_cpp, float **in_matrix_cpp_transpose, uint matrix_size, bool print_transpose = false, bool print_output = false)
{
    // using transpose to minimize cache misses
    // only works on square matrices
    // set print_transpose or print_output to true for printing
    uint num_of_regs = ceil(matrix_size / 8.0);  
    alignas(32) __m256 in_matrix_avxa[matrix_size][num_of_regs];
    alignas(32) __m256 in_matrix_avx_transposea[matrix_size][num_of_regs];

    // take input matrix (vector form) and create the vectorized version using __m256
    // since matrix is square, the indecies are the same for the transpose
    for (uint vert = 0; vert < matrix_size; vert++)
    {   // need to double reg size because only half of the data is multiplied at once 
        for (uint horiz = 0; horiz < num_of_regs; horiz++)
        {
            // looping through each element in register and checking if it exists
            static ALIGN(float temp1[8]);
            static ALIGN(float temp1_t[8]);
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
                    temp1[reg_i] = 0.0;
                    temp1_t[reg_i] = 0.0;
                    if (print_transpose)
                    {
                        cout << left << setfill(' ') << setw(11);
                        cout << " 0 ";
                        output << left << setfill(' ') << setw(11);
                        output << " 0 ";
                    }
                }
            }
            // seg fault here with large matrices. Can't figure out the issue. Neither could prof.             
            in_matrix_avxa[vert][horiz] = _mm256_loadu_ps(temp1);
            in_matrix_avx_transposea[vert][horiz] = _mm256_loadu_ps(temp1_t);

        };
        if (print_transpose)
        {
            cout << endl;
            cout << endl;
            output << endl;
            output << endl;
        }
    };
    //cout << " the transpose has been processed " << endl;
    // compute the multiplication using in_matrix and in_matrix_transpose
    // must loop through all rows of first matrix
    float sum = 0;
    __m256 vector_sum = _mm256_set_ps(0, 0, 0, 0, 0, 0, 0, 0);
    for (uint vert = 0; vert < matrix_size; vert++)
    {
        // must loop through all rows of second matrix
        for (uint vert_t = 0; vert_t < matrix_size; vert_t++)
        {
            vector_sum = _mm256_set_ps(0, 0, 0, 0, 0, 0, 0, 0);
            // only need to loop through columns once because the indecies must be aligned
            for (uint horiz = 0; horiz < num_of_regs; horiz++)
            {
                // multiply every register in in matrix with every register in in matrix transpose
                // then sum together and put into output matrix

                __m256 mulresult = _mm256_mul_ps(in_matrix_avxa[vert][horiz],in_matrix_avx_transposea[vert_t][horiz]);
                //cout << " " << result[0] << " "<< result[1] << " " << result[2] << " "<< result[3]<< " "<< result[4]<< " "<< result[5] << " "<< result[6] << " "<< result[7] << endl;
                //sum += result[0] + result[1] + result[2] + result[3] + result[4] + result[5] + result[6] + result[7];
                // Horizontally sums the multiplied register, adds to the output
                vector_sum = _mm256_add_ps(vector_sum, mulresult);
            }
            sum = hadd(vector_sum);
            if (print_output)
            {
                cout << left << setfill(' ') << setw(11);
                cout << sum;
                output << left << setfill(' ') << setw(11);
                output << sum;
            }
        }
        if (print_output)
        {
            cout << endl;
            output << endl;
        }
    };
};

void multiply_avx(ofstream &output, short int **in_matrix_cpp, short int **in_matrix_cpp_transpose,uint matrix_size, bool print_transpose = false, bool print_output = false)
{
    // using transpose to minimize cache misses
    // only works on square matrices
    // set print_transpose or print_output to true for printing
    
    uint num_of_regs = ceil(matrix_size / 8.0);
    
    __m256i **in_matrix_avx;
    __m256i **in_matrix_avx_transpose;
    
    in_matrix_avx = new __m256i *[matrix_size];
    in_matrix_avx_transpose = new __m256i *[matrix_size];
    
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
            //temp1 = new short int[matrix_size];
            temp1 = new short int[8];
            short int *temp1_t;
            //temp1_t = new short int[matrix_size];
            temp1_t = new short int[8];
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
    cout << " the transpose has been processed " << endl;
    //short int sum2 = 0;
    //__m256i vector_sum = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, 0);
    // compute the multiplication using in_matrix and in_matrix_transpose
    // must loop through all rows of first matrix
    for (uint vert = 0; vert < matrix_size; vert++)
    {
        // must loop through all rows of second matrix
        for (uint vert_t = 0; vert_t < matrix_size; vert_t++)
        {
            int sum = 0;
            //vector_sum = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, 0);
            // only need to loop through columns once because the indecies must be aligned
            for (uint horiz = 0; horiz < num_of_regs; horiz++)
            {
                // multiply every register in in matrix with every register in in matrix transpose
                // then sum together and put into output matrix
                //__m256i mulresult = _mm256_mullo_epi32(in_matrix_avx[vert][horiz], in_matrix_avx_transpose[vert_t][horiz]);
                //vector_sum = _mm256_add_epi32(vector_sum, mulresult);
                // need pointers to access data so its not garbage
                int *row = (int *)&in_matrix_avx[vert][horiz];
                int *row_t = (int *)&in_matrix_avx_transpose[vert_t][horiz];

                __m256i rowaa = _mm256_setr_epi32(row[0], 0, row[1], 0,
                                                  row[2], 0, row[3], 0);
                __m256i rowab = _mm256_setr_epi32(row[4], 0, row[5], 0,
                                                  row[6], 0, row[7], 0);
                __m256i rowba = _mm256_setr_epi32(row_t[0], 0, row_t[1], 0,
                                                  row_t[2], 0, row_t[3], 0);
                __m256i rowbb = _mm256_setr_epi32(row_t[4], 0, row_t[5], 0,
                                                  row_t[6], 0, row_t[7], 0);
                __m256i resa = _mm256_mul_epi32(rowaa, rowba);
                __m256i resb = _mm256_mul_epi32(rowab, rowbb);

                // need to use some pointer trickery to access the values stored within the resulting __m256i.
                // __m256i in this case stores 4 values which are encoded (or maybe are ram locations) which need to be in readable form
                int *resa_readable = (int *)&resa;
                int *resb_readable = (int *)&resb;
                sum += resa_readable[0] + resa_readable[2] + resa_readable[4] + resa_readable[6];
                sum += resb_readable[0] + resb_readable[2] + resb_readable[4] + resb_readable[6];

            }
            //sum2 = hsum_8x32(vector_sum);
            if (print_output)
            {
                cout << left << setfill(' ') << setw(11);
                cout << sum;
                output << left << setfill(' ') << setw(11);
                output << sum;
            }
        }
        if (print_output)
        {
            cout << endl;
            output << endl;
        }
    };
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

void multiply_cpp(ofstream &output, short int **in_matrix_cpp, int **out_matrix_cpp, uint matrix_size, bool print_output = false)
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
            int temp = 0;
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

// Obtains the horizontal sum of all ints in a __m256i
uint32_t hsum_8x32(__m256i v)
{
    __m128i sum128 = _mm_add_epi32(
                 _mm256_castsi256_si128(v),
                 _mm256_extracti128_si256(v, 1));
    __m128i hi64  = _mm_unpackhi_epi64(sum128, sum128);           // 3-operand non-destructive AVX lets us save a byte without needing a movdqa
    __m128i sum64 = _mm_add_epi32(hi64, sum128);
    __m128i hi32  = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));    // Swap the low two elements
    __m128i sum32 = _mm_add_epi32(sum64, hi32);
    return _mm_cvtsi128_si32(sum32);
}


int main(int argc, char *argv[])
{
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

    // output << "abcd" << endl;
    cout << "Inputs Accepted!" << endl;
    cout << "matrix size: " << matrix_size << " x " << matrix_size << endl;
    cout << "number type " << numtype << " " << numsize << " bytes " << endl;
    output << "Inputs Accepted!" << endl;
    output << "matrix size: " << matrix_size << " x " << matrix_size << endl;
    output << "number type " << numtype << " " << numsize << " bytes " << endl;

    if ((numtype == "float") && (numsize == 4))
    {
        float **in_matrix_cpp;
        float **in_matrix_cpp_transpose;
        float **out_matrix_cpp;

        in_matrix_cpp = new float *[matrix_size];
        in_matrix_cpp_transpose = new float *[matrix_size];
        out_matrix_cpp = new float *[matrix_size];

        // construct float matrix for regular cpp
        // also make empty array for out matrix and in matrix transpose

        for (uint i = 0; i < matrix_size; i++)
        {
            in_matrix_cpp[i] = new float [matrix_size];
            in_matrix_cpp_transpose[i] = new float [matrix_size];
            out_matrix_cpp[i] = new float [matrix_size];
            
            for (uint j = 0; j < matrix_size; j++)
            {
                in_matrix_cpp[i][j] = get_float();
                in_matrix_cpp_transpose[i][j] = 0.0;
                out_matrix_cpp[i][j] = 0.0;
            }
        }
        // transpose in matrix for computation in avx multiply
        //print_cpp(output,in_matrix_cpp,matrix_size);
        transpose(in_matrix_cpp, in_matrix_cpp_transpose,matrix_size);
        //print_cpp(output,in_matrix_cpp_transpose,matrix_size);
        // construct __m256 matrix
        // need to pad with 0's at the end if not perfectly divisable
        // looping through the required registers
        // [(x x x x x x x x) (x x x x x x 0 0)] <- two 8 float registers | two vectors down
        // [(x x x x x x x x) (x x x x x x 0 0)]                          |
        //                  ^ one vectotr wide

        // cout << "input matrix: " << endl;
        // cout << endl;
        // output << "input matrix: " << endl;
        // output << endl;
        // print_cpp(output, in_matrix_cpp);

        // cout << "transpose matrix: " << endl;
        // cout << endl;
        // output << "transpose matrix: " << endl;
        // output << endl;
        // print_cpp(output, in_matrix_cpp_transpose);

        // cout << "squaring results using cpp: " << endl;
        // cout << endl;
        // output << "squaring results using cpp: " << endl;
        // output << endl;

        high_resolution_clock::time_point start1 = high_resolution_clock::now(); // Starts CPP Time
        //multiply_cpp(output, in_matrix_cpp, out_matrix_cpp, matrix_size, false);
        //multiply_cpp(output,in_matrix_cpp,out_matrix_cpp,matrix_size,false);
        high_resolution_clock::time_point end1 = high_resolution_clock::now();   // Stops CPP Time
        duration<double, std::milli> t1 = (end1 - start1) / 1000;
        cout << "cpp multiplication took: " << t1.count() << endl;               // Displays CPP Time
        /*
        cout << "squaring results using avx: " << endl;
        cout << endl;
        output << "squaring results using avx: " << endl;
        output << endl;
        */
        high_resolution_clock::time_point start2 = high_resolution_clock::now(); // Starts AVX Time
        multiply_avx(output, in_matrix_cpp, in_matrix_cpp_transpose, matrix_size, false, false); // AVX Multiply
        high_resolution_clock::time_point end2 = high_resolution_clock::now();   // Stops AVX Time
        duration<double, std::milli> t2 = (end2 - start2) / 1000;
        cout << "avx multiplication took: " << t2.count() << endl;               // Displays AVX Time
    }
    else if ((numtype == "fixed") && (numsize == 2))
    {
        short int **in_matrix_cpp;
        short int **in_matrix_cpp_transpose;
        int **out_matrix_cpp;
        int **out_matrix_avx;

        in_matrix_cpp = new short int *[matrix_size];
        in_matrix_cpp_transpose = new short int *[matrix_size];
        out_matrix_cpp = new int *[matrix_size];

        // construct float matrix for regular cpp
        // also make empty array for out matrix and in matrix transpose

        for (uint i = 0; i < matrix_size; i++)
        {
            in_matrix_cpp[i] = new short int[matrix_size];
            in_matrix_cpp_transpose[i] = new short int[matrix_size];
            out_matrix_cpp[i] = new int[matrix_size];
            
            for (uint j = 0; j < matrix_size; j++)
            {
                in_matrix_cpp[i][j] = get_int();
                in_matrix_cpp_transpose[i][j] = 0;
                out_matrix_cpp[i][j] = 0;
            }
        }
        // transpose in matrix for computation in avx multiply
        transpose(in_matrix_cpp, in_matrix_cpp_transpose,matrix_size);

        // construct __m256 matrix
        // need to pad with 0's at the end if not perfectly divisable
        // looping through the required registers
        // [(x x x x x x x x) (x x x x x x 0 0)] <- two 8 float registers | two vectors down
        // [(x x x x x x x x) (x x x x x x 0 0)]                          |
        //                  ^ one vectotr wide

        // cout << "input matrix: " << endl;
        // cout << endl;
        // output << "input matrix: " << endl;
        // output << endl;
        // print_cpp(output, in_matrix_cpp);

        // cout << "transpose matrix: " << endl;
        // cout << endl;
        // output << "transpose matrix: " << endl;
        // output << endl;
        // print_cpp(output, in_matrix_cpp_transpose);

        // cout << "squaring results using cpp: " << endl;
        // cout << endl;
        // output << "squaring results using cpp: " << endl;
        // output << endl;

        high_resolution_clock::time_point start1 = high_resolution_clock::now(); // Starts CPP Time
        //multiply_cpp(output, in_matrix_cpp, out_matrix_cpp, matrix_size, false);
        //multiply_cpp(output,in_matrix_cpp,out_matrix_cpp,matrix_size,false);
        high_resolution_clock::time_point end1 = high_resolution_clock::now();   // Stops CPP Time
        duration<double, std::milli> t1 = (end1 - start1) / 1000;
        cout << "cpp multiplication took: " << t1.count() << endl;               // Displays CPP Time
        /*
        cout << "squaring results using avx: " << endl;
        cout << endl;
        output << "squaring results using avx: " << endl;
        output << endl;
        */
        high_resolution_clock::time_point start2 = high_resolution_clock::now(); // Starts AVX Time
        multiply_avx(output, in_matrix_cpp, in_matrix_cpp_transpose, matrix_size, false, false); // AVX Multiply
        high_resolution_clock::time_point end2 = high_resolution_clock::now();   // Stops AVX Time
        duration<double, std::milli> t2 = (end2 - start2) / 1000;
        cout << "avx multiplication took: " << t2.count() << endl;               // Displays AVX Time
    }
    else
    {
        cout << "funcitonality not added yet" << endl;
        output << "funcitonality not added yet" << endl;
    }

    output.close();
    return 0;
};

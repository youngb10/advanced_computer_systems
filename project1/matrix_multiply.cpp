#include <iostream>
#include <iomanip> // setw
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <fstream>     // input output files
#include <immintrin.h> // avx stuff
#include <xmmintrin.h>
#include <cmath> // floor
#include <sys/resource.h>
using namespace std;

// Useful Links:
// https://www.cs.fsu.edu/~engelen/courses/HPC-adv/intref_cls.pdf
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956&techs=AVX&text=mult
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956,6144&techs=AVX&text=set_ps
// https://chryswoods.com/vector_c++/immintrin.html

// VERY useful link to initializing intel intrinsics
// https://github.com/Triple-Z/AVX-AVX2-Example-Code#initialization-intrinsics

/* in launch
 to compile: g++ -mavx2 -Wall -g matrix_multiply.cpp -o matrix_multiply.out
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

// external avx library functions
// float versions
__m256 _mm256_set1_ps(float a);
__m256 _mm256_set_ps(float e7, float e6, float e5, float e4, float e3, float e2, float e1, float e0);
__m256 _mm256_mul_ps(__m256 a, __m256 b);
void _mm256_storeu_ps(float *mem_addr, __m256 a);

// integer versions
__m256i _mm256_set_epi32(int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0);
__m256i _mm256_mullo_epi32(__m256i a, __m256i b);
void _mm256_store_si256(__m256i *mem_addr, __m256i a);
// can upgrade to short int version, but will require some more code modification
__m256 _mm256_castsi256_ps(__m256i a);
__m256i _mm256_castps_si256(__m256 a);
__m256i _mm256_setr_epi32(int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0);
void _mm256_storeu_si256(__m256i *mem_addr, __m256i a);

// my function prototypes (will be using overloading)
float get_float();
short int get_int();
void print_cpp(ofstream &output, float **in_matrix_cpp, uint matrix_size);
void print_cpp(ofstream &output, short int **in_matrix_cpp, uint matrix_size);
void print_cpp(ofstream &output, int **in_matrix_cpp, uint matrix_size);
void transpose(float **in_matrix_cpp, float **in_matrix_cpp_transpose, uint matrix_size);
void transpose(short int **in_matrix_cpp, short int **in_matrix_cpp_transpose, uint matrix_size);
void multiply_cpp(ofstream &output, short int **in_matrix_cpp, int **out_matrix_cpp, uint matrix_size, bool print_output);
void multiply_cpp(ofstream &output, short int **in_matrix_cpp, int **out_matrix_cpp, uint matrix_size, bool print_output);

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

void multiply_avx(ofstream &output, float **in_matrix_cpp, float **in_matrix_cpp_transpose,float **out_matrix,uint matrix_size, bool print_transpose = false, bool print_output = false)
{
    // using transpose to minimize cache misses
    // only works on square matrices
    // set print_transpose or print_output to true for printing
    uint num_of_regs = ceil(matrix_size / 8.0);
    __m256 in_matrix_avx[matrix_size][num_of_regs];
    __m256 out_matrix_avx[matrix_size][num_of_regs];
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
                        output << left << setfill(' ') << setw(11);
                        output << in_matrix_cpp[vert][real_i];
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
                        output << left << setfill(' ') << setw(11);
                        output << " 0 ";
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
            output << endl;
            output << endl;
        }
    };

    // compute the multiplication using in_matrix and in_matrix_transpose
    // must loop through all rows of first matrix
    for (uint vert = 0; vert < matrix_size; vert++)
    {
        // must loop through all rows of second matrix
        for (uint vert_t = 0; vert_t < matrix_size; vert_t++)
        {
            float sum = 0;
            // only need to loop through columns once because the indecies must be aligned
            for (uint horiz = 0; horiz < num_of_regs; horiz++)
            {
                // multiply every register in in matrix with every register in in matrix transpose
                // then sum together and put into output matrix
                // __m256 _mm256_mul_ps (__m256 a, __m256 b);
                __m256 result = _mm256_mul_ps(in_matrix_avx[vert][horiz], in_matrix_avx_transpose[vert_t][horiz]);
                float result_flt[8];
                _mm256_storeu_ps(result_flt, result);
                // cout << result_flt[0] << " " << result_flt[1] << " " << result_flt[2] << " " << result_flt[3] << " ";
                // cout << result_flt[4] << " " << result_flt[5] << " " << result_flt[6] << " " << result_flt[7] << " ";
                for (uint i = 0; i < 8; i++)
                {
                    sum += result_flt[i];
                }
            }
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

void multiply_avx(ofstream &output, short int **in_matrix_cpp, short int **in_matrix_cpp_transpose,int **out_matrix, uint matrix_size, bool print_transpose = false, bool print_output = false)
{
    // using transpose to minimize cache misses
    // only works on square matrices
    // set print_transpose or print_output to true for printing
    
    uint num_of_regs = ceil(matrix_size / 8.0);
    
    __m256i **in_matrix_avx;
    __m256i **in_matrix_avx_transpose;
    __m256i **out_matrix_avx;

    in_matrix_avx = new __m256i *[matrix_size];
    in_matrix_avx_transpose = new __m256i *[matrix_size];
    out_matrix_avx = new __m256i *[matrix_size];

    // take input matrix (vector form) and create the vectorized version using __m256
    // since matrix is square, the indecies are the same for the transpose
    for (uint vert = 0; vert < matrix_size; vert++)
    {   // need to double reg size because only half of the data is multiplied at once 
        in_matrix_avx[vert] = new __m256i[num_of_regs*2];
        in_matrix_avx_transpose[vert] = new __m256i[num_of_regs*2];
        out_matrix_avx[vert] = new __m256i[num_of_regs*2];
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
        };
        if (print_transpose)
        {
            cout << endl;
            cout << endl;
            output << endl;
            output << endl;
        }
    };

    // compute the multiplication using in_matrix and in_matrix_transpose
    // must loop through all rows of first matrix
    for (uint vert = 0; vert < matrix_size; vert++)
    {
        // must loop through all rows of second matrix
        for (uint vert_t = 0; vert_t < matrix_size; vert_t++)
        {
            int sum = 0;
            // only need to loop through columns once because the indecies must be aligned
            for (uint horiz = 0; horiz < num_of_regs; horiz++)
            {
                // multiply every register in in matrix with every register in in matrix transpose
                // then sum together and put into output matrix

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

                /*
                // Needed debugging, please don't delete. was a pain to make.
                if( true ){
                    cout << endl;
                    int* j = (int*) &rowaa;
                    printf("rowaa:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", j[0], j[1], j[2], j[3], j[4], j[5], j[6], j[7]);
                    int* x = (int*) &rowab;
                    printf("rowab:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7]);
                    int* y = (int*) &rowba;
                    printf("rowba:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", y[0], y[1], y[2], y[3], y[4], y[5], y[6], y[7]);
                    int* z = (int*) &rowbb;
                    printf("rowbb:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", z[0], z[1], z[2], z[3], z[4], z[5], z[6], z[7]);
                    int* k = (int*) &resa;
                    printf("resa:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", k[0], k[1], k[2], k[3], k[4], k[5], k[6], k[7]);
                    int* l = (int*) &resb;
                    printf("resb:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7]);
                }
                else{
                    int* j = (int*) &rowaa;
                    //printf("rowaa:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", j[0], j[1], j[2], j[3], j[4], j[5], j[6], j[7]);
                    int* x = (int*) &rowab;
                    //printf("rowab:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7]);
                    int* y = (int*) &rowba;
                    //printf("rowba:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", y[0], y[1], y[2], y[3], y[4], y[5], y[6], y[7]);
                    int* z = (int*) &rowbb;
                    //printf("rowbb:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", z[0], z[1], z[2], z[3], z[4], z[5], z[6], z[7]);
                    int* k = (int*) &resa;
                    //printf("resa:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", k[0], k[1], k[2], k[3], k[4], k[5], k[6], k[7]);
                    int* l = (int*) &resb;
                    //printf("resb:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7]);
                    sum += k[0] + k[2] + k[4] + k[6];
                    sum += l[0] + l[2] + l[4] + l[6];
                }
                */
            }
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

    // output << "abcd" << endl;
    cout << "Inputs Accepted!" << endl;
    cout << "matrix size: " << matrix_size << " x " << matrix_size << endl;
    cout << "number type " << numtype << " " << numsize << " bytes " << endl;
    output << "Inputs Accepted!" << endl;
    output << "matrix size: " << matrix_size << " x " << matrix_size << endl;
    output << "number type " << numtype << " " << numsize << " bytes " << endl;

    // divide by 8.0 to convert to float
    uint num_of_regs = ceil(matrix_size / 8.0);

    if ((numtype == "float") && (numsize == 4))
    {

        float **in_matrix_cpp;
        float **in_matrix_cpp_transpose;
        float **out_matrix_cpp;
        float **out_matrix_avx;

        in_matrix_cpp = new float *[matrix_size];
        in_matrix_cpp_transpose = new float *[matrix_size];
        out_matrix_cpp = new float *[matrix_size];
        out_matrix_avx = new float *[matrix_size];

        // construct float matrix for regular cpp
        // also make empty array for out matrix and in matrix transpose
        for (uint i = 0; i < matrix_size; i++)
        {
            in_matrix_cpp[i] = new float[matrix_size];
            in_matrix_cpp_transpose[i] = new float [matrix_size];
            out_matrix_cpp[i] = new float[matrix_size];
            out_matrix_avx[i] = new float[matrix_size];
            
            for (uint j = 0; j < matrix_size; j++)
            {
                in_matrix_cpp[i][j] = get_int();
                in_matrix_cpp_transpose[i][j] = 0;
                out_matrix_cpp[i][j] = 0;
                out_matrix_avx[i][j] = 0;
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

        long long start1 = time(NULL);
        multiply_cpp(output, in_matrix_cpp, out_matrix_cpp, matrix_size, true);
        // multiply_cpp(output, in_matrix_cpp,out_matrix_cpp,true);
        long long end1 = time(NULL);
        cout << "cpp multiplication took: " << end1 - start1 << endl;
        /*
        cout << "squaring results using avx: " << endl;
        cout << endl;
        output << "squaring results using avx: " << endl;
        output << endl;
        */
        long long start2 = time(NULL);
        multiply_avx(output, in_matrix_cpp, in_matrix_cpp_transpose, out_matrix_avx, matrix_size, false, true);
        // multiply_avx(output, in_matrix_cpp,in_matrix_cpp_transpose,out_matrix_avx,false,true);
        long long end2 = time(NULL);
        cout << "avx multiplication took: " << end2 - start2 << endl;

        /*
        cout << "input matrix: " << endl;
        cout << endl;
        output << "input matrix: " << endl;
        output << endl;
        print_cpp(output, in_matrix_cpp);

        cout << "transpose matrix: " << endl;
        cout << endl;
        output << "transpose matrix: " << endl;
        output << endl;
        print_cpp(output, in_matrix_cpp_transpose);

        cout << "squaring results using cpp: " << endl;
        cout << endl;
        output << "squaring results using cpp: " << endl;
        output << endl;
        multiply_cpp(output, in_matrix_cpp,out_matrix_cpp,true);

        cout << "squaring results using avx: " << endl;
        cout << endl;
        output << "squaring results using avx: " << endl;
        output << endl;
        multiply_avx(output, in_matrix_cpp,in_matrix_cpp_transpose,out_matrix_avx,false,true);
        */
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
        out_matrix_avx = new int *[matrix_size];

        // construct float matrix for regular cpp
        // also make empty array for out matrix and in matrix transpose

        for (uint i = 0; i < matrix_size; i++)
        {
            in_matrix_cpp[i] = new short int[matrix_size];
            in_matrix_cpp_transpose[i] = new short int[matrix_size];
            out_matrix_cpp[i] = new int[matrix_size];
            out_matrix_avx[i] = new int[matrix_size];
            
            for (uint j = 0; j < matrix_size; j++)
            {
                in_matrix_cpp[i][j] = get_int();
                in_matrix_cpp_transpose[i][j] = 0;
                out_matrix_cpp[i][j] = 0;
                out_matrix_avx[i][j] = 0;
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

        long long start1 = time(NULL);
        //multiply_cpp(output, in_matrix_cpp, out_matrix_cpp, matrix_size, false);
        // multiply_cpp(output, in_matrix_cpp,out_matrix_cpp,true);
        long long end1 = time(NULL);
        cout << "cpp multiplication took: " << end1 - start1 << endl;
        /*
        cout << "squaring results using avx: " << endl;
        cout << endl;
        output << "squaring results using avx: " << endl;
        output << endl;
        */
        long long start2 = time(NULL);
        cout << "123" << endl;
        multiply_avx(output, in_matrix_cpp, in_matrix_cpp_transpose, out_matrix_avx, matrix_size, false, false);
        cout << "abcd" << endl;
        //multiply_avx(output, in_matrix_cpp,in_matrix_cpp_transpose,out_matrix_avx,false,true);
        long long end2 = time(NULL);
        cout << "avx multiplication took: " << end2 - start2 << endl;
    }
    else
    {
        cout << "funcitonality not added yet" << endl;
        output << "funcitonality not added yet" << endl;
    }

    output.close();
    return 0;
};

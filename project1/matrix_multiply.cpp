#include <iostream>
#include <iomanip> // setw
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <fstream> // input output files
#include <immintrin.h> // avx stuff
#include <xmmintrin.h>
#include <cmath> // floor
using namespace std;

// Useful Links:
    // https://www.cs.fsu.edu/~engelen/courses/HPC-adv/intref_cls.pdf
    // https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956&techs=AVX&text=mult
    // https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956,6144&techs=AVX&text=set_ps
    // https://chryswoods.com/vector_c++/immintrin.html

/* in launch
 to compile: g++ -mavx -Wall -g matrix_multiply.cpp -o matrix_multiply.out
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
__m256 _mm256_set1_ps (float a);
__m256 _mm256_set_ps (float e7, float e6, float e5, float e4, float e3, float e2, float e1, float e0);
__m256 _mm256_mul_ps (__m256 a, __m256 b);
void _mm256_storeu_ps (float * mem_addr, __m256 a);

// integer versions 
__m256i _mm256_set_epi32 (int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0);
// can upgrade to short int version, but will require some more code modification
__m256 _mm256_castsi256_ps (__m256i a);
__m256i _mm256_castps_si256 (__m256 a);
__m256i _mm256_setr_epi32 (int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0);
void _mm256_storeu_si256 (__m256i * mem_addr, __m256i a);

// my function prototypes (will be using overloading)
float get_float();
short int get_uint();
void print_cpp(ofstream &output, vector<vector<float>> & in_matrix_cpp);
void print_cpp(ofstream &output, vector<vector<unsigned short int>> & in_matrix_cpp);
void transpose(vector<vector<float>> & in_matrix_cpp, vector<vector<float>> & in_matrix_cpp_transpose);
void transpose(vector<vector<unsigned short int>> & in_matrix_cpp, vector<vector<unsigned short int>> & in_matrix_cpp_transpose);
void multiply_cpp(ofstream &output, vector<vector<float>> & in_matrix_cpp, vector<vector<float>> & out_matrix_cpp, bool print_output);
void multiply_cpp(ofstream &output, vector<vector<unsigned short int>> & in_matrix_cpp, vector<vector<unsigned short int>> & out_matrix_cpp, bool print_output);

float get_float(){
    // float will be between 0 and 1. 
    float rndflt = float(rand())/(float(RAND_MAX));
    return rndflt;
}

short int get_uint(){
    // want 2 byte fixed point, so short int
    // float will be between 0 and 1. 
    short int rndflt = (short int)(rand())/((short int)(RAND_MAX));
    return rndflt;
}

void print_cpp(ofstream &output, vector<vector<float>> & in_matrix_cpp){
    for(uint i = 0; i < in_matrix_cpp.size(); i++){
        for(uint j = 0; j < in_matrix_cpp.size(); j++){
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

void print_cpp(ofstream &output, vector<vector<unsigned short int>> & in_matrix_cpp){
    for(uint i = 0; i < in_matrix_cpp.size(); i++){
        for(uint j = 0; j < in_matrix_cpp.size(); j++){
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

void transpose(vector<vector<float>> & in_matrix_cpp, vector<vector<float>> & in_matrix_cpp_transpose){
    // creates transpose of input matrix
    // this is used for processing in multiplying avx
    for(uint i = 0; i < in_matrix_cpp.size(); i++){
        for(uint j = 0; j < in_matrix_cpp.size(); j++){
            in_matrix_cpp_transpose[j][i] = in_matrix_cpp[i][j];
        }
    }
};

void transpose(vector<vector<unsigned short int>> & in_matrix_cpp, vector<vector<unsigned short int>> & in_matrix_cpp_transpose){
    // creates transpose of input matrix
    // this is used for processing in multiplying avx
    for(uint i = 0; i < in_matrix_cpp.size(); i++){
        for(uint j = 0; j < in_matrix_cpp.size(); j++){
            in_matrix_cpp_transpose[j][i] = in_matrix_cpp[i][j];
        }
    }
};

void multiply_avx(ofstream &output, vector<vector<float>> & in_matrix_cpp, vector<vector<float>> & in_matrix_cpp_transpose, vector<vector<float>> & out_matrix, bool print_transpose = false, bool print_output = false){
    // using transpose to minimize cache misses
    // only works on square matrices
    // set print_transpose or print_output to true for printing 
    uint matrix_size = in_matrix_cpp.size();
    uint num_of_regs = ceil(matrix_size/8.0);
    __m256 in_matrix_avx [matrix_size][num_of_regs];
    __m256 out_matrix_avx [matrix_size][num_of_regs];
    __m256 in_matrix_avx_transpose [matrix_size][num_of_regs];
    // take input matrix (vector form) and create the vectorized version using __m256
    // since matrix is square, the indecies are the same for the transpose 
    for(uint vert = 0; vert < matrix_size; vert++){
        for(uint horiz = 0; horiz < num_of_regs; horiz++){
            // looping through each element in register and checking if it exists
            //cout << "vert " << vert << " horiz " << horiz << endl; 
            vector<float> temp_vector;
            vector<float> temp_vector_transpose;
            for(uint reg_i = 0; reg_i < 8; reg_i++){
                uint real_i = horiz*8 + reg_i;
                //cout << real_i << " ";
                if(real_i < matrix_size){
                    temp_vector.push_back(in_matrix_cpp[vert][real_i]);
                    temp_vector_transpose.push_back(in_matrix_cpp_transpose[vert][real_i]);
                    if(print_transpose){
                        cout << left << setfill(' ') << setw(11);
                        cout << in_matrix_cpp[vert][real_i];
                        output << left << setfill(' ') << setw(11);
                        output << in_matrix_cpp[vert][real_i];
                    }
                }
                else{
                    temp_vector.push_back(0);
                    temp_vector_transpose.push_back(0);
                    if(print_transpose){
                        cout << left << setfill(' ') << setw(11);
                        cout << " 0 ";
                        output << left << setfill(' ') << setw(11);
                        output << " 0 ";
                    }
                }
            }
            *(*(in_matrix_avx + vert) + horiz) = _mm256_set_ps(temp_vector[7],temp_vector[6],temp_vector[5],temp_vector[4],
                                                               temp_vector[3],temp_vector[2],temp_vector[1],temp_vector[0]);
            *(*(in_matrix_avx_transpose + vert) + horiz) = _mm256_set_ps(temp_vector_transpose[7],temp_vector_transpose[6],temp_vector_transpose[5],temp_vector_transpose[4],
                                                               temp_vector_transpose[3],temp_vector_transpose[2],temp_vector_transpose[1],temp_vector_transpose[0]);
        };
        if(print_transpose){
            cout << endl;
            cout << endl;
            output << endl;
            output << endl;
        }
    };
    
    // compute the multiplication using in_matrix and in_matrix_transpose 
    // must loop through all rows of first matrix
    for(uint vert = 0; vert < matrix_size; vert++){
        // must loop through all rows of second matrix 
        for(uint vert_t = 0; vert_t < matrix_size; vert_t++){
            float sum = 0;
            // only need to loop through columns once because the indecies must be aligned
            for(uint horiz = 0; horiz < num_of_regs; horiz++){
                // multiply every register in in matrix with every register in in matrix transpose
                // then sum together and put into output matrix 
                // __m256 _mm256_mul_ps (__m256 a, __m256 b);
                __m256 result = _mm256_mul_ps(in_matrix_avx[vert][horiz],in_matrix_avx_transpose[vert_t][horiz]);
                float result_flt[8];
                _mm256_storeu_ps(result_flt, result);
                //cout << result_flt[0] << " " << result_flt[1] << " " << result_flt[2] << " " << result_flt[3] << " ";
                //cout << result_flt[4] << " " << result_flt[5] << " " << result_flt[6] << " " << result_flt[7] << " ";
                for(uint i = 0; i < 8; i++){
                    sum += result_flt[i];
                }
            }
            if(print_output){
                cout << left << setfill(' ') << setw(11);
                cout << sum;
                output << left << setfill(' ') << setw(11);
                output << sum;
            }
        }
        if(print_output){
            cout << endl;
            output << endl;
        }
    };
};

void multiply_avx(ofstream &output, vector<vector<unsigned short int>> & in_matrix_cpp, vector<vector<unsigned short int>> & in_matrix_cpp_transpose, vector<vector<unsigned short int>> & out_matrix, bool print_transpose = false, bool print_output = false){
    // using transpose to minimize cache misses
    // only works on square matrices
    // set print_transpose or print_output to true for printing 
    uint matrix_size = in_matrix_cpp.size();
    uint num_of_regs = ceil(matrix_size/8.0);
    __m256i in_matrix_avx [matrix_size][num_of_regs];
    __m256i out_matrix_avx [matrix_size][num_of_regs];
    __m256i in_matrix_avx_transpose [matrix_size][num_of_regs];
    // take input matrix (vector form) and create the vectorized version using __m256
    // since matrix is square, the indecies are the same for the transpose 
    for(uint vert = 0; vert < matrix_size; vert++){
        for(uint horiz = 0; horiz < num_of_regs; horiz++){
            // looping through each element in register and checking if it exists
            //cout << "vert " << vert << " horiz " << horiz << endl; 
            vector<unsigned short int> temp_vector;
            vector<unsigned short int> temp_vector_transpose;
            for(uint reg_i = 0; reg_i < 8; reg_i++){
                uint real_i = horiz*8 + reg_i;
                //cout << real_i << " ";
                if(real_i < matrix_size){
                    temp_vector.push_back(in_matrix_cpp[vert][real_i]);
                    temp_vector_transpose.push_back(in_matrix_cpp_transpose[vert][real_i]);
                    if(print_transpose){
                        cout << left << setfill(' ') << setw(11);
                        cout << in_matrix_cpp[vert][real_i];
                        output << left << setfill(' ') << setw(11);
                        output << in_matrix_cpp[vert][real_i];
                    }
                }
                else{
                    temp_vector.push_back(0);
                    temp_vector_transpose.push_back(0);
                    if(print_transpose){
                        cout << left << setfill(' ') << setw(11);
                        cout << " 0 ";
                        output << left << setfill(' ') << setw(11);
                        output << " 0 ";
                    }
                }
            }
            *(*(in_matrix_avx + vert) + horiz) = _mm256_setr_epi32(temp_vector[7],temp_vector[6],temp_vector[5],temp_vector[4],
                                                                  temp_vector[3],temp_vector[2],temp_vector[1],temp_vector[0]);
            *(*(in_matrix_avx_transpose + vert) + horiz) = _mm256_setr_epi32(temp_vector_transpose[7],temp_vector_transpose[6],temp_vector_transpose[5],temp_vector_transpose[4],
                                                                             temp_vector_transpose[3],temp_vector_transpose[2],temp_vector_transpose[1],temp_vector_transpose[0]);
        };
        if(print_transpose){
            cout << endl;
            cout << endl;
            output << endl;
            output << endl;
        }
    };
    
    // compute the multiplication using in_matrix and in_matrix_transpose 
    // must loop through all rows of first matrix
    for(uint vert = 0; vert < matrix_size; vert++){
        // must loop through all rows of second matrix 
        for(uint vert_t = 0; vert_t < matrix_size; vert_t++){
            unsigned short int sum = 0;
            // only need to loop through columns once because the indecies must be aligned
            for(uint horiz = 0; horiz < num_of_regs; horiz++){
                // multiply every register in in matrix with every register in in matrix transpose
                // then sum together and put into output matrix 
                
                // have to convert ints to floats to multiply. 
                // for whatever reason, there is no integer multiplication in avx
                __m256 row1 = _mm256_castsi256_ps(in_matrix_avx[vert][horiz]);
                __m256 row2 = _mm256_castsi256_ps(in_matrix_avx_transpose[vert_t][horiz]);
                __m256 result = _mm256_mul_ps(row1,row2);
                __m256i result_int = _mm256_castps_si256(result);
                float result_flt[8];
                _mm256_storeu_ps(result_flt, result);
                //cout << result_flt[0] << " " << result_flt[1] << " " << result_flt[2] << " " << result_flt[3] << " ";
                //cout << result_flt[4] << " " << result_flt[5] << " " << result_flt[6] << " " << result_flt[7] << " ";
                for(uint i = 0; i < 8; i++){
                    sum += result_flt[i];
                }
            }
            if(print_output){
                cout << left << setfill(' ') << setw(11);
                cout << sum;
                output << left << setfill(' ') << setw(11);
                output << sum;
            }
        }
        if(print_output){
            cout << endl;
            output << endl;
        }
    };
};

void multiply_cpp(ofstream &output, vector<vector<float>> & in_matrix_cpp, vector<vector<float>> & out_matrix_cpp, bool print_output = false ){
    // only works for square matrices 
    for(uint i = 0; i < in_matrix_cpp.size();i++){
        for(uint j = 0; j < in_matrix_cpp.size(); j++){
            // grab left matrix row; in_matrix[i][1]
            // grab right matrix column; in_matrix[1][j]
            // multiply each value and add; so in_matrix[i][1]*in_matrix[1][j]
            // then in_matrix[i][2]*in_matrix[2][j] etc. 
            float temp = 0; 
            for(uint x = 0; x < in_matrix_cpp.size(); x++){
                temp += in_matrix_cpp[i][x]*in_matrix_cpp[x][j];
            }
            out_matrix_cpp[i][j] = temp;
        }
    }
    if(print_output){
        print_cpp(output,out_matrix_cpp);
    }
}

void multiply_cpp(ofstream &output, vector<vector<unsigned short int>> & in_matrix_cpp, vector<vector<unsigned short int>> & out_matrix_cpp, bool print_output = false ){
    // only works for square matrices 
    for(uint i = 0; i < in_matrix_cpp.size();i++){
        for(uint j = 0; j < in_matrix_cpp.size(); j++){
            // grab left matrix row; in_matrix[i][1]
            // grab right matrix column; in_matrix[1][j]
            // multiply each value and add; so in_matrix[i][1]*in_matrix[1][j]
            // then in_matrix[i][2]*in_matrix[2][j] etc. 
            short int temp = 0; 
            for(uint x = 0; x < in_matrix_cpp.size(); x++){
                temp += in_matrix_cpp[i][x]*in_matrix_cpp[x][j];
            }
            out_matrix_cpp[i][j] = temp;
        }
    }
    if(print_output){
        print_cpp(output,out_matrix_cpp);
    }
}

int main(int argc, char* argv[]){

    srand(1);
    
    // file input scheme from cs1200
    ifstream inputs(argv[1]); // input file 
    ofstream output(argv[2]); // output file 
    
    // check that the files are loaded
    if(!inputs.good()){ // pulled from submitty resources on helpful c++ programming information 
        std::cerr << "Can't open the inputs.txt file: " << argv[1] << " ; please try again" << std::endl;
        exit(true);
    }
    if(!output.good()){ // pulled from submitty resources on helpful c++ programming information 
        std::cerr << "Can't open the output.txt file: " << argv[1] << " ; please try again" << std::endl;
        exit(true);
    }
    
    uint matrix_size;
    inputs >> matrix_size; // read in first value in inputs
    string numtype; 
    inputs >> numtype;
    uint numsize;
    inputs >> numsize;

    //output << "abcd" << endl;
    cout << "Inputs Accepted!" <<endl;
    cout << "matrix size: " << matrix_size << " x " << matrix_size <<endl;
    cout << "number type " << numtype << " " << numsize << " bytes " <<endl;
    output << "Inputs Accepted!" <<endl;
    output << "matrix size: " << matrix_size << " x " << matrix_size <<endl;
    output << "number type " << numtype << " " << numsize << " bytes " <<endl;

    // divide by 8.0 to convert to float 
    uint num_of_regs = ceil(matrix_size/8.0);

    if( (numtype == "float") && (numsize == 4) ){
        
        vector<vector<float>> in_matrix_cpp; 
        vector<vector<float>> out_matrix_cpp;
        vector<vector<float>> in_matrix_cpp_transpose;
        vector<vector<float>> out_matrix_avx;

        // construct float matrix for regular cpp
        // also make empty array for out matrix and in matrix transpose
        for(uint i = 0; i < matrix_size; i++){
            vector<float> temp;
            vector<float> temp2;
            for(uint j = 0; j < matrix_size; j++){
                temp.push_back(get_float());
                temp2.push_back(0);
            }
            in_matrix_cpp.push_back(temp);
            out_matrix_cpp.push_back(temp2);
            out_matrix_avx.push_back(temp2);
            in_matrix_cpp_transpose.push_back(temp2);
        }
        // transpose in matrix for computation in avx multiply 
        transpose(in_matrix_cpp,in_matrix_cpp_transpose);
    
        // construct __m256 matrix
        // need to pad with 0's at the end if not perfectly divisable 
        // looping through the required registers 
        // [(x x x x x x x x) (x x x x x x 0 0)] <- two 8 float registers | two vectors down 
        // [(x x x x x x x x) (x x x x x x 0 0)]                          |
        //                  ^ one vectotr wide 
        
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
    }
    else if( (numtype == "fixed") && (numsize == 2) ){
    
        vector<vector<unsigned short int>> in_matrix_cpp; 
        vector<vector<unsigned short int>> out_matrix_cpp;
        vector<vector<unsigned short int>> in_matrix_cpp_transpose;
        vector<vector<unsigned short int>> out_matrix_avx;

        // construct float matrix for regular cpp
        // also make empty array for out matrix and in matrix transpose
        for(uint i = 0; i < matrix_size; i++){
            vector<unsigned short int> temp;
            vector<unsigned short int> temp2;
            for(uint j = 0; j < matrix_size; j++){
                temp.push_back(get_uint());
                temp2.push_back(0);
            }
            in_matrix_cpp.push_back(temp);
            out_matrix_cpp.push_back(temp2);
            out_matrix_avx.push_back(temp2);
            in_matrix_cpp_transpose.push_back(temp2);
        }
        // transpose in matrix for computation in avx multiply 
        transpose(in_matrix_cpp,in_matrix_cpp_transpose);
    
        // construct __m256 matrix
        // need to pad with 0's at the end if not perfectly divisable 
        // looping through the required registers 
        // [(x x x x x x x x) (x x x x x x 0 0)] <- two 8 float registers | two vectors down 
        // [(x x x x x x x x) (x x x x x x 0 0)]                          |
        //                  ^ one vectotr wide 
        
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
    }
    else{
        cout << "funcitonality not added yet" << endl;
        output << "funcitonality not added yet" << endl;
    }

    output.close();
    return 0;
};


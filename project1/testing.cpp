#include<iostream>
#include <iomanip> // setw
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include<vector>
#include<fstream> // input output files
#include <immintrin.h> // avx stuff
#include <xmmintrin.h>
#include <cmath> // floor
using namespace std;
/* in launch
 to compile: g++ -mavx -Wall -g testing.cpp -o testing.out
 to run: ./testing.out
 "args": ["<",
                     "inputs.txt",
                     ">",
                     "output.txt"],
"args": [ in tasks
                "-mavx"
                "-fdiagnostics-color=always",
                "-g",
                "${file}",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}"
            ],
*/
__m256 _mm256_set1_ps(float a);
__m256 _mm256_set_ps (float e7, float e6, float e5, float e4, float e3, float e2, float e1, float e0);
__m256 _mm256_mul_ps (__m256 a, __m256 b);
__m256 _mm256_mul_ps(__m256 a, __m256 b);

float get_float(){
    //cout<<RAND_MAX<<endl;
    // float will be between 0 and 1. 
    //     this may need to be fixed
    float rndflt = float(rand())/(float(RAND_MAX));
    //cout<<rndflt<<endl;
    return rndflt;
}

void print_float(vector<vector<float>> & in_matrix_cpp){
    for(uint i = 0; i < in_matrix_cpp.size(); i++){
        for(uint j = 0; j < in_matrix_cpp.size(); j++){
            cout << left << setfill(' ') << setw(10);
            cout << in_matrix_cpp[i][j];
        }
        cout << endl;
    }
    cout << endl;
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

void multiply_float_avx(vector<vector<float>> & in_matrix_cpp, vector<vector<float>> & in_matrix_cpp_transpose, vector<vector<float>> & out_matrix, bool print_transpose = false, bool print_output = false){
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
                        cout << left << setfill(' ') << setw(10);
                        cout << in_matrix_cpp[vert][real_i];
                    }
                }
                else{
                    temp_vector.push_back(0);
                    temp_vector_transpose.push_back(0);
                    if(print_transpose){
                        cout << left << setfill(' ') << setw(10);
                        cout << " 0 ";
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
                cout << left << setfill(' ') << setw(10);
                cout << sum;
            }
        }
        if(print_output){
            cout << endl;
        }
    };
};

void multiply_cpp( vector<vector<float>> & in_matrix_cpp, vector<vector<float>> & out_matrix_cpp, bool print_output = false ){
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
        print_float(out_matrix_cpp);
    }
}

int main(int argc, char* argv[]){
    srand(1);
    // file input scheme from cs1200
    ifstream inputs(argv[1]); // input file 
    ifstream output(argv[2]); // output file 
    
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

    cout<<"Inputs Accepted!"<<endl;
    cout<<"matrix size: " << matrix_size << " x " << matrix_size <<endl;
    cout<<"number type " << numtype << " " << numsize << " bytes " <<endl;
    
    // divide by 8.0 to convert to float 
    uint num_of_regs = ceil(matrix_size/8.0);

    vector<vector<float>> in_matrix_cpp; 
    vector<vector<float>> out_matrix_cpp;
    vector<vector<float>> in_matrix_cpp_transpose;
    vector<vector<float>> out_matrix_avx;

    if( (numtype == "float") && (numsize == 4) ){
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
        //print_float(in_matrix_cpp);
        // transpose in matrix for computation in avx multiply 
        transpose(in_matrix_cpp,in_matrix_cpp_transpose);
        //print_float(in_matrix_cpp_transpose);
        // construct __m256 matrix
        // need to pad with 0's at the end if not perfectly divisable 
        
        // looping through the required registers 
        // [(x x x x x x x x) (x x x x x x x x)] <- two 8 float registers | two vectors down 
        // [(x x x x x x x x) (x x x x x x x x)]                          |
        //                  ^ one vectotr wide 
        
        cout << "input matrix: " << endl;
        cout << endl;
        print_float(in_matrix_cpp);

        cout << "transpose matrix: " << endl;
        cout << endl;
        print_float(in_matrix_cpp_transpose);

        cout << "squaring results using cpp: " << endl;
        cout << endl;
        multiply_cpp(in_matrix_cpp,out_matrix_cpp,true);

        cout << "squaring results using avx: " << endl;
        cout << endl;
        multiply_float_avx(in_matrix_cpp,in_matrix_cpp_transpose,out_matrix_avx,false,true);
    }
    else{
        cout << "funcitonality not added yet" << endl;
    }

    // https://www.cs.fsu.edu/~engelen/courses/HPC-adv/intref_cls.pdf

    // https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956&techs=AVX&text=mult
    // https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956,6144&techs=AVX&text=set_ps
    // https://chryswoods.com/vector_c++/immintrin.html
    // _mm256_set1_ps(float a) : This returns a __m256 vector, where all eight elements of the vector are set equal to a, i.e. the vector is [a,a,a,a,a,a,a,a].

    //__m256 a = _mm256_set_ps(7.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0);
    //__m256 b = _mm256_set_ps(7.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
    //__m256 c = _mm256_set1_ps(2.0);
    //__m256 out = _mm256_mul_ps(a,b);
    //cout << a[0] << endl;

    return 0;
};


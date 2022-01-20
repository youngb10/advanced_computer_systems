#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include<vector>
#include<fstream> // input output files
#include <immintrin.h> // avx stuff
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

float get_float(){
    //cout<<RAND_MAX<<endl;
    // float will be between 0 and 1. 
    //     this may need to be fixed
    float rndflt = float(rand())/(float(RAND_MAX));
    //cout<<rndflt<<endl;
}

void print_float(vector<vector<float>> & in_matrix){
    for(int i = 0; i < in_matrix.size(); i++){
        for(int j = 0; j < in_matrix.size(); j++){
            cout << in_matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
};

void cpp_multiply( vector<vector<float>> & in_matrix, vector<vector<float>> & out_matrix ){
    // only works for square matrices  
    for(int i = 0; i < in_matrix.size();i++){
        for(int j = 0; j < in_matrix.size(); j++){

            // grab left matrix row; in_matrix[i][1]
            // grab right matrix column; in_matrix[1][j]
            // multiply each value and add; so in_matrix[i][1]*in_matrix[1][j]
            // then in_matrix[i][2]*in_matrix[2][j] etc. 
            float temp = 0; 
            for(int x = 0; x < in_matrix.size(); x++){
                temp += in_matrix[i][x]*in_matrix[x][j];
            }
            out_matrix[i][j] = temp;
        }
    }
}

void avx_multiply( vector<vector<float>> & in_matrix, vector<vector<float>> & out_matrix ){
    // only works for square matrices  
    for(int i = 0; i < in_matrix.size();i++){
        for(int j = 0; j < in_matrix.size(); j++){

            // grab left matrix row; in_matrix[i][1]
            // grab right matrix column; in_matrix[1][j]
            // multiply each value and add; so in_matrix[i][1]*in_matrix[1][j]
            // then in_matrix[i][2]*in_matrix[2][j] etc. 
            float temp = 0; 
            for(int x = 0; x < in_matrix.size(); x++){
                //temp += _mm256_mul_ps(in_matrix[i][x],in_matrix[x][j]);
            }
            out_matrix[i][j] = temp;
        }
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
    
    int matrix_size;
    inputs >> matrix_size; // read in first value in inputs
    string numtype; 
    inputs >> numtype;
    int numsize;
    inputs >> numsize;

    cout<<"Inputs Accepted!"<<endl;
    cout<<"matrix size: " << matrix_size << " x " << matrix_size <<endl;
    cout<<"number type " << numtype << " " << numsize << " bytes " <<endl;
    
    vector<vector<float>> in_matrix; 
    vector<vector<float>> out_matrix;
    vector<vector<__m256>> in_matrix_avx;
    if( (numtype == "float") && (numsize == 4) ){
        // construct float matrix 
        for(int i = 0; i < matrix_size; i++){
            vector<float> temp;
            vector<float> temp2;
            for(int j = 0; j < matrix_size; j++){
                temp.push_back(get_float());
                temp2.push_back(0);
            }
            in_matrix.push_back(temp);
            out_matrix.push_back(temp2);
        }
        print_float(in_matrix);
        // construct __m256 matrix
        // need to pad with 0's at the end if not perfectly divisable 
        int num_of_regs = floor(matrix_size/8) + 1;
        cout << num_of_regs << endl;
        for(int i = 0; i < num_of_regs; i++){
            for(int j = 0; j < num_of_regs; j++){

                __m256 temp_reg;
                //if(i + 1 < matrix_size)
                //temp_reg = _mm256_set_ps(in_matrix[i+0][j+0],)

            }
        }
    }
    else{
        cout << "funcitonality not added yet" << endl;
    }

    //cpp_multiply(in_matrix,out_matrix);
    //print_float(out_matrix);

    // https://www.cs.fsu.edu/~engelen/courses/HPC-adv/intref_cls.pdf

    // https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956&techs=AVX&text=mult
    // https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=5051,5042,4936,4956,6144&techs=AVX&text=set_ps
    // https://chryswoods.com/vector_c++/immintrin.html
    // _mm256_set1_ps(float a) : This returns a __m256 vector, where all eight elements of the vector are set equal to a, i.e. the vector is [a,a,a,a,a,a,a,a].


    __m256 a = _mm256_set_ps(7.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0);
    __m256 b = _mm256_set_ps(7.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
    __m256 c = _mm256_set1_ps(2.0);
    __m256 out = _mm256_mul_ps(a,b);
    //cout << a[0] << endl;


    return 0;
};


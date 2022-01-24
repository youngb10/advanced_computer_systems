#include<iostream>
#include <iomanip> // setw
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
    return rndflt;
}

void print_float(vector<vector<float>> & in_matrix){
    for(uint i = 0; i < in_matrix.size(); i++){
        for(uint j = 0; j < in_matrix.size(); j++){
            cout << in_matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
};

void print_float_avx(vector<vector<__m256>> & in_matrix_avx){
    cout << left;
    cout << "in print avx fn " << endl;
    cout << endl;
    uint matrix_size = in_matrix_avx.size();
    // divide by 8.0 to convert to float
    uint num_of_regs = ceil(matrix_size/8.0);
    for(uint vert = 0; vert < matrix_size; vert++){
        cout << endl;
        //vector<__m256> temp_row;
        for(uint horiz = 0; horiz < num_of_regs; horiz++){
            // looping through each element in register and checking if it exists
            // cout << "vert " << vert << " horiz " << horiz; 
            // set the temporary register with 0s 
            //__m256 temp_reg = _mm256_set_ps(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
            for(uint reg_i = 0; reg_i < 8; reg_i++){
                cout << setprecision(2) << setw(8) << in_matrix_avx[vert][horiz][reg_i];
            }
            //cout << endl;
            //temp_row.push_back(temp_reg);
        };
        // cout << endl;
        //in_matrix_avx.push_back(temp_row);
    };
};

void cpp_multiply( vector<vector<float>> & in_matrix, vector<vector<float>> & out_matrix ){
    // only works for square matrices  
    for(uint i = 0; i < in_matrix.size();i++){
        for(uint j = 0; j < in_matrix.size(); j++){

            // grab left matrix row; in_matrix[i][1]
            // grab right matrix column; in_matrix[1][j]
            // multiply each value and add; so in_matrix[i][1]*in_matrix[1][j]
            // then in_matrix[i][2]*in_matrix[2][j] etc. 
            float temp = 0; 
            for(uint x = 0; x < in_matrix.size(); x++){
                temp += in_matrix[i][x]*in_matrix[x][j];
            }
            out_matrix[i][j] = temp;
        }
    }
}

void avx_multiply( vector<vector<float>> & in_matrix, vector<vector<float>> & out_matrix ){
    // only works for square matrices  
    for(uint i = 0; i < in_matrix.size();i++){
        for(uint j = 0; j < in_matrix.size(); j++){

            // grab left matrix row; in_matrix[i][1]
            // grab right matrix column; in_matrix[1][j]
            // multiply each value and add; so in_matrix[i][1]*in_matrix[1][j]
            // then in_matrix[i][2]*in_matrix[2][j] etc. 
            float temp = 0; 
            for(uint x = 0; x < in_matrix.size(); x++){
                //temp += _mm256_mul_ps(in_matrix[i][x],in_matrix[x][j]);
            }
            out_matrix[i][j] = temp;
        }
    }
}
// getting weird seg fault on certain values. like 11x11
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
    
    vector<vector<float>> in_matrix; 
    vector<vector<float>> out_matrix;
    vector<vector<__m256>> in_matrix_avx;
    vector<vector<__m256>> out_matrix_avx;
    if( (numtype == "float") && (numsize == 4) ){
        // construct float matrix 
        for(uint i = 0; i < matrix_size; i++){
            vector<float> temp;
            vector<float> temp2;
            for(uint j = 0; j < matrix_size; j++){
                temp.push_back(get_float());
                temp2.push_back(0);
            }
            in_matrix.push_back(temp);
            out_matrix.push_back(temp2);
        }
        print_float(in_matrix);
        // construct __m256 matrix
        // need to pad with 0's at the end if not perfectly divisable 
        
        // divide by 8.0 to convert to float 
        uint num_of_regs = ceil(matrix_size/8.0);
        //cout << num_of_regs << endl;
        // looping through the required registers 
        // [(x x x x x x x x) (x x x x x x x x)] <- two 8 float registers | two vectors down 
        // [(x x x x x x x x) (x x x x x x x x)]                          |
        //                  ^ one vectotr wide 
        //vector<__m256> temp_row;
        //__m256 temp_reg = _mm256_set_ps(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
        //temp_row.push_back(temp_reg)
        for(uint vert = 0; vert < matrix_size; vert++){
            //temp_row.clear();
            vector<__m256> temp_row;
            for(uint horiz = 0; horiz < num_of_regs; horiz++){
                // looping through each element in register and checking if it exists
                cout << "vert " << vert << " horiz " << horiz << endl; 
                // set the temporary register with 0s 
                /*
                __m256 temp_reg = _mm256_set_ps(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
                for(uint reg_i = 0; reg_i < 8; reg_i++){
                    uint real_i = horiz*8 + reg_i;
                    cout << real_i << " ";
                    if(real_i < matrix_size){
                        temp_reg[reg_i] = in_matrix[vert][real_i];
                    }
                    else{
                        temp_reg[reg_i] = 0.0;
                    }
                }*/
                
                vector<float> temp_vector;
                for(uint reg_i = 0; reg_i < 8; reg_i++){
                    uint real_i = horiz*8 + reg_i;
                    cout << real_i << " ";
                    if(real_i < matrix_size){
                        //temp_reg[reg_i] = in_matrix[vert][real_i];
                        temp_vector.push_back(in_matrix[vert][real_i]);
                    }
                    else{
                        temp_vector.push_back(0);
                        //temp_reg[reg_i] = 0.0;
                    }
                }
                __m256 temp_reg = _mm256_set_ps(temp_vector[7],temp_vector[6],temp_vector[5],temp_vector[4],
                                                temp_vector[3],temp_vector[2],temp_vector[1],temp_vector[0]);
                cout << endl;
                temp_row.push_back(temp_reg);
            };
            in_matrix_avx.push_back(temp_row);
        };
        print_float_avx(in_matrix_avx);
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


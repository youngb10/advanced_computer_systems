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


__m256 _mm256_set1_ps(float a);
__m256 _mm256_set_ps (float e7, float e6, float e5, float e4, float e3, float e2, float e1, float e0);
__m256 _mm256_mul_ps (__m256 a, __m256 b);

void testfn(ofstream &output){
    output << "abcd" << endl;
}

int main(int argc, char* argv[]){
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
    testfn(output);
    int vert = 10;
    int horiz = 10;
    //__m256 (**arr2)[vert][horiz];
    //__m256 *arr2[vert];
    //int *arr3[vert];
    //int arr3[vert];
    //int *p;
    //p = arr3;
    //__m256 arr4[vert];
    //__m256 *ptr;
    //__m256 arr5[vert][horiz];
    //__m256 **ptr2;
    //ptr2 = arr5;

    // https://www.geeksforgeeks.org/creating-array-of-pointers-in-cpp/#:~:text=Dynamic%201D%20Array%20in%20C,int%20*p%5B3%5D%3B
    //__m256* ptr = new __m256[vert];
    //__m256 test = _mm256_set_ps(1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
    
    

    //_mm256_mul_ps(__m256 a, __m256 b) 
    __m256 a = _mm256_set_ps(0.0,0.0,0.0,0.0,0.0,0.0,1.0,1.0);
    __m256 b = _mm256_set_ps(0.0,0.0,0.0,0.0,0.0,0.0,1.0,1.0);
    __m256 out = _mm256_mul_ps(a, b);
     
    //float d[8];
    //_mm256_storeu_ps(d, out);
    //cout << "result " << d[0] << " " << d[1] << " " << d[2] << " " << d[3] << " " << d[4] << " " << d[5] << " " << d[6] << " " << d[7] << endl;

    __m256i num = _mm256_set_epi32(0,0,0,2,10,0,0,1);
    __m256i d[1];
    // void _mm256_storeu_si256 (__m256i * mem_addr, __m256i a
    _mm256_storeu_si256(d,num);
    //cout << d[0][0];
    cout << "result " << d[0][0] << " " << d[1][0] << " " << d[2][0] << " " << d[3][0] << " " << d[4][0] << " " << d[5][0] << " " << d[6][0] << " " << d[7][0] << endl;

    return 0;
};
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
    //__m256i d[1];
    // void _mm256_storeu_si256 (__m256i * mem_addr, __m256i a
    //_mm256_storeu_si256(d,num);
    //cout << d[0][0];
    //cout << "result " << d[0][0] << " " << d[1][0] << " " << d[2][0] << " " << d[3][0] << " " << d[4][0] << " " << d[5][0] << " " << d[6][0] << " " << d[7][0] << endl;

    // 32-bit integer
    __m256i int_vec = _mm256_set_epi32(1, 2, 3, 4, 5, 6, 7, 8);

    //int* i = (int*) &int_vec;
    //printf("int:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", i[0], i[1], i[2], i[3], i[4], i[5], i[6], i[7]);
    
    // Signed 32-bit integer multiplication (AVX2)
    __m256i epi32_vec_0 = _mm256_setr_epi32(2, 0, 3, 0, 4, 0, 5, 0);
    __m256i epi32_vec_1 = _mm256_setr_epi32(8, 0, 9, 0, 10, 0, 11, 0);
    __m256i epi32_result = _mm256_mul_epi32(epi32_vec_0, epi32_vec_1);
    //int* i = (int*) &epi32_result;
    //printf("int:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", i[0], i[1], i[2], i[3], i[4], i[5], i[6], i[7]);

    int arr[8];
    arr[0] = 69;
    arr[1] = 25;
    arr[2] = 0;
    arr[3] = 0;
    arr[4] = 0;
    arr[5] = 0;
    arr[6] = 0; 
    arr[7] = 0;
    int arr2[8];
    arr2[0] = 69;
    arr2[1] = -103;
    arr2[2] = 0;
    arr2[3] = 0;
    arr2[4] = 0;
    arr2[5] = 0;
    arr2[6] = 0; 
    arr2[7] = 0;
    //copy(temp_vector.begin(),temp_vector.end(),arr);
    __m256i row = _mm256_setr_epi32(arr[7],arr[6],arr[5],arr[4],
                                        arr[3],arr[2],arr[1],arr[0]);
    __m256i row_t = _mm256_setr_epi32(arr2[7],arr2[6],arr2[5],arr2[4],
                                        arr2[3],arr2[2],arr2[1],arr2[0]);
    __m256i row1 = _mm256_setr_epi32(69, 0, 35, 0, 0, 0, 0, 0);
    __m256i row2 = _mm256_setr_epi32(69, 0, -103, 0, 0, 0, 0, 0);
    //int* store_row = (int*) &row;
    //int* store_row_t = (int*) &row_t;
    __m256i res = _mm256_mul_epi32(row1,row2);
    int* i = (int*) &res;
    //cout << res[0] << " " << res[1] << " " << res[2] << " " << res[3] << " ";
    //cout << res[4] << " " << res[5] << " " << res[6] << " " << res[7] << " ";
    //int* i = (int*) &epi32_result;
    printf("int:\t\t%d, %d, %d, %d, %d, %d, %d, %d\n", i[0], i[1], i[2], i[3], i[4], i[5], i[6], i[7]);
    //cout << result[0] << " " << result[1] << " " << result[2] << " " << result[3] << " ";
    //cout << result[4] << " " << result[5] << " " << result[6] << " " << result[7] << " ";

    return 0;
};
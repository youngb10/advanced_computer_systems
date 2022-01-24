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

int main(int argc, char* argv[]){
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
    __m256 arr5[vert][horiz];
    //__m256 **ptr2;
    //ptr2 = arr5;
    for(int i = 0; i < 10; i++){
        //*(arr4+i) = _mm256_set_ps(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
        //*(p+i) = 1;
        for(int j = 0; j < 10; j++){

            *(*(arr5+i)+j) = _mm256_set_ps(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);

        }
    }
    return 0;
};
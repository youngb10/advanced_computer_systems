
#include <iostream>       // For I/O
#include <fstream>        // For I/O
#include <iomanip>        // For setw
#include <stdio.h>        // std
#include <stdlib.h>       // std
#include <ctime>          // Timekeeping
#include <ratio>          // Timekeeping
#include <chrono>         // Timekeeping
#include <vector>         // For vectors
#include <cmath>          // For floor
#include <stdint.h>
#include <string.h>
#include <sys/resource.h>
#include <cstddef>         // std::size_t
#include <unordered_map>

using namespace std;

using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::duration;
high_resolution_clock::time_point start_time = high_resolution_clock::now();
int time_first = 1;

duration<double, std::milli> delta(std::string msg = ""){
    //duration<double, std::milli> t2 = (end2 - start2) / 1000;
    duration<double, std::milli> del;
    //time_point<system_clock,duration<double>> zero_{};
    int silent = 0;
    if(msg == ""){silent = 1;}
    if(time_first){
        start_time = high_resolution_clock::now();
        time_first = 0;
        del = (high_resolution_clock::now() - high_resolution_clock::now()) / 1000;
    }
    else{
        del = (high_resolution_clock::now() - start_time) / 1000;
        if(!silent){std::cout << "  " << msg << ": " << del.count() << " s" << std::endl;}
        start_time = high_resolution_clock::now();
    }
    return del;
}

int main(int argc, char *argv[])
{
    delta();
    //std::ifstream inputFile(argv[1]); // input file
    std::string inputName = argv[1];
    std::string outName = "modified_" + inputName;
    
    std::string line;
    //std::ofstream outFile(outName);
    // while(!inputFile.eof()){
    //     // if you haven't read the whole part, keep reading
        
    //     getline(inputFile,line);
        
    //     // write to temp output (will copy 4x)
    //     outFile << line << line << line << line << std::endl;
        
    // }
    // inputFile.close();
    // outFile.close();
    
    FILE * pFile;
    FILE * outFile2;
    char buffer [100];

    pFile = fopen (argv[1], "r");
    char char_array[outName.length() +1];
    strcpy(char_array,outName.c_str());
    outFile2 = fopen (char_array, "w");
    if (pFile == NULL) perror ("Error opening file");
    else
    {
        while ( ! feof (pFile) )
        {
        if ( fgets (buffer , 100 , pFile) == NULL ) break;
        fputs (buffer , outFile2);
        }
        fclose (outFile2);
    }
    delta("time to read ");

    return 0;
};


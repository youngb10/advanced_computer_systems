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
using std::chrono::duration;

int main(int argc, char *argv[])
{
    std::ifstream inputFile(argv[1]); // input file
    std::string inputName = argv[1];
    std::string outName = "modified_" + inputName;
    
    std::string line;
    std::ofstream outFile(outName);
    while(!inputFile.eof()){
        // if you haven't read the whole part, keep reading
        
        getline(inputFile,line);
        
        // write to temp output (will copy 4x)
        outFile << line << line << line << line << std::endl;
        
    }
    inputFile.close();
    outFile.close();

    return 0;
};


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

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;



int main(int argc, char *argv[])
{
    // processing input parameters
    
    ifstream params(argv[1]); // input file
    // check that the parameters are loaded
    if (!params.good())
    { // pulled from submitty resources, cs1200, on helpful c++ programming information
      // https://www.cs.rpi.edu/academics/courses/spring20/csci1200/programming_information.php
        std::cerr << "Can't open the parameters.txt file: " << argv[1] << " ; please try again" << std::endl;
        exit(true);
    }
    
    int debug;
    std::string input_file;
    std::string output_file;
    
    std::string line;
    getline(params,line);
    getline(params,line);
    debug = stoi(line);
    if( !((debug == 0) || (debug == 1)) ){
        std::cerr << "Wrong input for debug; must be either 0 (disabled) or 1 (enabled); recieved value: " << debug << std::endl;
        std::cout << "Wrong input for debug; must be either 0 (disabled) or 1 (enabled); recieved value: " << debug << std::endl;
    }
    if(debug == 1) std::cout << line << std::endl;

    getline(params,line);
    getline(params,line);
    if(debug == 1) std::cout << line << std::endl;
    input_file = line;
    ifstream input(input_file); // input file
    // check that the parameters are loaded
    if (!input.good())
    { // pulled from submitty resources on helpful c++ programming information
        std::cerr << "Can't open the inputs.txt file: " << input_file << " ; please try again" << std::endl;
        exit(true);
    }

    std::string output_prefix = "compressed_";
    //https://www.cplusplus.com/reference/string/string/find_last_of/
    std::size_t found = input_file.find_last_of("/\\");
    output_file = input_file.substr(0,found) + output_prefix + input_file.substr(found+1);
    ofstream output(output_file); // output file
    if (!output.good())
    { // pulled from submitty resources on helpful c++ programming information
        std::cerr << "Can't open the output.txt file: " << output_file << " ; please try again" << std::endl;
        exit(true);
    }

    std::cout << "Input parameters accepted" << std::endl;
    
    

    

    output.close();
    return 0;
};


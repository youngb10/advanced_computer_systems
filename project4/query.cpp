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
#include <algorithm>
#include <pthread.h>


//namespace fs = std::filesystem;
using std::chrono::high_resolution_clock;
using std::chrono::time_point;
using std::chrono::duration;

// struct to contain data for pthreads
typedef struct myStruct
{
    std::unordered_map<std::size_t, std::string> dict;
    std::unordered_map<std::string, std::size_t> dict_rev;
    int threadId;
    std::string inPartName;
    std::string outPartName;
    int debug;
} compress_args_t;

// update the declarations
duration<double, std::milli> delta(std::string msg);

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

int query(std::string input_file, std::string str){
    int count = 0;
     // convert string to char array
    char inName[input_file.length() + 1];
    strcpy(inName, input_file.c_str());
    
    FILE *inFile; 
    char *line = NULL;
    size_t len = 0;

    std::string word;
    std::string wordTemp;
    inFile = fopen(inName, "r");
    if (inFile == NULL) perror ("Error opening file");
    while(getline(&line, &len, inFile) != -1){
        word = line;
        if(word != "\n"){
            wordTemp = strtok(line, "\n");
            
            if(word != "__dictbegin__\n"){
                
                if(stoi(word) == stoi(str)){
                    count++;
                }
            }
            else{
                return count;
            }
        }
    }
    
    return count;
}

int main(int argc, char *argv[])
{
    delta();
    std::string encoded_file = argv[1];
    std::string str = argv[2];
    //std::cout << str;
    int debug;
    
    if(debug){std::cout << "Input parameters accepted!" << std::endl; delta("time to construct parameters");}
    std::cout << "Query has started!" << " Searching for: " << str << std::endl;
    std::cout << query(encoded_file,str) << " instances of " << str << " found!" << std::endl;

    return 0;
};

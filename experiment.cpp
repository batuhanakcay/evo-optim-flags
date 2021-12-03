#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h> // for strtol()
#include <errno.h>  // for errno
#include <sstream>  

#include "tree_eda.h"


using namespace std;

//Specify experiments so they can automatically grab the routes and the filename
const string guide = "Usage:\n\
                \t experiment [dir] [exp_num] [small/large] [algorithm] \n\
                [dir]: directory containing the experiment files. Default is \"/experiment\"\n\
                [exp_num]: 0 - basicmath , 1 - bitcount, 2 - qsort, 3 - susan\n\
                [small/large]: Will the algorithm use the small (-s or --small) or large (-l or --large) input?\n\
                [algorithm]: 0 - GA, 1 - UMDA, 2 - TreeEDA\n";
                
                
//Data needed for every experiment
//filename
const vector<string> experiment_filename = {"basicmath_small", 
                                            "basicmath_large",
                                            "bitcnts_small",
                                            "bitcnts_large",
                                            "qsort_small",
                                            "qsort_large",
                                            "susan_small",
                                            "susan_large"};

//files that require compilation
const vector<string> experiment_compile_files = {"basicmath/basicmath_small.c   basicmath/rad2deg.c  basicmath/cubic.c   basicmath/isqrt.c",
                                                 "basicmath/basicmath_large.c   basicmath/rad2deg.c  basicmath/cubic.c   basicmath/isqrt.c",
                                                 "bitcount/bitcnt_1.c bitcount/bitcnt_2.c bitcount/bitcnt_3.c bitcount/bitcnt_4.c bitcount/bitcnts.c bitcount/bitfiles.c bitcount/bitstrng.c bitcount/bstr_i.c",
                                                 "bitcount/bitcnt_1.c bitcount/bitcnt_2.c bitcount/bitcnt_3.c bitcount/bitcnt_4.c bitcount/bitcnts.c bitcount/bitfiles.c bitcount/bitstrng.c bitcount/bstr_i.c",
                                                 "qsort/qsort_small.c",
                                                 "qsort/qsort_large.c",
                                                 "susan/susan.c",
                                                 "susan/susan.c"}; 

//additional options needed by every experiment like inputs, special flags, etc. 
const vector<string> run_options = {"",
                                    "",
                                    "75000",
                                    "1125000",
                                    "input_small.dat",
                                    "input_large.dat",
                                    "input_small.pgm output_small.smoothing.pgm -s",
                                    "susan input_large.pgm output_large.smoothing.pgm -s"};

// ask about additional susan runs 
// susan input_small.pgm output_small.edges.pgm -e
// susan input_small.pgm output_small.corners.pgm -c
// susan input_large.pgm output_large.edges.pgm -e
// susan input_large.pgm output_large.corners.pgm -c

const vector<string> ev_algorithms = {"GA", "UMDA", "TreeEDA"};

string updated_compile_files(string route, int exp_num){
    
    string og_compile_files = experiment_compile_files[exp_num];
    string result = "";

    istringstream iss(og_compile_files);
    string temp; 
    while(iss >> temp){
        result += route;
        result += "/";
        result += temp;
        result += " ";
    }

    return result; 
}

int main(int argc, char **argv){

    cout << "EECS583-FA21 Experiment!" << endl;
    
    if(argc != 5){
        cout << "Incorrect arguments!" << endl;
        cout << guide << endl;
        return 1;
    }

    string experiment_route = argv[1];
    string input_size = argv[3];

    char* p;
    char* q;
    errno = 0; // not 'int errno', because the '#include' already defined it
    long arg1 = strtol(argv[2], &p, 10);
    long arg2 = strtol(argv[4], &q, 10);
    if (*p != '\0' || *q != '\0' || errno != 0) {
        cout << "Argument error! Only select a number for the experiments and algorithms" << endl;
        return 2; 
    }

    if (arg1 < 0 || arg1 > experiment_filename.size() || arg2 < 0 || arg2 > ev_algorithms.size() ) {
        cout << "Argument error! Experiment or Algorithm number doesn't exists" << endl;
        return 3;
    }

    int uses_large_input = -1;

    if(input_size == "-l" || input_size == "--large"){
        uses_large_input = 1;
    }else if(input_size == "-s" || input_size == "--small"){
        uses_large_input = 0;
    }else{
        cout << "Argument error! Size of experiment invalid" << endl;
        return 3;   
    }

    int experiment_ind = 2*arg1 + uses_large_input;
    int algorithm_ind = arg2;

    string cur_filename( experiment_filename[experiment_ind]);
    string cur_compile_files =  updated_compile_files(experiment_route, experiment_ind);
    string cur_run_options( run_options[experiment_ind] );

    string algorithm = ev_algorithms[algorithm_ind];

    if(algorithm=="GA"){
        //
    }else if(algorithm == "UMDA"){
        //              
    }else if(algorithm == "TreeEDA"){
        TreeEDAProcess(cur_filename,cur_compile_files,cur_run_options);
    }else{
        cout << "Something has gone terribly wrong here!" << endl;
        return 4;
    }

    return 0;


}
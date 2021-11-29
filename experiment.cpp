#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h> // for strtol()
#include <errno.h>  // for errno


using namespace std;

//Specify experiments so they can automatically grab the routes and the filename
const string guide = "Usage:\n\
                \t experiment [dir] [exp_num] [algorithm] \n\
                [dir]: directory containing the experiment files. Default is \"/experiment\"\n\
                [exp_num]: 0 - basicmath_small , 1 - basicmath_large, 2 - bitcount, 3 - qsort_small, 4 - qsort_large, 5 - susan\n\
                [algorithm]: 0 - GA, 1 - UMDA, 2 - TreeEDA\n";
                
                
//Data needed for every experiment
const vector<string> experiment_filename = {"basicmath_small", 
                                            "basicmath_large",
                                            "bitcnts",
                                            "qsort_small",
                                            "qsort_large",
                                            "susan"};

const vector<string> experiment_compile_files = {"basicmath/basicmath_small.c   basicmath/rad2deg.c  basicmath/cubic.c   basicmath/isqrt.c",
                                                 "basicmath/basicmath_large.c   basicmath/rad2deg.c  basicmath/cubic.c   basicmath/isqrt.c",
                                                 "bitcount/bitcnt_1.c bitcount/bitcnt_2.c bitcount/bitcnt_3.c bitcount/bitcnt_4.c bitcount/bitcnts.c bitcount/bitfiles.c bitcount/bitstrng.c bitcount/bstr_i.c",
                                                 "qsort/qsort_small.c",
                                                 "qsort/qsort_large.c",
                                                 "susan/susan.c"}; 

const vector<string> ev_algorithms = {"GA", "UMDA", "TreeEDA"};

int main(int argc, char **argv){

    cout << "EECS583-FA21 Experiment!" << endl;
    
    if(argc != 4){
        cout << "Incorrect arguments!" << endl;
        cout << guide << endl;
        return 1;
    }

    string experiment_route = argv[1];

    char* p;
    char* q;
    errno = 0; // not 'int errno', because the '#include' already defined it
    long arg1 = strtol(argv[2], &p, 10);
    long arg2 = strtol(argv[3], &q, 10);
    if (*p != '\0' || *q != '\0' || errno != 0) {
        cout << "Argument error! Only select a number for the experiments and algorithms" << endl;
        return 2; 
    }

    if (arg1 < 0 || arg1 > experiment_filename.size() || arg2 < 0 || arg2 > ev_algorithms.size() ) {
        cout << "Argument error! Experiment or Algorithm number doesn't exists" << endl;
        return 3;
    }
    int experiment_ind = arg1;
    int algorithm_ind = arg2;

    // IMPORT ALGORITHMS NOW 

    return 0;


}
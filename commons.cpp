#include "commons.h"

random_device r;     // only used once to initialise (seed) engine
default_random_engine generator(r());    // random-number engine

vector<string> gcc_flags{"-fauto-inc-dec", "-fbranch-count-reg", "-fcombine-stack-adjustments", "-fcompare-elim", 
						 "-fcprop-registers", "-fdce", "-fdefer-pop", "-fdelayed-branch", "-fdse", "-fforward-propagate",
						 "-fguess-branch-probability", "-fif-conversion", "-fif-conversion2", "-finline-functions-called-once",
						 "-fipa-profile", "-fipa-pure-const", "-fipa-reference", "-fipa-reference-addressable", "-fmerge-constants",
						 "-fmove-loop-invariants", "-fomit-frame-pointer", "-freorder-blocks", "-fshrink-wrap", "-fshrink-wrap-separate",
						 "-fsplit-wide-types", "-fssa-backprop", "-fssa-phiopt", "-ftree-bit-ccp", "-ftree-ccp", "-ftree-ch", "-ftree-coalesce-vars",
						 "-ftree-copy-prop", "-ftree-dce", "-ftree-dominator-opts", "-ftree-dse", "-ftree-forwprop", "-ftree-fre",
						 "-ftree-phiprop", "-ftree-pta", "-ftree-scev-cprop", "-ftree-sink", "-ftree-slsr", "-ftree-sra",
						 "-ftree-ter", "-funit-at-a-time"};
                        /*{"-falign-functions", "-falign-jumps", "-falign-labels", "-falign-loops",
						 "-fcaller-saves", "-fcode-hoisting", "-fcrossjumping", "-fcse-follow-jumps", "-fcse-skip-blocks", 
						 "-fdelete-null-pointer-checks", "-fdevirtualize", "-fdevirtualize-speculatively", "-fexpensive-optimizations",
						 "-fgcse", "-fgcse-lm", "-fhoist-adjacent-loads", "-finline-small-functions", "-findirect-inlining", "-fipa-bit-cp",
						 "-fipa-cp", "-fipa-icf", "-fipa-ra", "-fipa-sra", "-fipa-vrp", "-fisolate-erroneous-paths-dereference", 
						 "-flra-remat", "-foptimize-sibling-calls", "-foptimize-strlen", "-fpartial-inlining", "-fpeephole2",
						 "-freorder-blocks-algorithm=stc", "-freorder-blocks-and-partition", "-freorder-functions", "-frerun-cse-after-loop",
						 "-fschedule-insns", "-fschedule-insns2", "-fsched-interblock", "-fsched-spec", "-fstore-merging", "-fstrict-aliasing",
						 "-fthread-jumps", "-ftree-builtin-call-dce", "-ftree-pre", "-ftree-switch-conversion", "-ftree-tail-merge",
						 "-ftree-vrp", "-fgcse-after-reload", "-finline-functions", "-fipa-cp-clone", "-floop-interchange", "-floop-unroll-and-jam",
						 "-fpeel-loops", "-fpredictive-commoning", "-fsplit-paths", "-ftree-loop-distribute-patterns", "-ftree-loop-distribution",
						 "-ftree-loop-vectorize", "-ftree-partial-pre", "-ftree-slp-vectorize", "-funswitch-loops", "-fvect-cost-model",
						 "-fversion-loops-for-strides"};*/




void print_flags(const vector<int> &flags){
    for (size_t i = 0; i < flags.size() - 1; i++){
        cout << flags[i] << ",";
    }
    cout << flags[flags.size() - 1] << endl;
}

void print_min(const vector<float> &min_runtime, const vector<vector<int> > &min_flags){
    float time = min_runtime[0];
    size_t gen = 0;
    vector<int> flags = min_flags[0];
    for (size_t i = 0; i < min_runtime.size(); i++){
        if (time > min_runtime[i]){
            time = min_runtime[i];
            flags = min_flags[i];
            gen = i;
        }
    }
    cout << "best" << ";time:" << time << ";gen:" << gen << ";flags:";
    print_flags(flags);
}

float objective(const vector<int> &flags, string compile_files, string filename, string run_ops){
	//compile_files: files to compile by gcc
	//sample compile_files: "basicmath/basicmath_large.c basicmath/rad2deg.c basicmath/cubic.c basicmath/isqrt.c "
	//filename: compiled filename
	//sample filename: "basicmath_large"
	string compile_flags;
	for(size_t i = 0; i < flags.size(); i++){
		if(flags[i] == 1){
			compile_flags += gcc_flags[i];
			compile_flags += " ";
		}
	}
    string command1 = "gcc " + compile_flags + compile_files + "-o " + filename + " -lm";
	string command2 = "bash -c '(TIMEFORMAT='%3R'; time ./" + filename + " " + run_ops + " > output.txt) &> time.txt'";

    system(command1.c_str());
    system(command2.c_str());

	ifstream myfile("time.txt");
	string line;
	float seconds;
	if (myfile.is_open())
	{
		getline(myfile, line);
		seconds = stof(line);
		myfile.close();
	}
	else{
		cout << "Unable to open file";
	}
	return seconds;
}


vector<int> selection(const vector<pair<float,int> > &sorted_runtimes, float r_sel){
    //select the fastest r_sel (a percentage) of population given the sorted runtimes vector
    //input: runtimes of size (n_pop,1), each element is a pair of <score, index>; selection rate
    //output: index of selected individuals
    int n_selected = int(r_sel * sorted_runtimes.size());
    vector<int> pop_index(n_selected);
    for (int i = 0; i < n_selected; i++){
        pop_index[i] = sorted_runtimes[i].second;
    }
    return pop_index;
}


void mutation(vector<vector<int> > & children, float r_mut){
    for (size_t i = 0 ; i < children.size(); i ++){
        for(size_t j = 0 ; j < children[i].size(); j++){
            if (((float) rand()) / (float) RAND_MAX < r_mut){
                children[i][j] = (children[i][j] + 1) % 2;
            }
        }
    }
}

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <tuple>
#include <algorithm>
#include <string>
#include <fstream>


using namespace std;

default_random_engine e1;    // random-number engine

vector<string> gcc_flags{"-fauto-inc-dec", "-fbranch-count-reg", "-fcombine-stack-adjustments", "-fcompare-elim", 
						 "-fcprop-registers", "-fdce", "-fdefer-pop", "-fdelayed-branch", "-fdse", "-fforward-propagate",
						 "-fguess-branch-probability", "-fif-conversion", "-fif-conversion2", "-finline-functions-called-once",
						 "-fipa-profile", "-fipa-pure-const", "-fipa-reference", "-fipa-reference-addressable", "-fmerge-constants",
						 "-fmove-loop-invariants", "-fomit-frame-pointer", "-freorder-blocks", "-fshrink-wrap", "-fshrink-wrap-separate",
						 "-fsplit-wide-types", "-fssa-backprop", "-fssa-phiopt", "-ftree-bit-ccp", "-ftree-ccp", "-ftree-ch", "-ftree-coalesce-vars",
						 "-ftree-copy-prop", "-ftree-dce", "-ftree-dominator-opts", "-ftree-dse", "-ftree-forwprop", "-ftree-fre",
						 "-ftree-phiprop", "-ftree-pta", "-ftree-scev-cprop", "-ftree-sink", "-ftree-slsr", "-ftree-sra",
						 "-ftree-ter", "-funit-at-a-time", "-falign-functions", "-falign-jumps", "-falign-labels", "-falign-loops",
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
						 "-fversion-loops-for-strides"};

struct result {
    vector<vector<int> > min_flags;
    vector<float> min_runtimes;
};

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


float objective(const vector<int> &flags, string compile_files, string filename){
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
	string command2 = "bash -c '(TIMEFORMAT='%3R'; time ./" + filename + " > output.txt) &> time.txt'";

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

vector<int> selection(const vector<tuple<float,int> > &sorted_runtimes, float r_sel){
    int n_parents = int(r_sel * sorted_runtimes.size());
    vector<int> parents_index(n_parents);
    for (int i = 0; i < n_parents; i++){
        parents_index[i] = get<1>(sorted_runtimes[i]);
    }
    return parents_index;
}

vector<vector<int> > crossover(const vector<int> &parent1, const vector<int> &parent2){
    uniform_int_distribution<int> uniform_dist(0, parent1.size());
    int crossover_point = uniform_dist(e1);

    vector<int> child1(parent1.size());
    vector<int> child2(parent1.size());

    for (int i = 0; i < parent1.size(); i++){
        if (i < crossover_point){
            child1[i] = parent1[i];
            child2[i] = parent2[i];
        }
        else{
            child1[i] = parent2[i];
            child2[i] = parent1[i];
        }
    }

    vector<vector<int> > children(2);
    children[0] = child1;
    children[1] = child2;
    return children;
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

result genetic_algorithm(float (*fun)(const vector<int> &, string, string), string compile_files, string filename, int n_flags, int n_gen, int n_pop, float r_mut, float r_sel){
    uniform_int_distribution<int> uniform_dist(0, 1);
    vector<vector<int> > population(n_pop, vector<int> (n_flags));
    for (int i = 0; i < n_pop; i++){
        for (int j = 0; j < n_flags ; j++){
            population[i][j] = uniform_dist(e1);
        }
    }

    //container for best performance in each generation
    vector<float> min_runtime(n_gen);
    vector<vector<int> > min_flags(n_gen, vector<int> (n_flags));

    for (int i = 0; i < n_gen; i++){
        vector<tuple<float,int> > runtimes(n_pop);
        for (int j = 0; j < n_pop; j++){
            float score = (*fun)(population[j], compile_files, filename);
            runtimes[j] = make_tuple(score, j);
        }
        sort(runtimes.begin(), runtimes.end());

        min_runtime[i] = get<0>(runtimes[0]);
        min_flags[i] = population[get<1>(runtimes[0])];

        cout << "gen:" << i << ";" << "time:" << min_runtime[i] << endl;

        vector<int> parents_index = selection(runtimes, r_sel);
        vector<vector<int> > new_generation;
        for (size_t j = 0 ; j < parents_index.size(); j = j + 2){
            vector<vector<int> > crossover_children = crossover(population[parents_index[j]], population[parents_index[j+1]]);
            mutation(crossover_children, r_mut);
            new_generation.push_back(crossover_children[0]);
            new_generation.push_back(crossover_children[1]);
        }
        for (size_t j = 0 ; j < (parents_index.size() / 2) - 1; j++){
            vector<vector<int> > crossover_children = crossover(population[parents_index[j]], population[parents_index[parents_index.size() - 1 - j]]);
            mutation(crossover_children, r_mut);
            new_generation.push_back(crossover_children[0]);
            new_generation.push_back(crossover_children[1]);
        }
        vector<vector<int> > crossover_children = crossover(population[parents_index[0]], population[parents_index[parents_index.size() / 2]]);
        mutation(crossover_children, r_mut);
        new_generation.push_back(crossover_children[0]);
        new_generation.push_back(crossover_children[1]);
        population = new_generation;
    }

    print_min(min_runtime, min_flags);

    result test;
    test.min_flags = min_flags;
    test.min_runtimes = min_runtime;

    return test;
}

int main(){
    // define the algorithm used
    string alg = "GA";
    // define the number of generations
    int n_gen = 50;
    // define the number of flags
    int n_flags = 107;
    // define the population size
    int n_pop = 100;
    // mutation rate
    float r_mut = 1.0 / float(n_flags);
    // trunctation rate
    float r_sel = 0.5;
    // perform the UMDA
    int execution_time = 30;

    vector<result> results_each_generation(execution_time);
	string filename("basicmath_small");
	string compile_files("experiment/basicmath/basicmath_small.c experiment/basicmath/rad2deg.c experiment/basicmath/cubic.c experiment/basicmath/isqrt.c ");

    cout << "parameters" << ";"; 
    cout << "alg:" << alg << ";";
    cout << "n_exe:" << execution_time << ";";
    cout << "n_gen:" << n_gen << ";";
    cout << "n_flags:" << n_flags << ";";
    cout << "n_pop:" << n_pop << ";";
    cout << "r_mut:" << r_mut << ";";
    cout << "r_sel:" << r_sel << ";";
    cout << "filename:" << filename << endl;
    for(size_t i = 0; i < execution_time; i++){
		cout << "exe:" << i << endl;
        result first = genetic_algorithm(objective, compile_files, filename, n_flags, n_gen, n_pop, r_mut, r_sel);

        results_each_generation[i] = first;
    }

    //calculate efficiency gain
    vector<float> avg_min_runtime(n_gen,0);
    for(size_t i = 0; i < n_gen; i++){
        float sum = 0;
        for(size_t j = 0; j < execution_time; j++){
            sum += results_each_generation[j].min_runtimes[i];
        }
        avg_min_runtime[i] = sum/execution_time;
    }
    vector<float> efficiency_gain(n_gen,0);
    for(size_t i = 1; i < n_gen; i++){
        efficiency_gain[i] = (avg_min_runtime[0]-avg_min_runtime[i])/avg_min_runtime[0];
    }
    
    cout << "Efficiency Gain" << endl;
    for (size_t i = 0; i < n_gen; i++){
        cout << efficiency_gain[i] << ", ";
    }
	cout << endl;

    return 0;
}

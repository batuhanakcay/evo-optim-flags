#include "UMDA.h" 

vector<vector<int> > sampling(const vector<float> &probability, int n_pop){
    //generate population
    //input: probability vector for each flag; # individuals in the population
    //return: vector of size (n_pop, #flags)
    size_t num_flags = probability.size();
    vector<vector<int> > pop_flags(n_pop, vector<int> (num_flags,0));
    for(size_t i = 0; i < num_flags; i++){
        bernoulli_distribution distribution(probability[i]);
        for(size_t j = 0; j < n_pop; j++){
            pop_flags[j][i] = distribution(generator);
        }
    }
    return pop_flags;
}

vector<float> cal_prob(const vector<vector<int> > &population){
    //calculate the marginal probability of each flag
    //input: selected population
    //output: probability vector
    size_t n_pop = population.size();
    size_t n_flag = population[0].size();
    //upper and lower to make sure that the probability is not stuck at 0 or 1
    float upper = 1.0 - 1.0/n_flag;
    float lower = 1.0/n_flag;

    vector<float> prob(n_flag,0.0);
    for(size_t i = 0; i < n_flag; i++){
        int sum = 0;
        float p = 0;
        for(size_t j = 0; j < n_pop; j++){
            sum += population[j][i];
        }
        p = sum*1.0/n_pop;
        if(p > upper){
            prob[i] = upper;
        }
        else if(p < lower){
            prob[i] = lower;
        }
        else{
            prob[i] = p;
        }

    }
    return prob;
}

result UMDA(float (*fun)(const vector<int> &, string, string, string), string compile_files, string filename, string run_ops, int n_flags, int n_gen, int n_pop, float r_mut, float r_sel){
    //initialize population; initial probability is 0.5 for each flag
    vector<float> prob(n_flags, 0.5);
    vector<vector<int> > population = sampling(prob, n_pop);
    vector<vector<int> > selected_population(int(n_pop*r_sel), vector<int> (n_flags,0));

    //container for best performance in each generation
    vector<float> min_runtime(n_gen, 0.0);
    vector<vector<int> > min_flags(n_gen, vector<int> (n_flags,0));

    for(size_t i = 0; i < n_gen; i++){
        min_runtime[i] = (*fun)(population[0], compile_files, filename, run_ops);
        min_flags[i] = population[0];

        vector<pair<float,int> > runtimes(n_pop, make_pair(0.0,0));
        //calculate runtime for each individual in the population
        for (int j = 0; j < n_pop; j++){
            float score = (*fun)(population[j], compile_files, filename, run_ops);
            runtimes[j] = make_pair(score, j);
        }

        //select population based on r_sel and calculate marginal probability
        sort(runtimes.begin(), runtimes.end());
        vector<int> selected_idx = selection(runtimes, r_sel);
        for(size_t j = 0; j < selected_idx.size(); j++){
            selected_population[j] = population[selected_idx[j]];
        }

        float cur_min_runtime = runtimes[0].first;
        vector<int> cur_min_flag = population[runtimes[0].second];
        if(cur_min_runtime < min_runtime[i]){
            min_runtime[i] = cur_min_runtime;
            min_flags[i] = cur_min_flag;
        }
        cout << "gen:" << i << ";" << "time:" << min_runtime[i] << endl;

        prob = cal_prob(selected_population);
        population = sampling(prob, n_pop);
        mutation(population, r_mut);
    }

    print_min(min_runtime, min_flags);
    
    result test;
    test.min_flags = min_flags;
    test.min_runtimes = min_runtime;

    return test;

}


void UMDAProcess(string fn, string cf, string r_op){
    // define the algorithm used
    string alg = "UMDA";
    // define the number of generations
    int n_gen = 50;
    // define the number of flags
    int n_flags = gcc_flags.size() ;//107;
    // define the population size
    int n_pop = 100;
    // mutation rate
    float r_mut = 1.0 / float(n_flags);
    // trunctation rate
    float r_sel = 0.5;
    // perform the UMDA
    int execution_time = 30;

    vector<result> results_each_generation;
	string filename = fn;
	string compile_files = cf;
    string run_ops = r_op;

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
        result first = UMDA(objective, compile_files, filename, run_ops, n_flags, n_gen, n_pop, r_mut, r_sel);

        results_each_generation.push_back(first);
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

}

/*
int main(){
    
    UMDAProcess("basicmath_small", "experiment/basicmath/basicmath_small.c experiment/basicmath/rad2deg.c experiment/basicmath/cubic.c experiment/basicmath/isqrt.c ", "");
    return 0;
}
*/
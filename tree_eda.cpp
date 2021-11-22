#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
using namespace std;

default_random_engine generator;

struct result {
    vector<vector<int> > min_flags;
    vector<float> min_runtimes;
};

void print_flags(vector<int> flags){
    cout << "[";
    for (size_t i = 0; i < flags.size() - 1; i++){
        cout << flags[i] << ", ";
    }
    cout << flags[flags.size() - 1] << "]" << endl;
}

float objective(vector<int> flags){
    int sum = 0;
    for (size_t i = 0; i< flags.size(); i++){
        sum += flags[i];
    }
    return float(sum);
};

vector<vector<int> > random_sampling(vector<float> probability, int n_pop){
    //generate a population from a random sampling
    //input: initial probability vector for each flag; # of individuals in the population
    //output: vector of size n_pop, #flags

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

vector<int> selection(vector<pair<float,int> > sorted_runtimes, float r_sel){
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

vector<float> cal_univariate_prob(vector<vector<int> > population){
    //calculate the univariate marginal probability of each flag
    //input: selected population
    //output: probability vector with probabilities for each flag

    size_t n_pop = population.size();
    size_t n_flag = population[0].size();

    //upper and lower to make sure that the probability is not stuck at 0 or 1 *****
    float upper = 1.0 - 1.0/n_flag;
    float lower = 1.0/n_flag;

    vector<float> probs(n_flag,0.0);

    for(size_t i=0; i<n_flag; i++){
        int sum = 0;
        float pr = 0;

        for(size_t j = 0; j < n_pop; j++){
            sum += population[j][i];
        }

        pr = sum * 1.0 / n_pop; 

        if(pr > upper){
            probs[i] = upper;
        }
        else if(pr < lower){
            probs[i] = lower;
        }
        else{
            probs[i] = pr;
        }


    }

    return probs;

}

vector< vector <float> > cal_bivariate_prob(vector<vector<int> > population){
    //calculate the bivariate marginal probability of each pair of flag
    //input: selected population
    //output: probability matrix with probabilities for each pair of flag (n_flag,n_flag)

    size_t n_pop = population.size();
    size_t n_flag = population[0].size();

    vector< vector <float> > probs(n_flag, vector<float>(n_flag,0.0));

    for(size_t i=0; i<n_flag; i++){
        for(size_t j=0; j<n_flag; j++){
            float pr = 0.0;
            if(i != j){

                int sum_flag_b = 0;

                for(size_t k = 0; k < n_pop; k++){
                    sum_flag_b += int(population[k][i] && population[k][j]);
                }

                pr = sum_flag_b * 1.0 / n_pop;

            }

            probs[i][j] = pr;

        }
    }

    return probs;

}

vector< vector<float> > cal_mutual_information( vector<float> univ, vector< vector<float> > biv){
    //Calculate the matrix of mutual information for the flags
    //Input: Univariate and Bivariate probabilities
    //Output: Matrix of size (n_flag,n_flag)

    size_t n_flag = univ.size();
    
    vector< vector <float> > mutual_info(n_flag, vector<float>(n_flag,0.0));

    for(size_t i=0; i<n_flag; i++){
        for(size_t j=0; j<n_flag; j++){
            mutual_info[i][j] = log( biv[i][j] / (univ[i] * univ[j]) );
        }
    }

    return mutual_info;

}

result TreeEDA(float (*fun)(vector<int>), int n_flags, int n_gen, int n_pop, float r_mut, float r_sel){
   // generate initial population of n_pop individuals 
   vector<float> prob(n_flags, 0.5);
   vector<vector<int> > population = random_sampling(prob, n_pop);
   vector<vector<int> > selected_population(int(n_pop*r_sel), vector<int> (n_flags,0));

   // best performance in each generation
   vector<float> min_runtime(n_gen, 0.0);
   vector<vector<int> > min_flags(n_gen, vector<int> (n_flags,0));

   // loop for n_gen iterations
   for(size_t i=0; i < n_gen; i++){
       //Calculation of performances of current population
       min_runtime[i] = (*fun)(population[0]);
       min_flags[i] = population[0];

       vector<pair<float,int> > runtimes(n_pop, make_pair(0.0,0));

       for (int j = 0; j < n_pop; j++){
            float score = (*fun)(population[j]);
            runtimes[j] = make_pair(score, j);
       }

       //Selection of individuals according to r_sel (percentage)
       sort(runtimes.begin(), runtimes.end());
       vector<int> selected_idx = selection(runtimes, r_sel);
       for(size_t j = 0; j < selected_idx.size(); j++){
           selected_population[j] = population[selected_idx[j]];
       }     

       //Compute univariate and bivariate marginal frequencies
        vector<float> univ_probs = cal_univariate_prob(selected_population);
        vector< vector<float> > biv_probs = cal_bivariate_prob(selected_population);

       //Calculate Matrix of Mutual Information
        vector< vector<float> > mutual_info = cal_mutual_information(univ_probs, biv_probs);

       //Calculate maximum weight spanning tree
        


       //Compute parameters of the model

       //Sample n_pop individuals from the tree

    }

    //result 
    result test;
    test.min_flags = min_flags;
    test.min_runtimes = min_runtime;

    return test;

}

int main(){
    // define the number of generations
    int n_gen = 50;
    // define the number of flags
    int n_flags = 58;
    // define the population size
    int n_pop = 100;
    // mutation rate
    float r_mut = 1.0 / float(n_flags);
    // trunctation rate
    float r_sel = 0.5;
    // perform the TreeEDA
    int execution_time = 30;
    vector<result> results_each_generation;

    for(size_t i = 0; i < execution_time; i++){
        result first = TreeEDA(objective, n_flags, n_gen, n_pop, r_mut, r_sel);
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

    return 0;
}

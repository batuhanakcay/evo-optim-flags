#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <tuple>
#include <algorithm>


using namespace std;

random_device r;     // only used once to initialise (seed) engine
default_random_engine e1(r());    // random-number engine

struct result {
    vector<vector<int>> min_flags;
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

vector<int> selection(vector<tuple<float,int>> sorted_runtimes, float r_sel){
    int n_parents = int(r_sel * sorted_runtimes.size());
    vector<int> parents_index(n_parents);
    for (int i = 0; i < n_parents; i++){
        parents_index[i] = get<1>(sorted_runtimes[i]);
    }
    return parents_index;
}

vector<vector<int>> crossover(vector<int> parent1, vector<int> parent2){
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

    vector<vector<int>> children(2);
    children[0] = child1;
    children[1] = child2;
    return children;
}

void mutation(vector<vector<int>> & children, float r_mut){
    for (size_t i = 0 ; i < children.size(); i ++){
        for(size_t j = 0 ; j < children[i].size(); j++){
            if (((float) rand()) / (float) RAND_MAX < r_mut){
                children[i][j] = (children[i][j] + 1) % 2;
            }
        }
    }
}

result genetic_algorithm(float (*fun)(vector<int>), int n_flags, int n_gen, int n_pop, float r_mut, float r_sel){
    result test;
    vector<vector<int>> min_flags(n_gen + 1);
    vector<float> min_runtimes(n_gen + 1);

    uniform_int_distribution<int> uniform_dist(0, 1);
    vector<vector<int>> population(n_pop, vector<int> (n_flags));
    for (int i = 0; i < n_pop; i++){
        for (int j = 0; j < n_flags ; j++){
            population[i][j] = uniform_dist(e1);
        }
    }
    float min_runtime = (*fun)(population[0]);
    int min_index = 0;
    for (int i = 0; i < n_gen; i++){
        vector<tuple<float,int>> runtimes(n_pop);
        for (int j = 0; j < n_pop; j++){
            float score = (*fun)(population[j]);
            runtimes[j] = make_tuple(score, j);
        }
        sort(runtimes.begin(), runtimes.end());
        float cur_gen_min_runtime = get<0>(runtimes[0]);
        int cur_gen_min_index = get<1>(runtimes[0]);
        if (cur_gen_min_runtime < min_runtime){
            min_runtime = cur_gen_min_runtime;
            min_index = cur_gen_min_index;
            /*
            cout << "Generation " << i << endl;
            cout << "New min runtime: " <<  min_runtime << endl;
            cout <<  "New min flags: [";
            for (int k = 0; k < n_flags - 1; k++){
                cout <<  population[min_index][k] << ", ";
            }
            cout <<  population[min_index][n_flags - 1] << "]" << endl;
            cout << endl;
            */
        }
        vector<int> parents_index = selection(runtimes, r_sel);
        vector<vector<int>> new_generation;
        for (size_t j = 0 ; j < parents_index.size(); j = j + 2){
            vector<vector<int>> crossover_children = crossover(population[parents_index[j]], population[parents_index[j+1]]);
            mutation(crossover_children, r_mut);
            new_generation.push_back(crossover_children[0]);
            new_generation.push_back(crossover_children[1]);
        }
        for (size_t j = 0 ; j < (parents_index.size() / 2) - 1; j++){
            vector<vector<int>> crossover_children = crossover(population[parents_index[j]], population[parents_index[parents_index.size() - 1 - j]]);
            mutation(crossover_children, r_mut);
            new_generation.push_back(crossover_children[0]);
            new_generation.push_back(crossover_children[1]);
        }
        vector<vector<int>> crossover_children = crossover(population[parents_index[0]], population[parents_index[parents_index.size() / 2]]);
        mutation(crossover_children, r_mut);
        new_generation.push_back(crossover_children[0]);
        new_generation.push_back(crossover_children[1]);
    min_flags[i] = population[min_index];
    min_runtimes[i] = min_runtime;
    population = new_generation;
    }
    vector<tuple<float,int>> runtimes(n_pop);
    for (int j = 0; j < n_pop; j++){
        float score = (*fun)(population[j]);
        runtimes[j] = make_tuple(score, j);
    }
    sort(runtimes.begin(), runtimes.end());
    min_runtime = get<0>(runtimes[0]);
    min_index = get<1>(runtimes[0]);
    min_flags[n_gen] = population[min_index];
    min_runtimes[n_gen] = min_runtime;
    test.min_flags = min_flags;
    test.min_runtimes = min_runtimes;
    return test;
}

int main(){
    // define the number of generations
    int n_gen = 50;
    // define the number of flags
    int n_flags = 58;
    // define the population size
    int n_pop = 100;
    // One point crossover
    // mutation rate
    float r_mut = 1.0 / float(n_flags);
    // trunctation rate
    float r_sel = 0.5;
    // perform the GA
    int execution_time = 30;
    vector<result> results_each_generation(execution_time);

    for(size_t i = 0; i < execution_time; i++){
        cout << "Execution " << i << endl;
        // cout << endl;
        // perform the genetic algorithm search
        result test = genetic_algorithm(objective, n_flags, n_gen, n_pop, r_mut, r_sel);
        results_each_generation[i] = test;
        /*
        for (size_t j = 0; j < n_gen; j++){
            cout << "Generation: " << j << endl;
            print_flags(results_each_generation[i].min_flags[j]);
            cout << "Runtime: " << results_each_generation[i].min_runtimes[j] << endl;
        }
        cout << endl;
        */
    }

    //calculate average min runtime for each generation
    vector<float> avg_min_runtime(n_gen + 1,0);
    for(size_t i = 0; i < n_gen + 1; i++){
        float sum = 0;
        for(size_t j = 0; j < execution_time; j++){
            sum += results_each_generation[j].min_runtimes[i];
        }
        avg_min_runtime[i] = sum/execution_time;
    }

    cout << endl;
    cout << "After running algorithm " << execution_time << " times the average runtime for generations are:" << endl;
    cout << endl;
    for (size_t i = 0; i < n_gen + 1; i++){
        cout << "Generation: " << i << endl;
        cout << "Average Runtime: " << avg_min_runtime[i] << endl;
    }

    return 0;
}

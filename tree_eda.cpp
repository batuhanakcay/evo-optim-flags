#include "tree_eda.h"

vector< vector<int> > random_sampling(const vector<float> &probability, int n_pop){
    //generate a population from a random sampling
    //input: initial probability vector for each flag; # of individuals in the population
    //output: vector of size n_pop, #flags

    size_t num_flags = probability.size();
    
    vector< vector<int> > pop_flags(n_pop, vector<int> (num_flags,0));


    for(size_t i = 0; i < num_flags; i++){
        bernoulli_distribution distribution(probability[i]);
        for(size_t j = 0; j < n_pop; j++){
            pop_flags[j][i] = distribution(generator);
        }
    }
    return pop_flags;

}

vector<float> cal_univariate_prob(const vector<vector<int> > &population){
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

vector< vector <float> > cal_bivariate_prob(const vector<vector<int> > &population){
    //calculate the bivariate marginal probability of each pair of flag
    //input: selected population
    //output: probability matrix with probabilities for each pair of flag (n_flag,n_flag)

    size_t n_pop = population.size();
    size_t n_flag = population[0].size();

    vector< vector <float> > probs(n_flag, vector<float>(n_flag,0.0));

    //upper and lower to make sure that the probability is not stuck at 0 or 1 *****
    float upper = 1.0 - 1.0/n_flag;
    float lower = 1.0/n_flag;

    for(size_t i=0; i<n_flag; i++){
        for(size_t j=0; j<n_flag; j++){
            //if i==j, pr should be zero for constructing the MWS tree
            float pr = 0.0;
            if(i != j){

                int sum_flag_b = 0;

                for(size_t k = 0; k < n_pop; k++){
                    sum_flag_b += int(population[k][i] && population[k][j]);
                }

                pr = sum_flag_b * 1.0 / n_pop;

            }

            if(pr > upper){
                probs[i][j] = upper;
            }
            else if(pr < lower){
                probs[i][j] = lower;
            }
            else{
                probs[i][j] = pr;
            }

        }
    }

    return probs;

}

vector< vector<float> > cal_mutual_information(const vector<float> &univ, const vector< vector<float> > &biv){
    //Calculate the matrix of mutual information for the flags
    //Input: Univariate and Bivariate probabilities
    //Output: Matrix of size (n_flag,n_flag)

    size_t n_flag = univ.size();
    
    vector< vector <float> > mutual_info(n_flag, vector<float>(n_flag,0.0));

    for(size_t i=0; i<n_flag; i++){
        float pij,pi,pj;
        for(size_t j=0; j<n_flag; j++){
            // i and j
            pij = biv[i][j];
            pi = univ[i];
            pj = univ[j];
            mutual_info[i][j] += pij * log(pij/(pi*pj)) ;
            // i and not j
            pij = univ[i] - biv[i][j];
            pi = univ[i];
            pj = 1 - univ[j];
            mutual_info[i][j] += pij * log(pij/(pi*pj)) ;
            // not i and j
            pij = univ[j] - biv[i][j];
            pi = 1 - univ[i];
            pj = univ[j];
            mutual_info[i][j] += pij * log(pij/(pi*pj)) ;
            // not i and not j
            pij = 1 - (univ[i] + univ[j] -  biv[i][j]);
            pi = 1 - univ[i];
            pj = 1 - univ[j];
            mutual_info[i][j] += pij * log(pij/(pi*pj)) ;
            //mutual_info must be non negative
            if(mutual_info[i][j] < 0  || isnan(mutual_info[i][j]))  {
                mutual_info[i][j] = 0; 
            }
        }
    }

    return mutual_info;

}

void print_tree(const map<int, vector<int> > &my_tree){

    for(map<int, vector<int> >::const_iterator it = my_tree.begin(); it != my_tree.end(); ++it){
        int parent = it->first;
        cout << parent << " : ";
        vector<int> sons = it->second;
        for(size_t i=0; i<sons.size(); i++){
            cout << sons[i] << ",";
        }
        cout << endl;
    }
    return;    
}

map<int, vector<int> > calc_max_weight_spanning_tree(const vector <vector <float> > &mutual_info){
    //Calculate the maximum weight spanning tree based on mutual informaiton
    //input: Mutual Information matrix (size n_flag,n_flag)
    //output: Tree-structure in a map 

    size_t n_flag = mutual_info.size();

    map<int, vector<int> > tree;
    vector<int> added; 

    //Select random root and add it to the tree
    uniform_int_distribution<> distrib(0,n_flag-1);
    int root = distrib(generator);
    //cout << "ROOT:" << root << endl;
    added.push_back(root);
    vector<int> best_match(n_flag, root);

    while(added.size() < n_flag){

        int parent = -1;
        int to_add = -1;
        float maximum = -1.0;

        vector<float> temp; 

        for(size_t i=0; i<n_flag; i++){
            if ( count(added.begin(), added.end(), i) == 0 ){
               
                temp.push_back(mutual_info[i][ best_match[i] ] );
               
                if( mutual_info[i][ best_match[i] ] > maximum ){
                    maximum = mutual_info[i][ best_match[i] ];
                    parent = best_match[i];
                    to_add = i;
                }
            }
        }
        
        /*for(size_t k=0; k<temp.size(); k++){
            cout << ">>" << temp[k] ;
        }
        cout << endl;
        cout << parent << ":" << to_add << "=>" << maximum << endl;*/

        if(tree.count(parent)){
            tree[parent].push_back(to_add);
        }else{
            tree[parent] = vector<int>(1,to_add);
        }

        added.push_back(to_add);

        for(size_t i=0; i<n_flag; i++){
            if ( count(added.begin(), added.end(), i) == 0 ){
                if( mutual_info[i][to_add] > mutual_info[i][ best_match[i] ] ){
                    best_match[i] = to_add;
                }
            }
        }        

    }

    //easy access to root with -1
    tree[-1] = vector<int>(1,root);

    return tree;

}

map<int,int> get_parents(map<int,vector<int> > my_tree){
    //get a map with the parents of each node
    //input: tree formated as parent int-> sons vector<int>
    //output: pairs of son(int)-> parent(int)

    map<int,int> result;

    for(map<int, vector<int> >::const_iterator it = my_tree.begin(); it != my_tree.end(); ++it){
        int parent = it->first;
        if(parent == -1) continue; //skipping our root access because is not important
        vector<int> sons = it->second;
        for(size_t i=0; i<sons.size(); i++){
            result[ sons[i] ] = parent;
        }
    }

    return result;

} 

vector< vector<int> > tree_sampling(map<int, vector<int> > tree, const vector<float> &univ , const vector <vector<float>> &biv , int n_pop ){
    //generate a population from a tree sampling, using probabilistic logic sampling
    //input: maximum weight spanning tree; univariate and bivariate probabilities;# of individuals in the population
    //output: vector of size n_pop, #flags

    size_t num_flags = univ.size();
    
    vector<vector<int> > pop_flags(n_pop, vector<int> (num_flags,0));

    //Get the root of the tree and the tree parents
    int root = tree[-1][0];
    map<int,int> parents = get_parents(tree);

    for(size_t i=0; i<n_pop; i++){
        // sample truth for root depending on the univariate distribution
        bernoulli_distribution root_dist(univ[root]);
        pop_flags[i][root] = root_dist(generator);
        
        //insert sons of the root into a stack
        vector<int> to_sample = tree[root];
        while(to_sample.size() != 0){
            //for every element of the stack, we sample according to its parent
            int node = to_sample.back();
            int parent = parents[node];

            //calculating conditional probability
            float intersection, condition;
            if(pop_flags[i][parent]){
                intersection = biv[parent][node] ;
                condition = univ[parent] ;
            }else{
                intersection = univ[node] - biv[parent][node] ;
                condition = 1-univ[parent];
            }
            
            //sampling element of the stack
            bernoulli_distribution node_dist(  intersection/condition  );
            pop_flags[i][node] = node_dist(generator);

            //popping element from the stack as it has been sampled
            to_sample.pop_back();

            //push sons of the element on the stack if they exist
            if(tree.count(node)){
                vector<int> to_add = tree[node];
                for(size_t j=0; j<to_add.size(); j++ ){
                    to_sample.push_back(to_add[j]);
                }
            }
        }

    }

    return pop_flags;
}


result TreeEDA(float (*fun)(const vector<int> &, string, string, string), string compile_files, string filename, string run_ops, int n_flags, int n_gen, int n_pop, float r_mut, float r_sel){
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
       min_runtime[i] = (*fun)(population[0], compile_files, filename, run_ops);
       min_flags[i] = population[0];

       vector<pair<float,int> > runtimes(n_pop, make_pair(0.0,0));

       for (int j = 0; j < n_pop; j++){
            float score = (*fun)(population[j], compile_files, filename, run_ops);
            runtimes[j] = make_pair(score, j);
       }

       //Selection of individuals according to r_sel (percentage)
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

       //Compute univariate and bivariate marginal frequencies
        vector<float> univ_probs = cal_univariate_prob(selected_population);
        vector< vector<float> > biv_probs = cal_bivariate_prob(selected_population);

       //Calculate Matrix of Mutual Information
        vector< vector<float> > mutual_info = cal_mutual_information(univ_probs, biv_probs);

       //Calculate maximum weight spanning tree
        map<int, vector<int> > tree = calc_max_weight_spanning_tree(mutual_info);
        //cout << i << endl; print_tree(tree); 

       //Sample n_pop individuals from the tree
        population = tree_sampling(tree, univ_probs, biv_probs, n_pop);
        //cout << "post sampling!" << endl; 

       // Mutation (not included originally, but used here for uniformity)
        mutation(population, r_mut);
        //cout << "post mutation!" << endl; 


    }

    print_min(min_runtime, min_flags);

    //result 
    result test;
    test.min_flags = min_flags;
    test.min_runtimes = min_runtime;

    return test;

}

void TreeEDAProcess(string fn, string cf, string r_op){
    
    // define the algorithm used
    string alg = "Tree-EDA";
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
    string run_options = r_op;

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
        result first = TreeEDA(objective, compile_files, filename, run_options, n_flags, n_gen, n_pop, r_mut, r_sel);

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

    TreeEDAProcess("basicmath_small","experiment/basicmath/basicmath_small.c experiment/basicmath/rad2deg.c experiment/basicmath/cubic.c experiment/basicmath/isqrt.c", "");

    return 0;
}
*/
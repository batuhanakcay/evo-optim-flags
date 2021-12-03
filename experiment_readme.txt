First compile experiment.cpp and then run experiments:
gcc -O3 experiment.cpp tree_eda.cpp -o experiment

experiment [dir] [exp_num] [small/large] [algorithm] \n\
                [dir]: directory containing the experiment files. Default is \"/experiment\"\n\
                [exp_num]: 0 - basicmath , 1 - bitcount, 2 - qsort, 3 - susan\n\
                [small/large]: Will the algorithm use the small (-s or --small) or large (-l or --large) input?\n\
                [algorithm]: 0 - GA, 1 - UMDA, 2 - TreeEDA\n";
                
Example: 

./experiment experiment 0 -s 2 > tree_eda_output.txt

This searches for the directory "experiment" to run basicmath with small input and the treeEDA algorithm.

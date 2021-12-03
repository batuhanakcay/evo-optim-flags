First compile experiment.cpp and then run experiments:
	
	make clean
	make 

OR alternatively 

	g++ -O3 experiment.cpp tree_eda.cpp UMDA.cpp ga.cpp commons.cpp -o experiment


To use, type: 

experiment [dir] [exp_num] [small/large] [algorithm]

			[dir]: directory containing the experiment files. 
			[exp_num]: 0 - basicmath , 1 - bitcount, 2 - qsort, 3 - susan
			[small/large]: Will the algorithm use the small (-s or --small) or large (-l or --large) input?
			[algorithm]: 0 - GA, 1 - UMDA, 2 - TreeEDA
                
Example: 

./experiment "./experiments" 0 -s 2 > tree_eda_output.txt

This searches for the directory "experiment" to run basicmath with small input and the treeEDA algorithm.


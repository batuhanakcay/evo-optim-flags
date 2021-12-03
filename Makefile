DEPS = experiment.cpp tree_eda.cpp UMDA.cpp ga.cpp commons.cpp 

experiment: ${DEPS} Makefile
	g++ -O3 ${DEPS} -o experiment

clean:
	rm -rf experiment
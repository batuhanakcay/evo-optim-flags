#ifndef GA_H
#define GA_H

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <tuple>
#include <algorithm>
#include <string>
#include <fstream>
#include <map>

#include "commons.h"

using namespace std;

result genetic_algorithm(float (*fun)(const vector<int> &, string, string, string), string compile_files, string filename, string run_ops, int n_flags, int n_gen, int n_pop, float r_mut, float r_sel);
void GAProcess(string fn, string cf, string r_op);

#endif
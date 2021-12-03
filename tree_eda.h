#ifndef TREE_EDA_H
#define TREE_EDA_H

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

result TreeEDA(float (*fun)(const vector<int> &, string, string, string), string compile_files, string filename, string run_ops, int n_flags, int n_gen, int n_pop, float r_mut, float r_sel);
void TreeEDAProcess(string fn, string cf, string r_op);

#endif
#ifndef TREE_EDA_H
#define TREE_EDA_H

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <map>
#include <math.h>
#include <fstream>
#include <string>
using namespace std;

struct result;

result TreeEDA(float (*fun)(const vector<int> &, string, string, string), string compile_files, string filename, string run_ops, int n_flags, int n_gen, int n_pop, float r_mut, float r_sel);
void TreeEDAProcess(string fn, string cf, string r_op);

#endif
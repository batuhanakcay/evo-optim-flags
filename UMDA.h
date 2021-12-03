#ifndef UMDA_H
#define UMDA_H

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

result UMDA(float (*fun)(const vector<int> &, string, string, string), string compile_files, string filename, string run_ops, int n_flags, int n_gen, int n_pop, float r_mut, float r_sel);
void UMDAProcess(string fn, string cf, string r_op);

#endif
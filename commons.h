#ifndef COMMONS_H
#define COMMONS_H

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <tuple>
#include <algorithm>
#include <string>
#include <fstream>
#include <map>

using namespace std;

struct result {
    vector<vector<int> > min_flags;
    vector<float> min_runtimes;
};

extern default_random_engine generator; 
extern vector<string> gcc_flags;

void print_flags(const vector<int> &flags);

void print_min(const vector<float> &min_runtime, const vector<vector<int> > &min_flags);

float objective(const vector<int> &flags, string compile_files, string filename, string run_ops);

vector<int> selection(const vector<pair<float,int> > &sorted_runtimes, float r_sel);

void mutation(vector<vector<int> > & children, float r_mut);

#endif
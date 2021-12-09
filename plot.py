import numpy as np
import matplotlib.pyplot as plt

algorithms = ["ga", "umda", "tree"]
program_names = ["basicmath" , "bitcount", "qsort", "susan"]
input_types = ["small", "large"]

flag_type = "O1"
output_string = "output"
input_type = 0
program_name = 0

program_name = program_names[program_name]
input_type = input_types[input_type]

result_files = []
for algorithm in algorithms:
    result_files.append("_".join([program_name, input_type, algorithm, flag_type, output_string]) + ".txt")


results = {}
parameters = {}

for result_file in result_files:
    with open(result_file) as f:
        lines = f.readlines()
        cur_filename = ""
        cur_alg = -1
        cur_execution = -1
        for line in lines:
            line = line.strip()
            content = line.split(";")
            if content[0] == "parameters":
                for field in content[1:]:
                    field = field.split(":")
                    if field[0] == "alg":
                        if field[1] == "GA":
                            cur_alg = 0
                        elif field[1] == "UMDA":
                            cur_alg = 1                         
                        elif field[1] == "Tree-EDA":
                            cur_alg = 2
                        cur_alg = algorithms[cur_alg]
                    elif field[0] != "filename" and field[0] not in parameters:
                        parameters[field[0]] = field[1]
                times = [([float('inf')] * int(parameters["n_gen"])) for x in range(int(parameters["n_exe"]))]
                bests = [None] * int(parameters["n_exe"])
                results[cur_alg] = [times, bests]
            elif content[0] == "best":
                best_values = {}
                for field in content[1:]:
                    field = field.split(":")
                    if field[0] == "time":
                        best_values["time"] = float(field[1])
                    elif field[0] == "gen":
                        best_values["gen"] = int(field[1])
                    elif field[0] == "flags":
                        best_flags = field[1].split(",")
                        for i in range(len(best_flags)):
                            best_flags[i] = int(best_flags[i])
                        best_values["flags"] = best_flags
                # bests
                results[cur_alg][1][cur_execution] = best_values
            else:
                field = content[0].split(":")
                if field[0] == "exe":
                    cur_execution = int(field[1])
                elif field[0] == "gen":
                    cur_generation = int(field[1])
                    for new_field in content[1:]:
                        new_field = new_field.split(":")
                        if new_field[0] == "time":
                            # times
                            results[cur_alg][0][cur_execution][cur_generation] = float(new_field[1])
    

# define the number of executions
n_exe = int(parameters["n_exe"])
# define the number of generations
n_gen = int(parameters["n_gen"])
# define the number of flags
n_flags = int(parameters["n_flags"])
# define the population size
n_pop = int(parameters["n_pop"])
# mutation rate
r_mut = float(parameters["r_mut"])
# trunctation rate
r_sel = float(parameters["r_sel"])

def non_decreasing_eg(runtimes):
    best_runtimes = np.copy(runtimes)
    shape = best_runtimes.shape
    for i in range(shape[0]):
        best_runtime = float('inf')
        for j in range(shape[1]):
            if best_runtimes[i][j] < best_runtime:
                best_runtime = best_runtimes[i][j]
            else:
                best_runtimes[i][j] = best_runtime
    return best_runtimes


egs = {}
min_gen_runtimes = {}
all_executions = {}
flags = {}
flag_frequencies_sorted = {}
flag_frequencies = {}
best_runtimes = {}
best_runtimes_avg = {}

for alg in results:
    #times
    results[alg][0] = np.array(results[alg][0])
    #file_results = results[filename][0]
    alg_results = non_decreasing_eg(results[alg][0])

    all_executions[alg] = alg_results

    alg_avg_gens = np.mean(alg_results, axis=0)
    alg_egs = (alg_avg_gens[0] - alg_avg_gens[:]) / alg_avg_gens[0]

    egs[alg] = alg_egs

    alg_min_gens = np.amin(alg_results, axis=0)

    min_gen_runtimes[alg] = alg_min_gens

    alg_flags = []
    alg_runtimes = []

    for exe in range(n_exe):
        alg_flags.append(results[alg][1][exe]["flags"])
        alg_runtimes.append(results[alg][1][exe]["time"])

    alg_flags = np.array(alg_flags)
    flags[alg] = alg_flags

    alg_runtimes = np.array(alg_runtimes)
    best_runtimes[alg] = alg_runtimes
    best_runtimes_avg[alg] = np.mean(alg_runtimes)

    alg_flag_frequency = np.sum(alg_flags, axis = 0)
    flag_frequencies[alg] = alg_flag_frequency  

    alg_flag_frequency_sorted = np.sort(alg_flag_frequency)[::-1]
    alg_flag_frequency_sorted_indices = np.argsort(alg_flag_frequency)[::-1]

    flag_frequencies_sorted[alg] = [alg_flag_frequency_sorted, alg_flag_frequency_sorted_indices]

all_flag_frequency = np.zeros(n_flags, dtype=int)
for alg in flag_frequencies:
    all_flag_frequency += flag_frequencies[alg]
all_flag_frequency_indices = np.argsort(all_flag_frequency)[::-1]
for alg in flag_frequencies:
    flag_frequencies[alg] = flag_frequencies[alg][all_flag_frequency_indices]



gens = range(n_gen)
flag_range = range(n_flags)
program_type = program_name + "_" + input_type

plt.gcf().set_size_inches(14, 9)
plt.rcParams['axes.labelsize'] = 18
plt.rcParams['axes.titlesize'] = 22
plt.rcParams['legend.fontsize'] = 18

for alg in results:

    alg_results = all_executions[alg] 
    alg_egs = egs[alg]
    alg_min_gens = min_gen_runtimes[alg]

    plt.plot(gens, alg_egs)
    plt.xlabel('Generation Number')
    plt.ylabel('Runtime Performance Efficiency Gain (vs. Generation 0)')
    plt.title('Runtime performance efficiency gain for the ' + alg + " algorithm")
    plt.legend([program_type])
    plt.savefig("eg" + "_" + alg + "_" + program_type + "_exe" + str(n_exe) + "_gen" + str(n_gen) + ".png")
    plt.clf()

    plt.plot(gens, alg_min_gens)
    plt.xlabel('Generation Number')
    plt.ylabel('Best Runtime Performance')
    plt.title('Best runtime performance of each generation for the ' + alg + " algorithm")
    plt.legend([program_type])
    plt.savefig("min" + "_" + alg + "_" + program_type + "_exe" + str(n_exe) + "_gen" + str(n_gen) + ".png")
    plt.clf()

    #legend_list = ["exe_"] * n_exe
    for x in range(n_exe):
        plt.plot(gens, alg_results[x])
        #legend_list[x] = legend_list[x] + str(x + 1)
    plt.xlabel('Generation Number')
    plt.ylabel('Runtime Performance')
    plt.title('Runtime for the ' + alg + " algorithm on " + program_type)
    #plt.legend(legend_list)
    plt.savefig("all" + "_" + alg + "_" + program_type + "_exe" + str(n_exe) + "_gen" + str(n_gen) + ".png")
    plt.clf()

    plt.bar(flag_range, flag_frequencies_sorted[alg][0])
    plt.xlabel('Flag')
    plt.ylabel('Frequency')
    plt.title('Flag frequencies for the ' + alg + " algorithm on " + program_type)
    plt.legend([alg])
    plt.xticks(flag_range, flag_frequencies_sorted[alg][1])
    plt.savefig("frequencies" + "_" + alg + "_" + program_type + "_exe" + str(n_exe) + "_gen" + str(n_gen) + ".png")
    plt.clf()

legend_algorithms = []
for alg in results:
    plt.plot(gens, egs[alg])
    legend_algorithms.append(alg)
plt.xlabel('Generation Number')
plt.ylabel('Runtime Performance Efficiency Gain (vs. Generation 0)')
plt.title('Efficiency gain for different algorithms on ' + program_type)
plt.legend(legend_algorithms)
#plt.xticks(gens)
plt.savefig("eg" + "_" + program_type + "_exe" + str(n_exe) + "_gen" + str(n_gen) + ".png")
plt.clf()


frequency_sum = np.zeros(n_flags, dtype = int)
for i in range(len(algorithms)):
    plt.bar(flag_range, flag_frequencies[algorithms[i]], bottom = frequency_sum)
    frequency_sum += flag_frequencies[algorithms[i]]
plt.xlabel('Flag')
plt.ylabel('Frequency')
plt.title('Flag frequencies for all algorithms on ' + program_type)
plt.legend(legend_algorithms)
plt.xticks(flag_range, all_flag_frequency_indices)
plt.savefig("frequencies" + "_" + program_type + "_exe" + str(n_exe) + "_gen" + str(n_gen) + ".png")
plt.clf()


labels = list(best_runtimes_avg.keys())
values = list(best_runtimes_avg.values())
for i in range(len(best_runtimes_avg)):
    plt.bar([i], [values[i]])
plt.xlabel('Algorithms')
plt.ylabel('Average best runtime (seconds)')
plt.title('Average best runtime for all algorithms on ' + program_type)
plt.legend(labels)
plt.xticks(range(len(best_runtimes_avg)), labels)
min_value = min(values)
max_value = max(values)
dif = max_value - min_value
plt.ylim(min_value - dif, max_value + dif)
plt.savefig("best_runtimes_avg" + "_" + program_type + "_exe" + str(n_exe) + "_gen" + str(n_gen) + ".png")
plt.clf()

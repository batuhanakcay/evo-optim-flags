import numpy as np
import matplotlib.pyplot as plt

result_file = "umda_gen3_exe3_output.txt"

results = {}

# define the algorithm 
alg = "UMDA"

# define the number of executions
n_exe = 3
# define the number of generations
n_gen = 3
# define the number of flags
n_flags = 107
# define the population size
n_pop = 100
# mutation rate
r_mut = 1.0 / n_flags
# trunctation rate
r_sel = 0.5

with open(result_file) as f:
    lines = f.readlines()
    cur_filename = ""
    cur_execution = -1
    for line in lines:
        content = line.split(",")
        if content[0] == "alg":
            alg = content[1]
        elif content[0] == "n_exe":
            n_exe = int(content[1])
        elif content[0] == "n_gen":
            n_gen = int(content[1])
        elif content[0] == "n_flags":
            n_flags = int(content[1])
        elif content[0] == "n_pop":
            n_pop = int(content[1])
        elif content[0] == "r_mut":
            r_mut = float(content[1])
        elif content[0] == "r_sel":
            r_sel = float(content[1])
        elif content[0] == "F":
            cur_filename = content[1]
            executions = [([float('inf')] * n_gen) for x in range(n_exe)]
            results[cur_filename] = executions
        elif content[0] == "E":
            cur_execution = int(content[1])
        elif content[0] == "G":
            cur_generation = int(content[1])
            results[cur_filename][cur_execution][cur_generation] = float(content[2])

for filename in results:
    results[filename] = np.array(results[filename])
    file_results = results[filename]

    file_avg_gens = np.mean(file_results, axis=0)
    file_egs = (file_avg_gens[0] - file_avg_gens[:]) / file_avg_gens[0]

    file_min_gens = np.amin(file_results, axis=0)

    gens = range(n_gen)

    plt.plot(gens, file_egs)
    plt.xlabel('Generation Number')
    plt.ylabel('Runtime Performance Efficiency Gain (vs. Generation 0)')
    plt.title('Runtime performance efficiency gain for the ' + alg + " algorithm")
    plt.legend([filename])
    plt.xticks(gens)
    plt.savefig("eg" + "_" + alg + "_" + filename + "_exe" + str(n_exe) + "_gen" + str(n_gen) + ".png")
    plt.clf()

    plt.plot(gens, file_min_gens)
    plt.xlabel('Generation Number')
    plt.ylabel('Best Runtime Performance')
    plt.title('Best runtime performance of each generation for the ' + alg + " algorithm")
    plt.legend([filename])
    plt.xticks(gens)
    plt.savefig("min" + "_" + alg + "_" + filename + "_exe" + str(n_exe) + "_gen" + str(n_gen) + ".png")
    plt.clf()

    legend_list = ["exe_"] * n_exe
    for x in range(n_exe):
        plt.plot(gens, file_results[x])
        legend_list[x] = legend_list[x] + str(x + 1)
    plt.xlabel('Generation Number')
    plt.ylabel('Runtime Performance')
    plt.title('Runtime for the ' + alg + " algorithm on " + filename)
    plt.legend(legend_list)
    plt.xticks(gens)
    plt.savefig("all" + "_" + alg + "_" + filename + "_exe" + str(n_exe) + "_gen" + str(n_gen) + ".png")
    plt.clf()


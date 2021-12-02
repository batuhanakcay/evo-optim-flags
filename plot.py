import numpy as np
import matplotlib.pyplot as plt

result_file = "tree_output.txt"

results = {}

parameters = {}

with open(result_file) as f:
    lines = f.readlines()
    cur_filename = ""
    cur_execution = -1
    for line in lines:
        line = line.strip()
        content = line.split(";")
        if content[0] == "parameters":
            for field in content[1:]:
                field = field.split(":")
                parameters[field[0]] = field[1]
            times = [([float('inf')] * int(parameters["n_gen"])) for x in range(int(parameters["n_exe"]))]
            bests = [None] * int(parameters["n_exe"])
            results[parameters["filename"]] = [times, bests]
        elif content[0] == "best":
            best_values = {}
            for field in content[1:]:
                field = field.split(":")
                best_values[field[0]] = field[1]
                # bests
                results[parameters["filename"]][1][cur_execution]= best_values
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
                        results[parameters["filename"]][0][cur_execution][cur_generation] = float(new_field[1])

# define the algorithm 
alg = parameters["alg"]
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

for filename in results:
    #times
    results[filename][0] = np.array(results[filename][0])
    file_results = results[filename][0]

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


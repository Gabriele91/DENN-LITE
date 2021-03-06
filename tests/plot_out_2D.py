import csv as csv_parser
import matplotlib.pyplot as plt
import os
import sys 

population = []
for arg in sys.argv[1:]:
    base_col = len(population)
    with open(arg, 'r') as csvFile:
        reader = csv_parser.reader(csvFile, delimiter=';')
        for row in reader:
            for col, val in enumerate(row):
                if val.strip() == "":
                    continue
                if (len(population)-base_col) <= col:
                    population.append([])
                population[col+base_col].append(float(val))

for individual in population:
    plt.plot(individual)
plt.ylabel('cross entropy')
plt.xlabel('generation')
plt.show()
